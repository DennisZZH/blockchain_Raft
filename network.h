#pragma once
#include <iostream>
#include <stdint.h>
#include <thread>
#include <deque>
#include <vector>
#include <mutex>
#include "raft.h"
#include "parameter.h"
#include "message.h"

#define REPLICA_SERVER_IP               "127.0.0.1"
#define REPLICA_SERVER_BASE_PORT        8000            // port base number when replicas work like server to accept other replicas
#define REPLICA_CLIENT_BASE_PORT        8010            // port base number when replicas work like client to connect to other replicas
#define REPLICA_NODE_COUNT              3

struct replica_info_t {
    int socket;
    bool valid;
    std::thread task;
};

struct client_info_t {
    bool connected;
    int id;
    int port;
    int sock;
    std::thread *recv_task;
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
    replica_info_t replicas_info[REPLICA_NODE_COUNT];
    
    
    // replica related
    std::deque<msg_t> server_message_queue;                             // The message buffer between the server.
    std::thread replica_wait_thread;                                    // Thread for listening & accepting connections from peers.
    std::thread replica_conn_thread;                                    // Thread for connecting to other peers.

    void setup_replica_server();                                        // Setup up replica interconnections.
    void wait_connection();                                             // Thread function for listening & accepting connections.
    void setup_connections();                                           // Thread function for connecting to lower id sites.
    void replica_recv_handler(int id);                                  // Thread function for recving messages from peers. Index is used for freeing the client slot.
    
    // client related
    client_info_t clients[CLIENT_COUNT] = {0};                          // saves the client information

    std::mutex client_req_mutex;                                        // lock of the client_req_queue.

    std::deque<request_t*> client_req_queue;                            // Hold the request from client.
    std::thread client_wait_thread;                                     // Thread for listening & accepting clients.

    void setup_client_server();                                         // Setup client connections.
    void client_wait_handler();                                         // Thread function for listening & accepting clients.
    void client_recv_handler(int client_id);                            // Thread function for receiving clients message.

public:
    const uint32_t RECYCLE_CHECK_SLEEP_MS = 50;                  // The sleep time until check next time if the queue is empty.
    
    Network(int id);
    
    // replica related APIs
    void replica_send_message(msg_t &msg, int id = -1);                         // Send the message to the served identified by the id. If id == -1, send to all.
    void replica_pop_message(msg_t &msg);                                       // Pop the message saved in the message queue and fill the info into msg.
    size_t replica_get_message_count();                                         // Get the count in the message buffer.

    // request related APIs
    void client_push_request(request_t* request);
    request_t* client_pop_request();
    void client_send_message();
    size_t client_get_request_count();
};