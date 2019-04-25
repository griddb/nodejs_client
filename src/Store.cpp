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

namespace griddb {

    /**
     * @brief Constructor a new Store::Store object
     * @param *store A pointer which provides functions to manipulate the entire data managed in one GridDB system.
     */
    Store::Store(GSGridStore *store) : mStore(store), timestamp_output_with_float(false) {
    }

    Store::~Store() {
        // allRelated = FALSE, since all container object is managed by Container class
        close(GS_FALSE);
    }

    /**
     * @brief Release Store resource
     */
    void Store::close(GSBool allRelated) {
        // close store
        if (mStore != NULL) {
            gsCloseGridStore(&mStore, allRelated);
            mStore = NULL;
        }
    }

    /**
     * @brief Delete container with specified name
     * @param *name Container name
     */
    void Store::drop_container(const char* name) {
        GSResult ret = gsDropContainer(mStore, name);
        if (!GS_SUCCEEDED(ret)) {
            throw GSException(mStore, ret);
        }
    }

    /**
     * @brief Get information object of a specific container
     * @param *name Container name
     * @return Return a pointer which stores all information of container
     */
    ContainerInfo* Store::get_container_info(const char* name) {
        GSContainerInfo gsContainerInfo = GS_CONTAINER_INFO_INITIALIZER;
        GSChar bExists;
        GSResult ret = gsGetContainerInfo(mStore, name, &gsContainerInfo, &bExists);
        if (!GS_SUCCEEDED(ret)) {
            throw GSException(mStore, ret);
        }
        if (bExists == false) {
            return NULL;
        }

        try {
            ContainerInfo* containerInfo = new ContainerInfo(&gsContainerInfo);
            return containerInfo;
        } catch (bad_alloc& ba) {
            throw GSException(mStore, "Memory allocation error");
        }
    }

    /**
     * @brief Creates or update a Container with the specified GSContainerInfo.
     * @param *info A pointer which stores all information of container
     * @param modifiable Indicates whether the column layout of the existing Container can be modified or not
     * @return The pointer to a pointer variable to store Container instance
     */
    Container* Store::put_container(ContainerInfo* info,
            bool modifiable) {
        if (info == NULL) {
            throw GSException(mStore, "Invalid input for \"Store::put_container\" method. Argument container info can not be null");
        }
        // Get Container information
        GSContainerInfo* gsInfo = info->gs_info();
        GSContainer* pContainer = NULL;
        // Create new gsContainer
        GSResult ret = gsPutContainerGeneral(mStore, gsInfo->name, gsInfo, modifiable, &pContainer);
        if (!GS_SUCCEEDED(ret)) {
            throw GSException(mStore, ret);
        }

        try {
            Container* container = new Container(pContainer, gsInfo);
            return container;
        } catch (bad_alloc& ba) {
            gsCloseContainer(&pContainer, GS_FALSE);
            throw GSException(mStore, "Memory allocation error");
        }
    }

    /**
     * @brief Get container object with corresponding name
     * @param *name Container name
     * @return The pointer to a pointer variable to store Container instance
     */
    Container* Store::get_container(const char* name) {
        GSContainer* pContainer;
        GSResult ret = gsGetContainerGeneral(mStore, name, &pContainer);
        if (!GS_SUCCEEDED(ret)) {
            throw GSException(mStore, ret);
        }
        if (pContainer == NULL) {
            //If not found container, return NULL in target language
            return NULL;
        }
        GSContainerInfo containerInfo = GS_CONTAINER_INFO_INITIALIZER;
        GSChar bExists;
        ret = gsGetContainerInfo(mStore, name, &containerInfo, &bExists);
        if (!GS_SUCCEEDED(ret)) {
            gsCloseContainer(&pContainer, GS_FALSE);
            throw GSException(mStore, ret);
        }
        try {
            Container* container = new Container(pContainer, &containerInfo);
            return container;
        } catch (bad_alloc& ba) {
            gsCloseContainer(&pContainer, GS_FALSE);
            throw GSException(mStore, "Memory allocation error");
        }
    }

    /**
     * @brief Query execution and fetch is carried out on a specified arbitrary number of Query, with the request unit enlarged as much as possible.
     * @param **queryList A list of query
     * @param queryCount Number of element in query list
     */
    void Store::fetch_all(GSQuery* const* queryList, size_t queryCount) {
        GSResult ret = gsFetchAll(mStore, queryList, queryCount);
        if (!GS_SUCCEEDED(ret)) {
            throw GSException(mStore, ret);
        }
    }

    /**
     * @brief Get Partition controller.
     * @return The pointer to a pointer variable to store PartitionController instance
     */
    PartitionController* Store::partition_info() {
         GSPartitionController* partitionController;

        GSResult ret = gsGetPartitionController(mStore, &partitionController);

        if (!GS_SUCCEEDED(ret)) {
            throw GSException(mStore, ret);
        }

        try {
            PartitionController* partition = new PartitionController(partitionController);
            return partition;
        } catch (bad_alloc& ba) {
            gsClosePartitionController(&partitionController);
            throw GSException(mStore, "Memory allocation error");
        }
    }

    /**
     * @brief Create row key predicate.
     * @param type The type of Row key used as a matching condition
     * @return The pointer to a pointer variable to store RowKeyPredicate instance
     */
    RowKeyPredicate* Store::create_row_key_predicate(GSType type) {
        GSRowKeyPredicate* predicate;

        GSResult ret = gsCreateRowKeyPredicate(mStore, type, &predicate);

        if (!GS_SUCCEEDED(ret)) {
            throw GSException(mStore, ret);
        }

        try {
            RowKeyPredicate* rowKeyPredicate = new RowKeyPredicate(predicate, type);
            return rowKeyPredicate;
        } catch (bad_alloc& ba) {
            gsCloseRowKeyPredicate(&predicate);
            throw GSException(mStore, "Memory allocation error");
        }
    }

    /**
     * @brief New creation or update operation is carried out on an arbitrary number of rows of multiple Containers, with the request unit enlarged as much as possible.
     * @param ***listRow A pointer refers list of row data
     * @param *listRowContainerCount A array store number of list row for each container
     * @param **listContainerName list container name
     * @param containerCount Number of container
     */
    void Store::multi_put(GSRow*** listRow, const int *listRowContainerCount,
            const char ** listContainerName, size_t containerCount) {
        assert(listRowContainerCount != NULL);
        assert(listContainerName != NULL);
        GSResult ret;
        GSContainerRowEntry* entryList;
        
        try {
            entryList = new GSContainerRowEntry[containerCount]();
        } catch (bad_alloc& ba) {
            throw GSException(mStore, "Memory allocation error");
        }
        for (int i= 0; i < containerCount; i++) {
            entryList[i].containerName = listContainerName[i];
            entryList[i].rowCount = listRowContainerCount[i];
            entryList[i].rowList = (void* const*) listRow[i];
        }
        ret = gsPutMultipleContainerRows(mStore, entryList, containerCount);
        delete[] entryList;
        if (!GS_SUCCEEDED(ret)) {
            throw GSException(mStore, ret);
        }
    }

    /**
     * @brief get multi row from multi container
     * @param **predicateList A pointer refers list of the specified condition entry by a container for representing the acquisition conditions for a plurality of containers.
     * @param predicateCount Number of predicate list
     * @param **entryList A pointer refers to a list of the Row contents entry by a container used when operating collectively a plurality of Rows of a plurality of containers.
     * @param *containerCount Number of container
     * @param **colNumList A pointer refers to an array stores number of column of each container
     * @param ***typeList A pointer refers to an array stores type of column of each container
     * @param **orderFromInput A pointer refers to an array stores order of each container for output
     */
    void Store::multi_get(const GSRowKeyPredicateEntry* const * predicateList,
            size_t predicateCount, GSContainerRowEntry **entryList, size_t* containerCount,
            int **colNumList, GSType*** typeList, int **orderFromInput) {
        assert(predicateList != NULL);
        assert(predicateCount >= 0);
        assert(colNumList != NULL);
        assert(typeList != NULL);
        assert(orderFromInput != NULL);
        assert(containerCount != NULL);
        *colNumList = NULL;
        *typeList = NULL;
        *orderFromInput = NULL;
        *containerCount = 0;
        int length = (int) predicateCount;

        try {
            // get number of column of rows in each container.
            *colNumList = new int[predicateCount](); //will be free in argout
            *typeList = new GSType*[predicateCount](); //will be free in argout
            *orderFromInput = new int[length](); //will be free in argout
        } catch (bad_alloc& ba) {
            this->freeMemoryMultiGet(colNumList, typeList, length, orderFromInput);
            throw GSException(mStore, "Memory allocation error");
        }

        bool setNumList = this->setMultiContainerNumList(predicateList,
                    length, &colNumList, &typeList);
        if (!setNumList) {
            this->freeMemoryMultiGet(colNumList, typeList, length, orderFromInput);
            throw GSException(mStore, "Set multi containers number list and type list error");
        }

        // Get data for entryList
        GSResult ret = gsGetMultipleContainerRows(mStore, predicateList,
                predicateCount, (const GSContainerRowEntry**) entryList, containerCount);
        if (!GS_SUCCEEDED(ret)) {
            this->freeMemoryMultiGet(colNumList, typeList, length, orderFromInput);
            throw GSException(mStore, ret);
        }

        // set data for orderFromInput
        for (int i = 0; i < length; i++) {
            for (int j = 0; j < length; j++) {
                if (strcmp((*predicateList)[i].containerName, (*entryList)[j].containerName) == 0){
                    (*orderFromInput)[i] = j;
                }
            }
        }
    }

    /**
     * Support free memory in multi_get function when exception happen
     */
    void Store::freeMemoryMultiGet(int **colNumList, GSType*** typeList,
            int length, int **orderFromInput) {
        if (*colNumList) {
            delete [] *colNumList;
            *colNumList = NULL;
        }

        if (*typeList) {
            for (int i = 0; i < length; i++) {
                if ((*typeList)[i]) {
                    delete[] (*typeList)[i];
                }
            }
            delete [] *typeList;
            *typeList = NULL;
        }
        if (*orderFromInput) {
            delete [] *orderFromInput;
            *orderFromInput = NULL;
        }
    }

    /**
     * Support multi_get function to put data into colNumList and typeList
     */
    bool Store::setMultiContainerNumList(const GSRowKeyPredicateEntry* const * predicateList,
            int length, int ***colNumList, GSType**** typeList) {
        for (int i = 0; i < length; i++) {
            Container *tmpContainer;
            try {
                tmpContainer = this->get_container((*predicateList)[i].containerName);
            } catch (GSException e) {
                return false;
            }
            if (tmpContainer == NULL) {
                return false;
            }
            (**colNumList)[i] = tmpContainer->getColumnCount();

            try {
                //(**typeList)[i] will be freed in freeMemoryMultiGet() function or argout
                (**typeList)[i] = new GSType[(**colNumList)[i]]();
            } catch (bad_alloc& ba) {
                delete tmpContainer;
                return false;
            }
            
            for (int j = 0; j < (**colNumList)[i]; j++) {
                (**typeList)[i][j] = tmpContainer->getGSTypeList()[j];
            }
            delete tmpContainer;
        }
        return true;
    }

}
