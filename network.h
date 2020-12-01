#pragma once
#include <iostream>
#include <stdint.h>
#include <thread>
#include <deque>
#include "raft.h"

#define CLIENT_IP               "127.0.0.1"
#define SERVER_BASE_PORT        8000            // port base number when replicas work like server to accept other replicas
#define CLIENT_BASE_PORT        8010            // port base number when replicas work like client to connect to other replicas
#define CLIENT_CONN_BASE_PORT   8020            // port base number that clients try to connect
#define MAX_CLIENT_NUM          3

struct replica_info_t {
    int socket;
    bool valid;
    std::thread task;
};

struct client_info_t {
    int socket;
    bool valid;
    std::thread task;
};

struct client_req_t {
    int client_id;
    int fd;
    // TODO: May need to change to UUID to give the request a unique identifier.
    // TODO: request*
};

typedef enum {
    NONE,
    REQ_VOTE_RPC,               // request vote RPC
    REQ_VOTE_RPL,               // request vote reply
    APP_ENTR_RPC,               // append entry RPC
    APP_ENTR_RPL                // append entry reply
} msg_type_t;

struct msg_t{
    msg_type_t type;            // type determines which struct we should cast the message payload to.
    void* payload;              // cast this one to one of the message struct.
};

class Network {
private:
    int server_id;
    int replica_server_fd;                                              // The socket fd used to listen replica connections.
    int client_server_fd;                                               // The socket fd used to listen client connections.
    bool stop_flag = false;
    replica_info_t replicas_info[MAX_CLIENT_NUM];
    
    std::deque<client_info_t*> clients_recycling;                       // Holds clients that will be deallocated.
                                                                        // Needed because the thread obj need to be saved some where
                                                                        // so cannot be stack. It needs to be freed if allocated dynamically.
    
    std::deque<msg_t> server_message_queue;                             // The message buffer between the server.
    std::deque<client_req_t> client_req_queue;                          // Hold the request from client.
    
    std::thread client_wait_thread;                                     // Thread for listening & accepting clients.
    std::thread client_recycle_thread;                                  // Thread for freeing the dynamic memory used by accepting & handling the client.
    std::thread replica_wait_thread;                                    // Thread for listening & accepting connections from peers.
    std::thread replica_conn_thread;                                    // Thread for connecting to other peers.
    void setup_replica_server();                                        // Setup up replica interconnections.
    void setup_client_server();                                         // Setup client connections.

    void wait_connection();                                             // Thread function for listening & accepting connections.
    void setup_connections();                                           // Thread function for connecting to lower id sites.
    void receive_message(int id);                                       // Thread function for recving messages from peers. Index is used for freeing the client slot.
    
    void wait_clients();                                                // Thread function for listening & accepting clients.
    void receive_clients_message(client_info_t* client_info);           // Thread function for receiving clients message.
    void recycle_clients();                                             // Thread function for recycling unconnected clients.

public:
    const uint32_t RECYCLE_CHECK_SLEEP_MS = 50;                  // The sleep time until check next time if the queue is empty.
    
    Network(int id);
    void send_message(msg_t &msg, int id = -1);                         // Send the message to the served identified by the id. If id == -1, send to all.
    void pop_message(msg_t &msg);                                       // Pop the message saved in the message queue and fill the info into msg.
    size_t get_message_count();                                         // Get the count in the message buffer.
};