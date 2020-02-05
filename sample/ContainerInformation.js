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
var containerName = 'Sample_Info';
var conInfo = new griddb.ContainerInfo({
    'name': containerName,
    'columnInfoList': [
        ["id", griddb.Type.INTEGER],
        ["productName", griddb.Type.STRING],
        ["count", griddb.Type.INTEGER]
    ],
    'type': griddb.ContainerType.COLLECTION, 'rowKey': true
});

store.dropContainer(containerName)
    .then(() => {
        console.log('Connect to Cluster');
        return store.putContainer(conInfo);
    })
    .then(() => {
        console.log("Sample data generation: Create Collection name=%s", containerName);
        return store.getContainerInfo(containerName);
    })
    .then((info) => {
        console.log("Get ContainerInfo: \n    name=%s", info.name);
        if ( info.type == griddb.ContainerType.COLLECTION ){
            console.log('    type=Collection');
        } else {
            console.log('    type=TimeSeries');
        }
        console.log("    rowKeyAssigned=%s", info.rowKey.toString());
        console.log("    columnCount=%d", info.columnInfoList.length);
        info.columnInfoList.forEach(
            element => console.log("    column (%s, %d)",
                element[0], element[1])
        );
        console.log('Success!');
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
