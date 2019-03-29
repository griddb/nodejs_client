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

    /**
     * @brief Constructor a new TimeSeriesProperties::TimeSeriesProperties object
     * @param *timeSeriesProps Represents the information about optional configuration settings used for newly creating or updating a TimeSeries
     */
    TimeSeriesProperties::TimeSeriesProperties(const GSTimeSeriesProperties* timeSeriesProps) : mTsProps(*timeSeriesProps) {
    }

    /**
     * @brief Constructor a new TimeSeriesProperties::TimeSeriesProperties object
     * @param elapsedTime The elapsed time period of a Row to be used as the basis of the validity period
     * @param timeUnit The unit of elapsed time referenced for the expiration date of a Row
     * @param ExpirationDivisionCount The division number for the validity period as the number of expired Row data units to be released
     */
    TimeSeriesProperties::TimeSeriesProperties(int32_t elapsedTime, GSTimeUnit timeUnit,
            int32_t ExpirationDivisionCount) : mTsProps{elapsedTime, timeUnit, -1,
            timeUnit, GS_COMPRESSION_NO, 0, NULL, ExpirationDivisionCount} {
    }

    TimeSeriesProperties::~TimeSeriesProperties() {
    }

    /**
     * @brief Set rowExpiration for TimeSeriesProperties.
     * @param elapsedTime The elapsed time period of a Row to be used as the basis of the validity period
     * @param timeUnit The unit of elapsed time referenced for the expiration date of a Row
     */
    void TimeSeriesProperties::set_row_expiration_time(int elapsedTime, GSTimeUnit timeUnit) {
        mTsProps.rowExpirationTime = elapsedTime;
        mTsProps.rowExpirationTimeUnit = timeUnit;
    }

    /**
     * @brief Set expirationDivisionCount for TimeSeriesProperties
     * @param count The division number for the validity period as the number of expired Row data units to be released
     */
    void TimeSeriesProperties::set_expiration_division_count(int count) {
        mTsProps.expirationDivisionCount = count;
    }

    /**
     * @brief Get rowExpirationTime from TimeSeriesProperties
     * @return The elapsed time period of a Row to be used as the basis of the validity period
     */
    int TimeSeriesProperties::get_row_expiration_time() {
        return mTsProps.rowExpirationTime;
    }

    /**
     * @brief Get rowExpirationTimeUnit from TimeSeriesProperties
     * @return timeUnit The unit of elapsed time referenced for the expiration date of a Row
     */
    GSTimeUnit TimeSeriesProperties::get_row_expiration_time_unit() {
        return mTsProps.rowExpirationTimeUnit;
    }

    /**
     * @brief Get expirationDivisionCount from TimeSeriesProperties
     * @return The division number for the validity period as the number of expired Row data units to be released
     */
    int TimeSeriesProperties::get_expiration_division_count() {
        return mTsProps.expirationDivisionCount;
    }

    /**
     * @brief Get attribute: mTsProps
     * @return A pointer store GSTimeSeriesProperties of TimeSeriesProperties
     */
    GSTimeSeriesProperties* TimeSeriesProperties::gs_ptr() {
        return &mTsProps;
    }

} /* namespace griddb */
