OPT ?= -O2 -g

CXXFLAGS += -I./net -I./rpc/protobuf -I./rpc/protorpc -lprotobuf $(OPT) -std=c++11 -pthread

NET_SOURCES = $(shell find net -name '*.cc')
NET_OBJ = $(NET_SOURCES:.cc=.o)

RPC_PROTOBUF_SOURCES = $(shell find rpc/protobuf -name '*.cc')
RPC_PROTOBUF_OBJ = $(RPC_PROTOBUF_SOURCES:.cc=.o)

RPC_SOURCES = $(shell find rpc/protorpc -name '*.cc')
RPC_OBJ = $(RPC_SOURCES:.cc=.o)

EXAMPLES_SOURCE = $(shell find examples -name '*.cc')
EXAMPLES_OBJ = $(EXAMPLES_SOURCE:.cc=.o)

TEST_SOURCES = $(shell find unit_test -name '*.cc')
TEST_OBJ = $(TEST_SOURCES:.cc=.o)

all: test http_server test_client test_server test_codec test_rpc_client test_rpc_server#cleanObj

test: unit_test/unit_test.o unit_test/test.o $(NET_OBJ)
	$(CXX) $^ -o $@ ${CXXFLAGS}

test_client: examples/pingpong/test_client.o $(NET_OBJ)
	$(CXX) $^ -o $@ ${CXXFLAGS}

test_server: examples/pingpong/test_server.o $(NET_OBJ)
	$(CXX) $^ -o $@ ${CXXFLAGS}

test_rpc_server: examples/rpc/echo.pb.o examples/rpc/test_server.o $(NET_OBJ) $(RPC_OBJ) $(RPC_PROTOBUF_OBJ)
	$(CXX) $^ -o $@ ${CXXFLAGS}

test_rpc_client: examples/rpc/echo.pb.o examples/rpc/test_client.o $(NET_OBJ) $(RPC_OBJ) $(RPC_PROTOBUF_OBJ)
	$(CXX) $^ -o $@ ${CXXFLAGS}

http_server: examples/http/http_server.o $(NET_OBJ)
	$(CXX) $^ -o $@ ${CXXFLAGS}

test_codec: unit_test/unit_test.o unit_test/test_codec.o $(NET_OBJ) $(RPC_PROTOBUF_OBJ) $(RPC_OBJ)
	$(CXX) $^ -o $@ $(CXXFLAGS)

# 隐含规则包括了下面这个，但是写出来是最好的
# 定义模式规则
# .cc.o:
%.o:%.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

cleanObj:
	rm $(TEST_OBJ) $(NET_OBJ) $(EXAMPLES_OBJ) $(RPC_PROTOBUF_OBJ) $(RPC_OBJ)

.PHONY: clean
clean:
	rm test test_client http_server test_server test_codec
