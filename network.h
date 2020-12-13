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

class Server;

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

class Network {
private:
    Server *context;
    bool stop_flag = false;

    Server* get_context() {return context;};

    /////////////////////
    /* replica related */
    /////////////////////
    int replica_socket = 0;
    bool mesh_connected = false;
    std::deque<replica_msg_wrapper_t*> replica_msg_queue;            // The message buffer between the server.
    std::thread replica_conn_thread;                                    // Thread for connecting to other peers.
    std::thread replica_recv_thread;

    void setup_replica_server();                                        // Setup up replica interconnections.
    void replica_conn_handler();                                        // Thread function for connecting to lower id sites.
    void replica_recv_handler();                                        // Thread function for recving messages from the mesh
    
    ////////////////////
    /* client related */
    ////////////////////
    int client_server_fd;
    client_info_t clients[CLIENT_COUNT] = {0};                          // saves the client information
    std::mutex client_req_mutex;                                        // lock of the client_req_queue.
    std::deque<request_t*> client_req_queue;                            // Hold the request from client.
    std::thread client_wait_thread;                                     // Thread for listening & accepting clients.

    void setup_client_server();                                         // Setup client connections.
    void client_wait_handler();                                         // Thread function for listening & accepting clients.
    void client_recv_handler(int client_id);                            // Thread function for receiving clients message.

public:
    const uint32_t RECYCLE_CHECK_SLEEP_MS = 50;                         // The sleep time until check next time if the queue is empty.
    
    Network(Server *context);
    
    // replica related APIs
    void replica_send_message(replica_msg_wrapper_t &msg, int id = -1);         // Send the message to the replica identified by the id. If id == -1, send to all.
    void replica_pop_message(replica_msg_wrapper_t &msg);                       // Pop the message saved in the message queue and fill the info into msg.
    size_t replica_get_message_count();                                         // Get the count in the message buffer.

    // request related APIs
    void client_push_request(request_t* request);
    request_t* client_pop_request();
    void client_send_message(response_t& reponse, int client_id = -1);          // Send the message to the client identified by the id. If id == -1, send to all.   
    size_t client_get_request_count();
};