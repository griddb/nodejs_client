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
var containerName = 'Sample_RemoveRowByRowkey';
var rowCount = 5;
var nameList = ["notebook PC", "desktop PC", "keyboard", "mouse", "printer"];
var numberList = [108, 72, 25, 45, 62];
var conInfo = new griddb.ContainerInfo({
    'name': containerName,
    'columnInfoList': [
        ["id", griddb.Type.INTEGER],
        ["productName", griddb.Type.STRING],
        ["count", griddb.Type.INTEGER]
    ],
    'type': griddb.ContainerType.COLLECTION, 'rowKey': true
});
var container;

store.dropContainer(containerName)
    .then(() => {
        return store.putContainer(conInfo);
    })
    .then(cont => {
        console.log("Sample data generation: Create Collection name=%s", containerName);
        var rows = [];
        container = cont;
        for (var i = 0;i < rowCount; i++) {
            rows.push([i, nameList[i], numberList[i]]);
        }
        return container.multiPut(rows);
    })
    .then(() => {
        console.log("Sample data generation: Put Rows count=%d", rowCount);
        var rowKey = 3;
        console.log("Delete Row rowkey=%d", rowKey);
        return container.remove(rowKey);
    })
    .then((ret) => {
        console.log("Success!");
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
