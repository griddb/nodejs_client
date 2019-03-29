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

#ifndef SRC_QUERYANALYSISENTRY_H_
#define SRC_QUERYANALYSISENTRY_H_

#include <stdio.h>
#include <string.h>

#include "gridstore.h"
#include "GSException.h"
#include "Util.h"

using namespace std;

namespace griddb {

class QueryAnalysisEntry {
    private:
        GSQueryAnalysisEntry* mQueryAnalysis;
        void freeMemory();

    public:
        QueryAnalysisEntry(GSQueryAnalysisEntry* queryAnalysis);
        ~QueryAnalysisEntry();
        void close();
        void get(GSQueryAnalysisEntry* queryAnalysis);

};

}
#endif /* SRC_QUERYANALYSISENTRY_H_ */

