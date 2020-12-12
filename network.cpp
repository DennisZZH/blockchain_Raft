#include <cstdlib>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "network.h"
#include "message.h"

Network::Network(int id) {
    server_id = id;
    if (id >= REPLICA_NODE_COUNT) {
        std::cerr << "The id number should be between: 0 to " << REPLICA_NODE_COUNT - 1 << std::endl;
        exit(1);
    }

    // Initialize the info of all clients
    for (int i = 0; i < REPLICA_NODE_COUNT; i++) {
        replicas_info[i].socket = 0;
        replicas_info[i].valid = false;
    }

    setup_replica_server();
    setup_client_server();
}

/**
 * @brief Initialize the server and connections between replicas.
 * 
 */
void Network::setup_replica_server() {
    replica_server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int flag;
    if (setsockopt(replica_server_fd, SOL_SOCKET, SO_REUSEPORT, &flag, sizeof(flag)) < 0) {
        std::cerr << "[setup_replica_server] Failed to set the socket options." << std::endl;
        exit(1);
    }
    sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(REPLICA_SERVER_IP);
    server_addr.sin_port = htons(REPLICA_SERVER_BASE_PORT + server_id);

    if (bind(replica_server_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "[setup_replica_server] Failed to bind the socket." << std::endl;
        exit(1);
    }

    if (listen(replica_server_fd, REPLICA_NODE_COUNT * 2) < 0) {
        std::cerr << "[setup_replica_server] Failed to listen the port." << std::endl;
        exit(1);
    }

    replica_wait_thread = std::thread(&Network::wait_connection, this);
    replica_conn_thread = std::thread(&Network::setup_connections, this);
}

/**
 * @brief Initialize the server so that the site can accept user's connection for requests.
 * 
 */
void Network::setup_client_server() {
    client_server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int flag;
    if (setsockopt(client_server_fd, SOL_SOCKET, SO_REUSEPORT, &flag, sizeof(flag)) < 0) {
        std::cerr << "[setup_client_server] Failed to set the socket options." << std::endl;
        exit(1);
    }

    sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    addr.sin_port = htons(SERVER_BASE_PORT + server_id);
    
    if (bind(client_server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "[setup_replica_server] Failed to bind the socket." << std::endl;
        exit(1);
    }

    // double the client_count to leave some margin
    if (listen(client_server_fd, CLIENT_COUNT * 2) < 0) {
        std::cerr << "[setup_replica_server] Failed to listen the port." << std::endl;
        exit(1);
    }
    
    client_wait_thread = std::thread(&Network::client_wait_handler, this);
    client_recycle_thread = std::thread(&Network::client_recycle_handler, this);
}

// Replicas
void Network::setup_connections() {
    while(!stop_flag) {
        for (int id = 0; id < server_id; id++) {
            if (replicas_info[id].valid == false) {
                int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
                int flag;
                if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEPORT, &flag, sizeof(flag)) < 0) {
                    std::cerr << "[setup_server] Failed to set the socket options." << std::endl;
                    exit(1);
                }

                sockaddr_in self_addr = {0};
                self_addr.sin_family = AF_INET;
                self_addr.sin_addr.s_addr = inet_addr(REPLICA_SERVER_IP);
                // Use the following method to avoid port conflicts. Note the following way only works for 3 sites.
                self_addr.sin_port = htons(REPLICA_CLIENT_BASE_PORT + server_id + id);       // Stupid way to avoid port conflicts.
                
                if (bind(socket_fd, (sockaddr*) &self_addr, sizeof(self_addr)) < 0) {
                    std::cerr << "[setup_connections] Failed to bind self address." << std::endl;
                    close(socket_fd);

                    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
                    continue;
                }
                
                sockaddr_in server_addr = {0};
                server_addr.sin_family = AF_INET;
                server_addr.sin_addr.s_addr = inet_addr(REPLICA_SERVER_IP);
                server_addr.sin_port = htons(REPLICA_SERVER_BASE_PORT + id);
                
                if (connect(socket_fd, (sockaddr*) &server_addr, sizeof(sockaddr_in)) < 0) {
                    std::cerr << "[setup_connections] Failed to connect the peer (server): " << id << std::endl;
                    close(socket_fd);

                    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
                    continue;
                }

                try { 
                    replicas_info[id].task.join();
                } catch(...) {};  
                replicas_info[id].valid = true;
                replicas_info[id].socket = socket_fd;
                replicas_info[id].task = std::thread(&Network::replica_recv_handler, this, id);
                std::cout << "[setup_connections] Connected to " << id << std::endl;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void Network::wait_connection() {
    while (!stop_flag) {
        sockaddr_in peer_addr = {0};
        socklen_t peer_addr_size = sizeof(peer_addr);
        int peer_socket = accept(replica_server_fd, (sockaddr*) &peer_addr, &peer_addr_size);
        if (peer_socket < 0) {
            std::cout << "[wait_connections] Failed to accept replica." << std::endl;
            continue;
        }

        int peer_port_num = ntohs(peer_addr.sin_port);
        int peer_id = peer_port_num - REPLICA_CLIENT_BASE_PORT - server_id;
        std::cout << "[wait_connections] Accepted connection from replica: " << peer_id << std::endl;
        
        if (replicas_info[peer_id].valid == false) {
            try { 
                replicas_info[peer_id].task.join();
            } catch(...) {};  
            replicas_info[peer_id].valid = true;
            replicas_info[peer_id].socket = peer_socket;
            replicas_info[peer_id].task = std::thread(&Network::replica_recv_handler, this, peer_id);
        } else {
            std::cout << "[wait_connections] Couldn't find empty replica info socket." << std::endl;
            close(peer_socket);
        }
    }
}

void Network::replica_recv_handler(int id) {
    // TODO: Receive messages and decode them into a buffer.
    while (!stop_flag) {
        std::cout << "Connected. Receiving messages." << std::endl;
        // TODO: Change this. Dummy variable.
        char buffer[10];
        
        int size = read(replicas_info[id].socket, buffer, 10);
        // std::cout<<"[recv_application] header size read = "<<size<<std::endl;
        if (size <= 0) {
            std::cout << "[recv_application] Connection is lost." << std::endl;
            break;
        }
    }

    // If this thread stops for any reason.
    // Need to free the client socket.
    // close(clients_connected[id].socket);  // May not need this.
    std::cout << "[recv_application] client: " << id << " is exiting." << std::endl;
    close(replicas_info[id].socket);
    replicas_info[id].valid = false;
}

// Clients
void Network::client_wait_handler() {
    while (!stop_flag) {
        sockaddr_in client_addr = {0};
        socklen_t client_addr_size = sizeof(client_addr);
        int client_socket = accept(client_server_fd, (sockaddr*) &client_addr, &client_addr_size);
        if (client_socket < 0) {
            std::cout << "[wait_clients] Failed to accept client." << std::endl;
            continue;
        }

        // by subtracting the client base port, we can get the client id here
        int client_id = ntohs(client_addr.sin_port) - CLIENT_BASE_PORT;
        
        // based on the client id we can save the information in client info array
        if (clients[client_id].connected == true) {
            std::cerr << "[Network::client_wait_handler] client: " << client_id << " is already connected." << std::endl;
            continue;
        }

        // if these is a lost connection before, need to try to free the previous connection
        if (clients[client_id].recv_task != NULL) {
            try {
                clients[client_id].recv_task->join();
            } catch(...) {}
            delete clients[client_id].recv_task;
            clients[client_id].recv_task = NULL;
        }
        
        // update the connected client information based on the client id
        clients[client_id].connected = true;
        clients[client_id].id = client_id;
        clients[client_id].port = ntohs(client_addr.sin_port);
        clients[client_id].sock = client_socket;
        clients[client_id].recv_task = new std::thread(&Network::client_recv_handler, this, client_id);
        
        std::cout << "[Network::client_wait_handler] client: " << client_id << " connected." << std::endl;
    }
}

/**
 * @brief Client message handler function running after any client is accepted.
 *        When the connection is lost, this function should add the client_info to the recycling list to free dynamic memory.
 * 
 * @param client_info 
 */
void Network::client_recv_handler(int client_id) {
    int sock = clients[client_id].sock;
    while (!stop_flag && clients[client_id].connected) {
        COMM_HEADER_TYPE msg_bytes = 0;
        int count = 0;

        // read the message header
        count = read(sock, &msg_bytes, sizeof(msg_bytes));
        if (count <= 0) {
            break;
        } else if (count < sizeof(msg_bytes)) {
            std::cout << "[Network::client_recv_handler] received borken header from client " << client_id << std::endl;
            continue; 
        }

        // read the message body
        msg_bytes = ntohs(msg_bytes);
        uint8_t* msg = new uint8_t[msg_bytes];
        count = read(sock, msg, msg_bytes);
        if (count <= 0) {
            delete [] msg;
            break;
        } else if (count < sizeof(msg_bytes)) {
            std::cout << "[Network::client_recv_handler] received borken message from client " << client_id << std::endl;
            delete [] msg;
            continue; 
        }

        request_msg_t request_msg;
        request_msg.ParseFromArray(msg, msg_bytes);
        
        request_t *request = new request_t();
        bzero(request, sizeof(request_t));
        request->type = (message_type_t)request_msg.type();
        request->request_id = request_msg.request_id();
        if (request->type == TRANSACTION_REQUEST) {
            uint32_t sid = request_msg.transaction().sender_id();
            uint32_t rid = request_msg.transaction().recver_id();
            float amount = request_msg.transaction().amount();
            request->additional = new Transaction(sid, rid, amount);
        }

        client_push_request(request);
        std::cout << "[Network::client_recv_handler] received request from client: " << client_id;
        std::cout << " req# " << request->request_id << std::endl; 
    }
    // The client connection is lost. Need to free the dynamically allocated client information.
    clients[client_id].connected = false;
    
}

/**
 * @brief synchronously push a new request to the request queue
 * 
 * @param request 
 */
void Network::client_push_request(request_t* request) {
    client_req_mutex.lock();
    client_req_queue.push_back(request);    
    client_req_mutex.unlock();
}

/**
 * @brief pop the earliest request from the queue
 * 
 * @return request_t* return null if the queue is empty
 */
request_t* Network::client_pop_request() {
    if (client_get_request_count() == 0)
        return NULL;
    request_t *req = client_req_queue.at(0); 
    client_req_queue.pop_front();
    return req;
}

void Network::replica_send_message(msg_t &msg, int id) {
    return;
}

/**
 * @brief This function should fill the msg using the info of the first msg in the buffer and delete
 *        the first msg in the buffer.
 * 
 * @param msg 
 */
void Network::replica_pop_message(msg_t &msg) {
    // TODO: Pop out a message at the front of the queue.
    // TODO: Fill in the msg.
}

size_t Network::replica_get_message_count() {
    return server_message_queue.size();
}





