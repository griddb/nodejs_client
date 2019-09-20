GridDB Node.JS Client

## Overview

GridDB Node.JS Client is developed using GridDB C Client and [SWIG](http://www.swig.org/) (Simplified Wrapper and Interface Generator).

## Operating environment

Building of the library and execution of the sample programs have been checked in the following environment.

    OS:              Windows 10(x64)
    SWIG:            4.0.0
    Node.js:         v8
    GridDB Server:   4.2 CE
    GridDB C Client: 4.2.0
    CMAKE:           3.15.2
    CMAKE-JS:        5.3.1
    Visual Studio:   2017(x64)

## QuickStart
### Preparations

Install SWIG as below.
- Download zip package from http://prdownloads.sourceforge.net/swig/swigwin-4.0.0.zip
- Extract the zip package then set path for SWIG_RESOURCE in CmakeList.txt.
```
set(SWIG_RESOURCE C:/Users/User/Downloads/swigwin-4.0.0)
```

Install Node.js(v8)
- Download and install package from https://nodejs.org/dist/latest-v8.x/node-v8.16.1-x64.msi

Install GridDB C Client.
- Please refer to https://github.com/griddb/c_client to install GridDB C client.
- After installing GridDB C client, set path for C_LIB (path for gridstore_c.lib) in CmakeList.txt.
```
    set(C_LIB C:/Users/User/Downloads/griddb_c_lib)
```

Install CMake
- Download and install package from https://github.com/Kitware/CMake/releases/download/v3.15.2/cmake-3.15.2-win64-x64.msi

Install Cmake-js
- Use command line:
npm install cmake-js

### Build and Run in cmd

1. Execute the command on project directory.

    ```
    $ npm install nan
    $ node .\node_modules\cmake-js\bin\cmake-js compile -G "Visual Studio 15 2017 Win64"
    ```

2. Set the NODE_PATH variable for griddb Node.js module files.
    ```
    $ set NODE_PATH=<installed directory path>
    ```

3. Set PATH variable for C Client library folder.

	```
    $ set PATH=%PATH%;<C Client library folder>
    ```

4. Write require("griddb_node") in Node.js source code.

5. Copy build file to project directory.

    ```
    $ copy build\Release\griddb_client.node .
    ```

### How to run sample

GridDB Server need to be started in advance.

1. The command to run sample

    ```
    $ node sample/sample1.js <GridDB notification address> <GridDB notification port>
        <GridDB cluster name> <GridDB user> <GridDB password>
        -->[ 'name01', false, 1, <Buffer 41 42 43 44 45 46 47 48 49 4a> ]
    ```

## Function

(available)
- STRING, BOOL, BYTE, SHORT, INTEGER, LONG, FLOAT, DOUBLE, TIMESTAMP, BLOB type for GridDB
- put single row, get row with key
- normal query, aggregation with TQL
- Multi-Put/Get/Query (batch processing)

(not available)
- GEOMETRY, Array type for GridDB
- timeseries compression
- timeseries-specific function like gsAggregateTimeSeries, gsQueryByTimeSeriesSampling in C client
- trigger, affinity

Please refer to the following files for more detailed information.
- [Node.JS Client API Reference](https://griddb.github.io/nodejs_client/NodejsAPIReference.htm)

Note:
1. The current API might be changed in the next version. e.g. ContainerInfo()
2. Cmake-js only support node v8

## Community

  * Issues
    Use the GitHub issue function if you have any requests, questions, or bug reports.
  * PullRequest
    Use the GitHub pull request function if you want to contribute code.
    You'll need to agree GridDB Contributor License Agreement(CLA_rev1.1.pdf).
    By using the GitHub pull request function, you shall be deemed to have agreed to GridDB Contributor License Agreement.

## License

  GridDB Node.JS Client source license is Apache License, version 2.0.
