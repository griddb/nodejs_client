/*
    Copyright (c) 2017 TOSHIBA Digital Solutions Corporation.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "Store.h"
#include "GSException.h"

namespace griddb {

    Store::Store(GSGridStore *store) : mStore(store), timestamp_output_with_float(false) {
    }
    Store::~Store() {
        // allRelated = FALSE, since all container object is managed by Container class
        close(GS_FALSE);
    }
    /**
     * Release all resources created by this gridstore object
     */
    void Store::close(GSBool allRelated) {
        // close store
        if (mStore != NULL) {
            gsCloseGridStore(&mStore, allRelated);
            mStore = NULL;
        }
    }
    /**
     * Delete container with specified name
     */
    void Store::drop_container(const char* name) {
        GSResult ret = gsDropContainer(mStore, name);
        if (ret != GS_RESULT_OK) {
            throw GSException(mStore, ret);
        }
    }
    /**
     * Return information object of a specific container
     */
    ContainerInfo* Store::get_container_info(const char* name) {
        GSContainerInfo containerInfo = GS_CONTAINER_INFO_INITIALIZER;
        GSChar bExists;
        GSResult ret = gsGetContainerInfo(mStore, name, &containerInfo, &bExists);
        if (ret != GS_RESULT_OK) {
            throw GSException(mStore, ret);
        }
        if (bExists == false) {
            return NULL;
        }
        return new ContainerInfo(&containerInfo);
    }
    /**
     * Put container. Convert from method gsPutContainerGeneral()
     */
    Container* Store::put_container(ContainerInfo* info,
            bool modifiable) {
        // Get Container information
        GSContainerInfo* gsInfo = info->gs_info();
        GSContainer* pContainer = NULL;
        // Create new gsContainer
        GSResult ret = gsPutContainerGeneral(mStore, gsInfo->name, gsInfo, modifiable, &pContainer);
        if (ret != GS_RESULT_OK) {
            throw GSException(mStore, ret);
        }
        if (pContainer == NULL) {
            return NULL;
        }
        return new Container(pContainer, gsInfo);
    }
    /**
     * Get container object with corresponding name
     */
    Container* Store::get_container(const char* name) {
        GSContainer* pContainer;
        GSResult ret = gsGetContainerGeneral(mStore, name, &pContainer);
        if (ret != GS_RESULT_OK) {
            throw GSException(mStore, ret);
        }
        if (pContainer == NULL) {
            //If not found container, return NULL in target language
            return NULL;
        }
        GSContainerInfo containerInfo = GS_CONTAINER_INFO_INITIALIZER;
        GSChar bExists;
        ret = gsGetContainerInfo(mStore, name, &containerInfo, &bExists);
        if (ret != GS_RESULT_OK) {
            throw GSException(mStore, ret);
        }
        return new Container(pContainer, &containerInfo);
    }
    /**
     * Query execution and fetch is carried out on a specified arbitrary number of Query, with the request unit enlarged as much as possible.
     */
    void Store::fetch_all(GSQuery* const* queryList, size_t queryCount) {
        GSResult ret = gsFetchAll(mStore, queryList, queryCount);
        if (ret != GS_RESULT_OK) {
            throw GSException(mStore, ret);
        }
    }
    /**
     * Get Partition controller. Convert from C-API: gsGetPartitionController
     */
    PartitionController* Store::partition_info() {
        GSPartitionController* partitionController;
        GSResult ret = gsGetPartitionController(mStore, &partitionController);

        if (ret != GS_RESULT_OK) {
            throw GSException(mStore, ret);
        }

        return new PartitionController(partitionController);
    }
    /**
     * Create row key predicate. Convert from C-API: gsCreateRowKeyPredicate
     */
    RowKeyPredicate* Store::create_row_key_predicate(GSType type) {
        GSRowKeyPredicate* predicate;
        GSResult ret = gsCreateRowKeyPredicate(mStore, type, &predicate);

        if (ret != GS_RESULT_OK) {
            throw GSException(mStore, ret);
        }

        return new RowKeyPredicate(predicate, type);
    }
    /**
     * New creation or update operation is carried out on an arbitrary number of rows of multiple Containers, with the request unit enlarged as much as possible.
     */
    void Store::multi_put(GSRow*** listRow, const int *listRowContainerCount,
            const char ** listContainerName, size_t containerCount) {
        GSResult ret;
        GSContainerRowEntry * entryList = (GSContainerRowEntry*) malloc (containerCount * sizeof(GSContainerRowEntry));
        for (int i= 0; i < containerCount; i++) {
            entryList[i].containerName = listContainerName[i];
            entryList[i].rowCount = listRowContainerCount[i];
            entryList[i].rowList = (void* const*) listRow[i];
        }
        ret = gsPutMultipleContainerRows(mStore, entryList, containerCount);
        free((void*) entryList);
        if (ret != GS_RESULT_OK) {
            throw GSException(mStore, ret);
        }
    }
    /**
     * muti_get method. Using gsGetMultipleContainerRows C-API
     */
    void Store::multi_get(const GSRowKeyPredicateEntry* const * predicateList,
            size_t predicateCount, GSContainerRowEntry **entryList, size_t* containerCount, int **colNumList, GSType*** typeList) {
        // get number of column of rows in each container.
        *colNumList = new int[predicateCount]; //will be free in argout
        *typeList = new GSType*[predicateCount]; //will be free in argout
        int length = (int)predicateCount;
        for (int i = 0; i < length; i++) {
            Container *tmpContainer = this->get_container((*predicateList)[i].containerName);
            if (tmpContainer == NULL) {
                throw GSException(mStore, "Not found container");
            }
            (*colNumList)[i] = tmpContainer->getColumnCount();
            (*typeList)[i] = (GSType*) malloc(sizeof(GSType) * (*colNumList)[i]);
            for (int j = 0; j < (*colNumList)[i]; j++) {
                (*typeList)[i][j] = tmpContainer->getGSTypeList()[j];
            }
            delete tmpContainer;
        }
        // Get data for entryList
        GSResult ret = gsGetMultipleContainerRows(mStore, predicateList,
                predicateCount, (const GSContainerRowEntry**) entryList, containerCount);
        if (ret != GS_RESULT_OK) {
            throw GSException(mStore, ret);
        }
    }

}
