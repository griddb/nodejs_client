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

#include "Container.h"

#include <stdarg.h>

namespace griddb {

    Container::Container(GSContainer *container, GSContainerInfo* containerInfo) : mContainer(container),
            mContainerInfo(NULL), mRow(NULL), mTypeList(NULL), timestamp_output_with_float(false) {
        assert(container != NULL);
        assert(containerInfo != NULL);
        GSResult ret = gsCreateRowByContainer(mContainer, &mRow);
        if (!GS_SUCCEEDED(ret)) {
            throw GSException(mContainer, ret);
        }

        GSColumnInfo* columnInfoList;
        //create local mContainerInfo: there is issue from C-API about using share memory that
        // make GSContainerInfo* pointer error in case : create gsRow, get GSContainerInfo from gsRow, set field of gs Row
        try {
            mContainerInfo = new GSContainerInfo();
            (*mContainerInfo) = (*containerInfo); // this is for set for normal data (int, float, double..)
            mContainerInfo->name = NULL;
            if (containerInfo->name) {
                Util::strdup(&(mContainerInfo->name), containerInfo->name);
            }

            columnInfoList = new GSColumnInfo[containerInfo->columnCount]();
            mContainerInfo->columnInfoList = columnInfoList;

            for (int i = 0; i < containerInfo->columnCount; i++) {
                columnInfoList[i].type = containerInfo->columnInfoList[i].type;
                if (containerInfo->columnInfoList[i].name) {
                    Util::strdup(&(columnInfoList[i].name), containerInfo->columnInfoList[i].name);
                } else {
                    columnInfoList[i].name = NULL;
                }

                columnInfoList[i].indexTypeFlags = containerInfo->columnInfoList[i].indexTypeFlags;
                columnInfoList[i].options = containerInfo->columnInfoList[i].options;
            }

            mTypeList = new GSType[mContainerInfo->columnCount]();
        } catch (bad_alloc& ba) {
            //Memory allocation error
            freeMemoryContainer();
            throw GSException(mContainer, "Memory allocation error");
        }
        
        mContainerInfo->timeSeriesProperties = NULL;
        mContainerInfo->triggerInfoList = NULL;
        mContainerInfo->dataAffinity = NULL;

        if (mTypeList && mContainerInfo->columnInfoList) {
            for (int i = 0; i < mContainerInfo->columnCount; i++){
                mTypeList[i] = mContainerInfo->columnInfoList[i].type;
            }
        }
    }

    Container::~Container() {
        

    // allRelated = FALSE, since all row object is managed by Row class
        close(GS_FALSE);
    }

    void Container::freeMemoryContainer() {
        if (mContainerInfo) {
            for (int i = 0; i < mContainerInfo->columnCount; i++) {
                if (mContainerInfo->columnInfoList && mContainerInfo->columnInfoList[i].name) {
                    delete[] mContainerInfo->columnInfoList[i].name;
                }
            }
            if (mContainerInfo->columnInfoList) {
                delete[] mContainerInfo->columnInfoList;
            }
            if (mContainerInfo->name) {
                delete[] mContainerInfo->name;
            }
            delete mContainerInfo;
            mContainerInfo = NULL;
        }
        if (mTypeList) {
            delete[] mTypeList;
            mTypeList = NULL;
        }
    }

    /**
     * @brief Release Container resource
     * @param allRelated Indicates whether all unclosed resources in the lower resources related to the specified GSContainer will be closed or not
     */
    void Container::close(GSBool allRelated) {
        if (mRow != NULL) {
            gsCloseRow(&mRow);
            mRow = NULL;
        }

        //Release container and all related resources
        if (mContainer != NULL) {
            gsCloseContainer(&mContainer, allRelated);
            mContainer = NULL;
        }
        freeMemoryContainer();
    }

    /**
     * @brief Removes the specified type of index among indexes on the specified Column
     * @param *column_name Column name
     * @param index_type Flag value which shows index classification
     * @param *name Index name
     */
    void Container::drop_index(const char* column_name, GSIndexTypeFlags index_type, const char *name) {
        GSResult ret = GS_RESULT_OK;

        if (name) {
            GSIndexInfo indexInfo = GS_INDEX_INFO_INITIALIZER;
            indexInfo.name = name;
            indexInfo.type = index_type;
            indexInfo.columnName = column_name;
            ret = gsDropIndexDetail(mContainer, &indexInfo);
        } else {
            ret = gsDropIndex(mContainer, column_name, index_type);
        }

        if (!GS_SUCCEEDED(ret)) {
            throw GSException(mContainer, ret);
        }
    }

    /**
     * @brief Creates a specified type of index on the specified Column
     * @param *column_name Column name
     * @param index_type Flag value which shows index classification
     * @param *name Index name
     */
    void Container::create_index(const char *column_name, GSIndexTypeFlags index_type, const char *name) {
        GSResult ret = GS_RESULT_OK;

        if (name){
            GSIndexInfo indexInfo = GS_INDEX_INFO_INITIALIZER;
            indexInfo.name = name;
            indexInfo.type = index_type;
            indexInfo.columnName = column_name;
            ret = gsCreateIndexDetail(mContainer, &indexInfo);
        } else {
            ret = gsCreateIndex(mContainer, column_name, index_type);
        }

        if (!GS_SUCCEEDED(ret)) {
            throw GSException(mContainer, ret);
        }
    }

    /**
     * @brief Writes the results of earlier updates to a non-volatile storage medium, such as SSD, so as to prevent the data from being lost even if all cluster nodes stop suddenly.
     */
    void Container::flush() {
        GSResult ret = gsFlush(mContainer);

        if (!GS_SUCCEEDED(ret)) {
            throw GSException(mContainer, ret);
        }
    }

    /**
     * @brief Put row to database.
     * @param *row A Row object representing the content of a Row to be put to database
     * @return Return bool value to indicate row exist or not
     */
    bool Container::put(GSRow *row) {
        GSBool bExists;
        GSResult ret = gsPutRow(mContainer, NULL, mRow, &bExists);
        if (!GS_SUCCEEDED(ret)) {
            throw GSException(mContainer, ret);
        }
        return bExists;
    }

    /**
     * @brief Get current container type
     * @return Return container type
     */
    GSContainerType Container::get_type() {
        GSContainerType containerType;
        GSResult ret = gsGetContainerType(mContainer, &containerType);

        if (!GS_SUCCEEDED(ret)) {
            throw GSException(mContainer, ret);
        }

        return containerType;
    }

    /**
     * @brief Rolls back the result of the current transaction and starts a new transaction in the manual commit mode.
     */
    void Container::abort() {
        GSResult ret = gsAbort(mContainer);

        if (!GS_SUCCEEDED(ret)) {
            throw GSException(mContainer, ret);
        }
    }

    /**
     * @brief Create query from input string.
     * @param *query TQL statement
     * @return Return a Query object
     */
    Query* Container::query(const char* query) {
        GSQuery *pQuery;
        GSResult ret = gsQuery(mContainer, query, &pQuery);

        if (!GS_SUCCEEDED(ret)) {
            throw GSException(mContainer, ret);
        }

        try {
            Query* queryObj = new Query(pQuery, mContainerInfo, mRow);
            return queryObj;
        } catch(bad_alloc& ba) {
            gsCloseQuery(&pQuery);
            throw GSException(mContainer, "Memory allocation error");
        }
    }

    /**
     * @brief Set auto commit to true or false.
     * @param enabled Indicates whether container enables auto commit mode or not
     */
    void Container::set_auto_commit(bool enabled){
        GSBool gsEnabled;
        gsEnabled = (enabled == true ? GS_TRUE:GS_FALSE);
        GSResult ret = gsSetAutoCommit(mContainer, gsEnabled);
        if (!GS_SUCCEEDED(ret)) {
            throw GSException(mContainer, ret);
        }
    }

    /**
     * @brief Commit changes to database when autocommit is set to false.
     */
    void Container::commit() {
        GSResult ret = gsCommit(mContainer);
        if (!GS_SUCCEEDED(ret)) {
            throw GSException(mContainer, ret);
        }
    }

    /**
     * @brief Returns the content of a Row.
     * @param *keyFields The variable to store the target Row key
     * @param *rowdata The Row object to store the contents of target Row to be obtained
     * @return Return bool value to indicate row exist or not
     */
    GSBool Container::get(Field* keyFields, GSRow *rowdata) {
        assert(keyFields != NULL);
        GSBool exists;
        GSResult ret;
        void *key = NULL;
        switch (keyFields->type) {
        case GS_TYPE_STRING:
            if (mContainerInfo->columnInfoList[0].type != GS_TYPE_STRING) {
                throw GSException("wrong type of rowKey string");
            }
            key = &keyFields->value.asString;
            break;
        case GS_TYPE_INTEGER:
            if (mContainerInfo->columnInfoList[0].type != GS_TYPE_INTEGER) {
                throw GSException("wrong type of rowKey integer");
            }
            key = &keyFields->value.asInteger;
            break;
        case GS_TYPE_LONG:
            if (mContainerInfo->columnInfoList[0].type != GS_TYPE_LONG) {
                throw GSException("wrong type of rowKey long");
            }
            key = &keyFields->value.asLong;
            break;
        case GS_TYPE_TIMESTAMP:
            if (mContainerInfo->columnInfoList[0].type != GS_TYPE_TIMESTAMP) {
                throw GSException("wrong type of rowKey timestamp");
            }
            key = &keyFields->value.asTimestamp;
            break;
        default:
            throw GSException("wrong type of rowKey field");
        }

        ret = gsGetRow(mContainer, key, mRow, &exists);
        if (!GS_SUCCEEDED(ret)) {
            throw GSException(mContainer, ret);
        }

        return exists;
    }

    /**
     * @brief Deletes a Row corresponding to Row key
     * @param *keyFields The variable to store the target Row key
     * @return Return bool value to indicate row exist or not
     */
    bool Container::remove(Field* keyFields) {
        assert(keyFields != NULL);
        GSBool exists = GS_FALSE;
        GSResult ret;

        if (keyFields->type == GS_TYPE_NULL) {
            ret = gsDeleteRow(mContainer, NULL, &exists);
        } else {
            switch (keyFields->type) {
            case GS_TYPE_STRING:
                if (mContainerInfo->columnInfoList[0].type != GS_TYPE_STRING) {
                    throw GSException("wrong type of rowKey string");
                }
                ret = gsDeleteRow(mContainer, &keyFields->value.asString, &exists);
                break;
            case GS_TYPE_INTEGER:
                if (mContainerInfo->columnInfoList[0].type != GS_TYPE_INTEGER) {
                    throw GSException("wrong type of rowKey integer");
                }
                ret = gsDeleteRow(mContainer, &keyFields->value.asInteger, &exists);
                break;
            case GS_TYPE_LONG:
                if (mContainerInfo->columnInfoList[0].type != GS_TYPE_LONG) {
                    throw GSException("wrong type of rowKey long");
                }
                ret = gsDeleteRow(mContainer, &keyFields->value.asLong, &exists);
                break;
            case GS_TYPE_TIMESTAMP:
                if (mContainerInfo->columnInfoList[0].type != GS_TYPE_TIMESTAMP) {
                    throw GSException("wrong type of rowKey timestamp");
                }
                ret = gsDeleteRow(mContainer, &keyFields->value.asTimestamp, &exists);
                break;
            default:
                throw GSException("wrong type of rowKey field");
            }
        }

        if (!GS_SUCCEEDED(ret)) {
            throw GSException(mContainer, ret);
        }

        return (bool) exists;
    }

    /**
     * @brief Put multi row data to database
     * @param **listRowdata The array of row to be put to data base
     * @param rowCount The number of row to be put to database
     */
    void Container::multi_put(GSRow** listRowdata, int rowCount) {
        GSResult ret;
        GSBool bExists;
        //data for each container
        ret = gsPutMultipleRows(mContainer, (const void * const *) listRowdata,
                rowCount, &bExists);
        if (!GS_SUCCEEDED(ret)) {
            throw GSException(mContainer, ret);
        }
    }

    /**
     * @brief Get GSContainer of Container object to support Store::multi_put
     * @return Return a pointer which store GSContainer of container
     */
    GSContainer* Container::getGSContainerPtr(){
        return mContainer;
    }

    /**
     * @brief Get GSType of Container object to support put row
     * @return Return a pointer which store type the list of column of row in container
     */
    GSType* Container::getGSTypeList(){
        return mTypeList;
    }

    /**
     * @brief Get GSRow of Container object to support put row
     * @return Return a pointer which store GSRow of container
     */
    GSRow* Container::getGSRowPtr(){
        return mRow;
    }

    /**
     * @brief Get number of column of row in container
     * @return Return number of column of row in container
     */
    int Container::getColumnCount(){
        return mContainerInfo->columnCount;
    }
}
