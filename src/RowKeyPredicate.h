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

#ifndef _ROWKEYPREDICATE_H_
#define _ROWKEYPREDICATE_H_

#include <string>
#include <vector>
#include "gridstore.h"
#include "Row.h"
#include <string.h>


using namespace std;

namespace griddb {

class RowKeyPredicate {
    GSRowKeyPredicate *mPredicate;
    GSType mType;

    friend class Store;

    public:
        bool timestamp_output_with_float;
        ~RowKeyPredicate();
        void close();

        void get_range(Field* startField, Field* finishField);
        void set_range(Field* startKey, Field* finishKey);
        void set_distinct_keys(const Field *keys, size_t keyCount);
        void get_distinct_keys(Field **keys, size_t* keyCount);
        GSRowKeyPredicate* gs_ptr();
        GSType get_key_type();

    private:
        RowKeyPredicate(GSRowKeyPredicate *predicate, GSType type);

};

} /* namespace griddb */

#endif /* _ROWKEYPREDICATE_H_ */
