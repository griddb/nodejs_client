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

#define DEFAULT_ERROR_CODE -1
#define DEFAULT_ERROR_STACK_SIZE 1

namespace griddb {

/**
 * This class creates exception corresponding to error code
 */
class GSException : public exception {
    private:
        bool mIsTimeout;
        int32_t mCode;
        string mMessage;
        void *mResource;

        bool hasInnerError;
        size_t mInnerErrStackSize;
        GSResult* mInnerErrCodeStack;
        string* mInnerMessagesStack;
        string* mInnerErrorLocationStack;
        
    public:
        GSException(const char* message) : exception(),
            mCode(DEFAULT_ERROR_CODE), mMessage(message), mResource(NULL) {
            hasInnerError = false;
            mInnerErrStackSize = 0;
            mInnerErrCodeStack = NULL;
            mInnerMessagesStack = NULL;
            mInnerErrorLocationStack = NULL;
            mIsTimeout = false;
        }
        GSException(void *resource, const char* message) : exception(),
            mCode(DEFAULT_ERROR_CODE), mMessage(message), mResource(resource) {
            hasInnerError = false;
            mInnerErrStackSize = 0;
            mInnerErrCodeStack = NULL;
            mInnerMessagesStack = NULL;
            mInnerErrorLocationStack = NULL;
            mIsTimeout = false;
        }
        GSException(void *resource, int32_t code) : exception(),
            mCode(code), mResource(resource) {
            mMessage = "Error with number " + to_string((long long int)mCode);
            if (mCode != DEFAULT_ERROR_CODE && mResource != NULL) {
                //Case exception with error code from c layer
                mIsTimeout = gsIsTimeoutError(mCode);
                // Store error stack
                hasInnerError = true;
                mInnerErrStackSize = get_error_stack_size_from_lower_layer();
                mInnerErrCodeStack = new GSResult[mInnerErrStackSize]();
                mInnerMessagesStack = new string[mInnerErrStackSize]();
                mInnerErrorLocationStack = new string[mInnerErrStackSize]();
                for (int i = 0; i < mInnerErrStackSize; i++) {
                    mInnerErrCodeStack[i] = get_error_code_from_lower_layer(i);
                    mInnerMessagesStack[i] = get_message_from_lower_layer(i);
                    mInnerErrorLocationStack[i] = get_location_from_lower_layer(i);
                }
            } else {
                hasInnerError = false;
                mInnerErrStackSize = 0;
                mInnerErrCodeStack = NULL;
                mInnerMessagesStack = NULL;
                mInnerErrorLocationStack = NULL;
                mIsTimeout = false;
            }
        }
        GSException(const GSException* exception) {
            mCode = exception->mCode;
            mIsTimeout = exception->mIsTimeout;
            mMessage = exception->mMessage;
            mResource = exception->mResource;
            hasInnerError = exception->hasInnerError;
            if (hasInnerError == true) {
                mInnerErrStackSize = exception->mInnerErrStackSize;
                mInnerErrCodeStack = new GSResult[mInnerErrStackSize]();
                mInnerMessagesStack = new string[mInnerErrStackSize]();
                mInnerErrorLocationStack = new string[mInnerErrStackSize]();
                for (int i = 0; i < mInnerErrStackSize; i++) {
                    mInnerErrCodeStack[i] = exception->mInnerErrCodeStack[i];
                    mInnerMessagesStack[i] = exception->mInnerMessagesStack[i];
                    mInnerErrorLocationStack[i] = exception->mInnerErrorLocationStack[i];
                }
            } else {
                mInnerErrStackSize = 0;
                mInnerErrCodeStack = NULL;
                mInnerMessagesStack = NULL;
                mInnerErrorLocationStack = NULL;
            }
        }
        ~GSException() throw() {
            close();
        }

        void close() {
            if (mInnerErrCodeStack != NULL) {
                delete[] mInnerErrCodeStack;
                mInnerErrCodeStack = NULL;
            }
            if (mInnerMessagesStack != NULL) {
                delete[] mInnerMessagesStack;
                mInnerMessagesStack = NULL;
            }
            if (mInnerErrorLocationStack != NULL) {
                delete[] mInnerErrorLocationStack;
                mInnerErrorLocationStack = NULL;
            }
        }
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
            if (hasInnerError == false) {
                return DEFAULT_ERROR_STACK_SIZE;
            }
            return mInnerErrStackSize;
        }
        /**
         * Get error code. Convert from C-API:  gsGetErrorCode.
        */
        GSResult get_error_code(size_t stack_index) {
            if (hasInnerError == false) {
                if (stack_index == 0) return mCode;
                return 0;
            } else {
                if (stack_index >= mInnerErrStackSize) return 0;
                return mInnerErrCodeStack[stack_index];
            }
        }
        /**
         * Get error message. Convert from C-API: gsFormatErrorMessage.
        */
        string get_message(size_t stack_index, size_t buf_size = 1024) {
            if (hasInnerError == false) {
                if (stack_index == 0) return mMessage;
                return "";
            } else {
                if (stack_index >= mInnerErrStackSize) return "";
                return mInnerMessagesStack[stack_index];
            }
        }
        /**
         * Get error location. Convert from C-API: gsFormatErrorLocation.
         */
        string get_location(size_t stack_index, size_t buf_size = 1024) {
            if (hasInnerError == false) {
                return "";
            } else {
                if (stack_index >= mInnerErrStackSize) return "";
                return mInnerErrorLocationStack[stack_index];
            }
        }
    private:
        /**
         * Get error stack size. Convert from C-API: gsGetErrorStackSize.
        */
        size_t get_error_stack_size_from_lower_layer() {
            return gsGetErrorStackSize(mResource);
        }
        /**
         * Get error code. Convert from C-API:  gsGetErrorCode.
        */
        GSResult get_error_code_from_lower_layer(size_t stack_index) {
            return gsGetErrorCode(mResource, stack_index);
        }
        /**
         * Get error message. Convert from C-API: gsFormatErrorMessage.
        */
        string get_message_from_lower_layer(size_t stack_index, size_t buf_size = 1024) {
            char* strBuf = new char[buf_size];
            size_t stringSize = gsFormatErrorMessage(mResource, stack_index, strBuf, buf_size);
            string ret(strBuf, stringSize);
            delete [] strBuf;
            return ret;
        }
        /**
         * Get error location. Convert from C-API: gsFormatErrorLocation.
         */
        string get_location_from_lower_layer(size_t stack_index, size_t buf_size = 1024) {
            char* strBuf = new char[buf_size];
            size_t stringSize = gsFormatErrorLocation(mResource, stack_index, strBuf, buf_size);
            string ret(strBuf, stringSize);
            delete [] strBuf;
            return ret;
        }
};

} /* namespace griddb */

#endif
