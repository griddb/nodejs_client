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

#ifndef _GS_EXXCEPTION_H_
#define _GS_EXXCEPTION_H_

#include <exception>
#include <string>
#include "gridstore.h"

using namespace std;

namespace griddb {

/**
 * This class creates exception corresponding to error code
 */
class GSException : public exception {
    bool mIsTimeout;
    int32_t mCode;
    string mMessage;
    void *mResource;

    public:
        GSException(int32_t code) : exception(), mCode(code), mResource(NULL) {
            mMessage = "Error with number " + to_string((long long int)mCode);
            if (mCode != -1) {
                //Case exception with error code.
                mIsTimeout = gsIsTimeoutError(mCode);
            }
            else {
                mIsTimeout = false;
            }
        }
        GSException(const char* message) : exception(),
            mCode(-1), mMessage(message), mResource(NULL) {
            if (mCode != -1) {
                //Case exception with error code.
                mIsTimeout = gsIsTimeoutError(mCode);
            }
            else {
                mIsTimeout = false;
            }
        }
        GSException(void *resource, const char* message) : exception(),
            mCode(-1), mMessage(message), mResource(resource) {
            if (mCode != -1) {
                //Case exception with error code.
                mIsTimeout = gsIsTimeoutError(mCode);
            }
            else {
                mIsTimeout = false;
            }
        }
        GSException(void *resource, int32_t code) : exception(),
            mCode(code), mResource(resource) {
            mMessage = "Error with number " + to_string((long long int)mCode);
            if (mCode != -1) {
                //Case exception with error code.
                mIsTimeout = gsIsTimeoutError(mCode);
            }
            else {
                mIsTimeout = false;
            }
        }
        GSException(int32_t code, const char* message) : exception(),
            mCode(code), mMessage(message), mResource(NULL) {
            if (mCode != -1) {
                //Case exception with error code.
                mIsTimeout = gsIsTimeoutError(mCode);
            }
            else {
                mIsTimeout = false;
            }
        }
        GSException(void *resource, int32_t code, const char* message) : exception(),
            mCode(code), mMessage(message), mResource(resource) {
            if (mCode != -1) {
                //Case exception with error code.
                mIsTimeout = gsIsTimeoutError(mCode);
            }
            else {
                mIsTimeout = false;
            }
        }
        GSException(const GSException* exception) {
            mCode = exception->mCode;
            mIsTimeout = exception->mIsTimeout;
            mMessage = exception->mMessage;
            mResource = exception->mResource;
        }
        ~GSException() throw() {}
        int32_t get_code() {
            return mCode;
        }
        virtual const char* what() const throw() {
            return mMessage.c_str();
        }
        /*
         * Check timeout. Convert from C-API: gsIsTimeoutError
         */
        bool is_timeout() {
            return mIsTimeout;
        }
        /**
         * Get error stack size. Convert from C-API: gsGetErrorStackSize.
        */
        size_t get_error_stack_size() {
            return gsGetErrorStackSize(mResource);
        }
        /**
         * Get error stack code. Convert from C-API:  gsGetErrorCode.
        */
        GSResult get_error_code(size_t stack_index) {
            return gsGetErrorCode(mResource, stack_index);
        }
        /**
         * Get error message. Convert from C-API: gsFormatErrorMessage.
        */
        string get_message(size_t stack_index, size_t buf_size = 1024) {
            char* strBuf = new char[buf_size];
            size_t stringSize = gsFormatErrorMessage(mResource, stack_index, strBuf, buf_size);
            string ret(strBuf, stringSize);
            delete [] strBuf;
            return ret;
        }
        /**
         * Get error location. Convert from C-API: gsFormatErrorLocation.
         */
        string get_location(size_t stack_index, size_t buf_size = 1024) {
            char* strBuf = new char[buf_size];
            size_t stringSize = gsFormatErrorLocation(mResource, stack_index, strBuf, buf_size);
            string ret(strBuf, stringSize);
            delete [] strBuf;
            return ret;
        }
};

} /* namespace griddb */

#endif
