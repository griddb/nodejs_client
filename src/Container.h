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

#ifndef _CONTAINER_H_
#define _CONTAINER_H_

#include <assert.h>

#include "Field.h"
#include "Query.h"
#include "GSException.h"
#include "Util.h"

using namespace std;

namespace griddb {

class Container {

    GSContainerInfo* mContainerInfo;
    GSContainer *mContainer;

    friend class Store;

    GSRow* mRow;
    GSType* mTypeList;

    public:
        bool timestamp_output_with_float;
        ~Container();
        void close(GSBool allRelated = GS_FALSE);
        GSContainerType get_type();
        void create_index(const char* column_name, GSIndexTypeFlags index_type = GS_INDEX_FLAG_DEFAULT, const char* name=NULL);
        void drop_index(const char* column_name, GSIndexTypeFlags index_type = GS_INDEX_FLAG_DEFAULT, const char* name=NULL);
        bool put(GSRow *row);
        Query* query(const char *query);
        void abort();
        void flush();
        void set_auto_commit(bool enabled);
        void commit();
        GSBool get(Field* keyFields, GSRow *rowdata);
        bool remove(Field* keyFields);
        void multi_put(GSRow** listRowdata, int rowCount);
        GSContainer* getGSContainerPtr();
        GSType* getGSTypeList();
        int getColumnCount();
        GSRow* getGSRowPtr();

    private:
        Container(GSContainer *container, GSContainerInfo* containerInfo);
        void freeMemoryContainer();
};

} /* namespace griddb */

#endif /* _CONTAINER_H_ */
