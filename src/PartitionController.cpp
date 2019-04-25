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

#include "PartitionController.h"

namespace griddb {

    /**
     * @brief Constructor a new PartitionController::PartitionController object
     * @param *controller A pointer for acquiring and processing the partition status
     */
    PartitionController::PartitionController(GSPartitionController *controller) :
        mController(controller) {
    }

    /**
     * @brief Destructor to free resources for a PartitionController object
     */
    PartitionController::~PartitionController() {
        close();
    }

    /**
     * @brief Release PartitionController resource
     */
    void PartitionController::close() {
        if (mController != NULL) {
            gsClosePartitionController(&mController);
            mController = NULL;
        }
    }

    /**
     * @brief Get partition count
     * @return The number of partitions in the target GridDB cluster
     */
    int32_t PartitionController::get_partition_count() {
        int32_t value;
        GSResult ret = gsGetPartitionCount(mController, &value);

        // Check ret, if error, throw exception
        if (!GS_SUCCEEDED(ret)) {
            throw GSException(mController, ret);
        }
        return value;
    }

    /**
     * @brief Get container partition count
     * @param partition_index The partition index, from 0 to the number of partitions minus one
     * @return The number of Container
     */
    int64_t PartitionController::get_container_count(int32_t partition_index) {
        int64_t value;
        GSResult ret = gsGetPartitionContainerCount(mController, partition_index, &value);

        // Check ret, if error, throw exception
        if (!GS_SUCCEEDED(ret)) {
            throw GSException(mController, ret);
        }
        return value;
    }

    /**
     * @brief Get a list of the Container names belonging to a specified partition.
     * @param partition_index The partition index, from 0 to the number of partitions minus one
     * @param start The start position of the acquisition range. A value must be greater than or equal to 0
     * @param ***stringList The pointer to a pointer variable to store the array list of Container name
     * @param *size The pointer to a variable to store the number of array elements of the Container name list
     * @param limit The upper limit of the number of cases acquired
     */
    void PartitionController::get_container_names(int32_t partition_index, int64_t start,
            const GSChar * const ** stringList, size_t *size, int64_t limit) {
        int64_t* limitPtr;
        if (limit >= 0) {
            limitPtr = &limit;
        } else {
            limitPtr = NULL;
        }
        GSResult ret = gsGetPartitionContainerNames(mController, partition_index, start, limitPtr, stringList, size);

        if (!GS_SUCCEEDED(ret)) {
            throw GSException(mController, ret);
        }
    }

    /**
     * @brief Get the partition index corresponding to the specified Container name.
     * @param *container_name Container name
     * @return The partition index
     */
    int32_t PartitionController::get_partition_index_of_container(const GSChar* container_name) {
        int32_t value;
        GSResult ret = gsGetPartitionIndexOfContainer(mController, container_name, &value);

        // Check ret, if error, throw exception
        if (!GS_SUCCEEDED(ret)) {
            throw GSException(mController, ret);
        }
        return value;
    }

} /* namespace griddb */
