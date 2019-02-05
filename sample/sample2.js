var griddb = require('griddb_node');
var fs = require('fs');

var factory = griddb.StoreFactory.getInstance();
var store = factory.getStore({
    "host": process.argv[2],
    "port": parseInt(process.argv[3]),
    "clusterName": process.argv[4],
    "username": process.argv[5],
    "password": process.argv[6]
});

var conInfo = new griddb.ContainerInfo({
    'name': "point01",
    'columnInfoList': [
        ["timestamp", griddb.Type.TIMESTAMP],
        ["active", griddb.Type.BOOL],
        ["voltage", griddb.Type.DOUBLE]
    ],
    'type': griddb.ContainerType.TIME_SERIES, 'rowKey': true
});

var time_series;
store.putContainer(conInfo, false)
    .then(ts => {
        time_series = ts;
        return ts.put([new Date(), false, 100]);
    })
    .then(() => {
        query = time_series.query("select * where timestamp > TIMESTAMPADD(HOUR, NOW(), -6)");
        return query.fetch();
    })
    .then(rowset => {
        while (rowset.hasNext()) {
            var row = rowset.next();
            console.log("Time =", row[0], "Active =", row[1].toString(), "Voltage =", row[2]);
        }
    })
    .catch(err => {
        if (err.constructor.name == "GSException") {
            for (var i = 0; i < err.getErrorStackSize(); i++) {
                console.log("[", i, "]");
                console.log(err.getErrorCode(i));
                console.log(err.getMessage(i));
            }
        } else {
            console.log(err);
        }
    });
