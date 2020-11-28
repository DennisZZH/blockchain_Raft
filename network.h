#pragma once
#include <iostream>
#include <stdint.h>
#include <thread>
#include "raft.h"

#define CLIENT_IP           "127.0.0.1"
#define SERVER_BASE_PORT    8000
#define CLIENT_BASE_PORT    8010
#define MAX_CLIENT_NUM      3


struct client_info_t {
    int socket;
    bool valid;
    std::thread task;
};

typedef enum {
    NONE,
    REQ_VOTE_RPC,               // request vote RPC
    REQ_VOTE_RPL,               // request vote reply
    APP_ENTR_RPC,               // append entry RPC
    APP_ENTR_RPL                // append entry reply
} msg_type_t;

typedef struct {
    msg_type_t type;            // type determines which struct we should cast the message payload to.
    void* payload;              // cast this one to one of the message struct.
} msg_t;

class Network {
private:    
    int client_id;
    int socket_fd;                                 // The sock fd this client binds to.
    bool stop_flag = false;
    client_info_t clients_info[MAX_CLIENT_NUM];

    std::thread wait_thread;                        // Thread for listening & accepting connections from peers.
    std::thread conn_thread;                        // Thread for connecting to other peers.
    void setup_server();
    void wait_connection();                         // Thread function for listening & accepting connections.
    void setup_connections();                       // Thread function for connecting to lower id sites.
    void receive_message(int id);                   // Thread function for recving messages from peers. Index is used for freeing the client slot.
    
public:
    Network(int id);
};