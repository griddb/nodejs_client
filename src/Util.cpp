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

#include "Util.h"

namespace griddb {

    /**
     * @brief Allocate new memory and setup string data from source string data for the new memory
     * @param **to A pointer variable that refers to new string data
     * @param *from A pointer stores source string data
     */
    void Util::strdup(const GSChar** const to, const GSChar* from) {
        GSChar* temp = new char[strlen(from) + 1]();
        strcpy(temp, from);
        *to = temp;
    }
}
