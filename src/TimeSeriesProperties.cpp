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

#include "TimeSeriesProperties.h"

namespace griddb {

    TimeSeriesProperties::TimeSeriesProperties(const GSTimeSeriesProperties* timeSeriesProps) : mTsProps(*timeSeriesProps) {
    }

    TimeSeriesProperties::TimeSeriesProperties(int32_t elapsedTime, GSTimeUnit timeUnit,
            int32_t ExpirationDivisionCount) : mTsProps{elapsedTime, timeUnit, -1,
            timeUnit, GS_COMPRESSION_NO, 0, NULL, ExpirationDivisionCount} {
    }
    TimeSeriesProperties::~TimeSeriesProperties() {
    }
    void TimeSeriesProperties::set_row_expiration_time(int elapsedTime,
            GSTimeUnit timeUnit) {
        mTsProps.rowExpirationTime = elapsedTime;
        mTsProps.rowExpirationTimeUnit = timeUnit;
    }
    void TimeSeriesProperties::set_expiration_division_count(int count) {
        mTsProps.expirationDivisionCount = count;
    }
    int TimeSeriesProperties::get_row_expiration_time() {
        return mTsProps.rowExpirationTime;
    }
    GSTimeUnit TimeSeriesProperties::get_row_expiration_time_unit() {
        return mTsProps.rowExpirationTimeUnit;
    }
    int TimeSeriesProperties::get_expiration_division_count() {
        return mTsProps.expirationDivisionCount;
    }
    GSTimeSeriesProperties* TimeSeriesProperties::gs_ptr() {
        return &mTsProps;
    }

} /* namespace griddb */
