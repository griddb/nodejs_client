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

#include "QueryAnalysisEntry.h"

#include <assert.h>

namespace griddb {

    /**
     * @brief Constructor a new QueryAnalysisEntry::QueryAnalysisEntry object
     * @param *queryAnalysis Represents one of information entries composing a query plan and the results of analyzing a query operation.
     */
    QueryAnalysisEntry::QueryAnalysisEntry(GSQueryAnalysisEntry* queryAnalysis) : mQueryAnalysis(NULL) {
        if (!queryAnalysis) {
            return;
        }

        try {
            if (!mQueryAnalysis) {
                mQueryAnalysis = new GSQueryAnalysisEntry();
            }

            mQueryAnalysis->statement = NULL;
            mQueryAnalysis->type = NULL;
            mQueryAnalysis->value = NULL;
            mQueryAnalysis->valueType = NULL;
            
            if (queryAnalysis->statement) {
                Util::strdup(&(mQueryAnalysis->statement), queryAnalysis->statement);
            }
            
            if (queryAnalysis->type) {
                Util::strdup(&(mQueryAnalysis->type), queryAnalysis->type);
            }
            
            if (queryAnalysis->value) {
                Util::strdup(&(mQueryAnalysis->value), queryAnalysis->value);
            }

            if (queryAnalysis->valueType) {
                Util::strdup(&(mQueryAnalysis->valueType), queryAnalysis->valueType);
            }
        } catch (bad_alloc& ba) {
            this->freeMemory();
            throw GSException(mQueryAnalysis, "Memory allocation error");
        }

        //Copy value which queryAnalysis point to
        mQueryAnalysis->id = queryAnalysis->id;
        mQueryAnalysis->depth = queryAnalysis->depth;
    }

    QueryAnalysisEntry::~QueryAnalysisEntry() {
        this->close();
    }

    /**
     * @brief Release QueryAnalysisEntry resource
     */
    void QueryAnalysisEntry::close() {
        this->freeMemory();
        if (mQueryAnalysis) {
            mQueryAnalysis = NULL;
        }
    }

    void QueryAnalysisEntry::freeMemory() {
        if (mQueryAnalysis) {
            if (mQueryAnalysis->statement) {
                delete[] mQueryAnalysis->statement;
            }
            if (mQueryAnalysis->type) {
                delete[] mQueryAnalysis->type;
            }
            if (mQueryAnalysis->value) {
                delete[] mQueryAnalysis->value;
            }
            if (mQueryAnalysis->valueType) {
                delete[] mQueryAnalysis->valueType;
            }
            delete mQueryAnalysis;
        }
    }

    /**
     * @brief get QueryAnalysisEntry data
     * @param *queryAnalysis Represents one of information entries composing a query plan and the results of analyzing a query operation.
     */
    void QueryAnalysisEntry::get(GSQueryAnalysisEntry* queryAnalysis) {
        assert(queryAnalysis != NULL);
        queryAnalysis->id = mQueryAnalysis->id;
        queryAnalysis->depth = mQueryAnalysis->depth;
        queryAnalysis->statement = NULL;
        queryAnalysis->type = NULL;
        queryAnalysis->value = NULL;
        queryAnalysis->valueType = NULL;

        try {
            if (mQueryAnalysis->statement) {
                Util::strdup(&(queryAnalysis->statement), mQueryAnalysis->statement);
            }

            if (mQueryAnalysis->type) {
                Util::strdup(&(queryAnalysis->type), mQueryAnalysis->type);
            }

            if (mQueryAnalysis->value) {
                Util::strdup(&(queryAnalysis->value), mQueryAnalysis->value);
            }

            if (mQueryAnalysis->valueType) {
                Util::strdup(&(queryAnalysis->valueType), mQueryAnalysis->valueType);
            }
        } catch (bad_alloc& ba) {
            if (queryAnalysis->statement) {
                delete[] queryAnalysis->statement;
            }
            if (queryAnalysis->type) {
                delete[] queryAnalysis->type;
            }
            if (queryAnalysis->value) {
                delete[] queryAnalysis->value;
            }
            if (queryAnalysis->valueType) {
                delete[] queryAnalysis->valueType;
            }
            throw GSException(mQueryAnalysis, "Memory allocation error");
        }
    }
}
