#include "server.h"
#include "network.h"

const char* usage = "Run the program by typing ./client <cid> where cid is within range [0, 2].";
inline void print_usage() {
    printf("%s\n", usage);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        print_usage();
        exit(1);
    }
    
    int cid = atoi(argv[1]);
    if (cid < 0 || cid > 2) {
        std::cout << "Your input cid is out of the accepted range." << std::endl;
        print_usage();
        exit(1);
    }

    Network network = Network(cid);
    while(true);
}