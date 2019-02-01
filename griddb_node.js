var griddb = require('griddb_client');
var exports = module.exports;

griddb['StoreFactory_'] = griddb.StoreFactory;

class StoreFactory {
    constructor(factory) {
        this.factory = factory;
    }
    getStore(obj) {
        return new Store(this.factory.getStore(obj));
    }

    static getInstance() {
        return new StoreFactory(griddb.StoreFactory_.getInstance());
    }
    getVersion() {
        return this.factory.getVersion();
    }
};

class Store {
    constructor(store) {
        this.store = store;
        this.pController = null;
    }
    get partitionController() {
        if (this.pController == null) {
            this.pController = new PartitionController(this.store.partitionInfo);
        }
        return this.pController;
    }

    set timestampOutput(value) {
        this.store.timestamp_output_with_float = value;
    }

    get timestampOutput() {
        return this.store.timestamp_output_with_float;
    }

    // Async functions
    putContainer(info, modifiable = false) {
        var this_ = this;
        return new Promise(function(resolve, reject) {
            setTimeout(function() {
                try{
                    var container = this_.store.putContainer(info, modifiable);
                    resolve(new Container(container));
                } catch(err) {
                    reject(convertToGSException(err));
                }
            }, 0);
        });
    }

    getContainer(name) {
        var this_ = this;
        return new Promise(function(resolve, reject) {
            setTimeout(function() {
                try {
                    var container = this_.store.getContainer(name);
                    resolve(new Container(container));
                } catch(err) {
                    reject(convertToGSException(err));
                }
            }, 0);
        });
    }

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
                        queryListTmp.push(queryListRet[i].query);
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

    multiGet(predicateEntry) {
        var this_ = this;
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

    close(allRelated = false) {
        var this_ = this;
        return new Promise(function(resolve, reject) {
            setTimeout(function() {
                try {
                    resolve(this_.store.close(allRelated));
                } catch(err) {
                    reject(convertToGSException(err));
                }
            }, 0);
        });
    }

    // Sync functions
    createRowKeyPredicate(type) {
        try {
            return this.store.createRowKeyPredicate(type);
        } catch(err) {
            throw(convertToGSException(err));
        }
    }

    closeSync(allRelated = false) {
        try {
            this.store.close(allRelated);
        } catch(err) {
            throw(convertToGSException(err));
        }
    }

};

class Container {
    constructor(container) {
        this.container = container;
    }

    get type() {
        return this.container.type;
    }

    set timestampOutput(value) {
        this.container.timestamp_output_with_float = value;
    }

    get timestampOutput() {
        return this.container.timestamp_output_with_float;
    }

    // Async functions
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

    // Sync functions
    query(strQuery) {
        try {
            return new Query(this.container.query(strQuery));
        } catch(err) {
            throw(convertToGSException(err));
        }
    }

    setAutoCommit(commitFlg) {
        try {
            this.container.setAutoCommit(commitFlg);
        } catch(err) {
            reject(convertToGSException(err));
        }
    }

    closeSync() {
        try {
            this.container.close();
        } catch(err) {
            throw(convertToGSException(err));
        }
    }
};

class Query {
    constructor(query) {
        this.query = query;
    }
    // Async functions
    fetch() {
        var this_ = this;
        return new Promise(function(resolve, reject) {
          setTimeout(function() {
              try {
                  var rowSet = this_.query.fetch(false);
                  resolve(new RowSet(rowSet));
              } catch(err) {
                  reject(convertToGSException(err));
              }
          }, 0);
        });
    }

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

    // Sync functions
    getRowSet() {
        try {
            return new RowSet(this.query.getRowSet());
        } catch(err) {
            throw(convertToGSException(err));
        }
    }

    setFetchOptions(option) {
        try {
            this.query.setFetchOptions(option);
        } catch (err) {
            throw(convertToGSException(err));
        }
    }

    closeSync() {
        try {
            this.query.close();
        } catch(err) {
            throw(convertToGSException(err));
        }
    }
};

class RowSet {
    constructor(rowSet) {
        this.rowSet = rowSet;
    }

    get size() {
        return this.rowSet.size;
    }

    get type() {
        return this.rowSet.type;
    }

    set timestampOutput(value) {
        this.rowSet.timestamp_output_with_float = value;
    }

    get timestampOutput() {
        return this.rowSet.timestamp_output_with_float;
    }

    // Async functions
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

    // Sync functions
    next() {
        try {
            return this.rowSet.next();
        } catch(err) {
            throw(convertToGSException(err));
        }
    }

    hasNext() {
        try {
            return this.rowSet.hasNext();
        } catch(err) {
            throw(convertToGSException(err));
        }
    }

    closeSync() {
        try {
            this.rowSet.close();
        } catch(err) {
            throw(convertToGSException(err));
        }
    }
}

class AggregationResult {
    constructor(aggResult) {
        this.aggResult = aggResult;
    }
    set timestampOutput(value) {
        this.aggResult.timestamp_output_with_float = value;
    }

    get timestampOutput() {
        return this.aggResult.timestamp_output_with_float;
    }

    // Sync function
    get(type) {
        try {
            return this.aggResult.get(type);
        } catch(err) {
            throw(convertToGSException(err));
        }
    }
}

class QueryAnalysisEntry {
    constructor(queryAnalysis) {
        this.queryAnalysis = queryAnalysis;
    }

    // Sync function
    get() {
        try {
            return this.queryAnalysis.get();
        } catch(err) {
            throw(convertToGSException(err));
        }
    }
}

class PartitionController {
    constructor(partitionController) {
        this.partitionController = partitionController;
    }
    get partitionCount() {
        return this.partitionController.partitionCount;
    }
    // Async function
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

    // Sync function
    closeSync() {
        try {
            this.partitionController.close();
        } catch(err) {
            throw(convertToGSException(err));
        }
    }
}

class RowKeyPredicate {
    constructor(rowKeyPredicate) {
        this.rowKeyPredicate = rowKeyPredicate;
    }
    get keyType() {
        return this.rowKeyPredicate.keyType;
    }

    // Sync function
    getRange() {
        try {
            return this.rowKeyPredicate.getRange();
        } catch(err) {
            throw(convertToGSException(err));
        }
    }
    setRange(start, end) {
        try {
            this.rowKeyPredicate.setRange(start, end);
        } catch(err) {
            throw(convertToGSException(err));
        }
    }
    setDistinctKeys(keys) {
        try {
            this.rowKeyPredicate.setDistinctKeys(keys);
        } catch(err) {
            throw(convertToGSException(err));
        }
    }
    getDistinctKeys() {
        try {
            return this.rowKeyPredicate.getDistinctKeys();
        } catch(err) {
            throw(convertToGSException(err));
        }
    }
}

class GSException{
    constructor(exception) {
        this.exception = exception;
    }
    // Sync function
    isTimeout() {
        return this.exception.isTimeout();
    }
    getErrorStackSize() {
        return this.exception.getErrorStackSize();
    }
    getErrorCode(stackIndex) {
        return this.exception.getErrorCode(stackIndex);
    }
    getMessage(stackIndex) {
        return this.exception.getMessage(stackIndex);
    }
    getLocation() {
        return this.exception.getLocation(stackIndex);
    }
    what() {
        return this.exception.what();
    }
}

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
