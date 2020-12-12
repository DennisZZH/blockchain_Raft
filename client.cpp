#include <iostream>
#include <thread>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdint.h>
#include "client.h"
using namespace RaftClient;

#define DEBUG_MODE

Network::Network(Client *client) {
    this->client = client;
}

/**
 * @brief this function keeps trying to connect to three servers.
 * 
 */
void Network::conn_handler() {
    while (true) {
        for (int i = 0; i < SERVER_COUNT; i++) {
            // check the next one if the server is connected
            if (servers[i].connected)
                continue;

            #ifdef DEBUG_MODE
            std::cout << "[Network::conn_handler] connecting to server: " << servers[i].id << std::endl;
            #endif

            int s = socket(AF_INET, SOCK_STREAM, 0);
            int status = 0;
            
            if (setsockopt(s, SOL_SOCKET, SO_REUSEPORT, &status, sizeof(status)) < 0) {
                #ifdef DEBUG_MODE
                std::cerr << "[Network::conn_handler] failed to set the socket options." << std::endl;
                #endif
                continue;
            }

            sockaddr_in addr = {0};
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = inet_addr(SERVER_IP);
            addr.sin_port = htons(servers[i].port);
            
            if (connect(s, (sockaddr*) &addr, sizeof(sockaddr_in)) < 0) {
                #ifdef DEBUG_MODE
                std::cerr << "[Network::conn_handler] Failed to connect the server " << servers[i].id << std::endl;
                #endif
                continue;
            }
            
            // need to check if the previous task is still running
            // need to stop it and free the memory if it's running
            if (servers[i].recv_task != NULL) {
                try {
                    servers[i].recv_task->join();
                    free(servers[i].recv_task);
                    servers[i].recv_task = NULL;
                } catch (...) {};
            }

            // need to update server information so that the listening thread can use it
            servers[i].sock = s;
            servers[i].connected = true;
            servers[i].recv_task = new std::thread(&RaftClient::Network::recv_handler, this, i);
        }
        // sleep the current thread after looping for one round
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void Network::recv_handler(int index) {
    const int server_id = servers[index].id;
    const int server_sock = servers[index].sock;
    
    while (true) {
        if (!servers[index].connected)
            break;
        
        int count = 0;
        COMM_HEADER_TYPE msg_bytes = 0;
        
        // no need to use while because read on stream runs as byte-stream mode in default
        count = read(server_sock, &msg_bytes, sizeof(msg_bytes));
        if (count <= 0) {
            break;
        } else if (count < sizeof(msg_bytes)) {
            std::cout << "[Network::recv_handler] received borken header from server " << server_id << std::endl;
            continue; 
        }
        // parse the retrive size and allocate message buffer
        msg_bytes = ntohl(msg_bytes);
        char *msg = new char[msg_bytes];

        // read the body message
        count = read(server_sock, msg, msg_bytes);
        if (count <= 0) {
            break;
        } else if (count < msg_bytes) {
            std::cout << "[Network::recv_handler] received borken message from server " << server_id << std::endl;
            delete [] msg;
            continue;
        }

        // TODO: parse the message from the server
        // TODO: add to the local queue

        delete [] msg;
    }

    std::cout << "[Network::recv_handler] connection with server: " << server_id << " is lost." << std::endl;
    close(server_sock);
    servers[index].connected = false;

    std::cout << "[Network::recv_hanlder] message received and saved!" << std::endl;
}

int main (int argc, char* argv[]) {
    
    return 0;
}






