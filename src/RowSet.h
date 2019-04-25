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

#ifndef _ROWSET_H_
#define _ROWSET_H_

#include <memory>
#include <string>
#include <stdio.h>
#include <time.h>
#include <assert.h>

#include "gridstore.h"
#include "Field.h"
#include "AggregationResult.h"
#include "QueryAnalysisEntry.h"
#include "GSException.h"
#include "Util.h"

using namespace std;

namespace griddb {

/**
 * Convert from GSRowSet
 */
class RowSet {
    GSRowSet *mRowSet;
    GSContainerInfo *mContainerInfo;
    GSRow *mRow;
    GSType* typeList;

    friend class Query;

    GSRowSetType mType;

    public:
        bool timestamp_output_with_float;
        ~RowSet();
        void close();
        int32_t size();
        // Iterator
        bool has_next();
        void next(GSRowSetType* type, bool* hasNextRow,
                QueryAnalysisEntry** queryAnalysis, AggregationResult** aggResult);
        void update(GSRow* row);
        void remove();
        GSRowSetType type();
        void get_column_names(char*** listName, int* num);
        QueryAnalysisEntry* get_next_query_analysis();
        AggregationResult* get_next_aggregation();
        void next_row(bool* hasNextRow);
        GSType* getGSTypeList();
        int getColumnCount();

        GSRow* getGSRowPtr();

    private:
        RowSet(GSRowSet *rowSet, GSContainerInfo *containerInfo, GSRow *mRow);
};

}

#endif /* _ROWSET_H_ */
