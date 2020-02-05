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

var containerNameList = ["Sample_MultiGet1", "Sample_MultiGet2"];
var rowCount = 5;
var nameList = ["notebook PC", "desktop PC", "keyboard", "mouse", "printer"];
var numberList = [[108, 72, 25, 45, 62], [50, 11, 208, 23, 153]];
var predicateList = [];
var container;

var promise = new Promise(function(resolve, reject) {
    resolve(true);
});

//Create container, rows, rowkey predicate
var createContainerRowsPredicate = function(containerName, count){
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
    .then(cont => {
        console.log("Sample data generation: Create Collection name=%s",
                containerName);
        container = cont;
        var rows = [];
        for (var i=0;i < rowCount;i++) {
            var row = [];
            row[0] = i;
            row[1] = nameList[i];
            row[2] = numberList[count][i];
            rows.push(row);
        }
        return container.multiPut(rows);
    })
    .then(() => {
        console.log("Sample data generation: Put Rows count=%d", rowCount);
        var predicate = store.createRowKeyPredicate(griddb.Type.INTEGER);
        predicate.setRange(0, 2);
        predicateList.push(predicate);
        return true;
    })
}

//Get data and print to output.
var getRowSet = function(query, containerName){
    rs = query.getRowSet();
    while (rs.hasNext()) {
        row = rs.next();
        console.log("FetchAll result: container=%s, row=(%d, \"%s\", %d)", 
            containerName, row[0], row[1], row[2]);
    }
    return true;
}

containerNameList.forEach(function(item, i) {
    promise = promise
    .then(() => {
        return createContainerRowsPredicate(item, i);})
});

promise = promise
    .then(() => {
        var param = {};
        param[containerNameList[0]] = predicateList[0];
        param[containerNameList[1]] = predicateList[1];
        return store.multiGet(param);
    })
    .then((ret) => {
        for (var key in ret) {
            for (i = 0; i< ret[key].length; i++) {
                console.log("MultiGet: container=%s, id=%d, productName=%s, count=%d", key, ret[key][i][0], ret[key][i][1],
                    ret[key][i][2]);
            }
        }
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
