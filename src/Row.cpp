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

#include "Row.h"
#include "GSException.h"
#include "ContainerInfo.h"

namespace griddb {

    Row::Row(int size, GSRow* gsRow) : mFields(NULL), mCount(size), mRow(gsRow) {
        if (size > 0) {
            resize(size);
        }
        else mCount = 0;
    }
    Row::~Row() {
        del_array_field();
        //gsCloseRow(&mRow);
    }
    /**
    * Set for field from GSRow.
    */
    void Row::del_array_field() {
        if (mFields) {
            delete [] mFields;
            mFields = NULL;
        }
    }
    /**
     * Set for field from GSRow.
     */
    void Row::set_from_row(GSRow* row) {
        // Resize fields as columnCount
        GSContainerInfo containerInfo;
        GSResult ret = gsGetRowSchema(row, &containerInfo);
        if (ret != GS_RESULT_OK) {
            throw GSException(ret, "can not get row schema to get");
        }
        if (containerInfo.columnCount != mCount) {
            resize(containerInfo.columnCount);
        }

        // Read value from row to fields
        for (int i = 0; i < mCount; i++) {
            set_for_field(row, i);
        }
    }
    /**
     * Set for GSRow from Field
     */
    void Row::set_for_row(GSRow* row, GSContainerInfo* containerInfo) {
        if (containerInfo) {
            if (containerInfo->columnCount != mCount) {
                throw GSException("column number is not match");
            }
            for (int i = 0; i < mCount; i++) {
                get_from_field(row, i, containerInfo->columnInfoList[i].type);
            }
        } else {
            GSContainerInfo containerInfo;
            GSResult ret = gsGetRowSchema(row, &containerInfo);
            if (ret != GS_RESULT_OK) {
                throw GSException(ret, "can not get row schema to set");
            }
            if (containerInfo.columnCount != mCount) {
                throw GSException("column number is not match");
            }
            for (int i = 0; i < mCount; i++) {
                get_from_field(row, i, containerInfo.columnInfoList[i].type);
            }
        }
    }
    void Row::resize(int size) {
        if (mFields) {
            del_array_field();
        }
        mFields = new Field[size];
        //memset(mFields, 0x0, sizeof(Field) * size);
        mCount = size;
    }
    Field* Row::get_field_ptr() {
        return mFields;
    }

    int Row::get_count() {
        return mCount;
    }

    template<typename T>
    const T* cloneArray(const void* source, size_t size) {
        T* newT = (T*)malloc(size * sizeof(T));
        memcpy(newT, source, size * sizeof(T));
        return newT;
    }

    const GSChar* const* cloneArrayString(const GSChar* const* source, size_t size) {
        GSChar** newArray = (GSChar**)malloc(size * sizeof(GSChar*));
        memset(newArray, 0x0, size * sizeof(GSChar*));
        for (int i = 0; i < size; i++) {
            if (source[i]) {
                newArray[i] = strdup(source[i]);
            }
        }
        return newArray;
    }

    void Row::set_for_field(GSRow* row, int no) {
        Field* field = &mFields[no];
        GSBool nullValue = GS_TRUE;
        GSResult ret;

        ret = gsGetRowFieldGeneral(row, no, &field->value, &field->type);
        if (ret != GS_RESULT_OK) {
            throw GSException(ret, "can not set for field");
        }
        GSChar *tmp;

        switch (field->type) {
        case GS_TYPE_STRING:
            tmp = strdup(field->value.asString);
            field->value.asString = tmp;
            break;
        case GS_TYPE_BLOB:
            field->value.asBlob.data = (void*)cloneArray<char>(field->value.asBlob.data, field->value.asBlob.size);
            break;
        case GS_TYPE_INTEGER_ARRAY:
#if GS_COMPATIBILITY_VALUE_1_1_106
            field->value.asIntegerArray.elements = cloneArray<int32_t>(field->value.asIntegerArray.elements, field.value.asIntegerArray.size);
#else
            field->value.asArray.elements.asInteger = cloneArray<int32_t>(field->value.asArray.elements.asInteger, field->value.asArray.length);
#endif
            break;
        case GS_TYPE_STRING_ARRAY:
#if GS_COMPATIBILITY_VALUE_1_1_106
            field->value.asStringArray.elements = cloneArrayString(field->value.asStringArray.elements, field->value.asStringArray.size);
#else
            field->value.asArray.elements.asString = cloneArrayString(field->value.asArray.elements.asString, field->value.asArray.length);
#endif
            break;
        case GS_TYPE_BOOL_ARRAY:
#if GS_COMPATIBILITY_VALUE_1_1_106
            field->value.asBoolArray.elements = cloneArray<GSBool>(field->value.asBoolArray.elements, field.value.asIntegerArray.size);
#else
            field->value.asArray.elements.asBool = cloneArray<GSBool>(field->value.asArray.elements.asBool, field->value.asArray.length);
#endif
            break;
        case GS_TYPE_BYTE_ARRAY:
#if GS_COMPATIBILITY_VALUE_1_1_106
            field->value.asByteArray.elements = cloneArray<int8_t>(field->value.asByteArray.elements, field->value.asByteArray.size);
#else
            field->value.asArray.elements.asByte = cloneArray<int8_t>(field->value.asArray.elements.asByte, field->value.asArray.length);
#endif
            break;
        case GS_TYPE_SHORT_ARRAY:
#if GS_COMPATIBILITY_VALUE_1_1_106
            field->value.asShortArray.elements = cloneArray<int16_t>(field->value.asShortArray.elements, field->value.asShortArray.size);
#else
            field->value.asArray.elements.asShort = cloneArray<int16_t>(field->value.asArray.elements.asShort, field->value.asArray.length);
#endif
            break;
        case GS_TYPE_LONG_ARRAY:
#if GS_COMPATIBILITY_VALUE_1_1_106
            field->value.asLongArray.elements = cloneArray<int64_t>(field->value.asLongArray.elements, field->value.asLongArray.size);
#else
            field->value.asArray.elements.asLong = cloneArray<int64_t>(field->value.asArray.elements.asLong, field->value.asArray.length);
#endif
            break;
        case GS_TYPE_FLOAT_ARRAY:
#if GS_COMPATIBILITY_VALUE_1_1_106
            field->value.asFloatArray.elements = cloneArray<float>(field->value.asFloatArray.elements, field->value.asFloatArray.size);
#else
            field->value.asArray.elements.asFloat = cloneArray<float>(field->value.asArray.elements.asFloat, field->value.asArray.length);
#endif
            break;
        case GS_TYPE_DOUBLE_ARRAY:
#if GS_COMPATIBILITY_VALUE_1_1_106
            field->value.asDoubleArray.elements = cloneArray<double>(field->value.asDoubleArray.elements, field->value.asDoubleArray.size);
#else
            field->value.asArray.elements.asDouble = cloneArray<double>(field->value.asArray.elements.asDouble, field->value.asArray.length);
#endif
            break;
        case GS_TYPE_TIMESTAMP_ARRAY:
#if GS_COMPATIBILITY_VALUE_1_1_106
            field->value.asTimestampArray.elements = cloneArray<GSTimestamp>(field->value.asTimestampArray.elements, field->value.asTimestampArray.size);
#else
            field->value.asArray.elements.asTimestamp = cloneArray<GSTimestamp>(field->value.asArray.elements.asTimestamp, field->value.asArray.length);
#endif
            break;
        }
    }

    void Row::get_from_field(GSRow* row, int no, GSType type) {
        GSResult ret;
        Field* field = &mFields[no];
        GSValue fieldValue;

#if GS_COMPATIBILITY_SUPPORT_3_5
        if (type >= GS_TYPE_NULL &&
                type <= GS_TYPE_TIMESTAMP_ARRAY &&
                field->type == GS_TYPE_NULL) {
            ret = gsSetRowFieldNull(row, no);
            if (ret != GS_RESULT_OK) {
                throw GSException("Can not set value for row field");
            }
            return;
        }
#endif

        switch (type) {
        case GS_TYPE_BLOB:
            if(field->type == GS_TYPE_BLOB) {
                ret = gsSetRowFieldGeneral(row, no, &field->value, GS_TYPE_BLOB);
            } else if(field->type == GS_TYPE_STRING) {
                fieldValue.asBlob.size = strlen(field->value.asString);
                fieldValue.asBlob.data = field->value.asString;
                ret = gsSetRowFieldGeneral(row, no, &fieldValue, GS_TYPE_BLOB);
            } else {
                throw GSException("incorrect column type to set for blob");
            }
            break;
        case GS_TYPE_BOOL:
            if (field->type == GS_TYPE_BOOL) {
                ret = gsSetRowFieldGeneral(row, no, &field->value, GS_TYPE_BOOL);
            } else if(field->type == GS_TYPE_LONG) {
                field->value.asBool = (field->value.asLong) ? true : false;
                ret = gsSetRowFieldGeneral(row, no, &field->value, GS_TYPE_BOOL);
            } else if (field->type == GS_TYPE_BYTE) {
                field->value.asBool = (field->value.asByte) ? true : false;
                ret = gsSetRowFieldGeneral(row, no, &field->value, GS_TYPE_BOOL);
            } else if (field->type == GS_TYPE_SHORT) {
                field->value.asBool = (field->value.asShort) ? true : false;
                ret = gsSetRowFieldGeneral(row, no, &field->value, GS_TYPE_BOOL);
            } else if(field->type == GS_TYPE_INTEGER) {
                field->value.asBool = (field->value.asInteger) ? true : false;
                ret = gsSetRowFieldGeneral(row, no, &field->value, GS_TYPE_BOOL);
            } else {
            	throw GSException("incorrect column type to set for bool");
            }
            break;
        case GS_TYPE_INTEGER:
            if (field->type == GS_TYPE_BYTE) {
                field->value.asInteger = field->value.asByte;
                ret = gsSetRowFieldGeneral(row, no, &field->value, GS_TYPE_INTEGER);
            } else if (field->type == GS_TYPE_SHORT) {
                field->value.asInteger = field->value.asShort;
                ret = gsSetRowFieldGeneral(row, no, &field->value, GS_TYPE_INTEGER);
            } else if(field->type == GS_TYPE_INTEGER) {
                ret = gsSetRowFieldGeneral(row, no, &field->value, GS_TYPE_INTEGER);
            } else {
                throw GSException("incorrect column type to set for integer");
            }
            break;
        case GS_TYPE_LONG:
            if(field->type == GS_TYPE_LONG) {
                ret = gsSetRowFieldGeneral(row, no, &field->value, GS_TYPE_LONG);
            } else if (field->type == GS_TYPE_BYTE) {
                field->value.asLong = field->value.asByte;
                ret = gsSetRowFieldGeneral(row, no, &field->value, GS_TYPE_LONG);
            } else if (field->type == GS_TYPE_SHORT) {
                field->value.asLong = field->value.asShort;
                ret = gsSetRowFieldGeneral(row, no, &field->value, GS_TYPE_LONG);
            } else if(field->type == GS_TYPE_INTEGER) {
                field->value.asLong = field->value.asInteger;
                ret = gsSetRowFieldGeneral(row, no, &field->value, GS_TYPE_LONG);
            } else {
                throw GSException("incorrect column type to set for long");
            }
            break;
        case GS_TYPE_FLOAT:
            if(field->type == GS_TYPE_FLOAT) {
                ret = gsSetRowFieldGeneral(row, no, &field->value, GS_TYPE_FLOAT);
            } else if (field->type == GS_TYPE_BYTE) {
                field->value.asFloat = field->value.asByte;
                ret = gsSetRowFieldGeneral(row, no, &field->value, GS_TYPE_FLOAT);
            } else if (field->type == GS_TYPE_SHORT) {
                field->value.asFloat = field->value.asShort;
                ret = gsSetRowFieldGeneral(row, no, &field->value, GS_TYPE_FLOAT);
            } else if(field->type == GS_TYPE_INTEGER) {
                field->value.asFloat = field->value.asInteger;
                ret = gsSetRowFieldGeneral(row, no, &field->value, GS_TYPE_FLOAT);
            } else {
                throw GSException("incorrect column type to set for float");
            }
            break;
        case GS_TYPE_DOUBLE:
            if(field->type == GS_TYPE_DOUBLE) {
                ret = gsSetRowFieldGeneral(row, no, &field->value, GS_TYPE_DOUBLE);
            } else if(field->type == GS_TYPE_FLOAT) {
                field->value.asDouble = field->value.asFloat;
                ret = gsSetRowFieldGeneral(row, no, &field->value, GS_TYPE_DOUBLE);
            } else if (field->type == GS_TYPE_BYTE) {
                field->value.asDouble = field->value.asByte;
                ret = gsSetRowFieldGeneral(row, no, &field->value, GS_TYPE_DOUBLE);
            } else if (field->type == GS_TYPE_SHORT) {
                field->value.asDouble = field->value.asShort;
                ret = gsSetRowFieldGeneral(row, no, &field->value, GS_TYPE_DOUBLE);
            } else if(field->type == GS_TYPE_INTEGER) {
                field->value.asDouble = field->value.asInteger;
                ret = gsSetRowFieldGeneral(row, no, &field->value, GS_TYPE_DOUBLE);
            } else {
                throw GSException("incorrect column type to set for double");
            }
            break;
        case GS_TYPE_STRING:
            if (field->type == GS_TYPE_STRING) {
                ret = gsSetRowFieldGeneral(row, no, &field->value, GS_TYPE_STRING);
            } else {
                throw GSException("incorrect column type to set for string");
            }
            break;
        case GS_TYPE_TIMESTAMP:
            if (field->type == GS_TYPE_TIMESTAMP) {
                ret = gsSetRowFieldGeneral(row, no, &field->value, GS_TYPE_TIMESTAMP);
            } else {
                throw GSException("incorrect column type to set for timestamp");
            }
            break;
        case GS_TYPE_SHORT:
            if(field->type == GS_TYPE_SHORT) {
                ret = gsSetRowFieldGeneral(row, no, &field->value, GS_TYPE_SHORT);
            } else if(field->type == GS_TYPE_BYTE) {
                field->value.asShort = field->value.asByte;
                ret = gsSetRowFieldGeneral(row, no, &field->value, GS_TYPE_SHORT);
            } else {
                throw GSException("incorrect column type to set for short");
            }
            break;
        case GS_TYPE_BYTE:
            if(field->type == GS_TYPE_BYTE) {
                ret = gsSetRowFieldGeneral(row, no, &field->value, GS_TYPE_BYTE);
            } else {
                throw GSException("incorrect column type to set for byte");
            }
            break;
        case GS_TYPE_INTEGER_ARRAY:
        case GS_TYPE_STRING_ARRAY:
        case GS_TYPE_BOOL_ARRAY:
        case GS_TYPE_BYTE_ARRAY:
        case GS_TYPE_SHORT_ARRAY:
        case GS_TYPE_LONG_ARRAY:
        case GS_TYPE_FLOAT_ARRAY:
        case GS_TYPE_DOUBLE_ARRAY:
        case GS_TYPE_TIMESTAMP_ARRAY:
            ret = gsSetRowFieldGeneral(row, no, &(field->value), type);
            break;
        default:
            throw GSException("No type to support for getting field");
            break;
        }

        if (ret != GS_RESULT_OK) {
            throw GSException("Can not set value for row field");
        }
    }

}
