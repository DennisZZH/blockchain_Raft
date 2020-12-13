#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "parameter.h"
#include "mesh.h"
using namespace RaftMesh;

void Mesh::Mesh() {
    setup_mesh_server();
}

void Mesh::setup_mesh_server() {
    mesh_sock = socket(AF_INET, SOCK_STREAM, 0);
    int status = 0;
    if (setsockopt(mesh_sock, SOL_SOCKET, SO_REUSEPORT, &status, sizeof(status)) < 0) {
        std::cerr << "[Mesh::setup_mesh_server] failed to set the socket options." << std::endl;
        exit(1);
    }
    
    sockaddr_in bind_addr = {0};
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_addr.s_addr = inet_addr(MESH_IP);
    bind_addr.sin_port = htons(MESH_PORT);
    
    if (bind(mesh_sock, (sockaddr*) &bind_addr, sizeof(sockaddr_in)) < 0) {
        std::cerr << "[Mesh::setup_mesh_server] failed to bind the socket to mesh port." << std::endl;
        exit(1);
    }
    
    if (listen(mesh_sock, SERVER_COUNT) < 0) {
        std::cerr << "[Mesh::setup_mesh_server] failed to listen the port." << std::endl;
        exit(1);
    }

    wait_conn_thread = std::thread(&Mesh::wait_conn_handler, this);
}

void Mesh::wait_conn_handler() {
    while (!is_stopped) {
        sockaddr_in replica_addr = {0};
        socklen_t addr_size = sizeof(replica_addr);
        
        int replica_sock = accept(mesh_sock, (sockaddr*) &replica_addr, &addr_size);
        if (replica_sock < 0) {
            std::cout << "[Mesh::wait_conn_handler] failed to accept client." << std::endl;
            continue;
        }

        // by subtracting the client base port, we can get the client id here
        int replica_id = ntohs(replica_addr.sin_port) - RAFT_CLIENT_BASE_PORT;
        
        if (replica_id > 2 || replica_id < 0) {
            std::cerr << "[Mesh::wait_conn_handler] received invalid replica_id: " << replica_id << std::endl;
            continue;
        }

        if (servers[replica_id].connected == true) {
            std::cerr << "[Mesh::wait_conn_handler] replica: " << replica_id << " is already connected." << std::endl;
            continue;
        }

        // if there is a lost connection before, need to try to free the previous connection.
        if (servers[replica_id].recv_task != NULL) {
            try {
                servers[replica_id].recv_task->join();
            } catch (...) {}
            delete servers[replica_id].recv_task;
            servers[replica_id].recv_task = NULL;
        }

        if (servers[replica_id].send_task != NULL) {
            try {
                servers[replica_id].send_task->join();
            } catch (...) {}
            delete servers[replica_id].send_task;
            servers[replica_id].send_task = NULL;
        }

        // update the server information
        servers[replica_id].connected = true;
        servers[replica_id].partitioned = false;
        servers[replica_id].sock = replica_sock;
        servers[replica_id].recv_task = new std::thread(&Mesh::recv_handler, this, replica_id);
        servers[replica_id].send_task = new std::thread(&Mesh::send_handler, this, replica_id);
        servers[replica_id].trans_queue.clear(); // REVIEW: Need to double check this one.
    }
}

void Mesh::recv_handler(int replica_id) {
    int replica_sock = servers[replica_id].sock;
    while (!is_stopped && servers[replica_id].connected) {
        COMM_HEADER_TYPE msg_bytes = 0;
        int count = 0;
        count = read(replica_sock, &msg_bytes, sizeof(msg_bytes));
        if (count <= 0) {
            break;
        } else if (count < sizeof(msg_bytes)) {
            std::cout << "[Network::recv_handler] received borken header from replica " << replica_id << std::endl;
            continue; 
        }

        msg_bytes = ntohs(msg_bytes);
        uint8_t* msg = new uint8_t[msg_bytes];
        count = read(replica_sock, msg, msg_bytes);
        if (count <= 0) {
            delete [] msg;
            break;
        } else if (count < sizeof(msg_bytes)) {
            std::cout << "[Network::recv_handler] received borken message from replica " << replica_id << std::endl;
            delete [] msg;
            continue; 
        }

        raft_msg_t raft_msg;
        raft_msg.ParseFromArray(msg, msg_bytes);
        delete [] msg;

        // TODO: decode the message to find the receiver id

        delete [] msg;
        
    }
    servers[replica_id].connected = false;
}

void Mesh::send_handler(int replica_id) {
    int replica_sock = servers[replica_id].sock;
    while (!is_stopped && servers[replica_id].connected) {

    }
    servers[replica_id].connected = false;
}