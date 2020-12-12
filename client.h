#pragma once
#include <thread>
#include <stdint.h>

#define SERVER_IP           "127.0.0.1"
#define SERVER_BASE_PORT    8020
#define SERVER_COUNT        3

#define COMM_HEADER_TYPE    uint32_t

namespace RaftClient {
    class Network;
    class Client;

    struct server_t {
        bool connected;
        const int id;
        const int port;
        int sock;
        std::thread* recv_task;
    };

    class Client {
    private:
        int client_id = 0;
        int leader_id = 0;
        Network* network;
    public:
        Client(int id) {client_id = id;}
        void set_leader_id(int id) {client_id = id;}
        
        int get_client_id() {return client_id;}
        int get_leader_id() {return leader_id;}
        Network* get_network() {return network;}
    };

    class Network {
    private:
        // client back reference.
        Client* client = NULL;
        
        server_t servers[SERVER_COUNT] = {
            {.connected = false, .id = 0, .port = SERVER_BASE_PORT + 0, .sock = 0, NULL},
            {.connected = false, .id = 1, .port = SERVER_BASE_PORT + 1, .sock = 0, NULL},
            {.connected = false, .id = 2, .port = SERVER_BASE_PORT + 2, .sock = 0, NULL}
        };

        // threads declarations
        std::thread conn_thread;
        std::thread conn_recycle_thread;
        
        void send_message(request_msg_t &);
        
        Client* get_client();

        // thread function kept trying to connect to three servers.
        void conn_handler();

        // thread function that free the dynamic thread pointer in each server_t after the conn is lost.
        void conn_recycle_handler();

        // thread function listening to a sock after the connection is established.                
        void recv_handler(int index);          

    public:
        Network(Client* client);
        ~Network();
        
        // send transaction to the estimated leader.
        void send_transaction(uint32_t recv_id, uint32_t amount, uint64_t req_id);

        // send balance to the estimated leader.
        void send_balance(uint64_t req_id);
    };
}