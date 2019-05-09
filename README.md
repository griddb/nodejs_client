GridDB Node.JS Client

## Overview

GridDB Node.JS Client is developed using GridDB C Client and [SWIG](http://www.swig.org/) (Simplified Wrapper and Interface Generator).  

## Operating environment

Building of the library and execution of the sample programs have been checked in the following environment.

    OS:              CentOS 7.6(x64)
    SWIG:            4.0.0
    GCC:             4.8.5
    Node.js:         v8/v10
    GridDB Server and C Client:   4.1 CE

## QuickStart
### Preparations

Install SWIG as below.

    $ wget https://sourceforge.net/projects/pcre/files/pcre/8.39/pcre-8.39.tar.gz
    $ tar xvfz pcre-8.39.tar.gz
    $ cd pcre-8.39
    $ ./configure
    $ make
    $ make install

    $ wget https://prdownloads.sourceforge.net/swig/swig-4.0.0.tar.gz
    $ tar xvfz swig-4.0.0.tar.gz
    $ cd swig-4.0.0
    $ ./configure
    $ make
    $ make install

Install Node.js(v8 or v10) and GridDB C Client.

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

        $ node sample/sample.js <GridDB notification address> <GridDB notification port>
            <GridDB cluster name> <GridDB user> <GridDB password>
          -->[ 'name01', false, 1, <Buffer 41 42 43 44 45 46 47 48 49 4a> ]

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
2. There is [Package for 0.7.1 on npm](https://www.npmjs.com/package/griddb_node) .

## Community

  * Issues  
    Use the GitHub issue function if you have any requests, questions, or bug reports. 
  * PullRequest  
    Use the GitHub pull request function if you want to contribute code.
    You'll need to agree GridDB Contributor License Agreement(CLA_rev1.1.pdf).
    By using the GitHub pull request function, you shall be deemed to have agreed to GridDB Contributor License Agreement.

## License
  
  GridDB Node.JS Client source license is Apache License, version 2.0.
