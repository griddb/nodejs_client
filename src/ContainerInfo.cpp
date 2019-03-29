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

#include "ContainerInfo.h"

namespace griddb {

    /**
     * @brief Constructor a new ContainerInfo::ContainerInfo object
     * @param *containerInfo Stores the information about a specific Container
     */
    ContainerInfo::ContainerInfo(GSContainerInfo *containerInfo) {
        assert(containerInfo != NULL);
        init(containerInfo->name, containerInfo->type,
                containerInfo->columnInfoList, containerInfo->columnCount,
                containerInfo->rowKeyAssigned, NULL);
        //Assign values from argument to mContainer
        GSTimeSeriesProperties* gsProps = NULL;
        GSTriggerInfo* triggerInfoList = NULL;

        try {
            if (containerInfo->timeSeriesProperties) {
                gsProps = new GSTimeSeriesProperties();
            }

            if (containerInfo->triggerInfoList) {
                triggerInfoList = new GSTriggerInfo();
            }

            if (containerInfo->dataAffinity) {
                Util::strdup(&mContainerInfo.dataAffinity, containerInfo->dataAffinity);
            } else {
                mContainerInfo.dataAffinity = NULL;
            }
        } catch (bad_alloc& ba) {
            //case allocation memory error
            if (gsProps) {
                delete gsProps;
            }
            if (triggerInfoList) {
                delete triggerInfoList;
            }
            if (mContainerInfo.dataAffinity) {
                delete[] mContainerInfo.dataAffinity;
            }
            throw GSException("Memory allocation error");
        }

        if (containerInfo->timeSeriesProperties) {
            memcpy(gsProps, containerInfo->timeSeriesProperties, sizeof(GSTimeSeriesProperties));
        }
        mContainerInfo.timeSeriesProperties = gsProps;

        if (containerInfo->triggerInfoList) {
            memcpy(triggerInfoList, containerInfo->triggerInfoList, sizeof(GSTriggerInfo));
        }

        mContainerInfo.triggerInfoList = triggerInfoList;

        mContainerInfo.columnOrderIgnorable = containerInfo->columnOrderIgnorable;
        mContainerInfo.triggerInfoCount = containerInfo->triggerInfoCount;
        mColumnInfoList.columnInfo = NULL;
        mColumnInfoList.size = 0;
    }

    /**
     * @brief Constructor a new ContainerInfo::ContainerInfo object
     * @param *name The name of Container
     * @param *props Stores the information about the schema of a Column
     * @param propsCount Number of columns
     * @param type The type of Container
     * @param row_key The boolean value indicating whether the Row key Column is assigned
     * @param *expiration Stores the information about option of TimeSeries configuration
     */
    ContainerInfo::ContainerInfo(const GSChar* name, const GSColumnInfo* props, int propsCount,
            GSContainerType type, bool row_key, ExpirationInfo* expiration) {
        init(name, type, props, propsCount, row_key, expiration);
    }

    /**
     * Initialize values of Container Info object
     */
    void ContainerInfo::init(const GSChar* name,
            GSContainerType type, const GSColumnInfo* props,
            int propsCount, bool rowKeyAssigned, ExpirationInfo* expiration) {
        GSColumnInfo* columnInfoList = NULL;
        GSChar* containerName = NULL;
        GSTimeSeriesProperties* timeProps = NULL;

        try {
            if (propsCount > 0 && props != NULL) {
                columnInfoList = new GSColumnInfo[propsCount]();
                //Copy memory of GSColumnInfo list
                memcpy(columnInfoList, props, propsCount*sizeof(GSColumnInfo));
                //Copy memory of columns name
                for (int i = 0; i < propsCount; i++) {
                    if (props[i].name != NULL) {
                        Util::strdup(&(columnInfoList[i].name), props[i].name);
                    } else {
                        columnInfoList[i].name = NULL;
                    }
                }
            }

            if (expiration != NULL) {
                timeProps = new GSTimeSeriesProperties();
            }

            //Container name memory is copied via strdup function
            if (name != NULL) {
                Util::strdup((const GSChar**)&containerName, name);
            }
        } catch (bad_alloc& ba) {
            if (columnInfoList) {
                for (int i = 0; i < propsCount; i++) {
                    if (columnInfoList[i].name) {
                        delete[] columnInfoList[i].name;
                    }
                }
                delete[] columnInfoList;
            }
            if (containerName) {
                delete[] containerName;
            }
            if (timeProps) {
                delete timeProps;
            }
            throw GSException("Memory allocation error");
        }

        if (expiration != NULL) {
            memcpy(timeProps, expiration->gs_ts(), sizeof(GSTimeSeriesProperties));
        }

        mContainerInfo = {containerName, type, (size_t)propsCount, columnInfoList, rowKeyAssigned};
        if (timeProps != NULL) {
            mContainerInfo.timeSeriesProperties = timeProps;
        }
        mExpInfo = NULL;
        mColumnInfoList.columnInfo = NULL;
        mColumnInfoList.size = 0;
    }

    ContainerInfo::~ContainerInfo() {
        //Free memory for the copy of container name
        if (mContainerInfo.name) {
            delete[] mContainerInfo.name;
        }

        //Free memory for the copy of ColumnInfo list
        if (mContainerInfo.columnInfoList) {
            //Free memory of columns name
            for(int i = 0; i < mContainerInfo.columnCount; i++) {
                if(mContainerInfo.columnInfoList[i].name) {
                    delete[] mContainerInfo.columnInfoList[i].name;
                }
            }
            delete[] mContainerInfo.columnInfoList;
        }

        //Free memory of TimeSeriesProperties if existed
        if (mContainerInfo.timeSeriesProperties) {
            delete mContainerInfo.timeSeriesProperties;
        }

        //Free memory of dataAffinity if existed
        if (mContainerInfo.dataAffinity) {
            delete[] mContainerInfo.dataAffinity;
        }

        //Free memory of triggerInfoList if existed
        if(mContainerInfo.triggerInfoList) {
            delete mContainerInfo.triggerInfoList;
        }
        if (mExpInfo != NULL) {
            delete mExpInfo;
        }
    }

    /**
     * @brief Set name of Container which is stored in ContainerInfo
     * @param *containerName Stores the name of Container
     */
    void ContainerInfo::set_name(GSChar* containerName) {
        if (mContainerInfo.name) {
            delete[] mContainerInfo.name;
        }
        if (containerName == NULL) {
            mContainerInfo.name = NULL;
        } else {
            try {
                Util::strdup(&(mContainerInfo.name), containerName);
            } catch (bad_alloc& ba) {
                throw GSException("Memory allocation error");
            }
        }
    }

    /**
     * @brief Set type of Container which is stored in ContainerInfo
     * @param containerType The type of Container
     */
    void ContainerInfo::set_type(GSContainerType containerType) {
        mContainerInfo.type = containerType;
    }

    /**
     * @brief Set rowKeyAssigned for ContainerInfo
     * @param rowKeyAssigned The boolean value indicating whether the Row key Column is assigned
     */
    void ContainerInfo::set_row_key_assigned(bool rowKeyAssigned) {
        mContainerInfo.rowKeyAssigned = rowKeyAssigned;
    }

    /**
     * @brief Get name of Container which is stored in ContainerInfo
     * @return The name of Container
     */
    const GSChar* ContainerInfo::get_name() {
        return mContainerInfo.name;
    }

    /**
     * @brief Get type of Container which is stored in ContainerInfo
     * @return The type of Container
     */
    GSContainerType ContainerInfo::get_type() {
        return mContainerInfo.type;
    }

    /**
     * @brief Get columnInfo which is stored in ContainerInfo
     * @param column The number of column
     * @return The information of column which is stored in ContainerInfo
     */
    GSColumnInfo ContainerInfo::get_column_info(size_t column) {
        if (column >= mContainerInfo.columnCount) {
            throw GSException("Index out of bound error");
        }
        return mContainerInfo.columnInfoList[column];
    }

    /**
     * @brief Get rowKeyAssigned value of ContainerInfo
     * @return The boolean value indicating whether the Row key Column is assigned
     */
    bool ContainerInfo::get_row_key_assigned() {
        return mContainerInfo.rowKeyAssigned;
    }

    /**
     * @brief Get all information of Container
     * @return A pointer which store all information of Container
     */
    GSContainerInfo* ContainerInfo::gs_info() {
        return &mContainerInfo;
    }

    /**
     *  @brief Set information of column stored in ContainerInfo
     *  @param columnInfoList A struct which store information of column
     */
    void ContainerInfo::set_column_info_list(ColumnInfoList columnInfoList) {
        //Free current stored ColumnInfo list
        if (mContainerInfo.columnInfoList) {
            //Free memory of columns name
            for(int i = 0; i < mContainerInfo.columnCount; i++) {
                delete[] mContainerInfo.columnInfoList[i].name;
            }
            delete[] mContainerInfo.columnInfoList;
        }

        mContainerInfo.columnCount = columnInfoList.size;
        mContainerInfo.columnInfoList = NULL;

        if (columnInfoList.size == 0 || columnInfoList.columnInfo == NULL) {
            return;
        }
        
        GSColumnInfo* tmpColumnInfoList;
        try {
            tmpColumnInfoList = new GSColumnInfo[columnInfoList.size]();
        } catch (bad_alloc& ba) {
            throw GSException("Memory allocation error");
        }
        
        //Copy memory of GSColumnInfo list
        memcpy(tmpColumnInfoList, columnInfoList.columnInfo, columnInfoList.size*sizeof(GSColumnInfo));
        //Copy memory of columns name
        for (int i = 0; i < columnInfoList.size; i++) {
            if (columnInfoList.columnInfo[i].name) {
                try {
                    Util::strdup(&(tmpColumnInfoList[i].name), columnInfoList.columnInfo[i].name);
                } catch (bad_alloc& ba) {
                    delete[] tmpColumnInfoList;
                    tmpColumnInfoList = NULL;
                    throw GSException("Memory allocation error");
                }
            } else {
                tmpColumnInfoList[i].name = NULL;
            }
        }
        mContainerInfo.columnInfoList = tmpColumnInfoList;
    }

    /**
     *  @brief Get information of column stored in ContainerInfo
     *  @return A struct which store information of column
     */
    ColumnInfoList ContainerInfo::get_column_info_list() {
        mColumnInfoList.columnInfo = (GSColumnInfo*) mContainerInfo.columnInfoList;
        mColumnInfoList.size = mContainerInfo.columnCount;
        return mColumnInfoList;
    }

    /**
     *  @brief Set expirationInfo for timeseries container which is stored in ContainerInfo
     *  @param *expirationInfo A ExpirationInfo object which store the information about optional configuration settings used for newly creating or updating a TimeSeries
     */
    void ContainerInfo::set_expiration_info(ExpirationInfo* expirationInfo) {
        if (mContainerInfo.timeSeriesProperties != NULL) {
            delete mContainerInfo.timeSeriesProperties;
            mContainerInfo.timeSeriesProperties = NULL;
        }
        if (expirationInfo) {
            GSTimeSeriesProperties* ts;
            try {
                ts = new GSTimeSeriesProperties();
            } catch (bad_alloc& ba) {
                throw GSException("Memory allocation error");
            }

            memcpy(ts, expirationInfo->gs_ts(), sizeof(GSTimeSeriesProperties));

            mContainerInfo.timeSeriesProperties = ts;
        }
    }

    /**
     *  @brief Get expirationInfo for timeseries container which is stored in ContainerInfo
     *  @return A ExpirationInfo object which store the information about optional configuration settings used for newly creating or updating a TimeSeries
     */
    ExpirationInfo* ContainerInfo::get_expiration_info() {
        if (mContainerInfo.timeSeriesProperties != NULL){
            if (mExpInfo != NULL) {
                mExpInfo->set_time(mContainerInfo.timeSeriesProperties->rowExpirationTime);
                mExpInfo->set_time_unit(mContainerInfo.timeSeriesProperties->rowExpirationTimeUnit);
                mExpInfo->set_division_count(mContainerInfo.timeSeriesProperties->expirationDivisionCount);
            } else {
                try {
                    mExpInfo = new ExpirationInfo(mContainerInfo.timeSeriesProperties->rowExpirationTime,
                            mContainerInfo.timeSeriesProperties->rowExpirationTimeUnit,
                            mContainerInfo.timeSeriesProperties->expirationDivisionCount);
                } catch (bad_alloc& ba) {
                    throw GSException("Memory allocation error");
                }
            }
        } else {
            mExpInfo = NULL;
        }
        return mExpInfo;
    }

} /* namespace griddb */
