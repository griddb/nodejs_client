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

#include "StoreFactory.h"
#include "GSException.h"

#define MAX_PROPS 10

namespace griddb {

    StoreFactory::StoreFactory() : mFactory(NULL) {
    }
    StoreFactory::~StoreFactory() {
        //allRelated = FALSE, since Gridstore object is managed by Store class
        close(GS_FALSE);
    }
    /**
    * Release all GridStore created by this factory and related resources
    */
    void StoreFactory::close(GSBool allRelated) {
        if (mFactory != NULL) {
            gsCloseFactory(&mFactory, allRelated);
            mFactory = NULL;
        }
    }
    StoreFactory* StoreFactory::get_instance() {
        GSGridStoreFactory* pFactory = gsGetDefaultFactory();
        StoreFactory* factory(new StoreFactory());
        factory->set_factory(pFactory);

        return factory;
    }
    /*
     * set GSPropertyEntry
     */
    void StoreFactory::set_property_entry(GSPropertyEntry *prop, const char* name, const char* value) {
        prop->name = name;
        prop->value = value;
    }
    /*
     * Check whether in MULTICAST mode
     */
    bool StoreFactory::check_multicast(const char* address) {
        if (address) {
            char *tmp = strdup(address);
            char *octets = strtok((char*)tmp, ".");
            if (octets) {
                int firstOctet = atoi(octets);
                int first4Bits = firstOctet >> 4 & 0x0f;
                if (first4Bits == 0x0E) {
                    free((void *) tmp);
                    return true;
                }
            }
            if (tmp) {
                free((void *) tmp);
            }
        }
        return false;
    }
    /*
     * Returns a Store with the specified properties
     */
    Store* StoreFactory::get_store(const char* host, int32_t port, const char* cluster_name,
            const char* database, const char* user, const char* password,
            const char* notification_member, const char* notification_provider) {
        int index = 0;
        GSPropertyEntry local_props[MAX_PROPS] = {0};
        std::string lport = std::to_string((long long int)port);

        if (check_multicast(host)) {
            set_property_entry(&local_props[0], "notificationAddress", host);
            set_property_entry(&local_props[1], "notificationPort", lport.c_str());
            index = 2;
        } else {
            if (host) {
                set_property_entry(&local_props[0], "host", host);
                set_property_entry(&local_props[1], "port", lport.c_str());
                index += 2;
            }

            if (notification_member) {
                set_property_entry(&local_props[index], "notificationMember", notification_member);
                index++;
            }
            if (notification_provider) {
                set_property_entry(&local_props[index], "notificationProvider", notification_provider);
                index++;
            }

        }
        if (cluster_name) {
            set_property_entry(&local_props[index], "clusterName", cluster_name);
            index++;
        }
        if (database) {
            set_property_entry(&local_props[index], "database", database);
            index++;
        }
        if (user) {
            set_property_entry(&local_props[index], "user", user);
            index++;

        }
        if (password) {
            set_property_entry(&local_props[index], "password", password);
            index++;
        }

        GSGridStore *store;
        GSResult ret = gsGetGridStore(mFactory, local_props, index, &store);

        // Check ret, if error, throw exception
        if (ret != GS_RESULT_OK) {
            throw GSException(mFactory, ret);
        }
        return new Store(store);
    }

    /**
     * Changes the settings for this Factory.
     * The changed settings will be reflected in GridStore object which is already created by the specified Factory and GridStore object which will be created later by the Factory.
     */
    void StoreFactory::set_properties(const GSPropertyEntry* props,
            int propsCount) {
        GSResult ret = gsSetFactoryProperties(mFactory, props, propsCount);

        // Check ret, if error, throw exception
        if (ret != GS_RESULT_OK) {
            throw GSException(mFactory, ret);
        }
    }

    /*
     * Return current client version
     */
    string StoreFactory::get_version() {
        return CLIENT_VERSION;
    }

    void StoreFactory::set_factory(GSGridStoreFactory* factory) {
        mFactory = factory;
    }

}
