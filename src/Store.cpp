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
        //
        std::map<string, griddb::Container*>::iterator it = mContainerList.begin();
        while(it != mContainerList.end()) {
            if (it->second) {
                delete(it->second);
            }
            it++;
        }
        mContainerList.clear();
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
        GSContainerInfo containerInfo;
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
        GSContainerInfo containerInfo;
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
    void Store::multi_put(Row*** listRow, const int *listRowContainerCount,
            const char ** listContainerName, size_t containerCount) {
        const GSChar *containerName;
        GSResult ret;
        GSContainerRowEntry * entryList = (GSContainerRowEntry*) malloc (containerCount * sizeof(GSContainerRowEntry));
        std::map<string,griddb::Container*>::iterator it;
        for (int i= 0; i < containerCount; i++) {
            containerName = listContainerName[i];
            entryList[i].containerName = containerName;
            entryList[i].rowCount = listRowContainerCount[i];
            GSRow** listContainerRow = (GSRow**) malloc(listRowContainerCount[i] * sizeof(GSRow*));
            string tmpString(containerName);
            it = mContainerList.find(tmpString);
            Container* pContainer;
            if (it != mContainerList.end()) {
                //Find it in map
                pContainer = it->second;
            } else {
                //Not find in map
                pContainer = this->get_container(containerName);
                it = mContainerList.begin();
                mContainerList.insert(it, std::pair<string, griddb::Container*>(containerName, pContainer));
            }

            for (int j = 0; j < listRowContainerCount[i]; j++) {
                GSRow* gsrow;
                gsCreateRowByContainer(pContainer->getGSContainerPtr(), &gsrow);
                listRow[i][j]->set_for_row(gsrow, NULL);
                listContainerRow[j] = gsrow;
            }
            entryList[i].rowList = (void* const*) listContainerRow;
            pContainer = NULL;

        }
        ret = gsPutMultipleContainerRows(mStore, entryList, containerCount);

        //Free memory
        for (int i = 0; i < containerCount; i++) {
            containerName = listContainerName[i];
            string tmpString(containerName);
            it = mContainerList.find(tmpString);
            Container* pContainer;
            if (it != mContainerList.end()) {
                //Find it in map
                pContainer = it->second;
            } else {
                //Not find in map
                pContainer = this->get_container(containerName);
                it = mContainerList.begin();
                mContainerList.insert(it, std::pair<string, griddb::Container*>(containerName, pContainer));
            }

            for (int j = 0; j < listRowContainerCount[i]; j++) {
                GSRow* gsrow = (GSRow*)entryList[i].rowList[j];
                gsCloseRow(&gsrow);
            }
            free((void*) entryList[i].rowList);
            pContainer = NULL;
        }
        free((void*) entryList);
        if (ret != GS_RESULT_OK) {
            throw GSException(mStore, ret);
        }
    }
    /**
     * muti_get method. Using gsGetMultipleContainerRows C-API
     */
    void Store::multi_get(const GSRowKeyPredicateEntry* const * predicateList,
            size_t predicateCount, std::vector<griddb::Row*> *listRow, size_t **listRowContainerCount,
            char*** listContainerName, size_t* containerCount) {

        GSContainerRowEntry *entryList;

        GSResult ret = gsGetMultipleContainerRows(mStore, predicateList,
                predicateCount, (const GSContainerRowEntry**) &entryList, containerCount);
        if (ret != GS_RESULT_OK) {
            throw GSException(mStore, ret);
        }

        //Store list of GsRow.
        GSRow*** listGsRow = (GSRow***) malloc(*containerCount * sizeof(GSRow**));

        //Memory will be free in typemap out
        *listContainerName = (char **) malloc(*containerCount * sizeof(char*));

        //Memory will be free in typemap out
        *listRowContainerCount = (size_t*) malloc(*containerCount * sizeof(size_t));
        std::map<string,griddb::Container*>::iterator it;

        GSContainerRowEntry* rowKeyEntry;
        for (int i = 0; i< *containerCount; i++) {
            //data for each container
            rowKeyEntry = &(entryList[i]);

            GSContainerInfo containerInfo;
            GSChar bExists;
            (*listContainerName)[i] = strdup(rowKeyEntry->containerName);
            (*listRowContainerCount)[i] = rowKeyEntry->rowCount;
            (listGsRow)[i] = (GSRow**) malloc(rowKeyEntry->rowCount * sizeof(GSRow*));

            for (int j = 0;j < rowKeyEntry->rowCount; j++){
                GSRow* gsrow = (GSRow*) (rowKeyEntry->rowList[j]);
                (listGsRow)[i][j] = gsrow;
            }
        }

        //Use this loop to work around the error when call gsGetContainerInfo when process output of gsGetMultipleContainerRows
        //Need to be refactored when C-API fix error above.

        for (int i = 0; i< *containerCount; i++) {
            for (int j = 0; j < (*listRowContainerCount)[i]; j++){
                //Dummy row
                Row* tmpRow = new Row(1, NULL);
                tmpRow->set_from_row((listGsRow)[i][j]);
                (*listRow).push_back(tmpRow);
            	gsCloseRow(&(listGsRow)[i][j]);
            }
            free((void *) (listGsRow)[i]);
        }
        free((void *) listGsRow);
    }

}
