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
     * Check if RowSet has next row data. Convert from gsHasNextRow.
     */
    bool RowSet::has_next() {
        GSRowSetType type;
        type = this->type();
        switch(type) {
        case (GS_ROW_SET_CONTAINER_ROWS):
        case (GS_ROW_SET_AGGREGATION_RESULT):
        case (GS_ROW_SET_QUERY_ANALYSIS):
            return (bool) gsHasNextRow(mRowSet);
            break;
        default:
            return false;
            break;
        }
    }
    RowSet::~RowSet() {
        close();
        if (typeList) {
            free(typeList);
        }
    }
    /**
     * Close rowset.
     */
    void RowSet::close() {
        if (mRowSet != NULL) {
            gsCloseRowSet(&mRowSet);
            mRowSet = NULL;
        }

    }
    /**
     * Update current row from RowSet
     */
    void RowSet::update(GSRow* row) {
        GSResult ret = gsUpdateCurrentRow(mRowSet, mRow);

        if (ret != GS_RESULT_OK) {
            throw GSException(mRowSet, ret);
        }
    }
    /**
     * Get next row data. Convert from gsGetNextRow.
     */
    void RowSet::next_row(bool* hasNextRow) {
        *hasNextRow = this->has_next();
        if (*hasNextRow) {
            GSResult ret = gsGetNextRow(mRowSet, mRow);
            if (ret != GS_RESULT_OK) {
                throw GSException(mRowSet, ret);
            }
        }
    }
    /**
     * Get next row
     */
    void RowSet::next(GSRowSetType* type, bool* hasNextRow,
            QueryAnalysisEntry** queryAnalysis, AggregationResult** aggResult){
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
     * Return size of this rowset
     */
    int32_t RowSet::size() {
        return gsGetRowSetSize(mRowSet);
    }

    /**
     * Delete current row data. Convert from C-API: gsDeleteCurrentRow.
     */
    void RowSet::remove() {
        GSResult ret = gsDeleteCurrentRow(mRowSet);
        if (ret != GS_RESULT_OK) {
            throw GSException(mRowSet, ret);
        }
    }

    /**
     * Moves to the next Row in a Row set and returns the aggregation result at the moved position.
     */
    AggregationResult* RowSet::get_next_aggregation() {
        GSAggregationResult* pAggResult;

        GSResult ret = gsGetNextAggregation(mRowSet, &pAggResult);
        if (ret != GS_RESULT_OK) {
            throw GSException(mRowSet, ret);
        }

        return new AggregationResult(pAggResult);
    }

    /**
     * Get current row type. Convert from C-API: gsGetRowSetType.
     */
    GSRowSetType RowSet::type(){
        return mType;
    }

    /**
     * Get next query analysis
     */
    QueryAnalysisEntry* RowSet::get_next_query_analysis(){
    	GSQueryAnalysisEntry queryAnalysis = GS_QUERY_ANALYSIS_ENTRY_INITIALIZER;
        GSResult ret;
        ret = gsGetNextQueryAnalysis(mRowSet, &queryAnalysis);
        if (ret != GS_RESULT_OK) {
            throw GSException(mRowSet, ret);
        }
        return new QueryAnalysisEntry(&queryAnalysis);
    }

    /**
     * Get column name from RowSet. Use in python only.
     */
    void RowSet::get_column_names(char*** listName, int* num){
        if (mContainerInfo){
            //Memory will be free from typemap
            (*listName) = (char **) malloc(mContainerInfo->columnCount * sizeof(char*));
            *num = mContainerInfo->columnCount;
            for (int i = 0; i < mContainerInfo->columnCount; i++){
                if (mContainerInfo->columnInfoList[i].name) {
                    (*listName)[i] = strdup(mContainerInfo->columnInfoList[i].name);
                } else {
                    (*listName)[i] = NULL;
                }
            }
        }
    }

    /**
     * Support put row
     */
    GSType* RowSet::getGSTypeList(){
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
    int RowSet::getColumnCount(){
        return mContainerInfo->columnCount;
    }

    GSRow* RowSet::getGSRowPtr(){
        return mRow;
    }

}
