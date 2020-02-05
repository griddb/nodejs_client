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

var containerNameList = ["Sample_MultiPut1", "Sample_MultiPut2"];
var rowCount = 5;
var nameList = ["notebook PC", "desktop PC", "keyboard", "mouse", "printer"];
var numberList = [[108, 72, 25, 45, 62], [50, 11, 208, 23, 153]];

var promise = new Promise(function(resolve, reject) {
    resolve(true);
});

//Create container
var createContainer = function(containerName, count){
    return store.dropContainer(containerName)
    .then(() => {
        var conInfo = new griddb.ContainerInfo({
            'name': containerName,
            'columnInfoList': [
                ["id", griddb.Type.INTEGER],
                ["productName", griddb.Type.STRING],
                ["count", griddb.Type.INTEGER]
            ],
            'type': griddb.ContainerType.COLLECTION, 'rowKey': true
        });
        return store.putContainer(conInfo);
    })
    .then(() => {
        console.log("Create Collection name=%s", containerName);
        return true;
    })
}


containerNameList.forEach(function(item, i) {
    promise = promise
    .then(() => {
        return createContainer(item, i);})
});

promise = promise
    .then(() => {
        var rows = [];
        for (var count=0; count < containerNameList.length; count++) {
            var arr = [];
            for (var i=0;i < rowCount;i++) {
                var row = [];
                row[0] = i;
                row[1] = nameList[i];
                row[2] = numberList[count][i];
                arr.push(row);
            }
            rows.push(arr);
        }
        var param = {};
        param[containerNameList[0]] = rows[0];
        param[containerNameList[1]] = rows[1];
        return store.multiPut(param);
    })
    .then((ret) => {
        console.log("Multi Put");
        console.log("Success!");
        return true;
    });

promise = promise.
    catch(err => {
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
