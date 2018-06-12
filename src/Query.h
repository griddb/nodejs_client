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

#ifndef _QUERY_H_
#define _QUERY_H_

#include <memory>
#include "gridstore.h"
#include "RowSet.h"
#include "GSException.h"
using namespace std;

namespace griddb {

/**
 * Convert from GSQuery
 */
class Query {
    GSQuery *mQuery;

    friend class Container;

    GSContainerInfo *mContainerInfo;
    GSRow* mRow;

    public:
        ~Query();
        void close();
        RowSet* fetch(bool for_update = false);
        void set_fetch_options(int limit = -1, bool partial = false);
        RowSet* get_row_set();
        GSQuery* gs_ptr();

    private:
        Query(GSQuery *query, GSContainerInfo *containerInfo, GSRow *gsRow);

};

}

#endif /* _QUERY_H_ */
