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

#ifndef _AGGREGATIONRESULT_H_
#define _AGGREGATIONRESULT_H_

#include "GSException.h"
#include "Row.h"
#include "gridstore.h"

using namespace std;

namespace griddb {

class AggregationResult {

    GSAggregationResult* mAggResult;

    friend class RowSet;

    public:
        bool timestamp_output_with_float;
        ~AggregationResult();
        void close();

        void get(GSType type, griddb::Field *agValue);
        AggregationResult(GSAggregationResult* aggResult);

    private:

};

} /* namespace griddb */

#endif /* _AGGREGATIONRESULT_H_ */
