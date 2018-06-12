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

#if defined(SWIGPYTHON)
%include "gstype_python.i"
%module griddb_python
%begin %{
#define SWIG_PYTHON_2_UNICODE
%}
#elif defined(SWIGRUBY)
%include "gstype_ruby.i"
%module griddb_ruby_client
#elif defined(SWIGPHP)
%module griddb_php_client
#elif defined(SWIGJAVASCRIPT)
%module griddb_js_client
#endif

%include <stdint.i>
%include <std_string.i>
%include <std_map.i>
%include <std_except.i>
#if defined(SWIGPYTHON)
%include <std_shared_ptr.i>
#endif

%include <typemaps.i>
%catches(griddb::GSException);

#if !defined(SWIGGO)
enum GSContainerTypeTag {
    GS_CONTAINER_COLLECTION,
    GS_CONTAINER_TIME_SERIES,
};
#endif

enum GSRowSetTypeTag {
    GS_ROW_SET_CONTAINER_ROWS,
    GS_ROW_SET_AGGREGATION_RESULT,
    GS_ROW_SET_QUERY_ANALYSIS
};

#if !defined(SWIGGO)
// Enum GSTypeTag
enum GSTypeTag {
    GS_TYPE_STRING,
    GS_TYPE_BOOL,
    GS_TYPE_BYTE,
    GS_TYPE_SHORT,
    GS_TYPE_INTEGER,
    GS_TYPE_LONG,
    GS_TYPE_FLOAT,
    GS_TYPE_DOUBLE,
    GS_TYPE_TIMESTAMP,
    GS_TYPE_GEOMETRY,
    GS_TYPE_BLOB,
    GS_TYPE_STRING_ARRAY,
    GS_TYPE_BOOL_ARRAY,
    GS_TYPE_BYTE_ARRAY,
    GS_TYPE_SHORT_ARRAY,
    GS_TYPE_INTEGER_ARRAY,
    GS_TYPE_LONG_ARRAY,
    GS_TYPE_FLOAT_ARRAY,
    GS_TYPE_DOUBLE_ARRAY,
    GS_TYPE_TIMESTAMP_ARRAY

#if GS_COMPATIBILITY_SUPPORT_3_5
    GS_TYPE_NULL = -1
#endif
};
#endif

#if !defined(SWIGGO)
// Enum GSIndexTypeFlagTag
enum GSIndexTypeFlagTag {
    GS_INDEX_FLAG_DEFAULT,
    GS_INDEX_FLAG_TREE,
    GS_INDEX_FLAG_HASH,
    GS_INDEX_FLAG_SPATIAL
};
#endif

enum GSFetchOptionTag {

    GS_FETCH_LIMIT,

#if GS_COMPATIBILITY_SUPPORT_1_5

#if GS_INTERNAL_DEFINITION_VISIBLE
#if !GS_COMPATIBILITY_DEPRECATE_FETCH_OPTION_SIZE

    GS_FETCH_SIZE = (GS_FETCH_LIMIT + 1)
#endif
#endif

#endif
};

#if !defined(SWIGGO)
enum GSTimeUnitTag {

    GS_TIME_UNIT_YEAR,

    GS_TIME_UNIT_MONTH,

    GS_TIME_UNIT_DAY,

    GS_TIME_UNIT_HOUR,

    GS_TIME_UNIT_MINUTE,

    GS_TIME_UNIT_SECOND,

    GS_TIME_UNIT_MILLISECOND
};
#endif

enum GSTypeOptionTag {

    GS_TYPE_OPTION_KEY = 1 << 0,

#if GS_COMPATIBILITY_SUPPORT_3_5
    GS_TYPE_OPTION_NULLABLE = 1 << 1,

    GS_TYPE_OPTION_NOT_NULL = 1 << 2,
#endif

};

typedef int32_t GSTypeOption;

typedef int32_t GSIndexTypeFlags;

typedef int64_t GSTimestamp;

typedef int32_t GSEnum;

typedef GSEnum GSRowSetType;

typedef GSEnum GSContainerType;

typedef GSEnum GSType;

typedef int32_t GSResult;

typedef char GSChar;

typedef char GSBool;

typedef GSEnum GSTimeUnit;

typedef struct GSQueryAnalysisEntryTag GSQueryAnalysisEntry;
