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
var containerName = 'Sample_timeseries1';
var conInfo = new griddb.ContainerInfo({
    'name': containerName,
    'columnInfoList': [
        ["date", griddb.Type.TIMESTAMP],
        ["value", griddb.Type.DOUBLE]
    ],
    'type': griddb.ContainerType.TIME_SERIES
});

store.dropContainer(containerName)
    .then(() => {
        return store.putContainer(conInfo);
    })
    .then(() => {
        console.log("Create TimeSeries name=%s", containerName);
        console.log('Success!')
        return true;
    })
    .catch(err => {
        if (err.constructor.name == 'GSException') {
            for (var i = 0; i < err.getErrorStackSize(); i++) {
                console.log("[%d]", i);
                console.log(err.getErrorCode(i));
                console.log(err.getMessage(i));
            }
        } else {
            console.log(err);
        }
    });
