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

#ifndef _PARTITIONCONTROLLER_H_
#define _PARTITIONCONTROLLER_H_

#include "gridstore.h"
#include "GSException.h"
#include <vector>

namespace griddb {

class PartitionController {
    GSPartitionController *mController;

    friend class Store;

    public:
        ~PartitionController();
        void close();
        int32_t get_partition_count();
        int64_t get_container_count(int32_t partition_index);
        void get_container_names(int32_t partition_index, int64_t start,
                const GSChar * const ** stringList, size_t *size, int64_t limit=-1);
        int32_t get_partition_index_of_container(const GSChar *container_name);

    private:
        PartitionController(GSPartitionController *controller);
};

} /* namespace griddb */

#endif /* _PARTITIONCONTROLLER_H_ */
