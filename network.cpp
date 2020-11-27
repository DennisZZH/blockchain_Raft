#include "network.h"
#include <cstdlib>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


Network::Network(int id) {
    client_id = id;
    if (id >= MAX_CLIENT_NUM) {
        std::cerr << "The id number should be between: 0 to " << MAX_CLIENT_NUM - 1 << std::endl;
        exit(1);
    }

    // Initialize the info of all clients
    for (int i = 0; i < MAX_CLIENT_NUM; i++) {
        clients_info[i].socket = 0;
        clients_info[i].valid = false;
    }

    setup_server();
    wait_thread = std::thread(&Network::wait_connection, this);
    conn_thread = std::thread(&Network::setup_connections, this);
}

void Network::setup_server() {
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    int flag;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEPORT, &flag, sizeof(flag)) < 0) {
        std::cerr << "[setup_server] Failed to set the socket options." << std::endl;
        exit(1);
    }
    sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(CLIENT_IP);
    server_addr.sin_port = htons(SERVER_BASE_PORT + client_id);

    if (bind(socket_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "[setup_server] Failed to bind the socket." << std::endl;
        exit(1);
    }

    if (listen(socket_fd, MAX_CLIENT_NUM * 2) < 0) {
        std::cerr << "[setup_server] Failed to listen the port." << std::endl;
        exit(1);
    }
}

void Network::setup_connections() {
    while(!stop_flag) {
        for (int id = 0; id < client_id; id++) {
            if (clients_info[id].valid == false) {
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
                self_addr.sin_port = htons(CLIENT_BASE_PORT + client_id + id);       // Stupid way to avoid port conflicts.
                
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
                    clients_info[id].task.join();
                } catch(...) {};  
                clients_info[id].valid = true;
                clients_info[id].socket = socket_fd;
                clients_info[id].task = std::thread(&Network::receive_message, this, id);
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
        int peer_socket = accept(socket_fd, (sockaddr*) &peer_addr, &peer_addr_size);
        if (peer_socket < 0) {
            std::cout << "[wait_connections] Failed to accept client." << std::endl;
            continue;
        }

        int peer_port_num = ntohs(peer_addr.sin_port);
        int peer_id = peer_port_num - CLIENT_BASE_PORT - client_id;
        std::cout << "[wait_connections] Accepted connection from client: " << peer_id << std::endl;
        
        if (clients_info[peer_id].valid == false) {
            try { 
                clients_info[peer_id].task.join();
            } catch(...) {};  
            clients_info[peer_id].valid = true;
            clients_info[peer_id].socket = peer_socket;
            clients_info[peer_id].task = std::thread(&Network::receive_message, this, peer_id);
        } else {
            std::cout << "[wait_connections] Couldn't find empty client_info socket." << std::endl;
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
        
        int size = read(clients_info[id].socket, buffer, 10);
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
    close(clients_info[id].socket);
    clients_info[id].valid = false;
}