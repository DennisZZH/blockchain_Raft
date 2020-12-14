#include "server.h"
#include "network.h"

const char* usage = "Run the program by typing ./main <server_id> where server_id is within range [0, 2].";
inline void print_usage() {
    printf("%s\n", usage);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        print_usage();
        exit(1);
    }
    
    int server_id = atoi(argv[1]);
    if (server_id < 0 || server_id >= SERVER_COUNT) {
        std::cout << "Your input cid is out of the accepted range." << std::endl;
        print_usage();
        exit(1);
    }

    // Network network = Network(cid);
    Server server(server_id);
    while (true);
}