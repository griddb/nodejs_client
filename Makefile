
SWIG = swig -DSWIGWORDSIZE64
CXX = g++

JS_ENGINE ?= node

JS_OPTION = v8

ARCH = $(shell arch)

LDFLAGS = -Llibs -lpthread -lrt -lgridstore
LDFLAGS_JS = -rdynamic -pthread

CPPFLAGS = -fPIC -std=c++0x -g -O2
INCLUDES = -Iinclude -Isrc

INCLUDES_JS = $(INCLUDES)
INCLUDES_JS += -I/usr/include/node

CPPFLAGS_JS   = $(CPPFLAGS) $(INCLUDES_JS)	\
				-DNODE_GYP_MODULE_NAME=griddb	\
				-DV8_DEPRECATION_WARNINGS=1 \
				-DBUILDING_NODE_EXTENSION

PROGRAM = griddb_client.node
EXTRA =

SOURCES = 	  src/TimeSeriesProperties.cpp \
		  src/ContainerInfo.cpp			\
  		  src/AggregationResult.cpp	\
		  src/Container.cpp			\
		  src/Store.cpp			\
		  src/StoreFactory.cpp	\
		  src/PartitionController.cpp	\
		  src/Query.cpp				\
		  src/Row.cpp				\
		  src/QueryAnalysisEntry.cpp			\
		  src/RowKeyPredicate.cpp	\
		  src/RowSet.cpp			\


all: $(PROGRAM)

SWIG_DEF = src/griddb.i

SWIG_JS_SOURCES     = src/griddb_js.cxx

OBJS = $(SOURCES:.cpp=.o)
SWIG_JS_OBJS = $(SWIG_JS_SOURCES:.cxx=.o)

$(SWIG_JS_SOURCES) : $(SWIG_DEF)
	$(SWIG) -Ilibs/js/$(JS_OPTION) -outdir . -o $@ -c++ -javascript -$(JS_ENGINE) $<

.cpp.o:
	$(CXX) $(CPPFLAGS) -c -o $@ $(INCLUDES) $<

$(SWIG_JS_OBJS): $(SWIG_JS_SOURCES)
	$(CXX) $(CPPFLAGS_JS) -c -o $@ $<

$(PROGRAM): $(OBJS) $(SWIG_JS_OBJS)
	$(CXX) -shared  -o $@ $(OBJS) $(SWIG_JS_OBJS) $(LDFLAGS) $(LDFLAGS_JS)

clean:
	rm -rf $(OBJS) $(SWIG_JS_OBJS)
	rm -rf $(SWIG_JS_SOURCES)
	rm -rf $(PROGRAM) $(EXTRA)
