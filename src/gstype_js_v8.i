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

#define UTC_TIMESTAMP_MAX 253402300799.999 // Max timestamp in seconds
%{
#include <ctime>
#include <limits>
#include <node_buffer.h>
#include <nan.h>
%}

// rename all method to camel cases
%rename("%(lowercamelcase)s", %$isfunction) "";
//Correct attribute name to camel case
%rename(timestampOutput) *::timestamp_output_with_float;
/*
 * ignore unnecessary functions
 */
%ignore griddb::Container::setOutputTimestamp;
%ignore griddb::RowSet::next_row;
%ignore griddb::RowSet::get_next_query_analysis;
%ignore griddb::RowSet::get_next_aggregation;
%ignore griddb::ContainerInfo::ContainerInfo(GSContainerInfo* containerInfo);
%ignore griddb::AggregationResult::setOutputTimestamp;
%ignore griddb::RowKeyPredicate::setOutputTimestamp;
%ignore griddb::RowSet::setOutputTimestamp;
%ignore griddb::Store::setOutputTimestamp;
%ignore ColumnInfoList;

/*
 * Use attribute in Nodejs
 */
%include <attribute.i>

//Read only attribute Container::type
%attribute(griddb::Container, int, type, get_type);
//Read only attribute GSException::is_timeout 
%attribute(griddb::GSException, bool, isTimeout, is_timeout);
//Read only attribute PartitionController::partition_count 
%attribute(griddb::PartitionController, int, partitionCount, get_partition_count);
//Read only attribute RowKeyPredicate::partition_count 
%attribute(griddb::RowKeyPredicate, GSType, keyType, get_key_type);
//Read only attribute RowSet::size 
%attribute(griddb::RowSet, int32_t, size, size);
//Read only attribute RowSet::type 
%attribute(griddb::RowSet, GSRowSetType, type, type);
//Read only attribute Store::partition_info 
%attribute(griddb::Store, griddb::PartitionController*, partitionInfo, partition_info);
//Read only attribute ContainerInfo::name 
%attribute(griddb::ContainerInfo, GSChar*, name, get_name, set_name);
//Read only attribute ContainerInfo::type 
%attribute(griddb::ContainerInfo, GSContainerType, type, get_type, set_type);
//Read only attribute ContainerInfo::rowKey 
%attribute(griddb::ContainerInfo, bool, rowKey, get_row_key_assigned, set_row_key_assigned);
//Read only attribute ContainerInfo::columnInfoList 
%attributeval(griddb::ContainerInfo, ColumnInfoList, columnInfoList, get_column_info_list, set_column_info_list);
//Read only attribute ContainerInfo::expiration 
%attribute(griddb::ContainerInfo, griddb::ExpirationInfo*, expiration, get_expiration_info, set_expiration_info);
//Read only attribute ExpirationInfo::time 
%attribute(griddb::ExpirationInfo, int, time, get_time, set_time);
//Read only attribute ExpirationInfo::unit 
%attribute(griddb::ExpirationInfo, GSTimeUnit, unit, get_time_unit, set_time_unit);
//Read only attribute ExpirationInfo::divisionCount 
%attribute(griddb::ExpirationInfo, int, divisionCount, get_division_count, set_division_count);

/*
 * Typemaps for catch GSException
 */
%typemap(throws) griddb::GSException %{
    SWIGV8_THROW_EXCEPTION(SWIG_V8_NewPointerObj(SWIG_as_voidptr(new (nothrow) griddb::GSException(&$1)), $descriptor(griddb::GSException *), SWIG_POINTER_OWN |  0));
%}

%fragment("convertFieldToObject", "header", fragment = "convertTimestampToObject") {
static v8::Handle<v8::Value> convertFieldToObject(GSValue* value, GSType type, bool timestampToFloat = true) {

    size_t size;
    v8::Local<v8::Array> list;
    int i;
    v8::Local<v8::Context> context = Nan::GetCurrentContext();
    switch (type) {
        case GS_TYPE_LONG:
            return SWIGV8_NUMBER_NEW(value->asLong);
        case GS_TYPE_STRING:
            if (value->asString == NULL) {
                return SWIGV8_NULL();
            }
            return SWIGV8_STRING_NEW(value->asString);
        case GS_TYPE_NULL:
            return SWIGV8_NULL();
        case GS_TYPE_BLOB:
            return Nan::CopyBuffer((char *)value->asBlob.data, value->asBlob.size).ToLocalChecked();
        case GS_TYPE_BOOL:
            return SWIGV8_BOOLEAN_NEW((bool)value->asBool);
        case GS_TYPE_INTEGER:
            return SWIGV8_INT32_NEW(value->asInteger);
        case GS_TYPE_FLOAT:
            return SWIGV8_NUMBER_NEW(value->asFloat);
        case GS_TYPE_DOUBLE:
            return SWIGV8_NUMBER_NEW(value->asDouble);
        case GS_TYPE_TIMESTAMP:
            return convertTimestampToObject(&value->asTimestamp, timestampToFloat);
        case GS_TYPE_BYTE:
            return SWIGV8_INT32_NEW(value->asByte);
        case GS_TYPE_SHORT:
            return SWIGV8_INT32_NEW(value->asShort);
        case GS_TYPE_GEOMETRY:
            if (value->asGeometry == NULL) {
                return SWIGV8_NULL();
            }
            return SWIGV8_STRING_NEW(value->asGeometry);
        case GS_TYPE_INTEGER_ARRAY: {
            const int32_t *intArrVal;
            size = value->asArray.length;
            intArrVal = value->asArray.elements.asInteger;
            list = SWIGV8_ARRAY_NEW();
            for (i = 0; i < size; i++) {
                list->Set(context, i, SWIG_From_int(intArrVal[i]));
            }
            return list;
        }
        case GS_TYPE_STRING_ARRAY: {
            const GSChar *const *stringArrVal;
            size = value->asArray.length;
            stringArrVal = value->asArray.elements.asString;
            list = SWIGV8_ARRAY_NEW();
            for (i = 0; i < size; i++) {
                list->Set(context, i, SWIGV8_STRING_NEW((GSChar *)stringArrVal[i]));
            }
            return list;
        }
        case GS_TYPE_BOOL_ARRAY: {
            const GSBool *boolArrVal;
            size = value->asArray.length;
            boolArrVal = value->asArray.elements.asBool;
            list = SWIGV8_ARRAY_NEW();
            for (i = 0; i < size; i++) {
                list->Set(context, i, SWIG_From_bool(boolArrVal[i]));
            }
            return list;
        }
        case GS_TYPE_BYTE_ARRAY: {
            const int8_t *byteArrVal;
            size = value->asArray.length;
            byteArrVal = value->asArray.elements.asByte;
            list = SWIGV8_ARRAY_NEW();
            for (i = 0; i < size; i++) {
                list->Set(context, i, SWIG_From_int(byteArrVal[i]));
            }
            return list;
        }
        case GS_TYPE_SHORT_ARRAY: {
            const int16_t *shortArrVal;
            size = value->asArray.length;
            shortArrVal = value->asArray.elements.asShort;
            list = SWIGV8_ARRAY_NEW();
            for (i = 0; i < size; i++) {
                list->Set(context, i, SWIG_From_int(shortArrVal[i]));
            }
            return list;
        }
        case GS_TYPE_LONG_ARRAY: {
            const int64_t *longArrVal;
            size = value->asArray.length;
            longArrVal = value->asArray.elements.asLong;
            list = SWIGV8_ARRAY_NEW();
            for (i = 0; i < size; i++) {
                list->Set(context, i, SWIGV8_NUMBER_NEW(longArrVal[i]));
            }
            return list;
        }
        case GS_TYPE_FLOAT_ARRAY: {
            const float *floatArrVal;
            size = value->asArray.length;
            floatArrVal = value->asArray.elements.asFloat;
            list = SWIGV8_ARRAY_NEW();
            for (i = 0; i < size; i++) {
                list->Set(context, i,
                    SWIGV8_NUMBER_NEW(((float *)floatArrVal)[i]));
            }
            return list;
        }
        case GS_TYPE_DOUBLE_ARRAY: {
            const double *doubleArrVal;
            size = value->asArray.length;
            doubleArrVal = value->asArray.elements.asDouble;
            list = SWIGV8_ARRAY_NEW();
            for (i = 0; i < size; i++) {
                list->Set(context, i,
                    SWIGV8_NUMBER_NEW(((double *)doubleArrVal)[i]));
            }
            return list;
        }
        case GS_TYPE_TIMESTAMP_ARRAY: {
            const GSTimestamp *timestampArrVal;
            size = value->asArray.length;
            timestampArrVal = value->asArray.elements.asTimestamp;
            list = SWIGV8_ARRAY_NEW();
            for (i = 0; i < size; i++) {
                list->Set(context, i, convertTimestampToObject(
                    (GSTimestamp*)&(timestampArrVal[i]), timestampToFloat));
            }
            return list;
        }
        default:
            return SWIGV8_NULL();
    }

    return SWIGV8_NULL();
}
}

%fragment("cleanStringArray", "header") {
static void cleanStringArray(GSChar** arrString, size_t size) {
    if (!arrString) {
        return;
    }

    for (int i = 0; i < size; i++) {
        if (arrString[i]) {
            delete[] arrString[i];
        }
    }

    delete[] arrString;
}
}

%fragment("convertObjectToStringArray", "header",
        fragment = "cleanStringArray", fragment = "cleanString") {
static GSChar** convertObjectToStringArray(v8::Local<v8::Value> value, int* size) {
    GSChar** arrString = NULL;
    size_t arraySize;
    int alloc = 0;
    char* v;
    v8::Local<v8::Array> arr;
    if (!value->IsArray()) {
        return NULL;
    }
    arr = v8::Local<v8::Array>::Cast(value);
    arraySize = (int) arr->Length();

    *size = (int)arraySize;

    try {
        arrString = new GSChar*[arraySize]();
    } catch (std::bad_alloc& ba) {
        return NULL;
    }

    v8::Local<v8::Context> context = Nan::GetCurrentContext();
    memset(arrString, 0x0, arraySize * sizeof(GSChar*));
    for (int i = 0; i < arraySize; i++) {
        v8::Local<v8::Value> value = arr->Get(context, i).
                                         ToLocalChecked();
        if (!value->IsString()) {
            cleanStringArray(arrString, arraySize);
            return NULL;
        }

        int res = SWIG_AsCharPtrAndSize(value, &v, NULL, &alloc);
        if (!SWIG_IsOK(res)) {
            cleanStringArray(arrString, arraySize);
            return NULL;
        }

        if (v) {
            griddb::Util::strdup((const GSChar**)(&arrString[i]), v);
            cleanString(v, alloc);
        }
    }

    return arrString;
}
}

/**
 * Support convert type from object to Bool. input in target language can be :
 * integer or boolean
 */
%fragment("convertObjectToBool", "header", fragment = "SWIG_AsVal_bool", fragment = "SWIG_AsVal_int") {
static bool convertObjectToBool(v8::Local<v8::Value> value, bool* boolValPtr) {
    int checkConvert = 0;
    if (value->IsInt32()) {
        //input can be integer
        int intVal ;
        checkConvert = SWIG_AsVal_int(value, &intVal);
        if (!SWIG_IsOK(checkConvert)) {
            return false;
        }
        *boolValPtr = (intVal != 0);
        return true;
    } else {
        //input is boolean
        checkConvert = SWIG_AsVal_bool(value, boolValPtr);
        if (!SWIG_IsOK(checkConvert)) {
            return false;
        }
        return true;
    }
}
}

%fragment("convertTimestampToObject", "header") {
static v8::Local<v8::Value> convertTimestampToObject(GSTimestamp* timestamp, bool timestamp_to_float = true) {
    if (timestamp_to_float) {
        return SWIGV8_NUMBER_NEW(*timestamp);
    }

%#if (V8_MAJOR_VERSION-0) < 4 && (SWIG_V8_VERSION < 0x032318)
    return v8::Date::New(*timestamp);
%#else
    return v8::Date::New(Nan::GetCurrentContext(), (double)(*timestamp)).ToLocalChecked();
%#endif
}
}

/**
 * Support clean output of SWIG_AsCharPtrAndSize after used
 */
%fragment("cleanString", "header") {
static void cleanString(const GSChar* string, int alloc){
    if (!string) {
        return;
    }

    if (string && alloc == SWIG_NEWOBJ) {
        delete [] string;
    }
}
}

/**
 * Support check number is int64_t
 */
%fragment("isInt64", "header") {
static bool isInt64(double x) {
    return x == static_cast<double>(static_cast<int64_t>(x));
}
}
/**
 * Support convert type from object to long.
 */
%fragment("convertObjectToLong", "header", fragment = "isInt64") {
static bool convertObjectToLong(v8::Local<v8::Value> value, int64_t* longVal) {
    int checkConvert = 0;
    if (isInt64(value->NumberValue(Nan::GetCurrentContext()).FromJust())) {
        //input can be integer
        if (!(value->IsNumber())) {
            return false;
        }
        *longVal = SWIGV8_INTEGER_VALUE(value);
        //When input value is integer, it should be between -9007199254740992(-2^53)/9007199254740992(2^53).
        return (-9007199254740992 <= *longVal && 9007199254740992 >= *longVal);
    } else {
        return false;
    }
}
}

/**
 * Support convert type from object to Float. input in target language can be :
 * float or integer
 */
%fragment("convertObjectToDouble", "header") {
static bool convertObjectToDouble(v8::Local<v8::Value> value, double* floatValPtr) {
    //input is float
    if (!(value->IsNumber())) {
        return false;
    }
    *floatValPtr = value->NumberValue(Nan::GetCurrentContext()).FromJust();
    return true;
}
}
/**
 * Support convert type from object to Float. input in target language can be :
 * float or integer
 */
%fragment("convertObjectToFloat", "header") {
static bool convertObjectToFloat(v8::Local<v8::Value> value, float* floatValPtr) {
    if (!(value->IsNumber())) {
        return false;
    }
    *floatValPtr = value->NumberValue(Nan::GetCurrentContext()).FromJust();

    return (*floatValPtr <= std::numeric_limits<float>::max() &&
            *floatValPtr >= -1 *std::numeric_limits<float>::max());
}
}
/**
 * Support convert type from object to GSTimestamp: input in target language can be :
 * datetime object, string or float
 */
%fragment("convertObjectToGSTimestamp", "header", fragment = "convertObjectToFloat", fragment = "cleanString") {
static bool convertObjectToGSTimestamp(v8::Local<v8::Value> value, GSTimestamp* timestamp) {
    int year, month, day, hour, minute, second, milliSecond, microSecond;
    size_t size = 0;
    int res;
    char* v = 0;
    int alloc;
    GSBool retConvertTimestamp;

    if (value->IsBoolean()) {
        return false;
    }
    float floatTimestamp;
    double utcTimestamp;
    if (value->IsDate()) {
        *timestamp = value->NumberValue(Nan::GetCurrentContext()).FromJust();
        return true;
    } else if (value->IsString()) {

        // Input is datetime string: ex
        res = SWIG_AsCharPtrAndSize(value, &v, &size, &alloc);

        if (!SWIG_IsOK(res)) {
           return false;
        }

        retConvertTimestamp = gsParseTime(v, timestamp);
        cleanString(v, alloc);
        return (retConvertTimestamp == GS_TRUE);
    } else if (value->IsNumber()) {
        *timestamp = value->NumberValue(Nan::GetCurrentContext()).FromJust();
        if (*timestamp > (UTC_TIMESTAMP_MAX * 1000)) { //miliseconds
            return false;
        }
        return true;
    } else {
        // Invalid input
        return false;
    }
}
}

/**
 * Support convert row key Field from NodeJS object to C Object with specific type
 */
%fragment("convertToRowKeyFieldWithType", "header", fragment = "SWIG_AsCharPtrAndSize"
        , fragment = "convertObjectToBool", fragment = "convertObjectToGSTimestamp"
        , fragment = "convertObjectToDouble", fragment = "convertObjectToLong"
        , fragment = "convertObjectToStringArray", fragment = "cleanString") {
static bool convertToRowKeyFieldWithType(griddb::Field &field, v8::Local<v8::Value> value, GSType type) {
    size_t size = 0;
    int res;
    char* v = 0;
    int alloc;
    int checkConvert = 0;

    field.type = type;

    if (value->IsNull() || value->IsUndefined()) {
        // Not support null
        return false;
    }

    switch (type) {
        case GS_TYPE_STRING:
            if (!value->IsString()) {
                return false;
            }
            res = SWIG_AsCharPtrAndSize(value, &v, &size, &alloc);
            if (!SWIG_IsOK(res)) {
               return false;
            }
            if (v) {
                griddb::Util::strdup(&(field.value.asString), v);
                cleanString(v, alloc);
            } else {
                field.value.asString = NULL;
            }
            break;
        case GS_TYPE_INTEGER:
            if (!value->IsInt32()) {
                return false;
            }
            field.value.asInteger = value->IntegerValue(Nan::GetCurrentContext()).FromJust();
            break;
        case GS_TYPE_LONG:
            return convertObjectToLong(value, &field.value.asLong);
            break;
        case GS_TYPE_TIMESTAMP:
            return convertObjectToGSTimestamp(value, &field.value.asTimestamp);
            break;
        default:
            //Not support for now
            return false;
            break;
    }
    return true;
}
}

%fragment("convertToFieldWithType", "header", fragment = "SWIG_AsCharPtrAndSize",
        fragment = "convertObjectToDouble", fragment = "convertObjectToGSTimestamp", 
        fragment = "convertObjectToBool", fragment = "convertObjectToFloat", 
        fragment = "convertObjectToStringArray", fragment = "cleanString",
        fragment = "convertObjectToLong") {
static bool convertToFieldWithType(GSRow *row, int column, v8::Local<v8::Value> value, GSType type) {
    int32_t intVal;
    size_t size;
    int tmpInt; //support convert to byte array, short array
    int res;
    bool vbool;
    int alloc;
    int i;
    GSResult ret;
    v8::Local<v8::Array> arr;

    if (value->IsNull() || value->IsUndefined()) {
        ret = gsSetRowFieldNull(row, column);
        return GS_SUCCEEDED(ret);
    }

    int checkConvert = 0;
    v8::Local<v8::Context> context = Nan::GetCurrentContext();
    switch (type) {
        case GS_TYPE_STRING: {
            GSChar* stringVal;
            if (!value->IsString()) {
                return false;
            }
            res = SWIG_AsCharPtrAndSize(value, &stringVal, &size, &alloc);
            if (!SWIG_IsOK(res)) {
                return false;
            }
            ret = gsSetRowFieldByString(row, column, stringVal);
            cleanString(stringVal, alloc);
            break;
        }
        case GS_TYPE_LONG: {
            int64_t longVal;
            vbool = convertObjectToLong(value, &longVal);
            if (!vbool) {
                return false;
            }
            ret = gsSetRowFieldByLong(row, column, longVal);
            break;
        }
        case GS_TYPE_BOOL: {
            GSBool boolVal;
            vbool = convertObjectToBool(value, (bool*)&boolVal);
            if (!vbool) {
                return false;
            }
            ret = gsSetRowFieldByBool(row, column, boolVal);
            break;
        }
        case GS_TYPE_BYTE: {
            if (!value->IsInt32()) {
                return false;
            }
            int64_t tmpInt = value->IntegerValue(Nan::GetCurrentContext()).FromJust();
            if (tmpInt < std::numeric_limits<int8_t>::min() || 
                    tmpInt > std::numeric_limits<int8_t>::max()) {
                return false;
            }
            ret = gsSetRowFieldByByte(row, column, tmpInt);
            break;
        }
        case GS_TYPE_SHORT: {
            if (!value->IsInt32()) {
                return false;
            }
            int64_t tmpInt = value->IntegerValue(Nan::GetCurrentContext()).FromJust();
            if (tmpInt < std::numeric_limits<int16_t>::min() ||
                    tmpInt > std::numeric_limits<int16_t>::max()) {
                return false;
            }
            ret = gsSetRowFieldByShort(row, column, tmpInt);
            break;
        }
        case GS_TYPE_INTEGER:
            if (!value->IsInt32()) {
                return false;
            }
            ret = gsSetRowFieldByInteger(row, column, value->IntegerValue(Nan::GetCurrentContext()).FromJust());
            break;
        case GS_TYPE_FLOAT: {
            float floatVal;
            vbool = convertObjectToFloat(value, &floatVal);
            if (!vbool) {
                return false;
            }
            ret = gsSetRowFieldByFloat(row, column, floatVal);
            break;
        }
        case GS_TYPE_DOUBLE: {
            double doubleVal;
            vbool = convertObjectToDouble(value, &doubleVal);
            if (!vbool) {
                return false;
            }
            ret = gsSetRowFieldByDouble(row, column, doubleVal);
            break;
        }
        case GS_TYPE_TIMESTAMP: {
            GSTimestamp timestampVal;
            vbool = convertObjectToGSTimestamp(value, &timestampVal);
            if (!vbool) {
                return false;
            }
            ret = gsSetRowFieldByTimestamp(row, column, timestampVal);
            break;
        }
        case GS_TYPE_BLOB: {
            GSBlob blobVal;
            if (!node::Buffer::HasInstance(value)) {
                return false;
            }
            char* v = (char*) node::Buffer::Data(value);
            size = node::Buffer::Length (value);

            blobVal.data = v;
            blobVal.size = size;
            ret = gsSetRowFieldByBlob(row, column, (const GSBlob *)&blobVal);
            break;
        }
        case GS_TYPE_STRING_ARRAY: {
            const GSChar *const *stringArrVal;
            int length;
            stringArrVal = convertObjectToStringArray(value, &length);
            if (stringArrVal == NULL) {
                return false;
            }
            size = length;
            ret = gsSetRowFieldByStringArray(row, column, stringArrVal, size);
            for (i = 0; i < length; i++) {
                if (stringArrVal[i]) {
                    delete[] stringArrVal[i];
                }
            }
            delete[] stringArrVal;
            break;
        }
        case GS_TYPE_GEOMETRY: {
            GSChar *geometryVal;
            if (!value->IsString()) {
                return false;
            }
            res = SWIG_AsCharPtrAndSize(value, &geometryVal, &size, &alloc);

            if (!SWIG_IsOK(res)) {
                return false;
            }
            ret = gsSetRowFieldByGeometry(row, column, geometryVal);
            cleanString(geometryVal, alloc);
            break;
        }
        case GS_TYPE_INTEGER_ARRAY: {
            int32_t *intArrVal;
            if (!value->IsArray()) {
                return false;
            }
            arr = v8::Local<v8::Array>::Cast(value);
            size = (int) arr->Length();

            try {
                intArrVal = new int32_t[size]();
            } catch (std::bad_alloc& ba) {
                return false;
            }
            for (i = 0; i < size; i++) {
                v8::Local<v8::Value> value = arr->Get(context, i).ToLocalChecked();
                checkConvert = SWIG_AsVal_int(value, (intArrVal + i));
                if (!SWIG_IsOK(checkConvert)) {
                    delete[] intArrVal;
                    intArrVal = NULL;
                    return false;
                }
            }
            ret = gsSetRowFieldByIntegerArray(row, column, (const int32_t *) intArrVal, size);
            delete[] intArrVal;
            break;
        }
        case GS_TYPE_BOOL_ARRAY: {
            GSBool *boolArrVal;
            if (!value->IsArray()) {
                return false;
            }
            arr = v8::Local<v8::Array>::Cast(value);
            size = (int) arr->Length();
            try {
                boolArrVal = new GSBool[size]();
            } catch (std::bad_alloc& ba) {
                return false;
            }
            for (i = 0; i < size; i++) {
                v8::Local<v8::Value> value = arr->Get(context, i).ToLocalChecked();
                vbool = convertObjectToBool(value, (bool*)(boolArrVal + i));
                if (!vbool) {
                    delete[] boolArrVal;
                    boolArrVal = NULL;
                    return false;
                }
            }
            ret = gsSetRowFieldByBoolArray(row, column, (const GSBool *)boolArrVal, size);
            delete[] boolArrVal;
            break;
        }
        case GS_TYPE_BYTE_ARRAY: {
            int8_t *byteArrVal;
            if (!value->IsArray()) {
                return false;
            }
            arr = v8::Local<v8::Array>::Cast(value);
            size = (int) arr->Length();
            try {
                byteArrVal = new int8_t[size]();
            } catch (std::bad_alloc& ba) {
                return false;
            }
            for (i = 0; i < size; i++) {
                v8::Local<v8::Value> value = arr->Get(context, i).ToLocalChecked();
                checkConvert = SWIG_AsVal_int(value, &tmpInt);
                byteArrVal[i] = (int8_t)tmpInt;
                 if (!SWIG_IsOK(checkConvert) ||
                    tmpInt < std::numeric_limits<int8_t>::min() ||
                    tmpInt > std::numeric_limits<int8_t>::max() ||
                    (!value->IsInt32())) {
                    delete[] byteArrVal;
                    byteArrVal = NULL;
                    return false;
                }
            }
            ret = gsSetRowFieldByByteArray(row, column, (const int8_t *)byteArrVal, size);
            delete[] byteArrVal;
            break;
        }
        case GS_TYPE_SHORT_ARRAY: {
            int16_t *shortArrVal;
            if (!value->IsArray()) {
                return false;
            }
            arr = v8::Local<v8::Array>::Cast(value);
            size = (int) arr->Length();
            try {
                shortArrVal = new int16_t[size]();
            } catch (std::bad_alloc& ba) {
                return false;
            }
            for (i = 0; i < size; i++) {
                v8::Local<v8::Value> value = arr->Get(context, i).ToLocalChecked();
                checkConvert = SWIG_AsVal_int(value, &tmpInt);
                if (!SWIG_IsOK(checkConvert) ||
                    tmpInt < std::numeric_limits<int16_t>::min() ||
                    tmpInt > std::numeric_limits<int16_t>::max() ||
                    (!value->IsInt32())) {
                        delete[] shortArrVal;
                        shortArrVal = NULL;
                    return false;
                }
                shortArrVal[i] = (int16_t)tmpInt;
            }
            ret = gsSetRowFieldByShortArray(row, column, (const int16_t *)shortArrVal, size);
            delete[] shortArrVal;
            break;
        }
        case GS_TYPE_LONG_ARRAY: {
            int64_t *longArrVal;
            if (!value->IsArray()) {
                return false;
            }
            arr = v8::Local<v8::Array>::Cast(value);
            size = (int) arr->Length();
            try {
                longArrVal = new int64_t[size]();
            } catch (std::bad_alloc& ba) {
                return false;
            }
            for (i = 0; i < size; i++) {
                v8::Local<v8::Value> value = arr->Get(context, i).ToLocalChecked();
                vbool = convertObjectToLong(value, &longArrVal[i]);
                if (!vbool) {
                    delete[] longArrVal;
                    longArrVal = NULL;
                    return false;
                }
            }
            ret = gsSetRowFieldByLongArray(row, column, (const int64_t *)longArrVal, size);
            delete[] longArrVal;
            break;
        }
        case GS_TYPE_FLOAT_ARRAY: {
            float *floatArrVal;
            if (!value->IsArray()) {
                return false;
            }
            arr = v8::Local<v8::Array>::Cast(value);
            size = (int) arr->Length();
            try {
                floatArrVal = new float[size]();
            } catch (std::bad_alloc& ba) {
                return false;
            }
            for (i = 0; i < size; i++) {
                v8::Local<v8::Value> value = arr->Get(context, i).ToLocalChecked();
                vbool = convertObjectToFloat(value, &floatArrVal[i]);
                if (!vbool) {
                    delete[] floatArrVal;
                    floatArrVal = NULL;
                    return false;
                }
            }
            ret = gsSetRowFieldByFloatArray(row, column, (const float *) floatArrVal, size);
            delete[] floatArrVal;
            break;
        }
        case GS_TYPE_DOUBLE_ARRAY: {
            double *doubleArrVal;
            double tmpDouble; //support convert to double array
            if (!value->IsArray()) {
                return false;
            }
            arr = v8::Local<v8::Array>::Cast(value);
            size = (int) arr->Length();
            try {
                doubleArrVal = new double[size]();
            } catch (std::bad_alloc& ba) {
                return false;
            }
            for (i = 0; i < size; i++) {
                v8::Local<v8::Value> value = arr->Get(context, i).ToLocalChecked();
                vbool = convertObjectToDouble(value, &tmpDouble);
                doubleArrVal[i] = tmpDouble;
                if (!vbool) {
                    delete[] doubleArrVal;
                    doubleArrVal = NULL;
                    return false;
                }
            }
            ret = gsSetRowFieldByDoubleArray(row, column, (const double *)doubleArrVal, size);
            delete[] doubleArrVal;
            break;
        }
        case GS_TYPE_TIMESTAMP_ARRAY: {
            GSTimestamp *timestampArrVal;
            if (!value->IsArray()) {
                return false;
            }
            arr = v8::Local<v8::Array>::Cast(value);
            size = (int) arr->Length();
            try {
                timestampArrVal = new GSTimestamp[size]();
            } catch (std::bad_alloc& ba) {
                return false;
            }
            bool checkRet;
            for (i = 0; i < size; i++) {
                v8::Local<v8::Value> value = arr->Get(context, i).ToLocalChecked();
                checkRet = convertObjectToGSTimestamp(value, (timestampArrVal + i));
                if (!checkRet) {
                    delete[] timestampArrVal;
                    timestampArrVal = NULL;
                    return false;
                }
            }
            ret = gsSetRowFieldByTimestampArray(row, column, (const GSTimestamp *)timestampArrVal, size);
            delete[] timestampArrVal;
            break;
        }
        default:
            //Not support for now
            return false;
            break;
    }
    return GS_SUCCEEDED(ret);
}
}

/**
* Typemaps for get_store() function
*/
%typemap(in, numinputs = 1, fragment = "SWIG_AsCharPtrAndSize", fragment = "cleanString", fragment = "freeargGetStore") 
        (const char* host, int32_t port, const char* cluster_name, const char* database, const char* username, const char* password,
        const char* notification_member, const char* notification_provider) 
        (v8::Local<v8::Object> obj, v8::Local<v8::Array> keys, int i, int j, size_t size = 0, int* alloc = 0, int res, char* v = 0) {
    if (!$input->IsObject()) {
        SWIG_V8_Raise("Expected object property as input");
        SWIG_fail;
    }
    v8::Local<v8::Context> context = Nan::GetCurrentContext();
    obj = $input->ToObject(context).ToLocalChecked();
    keys = obj->GetOwnPropertyNames(context).ToLocalChecked();
    int len = (int) keys->Length();
    char* name = 0;
    $1 = NULL;
    $2 = 0;
    $3 = NULL;
    $4 = NULL;
    $5 = NULL;
    $6 = NULL;
    $7 = NULL;
    $8 = NULL;
    if (len > 0) {
        try {
            alloc = new int[len * 2]();
        } catch (std::bad_alloc& ba) {
            SWIG_V8_Raise("Memory allocation error");
            SWIG_fail;
        }

        j = 0;
        for (int i = 0; i < len; i++) {
            v8::Local<v8::Value> key = keys->Get(context, i).ToLocalChecked();
            res = SWIG_AsCharPtrAndSize(key, &name, &size, &alloc[j]);
            if (!SWIG_IsOK(res)) {
                freeargGetStore($1, $3, $4, $5, $6, $7, $8, alloc);
                %variable_fail(res, "String", "name");
            }

            v8::Local<v8::Value> value = obj->Get(context, key).
                                             ToLocalChecked();
            if (strcmp(name, "port") == 0) {
                //Input valid is number only
                if (value->IsInt32()) {
                    $2 = value->IntegerValue(context).FromJust();
                } else {
                    cleanString(name, alloc[j]);
                    freeargGetStore($1, $3, $4, $5, $6, $7, $8, alloc);
                    %variable_fail(res, "String", "value");
                }
            } else {
                res = SWIG_AsCharPtrAndSize(value, &v, &size, &alloc[j + 1]);
                if (!SWIG_IsOK(res)) {
                    cleanString(name, alloc[j]);
                    freeargGetStore($1, $3, $4, $5, $6, $7, $8, alloc);
                    %variable_fail(res, "String", "value");
                }
                if (strcmp(name, "host") == 0 && v) {
                    griddb::Util::strdup((const GSChar**)&$1, v);
                } else if (strcmp(name, "clusterName") == 0 && v) {
                    griddb::Util::strdup((const GSChar**)&$3, v);
                } else if (strcmp(name, "database") == 0 && v) {
                    griddb::Util::strdup((const GSChar**)&$4, v);
                } else if (strcmp(name, "username") == 0 && v) {
                    griddb::Util::strdup((const GSChar**)&$5, v);
                } else if (strcmp(name, "password") == 0 && v) {
                    griddb::Util::strdup((const GSChar**)&$6, v);
                } else if (strcmp(name, "notificationMember") == 0 && v) {
                    griddb::Util::strdup((const GSChar**)&$7, v);
                } else if (strcmp(name, "notificationProvider") == 0 && v) {
                    griddb::Util::strdup((const GSChar**)&$8, v);
                } else {
                    cleanString(name, alloc[j]);
                    cleanString(v, alloc[j + 1]);
                    freeargGetStore($1, $3, $4, $5, $6, $7, $8, alloc);
                    SWIG_V8_Raise("Invalid Property");
                    SWIG_fail;
                }
            }
            cleanString(name, alloc[j]);
            if (v) {
                cleanString(v, alloc[j + 1]);
                v = NULL;
            }

            j += 2;
        }
    }
}

%typemap(freearg, fragment = "freeargGetStore") (const char* host, int32_t port, const char* cluster_name,
        const char* database, const char* username, const char* password,
        const char* notification_member, const char* notification_provider) {
    freeargGetStore($1, $3, $4, $5, $6, $7, $8, alloc$argnum);
}

%fragment("freeargGetStore", "header") {
    //SWIG does not include freearg in fail: label (not like Python, so we need this function)
static void freeargGetStore(const char* host, const char* cluster_name,
        const char* database, const char* username, const char* password,
        const char* notification_member, const char* notification_provider, int* alloc) {
    if (host) {
        delete[] host;
    }
    if (cluster_name) {
        delete[] cluster_name;
    }
    if (database) {
        delete[] database;
    }
    if (username) {
        delete[] username;
    }
    if (password) {
        delete[] password;
    }
    if (notification_member) {
        delete[] notification_member;
    }
    if (notification_provider) {
        delete[] notification_provider;
    }
    if (alloc) {
        delete[] alloc;
    }
}
}

/**
* Typemaps for fetch_all() function
*/
%typemap(in) (GSQuery* const* queryList, size_t queryCount)
(v8::Local<v8::Array> arr, v8::Local<v8::Value> query, griddb::Query *vquery, int res = 0) {
    if ($input->IsNull()) {
        $1 = NULL;
        $2 = 0;
    } else if (!$input->IsArray()) {
        SWIG_V8_Raise("Expected array as input");
        SWIG_fail;
    } else {
        arr = v8::Local<v8::Array>::Cast($input);
        $2 = (int) arr->Length();
        $1 = NULL;
        if ($2 > 0) {
            try {
                $1 = new GSQuery*[$2]();
            } catch (std::bad_alloc& ba) {
                SWIG_V8_Raise("Memory allocation error");
                SWIG_fail;
            }
            v8::Local<v8::Context> context = Nan::GetCurrentContext();
            for (int i = 0; i < $2; i++) {
                query = arr->Get(context, i).ToLocalChecked();
                res = SWIG_ConvertPtr(query, (void**)&vquery, $descriptor(griddb::Query*), 0);
                if (!SWIG_IsOK(res)) {
                    delete[] $1;
                    SWIG_V8_Raise("Convert pointer failed");
                    SWIG_fail;
                }
                $1[i] = vquery->gs_ptr();
            }
        }
    }
}

%typemap(freearg) (GSQuery* const* queryList, size_t queryCount) {
    if ($1) {
        delete[] $1;
    }
}

/**
* Typemaps output for partition controller function
*/
%typemap(in, numinputs = 0) (const GSChar *const ** stringList, size_t *size) (GSChar **nameList1, size_t size1) {
    $1 = &nameList1;
    $2 = &size1;
}

%typemap(argout, numinputs = 0) (const GSChar *const ** stringList, size_t *size)
(v8::Local<v8::Array> arr, v8::Handle<v8::String> val) {
    arr = SWIGV8_ARRAY_NEW();
    v8::Local<v8::Context> context = Nan::GetCurrentContext();
    for (int i = 0; i < size1$argnum; i++) {
        val = SWIGV8_STRING_NEW2(nameList1$argnum[i], strlen(nameList1$argnum[i]));
        arr->Set(context, i, val);
    }

    $result = arr;
}

%typemap(in, numinputs = 0) (const int **intList, size_t *size) (int *intList1, size_t size1) {
    $1 = &intList1;
    $2 = &size1;
}

%typemap(argout, numinputs = 0) (const int **intList, size_t *size)
(v8::Local<v8::Array> arr, v8::Handle<v8::Integer> val) {
    arr = SWIGV8_ARRAY_NEW();
    v8::Local<v8::Context> context = Nan::GetCurrentContext();
    for (int i = 0; i < size1$argnum; i++) {
        val = SWIGV8_INTEGER_NEW(intList1$argnum[i]);
        arr->Set(context, i, val);
    }

    $result = arr;
}

%typemap(in, numinputs = 0) (const long **longList, size_t *size) (long *longList1, size_t size1) {
    $1 = &longList1;
    $2 = &size1;
}

%typemap(argout, numinputs = 0) (const long **longList, size_t *size)
(v8::Local<v8::Array> arr, v8::Handle<v8::Number> val) {
    arr = SWIGV8_ARRAY_NEW();
    v8::Local<v8::Context> context = Nan::GetCurrentContext();
    for (int i = 0; i < size1$argnum; i++) {
        val = SWIGV8_NUMBER_NEW(longList1$argnum[i]);
        arr->Set(context, i, val);
    }

    $result = arr;
}

/*
* typemap for get function in AggregationResult class
*/
%typemap(in, numinputs = 0) (griddb::Field *agValue) (griddb::Field tmpAgValue) {
    $1 = &tmpAgValue;
}
%typemap(argout, fragment = "convertFieldToObject") (griddb::Field *agValue) {
    $result = convertFieldToObject(&($1->value), $1->type, arg1->timestamp_output_with_float);
}

/**
* Typemaps for RowSet::update() and Container::put() function
* The argument "GSRow *rowdata" is not used in the function body, it only for the purpose of typemap matching pattern
* The actual input data is store in class member and can be get by function getGSRowPtr()
*/
%typemap(in, fragment = "convertToFieldWithType") (GSRow* row) {
    if (!$input->IsArray()) {
        SWIG_V8_Raise("Expected array as input");
        SWIG_fail;
    }
    v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast($input);
    int leng = (int)arr->Length();
    GSRow *tmpRow = arg1->getGSRowPtr();
    int colNum = arg1->getColumnCount();
    if (leng != colNum) {
        SWIG_V8_Raise("Num row is different with container info");
        SWIG_fail;
    }
    GSType* typeList = arg1->getGSTypeList();
    v8::Local<v8::Context> context = Nan::GetCurrentContext();
    for (int i = 0; i < leng; i++) {
        GSType type = typeList[i];
        v8::Local<v8::Value> value = arr->Get(context, i).ToLocalChecked();
        if (!(convertToFieldWithType(tmpRow, i, value, type))) {
            char errorMsg[60];
            sprintf(errorMsg, "Invalid value for column %d, type should be : %d", i, type);
            SWIG_V8_Raise(errorMsg);
            SWIG_fail;
        }
    }
}

/*
* typemap for Container::get()
*/
%typemap(in, fragment = "convertToRowKeyFieldWithType") (griddb::Field* keyFields)(griddb::Field field) {
    $1 = &field;
    if ($input->IsNull() || $input->IsUndefined()) {
        $1->type = GS_TYPE_NULL;
    } else {
        GSType* typeList = arg1->getGSTypeList();
        GSType type = typeList[0];
        if (!convertToRowKeyFieldWithType(*$1, $input, type)) {
            SWIG_V8_Raise("Can not convert to row field");
            SWIG_fail;
        }
    }
}

%typemap(in, numinputs = 0) (GSRow *rowdata) {
    $1 = NULL;
}

/**
 * Support convert data from GSRow* row to javascript data
 */
%fragment("getRowFields", "header", fragment = "convertTimestampToObject") {
static bool getRowFields(GSRow* row, int columnCount, GSType* typeList, bool timestampOutput, int* columnError, 
        GSType* fieldTypeError, v8::Local<v8::Array> outList) {
    GSResult ret;
    GSValue mValue;
    bool retVal = true;
    v8::Local<v8::Context> context = Nan::GetCurrentContext();
    for (int i = 0; i < columnCount; i++) {
        //Check NULL value
        GSBool nullValue;

        ret = gsGetRowFieldNull(row, (int32_t) i, &nullValue);
        if (!GS_SUCCEEDED(ret)) {
            *columnError = i;
            retVal = false;
            *fieldTypeError = GS_TYPE_NULL;
            return retVal;
        }
        if (nullValue) {
            outList->Set(context, i, SWIGV8_NULL());
            continue;
        }

        switch(typeList[i]) {
            case GS_TYPE_LONG: {
                int64_t longValue;
                ret = gsGetRowFieldAsLong(row, (int32_t) i, &longValue);
                if (!GS_SUCCEEDED(ret)) {
                    break;
                }
                outList->Set(context, i, SWIGV8_NUMBER_NEW(longValue));
                break;
            }
            case GS_TYPE_STRING: {
                GSChar* stringValue;
                ret = gsGetRowFieldAsString(row, (int32_t) i, (const GSChar **)&stringValue);
                if (!GS_SUCCEEDED(ret)) {
                    break;
                }
                outList->Set(context, i, SWIGV8_STRING_NEW((const char * )stringValue));
                break;
            }
            case GS_TYPE_BLOB: {
                GSBlob blobValue;
                ret = gsGetRowFieldAsBlob(row, (int32_t) i, &blobValue);
                if (!GS_SUCCEEDED(ret)) {
                    break;
                }
                outList->Set(context, i, Nan::CopyBuffer((char *)blobValue.data,
                    blobValue.size).ToLocalChecked());
                break;
            }
            case GS_TYPE_BOOL: {
                GSBool boolValue;
                ret = gsGetRowFieldAsBool(row, (int32_t) i, &boolValue);
                if (!GS_SUCCEEDED(ret)) {
                    break;
                }
                outList->Set(context, i, SWIGV8_BOOLEAN_NEW((bool)boolValue));
                break;
            }
            case GS_TYPE_INTEGER: {
                int32_t intValue;
                ret = gsGetRowFieldAsInteger(row, (int32_t) i, &intValue);
                if (!GS_SUCCEEDED(ret)) {
                    break;
                }
                outList->Set(context, i, SWIGV8_INT32_NEW(intValue));
                break;
            }
            case GS_TYPE_FLOAT: {
                float floatValue;
                ret = gsGetRowFieldAsFloat(row, (int32_t) i, &floatValue);
                if (!GS_SUCCEEDED(ret)) {
                    break;
                }
                outList->Set(context, i, SWIGV8_NUMBER_NEW(floatValue));
                break;
            }
            case GS_TYPE_DOUBLE: {
                double doubleValue;
                ret = gsGetRowFieldAsDouble(row, (int32_t) i, &doubleValue);
                if (!GS_SUCCEEDED(ret)) {
                    break;
                }
                outList->Set(context, i, SWIGV8_NUMBER_NEW(doubleValue));
                break;
            }
            case GS_TYPE_TIMESTAMP: {
                GSTimestamp timestampValue;
                ret = gsGetRowFieldAsTimestamp(row, (int32_t) i, &timestampValue);
                if (!GS_SUCCEEDED(ret)) {
                    break;
                }
                outList->Set(context, i, convertTimestampToObject(&timestampValue, timestampOutput));
                break;
            }
            case GS_TYPE_BYTE: {
                int8_t byteValue;
                ret = gsGetRowFieldAsByte(row, (int32_t) i, &byteValue);
                if (!GS_SUCCEEDED(ret)) {
                    break;
                }
                outList->Set(context, i, SWIGV8_INT32_NEW(byteValue));
                break;
            }
            case GS_TYPE_SHORT: {
                int16_t shortValue;
                ret = gsGetRowFieldAsShort(row, (int32_t) i, &shortValue);
                if (!GS_SUCCEEDED(ret)) {
                    break;
                }
                outList->Set(context, i, SWIGV8_INT32_NEW(shortValue));
                break;
            }
            case GS_TYPE_GEOMETRY: {
                GSChar* geoValue;
                ret = gsGetRowFieldAsGeometry(row, (int32_t) i, (const GSChar **)&geoValue);
                if (!GS_SUCCEEDED(ret)) {
                    break;
                }
                outList->Set(context, i, SWIGV8_STRING_NEW(geoValue));
                break;
            }
            case GS_TYPE_INTEGER_ARRAY: {
                int32_t* intArr;
                size_t size;
                ret = gsGetRowFieldAsIntegerArray (row, (int32_t) i, (const int32_t **)&intArr, &size);
                if (!GS_SUCCEEDED(ret)) {
                    break;
                }
                v8::Local<v8::Array> list = SWIGV8_ARRAY_NEW();
                for (int j = 0; j < size; j++) {
                    list->Set(context, j, SWIG_From_int(intArr[j]));
                }
                outList->Set(context, i, list);
                break;
            }
            case GS_TYPE_STRING_ARRAY: {
                GSChar** stringArrVal;
                size_t size;
                ret = gsGetRowFieldAsStringArray (row, (int32_t) i, ( const GSChar *const **)&stringArrVal, &size);
                if (!GS_SUCCEEDED(ret)) {
                    break;
                }
                v8::Local<v8::Array> list = SWIGV8_ARRAY_NEW();
                for (int j = 0; j < size; j++) {
                    list->Set(context, j,
                        SWIGV8_STRING_NEW((GSChar *)stringArrVal[j]));
                }
                outList->Set(context, i, list);
                break;
            }
            case GS_TYPE_BOOL_ARRAY: {
                GSBool* boolArr;
                size_t size;
                ret = gsGetRowFieldAsBoolArray(row, (int32_t) i, (const GSBool **)&boolArr, &size);
                if (!GS_SUCCEEDED(ret)) {
                    break;
                }
                v8::Local<v8::Array> list = SWIGV8_ARRAY_NEW();
                for (int j = 0; j < size; j++) {
                    list->Set(context, j, SWIG_From_bool(boolArr[j]));
                }
                outList->Set(context, i, list);
                break;
            }
            case GS_TYPE_BYTE_ARRAY: {
                int8_t* byteArr;
                size_t size;
                ret = gsGetRowFieldAsByteArray(row, (int32_t) i, (const int8_t **)&byteArr, &size);
                if (!GS_SUCCEEDED(ret)) {
                    break;
                }
                v8::Local<v8::Array> list = SWIGV8_ARRAY_NEW();
                for (int j = 0; j < size; j++) {
                    list->Set(context, j, SWIG_From_int(byteArr[j]));
                }
                outList->Set(context, i, list);
                break;
            }
            case GS_TYPE_SHORT_ARRAY: {
                int16_t* shortArr;
                size_t size;
                ret = gsGetRowFieldAsShortArray(row, (int32_t) i, (const int16_t **)&shortArr, &size);
                if (!GS_SUCCEEDED(ret)) {
                    break;
                }
                v8::Local<v8::Array> list = SWIGV8_ARRAY_NEW();
                for (int j = 0; j < size; j++) {
                    list->Set(context, j, SWIG_From_int(shortArr[j]));
                }
                outList->Set(context, i, list);
                break;
            }
            case GS_TYPE_LONG_ARRAY: {
                int64_t* longArr;
                size_t size;
                ret = gsGetRowFieldAsLongArray(row, (int32_t) i, (const int64_t **)&longArr, &size);
                if (!GS_SUCCEEDED(ret)) {
                    break;
                }
                v8::Local<v8::Array> list = SWIGV8_ARRAY_NEW();
                for (int j = 0; j < size; j++) {
                    list->Set(context, j, SWIGV8_NUMBER_NEW(longArr[j]));
                }
                outList->Set(context, i, list);
                break;
            }
            case GS_TYPE_FLOAT_ARRAY: {
                float* floatArr;
                size_t size;
                ret = gsGetRowFieldAsFloatArray(row, (int32_t) i, (const float **)&floatArr, &size);
                if (!GS_SUCCEEDED(ret)) {
                    break;
                }
                v8::Local<v8::Array> list = SWIGV8_ARRAY_NEW();
                for (int j = 0; j < size; j++) {
                    list->Set(context, j,
                        SWIGV8_NUMBER_NEW(((float *)floatArr)[j]));
                }
                outList->Set(context, i, list);
                break;
            }
            case GS_TYPE_DOUBLE_ARRAY: {
                double* doubleArr;
                size_t size;
                ret = gsGetRowFieldAsDoubleArray(row, (int32_t) i, (const double **)&doubleArr, &size);
                if (!GS_SUCCEEDED(ret)) {
                    break;
                }
                v8::Local<v8::Array> list = SWIGV8_ARRAY_NEW();
                for (int j = 0; j < size; j++) {
                    list->Set(context, j,
                        SWIGV8_NUMBER_NEW(((double *)doubleArr)[j]));
                }
                outList->Set(context, i, list);
                break;
            }
            case GS_TYPE_TIMESTAMP_ARRAY: {
                GSTimestamp* timestampArr;
                size_t size;
                ret = gsGetRowFieldAsTimestampArray(row, (int32_t) i, (const GSTimestamp **)&timestampArr, &size);
                if (!GS_SUCCEEDED(ret)) {
                    break;
                }
                v8::Local<v8::Array> list = SWIGV8_ARRAY_NEW();
                for (int j = 0; j < size; j++) {
                    list->Set(context, j, convertTimestampToObject(
                        (GSTimestamp*)&(timestampArr[j]), timestampOutput));
                }
                outList->Set(context, i, list);
                break;
            }
            default: {
                // NOT OK
                ret = -1;
                break;
            }
        }
        if (!GS_SUCCEEDED(ret)) {
            *columnError = i;
            *fieldTypeError = typeList[i];
            retVal = false;
            return retVal;
        }
    }
    return retVal;
}
}

/*
* This typemap argument out does not get data from argument "GSRow *rowdata"
* The argument "GSRow *rowdata" is not used in the function Container::get(), it only for the purpose of typemap matching pattern
* The actual output data is store in class member and can be get by function getGSRowPtr()
*/
%typemap(argout, fragment = "getRowFields") (GSRow *rowdata) (v8::Local<v8::Array> obj, v8::Handle<v8::Value> val) {
    if (result == GS_FALSE) {
        $result = SWIGV8_NULL();
    } else {
        GSRow* row = arg1->getGSRowPtr();
        obj = SWIGV8_ARRAY_NEW();
        bool retVal;
        int errorColumn;
        GSType errorType;
        retVal = getRowFields(row, arg1->getColumnCount(), arg1->getGSTypeList(), arg1->timestamp_output_with_float, &errorColumn, &errorType, obj);
        if (retVal == false) {
            char errorMsg[60];
            sprintf(errorMsg, "Can't get data for field %d with type %d", errorColumn, errorType);
            SWIG_V8_Raise(errorMsg);
            SWIG_fail;
        }
        $result = obj;
    }
}

/**
 * Typemaps for Store.multi_put
 */
%typemap(in, fragment = "convertToRowKeyFieldWithType", fragment = "SWIG_AsCharPtrAndSize", fragment = "cleanString"
        , fragment = "freeargStoreMultiPut") (GSRow*** listRow, const int *listRowContainerCount, const char ** listContainerName, size_t containerCount)
(v8::Local<v8::Object> obj, v8::Local<v8::Array> keys, v8::Local<v8::Array> arr, int res = 0, v8::Local<v8::Array> rowArr,
size_t sizeTmp = 0, int* alloc = 0, char* v = 0) {
    if (!$input->IsObject()) {
        SWIG_V8_Raise("Expected object property as input");
        SWIG_fail;
    }
    $1 = NULL;
    $2 = NULL;
    $3 = NULL;
    $4 = 0;
    v8::Local<v8::Context> context = Nan::GetCurrentContext();
    obj = $input->ToObject(context).ToLocalChecked();
    keys = obj->GetOwnPropertyNames(context).ToLocalChecked();
    $4 = (size_t) keys->Length();
    griddb::Container* tmpContainer;

    if ($4 > 0) {
        try {
            $1 = new GSRow**[$4]();
            $2 = new int[$4]();
            $3 = new char*[$4]();
            alloc = new int[$4]();
        } catch (std::bad_alloc& ba) {
            freeargStoreMultiPut($1, $2, $3, $4, alloc);
            SWIG_V8_Raise("Memory allocation error");
            SWIG_fail;
        }

        for (int i = 0; i < $4; i++) {
            // Get container name
            v8::Local<v8::Value> key = keys->Get(context, i).ToLocalChecked();
            res = SWIG_AsCharPtrAndSize(key, &v, &sizeTmp, &alloc[i]);
            if (!SWIG_IsOK(res)) {
                freeargStoreMultiPut($1, $2, $3, $4, alloc);
                %variable_fail(res, "String", "containerName");
            }
            if (v) {
                griddb::Util::strdup((const GSChar**)&$3[i], v);
                cleanString(v, alloc[i]);
            } else {
                $3[i] = NULL;
            }
            // Get row
            v8::Local<v8::Value> value = obj->Get(context, key).ToLocalChecked();
            if (!value->IsArray()) {
                freeargStoreMultiPut($1, $2, $3, $4, alloc);
                SWIG_V8_Raise("Expected an array as rowList");
                SWIG_fail;
            }

            arr = v8::Local<v8::Array>::Cast(value);
            $2[i] = (int) arr->Length();
            try {
                $1[i] = new GSRow*[$2[i]]();
            } catch (std::bad_alloc& ba) {
                freeargStoreMultiPut($1, $2, $3, $4, alloc);
                SWIG_V8_Raise("Memory allocation error");
                SWIG_fail;
            }
            //Get container info
            griddb::ContainerInfo* containerInfoTmp = NULL;
            try {
                containerInfoTmp = arg1->get_container_info($3[i]);
            } catch (griddb::GSException& e) {
                freeargStoreMultiPut($1, $2, $3, $4, alloc);
                string innerErrMsg((&e)->what());
                string errMessage = "Get container info for Multiput Error: " + innerErrMsg;
                SWIG_V8_Raise(errMessage.c_str());
                SWIG_fail;
            }
            if (containerInfoTmp == NULL) {
                freeargStoreMultiPut($1, $2, $3, $4, alloc);
                SWIG_V8_Raise("Can not get Container info");
                SWIG_fail;
            }
            ColumnInfoList infoListTmp = containerInfoTmp->get_column_info_list();
            int* typeArr;
            try {
                typeArr = new int[infoListTmp.size]();
            } catch (std::bad_alloc& ba) {
                freeargStoreMultiPut($1, $2, $3, $4, alloc);
                delete containerInfoTmp;
                SWIG_V8_Raise("Memory allocation error");
                SWIG_fail;
            }
            for (int m = 0; m < infoListTmp.size; m++) {
                typeArr[m] = infoListTmp.columnInfo[m].type;
            }
            try {
                tmpContainer = arg1->get_container($3[i]);
            } catch (griddb::GSException& e) {
                freeargStoreMultiPut($1, $2, $3, $4, alloc);
                delete containerInfoTmp;
                string innerErrMsg((&e)->what());
                string errMessage = "Get container for Multiput Error: " + innerErrMsg;
                SWIG_V8_Raise(errMessage.c_str());
                SWIG_fail;
            }
            if (tmpContainer == NULL) {
                delete containerInfoTmp;
                delete[] typeArr;
                freeargStoreMultiPut($1, $2, $3, $4, alloc);
                SWIG_V8_Raise("Expect row is array");
                SWIG_fail;
            }
            GSResult ret;
            for (int j = 0; j < $2[i]; j++) {
                ret = gsCreateRowByContainer(tmpContainer->getGSContainerPtr(), &$1[i][j]);
                v8::Local<v8::Value> rowArrValue =
                    arr->Get(context, j).ToLocalChecked();
                rowArr = v8::Local<v8::Array>::Cast(rowArrValue);
                if (!rowArr->IsArray()) {
                    delete containerInfoTmp;
                    delete[] typeArr;
                    delete tmpContainer;
                    freeargStoreMultiPut($1, $2, $3, $4, alloc);
                    SWIG_V8_Raise("Expect row is array");
                    SWIG_fail;
                }
                int rowLen = (int) rowArr->Length();
                int k;
                for (k = 0; k < rowLen; k++) {
                    v8::Local<v8::Value> rowValue =
                        rowArr->Get(context, k).ToLocalChecked();
                    if (!(convertToFieldWithType($1[i][j], k, rowValue, typeArr[k]))) {
                        char errorMsg[60];
                        sprintf(errorMsg, "Invalid value for column %d, type should be : %d", k, typeArr[k]);
                        delete tmpContainer;
                        delete containerInfoTmp;
                        delete[] typeArr;
                        freeargStoreMultiPut($1, $2, $3, $4, alloc);
                        SWIG_V8_Raise(errorMsg);
                        SWIG_fail;
                    }
                }
            }
            delete tmpContainer;
            delete[] typeArr;
            delete containerInfoTmp;
        }
    }
}

%typemap(freearg, fragment = "freeargStoreMultiPut") (GSRow*** listRow, const int *listRowContainerCount, const char ** listContainerName, size_t containerCount) {
    freeargStoreMultiPut($1, $2, $3, $4, alloc$argnum);
}

%fragment("freeargStoreMultiPut", "header") {
    //SWIG does not include freearg in fail: label (not like Python, so we need this function)
static void freeargStoreMultiPut(GSRow*** listRow, const int *listRowContainerCount, char ** listContainerName, size_t containerCount, int* alloc) {
    if (listRow) {
        for (int i = 0; i < containerCount; i++) {
            if (listRow[i]) {
                for (int j = 0; j < listRowContainerCount[i]; j++) {
                    gsCloseRow(&listRow[i][j]);
                }
                delete [] listRow[i];
                listRow[i] = NULL;
            }
        }
        delete [] listRow;
        listRow = NULL;
    }

    if (listRowContainerCount) delete [] listRowContainerCount;
    if (listContainerName) {
        for (int i = 0; i < containerCount; i++) {
            if (listContainerName[i]) {
                delete[] listContainerName[i];
                listContainerName[i] = NULL;
            }
        }
        delete[] listContainerName;
    }
    if (alloc) {
        delete[] alloc;
    }
}
}

/**
* Typemaps input for Store.multi_get() function
*/
%typemap(in, fragment = "freeargStoreMultiGet") (const GSRowKeyPredicateEntry *const * predicateList, size_t predicateCount
        , GSContainerRowEntry **entryList, size_t* containerCount, int **colNumList, GSType*** typeList, int **orderFromInput)
        (v8::Local<v8::Object> obj, v8::Local<v8::Array> keys, GSRowKeyPredicateEntry* pList,
        griddb::RowKeyPredicate *vpredicate, int res = 0, size_t size = 0, int* alloc = 0, char* v = 0, 
        GSContainerRowEntry *tmpEntryList, size_t tmpContainerCount, int *tmpcolNumList, GSType** tmpTypeList, int *tmpOrderFromInput) {
    if (!$input->IsObject()) {
        SWIG_V8_Raise("Expected object property as input");
        SWIG_fail;
    }
    v8::Local<v8::Context> context = Nan::GetCurrentContext();
    obj = $input->ToObject(context).ToLocalChecked();
    keys = obj->GetOwnPropertyNames(context).ToLocalChecked();
    $1 = NULL;
    $2 = (int) keys->Length();
    $3 = &tmpEntryList;
    $4 = &tmpContainerCount;
    $5 = &tmpcolNumList;
    $6 = &tmpTypeList;
    $7 = &tmpOrderFromInput;
    if ($2 > 0) {
        try {
            pList = new GSRowKeyPredicateEntry[$2]();
        } catch (std::bad_alloc& ba) {
            SWIG_V8_Raise("Memory allocation error");
            SWIG_fail;
        }
        $1 = &pList;

        try {
            alloc = new int[$2 * 2]();
        } catch (std::bad_alloc& ba) {
            freeargStoreMultiGet($1, $2, NULL, NULL, NULL, NULL, NULL, NULL);
            SWIG_V8_Raise("Memory allocation error");
            SWIG_fail;
        }
        
        for (int i = 0; i < $2; i++) {
            GSRowKeyPredicateEntry *predicateEntry = &pList[i];
            // Get container name
            v8::Local<v8::Value> key = keys->Get(context, i).ToLocalChecked();
            res = SWIG_AsCharPtrAndSize(key, &v, &size, &alloc[i]);
            if (!SWIG_IsOK(res)) {
                freeargStoreMultiGet($1, $2, NULL, NULL, NULL, NULL, NULL, alloc);
                %variable_fail(res, "String", "containerName");
            }
            predicateEntry->containerName = v;

            // Get predicate
            v8::Local<v8::Value> value = obj->Get(context, key).ToLocalChecked();
            res = SWIG_ConvertPtr(value, (void**)&vpredicate, $descriptor(griddb::RowKeyPredicate*), 0);
            if (!SWIG_IsOK(res)) {
                freeargStoreMultiGet($1, $2, NULL, NULL, NULL, NULL, NULL, alloc);
                SWIG_V8_Raise("Convert RowKeyPredicate pointer failed");
                SWIG_fail;
            }
            predicateEntry->predicate = vpredicate->gs_ptr();
        }
    } else {
        SWIG_V8_Raise("Input should not be empty object");
        SWIG_fail;
    }
}

%typemap(argout, fragment = "getRowFields", fragment = "freeargStoreMultiGet") 
        (const GSRowKeyPredicateEntry *const * predicateList, size_t predicateCount, 
        GSContainerRowEntry **entryList, size_t* containerCount, int **colNumList, GSType*** typeList, int **orderFromInput) 
        (v8::Local<v8::Object> obj, v8::Local<v8::Array> arr, v8::Local<v8::Array> rowArr, 
        v8::Handle<v8::String> key, v8::Handle<v8::Value> value, GSRow* row) {
    obj = SWIGV8_OBJECT_NEW();
    int numContainer = (int) *$4;
    bool retVal;
    int errorColumn;
    GSType errorType;
    v8::Local<v8::Context> context = Nan::GetCurrentContext();
    for (int i = 0; i < numContainer; i++) {
        key = SWIGV8_STRING_NEW2((*$3)[i].containerName, strlen((char*)(*$3)[i].containerName));
        arr = SWIGV8_ARRAY_NEW();
        for (int j = 0; j < (*$3)[i].rowCount; j++) {
            row = (GSRow*)(*$3)[i].rowList[j];
            rowArr = SWIGV8_ARRAY_NEW();
            retVal = getRowFields(row, (*$5)[i], (*$6)[i], arg1->timestamp_output_with_float, &errorColumn, &errorType, rowArr);
            if (retVal == false) {
                freeargStoreMultiGet($1, $2, $3, $4, $5, $6, $7, alloc$argnum);
                char errorMsg[60];
                sprintf(errorMsg, "Can't get data for field %d with type %d", errorColumn, errorType);
                SWIG_V8_Raise(errorMsg);
                SWIG_fail;
            }
            arr->Set(context, j, rowArr);
        }
        obj->Set(context, key, arr);
    }
    $result = obj;
}

%typemap(freearg, fragment = "freeargStoreMultiGet") (const GSRowKeyPredicateEntry *const * predicateList, size_t predicateCount ,
        GSContainerRowEntry **entryList, size_t* containerCount, int **colNumList, GSType*** typeList, int **orderFromInput) {
    freeargStoreMultiGet($1, $2, $3, $4, $5, $6, $7, alloc$argnum);
}

%fragment("freeargStoreMultiGet", "header") {
    //SWIG does not include freearg in fail: label (not like Python, so we need this function)
static void freeargStoreMultiGet(const GSRowKeyPredicateEntry *const * predicateList, size_t predicateCount, 
        GSContainerRowEntry **entryList, size_t* containerCount, int **colNumList, GSType*** typeList, int **orderFromInput, int* alloc) {
    int i;
    GSRowKeyPredicateEntry* pList;
    if (predicateList && *predicateList) {
        pList = (GSRowKeyPredicateEntry*) *predicateList;
        for (i = 0; i < predicateCount; i++) {
            cleanString((*predicateList)[i].containerName, alloc[i]);
        }
        if (pList) {
            delete[] pList;
            pList = NULL;
        }
    }
    if (alloc) {
        delete[] alloc;
        alloc = NULL;
    }

    if (colNumList && *colNumList) {
        delete [] *colNumList;
        *colNumList = NULL;
    }
    if (typeList && *typeList) {
        for (int j = 0; j < (int) predicateCount; j++) {
            if ((*typeList)[j]) {
                delete[] (*typeList)[j];
            }
        }
        delete [] (*typeList);
        *typeList = NULL;
    }
    if (entryList) {
        GSRow* row;
        for (int i = 0; i < *containerCount; i++) {
            for (int j = 0; j < (*entryList)[i].rowCount; j++) {
                row = (GSRow*)(*entryList)[i].rowList[j];
                gsCloseRow(&row);
            }
        }
        entryList = NULL;
    }
    if (orderFromInput && *orderFromInput) {
        delete [] *orderFromInput;
        *orderFromInput = NULL;
    }
}
}

/**
 * Create typemap for RowKeyPredicate.set_range
 */
%typemap(in, fragment = "convertToRowKeyFieldWithType") (griddb::Field* startKey)(griddb::Field field) {
    $1 = &field;
    GSType type = arg1->get_key_type();
    if (!(convertToRowKeyFieldWithType(*$1, $input, type))) {
        SWIG_V8_Raise("Can not create row based on input");
        SWIG_fail;
    }
}

%typemap(in, fragment = "convertToRowKeyFieldWithType") (griddb::Field* finishKey)(griddb::Field field) {
    $1 = &field;
    GSType type = arg1->get_key_type();
    if (!(convertToRowKeyFieldWithType(*$1, $input, type))) {
        SWIG_V8_Raise("Can not create row based on input");
        SWIG_fail;
    }
}

/**
 * Typemap for RowKeyPredicate.get_range
 */
%typemap(in, numinputs = 0) (griddb::Field* startField, griddb::Field* finishField) (griddb::Field startKeyTmp, griddb::Field finishKeyTmp) {
    $1 = &startKeyTmp;
    $2 = &finishKeyTmp;
}

%typemap(argout, fragment = "convertFieldToObject") (griddb::Field* startField, griddb::Field* finishField) {
    v8::Local<v8::Context> context = Nan::GetCurrentContext();
    v8::Local<v8::Array> arr = SWIGV8_ARRAY_NEW();
    arr->Set(context, 0, convertFieldToObject(
        &$1->value, $1->type, arg1->timestamp_output_with_float));
    arr->Set(context, 1, convertFieldToObject(
        &$2->value, $2->type, arg1->timestamp_output_with_float));
    $result = arr;
}

/**
 * Typemap for RowKeyPredicate.set_distinct_keys
 */
%typemap(in, fragment = "convertToRowKeyFieldWithType") (const griddb::Field *keys, size_t keyCount) {
    if (!$input->IsArray()) {
        SWIG_V8_Raise("Expected array as input");
        SWIG_fail;
    }
    v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast($input);
    $2 = (int)arr->Length();
    $1 = NULL;

    v8::Local<v8::Context> context = Nan::GetCurrentContext();
    if ($2 > 0) {
        try {
            $1 = new griddb::Field[$2]();
        } catch (std::bad_alloc& ba) {
            SWIG_V8_Raise("Memory allocation error");
            SWIG_fail;
        }
        GSType type = arg1->get_key_type();
        for (int i = 0; i < $2; i++) {
            v8::Local<v8::Value> value = arr->Get(context, i).ToLocalChecked();
            if (!(convertToRowKeyFieldWithType($1[i], value, type))) {
                delete [] $1;
                $1 = NULL;
                SWIG_V8_Raise("Can not create row based on input");
                SWIG_fail;
            }
        }
    }
}

%typemap(freearg) (const griddb::Field *keys, size_t keyCount) {
    if ($1) {
        delete [] $1;
    }
}


/**
* Typemaps output for RowKeyPredicate.get_distinct_keys
*/
%typemap(in, numinputs = 0) (griddb::Field **keys, size_t* keyCount) (griddb::Field *keys1, size_t keyCount1) {
    $1 = &keys1;
    $2 = &keyCount1;
}

%typemap(argout, numinputs = 0, fragment = "convertFieldToObject") (griddb::Field **keys, size_t* keyCount) {
    v8::Local<v8::Context> context = Nan::GetCurrentContext();
    v8::Local<v8::Array> obj = SWIGV8_ARRAY_NEW();
    for (int i = 0; i < keyCount1$argnum; i++) {
        v8::Handle<v8::Value> value = convertFieldToObject(&keys1$argnum[i].value, keys1$argnum[i].type, arg1->timestamp_output_with_float);
        obj->Set(context, i, value);
    }
    $result = obj;
}

%typemap(freearg) (griddb::Field **keys, size_t* keyCount) {
    if ($1) {
        delete [] (*$1);
    }
}

/**
 * Typemap for Container::multi_put
 */
%typemap(in, fragment = "convertToFieldWithType", fragment = "freeargContainerMultiPut") (GSRow** listRowdata, int rowCount) {
    if (!$input->IsArray()) {
        SWIG_V8_Raise("Expected array of array as input");
        SWIG_fail;
    }

    v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast($input);
    $2 = (size_t)arr->Length();

    v8::Local<v8::Context> context = Nan::GetCurrentContext();
    for (int i = 0; i < $2; i++) {
        if (!arr->Get(context, i).ToLocalChecked()->IsArray()) {
            SWIG_V8_Raise("Expected array of array as input");
            SWIG_fail;
        }
    }

    if ($2 > 0) {
        GSContainer *mContainer = arg1->getGSContainerPtr();
        GSType* typeList = arg1->getGSTypeList();
        try {
            $1 = new GSRow*[$2]();
        } catch (std::bad_alloc& ba) {
            SWIG_V8_Raise("Memory allocation error");
            SWIG_fail;
        }
        int length;
        for (int i = 0; i < $2; i++) {
            v8::Local<v8::Value> fieldArrValue =
                arr->Get(context, i).ToLocalChecked();
            v8::Local<v8::Array> fieldArr = v8::Local<v8::Array>::Cast(fieldArrValue);
            length = (int)fieldArr->Length();
            if (length != arg1->getColumnCount()) {
                freeargContainerMultiPut($1, i);
                SWIG_V8_Raise("Num row is different with container info");
                SWIG_fail;
            }
            GSResult ret = gsCreateRowByContainer(mContainer, &$1[i]);
            if (!GS_SUCCEEDED(ret)) {
                freeargContainerMultiPut($1, i);
                SWIG_V8_Raise("Can't create GSRow");
                SWIG_fail;
            }
            for (int k = 0; k < length; k++) {
                GSType type = typeList[k];
                v8::Local<v8::Value> fieldValue = fieldArr->Get(context, k).
                                                  ToLocalChecked();
                if (!(convertToFieldWithType($1[i], k, fieldValue, type))) {
                    char errorMsg[200];
                    sprintf(errorMsg, "Invalid value for row %d, column %d, type should be : %d", i, k, type);
                    freeargContainerMultiPut($1, i + 1);
                    SWIG_V8_Raise(errorMsg);
                    SWIG_fail;
                }
            }
        }
    }
}

%typemap(freearg, fragment = "freeargContainerMultiPut") (GSRow** listRowdata, int rowCount) {
    freeargContainerMultiPut($1, $2);
}

%fragment("freeargContainerMultiPut", "header") {
    //SWIG does not include freearg in fail: label (not like Python, so we need this function)
static void freeargContainerMultiPut(GSRow** listRowdata, int rowCount) {
    if (listRowdata) {
        for (int rowNum = 0; rowNum < rowCount; rowNum++) {
            gsCloseRow(&listRowdata[rowNum]);
        }
        delete [] listRowdata;
    }
}
}

/**
 * Typemap for QueryAnalysisEntry.get()
 */
%typemap(in, numinputs = 0) (GSQueryAnalysisEntry* queryAnalysis) (GSQueryAnalysisEntry queryAnalysis1) {
    queryAnalysis1 = GS_QUERY_ANALYSIS_ENTRY_INITIALIZER;
    $1 = &queryAnalysis1;
}

%typemap(argout) (GSQueryAnalysisEntry* queryAnalysis) {
    v8::Local<v8::Context> context = Nan::GetCurrentContext();
    v8::Local<v8::Array> obj = SWIGV8_ARRAY_NEW();
    obj->Set(context, 0, SWIGV8_INTEGER_NEW($1->id));
    obj->Set(context, 1, SWIGV8_INTEGER_NEW($1->depth));
    v8::Handle<v8::String> str = SWIGV8_STRING_NEW2($1->type, strlen((char*)$1->type));
    obj->Set(context, 2, str);
    str = SWIGV8_STRING_NEW2($1->valueType, strlen((char*)$1->valueType));
    obj->Set(context, 3, str);
    str = SWIGV8_STRING_NEW2($1->value, strlen((char*)$1->value));
    obj->Set(context, 4, str);
    str = SWIGV8_STRING_NEW2($1->statement, strlen((char*)$1->statement));
    obj->Set(context, 5, str);

    $result = obj;
}

%typemap(freearg) (GSQueryAnalysisEntry* queryAnalysis) {
    if ($1) {
        if ($1->statement) {
            delete [] $1->statement;
        }
        if ($1->type) {
            delete [] $1->type;
        }
        if ($1->value) {
            delete [] $1->value;
        }
        if ($1->valueType) {
            delete [] $1->valueType;
        }
    }
}

/**
 * Typemap for Rowset::next()
 */
%typemap(in, numinputs = 0) (GSRowSetType* type, bool* hasNextRow,
        griddb::QueryAnalysisEntry** queryAnalysis, griddb::AggregationResult** aggResult)
    (GSRowSetType typeTmp, bool hasNextRowTmp,
    griddb::QueryAnalysisEntry* queryAnalysisTmp, griddb::AggregationResult* aggResultTmp) {
    $1 = &typeTmp;
    hasNextRowTmp = true;
    $2 = &hasNextRowTmp;
    $3 = &queryAnalysisTmp;
    $4 = &aggResultTmp;
}

%typemap(argout) (GSRowSetType* type, bool* hasNextRow,
        griddb::QueryAnalysisEntry** queryAnalysis, griddb::AggregationResult** aggResult) 
    (v8::Local<v8::Array> obj) {
    GSRow* row;

    if (hasNextRowTmp$argnum == false) {
        $result = SWIGV8_NULL();
    } else {
        switch (typeTmp$argnum) {
            case GS_ROW_SET_CONTAINER_ROWS: {
                bool retVal;
                int errorColumn;
                GSType errorType;
                row = arg1->getGSRowPtr();
                obj = SWIGV8_ARRAY_NEW();
                if (obj->IsNull()) {
                    SWIG_V8_Raise("Memory allocation error");
                    SWIG_fail;
                }
                retVal = getRowFields(row, arg1->getColumnCount(), arg1->getGSTypeList(), arg1->timestamp_output_with_float, &errorColumn, &errorType, obj);
                if (retVal == false) {
                    char errorMsg[60];
                    sprintf(errorMsg, "Can't get data for field %d with type %d", errorColumn, errorType);
                    SWIG_V8_Raise(errorMsg);
                    SWIG_fail;
                }
                $result = obj;
                break;
            }
            case GS_ROW_SET_AGGREGATION_RESULT:
                $result = SWIG_V8_NewPointerObj((void *)aggResultTmp$argnum, $descriptor(griddb::AggregationResult *), SWIG_POINTER_OWN);
                break;
            case GS_ROW_SET_QUERY_ANALYSIS:
                $result = SWIG_V8_NewPointerObj((void *)queryAnalysisTmp$argnum, $descriptor(griddb::QueryAnalysisEntry *), SWIG_POINTER_OWN);
                break;
            default:
                SWIG_V8_Raise("Invalid Rowset type");
                SWIG_fail;
                break;
        }
    }
}

//attribute ContainerInfo::columnInfoList
%typemap(in, fragment = "freeargColumnInfoList") (ColumnInfoList*) 
        (v8::Local<v8::Array> arr, v8::Local<v8::Array> colInfo, v8::Local<v8::Array> keys, size_t sizeTmp = 0, int* alloc = 0, int res, char* v = 0, ColumnInfoList infolist) {

    if (!$input->IsArray()) {
        SWIG_V8_Raise("Expected array as input");
        SWIG_fail;
    }
    v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast($input);
    size_t len = (size_t)arr->Length();
    GSColumnInfo* containerInfo;
    $1 = &infolist;
    v8::Local<v8::Context> context = Nan::GetCurrentContext();
    if (len) {
        try {
            containerInfo = new GSColumnInfo[len]();
        } catch (std::bad_alloc& ba) {
            SWIG_V8_Raise("Memory allocation error");
            SWIG_fail;
        }

        try {
            alloc = new int[len]();
        } catch (std::bad_alloc& ba) {
            delete[] containerInfo;
            SWIG_V8_Raise("Memory allocation error");
            SWIG_fail;
        }
        $1->columnInfo = containerInfo;
        $1->size = len;

        for (int i = 0; i < len; i++) {
            v8::Local<v8::Value> colInfoKey = arr->Get(context, i).ToLocalChecked();
            if (!colInfoKey->IsArray()) {
                freeargColumnInfoList($1, alloc);
                SWIG_V8_Raise("Expected array property as ColumnInfo element");
                SWIG_fail;
            }

            colInfo = v8::Local<v8::Array>::Cast(colInfoKey);
            if (colInfo->Length() < 2) {
                freeargColumnInfoList($1, alloc);
                SWIG_V8_Raise("Expected at least two elements for ColumnInfo property");
                SWIG_fail;
            }
            v8::Local<v8::Value> key = colInfo->Get(context, 0).ToLocalChecked();
            v8::Local<v8::Value> value = colInfo->Get(context, 1).ToLocalChecked();

            res = SWIG_AsCharPtrAndSize(key, &v, &sizeTmp, &alloc[i]);
            if (!SWIG_IsOK(res)) {
                freeargColumnInfoList($1, alloc);
                %variable_fail(res, "String", "Column name");
            }

            if (!value->IsNumber()) {
                cleanString(v, alloc[i]);
                freeargColumnInfoList($1, alloc);
                SWIG_V8_Raise("Expected Integer as type of Column type");
                SWIG_fail;
            }

            griddb::Util::strdup((const GSChar**)&(containerInfo[i].name), v);
            cleanString(v, alloc[i]);
            v8::Local<v8::Context> context = Nan::GetCurrentContext();
            containerInfo[i].type = value->Uint32Value(context).FromJust();

            if (colInfo->Length() == 3) {
                v8::Local<v8::Value> options = colInfo->Get(context, 2).
                                                   ToLocalChecked();

                if (!options->IsUint32()) {
                    freeargColumnInfoList($1, alloc);
                    SWIG_V8_Raise("Expected Integer as type of Column options");
                    SWIG_fail;
                }

                containerInfo[i].options = options->Uint32Value(context).FromJust();
            }

            if (colInfo->Length() > 3) {
                freeargColumnInfoList($1, alloc);
                SWIG_V8_Raise("Expected at most 3 elements for ColumnInfo property");
                SWIG_fail;
            }
        }

    }
}

%typemap(freearg, fragment = "cleanString", fragment = "freeargColumnInfoList") (ColumnInfoList*) {
    freeargColumnInfoList($1, alloc$argnum);
}

%fragment("freeargColumnInfoList", "header", fragment = "cleanString") {
    //SWIG does not include freearg in fail: label (not like Python, so we need this function)
static void freeargColumnInfoList(ColumnInfoList* infoList, int* alloc) {
    size_t len = infoList->size;
    if (infoList->columnInfo) {
        if (alloc) {
            for (int i = 0; i < len; i++) {
                free((void*)infoList->columnInfo[i].name);
            }
            delete[] alloc;
        }
        delete[] infoList->columnInfo;
    }
}
}

%typemap(out) (ColumnInfoList*) {
    v8::Local<v8::Array> obj;
    size_t len = $1->size;
    v8::Local<v8::Context> context = Nan::GetCurrentContext();
    if (len > 0) {
        obj = SWIGV8_ARRAY_NEW();
        if (obj->IsNull()) {
            SWIG_V8_Raise("Memory allocation error");
            SWIG_fail;
        }
        for (int i = 0; i < len; i++) {
            v8::Local<v8::Array> prop;
            prop = SWIGV8_ARRAY_NEW();
            v8::Handle<v8::String> str = SWIGV8_STRING_NEW2($1->columnInfo[i].name, strlen((char*)$1->columnInfo[i].name));
            prop->Set(context, 0, str);
            prop->Set(context, 1, SWIGV8_NUMBER_NEW($1->columnInfo[i].type));
            prop->Set(context, 2, SWIGV8_NUMBER_NEW($1->columnInfo[i].options));
            obj->Set(context, i, prop);
        }
    }
    $result = obj;
    delete $1;
}

/**
* Typemaps for create_index()/ drop_index function : support keyword parameter ({"columnName" : str, "indexType" : int, "name" : str})
*/
%typemap(in, fragment = "SWIG_AsCharPtrAndSize", fragment = "cleanString" , fragment = "freeargContainerIndex") (const char* column_name, GSIndexTypeFlags index_type, const char* name) 
        (v8::Local<v8::Object> obj, v8::Local<v8::Array> keys, int i, int j, size_t size = 0,size_t size1 = 0, int* alloc = 0, int res,  char* v = 0) {
    char* name;
    if (!$input->IsObject()) {
        SWIG_V8_Raise("Expected object property as input");
        SWIG_fail;
    }
    v8::Local<v8::Context> context = Nan::GetCurrentContext();
    obj = $input->ToObject(context).ToLocalChecked();
    keys = obj->GetOwnPropertyNames(Nan::GetCurrentContext()).ToLocalChecked();
    int len = (int) keys->Length();
    //Create $1, $2, $3 with default value
    $1 = NULL;
    $2 = GS_INDEX_FLAG_DEFAULT;
    $3 = NULL;
    int allocKey;
    int allocValue;
    char errorMsg[60];
    if (len > 0) {
        for (int i = 0; i < len; i++) {
            v8::Local<v8::Value> key = keys->Get(context, i).ToLocalChecked();
            res = SWIG_AsCharPtrAndSize(key, &name, &size, &allocKey);
            if (!SWIG_IsOK(res)) {
                freeargContainerIndex($1, $3);
                %variable_fail(res, "String", "name");
            }
            v8::Local<v8::Value> value = obj->Get(context, key).ToLocalChecked();
            if (strcmp(name, "columnName") == 0 || strcmp(name, "name") == 0) {
                if (!value->IsString()) {
                    sprintf(errorMsg, "Invalid value for property %s", name);
                    cleanString(name, allocKey);
                    freeargContainerIndex($1, $3);
                    SWIG_V8_Raise(errorMsg);
                    SWIG_fail;
                }
                res = SWIG_AsCharPtrAndSize(value, &v, &size1, &allocValue);
                if (!SWIG_IsOK(res)) {
                    freeargContainerIndex($1, $3);
                    %variable_fail(res, "String", "value");
                }
                if (v) {
                    if (strcmp(name, "columnName") == 0 ) {
                        griddb::Util::strdup((const GSChar**)&$1, v);
                    } else {
                        griddb::Util::strdup((const GSChar**)&$3, v);
                    }
                    cleanString(v, allocValue);
                }
            } else if (strcmp(name, "indexType") == 0) {
                if (!value->IsInt32()) {
                    freeargContainerIndex($1, $3);
                    sprintf(errorMsg, "Invalid value for property %s", name);
                    cleanString(name, allocKey);
                    SWIG_V8_Raise(errorMsg);
                    SWIG_fail;
                }
                $2 = value->IntegerValue(context).FromJust();
            } else {
                sprintf(errorMsg, "Invalid property %s", name);
                cleanString(name, allocKey);
                freeargContainerIndex($1, $3);
                SWIG_V8_Raise(errorMsg);
                SWIG_fail;
            }
            cleanString(name, allocKey);
        }
    }
}

%typemap(freearg, fragment = "freeargContainerIndex") (const char* column_name, GSIndexTypeFlags index_type, const char* name) {
    freeargContainerIndex($1, $3);
}

%fragment("freeargContainerIndex", "header") {
    //SWIG does not include freearg in fail: label (not like Python, so we need this function)
static void freeargContainerIndex(const char* column_name, const char* name) {
    if (column_name) {
        delete[] column_name;
    }
    if (name) {
        delete[] name;
    }
}
}

/**
* Typemaps for set_fetch_options() : support keyword parameter ({"limit" : int, "partial" : bool})
*/
%typemap(in, fragment = "SWIG_AsCharPtrAndSize") (int limit, bool partial) 
        (v8::Local<v8::Object> obj, v8::Local<v8::Array> keys, int i, int j, size_t size = 0, int* alloc = 0, int res) {
    char* name;
    if (!$input->IsObject()) {
        SWIG_V8_Raise("Expected object property as input");
        SWIG_fail;
    }
    v8::Local<v8::Context> context = Nan::GetCurrentContext();
    obj = $input->ToObject(context).ToLocalChecked();
    keys = obj->GetOwnPropertyNames(Nan::GetCurrentContext()).ToLocalChecked();
    int len = (int) keys->Length();
    //Create $1, $2 with default value
    $1 = -1;
    $2 = false;
    int allocKey;
    char errorMsg[60];
    if (len > 0) {
        for (int i = 0; i < len; i++) {
            v8::Local<v8::Value> key = keys->Get(context, i).ToLocalChecked();
            res = SWIG_AsCharPtrAndSize(key, &name, &size, &allocKey);
            if (!SWIG_IsOK(res)) {
                %variable_fail(res, "String", "name");
            }
            if (strcmp(name, "limit") == 0) {
                v8::Local<v8::Value> value = obj->Get(context, key).
                                                 ToLocalChecked();
                if (!value->IsInt32()) {
                    sprintf(errorMsg, "Invalid value for property %s", name);
                    cleanString(name, allocKey);
                    SWIG_V8_Raise(errorMsg);
                    SWIG_fail;
                }
                $1 = value->IntegerValue(context).FromJust();
            } else if (strcmp(name, "partial") == 0) {
                cleanString(name, allocKey);
                //Throw error when parse partial parameter
                SWIG_V8_Raise("Not support partial parameter");
                SWIG_fail;
            } else {
                sprintf(errorMsg, "Invalid property %s", name);
                cleanString(name, allocKey);
                SWIG_V8_Raise(errorMsg);
                SWIG_fail;
            }
            cleanString(name, allocKey);
        }
    }
}

/**
* Typemaps for ContainerInfo : support keyword parameter ({"name" : str, "columnInfoList" : array, "type" : str, 'rowKey':boolean})
*/
%typemap(in, fragment = "SWIG_AsCharPtrAndSize", fragment = "cleanString", fragment = "freeargContainerInfo") (const GSChar* name, const GSColumnInfo* props, 
        int propsCount, GSContainerType type, bool row_key, griddb::ExpirationInfo* expiration)
        (v8::Local<v8::Object> obj, v8::Local<v8::Array> keys, int i, int j, size_t size = 0,size_t size1 = 0, int* alloc = 0, int res,  char* v = 0) {
    char* name;
    if (!$input->IsObject()) {
        SWIG_V8_Raise("Expected object property as input");
        SWIG_fail;
    }
    obj = $input->ToObject(Nan::GetCurrentContext()).ToLocalChecked();
    keys = obj->GetOwnPropertyNames(Nan::GetCurrentContext()).ToLocalChecked();
    int len = (int) keys->Length();
    //Create $1, $2, $3, $3, $4, $5, $6 with default value
    $1 = NULL;
    $2 = NULL;
    $3 = 0;
    $4 = GS_CONTAINER_COLLECTION;
    $5 = true; //defautl value rowKey = true
    $6 = NULL;
    int allocKey;
    int allocValue;
    char errorMsg[60];
    v8::Local<v8::Array> arr;
    v8::Local<v8::Array> colInfo;
    bool boolVal, vbool;
    griddb::ExpirationInfo* expiration;
    v8::Local<v8::Context> context = Nan::GetCurrentContext();
    if (len > 0) {
        for (int i = 0; i < len; i++) {
            v8::Local<v8::Value> key = keys->Get(context, i).ToLocalChecked();
            res = SWIG_AsCharPtrAndSize(key, &name, &size, &allocKey);
            if (!SWIG_IsOK(res)) {
                freeargContainerInfo($1, $2, $3, alloc);
                %variable_fail(res, "String", "name");
            }

            v8::Local<v8::Value> value = obj->Get(context, key).ToLocalChecked();
            if (strcmp(name, "name") == 0) {
                if (!value->IsString()) {
                    freeargContainerInfo($1, $2, $3, alloc);
                    sprintf(errorMsg, "Invalid value for property %s", name);
                    SWIG_V8_Raise(errorMsg);
                    cleanString(name, allocKey);
                    SWIG_fail;
                }
                res = SWIG_AsCharPtrAndSize(value, &v, &size1, &allocValue);
                if (!SWIG_IsOK(res)) {
                    freeargContainerInfo($1, $2, $3, alloc);
                    sprintf(errorMsg, "Memory allocation error for property %s", name);
                    SWIG_V8_Raise(errorMsg);
                    cleanString(name, allocKey);
                    SWIG_fail;
                }
                if (v) {
                    delete[] $1;
                    griddb::Util::strdup((const GSChar**)&$1, v);
                    cleanString(v, allocValue);
                }
            } else if (strcmp(name, "columnInfoList") == 0) {
                if (!value->IsArray()) {
                    freeargContainerInfo($1, $2, $3, alloc);
                    sprintf(errorMsg, "Expected array as input for property %s", name);
                    SWIG_V8_Raise(errorMsg);
                    cleanString(name, allocKey);
                    SWIG_fail;
                }
                arr = v8::Local<v8::Array>::Cast(value);
                $3 = (int) arr->Length();
                    if ($3 > 0) {
                        try {
                            $2 = new GSColumnInfo[$3]();
                        } catch (std::bad_alloc& ba) {
                             freeargContainerInfo($1, $2, $3, alloc);
                             cleanString(name, allocKey);
                             SWIG_V8_Raise("Memory allocation error");
                             SWIG_fail;
                        }

                        try {
                            alloc = new int[$3]();
                        } catch (std::bad_alloc& ba) {
                            freeargContainerInfo($1, $2, $3, alloc);
                            cleanString(name, allocKey);
                            SWIG_V8_Raise("Memory allocation error");
                            SWIG_fail;
                        }

                        for (int j = 0; j < $3; j++) {
                            v8::Local<v8::Value> containerInfoKey = arr->Get(context, j).
                                                                        ToLocalChecked();
                            if (!containerInfoKey->IsArray()) {
                                freeargContainerInfo($1, $2, $3, alloc);
                                cleanString(name, allocKey);
                                SWIG_V8_Raise("Expected array property as ColumnInfo element");
                                SWIG_fail;
                            }
                            colInfo = v8::Local<v8::Array>::Cast(containerInfoKey);
                            if ((int)colInfo->Length() < 2 || (int)colInfo->Length() > 3) {
                                freeargContainerInfo($1, $2, $3, alloc);
                                cleanString(name, allocKey);
                                SWIG_V8_Raise("Expected 2 or 3 elements for ColumnInfo property");
                                SWIG_fail;
                            }

                            res = SWIG_AsCharPtrAndSize(colInfo->Get(context, 0).ToLocalChecked(),
                                &v, &size, &alloc[j]);
                            if (!SWIG_IsOK(res)) {
                                freeargContainerInfo($1, $2, $3, alloc);
                                cleanString(name, allocKey);
                                %variable_fail(res, "String", "Column name");
                            }

                            if (!colInfo->Get(context, 1).ToLocalChecked()->IsInt32()) {
                                cleanString(v, alloc[j]);
                                freeargContainerInfo($1, $2, $3, alloc);
                                cleanString(name, allocKey);
                                SWIG_V8_Raise("Expected Integer as type of Column type");
                                SWIG_fail;
                            }
                            $2[j].name = v;
                            $2[j].type = (int) colInfo->Get(context, 1).
                                              ToLocalChecked()->Uint32Value(context).FromJust();

                            if ((int)colInfo->Length() == 3) {
                                v8::Local<v8::Value> options = colInfo->Get(context, 2).
                                                                   ToLocalChecked();
                                if (!options->IsInt32()) {
                                    freeargContainerInfo($1, $2, $3, alloc);
                                    cleanString(name, allocKey);
                                    SWIG_V8_Raise("Expected Integer as type of Column options");
                                    SWIG_fail;
                                }
                                $2[j].options = (int) options->Uint32Value(context).FromJust();
                            }
                        }
                    }
            } else if (strcmp(name, "type") == 0) {
                if (!value->IsInt32()) {
                    freeargContainerInfo($1, $2, $3, alloc);
                    sprintf(errorMsg, "Invalid value for property %s", name);
                    SWIG_V8_Raise(errorMsg);
                    cleanString(name, allocKey);
                    SWIG_fail;
                }
                $4 = value->IntegerValue(context).FromJust();
            } else if (strcmp(name, "rowKey") == 0) {
                vbool = convertObjectToBool(value, &boolVal);
                if (!vbool) {
                    freeargContainerInfo($1, $2, $3, alloc);
                    sprintf(errorMsg, "Invalid value for property %s", name);
                    SWIG_V8_Raise(errorMsg);
                    cleanString(name, allocKey);
                    SWIG_fail;
                }
                $5 = boolVal;
            } else if (strcmp(name, "expiration") == 0) {
                 res = SWIG_ConvertPtr(value, (void**)&expiration, $descriptor(griddb::ExpirationInfo*), 0 | 0 );
                 if (!SWIG_IsOK(res)) {
                     freeargContainerInfo($1, $2, $3, alloc);
                     sprintf(errorMsg, "Invalid value for property %s", name);
                     SWIG_V8_Raise(errorMsg);
                     cleanString(name, allocKey);
                     SWIG_fail; 
                 }
                 $6 = (griddb::ExpirationInfo *) expiration;
            } else {
                freeargContainerInfo($1, $2, $3, alloc);
                cleanString(name, allocKey);
                SWIG_V8_Raise(errorMsg);
                SWIG_fail;
            }
            cleanString(name, allocKey);
        }
    }
}

%typemap(freearg, fragment = "freeargContainerInfo") (const GSChar* name, const GSColumnInfo* props,
        int propsCount, GSContainerType type, bool row_key, griddb::ExpirationInfo* expiration) {
    freeargContainerInfo($1, $2, $3, alloc$argnum);
}

%fragment("freeargContainerInfo", "header", fragment = "cleanString") {
    //SWIG does not include freearg in fail: label (not like Python, so we need this function)
static void freeargContainerInfo(const GSChar* name, const GSColumnInfo* props,
        int propsCount, int* alloc) {
    if (name) {
        delete[] name;
    }
    if (props) {
        for (int i = 0; i < propsCount; i++) {
            cleanString(props[i].name, alloc[i]);
        }
        delete[] props;
    }

    if (alloc) {
        delete[] alloc;
    }
}
}

/**
 * Support close method : Store.close()
 */
%typemap(in, fragment = "convertObjectToBool") GSBool allRelated{
    bool boolVal;
    v8::Local<v8::Value> obj = $input;
    bool vbool = convertObjectToBool(obj, &boolVal);
    if (!vbool) {
        SWIG_V8_Raise("Type should be bool value");
        SWIG_fail;
    }
    $1 = (boolVal == true) ? GS_TRUE : GS_FALSE;
}

/*
 * Correct PartitionController.getContainerNames() function
 */
%extend griddb::PartitionController {
    void get_container_names(int32_t partition_index, int64_t start, int64_t limit = -1,
            const GSChar * const ** stringList = NULL, size_t *size = NULL){
        return $self->get_container_names(partition_index, start, stringList, size, limit);
    }
}

//Correct check for input integer: should check range and type
%fragment("convertObjectToInt", "header", fragment = "SWIG_AsVal_bool", fragment = "SWIG_AsVal_int") {
static bool convertObjectToInt(v8::Local<v8::Value> value, int* intValPtr) {
    if (!intValPtr) {
        return false;
    }
    if (!value->IsInt32()) {
        return false;
    }
    *intValPtr = value->IntegerValue(Nan::GetCurrentContext()).FromJust();
    return true;
}
}

%typemap(in, fragment = "convertObjectToInt") (int) {
    v8::Local<v8::Value> obj = $input;
    bool checkConvert = convertObjectToInt(obj, &$1);
    if (!checkConvert) {
        SWIG_V8_Raise("Type should be integer value");
        SWIG_fail;
    }
}

//Correct check for input size_t: should be integer, not float
%fragment("convertObjectToSizeT", "header", fragment = "SWIG_AsVal_size_t") {
static bool convertObjectToSizeT(v8::Local<v8::Value> value, size_t* intValPtr) {
    if (!intValPtr) {
        return false;
    }
    int checkConvert = SWIG_AsVal_size_t(value, intValPtr);
    if (!SWIG_IsOK(checkConvert)) {
        return false;
    }
    if (value->NumberValue(Nan::GetCurrentContext()).FromJust() != *intValPtr) {
        return false;
    }
    return true;
}
}

%typemap(in, fragment = "convertObjectToSizeT") (size_t) {
    v8::Local<v8::Value> obj = $input;

    bool vbool = convertObjectToSizeT(obj, &$1);
    if (!vbool) {
        SWIG_V8_Raise("Type should be unsigned integer value");
        SWIG_fail;
    }
}

