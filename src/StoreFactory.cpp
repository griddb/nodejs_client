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

#define MAX_PROPS 10

namespace griddb {

    StoreFactory::StoreFactory() : mFactory(NULL) {
    }

    StoreFactory::~StoreFactory() {
        //allRelated = FALSE, since Gridstore object is managed by Store class
        close(GS_FALSE);
    }

    /**
     * @brief Release StoreFactory resource
     */
    void StoreFactory::close(GSBool allRelated) {
        if (mFactory != NULL) {
            gsCloseFactory(&mFactory, allRelated);
            mFactory = NULL;
        }
    }

    /**
     * @brief Get a default GSGridStoreFactory instance.
     * @return The pointer to a pointer variable to store StoreFactory instance
     */
    StoreFactory* StoreFactory::get_instance() {
        GSGridStoreFactory* pFactory = gsGetDefaultFactory();

        try {
            StoreFactory* factory(new StoreFactory());
            factory->set_factory(pFactory);

            return factory;
        } catch (bad_alloc& ba) {
            gsCloseFactory(&pFactory, GS_FALSE);
            throw GSException("Memory allocation error");
        }
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
        if (address && address[0] != '\0') {
            char* tmp;
            try {
                Util::strdup((const GSChar**)&tmp, address);
            } catch (bad_alloc& ba) {
                throw GSException("Memory allocation error");
            }

            char *octets = strtok((char*)tmp, ".");
            if (octets) {
                int firstOctet = atoi(octets);
                int first4Bits = firstOctet >> 4 & 0x0f;
                if (first4Bits == 0x0E) {
                    delete[] tmp;
                    return true;
                }
            }
            delete[] tmp;
        }
        return false;
    }

    /**
     * @brief Get a Store with the specified properties
     * @param *host A destination host name
     * @param port A destination port number
     * @param *cluster_name A cluster name
     * @param *database A database name to be connected
     * @param *user A user name
     * @param *password A password for user authentication
     * @param *notification_member A list of address and port pairs in cluster
     * @param *notification_provider A URL of address provider
     * @return The pointer to a pointer variable to store Store instance
     */
    Store* StoreFactory::get_store(const char* host, int32_t port, const char* cluster_name,
            const char* database, const char* user, const char* password,
            const char* notification_member, const char* notification_provider) {
        size_t index = 0;
        GSPropertyEntry local_props[MAX_PROPS] = {0};
        std::string lport = std::to_string((long long int)port);

        if (check_multicast(host)) {
            set_property_entry(&local_props[0], "notificationAddress", host);
            set_property_entry(&local_props[1], "notificationPort", lport.c_str());
            index += 2;
        } else if (host && host[0] != '\0') {
            set_property_entry(&local_props[0], "host", host);
            set_property_entry(&local_props[1], "port", lport.c_str());
            index += 2;
        }

        if (notification_member && notification_member[0] != '\0') {
            set_property_entry(&local_props[index], "notificationMember", notification_member);
            index++;
        }
        if (notification_provider && notification_provider[0] != '\0') {
            set_property_entry(&local_props[index], "notificationProvider", notification_provider);
            index++;
        }
        if (cluster_name && cluster_name[0] != '\0') {
            set_property_entry(&local_props[index], "clusterName", cluster_name);
            index++;
        }
        if (database && database[0] != '\0') {
            set_property_entry(&local_props[index], "database", database);
            index++;
        }
        if (user && user[0] != '\0') {
            set_property_entry(&local_props[index], "user", user);
            index++;

        }
        if (password && password[0] != '\0') {
            set_property_entry(&local_props[index], "password", password);
            index++;
        }

        GSGridStore *gsStore;
        GSResult ret = gsGetGridStore(mFactory, local_props, index, &gsStore);

        // Check ret, if error, throw exception
        if (!GS_SUCCEEDED(ret)) {
            throw GSException(mFactory, ret);
        }

        try {
            //return new Store(store);
            Store* store = new Store(gsStore);
            return store;
        } catch (bad_alloc& ba) {
            gsCloseGridStore(&gsStore, GS_FALSE);
            throw GSException(mFactory, "Memory allocation error");
        }
    }

    /**
     * @brief Get current client version
     * @return Client version name
     */
    string StoreFactory::get_version() {
        return CLIENT_VERSION;
    }

    /*
     * Set attribute: mFactory
     */
    void StoreFactory::set_factory(GSGridStoreFactory* factory) {
        mFactory = factory;
    }

}
