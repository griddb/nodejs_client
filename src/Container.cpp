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
#include "GSException.h"

namespace griddb {

    Container::Container(GSContainer *container, GSContainerInfo* containerInfo) : mContainer(container),
            mContainerInfo(NULL), mRow(NULL), typeList(NULL), timestamp_output_with_float(false) {
        GSResult ret;
        if ((ret = gsCreateRowByContainer(mContainer, &mRow)) != GS_RESULT_OK) {
            throw GSException(ret, "can not create row from Container");
        }

        //create local mContainerInfo: there is issue from C-API about using share memory that
        // make GSContainerInfo* pointer error in case : create gsRow, get GSContainerInfo from gsRow, set field of gs Row
        mContainerInfo = (GSContainerInfo*) malloc(sizeof(GSContainerInfo));
        mContainerInfo->columnCount = containerInfo->columnCount;
        GSColumnInfo* columnInfoList = (GSColumnInfo *) malloc(sizeof (GSColumnInfo) * containerInfo->columnCount);
        for (int i = 0; i< containerInfo->columnCount; i++) {
            columnInfoList[i].type = containerInfo->columnInfoList[i].type;
            columnInfoList[i].name = strdup(containerInfo->columnInfoList[i].name);
#if GS_COMPATIBILITY_SUPPORT_1_5
            columnInfoList[i].indexTypeFlags = containerInfo->columnInfoList[i].indexTypeFlags;
#if GS_COMPATIBILITY_SUPPORT_3_5
            columnInfoList[i].options = containerInfo->columnInfoList[i].options;
#endif
#endif
        }
        mContainerInfo->columnInfoList = columnInfoList;
    }

    Container::~Container() {
        gsCloseRow(&mRow);

    // allRelated = FALSE, since all row object is managed by Row class
        close(GS_FALSE);

        for (int i = 0; i< mContainerInfo->columnCount;i++) {
            free((void*)mContainerInfo->columnInfoList[i].name);
        }
        free((void*) mContainerInfo->columnInfoList);
        free((void*) mContainerInfo);
    }

    /**
     * Close container.
     */
    void Container::close(GSBool allRelated) {
        //Release container and all related resources
        if (mContainer != NULL) {
            gsCloseContainer(&mContainer, allRelated);
            mContainer = NULL;
            free(typeList);
        }
    }

    /**
     * Removes the specified type of index among indexes on the specified Column.
     */
    void Container::drop_index(const char* column_name, GSIndexTypeFlags index_type, const char *name) {
        GSResult ret = GS_RESULT_OK;
#if GS_COMPATIBILITY_SUPPORT_3_5
        if (name) {
            int colIndex = this->get_column_index(column_name);
            //Case not found column name
            if (colIndex == -1) {
                throw GSException(mContainer, "Can't find column");
            }
            GSIndexInfo indexInfo = {name, index_type, colIndex, column_name};
            ret = gsDropIndexDetail(mContainer, &indexInfo);
        }
        else {
            ret = gsDropIndex(mContainer, column_name, index_type);
        }
#else
        ret = gsDropIndex(mContainer, column_name, index_type);
#endif
        if (ret != GS_RESULT_OK) {
            throw GSException(mContainer, ret);
        }
    }

    /*
     * Creates a specified type of index on the specified Column.
     */
    void Container::create_index(const char *column_name, GSIndexTypeFlags index_type, const char *name) {
        GSResult ret = GS_RESULT_OK;
#if GS_COMPATIBILITY_SUPPORT_3_5
        if (name){
            int colIndex = this->get_column_index(column_name);
            //Case not found column name
            if (colIndex == -1) {
                throw GSException(mContainer, "Can't find column");
            }
            GSIndexInfo indexInfo = {name, index_type, colIndex, column_name};
            ret = gsCreateIndexDetail(mContainer, &indexInfo);
        }
        else {
            ret = gsCreateIndex(mContainer, column_name, index_type);
        }
#else
            ret = gsCreateIndex(mContainer, column_name, index_type);
#endif
        if (ret != GS_RESULT_OK) {
            throw GSException(mContainer, ret);
        }
    }

    /**
     * Writes the results of earlier updates to a non-volatile storage medium, such as SSD, so as to prevent the data from being lost even if all cluster nodes stop suddenly.
     */
    void Container::flush() {
        GSResult ret = gsFlush(mContainer);

        if (ret != GS_RESULT_OK) {
            throw GSException(mContainer, ret);
        }
    }

    /**
     * Put row to database.
     */
    bool Container::put(Row *rowContainer) {
        GSBool bExists;
        rowContainer->set_for_row(mRow, mContainerInfo);
        GSResult ret = gsPutRow(mContainer, NULL, mRow, &bExists);
        if (ret != GS_RESULT_OK) {
            throw GSException(mContainer, ret);
        }

        return bExists;
    }

    /**
     * Get current container ty2pe
     */
    GSContainerType Container::get_type() {
        GSContainerType containerType;
        GSResult ret = gsGetContainerType(mContainer, &containerType);

        if (ret != GS_RESULT_OK) {
            throw GSException(mContainer, ret);
        }

        return containerType;
    }

    /**
     * Rolls back the result of the current transaction and starts a new transaction in the manual commit mode.
     */
    void Container::abort() {
        GSResult ret = gsAbort(mContainer);

        if (ret != GS_RESULT_OK) {
            throw GSException(mContainer, ret);
        }
    }

    /**
     * Create query from input string.
     */
    Query* Container::query(const char* query) {
        GSQuery *pQuery;
        GSResult ret = gsQuery(mContainer, query, &pQuery);

        if (ret != GS_RESULT_OK) {
            throw GSException(mContainer, ret);
        }

        return new Query(pQuery, mContainerInfo, mRow);
    }

    /**
     * Set auto commit to true or false.
     */
    void Container::set_auto_commit(bool enabled){
        GSBool gsEnabled;
        gsEnabled = (enabled == true ? GS_TRUE:GS_FALSE);
        GSResult ret = gsSetAutoCommit(mContainer, gsEnabled);
        if (ret != GS_RESULT_OK) {
            throw GSException(mContainer, ret);
        }
    }

    /**
     * Commit changes to database when autocommit is set to false.
     */
    void Container::commit() {
        GSResult ret = gsCommit(mContainer);
        if (ret != GS_RESULT_OK) {
            throw GSException(mContainer, ret);
        }
    }

    /**
     * Returns the content of a Row.
     */
    GSBool Container::get(Field* keyFields, Row *rowdata) {
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
        case GS_TYPE_BYTE:
            keyFields->value.asInteger = keyFields->value.asByte;
        case GS_TYPE_SHORT:
            keyFields->value.asInteger = keyFields->value.asShort;
        case GS_TYPE_INTEGER:
            if (!(mContainerInfo->columnInfoList[0].type == GS_TYPE_INTEGER ||
                    mContainerInfo->columnInfoList[0].type == GS_TYPE_LONG)) {
                throw GSException("wrong type of rowKey");
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
                throw GSException("wrong type of rowKey");
            }
            key = &keyFields->value.asTimestamp;
            break;
        default:
            throw GSException("wrong type of rowKey timestamp");
        }

        ret = gsGetRow(mContainer, key, mRow, &exists);
        if (ret != GS_RESULT_OK) {
            throw GSException(mContainer, ret);
        }

        if(exists) {
            rowdata->set_from_row(mRow);
        }

        return exists;
    }

    /*
     * Deletes a Row corresponding to Row key
     */
    bool Container::remove(Field* keyFields) {
        GSBool exists = GS_FALSE;
        GSResult ret;
#if GS_COMPATIBILITY_SUPPORT_3_5
        if (keyFields->type == GS_TYPE_NULL) {
            ret = gsDeleteRow(mContainer, NULL, &exists);
        } else {
#endif
            switch (keyFields->type) {
            case GS_TYPE_STRING:
                if (mContainerInfo->columnInfoList[0].type != GS_TYPE_STRING) {
                    throw GSException("wrong type of rowKey string");
                }
                ret = gsDeleteRow(mContainer, &keyFields->value.asString, &exists);
                break;
            case GS_TYPE_BYTE:
                keyFields->value.asInteger = keyFields->value.asByte;
            case GS_TYPE_SHORT:
                keyFields->value.asInteger = keyFields->value.asShort;
            case GS_TYPE_INTEGER:
                if (!(mContainerInfo->columnInfoList[0].type == GS_TYPE_INTEGER ||
                    mContainerInfo->columnInfoList[0].type == GS_TYPE_LONG)) {
                    throw GSException("wrong type of rowKey");
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
                    throw GSException("wrong type of rowKey");
                }
                ret = gsDeleteRow(mContainer, &keyFields->value.asTimestamp, &exists);
                break;
            default:
                throw GSException("wrong type of rowKey timestamp");
            }
#if GS_COMPATIBILITY_SUPPORT_3_5
        }
#endif

        if (ret != GS_RESULT_OK) {
            throw GSException(mContainer, ret);
        }

        return (bool) exists;
    }

    /**
     * Multiput data
     */
    void Container::multi_put(Row** listRowdata, int rowCount) {
        GSResult ret;
        GSBool bExists;
        GSRow** rowObjs = (GSRow**) malloc(rowCount * sizeof(GSRow *));

        for (int i = 0; i < rowCount; i++) {
            GSRow *gsrow;
            gsCreateRowByContainer(mContainer, &gsrow);
            Row* tmpRow = listRowdata[i];
            tmpRow->set_for_row(gsrow, NULL);
            rowObjs[i] = gsrow;
        }

        //data for each container
        ret = gsPutMultipleRows(mContainer, (const void * const *) rowObjs,
                rowCount, &bExists);

        for (int rowNum = 0; rowNum < rowCount; rowNum++) {
            GSRow* gsRow = (GSRow *) rowObjs[rowNum];
            gsCloseRow(&gsRow);
        }

        free((void*) rowObjs);
        if (ret != GS_RESULT_OK) {
            throw GSException(mContainer, ret);
        }
    }

    /**
     * Support Store::multi_put
     */
    GSContainer* Container::getGSContainerPtr(){
        return mContainer;
    }

    /**
     * Find column index from column name.
     * Return -1 if not found
     */
    int Container::get_column_index(const char* columnName){
        for (int i = 0; i < mContainerInfo->columnCount; i++) {
            if (strcmp(mContainerInfo->columnInfoList[i].name, columnName) == 0) {
                return i;
            }
        }
        return -1;
    }

    /**
     * Support put row
     */
    GSType* Container::getGSTypeList(){
        if (typeList == NULL){
             typeList = (GSType*) malloc(sizeof(GSType) * mContainerInfo->columnCount);
            for (int i = 0; i < mContainerInfo->columnCount; i++){
                typeList[i] = mContainerInfo->columnInfoList[i].type;
            }
        }
        return typeList;
    }

    /**
     * Support put row
     */
    int Container::getColumnCount(){
        return mContainerInfo->columnCount;
    }
}
