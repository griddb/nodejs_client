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

#ifndef _STORE_H_
#define _STORE_H_

#include <map>
#include <vector>
#include <assert.h>

#include "ContainerInfo.h"
#include "Container.h"
#include "PartitionController.h"
#include "RowKeyPredicate.h"
#include "GSException.h"

using namespace std;

namespace griddb {

class Store {
    GSGridStore *mStore;

    friend class StoreFactory;

    public:
        bool timestamp_output_with_float;
        ~Store();
        void close(GSBool allRelated = GS_FALSE);

        Container* put_container(ContainerInfo* info, bool modifiable = false);
        Container* get_container(const char* name);
        void drop_container(const char *name);

        void fetch_all(GSQuery* const * queryList, size_t queryCount);
        void multi_put(GSRow*** listRow, const int *listRowContainerCount,
                const char ** listContainerName, size_t containerCount);
        void multi_get(const GSRowKeyPredicateEntry* const * predicateList,
                size_t predicateCount, GSContainerRowEntry **entryList, size_t* containerCount,
                int **colNumList, GSType*** typeList, int **orderFromInput);

        ContainerInfo* get_container_info(const char *name);
        PartitionController* partition_info();
        RowKeyPredicate* create_row_key_predicate(GSType type);

    private:
        Store(GSGridStore* store);
        void freeMemoryMultiGet(int** colNumList, GSType*** typeList, int length, int** orderFromInput);
        bool setMultiContainerNumList(const GSRowKeyPredicateEntry* const * predicateList,
                int length, int*** colNumList, GSType**** typeList);
};

}

#endif /* Define _STORE_H */
