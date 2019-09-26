GridDB Node.JS Client

## Overview

GridDB Node.JS Client is developed using GridDB C Client and [SWIG](http://www.swig.org/) (Simplified Wrapper and Interface Generator).  

## Operating environment

Building of the library and execution of the sample programs have been checked in the following environment.

    OS: CentOS 7.6(x64) (GCC 4.8.5)
    SWIG: 4.0.0
    Node.js: v8/v10
    GridDB C client: V4.2 CE(Community Edition)
    GridDB server: V4.2 CE, CentOS 7.6(x64) (GCC 4.8.5)

    OS: Ubuntu 18.04(x64) (gcc 7.3.0)
    SWIG: 4.0.0
    Node.js: v8/v10
    GridDB C client: V4.2 CE (Note: If you build from source code, please use GCC 4.8.5.)
    GridDB server: V4.2 CE, Ubuntu 18.04(x64) (Note: If you build from source code, please use GCC 4.8.5.)
    
    OS: Windows 10(x64) (VS2017)
    SWIG: 4.0.0
    Node.js: v8
    GridDB C client: V4.2 CE
    GridDB server: V4.2 CE, CentOS 7.6(x64) (GCC 4.8.5)

## QuickStart (CentOS, Ubuntu)
### Preparations

Install SWIG as below.

    $ wget https://prdownloads.sourceforge.net/swig/swig-4.0.0.tar.gz
    $ tar xvfz swig-4.0.0.tar.gz
    $ cd swig-4.0.0
    $ ./configure
    $ make
    $ sudo make install
   
    Note: If CentOS, you might need to install pcre in advance.
    $ sudo yum install pcre2-devel.x86_64

Install [GridDB Server](https://github.com/griddb/griddb_nosql) and [C Client](https://github.com/griddb/c_client). (Note: If you build them from source code, please use GCC 4.8.5.) 

Install Node.js(v8 or v10).

If required, change INCLUDES_JS path in Makefile.

Set LIBRARY_PATH. 

    export LIBRARY_PATH=$LIBRARY_PATH:<C client library file directory path>

### Build and Run 

    1. Execute the command on project directory.

    $ npm install nan
    $ make

    2. Set the NODE_PATH variable for griddb Node.js module files.
    
    $ export NODE_PATH=<installed directory path>

    3. Write require("griddb_node") in Node.js.

### How to run sample

GridDB Server need to be started in advance.

    1. Set LD_LIBRARY_PATH

        export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:<C client library file directory path>

    2. The command to run sample

        $ node sample/sample1.js <GridDB notification address> <GridDB notification port>
            <GridDB cluster name> <GridDB user> <GridDB password>
          -->[ 'name01', false, 1, <Buffer 41 42 43 44 45 46 47 48 49 4a> ]

## QuickStart (Windows)
### Preparations

Install SWIG as below.
- Download zip package from http://prdownloads.sourceforge.net/swig/swigwin-4.0.0.zip
- Extract the zip package then set path for SWIG_RESOURCE in CmakeList.txt.
```
set(SWIG_RESOURCE C:/Users/User/Downloads/swigwin-4.0.0)
```

Install Node.js(v8)
- Download and install package from https://nodejs.org/dist/latest-v8.x/node-v8.16.1-x64.msi

Install [GridDB Server](https://github.com/griddb/griddb_nosql) on CentOS. (Note: If you build them from source code, please use GCC 4.8.5.) 

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
2. There is [Node.JS Client (0.8.4) Package for CentOS on npm](https://www.npmjs.com/package/griddb_node) .
3. Cmake-js only support node v8

## Community

  * Issues  
    Use the GitHub issue function if you have any requests, questions, or bug reports. 
  * PullRequest  
    Use the GitHub pull request function if you want to contribute code.
    You'll need to agree GridDB Contributor License Agreement(CLA_rev1.1.pdf).
    By using the GitHub pull request function, you shall be deemed to have agreed to GridDB Contributor License Agreement.

## License
  
  GridDB Node.JS Client source license is Apache License, version 2.0.
