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

#ifndef _TIMESERIESPROPERTIES_H_
#define _TIMESERIESPROPERTIES_H_

#include "gridstore.h"

namespace griddb {

class TimeSeriesProperties {
    GSTimeSeriesProperties mTsProps;

    public:
        TimeSeriesProperties(const GSTimeSeriesProperties* timeSeriesProps);
        TimeSeriesProperties(int32_t elapsedTime, GSTimeUnit timeUnit, int32_t ExpirationDivisionCount);
        ~TimeSeriesProperties();

        void set_row_expiration_time(int32_t elapsedTime, GSTimeUnit timeUnit);
        void set_expiration_division_count(int32_t count);

        int get_row_expiration_time();
        GSTimeUnit get_row_expiration_time_unit();
        int get_expiration_division_count();
        GSTimeSeriesProperties* gs_ptr();
};

} /* namespace griddb */

#endif /* Define _TIMESERIESPROPERTIES_H_ */
