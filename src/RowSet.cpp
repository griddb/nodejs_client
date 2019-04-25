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

#include "RowSet.h"

namespace griddb {

    /**
     * @brief Constructor a new RowSet::RowSet object
     * @param *rowSet A pointer manages a set of Rows obtained by a query
     * @param *containerInfo A pointer holding the information about a specific GSContainer
     * @param *gsRow A pointer holding the information about a row related to a specific GSContainer
     */
    RowSet::RowSet(GSRowSet *rowSet, GSContainerInfo *containerInfo, GSRow *gsRow) :
        mRowSet(rowSet), mContainerInfo(containerInfo), mRow(gsRow),
        timestamp_output_with_float(false), typeList(NULL) {
        if (mRowSet != NULL) {
            mType = gsGetRowSetType(mRowSet);
        } else {
            throw GSException(mRowSet, "mRowSet is NULL");
        }
    }

    /**
     * @brief Check if RowSet has next row data.
     * @return Returns whether a Row set has at least one Row ahead of the current cursor position
     */
    bool RowSet::has_next() {
        GSRowSetType type;
        type = this->type();
        switch(type) {
        case (GS_ROW_SET_CONTAINER_ROWS):
        case (GS_ROW_SET_AGGREGATION_RESULT):
        case (GS_ROW_SET_QUERY_ANALYSIS):
            return (bool) gsHasNextRow(mRowSet);
        default:
            return false;
        }
    }

    RowSet::~RowSet() {
        close();
        if (typeList) {
            delete[] typeList;
        }
    }

    /**
     * @brief Release RowSet resource
     */
    void RowSet::close() {
        if (mRowSet != NULL) {
            gsCloseRowSet(&mRowSet);
            mRowSet = NULL;
        }
    }

    /**
     * @brief Update current row from RowSet
     * @param *row A Row object representing the content of a Row to be put to database
     */
    void RowSet::update(GSRow* row) {
        GSResult ret = gsUpdateCurrentRow(mRowSet, mRow);

        if (!GS_SUCCEEDED(ret)) {
            throw GSException(mRowSet, ret);
        }
    }

    /**
     * @brief Get next row data.
     * @param *hasNextRow Indicate whether there is any row in RowSet or not
     */
    void RowSet::next_row(bool* hasNextRow) {
        *hasNextRow = this->has_next();
        if (*hasNextRow) {
            GSResult ret = gsGetNextRow(mRowSet, mRow);
            if (!GS_SUCCEEDED(ret)) {
                throw GSException(mRowSet, ret);
            }
        }
    }

    /**
     * @brief Get next row or queryAnalysis or aggResult corresponding query command
     * @param *type The type of content that can be extracted from GSRowSet.
     * @param *hasNextRow Indicate whether there is any row in RowSet or not
     * @param **queryAnalysis Represents one of information entries composing a query plan and the results of analyzing a query operation.
     * @param **aggResult Stores the result of an aggregation operation.
     */
    void RowSet::next(GSRowSetType* type, bool* hasNextRow,
            QueryAnalysisEntry** queryAnalysis, AggregationResult** aggResult){
        assert(type != NULL);
        assert(hasNextRow != NULL);
        assert(queryAnalysis != NULL);
        assert(aggResult != NULL);
        *type = this->type();
        switch(*type) {
        case (GS_ROW_SET_CONTAINER_ROWS):
            this->next_row(hasNextRow);
            break;
        case (GS_ROW_SET_AGGREGATION_RESULT):
            *hasNextRow = this->has_next();
            *aggResult = this->get_next_aggregation();
            break;
        case (GS_ROW_SET_QUERY_ANALYSIS):
            *queryAnalysis = this->get_next_query_analysis();
            *hasNextRow = true;
            break;
        default:
            throw GSException(mRowSet, "type for rowset is not correct");
        }
    }

    /**
     * @brief Get size of this rowset
     * @return Size of this rowset
     */
    int32_t RowSet::size() {
        return gsGetRowSetSize(mRowSet);
    }

    /**
     * @brief Delete current row data.
     */
    void RowSet::remove() {
        GSResult ret = gsDeleteCurrentRow(mRowSet);
        if (!GS_SUCCEEDED(ret)) {
            throw GSException(mRowSet, ret);
        }
    }

    /**
     * @brief Moves to the next Row in a Row set and returns the aggregation result at the moved position.
     * @return A pointer Stores the result of an aggregation operation.
     */
    AggregationResult* RowSet::get_next_aggregation() {
        GSAggregationResult* pAggResult;

        GSResult ret = gsGetNextAggregation(mRowSet, &pAggResult);
        if (!GS_SUCCEEDED(ret)) {
            throw GSException(mRowSet, ret);
        }

        try {
            AggregationResult* aggResult = new AggregationResult(pAggResult);
            return aggResult;
        } catch (bad_alloc& ba) {
            gsCloseAggregationResult(&pAggResult);
            throw GSException(mRowSet, "Memory allocation error");
        }
    }

    /**
     * @brief Get current row type.
     * @return The type of content that can be extracted from GSRowSet.
     */
    GSRowSetType RowSet::type(){
        return mType;
    }

    /**
     * @brief Get next query analysis
     * @return Represents one of information entries composing a query plan and the results of analyzing a query operation.
     */
    QueryAnalysisEntry* RowSet::get_next_query_analysis(){
        GSQueryAnalysisEntry gsQueryAnalysis = GS_QUERY_ANALYSIS_ENTRY_INITIALIZER;
        GSResult ret;
        ret = gsGetNextQueryAnalysis(mRowSet, &gsQueryAnalysis);
        if (!GS_SUCCEEDED(ret)) {
            throw GSException(mRowSet, ret);
        }

        try {
            QueryAnalysisEntry* queryAnalysis = new QueryAnalysisEntry(&gsQueryAnalysis);
            return queryAnalysis;
        } catch (bad_alloc& ba) {
            throw GSException(mRowSet, "Memory allocation error");
        }
    }

    /**
     * @brief Get column name from RowSet. Use in python only.
     * @param ***listName List name of column
     * @param *num Number of column
     */
    void RowSet::get_column_names(char*** listName, int* num){
        assert(listName != NULL);
        assert(num != NULL);
        if (!mContainerInfo){
            return;
        }

        //Memory will be free from typemap
        try {
            (*listName) = new char*[mContainerInfo->columnCount]();
            *num = mContainerInfo->columnCount;
            for (int i = 0; i < mContainerInfo->columnCount; i++){
                if (mContainerInfo->columnInfoList[i].name) {
                    Util::strdup((const GSChar**)(&((*listName)[i])), mContainerInfo->columnInfoList[i].name);
                } else {
                    (*listName)[i] = NULL;
                }
            }
        } catch (bad_alloc& ba) {
            for (int i = 0; i < mContainerInfo->columnCount; i++){
                if ((*listName)[i]) {
                    delete[] (*listName)[i];
                }
            }
            if ((*listName)) {
                delete[] (*listName);
            }
            throw GSException(mRowSet, "Memory allocation error");
        }
    }

    /**
     * @brief Get list type of column in row
     * @return A list type of column in row
     */
    GSType* RowSet::getGSTypeList(){
        if (typeList == NULL) {
            try {
                typeList = new GSType[mContainerInfo->columnCount]();
            } catch (bad_alloc& ba) {
                throw GSException(mRowSet, "Memory allocation error");
            }
            
            for (int i = 0; i < mContainerInfo->columnCount; i++){
                typeList[i] = mContainerInfo->columnInfoList[i].type;
            }
        }
        return typeList;
    }

    /**
     * @brief Get number of column in row
     * @return Number of column in row
     */
    int RowSet::getColumnCount(){
        return mContainerInfo->columnCount;
    }

    /**
     * @brief Get row data in RowSet object
     * @return A pointer stores row data in RowSet object
     */
    GSRow* RowSet::getGSRowPtr(){
        return mRow;
    }

}
