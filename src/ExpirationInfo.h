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

#ifndef _EXPIRATIONINFO_H_
#define _EXPIRATIONINFO_H_

#include <string>
#include <cstring>

using namespace std;

namespace griddb {
class ExpirationInfo {
    /*
     * Contains information about expiration configuration
     */
    GSTimeSeriesProperties mTimeSeriesProps;

    public:
        ExpirationInfo(const GSTimeSeriesProperties* timeSeriesProps) {
            mTimeSeriesProps.rowExpirationTime = timeSeriesProps->rowExpirationTime;
            mTimeSeriesProps.rowExpirationTimeUnit = timeSeriesProps->rowExpirationTimeUnit;
            mTimeSeriesProps.expirationDivisionCount = timeSeriesProps->expirationDivisionCount;
            mTimeSeriesProps.compressionList = NULL;
            mTimeSeriesProps.compressionListSize = 0;
            mTimeSeriesProps.compressionMethod = GS_COMPRESSION_NO;
            mTimeSeriesProps.compressionWindowSize = 0;
            mTimeSeriesProps.compressionWindowSizeUnit = GS_TIME_UNIT_YEAR;
        };
        ExpirationInfo(int time, GSTimeUnit unit, int division_count) {
            mTimeSeriesProps.rowExpirationTime = time;
            mTimeSeriesProps.rowExpirationTimeUnit = unit;
            mTimeSeriesProps.expirationDivisionCount = division_count;
            mTimeSeriesProps.compressionList = NULL;
            mTimeSeriesProps.compressionListSize = 0;
            mTimeSeriesProps.compressionMethod = GS_COMPRESSION_NO;
            mTimeSeriesProps.compressionWindowSize = 0;
            mTimeSeriesProps.compressionWindowSizeUnit = GS_TIME_UNIT_YEAR;
        };
        ~ExpirationInfo() {};
        int get_time() {return mTimeSeriesProps.rowExpirationTime;};
        void set_time(int time) {mTimeSeriesProps.rowExpirationTime = time;};
        GSTimeUnit get_time_unit() {return mTimeSeriesProps.rowExpirationTimeUnit;};
        void set_time_unit(GSTimeUnit unit) {mTimeSeriesProps.rowExpirationTimeUnit = unit;};
        int get_division_count() {return mTimeSeriesProps.expirationDivisionCount;};
        void set_division_count(int division_count) {mTimeSeriesProps.expirationDivisionCount = division_count;};
        GSTimeSeriesProperties* gs_ts() {
            return &mTimeSeriesProps;
        };
};

} /* namespace griddb */

#endif
