var griddb = require('griddb_node');
var fs     = require('fs');

var factory = griddb.StoreFactory.getInstance();
var store = factory.getStore({
                        "host": process.argv[2], 
                        "port": parseInt(process.argv[3]),
                        "clusterName": process.argv[4], 
                        "username": process.argv[5], 
                        "password": process.argv[6]});

var conInfo = new griddb.ContainerInfo({'name' : "col01",
    'columnInfoList' : [
            ["name", griddb.Type.STRING],
            ["status", griddb.Type.BOOL],
            ["count", griddb.Type.LONG],
            ["lob", griddb.Type.BLOB]
        ], 
        'type' : griddb.ContainerType.COLLECTION, 'rowKey' : true});

var col2;
store.putContainer(conInfo, false)
	.then(col => {
	 	col2 = col;
	 	col.createIndex({'columnName' : 'count', 'indexType': griddb.IndexType.DEFAULT});
		return col;
  	})
	.then(col => {
		col.setAutoCommit(false);
		col.put(["name01", false, 1, Buffer.from([65, 66, 67, 68, 69, 70, 71, 72, 73, 74])]);
		return col;
	})
	.then(col => {
		col.commit();
		return col;
	})
	.then(col => {
		query = col.query("select *");
		return query.fetch();
	})
	.then(rs => {
		while (rs.hasNext()) {
			console.log(rs.next());
		}
		col2.commit();
	})
	.catch(err => {
		if (err.constructor.name == "_exports_GSException") {
			console.log(err.what());
			for (var i = 0; i < err.getErrorStackSize(); i++) {
				console.log("[", i, "]");
				console.log(err.getErrorCode(i));
				console.log(err.getMessage(i));
			}
		} else {
			console.log(err);
		}
	});
