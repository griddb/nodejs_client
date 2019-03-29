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

#include "Query.h"

namespace griddb {

    /**
     * @brief Constructor a new Query::Query object
     * @param *query A pointer holding the information about a query related to a specific GSContainer
     * @param *containerInfo A pointer holding the information about a specific GSContainer
     * @param *gsRow A pointer holding the information about a row related to a specific GSContainer
     */
    Query::Query(GSQuery *query, GSContainerInfo *containerInfo, GSRow *gsRow) : mQuery(query),
            mContainerInfo(containerInfo), mRow(gsRow) {
    }

    Query::~Query() {
        close();
    }

    /**
     * @brief Release Query resource
     */
    void Query::close() {
        if (mQuery) {
            gsCloseQuery(&mQuery);
            mQuery = NULL;
        }
    }

    /**
     * @brief Fetch data from query result.
     * @param for_update Indicates whether it requests a lock for update or not
     * @return  The pointer to a pointer variable to store GSRowSet instance
     */
    RowSet* Query::fetch(bool for_update) {
        GSRowSet *gsRowSet;
        // Call method from C-Api.
        GSBool gsForUpdate = (for_update == true ? GS_TRUE:GS_FALSE);
        GSResult ret = gsFetch(mQuery, gsForUpdate, &gsRowSet);

        // Check ret, if error, throw exception
        if (!GS_SUCCEEDED(ret)) {
            throw GSException(mQuery, ret);
        }

        try {
            RowSet* rowset = new RowSet(gsRowSet, mContainerInfo, mRow);
            return rowset;
        } catch (bad_alloc& ba) {
            gsCloseRowSet(&gsRowSet);
            throw GSException(mQuery, "Memory allocation error");
        }
    }

    /**
     * @brief Get row set.
     * @return The pointer to a pointer variable to store GSRowSet instance
     */
    RowSet* Query::get_row_set() {
        GSRowSet *gsRowSet;
        GSResult ret = gsGetRowSet(mQuery, &gsRowSet);

        // Check ret, if error, throw exception
        if (!GS_SUCCEEDED(ret)) {
            throw GSException(mQuery, ret);
        }

        try {
            RowSet* rowset = new RowSet(gsRowSet, mContainerInfo, mRow);
            return rowset;
        } catch (bad_alloc& ba) {
            gsCloseRowSet(&gsRowSet);
            throw GSException(mQuery, "Memory allocation error");
        }
    }

    /**
     * @brief Get raw pointer of GSQuery
     * @return A pointer store raw pointer of GSQuery
     */
    GSQuery* Query::gs_ptr() {
        return mQuery;
    }

    /**
     * @brief Set fetch limit option for a result acquisition.
     * @param limit The maximum number of Rows to be fetched.
     * @param partial The option value for GSFetchOption
     */
    void Query::set_fetch_options(int limit, bool partial){
        GSResult ret;
        ret = gsSetFetchOption(mQuery, GS_FETCH_LIMIT, &limit, GS_TYPE_INTEGER);
        if (!GS_SUCCEEDED(ret)) {
            throw GSException(mQuery, ret);
        }
#if GS_COMPATIBILITY_SUPPORT_4_0
        //Need to call gsSetFetchOption as many as the number of options
        ret = gsSetFetchOption(mQuery, GS_FETCH_PARTIAL_EXECUTION, &partial, GS_TYPE_BOOL);
        if (!GS_SUCCEEDED(ret)) {
            throw GSException(mQuery, ret);
        }
#endif
    }
}
