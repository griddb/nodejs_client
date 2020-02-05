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
var containerName = 'Sample_BlobData';
var conInfo = new griddb.ContainerInfo({
    'name': containerName,
    'columnInfoList': [
        ["id", griddb.Type.INTEGER],
        ["blob", griddb.Type.BLOB]
    ],
    'type': griddb.ContainerType.COLLECTION, 'rowKey': true
});
var file = './sample/BlobData.js';
var container;

store.dropContainer(containerName)
    .then(() => {
        console.log('Connect to Cluster');
        return store.putContainer(conInfo);
    })
    .then(cont => {
        console.log("Create Collection name=%s", containerName);
        container = cont;
        var contents;
        try {
            contents = fs.readFileSync(file);
        } catch (err) {
            console.log('File not found');
            throw err;
        }
        return container.put([0, contents]);
    })
    .then(() => {
        console.log('Put Row (Blob)');
        return container.get(0);
    })
    .then(row => {
        console.log("Get Row (Blob size=%d)",row[1].length);
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
