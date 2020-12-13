#include <iostream>
#include <thread>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdint.h>
#include "client.h"
#include "message.h"
#include "Msg.pb.h"
using namespace RaftClient;

#define DEBUG_MODE

Client::Client(int id) {
    client_id = id;
    network = new Network(this);
}

Client::~Client() {
    delete network;
}

Network::Network(Client *client) {
    this->client = client;
    conn_thread = std::thread(&Network::conn_handler, this);
}

Network::~Network() {
    for (int i = 0; i < SERVER_COUNT; i++) {
        servers[i].connected = false;
        try {
            servers[i].recv_task->join();
        } catch(...) {}
        delete servers[i].recv_task;
        close(servers[i].port);
    }
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

            // bind client id to a specific port so that the server
            // side can identity the client id based on the client port
            sockaddr_in self_addr = {0}; 
            self_addr.sin_family = AF_INET;
            self_addr.sin_addr.s_addr = inet_addr(CLIENT_IP);
            self_addr.sin_port = htons(CLIENT_BASE_PORT + get_client()->get_client_id());

            if (bind(s, (sockaddr*) &self_addr, sizeof(self_addr)) < 0) {
                #ifdef DEBUG_MODE
                std::cerr << "[Network::conn_handler] failed to bind the self port." << std::endl;
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
    
    while (servers[server_id].connected) {
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
        uint8_t *msg = new uint8_t[msg_bytes];

        // read the body message
        count = read(server_sock, msg, msg_bytes);
        if (count <= 0) {
            delete [] msg;
            break;
        } else if (count < msg_bytes) {
            std::cout << "[Network::recv_handler] received borken message from server " << server_id << std::endl;
            delete [] msg;
            continue;
        }

        response_msg_t response_msg;
        response_msg.ParseFromArray(msg, msg_bytes);
        delete [] msg;

        // TODO: add the message to the queue.
        std::cout << "[Network::recv_handler] message received and saved!" << std::endl;
    }

    std::cout << "[Network::recv_handler] connection with server: " << server_id << " is lost." << std::endl;
    close(server_sock);
    servers[index].connected = false;
}

/**
 * @brief send message to estimated leader
 * 
 * @param request_msg 
 */
void Network::send_message(request_msg_t &request_msg) {
    const char* msg_string = request_msg.SerializeAsString().c_str();
    COMM_HEADER_TYPE header = htonl(request_msg.ByteSizeLong());
    
    uint32_t leader_id = get_client()->get_leader_id();
    if (!servers[leader_id].connected) {
        std::cout << "[Network::send_message] leader is not connected." << std::endl;
        return;
    }

    write(servers[leader_id].sock, &header, sizeof(header));
    write(servers[leader_id].sock, msg_string, request_msg.ByteSizeLong());
}

void Network::send_transaction(uint32_t recv_id, uint32_t amount, uint64_t req_id) {
    request_msg_t request_msg;
    txn_msg_t* transaction = new txn_msg_t();
    transaction->set_recver_id(recv_id);
    transaction->set_amount(amount);
    transaction->set_sender_id(get_client()->get_client_id());
    request_msg.set_allocated_transaction(transaction);
    request_msg.set_request_id(req_id);
    request_msg.set_type(TRANSACTION_REQUEST);
    send_message(request_msg);
}

void Network::send_balance(uint64_t req_id) {
    request_msg_t request_msg;
    request_msg.set_request_id(req_id);
    request_msg.set_type(BALANCE_REQUEST);
    send_message(request_msg);
}

int main (int argc, char* argv[]) {
    
    return 0;
}






