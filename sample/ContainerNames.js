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

for (var index = 0; index < store.partitionController.partitionCount; index++) {
    store.partitionController.getContainerNames(index, 0, -1)
    .then(nameList =>{
        nameList.forEach(element => console.log(element));
        return true;
    });
}
