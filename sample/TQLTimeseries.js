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
var containerName = 'Sample_TQLTimeseries';
var conInfo = new griddb.ContainerInfo({
    'name': containerName,
    'columnInfoList': [
        ["date", griddb.Type.TIMESTAMP],
        ["value1", griddb.Type.INTEGER],
        ["value2", griddb.Type.DOUBLE]
    ],
    'type': griddb.ContainerType.TIME_SERIES
});
var rowCount = 4;
var dateList = ["2018-12-01T10:00:00.000Z",
    "2018-12-01T10:10:00.000Z",
    "2018-12-01T10:20:00.000Z",
    "2018-12-01T10:40:00.000Z"];
var value1List = [1, 3, 2, 4];
var value2List = [10.3, 5.7, 8.2, 4.5];
var container;
var queryAvgStr = "SELECT TIME_AVG(value1)";
var queryTimeNextStr = "SELECT TIME_NEXT(*, TIMESTAMP('2018-12-01T10:10:00.000Z'))";
var queryTimeInter = "SELECT TIME_INTERPOLATED(value1, TIMESTAMP('2018-12-01T10:30:00.000Z'))";

store.dropContainer(containerName)
    .then(() => {
        return store.putContainer(conInfo);
    })
    .then(cont => {
        console.log("Sample data generation: Create Collection name=%s", containerName);
        console.log("Sample data generation:  column=(%s, %s, %s)", conInfo.columnInfoList[0][0], conInfo.columnInfoList[1][0], conInfo.columnInfoList[2][0]);
        container = cont;
        var rows = [];
        var row;
        for (var i = 0;i < rowCount; i++) {
            row = [new Date(dateList[i]), value1List[i], value2List[i]];
            rows.push(row);
            console.log("Sample data generation:  row=(%s, %d, %f)", row[0].toISOString(), row[1], row[2]);
        }
        return container.multiPut(rows);
    })
    .then(() => {
        console.log("Sample data generation: Put Rows count=%d", rowCount);
        return container.query(queryAvgStr);
    })
    .then((query) => {
        console.log("TQL query : %s", queryAvgStr);
        return query.fetch();
    })
    .then((rs) => {
        while(rs.hasNext()) {
            aggregation = rs.next();
            var value = aggregation.get(griddb.Type.DOUBLE);
            console.log("TQL result: %f", value);
        }
        return true;
    })
    .then(() => {
        return container.query(queryTimeNextStr);
    })
    .then((query) => {
        console.log("TQL query : %s", queryTimeNextStr);
        return query.fetch();
    })
    .then((rs) => {
        var row;
        while(rs.hasNext()) {
            row = rs.next();
            console.log("TQL result: row=(%s, %d, %f)", row[0].toISOString(), row[1], row[2]);
        }
        return container.query(queryTimeInter);
    })
    .then((query) => {
        console.log("TQL query : %s", queryTimeInter);
        return query.fetch();
    })
    .then((rs) => {
        var row;
        while(rs.hasNext()) {
            row = rs.next();
            console.log("TQL result: row=(%s, %d, %f)", row[0].toISOString(), row[1], row[2]);
        }
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
