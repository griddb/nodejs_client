cmake_minimum_required(VERSION 3.14)

project(griddb_client)

set(C_LIB C:/Users/User/Downloads/griddb_c_lib)
set(SWIG_RESOURCE C:/Users/User/Downloads/swigwin-4.0.0)
set(SOURCE_HEADER src)
set(C_HEADER include)
set(NAN_HEADER node_modules/nan)

set(SWIG_DIR ${SWIG_RESOURCE}/Lib)
set(SWIG_EXECUTABLE ${SWIG_RESOURCE}/swig.exe)
find_package(SWIG REQUIRED)
include(${SWIG_USE_FILE})
set(CMAKE_SWIG_FLAGS  -c++ -javascript -node)
set(CMAKE_CXX_FLAGS "/DLL /LD /D_USRDLL /D_WINDLL /DBUILDING_NODE_EXTENSION /MT /MD")

include_directories(${CMAKE_JS_INC} ${NAN_HEADER} ${C_HEADER} ${SOURCE_HEADER})

link_directories(${C_LIB})

file(GLOB SOURCE_FILES ${SOURCE_HEADER}/*.cpp)

set_property(SOURCE ${SOURCE_HEADER}/griddb.i PROPERTY CPLUSPLUS ON)
swig_add_library(${PROJECT_NAME} TYPE SHARED LANGUAGE javascript SOURCES ${SOURCE_HEADER}/griddb.i ${SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB} gridstore_c)