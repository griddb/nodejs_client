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

    ContainerInfo::ContainerInfo(GSContainerInfo *containerInfo) {
        init(containerInfo->name, containerInfo->type,
                containerInfo->columnInfoList, containerInfo->columnCount,
                containerInfo->rowKeyAssigned, NULL);
        //Assign values from argument to mContainer
        GSTimeSeriesProperties* gsProps = NULL;
        if (containerInfo->timeSeriesProperties) {
            gsProps = (GSTimeSeriesProperties*) malloc(sizeof(GSTimeSeriesProperties));
            memcpy(gsProps, containerInfo->timeSeriesProperties, sizeof(GSTimeSeriesProperties));
        }
        mContainerInfo.timeSeriesProperties = gsProps;
        GSTriggerInfo* triggerInfoList = NULL;
        if (containerInfo->triggerInfoList) {
            triggerInfoList = (GSTriggerInfo*) malloc(sizeof(GSTriggerInfo));
            memcpy(triggerInfoList, containerInfo->triggerInfoList, sizeof(GSTriggerInfo));
        }
        mContainerInfo.triggerInfoList = triggerInfoList;
#if GS_COMPATIBILITY_SUPPORT_2_1
        if (containerInfo->dataAffinity) {
            mContainerInfo.dataAffinity = strdup(containerInfo->dataAffinity);
        }
#endif
        mContainerInfo.columnOrderIgnorable = containerInfo->columnOrderIgnorable;
        mContainerInfo.triggerInfoCount = containerInfo->triggerInfoCount;
        mColumnInfoList.columnInfo = NULL;
        mColumnInfoList.size = 0;
    }

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

        if (propsCount > 0 && props != NULL) {
            columnInfoList = (GSColumnInfo *) malloc(propsCount*sizeof(GSColumnInfo));
            //Copy memory of GSColumnInfo list
            memcpy(columnInfoList, props, propsCount*sizeof(GSColumnInfo));
            //Copy memory of columns name
            for (int i = 0; i < propsCount; i++) {
                if (props[i].name != NULL) {
                    columnInfoList[i].name = strdup(props[i].name);
                } else {
                    columnInfoList[i].name = NULL;
                }
            }
        }

        if (expiration != NULL) {
            timeProps = (GSTimeSeriesProperties*) malloc(sizeof(GSTimeSeriesProperties));
            memcpy(timeProps, expiration->gs_ts(), sizeof(GSTimeSeriesProperties));
        }

        //Container name memory is copied via strdup function
        if (name != NULL) {
            containerName = strdup(name);
        }

        mContainerInfo = {containerName, type, propsCount, columnInfoList, rowKeyAssigned};
        if (timeProps != NULL) {
            mContainerInfo.timeSeriesProperties = timeProps;
        }
        mExpInfo = NULL;
    }

    ContainerInfo::~ContainerInfo() {
        //Free memory for the copy of container name
        if (mContainerInfo.name) {
            free((void*) mContainerInfo.name);
        }

        //Free memory for the copy of ColumnInfo list
        if (mContainerInfo.columnInfoList) {
            //Free memory of columns name
            for(int i = 0; i < mContainerInfo.columnCount; i++) {
                if(mContainerInfo.columnInfoList[i].name) {
                    free((void *) mContainerInfo.columnInfoList[i].name);
                }
            }
            free((void *) mContainerInfo.columnInfoList);
        }

        //Free memory of TimeSeriesProperties if existed
        if (mContainerInfo.timeSeriesProperties) {
            free((void *) mContainerInfo.timeSeriesProperties);
        }

        //Free memory of dataAffinity if existed
#if GS_COMPATIBILITY_SUPPORT_2_1
        if (mContainerInfo.dataAffinity) {
            free((void *) mContainerInfo.dataAffinity);
        }
#endif
        //Free memory of triggerInfoList if existed
        if(mContainerInfo.triggerInfoList) {
            free((void *) mContainerInfo.triggerInfoList);
        }
        if (mExpInfo != NULL) {
            delete mExpInfo;
        }
    }

    void ContainerInfo::set_name(GSChar* containerName) {
        if (mContainerInfo.name) {
            free((void*) mContainerInfo.name);
        }
        if (containerName == NULL) {
            mContainerInfo.name = NULL;
        } else {
            mContainerInfo.name = strdup(containerName);
        }
    }

    void ContainerInfo::set_type(GSContainerType containerType) {
        mContainerInfo.type = containerType;
    }

    void ContainerInfo::set_row_key_assigned(bool rowKeyAssigned) {
        mContainerInfo.rowKeyAssigned = rowKeyAssigned;
    }

    const GSChar* ContainerInfo::get_name() {
        return mContainerInfo.name;
    }

    GSContainerType ContainerInfo::get_type() {
        return mContainerInfo.type;
    }

    GSColumnInfo ContainerInfo::get_column_info(size_t column) {
        return mContainerInfo.columnInfoList[column];
    }

    bool ContainerInfo::get_row_key_assigned() {
        return mContainerInfo.rowKeyAssigned;
    }

    /**
     * Return GSContainerInfo variable
     */
    GSContainerInfo* ContainerInfo::gs_info() {
        return &mContainerInfo;
    }

    /**
     *  Set attribute :column_info_list
     */
    void ContainerInfo::set_column_info_list(ColumnInfoList columnInfoList) {
        //Free current stored ColumnInfo list
        if (mContainerInfo.columnInfoList) {
            //Free memory of columns name
            for(int i = 0; i < mContainerInfo.columnCount; i++) {
                free((void *) mContainerInfo.columnInfoList[i].name);
            }
            free((void*) mContainerInfo.columnInfoList);
        }

        //Copy memory of new ColumnInfo list
        GSColumnInfo* tmpColumnInfoList = NULL;
        if (columnInfoList.size > 0 && columnInfoList.columnInfo != NULL) {
            tmpColumnInfoList = (GSColumnInfo *) malloc(columnInfoList.size*sizeof(GSColumnInfo));
            //Copy memory of GSColumnInfo list
            memcpy(tmpColumnInfoList, columnInfoList.columnInfo, columnInfoList.size*sizeof(GSColumnInfo));
            //Copy memory of columns name
            for (int i = 0; i < columnInfoList.size; i++) {
                tmpColumnInfoList[i].name = strdup(columnInfoList.columnInfo[i].name);
            }
        }
        mContainerInfo.columnInfoList = tmpColumnInfoList;
        mContainerInfo.columnCount = columnInfoList.size;
    }

    /**
     *  Get attribute :column_info_list
     */
    ColumnInfoList ContainerInfo::get_column_info_list() {
        mColumnInfoList.columnInfo = (GSColumnInfo*) mContainerInfo.columnInfoList;
        mColumnInfoList.size = mContainerInfo.columnCount;
        return mColumnInfoList;
    }

    /*
     * Set attribute: expiration
     */
    void ContainerInfo::set_expiration_info(ExpirationInfo expirationInfo) {
#if GS_COMPATIBILITY_SUPPORT_1_5
        if (mContainerInfo.timeSeriesProperties != NULL) {
            free((void*) mContainerInfo.timeSeriesProperties);
        }
        GSTimeSeriesProperties* ts = (GSTimeSeriesProperties*) malloc(sizeof(GSTimeSeriesProperties));

        memcpy(ts, expirationInfo.gs_ts(), sizeof(GSTimeSeriesProperties));

        mContainerInfo.timeSeriesProperties = ts;
#endif
    }

    /*
     * Get attribute: expiration
     */
    ExpirationInfo& ContainerInfo::get_expiration_info() {
        if (mContainerInfo.timeSeriesProperties != NULL){
            if (mExpInfo != NULL) {
                mExpInfo->set_time(mContainerInfo.timeSeriesProperties->rowExpirationTime);
                mExpInfo->set_time_unit(mContainerInfo.timeSeriesProperties->rowExpirationTimeUnit);
                mExpInfo->set_division_count(mContainerInfo.timeSeriesProperties->expirationDivisionCount);
            } else {
                mExpInfo = new ExpirationInfo(mContainerInfo.timeSeriesProperties->rowExpirationTime,
                mContainerInfo.timeSeriesProperties->rowExpirationTimeUnit,
                mContainerInfo.timeSeriesProperties->expirationDivisionCount);
            }
        }
        return *mExpInfo;
    }

} /* namespace griddb */
