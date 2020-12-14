CC = g++
PC = protoc
CFLAGS += -std=c++11
PROTOBUF_LIB := `pkg-config --cflags --libs protobuf`
OPENSSL_FLAGS :=  -I/usr/local/opt/openssl@1.1/include -L/usr/local/opt/openssl@1.1/lib -lcrypto

# Sources files to be compiled
SOURCES = \
server.cpp 	\
network.cpp \
state.cpp \
main.cpp

BUILD_DIR = build

OBJECTS = $(addprefix $(BUILD_DIR)/, $(notdir $(SOURCES:.cpp=.o)))

server: $(OBJECTS) Msg.pb.cc
	$(CC) $(CFLAGS) $(PROTOBUF_LIB) $(OPENSSL_FLAGS) $^ -o $@ -g

client: $(BUILD_DIR)/client.o Msg.pb.cc
	$(CC) $(CFLAGS) $(PROTOBUF_LIB) $(OPENSSL_FLAGS) $^ -o $@ -g

mesh: $(BUILD_DIR)/mesh.o Msg.pb.cc
	$(CC) $(CFLAGS) $(PROTOBUF_LIB) $(OPENSSL_FLAGS) $^ -o $@ -g

blockchain_test: blockchain_test.cpp
	$(CC) $(CFLAGS) $(OPENSSL_FLAGS) $^ -o $@

message: Msg.proto
	$(PC) -I=. --cpp_out=. ./Msg.proto


$(BUILD_DIR)/%.o: %.cpp $(BUILD_DIR)
	$(CC) $(CFLAGS) $(PROTOBUF_LIB) $(OPENSSL_FLAGS) -c $^ -o $@ -g

$(BUILD_DIR):
	mkdir $@

clean:
	rm -rf build client mesh blockchain_test