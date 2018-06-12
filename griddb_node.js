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
					reject(err);
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
					reject(err);
				}
			}, 0);
		});
	}
	
	dropContainer(name) {
		var this_ = this;
		return new Promise(function(resolve, reject) {
			setTimeout(function() {
				try {
					this_.store.dropContainer(name);
				} catch(err) {
					reject(err);
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
					reject(err);
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
	    			this_.container.put(arr);
	    			resolve("Put row successfully");
	    		} catch(err) {
	    			reject(err);
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
	    			reject(err);
	    		}
	    	}, 0);
	    });
	}
	
	remove(key) {
		var this_ = this;
		return new Promise(function(resolve, reject) {
			setTimeout(function() {
				try{
					this_.container.remove(key);
					resolve("Remove row successfully");
				} catch(err) {
					reject(err);
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
						this_.container.createIndex(columnName, indexType);
					} else {
						this_.container.createIndex(columnName, indexType, name);
					}
					resolve("Create index successfully");
				} catch(err) {
					reject(err);
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
						this_.container.dropIndex(columnName, indexType);
					} else {
						this_.container.dropIndex(columnName, indexType, name);
					}
					resolve("Drop index successfully");
				} catch(err) {
					reject(err);
				}
			}, 0);
		});
	}
	
	flush() {
		var this_ = this;
		return new Promise(function(resolve, reject) {
			setTimeout(function() {
				try {
					this_.container.flush();
					resolve("Flush successfully");
				} catch(err) {
					reject(err);
				}
			}, 0);
		});
	}
	
	abort() {
		var this_ = this;
		return new Promise(function(resolve, reject) {
			setTimeout(function() {
				try {
					this_.container.abort();
					resolve("Abort successfully");
				} catch(err) {
					reject(err);
				}
			}, 0);
		});
	}
	
	commit() {
		var this_ = this;
		return new Promise(function(resolve, reject) {
			setTimeout(function() {
				try {
					this_.container.commit();
					resolve("Commit successfully");
				} catch(err) {
					reject(err);
				}
			}, 0);
		});
	}
	
	close() {
		var this_ = this;
		return new Promise(function(resolve, reject) {
			setTimeout(function() {
				try {
					this_.container.close();
					resolve("Close successfully");
				} catch(err) {
					reject(err);
				}
			}, 0);
		});
	}
	
	// Sync functions
	query(strQuery) {
		try {
			return new Query(this.container.query(strQuery));
		} catch(err) {
			throw(err);
		}
	}
	
	setAutoCommit(commitFlg) {
		try {
			this.container.setAutoCommit(commitFlg);
		} catch(err) {
			throw(err);
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
	    		  reject(err);
	    	  }
	      }, 0);
	    });
	}
	
	close() {
		var this_ = this;
		return new Promise(function(resolve, reject) {
			setTimeout(function() {
				try {
					this_.query.close();
					resolve("Close successfully");
				} catch(err) {
					reject(err);
				}
			}, 0);
		});
	}
	
	// Sync functions
	getRowSet() {
		try {
			return new RowSet(this.query.getRowSet());
		} catch(err) {
			throw(err);
		}
	}
	
	setFetchOption(limit = null) {
		try {
			this.query.setFetchOption(limit);
		} catch (err) {
			throw(err);
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
					this_.rowSet.close();
					resolve("Close successfully");
				} catch(err) {
					reject(err);
				}
			}, 0);
		});
	}
	
	// Sync functions
	next() {
		try {
			return this.rowSet.next();
		} catch(err) {
			throw(err);
		}
	}
	
	hasNext() {
		try {
			return this.rowSet.hasNext();
		} catch(err) {
			throw(err);
		}
	}

}

class AggregationResult {
	constructor(aggResult) {
		this.aggResult = aggResult;
	}
	
	// Async functions
	
	// Sync function
	get(type) {
		try {
			return this.aggResult.get(type);
		} catch(err) {
			throw(err);
		}
	}
}

class QueryAnalysis {
	constructor(queryAnalysis) {
		this.queryAnalysis = queryAnalysis;
	}
	
	// Async function

	// Sync function
	get() {
		try {
			return this.queryAnalysis.get();
		} catch(err) {
			throw(err);
		}
	}
}

var x = {
		StoreFactory: StoreFactory,
		Store: Store,
		Container: Container,
		Query: Query,
		AggregationResult: AggregationResult,
		QueryAnalysis: QueryAnalysis,
		RowSet: RowSet,
}

for (var key in x) {
	griddb[key] = x[key];
}

module.exports = griddb;
