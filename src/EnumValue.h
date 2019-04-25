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

#ifndef _ENUM_VALUE_H_
#define _ENUM_VALUE_H_

using namespace std;

namespace griddb {
// Represents the type(s) of a Container.
class ContainerType {
    public:
        static const int COLLECTION = 0;
        static const int TIME_SERIES = 1;
};
// Represents the type(s) of indexes set on a Container.
class IndexType {
    public:
        static const int DEFAULT = -1;
        static const int TREE = 1 << 0;
        static const int HASH = 1 << 1;
        static const int SPATIAL = 1 << 2;
};
// The type of content that can be extracted from GSRowSet.
class RowSetType {
    public:
        static const int CONTAINER_ROWS = 0;
        static const int AGGREGATION_RESULT = 1;
        static const int QUERY_ANALYSIS = 2;
};
// The options for fetching the result of a query.
class FetchOption {
    public:
        static const int LIMIT = 0;

#if GS_INTERNAL_DEFINITION_VISIBLE
#if !GS_COMPATIBILITY_DEPRECATE_FETCH_OPTION_SIZE
        static const int SIZE = (LIMIT + 1);
#endif
#endif

#if GS_COMPATIBILITY_SUPPORT_4_0
        static const int PARTIAL_EXECUTION = (LIMIT + 2);
#endif
};
// Represents the time unit(s) used in TimeSeries data operation.
class TimeUnit {
    public:
        static const int YEAR = 0;
        static const int MONTH = 1;
        static const int DAY = 2;
        static const int HOUR = 3;
        static const int MINUTE = 4;
        static const int SECOND = 5;
        static const int MILLISECOND = 6;
};
// Represents the type(s) of field values in GridDB.
class Type {
    public:
        static const int STRING = 0;
        static const int BOOL = 1;
        static const int BYTE = 2;
        static const int SHORT = 3;
        static const int INTEGER = 4;
        static const int LONG = 5;
        static const int FLOAT = 6;
        static const int DOUBLE = 7;
        static const int TIMESTAMP = 8;
        static const int GEOMETRY = 9;
        static const int BLOB = 10;
        static const int STRING_ARRAY = 11;
        static const int BOOL_ARRAY = 12;
        static const int BYTE_ARRAY = 13;
        static const int SHORT_ARRAY = 14;
        static const int INTEGER_ARRAY = 15;
        static const int LONG_ARRAY = 16;
        static const int FLOAT_ARRAY = 17;
        static const int DOUBLE_ARRAY = 18;
        static const int TIMESTAMP_ARRAY = 19;

        // Can't use NULL because it is keyword of C language
        static const int NULL_TYPE = -1;
};
// Sum of bits of value of the flag indicating the option setting for Column.
class TypeOption {
    public:
        static const int NULLABLE = 1 << 1;
        static const int NOT_NULL = 1 << 2;
};
}

#endif
