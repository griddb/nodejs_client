var griddb = require('griddb_node');
var fs     = require('fs');

var factory = griddb.StoreFactory.getInstance();
var store = factory.getStore({
                        "host": process.argv[2], 
                        "port": process.argv[3], 
                        "cluster_name": process.argv[4], 
                        "username": process.argv[5], 
                        "password": process.argv[6]});

var conInfo = new griddb.ContainerInfo("col01",
                   [
                        ["name", griddb.GS_TYPE_STRING],
                        ["status", griddb.GS_TYPE_BOOL],
                        ["count", griddb.GS_TYPE_LONG],
                        ["lob", griddb.GS_TYPE_BLOB]
                   ], griddb.GS_CONTAINER_COLLECTION,
                   true)

var col2;
store.putContainer(conInfo, false)
	.then(col => {
	 	col2 = col;
		col.createIndex("count", griddb.GS_INDEX_FLAG_DEFAULT);
		return col;
  	})
	.then(col => {
		col.setAutoCommit(false);
		col.put(["name01", false, 1, "ABCDEFGHIJ"]);
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
		console.log(err.what());
		for (var i = 0; i < err.getErrorStackSize(); i++) {
			console.log("[", i, "]");
			console.log(err.getErrorCode(i));
			console.log(err.getMessage(i));
		}
	});
