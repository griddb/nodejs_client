var griddb = require('./griddb_client');
var exports = module.exports;

griddb['StoreFactory_'] = griddb.StoreFactory;

/**
 * <p>
 * Manage a Store instance.
 * <br> It manages the client settings shared by Store instances and used connections.
 * <br> To access GridDB, you need to get a Store instance using this Factory.
 * </p>
 * @type {object}
 * @alias StoreFactory
 */
class StoreFactory {
    constructor(factory) {
        this.factory = factory;
    }
    
    /**
     * <p>
     * Returns a Store with the specified properties.
     * <br> When obtaining Store, it just searches for the name of a master node (hereafter, a master) administering each Container as necessary, but authentication is not performed. When a client really needs to connect to a node corresponding to each Container , authentication is performed.
     * <br> A new Store instance is created by each call of this method. Operations on different Store instances and related resources are thread safe. That is, if some two resources are each created based on Store instances or they are just Store instances, and if they are related to different Store instances respectively, any function related to one resource can be called, no matter when a function related to the other resource may be called from any thread. However, since thread safety is not guaranteed for Store itself, it is not allowed to call a method of a single Store instance from two or more threads at an arbitrary time.
     * </p>
     * @type {Store}
     * @param {object} options - Information to get store.
     * @param {string} options.host=null - A destination host name.
     * @param {number} options.port=null - A destination port number.
     * @param {string} options.clusterName=null - A cluster name.
     * @param {string} options.database=null - Name of the database to be connected.
     * @param {string} options.username=null - A user name.
     * @param {string} options.password=null - A password for user authentication.
     * @param {string} options.notificationMember=null - A list of address and port pairs in cluster.
     * @param {string} options.notificationProvider=null -  A URL of address provider.
     * @returns {Store} Store instance.
     */
    getStore(obj) {
        try {
            return new Store(this.factory.getStore(obj));
        } catch(err) {
            throw(convertToGSException(err));
        }
    }

    /**
     * <p>
     * Returns a StoreFactory instance.
     * <br> It can be used only for RowSet obtained with locking enabled.
     * </p>
     * @static
     * @type {StoreFactory}
     * @returns {StoreFactory} StoreFactory instance
     */
    static getInstance() {
        try {
            return new StoreFactory(griddb.StoreFactory_.getInstance());
        } catch(err) {
            throw(convertToGSException(err));
        }
    }
    
    /**
     * <p>
     * Returns the version of this client.
     * </p>
     * @type {string}
     * @returns {string} The version of this client
     */
    getVersion() {
        return this.factory.getVersion();
    }
};

/**
 * <p>
 * Provides functions to manipulate the entire data managed in one GridDB system.
 * <br> A function to add, delete, or change the composition of Collection and TimeSeries Containers as well as to process the Rows constituting a Container is provided.
 * <br> Regardless of container types, etc., multiple container names different only in uppercase and lowercase ASCII characters cannot be defined in a database.  See the GridDB Technical Reference for the details. In the operations specifying a container name, uppercase and lowercase ASCII characters are identified as the same unless otherwise noted.
 * <br> Thread safety of each method is not guaranteed.
 * </p>
 * @type {object}
 * @alias Store
 * @property {PartitionController} partitionController - PartitionController instance corresponding to GridDB cluster. (Read-only attribute)
 */
class Store {
    constructor(store) {
        this.store = store;
        this.pController = null;
    }
    
    /**
     * <p>
     * Get PartitionController corresponding to GridDB cluster.
     * <br> It can be used until the specified GSGridStore is closed.
     * </p>
     * @type {PartitionController}
     * @returns {PartitionController} PartitionController instance
     */
    get partitionController() {
        if (this.pController == null) {
            this.pController = new PartitionController(this.store.partitionInfo, this);
        }
        return this.pController;
    }

    /**
     * <p>
     * Set output type for row field timestamp.
     * <br> The default of output timestsamp is datatime.
     * </p>
     * @type {void}
     * @param {boolean} value - enable timestamp ouput float
     * @returns {void}
     */
    set timestampOutput(value) {
        this.store.timestampOutput = value;
    }

    /**
     * <p>
     * Get output type for row field timestamp.
     * <br> The default of output timestsamp is datatime.
     * </p>
     * @type {boolean}
     * @returns {boolean} PartitionController instance
     */
    get timestampOutput() {
        return this.store.timestampOutput;
    }
    
    /**
     * <p>
     * Newly creates or update a Container with the specified ContainerInfo.
     * <br> It can be used only for RowSet obtained with locking enabled.
     * </p>
     * @type {Promise<Container>}
     * @param {ContainerInfo} info - Container information to be processed.
     * @param {boolean} modifiable=false - Indicates whether the column layout of the existing Container can be modified or not.
     * @returns {Promise<Container>} Container instance
     */
    putContainer(info, modifiable = false) {
        var this_ = this;
        return new Promise(function(resolve, reject) {
            setTimeout(function() {
                try{
                    var container = this_.store.putContainer(info, modifiable);
                    resolve(new Container(container, this_));
                } catch(err) {
                    reject(convertToGSException(err));
                }
            }, 0);
        });
    }

    /**
     * <p>
     * Get a Container instance whose Rows can be processed using a Row.
     * </p>
     * @type {Promise<Container>}
     * @param {string} name - Container name to be processed
     * @returns {Promise<Container>} Container instance. If the Container with the specified name does not exist, null is returned.
     */
    getContainer(name) {
        var this_ = this;
        return new Promise(function(resolve, reject) {
            setTimeout(function() {
                try {
                    var container = this_.store.getContainer(name);
                    if (container === null) {
                        resolve(null);
                    } else {
                        resolve(new Container(container, this_));
                    }
                } catch(err) {
                    reject(convertToGSException(err));
                }
            }, 0);
        });
    }

    /**
     * <p>
     * Delete a Container with the specified name.
     * <br> If the specified Container is already deleted, nothing is changed.
     * <br> When a transaction(s) is active in a target Container, it deletes the Container after waiting for the transaction completion.
     * </p>
     * @type {Promise<void>}
     * @param {string} name - Container name to be processed.
     * @return {Promise<void>}
     */
    dropContainer(name) {
        var this_ = this;
        return new Promise(function(resolve, reject) {
            setTimeout(function() {
                try {
                    resolve(this_.store.dropContainer(name));
                } catch(err) {
                    reject(convertToGSException(err));
                }
            }, 0);
        });
    }

    /**
     * <p>
     * Get information related to a Container with the specified name.
     * <br> A name stored in GridDB is set for the Container name to be included in a returned ContainerInfo . Therefore, compared to the specified Container name, the notation of the ASCII uppercase characters and lowercase characters may differ.
     * <br> When a transaction(s) is active in a target Container, it deletes the Container after waiting for the transaction completion.
     * </p>
     * @type {Promise<ContainerInfo>}
     * @param {string} name - Container name to be processed.
     * @returns {Promise<ContainerInfo>} ContainerInfo instance for storing information about the Container with the specified name. If the Container with the specified name does not exist, null is returned.
     */
    getContainerInfo(name) {
        var this_= this;
        return new Promise(function(resolve, reject) {
            setTimeout(function() {
                try {
                    resolve(this_.store.getContainerInfo(name));
                } catch(err) {
                    reject(convertToGSException(err));
                }
            }, 0);
        });
    }

    /**
     * <p>
     * Query execution and fetch is carried out on a specified arbitrary number of Query , with the request unit enlarged as much as possible.
     * <br> For each Query included in a specified query column, perform a similar query execution and fetch as when Query.fetch() is performed individually and set the RowSet in the results. Use getRowSet() to extract the execution results of each Query . However, unlike the case when carried out individually, the target node is requested for the same storage destination, etc. with a unit that is as large as possible. Based on this, the larger the number of elements in the list, the higher is the possibility that the number of correspondences with the target node will be reduced. Query in a list are not executed in any particular order.
     * <br> Only a Query that has not been closed, including corresponding Container acquired via the specified Store instance, can be included in a specified query column. Like a fetch() , the Row operations via RowSet finally generated and held by each Query will be unavailable. If the same instance is included multiple times in an array, the behavior will be the same as the case in which the respective instances differ.
     * <br> Like other Container or Row operations, consistency between Containers is not guaranteed. Therefore, the processing results for a certain Container may be affected by other operation commands that have been completed prior to the start of the process.
     * <br> The commit mode of each Container corresponding to the specified Query can be used in either the auto commit mode or manual commit mode. The transaction status is reflected in the execution results of the query. If the operation is completed normally, the corresponding transaction of each Container will not be aborted so long as the transaction timeout time has not been reached.
     * <br> If an exception occurs in the midst of processing each Query , a new RowSet may be set for only some of the Query . In addition, uncommitted transactions of each Query corresponding to the designated Container may be aborted.
     * <br> If the system tries to acquire a large number of Rows all at once, the upper limit of the communication buffer size managed by the GridDB node may be reached, possibly resulting in a failure. Refer to "System limiting values" in the Appendix of GridDB API Reference for the upper limit size.
     * </p>
     * @type {Promise<void>}
     * @param {Query[]} queryList - A list of target Queries
     * @returns {Promise<void>}
     */
    fetchAll(queryList) {
        var this_ = this;
        if (queryList == null ||queryList.constructor !== Array || queryList.length == 0) {
            return new Promise(function(resolve, reject) {
                setTimeout(function() {
                    try {
                        resolve(this_.store.fetchAll(queryList));
                    } catch(err) {
                        reject(convertToGSException(err));
                    }
                }, 0);
            });
        } else {
            return new Promise(function(resolve, reject) {
                setTimeout(function() {
                    var queryListRet = queryList;
                    var queryListTmp = [];
                    
                    for (var i = 0; i < queryListRet.length; i++) {
                        if (queryListRet[i] && queryListRet[i].hasOwnProperty('query')) {
                            queryListTmp.push(queryListRet[i].query);
                        } else {
                            reject("Invalid input of Store.fetchAll()");
                        }
                    }
                    try {
                        resolve(this_.store.fetchAll(queryListTmp));
                    } catch(err) {
                        reject(convertToGSException(err));
                    }
                }, 0);
            });
        }
    }

    /**
     * <p>
     * New creation or update operation is carried out on an arbitrary number of rows of a Container, with the request unit enlarged as much as possible.
     * <br> For each Row object included in a specified entry column, a new creation or update operation is carried out just like the case when Container.put() is invoked individually. However, unlike the case when carried out individually, the target node is requested for the same storage destination, etc. with a unit that is as large as possible. Based on this, the larger the total number of Row objects specified and the larger the total number of target Containers, the higher is the possibility that the number of correspondences with the target node will be reduced.
     * <br> A specified entry column is composed of an arbitrary number of entries that adopt the Container name as its key and the column of Row objects as its value. A subject Container may be a mixture of different Container types and column layouts. However, the Containers must already exist. Null can not be set as the Container name in the entry column. Also null can not be set as the array address to the column of Row objects if the number of elements in the column of Row objects is positive value.
     * <br> An arbitrary number of Row with the same column layout as the subject Container can be included in each column of Row objects. In the current version, all the column order must also be the same. The Container cannot include null as an element of the column of Row objects. Depending on the Container type and setting, the same restrictions as Container.put() are established for the contents of Rows that can be operated. If there are multiple columns of Row objects having the same Row key targeting the same Container in the designated entry column, the contents of the rear-most Row object having a Row key with the same value will be reflected using the element order of entry column as a reference if it is between different lists, or the element order of the column of Row object as a reference if it is within the same column of Row object. The transaction cannot be maintained and the lock cannot continue to be retained. However, if the lock that affects the target Row is secured by an existing transaction, the system will continue to wait for all the locks to be released. Like other Container or Row operations, consistency between Containers is not guaranteed. Therefore, the processing results for a certain Container may be affected by other operation commands that have been completed prior to the start of the process. If an error occurs in the midst of processing a Container and its Rows, only the results for some of the Rows of some of the Containers may remain reflected.
     * </p>
     * @type {Promise<void>}
     * @param {object.<string, object[][]>} containerEntry - dict-type data consisting of container name and the list of Row objects
     * @returns {Promise<void>}
     */
    multiPut(containerEntry) {
        var this_ = this;
        return new Promise(function(resolve, reject) {
            setTimeout(function() {
                try {
                    resolve(this_.store.multiPut(containerEntry));
                } catch(err) {
                    reject(convertToGSException(err));
                }
            }, 0);
        });
    }

    /**
     * <p>
     * Returns an arbitrary number and range of Rows in any Container based on the specified conditions, with the request unit enlarged as much as possible.
     * <br> Returns the Row contents in accordance with the conditions included in the specified entry column, similar to invoking Container.get() or Query.fetch() individually. However, unlike the case when carried out individually, the target node is requested for the same storage destination, etc. with a unit that is as large as possible. Based on this, the larger the total number of Rows conforming to the conditions and the larger the total number of target Containers, the higher is the possibility that the number of correspondences with the target node will be reduced.
     * <br> A specified condition entry column is composed of an arbitrary number of condition entries that adopt the Container name as the key and the acquisition condition represented by RowKeyPredicate as the value. Multiple instances with the same RowKeyPredicate can also be included. In addition, a subject Container may be a mixture of different Container types and column layouts. However, there are some acquisition conditions that cannot be evaluated due to the composition of the Container. Refer to the definitions of the various setting functions for RowKeyPredicate for the specific restrictions. In addition, the specified Container name must be a real Container. It is prohibited to set null in the Container name or the acquisition condition.
     * <br> An acquired entry column is composed of entries that adopt the Container name as its key and column of Row objects as its value. All entries included in a specified entry as acquisition conditions are included in an acquired entry column. If multiple entries pointing the same Container are included in a specified condition entry column, a single entry consolidating these is stored in the acquired entry column. If multiple Row objects are included in the same list, the stored order follows the Container type and the definition of the individual Container type derived from the corresponding Container . If there is no Row corresponding to the specified Container, the number of elements in corresponding column of Row object will be 0.
     * <br> Like other Container or Row operations, consistency between Containers is not guaranteed. Therefore, the processing results for a certain Container may be affected by other operation commands that have been completed prior to the start of the process.
     * <br> Like Container.get() or Query.fetch() , a transaction cannot be maintained and requests for updating locks cannot be made.
     * <br> If the system tries to acquire a large number of Rows all at once, the upper limit of the communication buffer size managed by the GridDB node may be reached, possibly resulting in a failure. Refer to "System limiting values" in the Appendix of GridDB API Reference for the upper limit size.
     * </p>
     * @type {object.<string, object[]>}
     * @param {object.<string, RowKeyPredicate>} predicateEntry - The column of condition entry consisting of a combination of the target Container name and the acquisition condition. It consists of the array of RowKeyPredicate.
     * @returns {object.<string, object[]>} dict-type data consisting of container name and the list of Row.
     */
    multiGet(predicateEntry) {
        var this_ = this;
        var inputPredEntry = {};
        if (!predicateEntry || typeof predicateEntry != 'object') {
            return new Promise(function(resolve, reject) {
                setTimeout(function() {
                    try {
                        return resolve(this_.store.multiGet(predicateEntry));
                    } catch(err) {
                        reject(convertToGSException(err));
                    }
                }, 0);
            });
        }
        var error = false;
        Object.keys(predicateEntry).forEach(key => {
            var value = predicateEntry[key];
            if (value && typeof value == 'object' && value.hasOwnProperty('rowKeyPredicate')) {
                inputPredEntry[key] = value.rowKeyPredicate;
            } else {
                error = true;
            }
        });
        if (error) {
            return Promise.reject(new Error('Invalid input of multiGet'))
        }
        return new Promise(function(resolve, reject) {
            setTimeout(function() {
                try {
                    return resolve(this_.store.multiGet(inputPredEntry));
                } catch(err) {
                    reject(convertToGSException(err));
                }
            }, 0);
        });
    }

    /**
     * <p>
     * Creates a matching condition with the specified Type as the type of Row key.
     * <br> The target Container must have a Row key, and it must be the same type as the specified Type.
     * <br> The type of Row key that can be set must be the same type that is allowed by the individual Container type derived from Container.
     * </p>
     * @type {RowKeyPredicate}
     * @param {Type} type - The type of Row key used as a matching condition.
     * @returns {RowKeyPredicate} RowKeyPredicate instance
     */
    createRowKeyPredicate(type) {
        try {
            return new RowKeyPredicate(this.store.createRowKeyPredicate(type));
        } catch(err) {
            throw(convertToGSException(err));
        }
    }

    /**
     * <p>
     * Disconnects with GridDB and releases related resources as necessary.
     * </p>
     * @type {Promise<void>}
     * @param {boolean} allRelated=false - In the current version, this parameter does not affect the result.
     * @returns {Promise<void>}
     */
    close() {
        var this_ = this;
        return new Promise(function(resolve, reject) {
            setTimeout(function() {
                try {
                    resolve(this_.store.close(false));
                } catch(err) {
                    reject(convertToGSException(err));
                }
            }, 0);
        });
    }

    /**
     * <p>
     * Disconnects with GridDB and releases related resources as necessary.
     * </p>
     * @type {void}
     * @param {boolean} allRelated=false - In the current version, this parameter does not affect the result.
     * @returns {void}
     */
    closeSync() {
        try {
            this.store.close(false);
        } catch(err) {
            throw(convertToGSException(err));
        }
    }
};

/**
 * <p>CProvides management functions for sets of row having same type.
 * <br>Each column in GridDB schema is defined by a ContainerInfo. Each container consists of one or more columns.
 * <br>Mapping table between column type and value in a row object is following:
 * </p>
 * <p>
 * <table style="width:100%;" border="1">
 *  <tr><th><b>(Column) Type</th><th>string</th><th>boolean</th><th>number</th><th>Date</th><th>Buffer</th></tr>
 *  <tr><td>STRING</td><td>mapped</td><td> </td><td> </td><td> </td><td> </td></tr>
 *  <tr><td>BOOL</td><td> </td><td>mapped</td><td>mapped</td><td> </td><td> </td></tr>
 *  <tr><td>BYTE</td><td> </td><td> </td><td>mapped</td><td> </td><td> </td></tr>
 *  <tr><td>SHORT</td><td> </td><td> </td><td>mapped</td><td> </td><td> </td></tr>
 *  <tr><td>INTEGER</td><td> </td><td> </td><td>mapped</td><td> </td><td> </td></tr>
 *  <tr><td>LONG</td><td> </td><td> </td><td>mapped</td><td> </td><td> </td></tr>
 *  <tr><td>FLOAT</td><td> </td><td> </td><td>mapped</td><td> </td><td> </td></tr>
 *  <tr><td>DOUBLE</td><td> </td><td> </td><td>mapped</td><td></td><td> </td></tr>
 *  <tr><td>TIMESTAMP</td><td>mapped</td><td> </td><td>mapped</td><td>mapped</td><td> </td></tr>
 *  <tr><td>BLOB</td><td> </td><td> </td><td> </td><td> </td><td>mapped</td></tr>
 * </table>
 * </p>
 * <p></p>
 * <p>
 * TIMESTAMP represents milliseconds since the UNIX epoch (January 1, 1970 00:00:00 UTC) with long type.
 * <br>TIMESTAMP value suports msec. Range of time is from 1/1/1970 to 12/31/9999 (UTC).
 * There may be more limitation depending on a GridDB cluster configuration. Cannot store a value out of the range.
 * <br>There is an upper limit for the number of column and the length of column name.
 * The value has limitations for rage and size. Please refer to appendix of GridDB API Reference for more detail.
 * Cannot store a value exceeding these limitations.
 * <br>A limitation about a row key type, presence of column corresponding to a row key,
 * and availability of row value updates, may differ for each type derived from the container type.
 * <br>NULL in GridDB rows can be retained unless the NOT NULL constraint is set.
 * NOT NULL constraint can be set with columnInfoList object in ContainerInfo when put_container() is called.
 * <br>About transaction, auto commit mode is active as a default.
 * In the auto commit mode, each transaction is processed sequentially, and cannot be canceled.
 * For manual commit mode, transactions before a commit is canceled if there is an error on a cluster node during the transaction via Container instances.
 * Transaction isolation level supports only READ COMMITTED. Lock granularity may differ for each container type.
 * <br>When a row is updated, added, deleted, and got a lock for updates, a transaction is generated internally.
 * This transaction has a valid period.
 * After some period defined by GridDB is passed from the timing of this transaction for Container instance, any same type of transactions will be not accepted.
 * </p>
 * @type {object}
 * @alias Container
 * @property {ContainerType} type=0 - Container type
 */
class Container {
    constructor(container, store) {
        this.container = container;
        //need reference to avoid gc
        this.store = store;
    }

    /**
     * <p>
     * Get type of Container
     * </p>
     * @type {ContainerType}
     * @returns {ContainerType} Type of Container
     */
    get type() {
        return this.container.type;
    }

    /**
     * <p>
     * Set output type for row field timestamp.
     * <br> The default of output timestsamp is datatime.
     * </p>
     * @type {void}
     * @param {boolean} value - enable timestamp ouput float
     * @returns {void}
     */
    set timestampOutput(value) {
        this.container.timestampOutput = value;
    }

    /**
     * <p>
     * Get output type for row field timestamp.
     * <br> The default of output timestsamp is datatime.
     * </p>
     * @type {boolean}
     * @returns {boolean} PartitionController instance
     */
    get timestampOutput() {
        return this.container.timestampOutput;
    }

    /**
     * <p>
     * Creates a query to execute the specified TQL statement.
     * </p>
     * @type {Query}
     * @param {string} query - TQL statement.
     * @returns {Query} Query instance
     */
    query(strQuery) {
        try {
            return new Query(this.container.query(strQuery), this);
        } catch(err) {
            throw(convertToGSException(err));
        }
    }

    /**
     * <p>
     * Newly creates or update a Row.
     * <br>If a Column exists which corresponds to the specified Row key, it determines whether to newly create or update a Row, based on the Row key and the state of the Container. If there is no corresponding Row in the Container, it determines to newly create a Row; otherwise, it updates a relevant Row.
     * <br>If no Column exists which corresponds to the specified Row key, it always creates a new Row.
     * <br>In the manual commit mode, the target Row is locked.
     * </p>
     * @type {Promise<boolean>}
     * @param {object[]} row=null - A list object representing the content of a Row to be newly created or updated.
     * @returns {Promise<boolean>} True if a Row exists
     */
    put(arr) {
        var this_ = this;
        return new Promise(function(resolve, reject) {
            setTimeout(function() {
                try {
                    resolve(this_.container.put(arr));
                } catch(err) {
                    reject(convertToGSException(err));
                }
            }, 0);
        });
    }

    /**
     * <p>
     * Returns the content of a Row corresponding to Row key.
     * </p>
     * @type {Promise<object[]>}
     * @param {object} key - Row key to be processed
     * @param {boolean} forUpdate - Indicates whether it requests a lock for update or not 
     * @returns {Promise<object>} The list object representing the content of a Row to be obtained
     */
    get(key) {
        var this_ = this;
        return new Promise(function(resolve, reject) {
            setTimeout(function() {
                try {
                    var row = this_.container.get(key);
                    resolve(row);
                } catch(err) {
                    reject(convertToGSException(err));
                }
            }, 0);
        });
    }

    /**
     * <p>
     * Removes a Row corresponding to Row key.
     * <br> In the manual commit mode, the target Row is locked.
     * </p>
     * @type {Promise<boolean>}
     * @param {object} key - Row key to be processed
     * @returns {Promise<boolean>} A boolean-type value which can be used to identify whether the target Row exists or not.
     */
    remove(key) {
        var this_ = this;
        return new Promise(function(resolve, reject) {
            setTimeout(function() {
                try{
                    resolve(this_.container.remove(key));
                } catch(err) {
                    reject(convertToGSException(err));
                }
            }, 0);
        });
    }

    /**
     * <p>
     * Creates a specified type of index on the specified Column.
     * <br>Named index can be set with name parameter.
     * <br>No index cannot be set on a TimeSeries Row key (TIMESTAMP type).
     * <br>When a transaction is held, uncommitted updates will be rolled back.
     * <br>If an index is already set on the specified Column, nothing is changed.
     * <br>When a transaction(s) is active in a target Container, it creates an index after waiting for the transaction(s) to complete.
     * </p>
     * @type {Promise<void>}
     * @param {object} indexInfo - Index information 
     * @param {string} indexInfo.columnName - Column name to be processed
     * @param {IndexType} indexInfo.indexType=DEFAULT - Type of index
     * @param {string} indexInfo.name=null - Name of index
     * @returns {Promise<void>}
     */
    createIndex(parameterObj) {
        var this_ = this;
        return new Promise(function(resolve, reject) {
            setTimeout(function() {
                try {
                    resolve(this_.container.createIndex(parameterObj));
                } catch(err) {
                    reject(convertToGSException(err));
                }
            }, 0);
        });
    }

    /**
     * <p>
     * Removes the specified type of index among indexes on the specified Column.
     * <br>Nothing is changed if the specified index is not found.
     * <br>When a transaction(s) is active in a target Container, it removes the index after waiting for the transaction(s) to be completed.
     * </p>
     * @type {Promise<void>}
     * @param {object} indexInfo - Index information
     * @param {string} indexInfo.columnName - Column name to create index
     * @param {IndexType} indexInfo.indexType=DEFAULT - Type of index
     * @param {string} indexInfo.name=null - Name of index
     * @returns {Promise<void>}
     */
    dropIndex(parameterObj) {
        var this_ = this;
        return new Promise(function(resolve, reject) {
            setTimeout(function() {
                try {
                    resolve(this_.container.dropIndex(parameterObj));
                } catch(err) {
                    reject(convertToGSException(err));
                }
            }, 0);
        });
    }

    /**
     * <p>
     * Writes the results of earlier updates to a non-volatile storage medium, such as SSD, so as to prevent the data loss even if all cluster nodes stop suddenly.
     * <br>It can be used for operations which require higher reliability than usual. However, frequent execution of this operation would potentially cause degradation in response time.
     * <br>The details of behavior, such as the scope of cluster nodes which are the data export targets, will be changed depending on the configuration of GridDB.
     * </p>
     * @type {Promise<void>}
     * @returns {Promise<void>}
     */
    flush() {
        var this_ = this;
        return new Promise(function(resolve, reject) {
            setTimeout(function() {
                try {
                    resolve(this_.container.flush());
                } catch(err) {
                    reject(convertToGSException(err));
                }
            }, 0);
        });
    }

    /**
     * <p>
     * Rolls back the result of the current transaction and starts a new transaction in the manual commit mode.
     * </p>
     * @type {Promise<void>}
     * @returns {Promise<void>}
     */
    abort() {
        var this_ = this;
        return new Promise(function(resolve, reject) {
            setTimeout(function() {
                try {
                    resolve(this_.container.abort());
                } catch(err) {
                    reject(convertToGSException(err));
                }
            }, 0);
        });
    }

    /**
     * <p>
     * Commits the result of the current transaction and start a new transaction in the manual commit mode.
     * </p>
     * @type {Promise<void>}
     * @returns {Promise<void>}
     */
    commit() {
        var this_ = this;
        return new Promise(function(resolve, reject) {
            setTimeout(function() {
                try {
                    resolve(this_.container.commit());
                } catch(err) {
                    reject(convertToGSException(err));
                }
            }, 0);
        });
    }

    /**
     * <p>
     * Change the setting of the commit mode.
     * <br> In the auto commit mode, the transaction state cannot be controlled directly and change operations are committed sequentially. If the auto commit mode is disabled, i.e. in the manual commit mode, as long as the transaction has not timed out or commit() has been invoked directly, the same transaction will continue to be used in this Container and change operations will not be committed.
     * <br> When the auto commit mode is switched from disabled to enabled, uncommitted updates are committed implicitly. Unless the commit mode is changed, the state of the transaction will not be changed.
     * </p>
     * @type {void}
     * @param {boolean} enabled - Indicates whether it enables auto commit mode or not. if True, auto commit mode is enabled. if False, manual commit mode is enabled.
     */
    setAutoCommit(commitFlg) {
        try {
            this.container.setAutoCommit(commitFlg);
        } catch(err) {
            throw(convertToGSException(err));
        }
    }

    /**
     * <p>
     * Newly creates an arbitrary number of Rows together based on the specified Row objects group.
     * <br> In the manual commit mode, the target Row is locked.
     * </p>
     * @type {Promise<void>}
     * @param {object[][]} rowList - List object corresponding to contents of newly created Row collection
     * @returns {Promise<void>}
     */
    multiPut(rowList) {
        var this_ = this;
        return new Promise(function(resolve, reject) {
            setTimeout(function() {
                try {
                    resolve(this_.container.multiPut(rowList));
                } catch(err) {
                    reject(convertToGSException(err));
                }
            }, 0);
        });
    }

    /**
     * <p>
     * Disconnects with GridDB and releases related resources as necessary.
     * </p>
     * @type {Promise<void>}
     * @returns {Promise<void>}
     */
    close() {
        var this_ = this;
        return new Promise(function(resolve, reject) {
            setTimeout(function() {
                try {
                    resolve(this_.container.close());
                } catch(err) {
                    reject(convertToGSException(err));
                }
            }, 0);
        });
    }

    /**
     * <p>
     * Disconnects with GridDB and releases related resources as necessary.
     * </p>
     * @type {void}
     * @returns {void}
     */
    closeSync() {
        try {
            this.container.close();
        } catch(err) {
            throw(convertToGSException(err));
        }
    }
};

/**
 * <p>
 * Provides the functions of holding the information about a query related to a specific Container, specifying the options for fetching and retrieving the result.
 * </p>
 * @alias Query
 * @type {object}
 */
class Query {
    constructor(query, container) {
        this.query = query;
        //Need reference to avoid gc
        this.container = container;
    }

    /**
     * <p>
     * It locks all target Rows if True is specified as forUpdate . If the target Rows are locked, update operations on the Rows by any other transactions are blocked while a relevant transaction is active. True can be specified only if the auto commit mode is disabled on a relevant Container.
     * <br> When new set of Rows are obtained, any Row operation via RowSet as the last result of specified query is prohibited.
     * <br> If the system tries to acquire a large number of Rows all at once, the upper limit of the communication buffer size managed by the GridDB node may be reached, possibly resulting in a failure. Refer to "System limiting values" in the Appendix of GridDB API Reference for the upper limit size.
     * </p>
     * @type {Promise<RowSet>}
     * @returns {Promise<RowSet>} RowSet instance
     */
    fetch() {
        var this_ = this;
        var forUpdate = false;
        return new Promise(function(resolve, reject) {
          setTimeout(function() {
              try {
                  var rowSet = this_.query.fetch(forUpdate);
                  resolve(new RowSet(rowSet, this_));
              } catch(err) {
                  reject(convertToGSException(err));
              }
          }, 0);
        });
    }

    /**
     * <p>
     * Releases related resources properly.
     * </p>
     * @type {Promise<void>}
     * @returns {Promise<void>}
     */
    close() {
        var this_ = this;
        return new Promise(function(resolve, reject) {
            setTimeout(function() {
                try {
                    resolve(this_.query.close());
                } catch(err) {
                    reject(convertToGSException(err));
                }
            }, 0);
        });
    }

    /**
     * <p>
     * Returns RowSet as the latest result.
     * <br> Once RowSet is returned, it cannot be obtained until the new query is executed.
     * </p>
     * @type {RowSet}
     * @returns {RowSet} RowSet instance as the latest result
     */
    getRowSet() {
        try {
            return new RowSet(this.query.getRowSet(), this);
        } catch(err) {
            throw(convertToGSException(err));
        }
    }

    /**
     * <p>
     * Sets an fetch options for a result acquisition.
     * <br> Once RowSet is returned, it cannot be obtained until the new query is executed.
     * </p>
     * @type {void}
     * @param {object} options - The options for fetching the result of a query
     * @param {number} options.limit=null - Used to set the maximum number of Rows to be fetched
     */
    setFetchOptions(option) {
        try {
            this.query.setFetchOptions(option);
        } catch (err) {
            throw(convertToGSException(err));
        }
    }

    /**
     * <p>
     * Releases related resources properly.
     * </p>
     * @type {void}
     * @returns {void}
     */
    closeSync() {
        try {
            this.query.close();
        } catch(err) {
            throw(convertToGSException(err));
        }
    }
};

/**
 * <p>
 * Manages a set of Rows obtained by a query.
 * <br> It has a function of per-Row and per-Row-field manipulation and holds a cursor state to specify a target Row. The cursor is initially located just before the head of a Row set.
 * </p>
 * @type {object}
 * @alias RowSet
 * @property {RowSetType} type=CONTAINER_ROWS - The type of content that can be extracted from RowSet. (Read-only attribute)
 * @property {number} size=null - The size of Row set. (Read-only attribute)
 */
class RowSet {
    constructor(rowSet, query) {
        this.rowSet = rowSet;
        //Need reference to avoid gc
        this.query = query;
    }

    /**
     * <p>
     * Get size of row set
     * <br> The number of Row when a Row set is created
     * </p>
     * @type {number}
     * @returns {number} The size of row set
     */
    get size() {
        return this.rowSet.size;
    }

    /**
     * <p>
     * Get type of row set
     * </p>
     * @type {RowSetType}
     * @returns {RowSetType} The type of row set
     */
    get type() {
        return this.rowSet.type;
    }

    /**
    * <p>
    * Set output type for row field timestamp.
    * <br> The default of output timestsamp is datatime.
    * </p>
    * @type {void}
    * @param {boolean} value - enable timestamp ouput float
    * @returns {void}
    */
    set timestampOutput(value) {
        this.rowSet.timestampOutput = value;
    }

    /**
     * <p>
     * Get output type for row field timestamp.
     * <br> The default of output timestsamp is datatime.
     * </p>
     * @type {boolean}
     * @returns {boolean} PartitionController instance
     */
    get timestampOutput() {
        return this.rowSet.timestampOutput;
    }

    /**
     * <p>
     * Releases related resources as necessary.
     * </p>
     * @type {Promise<void>}
     * @returns {Promise<void>}
     */
    close() {
        var this_ = this;
        return new Promise(function(resolve, reject) {
            setTimeout(function() {
                try {
                    resolve(this_.rowSet.close());
                } catch(err) {
                    reject(convertToGSException(err));
                }
            }, 0);
        });
    }

    /**
     * <p>
     * Moves the cursor to the next Row in a Row set and returns the Row object at the moved position.
     * </p>
     * @type {object}
     * @returns {object[] | AggregationResult | QueryAnalysisEntry} The object that can be extracted from RowSet.
     */
    next() {
        var this_ = this;
        switch(this.type) {
        case griddb.GS_ROW_SET_CONTAINER_ROWS:
            try {
                return this.rowSet.next();
            } catch(err) {
                throw(convertToGSException(err));
            }
            break;
        case griddb.GS_ROW_SET_AGGREGATION_RESULT:
            try {
                return new AggregationResult(this.rowSet.next(),this_);
            } catch(err) {
                throw(convertToGSException(err));
            }
            break;
        case griddb.GS_ROW_SET_QUERY_ANALYSIS:
            try {
                return new QueryAnalysisEntry(this.rowSet.next(), this_);
            } catch(err) {
                throw(convertToGSException(err));
            }
            break;

        default:
            throw "Invalid Rowset type";
            break;
        }

    }

    /**
     * <p>
     * Returns whether a Row set has at least one Row ahead of the current cursor position.
     * </p>
     * @type {boolean}
     * @returns {boolean} True if a Row exists ahead of the current cursor position
     */
    hasNext() {
        try {
            return this.rowSet.hasNext();
        } catch(err) {
            throw(convertToGSException(err));
        }
    }

    /**
     * <p>
     * Releases related resources as necessary.
     * </p>
     * @type {void}
     */
    closeSync() {
        try {
            this.rowSet.close();
        } catch(err) {
            throw(convertToGSException(err));
        }
    }
}

/**
 * <p>
 * Stores the result of an aggregation operation.
 * <br>The type of the stored result depends on the type of aggregation operation and the type of the target Columns.
 * For specific rules, see the TQL specifications.
 * <br>The type of obtaining value depends on the stored type.
 * Float type and long type are only available when a result is of numeric type, and datetime type when a result is of TIMESTAMP type.
 * </p>
 * @type {object}
 * @alias AggregationResult
 */
class AggregationResult {
    constructor(aggResult, store) {
        this.aggResult = aggResult;
        //need reference to avoid gc
        this.store = store;
    }

    /**
    * <p>
    * Set output type for row field timestamp.
    * <br> The default of output timestsamp is datatime.
    * </p>
    * @type {void}
    * @param {boolean} value - enable timestamp ouput float
    * @returns {void}
    */
    set timestampOutput(value) {
        this.aggResult.timestampOutput = value;
    }

    /**
     * <p>
     * Get output type for row field timestamp.
     * <br> The default of output timestsamp is datatime.
     * </p>
     * @type {boolean}
     * @returns {boolean} PartitionController instance
     */
    get timestampOutput() {
        return this.aggResult.timestampOutput;
    }

    /**
     * <p>
     * Returns the aggregation result as the value with specified type.
     * </p>
     * @param {Type} type - Column type
     * @returns {object} The aggregation result
     */
    get(type) {
        try {
            return this.aggResult.get(type);
        } catch(err) {
            throw(convertToGSException(err));
        }
    }
}

/**
 * <p>
 * Represents one of information entries composing a query plan and the results of analyzing a query operation.
 * </p>
 * @type {object}
 * @alias QueryAnalysisEntry
 */
class QueryAnalysisEntry {
    constructor(queryAnalysis, store) {
        this.queryAnalysis = queryAnalysis;
        //need reference to avoid gc
        this.store = store;
    }

    /**
     * <p>
     * Returns one of information entries composing a query plan and the results of analyzing a query operation.
     * </p>
     * @type {Array}
     * @returns {Array} Query plan and the results of analyzing a query operation
     */
    get() {
        try {
            return this.queryAnalysis.get();
        } catch(err) {
            throw(convertToGSException(err));
        }
    }
}

/**
 * <p>
 * Controller for acquiring and processing the partition status.
 * <br>A partition is a theoretical region where data is stored. It is used to perform operations based on the data arrangement in a GridDB cluster.
 * </p>
 * @type {object}
 * @alias PartitionController
 * @property {number} partitionCount=null - The number of partitions in the target GridDB cluster. (Read-only attribute)
 */
class PartitionController {
    constructor(partitionController, store) {
        this.partitionController = partitionController;
        //need reference to avoid gc
        this.store = store;
    }

    /**
     * <p>
     * Get partition count
     * <br> Get the number of partitions in the target GridDB cluster.
     * </p>
     * @type {number}
     * @returns {number} The number of partitions in the target GridDB cluster
     */
    get partitionCount() {
        return this.partitionController.partitionCount;
    }

    /**
     * <p>
     * Get the total number of containers belonging to a specified partition.
     * <br> The calculated quantity when determining the number of containers is generally not dependent on the number of containers.
     * </p>
     * @type {Promise<number>}
     * @param {number} partitionIndex - The partition index, from 0 to the number of partitions minus one
     * @returns {Promise<number>} The number of Container
     */
    getContainerCount(partitionIndex) {
        var this_ = this;
        return new Promise(function(resolve, reject) {
            setTimeout(function() {
                try {
                    resolve(this_.partitionController.getContainerCount(partitionIndex));
                } catch(err) {
                    reject(convertToGSException(err));
                }
            }, 0);
        });
    }

    /**
     * <p>
     * Get a list of the Container names belonging to a specified partition.
     * <br> For the specified partition, the sequence of the list of acquisition results before and after will not be changed when the relevant Container is excluded even if a Container is newly created, its composition changed or the Container is deleted. All other lists are compiled in no particular order. No duplicate names will be included.
     * <br> If the upper limit of the number of acquisition cases is specified, the cases will be cut off starting from the ones at the back if the upper limit is exceeded. If no relevant specified condition exists, a blank list is returned.
     * </p>
     * @type {Promise<string[]>}
     * @param {number} partitionIndex - The partition index, from 0 to the number of partitions minus one
     * @param {number} start - The start position of the acquisition range. A value must be greater than or equal to 0
     * @param {number} limit - The upper limit of the number of cases acquired. It is optional. If not specified or limit < 0, it is considered as no upper limit.
     * @returns {Promise<string[]>} The list of Container name
     */
    getContainerNames(partitionIndex, start, limit) {
        var this_ = this;
        return new Promise(function(resolve, reject) {
            setTimeout(function() {
                try {
                    resolve(this_.partitionController.getContainerNames(partitionIndex, start, limit));
                } catch(err) {
                    reject(convertToGSException(err));
                }
            }, 0);
        });
    }

    /**
     * <p>
     * Get the partition index corresponding to the specified Container name.
     * <br> Once a GridDB cluster is constructed, there will not be any changes in the partitions of the destination that the Container belongs to and the partition index will also be fixed. Whether there is a Container corresponding to the specified name or not does not depend on the results.
     * <br> Information required in the computation of the partition index is cached and until the next cluster failure and cluster node failure is detected, no inquiry will be sent to the GridDB cluster again.
     * </p>
     * @type {Promise<number>}
     * @param {string} containerName - Container name
     * @returns {Promise<number>} The partition index
     */
    getPartitionIndexOfContainer(containerName) {
        var this_ = this;
        return new Promise(function(resolve, reject) {
            setTimeout(function() {
                try {
                    resolve(this_.partitionController.getPartitionIndexOfContainer(containerName));
                } catch(err) {
                    reject(convertToGSException(err));
                }
            }, 0);
        });
    }

    /**
     * <p>
     * The connection status with GridDB is released and related resources are released where necessary.
     * </p>
     * @type {Promise<void>}
     * @returns {Promise<void>}
     */
    close() {
        var this_ = this;
        return new Promise(function(resolve, reject) {
            setTimeout(function() {
                try {
                    resolve(this_.partitionController.close());
                } catch(err) {
                    reject(convertToGSException(err));
                }
            }, 0);
        });
    }

    /**
     * <p>
     * The connection status with GridDB is released and related resources are released where necessary.
     * </p>
     * @type {void}
     */
    closeSync() {
        try {
            this.partitionController.close();
        } catch(err) {
            throw(convertToGSException(err));
        }
    }
}

/**
 * <p>
 * Represents the condition that a row key satisfies.
 * <br> This is used as the search condition in Store.multiGet().
 * <br> There are two types of conditions, range condition and individual condition. The two types of conditions cannot be specified at the same time. If the condition is not specified, it means that the condition is satisfied in all the target row keys.
 * </p>
 * @type {object}
 * @alias RowKeyPredicate
 * @property {Type} keyType - The type of Row key used as a search condition. (Read-only attribute)
 */
class RowKeyPredicate {
    constructor(rowKeyPredicate) {
        this.rowKeyPredicate = rowKeyPredicate;
    }

    /**
     * <p>
     * Get type of RowkeyPredicate
     * </p>
     * @type {Type}
     * @returns {Type} The type of RowkeyPredicate
     */
    get keyType() {
        return this.rowKeyPredicate.keyType;
    }

    /**
     * <p>
     * Returns the value of Row key at the start and end position of the range condition.
     * </p>
     * @type {Array.<object, object>}
     * @returns {Array.<object, object>} The value of Row key at the start and end position of the range condition
     */
    getRange() {
        try {
            return this.rowKeyPredicate.getRange();
        } catch(err) {
            throw(convertToGSException(err));
        }
    }

    /**
     * <p>
     * Sets the value of Row key as the start and end position of the range conditions.
     * </p>
     * @type {void}
     * @param {object} start - The value of the Row key as the start position
     * @param {object} end - The value of the Row key as the end position
     */
    setRange(start, end) {
        try {
            this.rowKeyPredicate.setRange(start, end);
        } catch(err) {
            throw(convertToGSException(err));
        }
    }

    /**
     * <p>
     * Returns a list of the values of the Row keys that configure the individual condition.
     * </p>
     * @type {void}
     * @param {object[]} keys - List of the elements in the individual condition
     */
    setDistinctKeys(keys) {
        try {
            this.rowKeyPredicate.setDistinctKeys(keys);
        } catch(err) {
            throw(convertToGSException(err));
        }
    }

    /**
     * <p>
     * Sets list of the elements in the individual condition.
     * </p>
     * @type {Array.<object>}
     * @returns {object[]} List of the values of the Row keys that configure the individual condition
     */
    getDistinctKeys() {
        try {
            return this.rowKeyPredicate.getDistinctKeys();
        } catch(err) {
            throw(convertToGSException(err));
        }
    }
}

/**
 * <p>
 * Represents the exception for GridDB.
 * </p>
 * @type {object}
 * @alias GSException
 * @property {boolean} isTimeout=false - Read-only attribute to determine whether the result of the requested process shows the error code corresponding to the error that occurred when the requested process is not completed within a predetermined time
 */
class GSException{
    constructor(exception) {
        this.exception = exception;
    }

    /**
     * <p>
     *  Determine whether the result of the requested process shows the error code corresponding to the error that occurred when the requested process is not completed within a predetermined time.
     * </p>
     * @type {boolean}
     * @returns {boolean} Indicates whether the corresponding error code or not
     */
    isTimeout() {
        try {
            return this.exception.isTimeout;
        } catch(err) {
            throw(convertToGSException(err));
        }
    }

    /**
     * <p>
     * Returns the stack size of last error information related to this resource.
     * <br> Error information has become a stack structure. A large stack number corresponds to the more direct cause of the error.
     * </p>
     * @type {number}
     * @returns {number} The stack size. 0 is returned if corresponding information can not be obtained 
     */
    getErrorStackSize() {
        try {
            return this.exception.getErrorStackSize();
        } catch(err) {
            throw(convertToGSException(err));
        }
    }

    /**
     * <p>
     * Returns the error code of last error related to this resource.
     * <br> 0 indicates the successful execution for GridDB instructions.
     * </p>
     * @type {number}
     * @param {number} stackIndex - The index of error stack. A valid result will be returned only if a value which has more than 0 and less than the stack size is specified.
     * @returns {number} The error code
     */
    getErrorCode(stackIndex) {
        try {
            return this.exception.getErrorCode(stackIndex);
        } catch(err) {
            throw(convertToGSException(err));
        }
    }

    /**
     * <p>
     * Returns the message of last error related to this resource.
     * </p>
     * @type {string}
     * @param {number} stackIndex - The index of error stack. A valid result will be returned only if a value which has more than 0 and less than the stack size is specified.
     * @returns {string} The error message
     */
    getMessage(stackIndex) {
        try {
            return this.exception.getMessage(stackIndex);
        } catch(err) {
            throw(convertToGSException(err));
        }
    }

    /**
     * <p>
     * Returns the error location of the internal module to the message of last error related to this resource.
     * <br> It might always return empty string depending on the settings.
     * </p>
     * @type {string}
     * @param {number} stackIndex - The index of error stack. A valid result will be returned only if a value which has more than 0 and less than the stack size is specified.
     * @returns {string} The string to store the error location information.
     */
    getLocation(stackIndex) {
        try {
            return this.exception.getLocation(stackIndex);
        } catch(err) {
            throw(convertToGSException(err));
        }
    }

    /**
     * <p>
     * Returns the message include error code and error message
     * </p>
     * @type {string}
     * @returns {string} The error message and error message
     */
    what() {
        try {
            return this.exception.what();
        } catch(err) {
            throw(convertToGSException(err));
        }
    }
    
    /**
     * <p>
     * Disconnects with GridDB and releases related resources as necessary.
     * </p>
     * @type {void}
     * @returns {void}
     */
    close() {
        this.exception.close();
    }
}

/**
 * <p>
 * Correct class name GSException.
 * </p>
 * @type {GSException}
 * @returns {GSException} GSException instance
 */
function convertToGSException(exception) {
    if (exception.constructor.name == "_exports_GSException") {
        return new GSException(exception);
    }
    return exception;
}

var x = {
        StoreFactory: StoreFactory
};

for (var key in x) {
    griddb[key] = x[key];
}

module.exports = griddb;
