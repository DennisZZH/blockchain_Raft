CC = g++
PC = protoc
CFLAGS += -std=c++11
PROTOBUF_LIB := `pkg-config --cflags --libs protobuf`

# Sources files to be compiled
SOURCES = \
client.cpp 	\
network.cpp \
main.cpp

BUILD_DIR = build

OBJECTS = $(addprefix $(BUILD_DIR)/, $(notdir $(SOURCES:.cpp=.o)))

client: $(OBJECTS)
	$(CC) $(CFLAGS) $(PROTOBUF_LIB) $^ -o $@ -g

# message: Msg.proto
# 	$(PC) -I=. --cpp_out=. ./Msg.proto


$(BUILD_DIR)/%.o: %.cpp	$(BUILD_DIR)
	$(CC) $(CFLAGS) $(PROTOBUF_LIB) -c $^ -o $@ -g

$(BUILD_DIR):
	mkdir $@

clean:
	rm -rf build client