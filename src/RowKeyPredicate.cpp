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

#include "RowKeyPredicate.h"

namespace griddb {

    RowKeyPredicate::RowKeyPredicate(GSRowKeyPredicate *predicate, GSType type): mPredicate(predicate), mType(type),
            timestamp_output_with_float(false){
    }
    /**
     * Destructor. Call close methods to release resource
     */
    RowKeyPredicate::~RowKeyPredicate() {
        close();
    }
    /**
     * Convert from C-API: gsCloseRowKeyPredicate
     */
    void RowKeyPredicate::close() {
        if (mPredicate != NULL) {
            gsCloseRowKeyPredicate(&mPredicate);
            mPredicate = NULL;
        }
    }

    /**
     * Get key type. Convert from C-API: gsGetPredicateKeyType
     */
    GSType RowKeyPredicate::get_key_type() {
        return mType;
    }
    /*
     * Returns the value of Row key at the start and end position of the range condition
     */
    void RowKeyPredicate::get_range(Field* startField, Field* finishField) {
        startField->type = -1; // for all versions which do not support GS_TYPE_NULL
        finishField->type = -1; // for all versions which do not support GS_TYPE_NULL
        GSType key_type = get_key_type();
        const GSValue *startKey = NULL;
        const GSValue *endKey = NULL;
        GSResult ret = gsGetPredicateStartKeyGeneral(mPredicate, &startKey);
        if (ret != GS_RESULT_OK) {
            throw GSException(mPredicate, ret);
        }
        if (startKey != NULL) {
            startField->type = key_type;
            if (startField->type == GS_TYPE_STRING) {
                if (startKey->asString) {
                    startField->value.asString = strdup(startKey->asString);
                } else {
                    startField->value.asString = NULL;
                }
            } else {
                startField->value = *startKey;
            }
        }
        ret = gsGetPredicateFinishKeyGeneral(mPredicate, &endKey);
        if (ret != GS_RESULT_OK) {
            if (startField->type == GS_TYPE_STRING && startField->value.asString) {
                free((void*) startField->value.asString);
            }
            throw GSException(mPredicate, ret);
        }
        if (endKey != NULL) {
            finishField->type = key_type;
            if (finishField->type == GS_TYPE_STRING) {
                if (endKey->asString) {
                    finishField->value.asString = strdup(endKey->asString);
                } else {
                    finishField->value.asString = NULL;
                }
            } else {
                finishField->value = *endKey;
            }
        }
    }
    /*
     * Sets the value of Row key as the start and end position of the range conditions
     */
    void RowKeyPredicate::set_range(Field* startKey, Field* finishKey) {
        GSType key_type = get_key_type();
        GSResult ret;

        switch (key_type) {
        case GS_TYPE_LONG:
            ret = gsSetPredicateStartKeyByLong(mPredicate, (int64_t*)&startKey->value.asLong);
            if (ret != GS_RESULT_OK) {
                throw GSException(mPredicate, ret);
            }
            ret = gsSetPredicateFinishKeyByLong(mPredicate, (int64_t *) &finishKey->value.asLong);
            if (ret != GS_RESULT_OK) {
                throw GSException(mPredicate, ret);
            }
            break;
        case GS_TYPE_INTEGER:
            ret = gsSetPredicateStartKeyByInteger(mPredicate, (const int32_t *) &startKey->value.asInteger);
            if (ret != GS_RESULT_OK) {
                throw GSException(mPredicate, ret);
            }
            ret = gsSetPredicateFinishKeyByInteger(mPredicate, (const int32_t *)&finishKey->value.asInteger);
            if (ret != GS_RESULT_OK) {
                throw GSException(mPredicate, ret);
            }
            break;
        case GS_TYPE_STRING:
            ret = gsSetPredicateStartKeyByString(mPredicate, startKey->value.asString);
            if (ret != GS_RESULT_OK) {
                throw GSException(mPredicate, ret);
            }
            ret = gsSetPredicateFinishKeyByString(mPredicate, finishKey->value.asString);
            if (ret != GS_RESULT_OK) {
                throw GSException(mPredicate, ret);
            }
            break;
        case GS_TYPE_TIMESTAMP:
            ret = gsSetPredicateStartKeyByTimestamp(mPredicate,
                    (const GSTimestamp *) &(startKey->value.asTimestamp));
            if (ret != GS_RESULT_OK) {
                throw GSException(mPredicate, ret);
            }
            ret = gsSetPredicateFinishKeyByTimestamp(mPredicate,
                    (const GSTimestamp *) &(finishKey->value.asTimestamp));
            if (ret != GS_RESULT_OK) {
                throw GSException(mPredicate, ret);
            }
            break;
        default:
            throw GSException(mPredicate, "Not support type");
            break;
        }
    }
    /*
     * Adds the value of Row key as one of the elements in the individual condition
     */
    void RowKeyPredicate::set_distinct_keys(const Field *keys, size_t keyCount) {
        GSType key_type = get_key_type();
        GSResult ret;
        for (size_t i = 0; i < keyCount; i++) {
            const Field* key = keys + i;
            switch (key_type) {
            case GS_TYPE_LONG:
                ret = gsAddPredicateKeyByLong(mPredicate, key->value.asLong);
                if (ret != GS_RESULT_OK) {
                    throw GSException(mPredicate, ret);
                }
                break;
            case GS_TYPE_INTEGER:
                ret = gsAddPredicateKeyByInteger(mPredicate,
                        key->value.asInteger);
                if (ret != GS_RESULT_OK) {
                    throw GSException(mPredicate, ret);
                }
                break;
            case GS_TYPE_STRING:
                ret = gsAddPredicateKeyByString(mPredicate, key->value.asString);
                if (ret != GS_RESULT_OK) {
                    throw GSException(mPredicate, ret);
                }
                break;
            case GS_TYPE_TIMESTAMP:
                ret = gsAddPredicateKeyByTimestamp(mPredicate,
                        key->value.asTimestamp);
                if (ret != GS_RESULT_OK) {
                    throw GSException(mPredicate, ret);
                }
                break;
            default:
                throw GSException(mPredicate, "Not support type");
                break;
            }
        }
    }
    /*
     * Returns a set of the values of the Row keys that configure the individual condition.
     */
    void RowKeyPredicate::get_distinct_keys(Field **keys, size_t* keyCount) {
        size_t size;
        GSType key_type = get_key_type();
        GSValue * keyList;
        GSResult ret = gsGetPredicateDistinctKeysGeneral(mPredicate, (const GSValue **)&keyList, &size);
        *keyCount = size;

        Field* keyFields = new Field[size];
        for(int i = 0; i < size; i++) {
            keyFields[i].type = key_type;
            switch(key_type) {
            case GS_TYPE_STRING:
                if (keyList[i].asString) {
                    keyFields[i].value.asString = strdup(keyList[i].asString);
                } else {
                    keyFields[i].value.asString = NULL;
                }
                break;
            default:
                keyFields[i].value = keyList[i];
                break;
            }
        }

        *keys = keyFields;
        if (ret != GS_RESULT_OK) {
            throw GSException(mPredicate, ret);
        }
    }

    GSRowKeyPredicate* RowKeyPredicate::gs_ptr() {
        return mPredicate;
    }

} /* namespace griddb */
