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

#ifndef _CONTAINERINFO_H_
#define _CONTAINERINFO_H_

#include <string>
#include <cstring>
#include "TimeSeriesProperties.h"
#include "ExpirationInfo.h"
#include <utility>
#include "GSException.h"

//Support column_info_list attribute
struct ColumnInfoList {
    GSColumnInfo* columnInfo;
    size_t size;
};

using namespace std;

namespace griddb {

class ContainerInfo {
    /**
     * Contains information about a specific container
     */
    GSContainerInfo mContainerInfo;

    //tmp attribute to get column info list
    ColumnInfoList mColumnInfoList;

    //tmp attribute support get expiration attribute
    ExpirationInfo* mExpInfo;

    public:
        ContainerInfo(GSContainerInfo *containerInfo);
        ContainerInfo(const GSChar* name, const GSColumnInfo* props,
                int propsCount, GSContainerType type = GS_CONTAINER_COLLECTION,
                bool row_key = true, ExpirationInfo* expiration = NULL);
        ~ContainerInfo();

        void set_name(GSChar* containerName);
        void set_type(GSContainerType containerType);
        void set_row_key_assigned(bool rowKeyAssigned);
        const GSChar* get_name();
        GSContainerType get_type();
        GSColumnInfo get_column_info(size_t column);
        ColumnInfoList get_column_info_list();
        void set_column_info_list(ColumnInfoList columnInfoList);
        ExpirationInfo& get_expiration_info();
        void set_expiration_info(ExpirationInfo expirationInfo);
        bool get_row_key_assigned();
        GSContainerInfo* gs_info();

    private:
        void init(const GSChar* name, GSContainerType type, const GSColumnInfo* props,
                int propsCount, bool rowKeyAssigned, ExpirationInfo* expiration);
};

} /* namespace griddb */

#endif /* Define _CONTAINERINFO_H_ */
