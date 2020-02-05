var griddb = require('griddb_node');
var fs = require('fs');

var factory = griddb.StoreFactory.getInstance();
try {
    //Multicast method
    var store = factory.getStore({
        "host": process.argv[2],
        "port": parseInt(process.argv[3]),
        "clusterName": process.argv[4],
        "username": process.argv[5],
        "password": process.argv[6]
    });

    //Fixed list method
    //var store = factory.getStore({
    //    "notificationMember": process.argv[2],
    //    "clusterName": process.argv[3],
    //    "username": process.argv[4],
    //    "password": process.argv[5]
    //});

    //Provider method
    //var store = factory.getStore({
    //    "notificationProvider": process.argv[2],
    //    "clusterName": process.argv[3],
    //    "username": process.argv[4],
    //    "password": process.argv[5]
    //});
} catch(err) {
    console.log(err);
    return false;
}

var containerName = 'containerName';

store.getContainer(containerName)
    .then((cont) => {
        if (!cont) {
            console.log('getContainer error');
        }
        console.log('Connect to Cluster');
        console.log('Success');
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
