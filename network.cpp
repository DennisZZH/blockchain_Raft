#include "network.h"
#include <cstdlib>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


Network::Network(int id) {
    server_id = id;
    if (id >= MAX_CLIENT_NUM) {
        std::cerr << "The id number should be between: 0 to " << MAX_CLIENT_NUM - 1 << std::endl;
        exit(1);
    }

    // Initialize the info of all clients
    for (int i = 0; i < MAX_CLIENT_NUM; i++) {
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
    server_addr.sin_addr.s_addr = inet_addr(CLIENT_IP);
    server_addr.sin_port = htons(SERVER_BASE_PORT + server_id);

    if (bind(replica_server_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "[setup_replica_server] Failed to bind the socket." << std::endl;
        exit(1);
    }

    if (listen(replica_server_fd, MAX_CLIENT_NUM * 2) < 0) {
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
    addr.sin_addr.s_addr = inet_addr(CLIENT_IP);
    addr.sin_port = htons(CLIENT_CONN_BASE_PORT + server_id);
    
    if (bind(replica_server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "[setup_replica_server] Failed to bind the socket." << std::endl;
        exit(1);
    }

    if (listen(replica_server_fd, MAX_CLIENT_NUM * 2) < 0) {
        std::cerr << "[setup_replica_server] Failed to listen the port." << std::endl;
        exit(1);
    }
    
    client_wait_thread = std::thread(&Network::wait_clients, this);
    client_recycle_thread = std::thread(&Network::recycle_clients, this);
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
                self_addr.sin_addr.s_addr = inet_addr(CLIENT_IP);
                // Use the following method to avoid port conflicts. Note the following way only works for 3 sites.
                self_addr.sin_port = htons(CLIENT_BASE_PORT + server_id + id);       // Stupid way to avoid port conflicts.
                
                if (bind(socket_fd, (sockaddr*) &self_addr, sizeof(self_addr)) < 0) {
                    std::cerr << "[setup_connections] Failed to bind self address." << std::endl;
                    close(socket_fd);

                    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
                    continue;
                }
                
                sockaddr_in server_addr = {0};
                server_addr.sin_family = AF_INET;
                server_addr.sin_addr.s_addr = inet_addr(CLIENT_IP);
                server_addr.sin_port = htons(SERVER_BASE_PORT + id);
                
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
                replicas_info[id].task = std::thread(&Network::receive_message, this, id);
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
        int peer_id = peer_port_num - CLIENT_BASE_PORT - server_id;
        std::cout << "[wait_connections] Accepted connection from replica: " << peer_id << std::endl;
        
        if (replicas_info[peer_id].valid == false) {
            try { 
                replicas_info[peer_id].task.join();
            } catch(...) {};  
            replicas_info[peer_id].valid = true;
            replicas_info[peer_id].socket = peer_socket;
            replicas_info[peer_id].task = std::thread(&Network::receive_message, this, peer_id);
        } else {
            std::cout << "[wait_connections] Couldn't find empty replica info socket." << std::endl;
            close(peer_socket);
        }
    }
}

void Network::receive_message(int id) {
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
void Network::wait_clients() {
    while (!stop_flag) {
        sockaddr_in client_addr = {0};
        socklen_t client_addr_size = sizeof(client_addr);
        int client_socket = accept(client_server_fd, (sockaddr*) &client_addr, &client_addr_size);
        if (client_socket < 0) {
            std::cout << "[wait_clients] Failed to accept client." << std::endl;
            continue;
        }
        
        client_info_t* client_info = new client_info_t();
        client_info->socket = client_socket;
        client_info->valid = true;
        client_info->task = std::thread(Network::receive_clients_message, client_info);
    }
}

/**
 * @brief Client message handler function running after any client is accepted.
 *        When the connection is lost, this function should add the client_info to the recycling list to free dynamic memory.
 * 
 * @param client_info 
 */
void Network::receive_clients_message(client_info_t* client_info) {
    while (!stop_flag) {
        
    }
    // The client connection is lost. Need to free the dynamically allocated client information.
    clients_recycling.push_back(client_info);
}

void Network::recycle_clients() {
    while (!stop_flag) {
        if (clients_recycling.size() == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(RECYCLE_CHECK_SLEEP_MS));
            continue;
        }
        client_info_t* recycle_client_info = clients_recycling.at(0);
        if (recycle_client_info != NULL) {
            std::cout << "[Network::recycle_clients] deallocated client on socket: " << recycle_client_info->socket << std::endl; 
            delete recycle_client_info;
        }
        clients_recycling.pop_front();
    }
}


void Network::send_message(msg_t &msg, int id) {
    return;
}

/**
 * @brief This function should fill the msg using the info of the first msg in the buffer and delete
 *        the first msg in the buffer.
 * 
 * @param msg 
 */
void Network::pop_message(msg_t &msg) {
    // TODO: Pop out a message at the front of the queue.
    // TODO: Fill in the msg.
}

size_t Network::get_message_count() {
    return server_message_queue.size();
}





