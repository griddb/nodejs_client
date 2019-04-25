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

#include "AggregationResult.h"

namespace griddb {

    /**
     * @brief Constructor a new Aggregation Result:: Aggregation Result object
     * @param aggResult Stores the result of an aggregation operation
     */
    AggregationResult::AggregationResult(GSAggregationResult* aggResult) :
            mAggResult(aggResult), timestamp_output_with_float(false) {
    }

    AggregationResult::~AggregationResult() {
        close();
    }

    /**
     * @brief Release AggregationResult resource
     */
    void AggregationResult::close() {
        if (mAggResult != NULL) {
            gsCloseAggregationResult(&mAggResult);
        }
        mAggResult = NULL;
    }

    /**
     * @brief Obtains the result of aggregating numeric-type values.
     * @param type Column type
     * @param *agValue aggregation result
     */
    void AggregationResult::get(GSType type, griddb::Field *agValue) {
        assert(agValue != NULL);
        void *value;
        agValue->type = type;
        switch (type) {
        case GS_TYPE_DOUBLE:
            value = &agValue->value.asDouble;
            break;
        case GS_TYPE_LONG:
            value = &agValue->value.asLong;
            break;
        case GS_TYPE_TIMESTAMP:
            value = &agValue->value.asTimestamp;
            break;
        default:
            throw GSException(mAggResult, "Not support type from Aggregation result");
            break;
        }
        GSBool ret = gsGetAggregationValue(mAggResult, value, type);
        if (ret == GS_FALSE) {
            throw GSException(mAggResult,
                    "Value cannot be retrieved from Aggregation result");
        }
    }

} /* namespace griddb */
