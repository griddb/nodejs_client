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
    }

    // Async functions
    putContainer(info, modifiable = false) {
        var this_ = this;
        return new Promise(function(resolve, reject) {
            setTimeout(function(){
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

    // Sync functions

};

class Container {
    constructor(container) {
        this.container = container;
    }

    get type() {
        return this.container.type;
    }

    // Async functions
    put(arr) {
        var this_ = this;
        return new Promise(function(resolve, reject) {
            setTimeout(function(){
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
            setTimeout(function(){
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

    createIndex(columnName, indexType = griddb.GS_INDEX_FLAG_DEFAULT , name = null) {
        var this_ = this;
        return new Promise(function(resolve, reject) {
            setTimeout(function() {
                try {
                    if (name == null) {
                        resolve(this_.container.createIndex(columnName, indexType));
                    } else {
                        resolve(this_.container.createIndex(columnName, indexType, name));
                    }
                } catch(err) {
                    reject(convertToGSException(err));
                }
            }, 0);
        });
    }

    dropIndex(columnName, indexType = griddb.GS_INDEX_FLAG_DEFAULT , name = null) {
        var this_ = this;
        return new Promise(function(resolve, reject) {
            setTimeout(function() {
                try {
                    if (name == null) {
                        resolve(this_.container.dropIndex(columnName, indexType));
                    } else {
                        resolve(this_.container.dropIndex(columnName, indexType, name));
                    }
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
};

class Query {
    constructor(query) {
        this.query = query;
    }
    // Async functions
    fetch() {
        var this_ = this;
        return new Promise(function(resolve, reject) {
          setTimeout(function(){
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
}

class AggregationResult {
    constructor(aggResult) {
        this.aggResult = aggResult;
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
