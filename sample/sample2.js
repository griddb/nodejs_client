
var griddb = require('griddb_node');
var fs     = require('fs');

var factory = griddb.StoreFactory.getInstance();
var store = factory.getStore({
                        "host": process.argv[2], 
                        "port": parseInt(process.argv[3]),
                        "clusterName": process.argv[4], 
                        "username": process.argv[5], 
                        "password": process.argv[6]});

var conInfo = new griddb.ContainerInfo({'name' : "point01",
    'columnInfoList' : [
            ["timestamp", griddb.GS_TYPE_TIMESTAMP],
            ["active", griddb.GS_TYPE_BOOL],
            ["voltage", griddb.GS_TYPE_DOUBLE]
        ], 
        'type' : griddb.ContainerType.TIME_SERIES, 'rowKey' : true});

store.putContainer(conInfo, false)
	.then(ts => {
        var date = new Date();
        ts.put([date, false, 100]);
        return ts;
    })
	.then(ts => {
        query = ts.query("select * where timestamp > TIMESTAMPADD(HOUR, NOW(), -6)");
        return query.fetch();
    })
	.then(rowset => {
        var row;
        while (rowset.hasNext()) {
            row = rowset.next();
            console.log("Time =", row[0], "Active =", row[1].toString(), "Voltage =", row[2]);
        }
    })
    .catch(err => {
        var errMsg;
        if (err.constructor.name == "_exports_GSException") {
            errMsg = err.what();
        } else {
            errMsg = err;
        }
        console.log(errMsg);
    });
