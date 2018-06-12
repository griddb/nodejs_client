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

#include "gridstore.h"
#include "GSException.h"
#include "stdio.h"
#include "string.h"

using namespace std;

namespace griddb {

class QueryAnalysisEntry {
    GSQueryAnalysisEntry* mQueryAnalysis;

    public:
        QueryAnalysisEntry(GSQueryAnalysisEntry* queryAnalysis);
        ~QueryAnalysisEntry();
        void get(GSQueryAnalysisEntry* queryAnalysis);
    private:

};

}
#endif /* SRC_QUERYANALYSISENTRY_H_ */

