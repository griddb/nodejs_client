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

namespace griddb {

    QueryAnalysisEntry::QueryAnalysisEntry(GSQueryAnalysisEntry* queryAnalysis) : mQueryAnalysis(NULL) {
        if (queryAnalysis) {
            mQueryAnalysis = (GSQueryAnalysisEntry*) malloc(sizeof(GSQueryAnalysisEntry));
            //Copy value which queryAnalysis point to
            mQueryAnalysis->id = queryAnalysis->id;
            mQueryAnalysis->depth = queryAnalysis->depth;
            mQueryAnalysis->statement = strdup(queryAnalysis->statement);
            mQueryAnalysis->type = strdup(queryAnalysis->type);
            mQueryAnalysis->value = strdup(queryAnalysis->value);
            mQueryAnalysis->valueType = strdup(queryAnalysis->valueType);
        }
}

    QueryAnalysisEntry::~QueryAnalysisEntry() {
        if (mQueryAnalysis) {
            if (mQueryAnalysis->statement) {
                free((void*) mQueryAnalysis->statement);
            }
            if (mQueryAnalysis->type) {
                free((void*) mQueryAnalysis->type);
            }
            if (mQueryAnalysis->value) {
                free((void*) mQueryAnalysis->value);
            }
            if (mQueryAnalysis->valueType) {
                free((void*) mQueryAnalysis->valueType);
            }
            free((void *) mQueryAnalysis);
            mQueryAnalysis = NULL;
        }
}

    /**
     * get QueryAnalysisEntry data
     */
    void QueryAnalysisEntry::get(GSQueryAnalysisEntry* queryAnalysis) {
        queryAnalysis->id = mQueryAnalysis->id;
        queryAnalysis->depth = mQueryAnalysis->depth;
        queryAnalysis->statement = strdup(mQueryAnalysis->statement);
        queryAnalysis->type = strdup(mQueryAnalysis->type);
        queryAnalysis->value = strdup(mQueryAnalysis->value);
        queryAnalysis->valueType = strdup(mQueryAnalysis->valueType);
    }
}
