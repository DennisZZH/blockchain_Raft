#include <iostream>
#include <thread>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdint.h>
#include <sstream>
#include "client.h"
#include "message.h"
#include "Msg.pb.h"
#include "parameter.h"

using namespace RaftClient;

#define DEBUG_MODE

const char* usage = 
"Run the program by typing ./client <client_id> where client_id is within range [0, 2].\n"
"Commands: \n"
"transfer: [transfer or t or T] <recv_id> <amount>\n"
"balance: [balance or b or B]\n";

inline void print_usage() {
    printf("%s\n", usage);
}

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

            int sock = socket(AF_INET, SOCK_STREAM, 0);
            int status;
            if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &status, sizeof(status)) < 0) {
                std::cerr << "[Network::conn_handler] failed to set the socket options." << std::endl;
                close(sock);
                continue;
            }

            sockaddr_in self_addr = {0}; 
            self_addr.sin_family = AF_INET;
            self_addr.sin_addr.s_addr = inet_addr(CLIENT_IP);
            self_addr.sin_port = htons(CLIENT_BASE_PORT + get_client()->get_client_id() * CLIENT_PORT_MULT + i);

            if (bind(sock, (sockaddr*) &self_addr, sizeof(self_addr)) < 0) {
                std::cerr << "[Network::conn_handler] failed to bind the self port." << std::endl;
                close(sock);
                continue;
            }

            sockaddr_in addr = {0};
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = inet_addr(SERVER_IP);
            addr.sin_port = htons(servers[i].port);
            
            if (connect(sock, (sockaddr*) &addr, sizeof(sockaddr_in)) < 0) {
                #ifdef DEBUG_MODE
                std::cerr << "[Network::conn_handler] Failed to connect the server " << servers[i].id << std::endl;
                #endif
                close(sock);
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
            servers[i].connected = true;
            servers[i].sock = sock;
            servers[i].recv_task = new std::thread(&RaftClient::Network::recv_handler, this, i);
        }
        // sleep the current thread after looping for one round
        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }
}

void Network::recv_handler(int index) {
    const int server_id = servers[index].id;
    const int server_sock = servers[index].sock;
    std::cout << "[Network::recv_handler] start listening server: " << servers[index].id << "'s messages" << std::endl;
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

        // if client receives leader change response, it doesn't need to add to the queue
        // instread, it will change the estimated leader id directly.
        message_type_t type = (message_type_t) response_msg.type();
        if (type == LEADER_CHANGE) {
            int leader_id = response_msg.leader_id();
            get_client()->set_leader_id(leader_id);
            std::cout << "[Network::recv_handler] changed leader to leader: " << leader_id << std::endl;   
            continue;
        }

        // if the response is not leader change response
        // then need to save to the response queue
        auto response = new response_t();
        response->type = type;
        response->request_id = response_msg.request_id();
        response->succeed = response_msg.succeed();
        if (type == TRANSACTION_RESPONSE) {
            // do nothing
        } else if (type == BALANCE_RESPONSE) {
            response->balance = response_msg.balance();
        } else {
            std::cout << "[Network::recv_handler] received unknown type. discarded!" << std::endl;
            delete response;
            continue;
        }
        response_queue_push(response);
        std::cout << "[Network::recv_handler] message received and saved!" << std::endl;
    }

    std::cout << "[Network::recv_handler] connection with server: " << server_id << " is lost." << std::endl;
    servers[index].connected = false;
    close(server_sock);
}

void Network::response_queue_push(response_t* response) {
    if (response == NULL)
        return;
    response_queue_lock.lock();
    response_queue.push_back(response);
    response_queue_lock.unlock();
}

/**
 * @brief caller need to free the dynamic memory by himself.
 * 
 * @return response_t* 
 */
response_t* Network::response_queue_pop() {
    if (response_queue_get_count() == 0)
        return NULL;
    response_t* response = response_queue.at(0);
    response_queue.pop_front();
    return response;
}

size_t Network::response_queue_get_count() {
    return response_queue.size();
}

/**
 * @brief send message to estimated leader
 * 
 * @param request_msg 
 */
void Network::send_message(request_msg_t &request_msg) {
    std::string msg_string = request_msg.SerializeAsString();
    COMM_HEADER_TYPE header = htonl(request_msg.ByteSizeLong());
    
    uint32_t leader_id = get_client()->get_leader_id();
    if (!servers[leader_id].connected) {
        std::cout << "[Network::send_message] leader is not connected." << std::endl;
        return;
    }

    write(servers[leader_id].sock, &header, sizeof(header));
    write(servers[leader_id].sock, msg_string.c_str(), request_msg.ByteSizeLong());
}

void Network::send_transaction(uint32_t recv_id, uint32_t amount, uint64_t req_id) {
    request_msg_t request_msg;
    txn_msg_t* transaction = new txn_msg_t();
    transaction->set_recver_id(recv_id);
    transaction->set_amount(amount);
    transaction->set_sender_id(get_client()->get_client_id());
    request_msg.set_allocated_transaction(transaction);
    request_msg.set_request_id(req_id);
    request_msg.set_client_id(get_client()->get_client_id());
    request_msg.set_type(TRANSACTION_REQUEST);
    send_message(request_msg);
}

void Network::send_balance(uint64_t req_id) {
    request_msg_t request_msg;
    request_msg.set_request_id(req_id);
    request_msg.set_client_id(get_client()->get_client_id());
    request_msg.set_type(BALANCE_REQUEST);
    send_message(request_msg);
}

response_t* client_wait_reply(Client* client, uint32_t timeout_ms) {
    if (client == NULL) {
        return NULL;
    }
    typedef std::chrono::system_clock sysclk;
    auto t0 = sysclk::now();
    while (true) {
        if (client->get_network()->response_queue_get_count()) {
            return client->get_network()->response_queue_pop();
        } 

        auto t1 = sysclk::now();
        auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
        if (dt.count() > timeout_ms) {
            return NULL;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

int main (int argc, char* argv[]) {
    if (argc != 2) {
        print_usage();
        exit(1);
    }
    
    int client_id = atoi(argv[1]);
    if (client_id < 0 || client_id >= CLIENT_COUNT) {
        std::cout << "Your input cid is out of the accepted range." << std::endl;
        print_usage();
        exit(1);
    }
    
    Client client = Client(client_id);

    std::string input;
    while(true) {
        input.clear();
        std::getline(std::cin, input);
        std::stringstream ss(input);
        std::vector<std::string> args;
        while (ss.good()) {
            std::string arg = "";
            ss >> arg;
            args.push_back(arg);
        }
        
        std::string &cmd = args[0];
        if (cmd.compare("transfer") == 0 || cmd.compare("t") == 0 || cmd.compare("T") == 0) 
        {
            if (args.size() != 3) {
                std::cout << "wrong format." << std::endl;
                std::cout << "transfer <recv_id> <amount>" << std::endl;
                continue;
            }
            uint32_t recv_id = atoi(args[1].c_str());
            float amount = atof(args[2].c_str());
            uint64_t request_id = 0;
            do {
                client.get_network()->send_transaction(recv_id, amount, request_id);
                auto response = client_wait_reply(&client, CLIENT_REQ_TIMEOUT_MS);
                if (response == NULL) {
                    std::cout << "[main] request timeout. please retry sending the request." << std::endl;
                    break;
                }
                if (response->type != BALANCE_RESPONSE) {
                    std::cout << "[main] wrong response type received. clear response queue. please retry" << std::endl;
                    while (client.get_network()->response_queue_get_count()) {
                        client.get_network()->response_queue_pop();
                    }
                    break;
                }
                
                std::cout << "[main] transfer status: " << ((response->succeed) ? "succeed" : "failed") << std::endl;
                break;
            } while (true);
            
        } 
        else if (cmd.compare("balance") == 0 || cmd.compare("b") == 0 || cmd.compare("B") == 0) 
        {
            do {
                client.get_network()->send_balance(0);
                auto response = client_wait_reply(&client, CLIENT_REQ_TIMEOUT_MS);
                if (response == NULL) {
                    std::cout << "[main] request timeout. please retry sending the request." << std::endl;
                    break;
                }
                if (response->type != BALANCE_RESPONSE) {
                    std::cout << "[main] wrong response type received. clear response queue. please retry" << std::endl;
                    while (client.get_network()->response_queue_get_count()) {
                        client.get_network()->response_queue_pop();
                    }
                    break;
                }

                std::cout << "[main] balance check status: " << ((response->succeed) ? "succeed" : "failed") << std::endl;
                if (response->succeed) {
                    std::cout << "[main] balance amount: " << response->balance << std::endl;
                }
                break;
            } while (true);
        }
        else if (cmd.compare("p") == 0) // for debug only
        {
            while(client.get_network()->response_queue_get_count()) {
                auto res = client.get_network()->response_queue_pop();
                auto type = res->type;
                if (type == TRANSACTION_RESPONSE) {
                    std::cout << "type: TRANSACTION_RESPONSE" << std::endl;
                    std::cout << "request id: " << res->request_id << " succeed: " << res->succeed << std::endl;
                } else if (type == BALANCE_RESPONSE) {
                    std::cout << "type: BALANCE_RESPONSE" << std::endl;
                    std::cout << "request id: " << res->request_id << " succeed: " << res->succeed << " balance: " << res->balance << std::endl;
                }
                std::cout << "--------------------------------------------" << std::endl;
            }
        }
    }
    return 0;
}






