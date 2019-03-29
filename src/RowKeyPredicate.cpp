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

    /**
     * @brief Constructor a new RowSet::RowSet object
     * @param *predicate Represents the condition that a row key satisfies.
     * @param type The type of Row key used as a matching condition
     */
    RowKeyPredicate::RowKeyPredicate(GSRowKeyPredicate *predicate, GSType type): mPredicate(predicate), mType(type),
            timestamp_output_with_float(false){
    }

    /**
     * @brief Destructor to free resource of RowKeyPredicate object
     */
    RowKeyPredicate::~RowKeyPredicate() {
        close();
    }

    /**
     * @brief Release RowKeyPredicate resource
     */
    void RowKeyPredicate::close() {
        if (mPredicate != NULL) {
            gsCloseRowKeyPredicate(&mPredicate);
            mPredicate = NULL;
        }
    }

    /**
     * @brief Get key type
     * @return The type of Row key used as a matching condition
     */
    GSType RowKeyPredicate::get_key_type() {
        return mType;
    }

    /**
     * @brief Get the value of Row key at the start and end position of the range condition
     * @param *startField The pointer to a variable to store the value of the Row key at the starting position
     * @param *finishField The pointer to a variable to store the value of the Row key at the end position
     */
    void RowKeyPredicate::get_range(Field* startField, Field* finishField) {
        assert(startField != NULL);
        assert(finishField != NULL);
        startField->type = -1; // for all versions which do not support GS_TYPE_NULL
        finishField->type = -1; // for all versions which do not support GS_TYPE_NULL
        GSType key_type = get_key_type();
        const GSValue *startKey = NULL;
        const GSValue *endKey = NULL;
        GSResult ret = gsGetPredicateStartKeyGeneral(mPredicate, &startKey);
        if (!GS_SUCCEEDED(ret)) {
            throw GSException(mPredicate, ret);
        }
        if (startKey != NULL) {
            startField->type = key_type;
            if (startField->type == GS_TYPE_STRING) {
                if (startKey->asString) {
                    try {
                        Util::strdup(&(startField->value.asString), startKey->asString);
                    } catch (bad_alloc& ba) {
                        throw GSException(mPredicate, "Memory allocation error");
                    }
                } else {
                    startField->value.asString = NULL;
                }
            } else {
                startField->value = *startKey;
            }
        }
        ret = gsGetPredicateFinishKeyGeneral(mPredicate, &endKey);
        if (!GS_SUCCEEDED(ret)) {
            if (startField->type == GS_TYPE_STRING && startField->value.asString) {
                delete[] startField->value.asString;
            }
            throw GSException(mPredicate, ret);
        }
        if (endKey != NULL) {
            finishField->type = key_type;
            if (finishField->type == GS_TYPE_STRING) {
                if (endKey->asString) {
                    try {
                        Util::strdup(&(finishField->value.asString), endKey->asString);
                    } catch (bad_alloc& ba) {
                        if (startField->type == GS_TYPE_STRING && startField->value.asString) {
                            delete[] startField->value.asString;
                        }
                        throw GSException(mPredicate, "Memory allocation error");
                    }
                } else {
                    finishField->value.asString = NULL;
                }
            } else {
                finishField->value = *endKey;
            }
        }
    }

    /**
     * @brief Sets the value of Row key as the start and end position of the range conditions
     * @param *startKey The pointer to a variable to store the value of the Row key at the starting position
     * @param *finishKey The pointer to a variable to store the value of the Row key at the end position
     */
    void RowKeyPredicate::set_range(Field* startKey, Field* finishKey) {
        assert(startKey != NULL);
        assert(finishKey != NULL);
        GSType key_type = get_key_type();
        GSResult ret;

        switch (key_type) {
        case GS_TYPE_LONG:
            ret = gsSetPredicateStartKeyByLong(mPredicate, (int64_t*)&startKey->value.asLong);
            if (!GS_SUCCEEDED(ret)) {
                throw GSException(mPredicate, ret);
            }
            ret = gsSetPredicateFinishKeyByLong(mPredicate, (int64_t *) &finishKey->value.asLong);
            if (!GS_SUCCEEDED(ret)) {
                throw GSException(mPredicate, ret);
            }
            break;
        case GS_TYPE_INTEGER:
            ret = gsSetPredicateStartKeyByInteger(mPredicate, (const int32_t *) &startKey->value.asInteger);
            if (!GS_SUCCEEDED(ret)) {
                throw GSException(mPredicate, ret);
            }
            ret = gsSetPredicateFinishKeyByInteger(mPredicate, (const int32_t *)&finishKey->value.asInteger);
            if (!GS_SUCCEEDED(ret)) {
                throw GSException(mPredicate, ret);
            }
            break;
        case GS_TYPE_STRING:
            ret = gsSetPredicateStartKeyByString(mPredicate, startKey->value.asString);
            if (!GS_SUCCEEDED(ret)) {
                throw GSException(mPredicate, ret);
            }
            ret = gsSetPredicateFinishKeyByString(mPredicate, finishKey->value.asString);
            if (!GS_SUCCEEDED(ret)) {
                throw GSException(mPredicate, ret);
            }
            break;
        case GS_TYPE_TIMESTAMP:
            ret = gsSetPredicateStartKeyByTimestamp(mPredicate,
                    (const GSTimestamp *) &(startKey->value.asTimestamp));
            if (!GS_SUCCEEDED(ret)) {
                throw GSException(mPredicate, ret);
            }
            ret = gsSetPredicateFinishKeyByTimestamp(mPredicate,
                    (const GSTimestamp *) &(finishKey->value.asTimestamp));
            if (!GS_SUCCEEDED(ret)) {
                throw GSException(mPredicate, ret);
            }
            break;
        default:
            throw GSException(mPredicate, "Not support type");
            break;
        }
    }

    /**
     * @brief Adds the value of Row key as one of the elements in the individual condition
     * @param *keys The value of Row key to be added as one of the elements in the individual condition
     * @param keyCount Number of distinct key
     */
    void RowKeyPredicate::set_distinct_keys(const Field *keys, size_t keyCount) {
        assert(keys != NULL);
        GSType key_type = get_key_type();
        GSResult ret;
        for (size_t i = 0; i < keyCount; i++) {
            const Field* key = keys + i;
            assert(key != NULL);
            switch (key_type) {
            case GS_TYPE_LONG:
                ret = gsAddPredicateKeyByLong(mPredicate, key->value.asLong);
                if (!GS_SUCCEEDED(ret)) {
                    throw GSException(mPredicate, ret);
                }
                break;
            case GS_TYPE_INTEGER:
                ret = gsAddPredicateKeyByInteger(mPredicate,
                        key->value.asInteger);
                if (!GS_SUCCEEDED(ret)) {
                    throw GSException(mPredicate, ret);
                }
                break;
            case GS_TYPE_STRING:
                ret = gsAddPredicateKeyByString(mPredicate, key->value.asString);
                if (!GS_SUCCEEDED(ret)) {
                    throw GSException(mPredicate, ret);
                }

                break;
            case GS_TYPE_TIMESTAMP:
                ret = gsAddPredicateKeyByTimestamp(mPredicate,
                        key->value.asTimestamp);
                if (!GS_SUCCEEDED(ret)) {
                    throw GSException(mPredicate, ret);
                }
                break;
            default:
                throw GSException(mPredicate, "Not support type");
                break;
            }
        }
    }

    /**
     * @brief Get a set of the values of the Row keys that configure the individual condition.
     * @param **keys A pointer refers to list of Row key value
     * @param *keyCount A pointer stores number of distinct key
     */
    void RowKeyPredicate::get_distinct_keys(Field **keys, size_t* keyCount) {
        assert(keys != NULL);
        assert(keyCount != NULL);

        size_t size;
        GSType key_type = get_key_type();
        GSValue * keyList;
        GSResult ret = gsGetPredicateDistinctKeysGeneral(mPredicate, (const GSValue **)&keyList, &size);
        if (!GS_SUCCEEDED(ret)) {
            throw GSException(mPredicate, ret);
        }
        *keyCount = size;

        Field* keyFields;
        try {
            keyFields = new Field[size](); //will be free in typemap out
            for (int i = 0; i < size; i++) {
                keyFields[i].type = key_type;
                switch(key_type) {
                case GS_TYPE_STRING:
                    if (keyList[i].asString) {
                        Util::strdup(&(keyFields[i].value.asString), keyList[i].asString);
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
        } catch (bad_alloc& ba) {
            if (keyFields) {
                for (int i = 0; i < size; i++) {
                    if (keyFields[i].type == GS_TYPE_STRING && keyFields[i].value.asString) {
                        delete[] keyFields[i].value.asString;
                    }
                }
                delete[] keyFields;
            }

            throw GSException(mPredicate, "Memory allocation error");
        }

    }

    /**
     * @brief Get GSRowKeyPredicate data in RowKeyPredicate object
     * @return A pointer stores GSRowKeyPredicate data in RowKeyPredicate object
     */
    GSRowKeyPredicate* RowKeyPredicate::gs_ptr() {
        return mPredicate;
    }

} /* namespace griddb */
