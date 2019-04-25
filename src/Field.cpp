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

#include "Field.h"

namespace griddb {

    /**
     * @brief Constructor a new Field:: Field object
     */
    Field::Field() : type(GS_TYPE_STRING) {
        memset(&value, 0, sizeof(GSValue));
    };

    Field::~Field() {
        switch (type) {
        case GS_TYPE_STRING:
            if (value.asString) {
                delete [] value.asString;
                value.asString = NULL;
            }
            break;
        case GS_TYPE_BLOB:
            if (value.asBlob.data) {
                delete [] value.asBlob.data;
                value.asBlob.data = NULL;
            }
            break;
        case GS_TYPE_INTEGER_ARRAY:
            if (value.asArray.elements.asInteger) {
                delete [] value.asArray.elements.asInteger;
                value.asArray.elements.asInteger = NULL;
            }
            break;
        case GS_TYPE_STRING_ARRAY:
            if (value.asArray.elements.asString) {
                for (int j = 0; j < value.asArray.length; j++) {
                    if (value.asArray.elements.asString[j]) {
                        delete [] value.asArray.elements.asString[j];
                    }
                }
                delete [] value.asArray.elements.asString;
                value.asArray.elements.asString = NULL;
            }
            break;
        case GS_TYPE_BOOL_ARRAY:
            if (value.asArray.elements.asBool) {
                delete [] value.asArray.elements.asBool;
                value.asArray.elements.asBool = NULL;
            }
            break;
        case GS_TYPE_BYTE_ARRAY:
            if (value.asArray.elements.asByte) {
                delete [] value.asArray.elements.asByte;
                value.asArray.elements.asByte = NULL;
            }
            break;
        case GS_TYPE_SHORT_ARRAY:
            if (value.asArray.elements.asShort) {
                delete [] value.asArray.elements.asShort;
                value.asArray.elements.asShort = NULL;
            }
            break;
        case GS_TYPE_LONG_ARRAY:
            if (value.asArray.elements.asLong) {
                delete [] value.asArray.elements.asLong;
                value.asArray.elements.asLong = NULL;
            }
            break;
        case GS_TYPE_FLOAT_ARRAY:
            if (value.asArray.elements.asFloat) {
                delete [] value.asArray.elements.asFloat;
                value.asArray.elements.asFloat = NULL;
            }
            break;
        case GS_TYPE_DOUBLE_ARRAY:
            if (value.asArray.elements.asDouble) {
                delete [] value.asArray.elements.asDouble;
                value.asArray.elements.asDouble = NULL;
            }
            break;
        case GS_TYPE_TIMESTAMP_ARRAY:
            if (value.asArray.elements.asTimestamp) {
                delete [] value.asArray.elements.asTimestamp;
                value.asArray.elements.asTimestamp = NULL;
            }
            break;
        default:
            //not need to free allocation
            break;
        }
    }
}
