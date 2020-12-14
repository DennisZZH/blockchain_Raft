#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "parameter.h"
#include "mesh.h"
#include "raft.h"
using namespace RaftMesh;

Mesh::Mesh() {
    setup_mesh_server();
}

Mesh::~Mesh() {
    for (int i = 0; i < SERVER_COUNT; i++) {
        if (servers[i].connected) {
            servers[i].connected = false;
        }
        if (servers[i].recv_task) {
            try {
                servers[i].recv_task->join();
            } catch (...) {};
            delete servers[i].recv_task;
            servers[i].recv_task = NULL;
        }
        if (servers[i].send_task) {
            try {
                servers[i].recv_task->join();
            } catch (...) {};
            delete servers[i].send_task;
            servers[i].send_task = NULL;
        }
        close(servers[i].sock);
        flush_server_trans_queue(i);
    }
}

void Mesh::setup_mesh_server() {
    mesh_sock = socket(AF_INET, SOCK_STREAM, 0);
    int status = 0;
    if (setsockopt(mesh_sock, SOL_SOCKET, SO_REUSEADDR, &status, sizeof(status)) < 0) {
        std::cerr << "[Mesh::setup_mesh_server] failed to set the socket options." << std::endl;
        close(mesh_sock);
        exit(1);
    }
    
    sockaddr_in bind_addr = {0};
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_addr.s_addr = inet_addr(MESH_IP);
    bind_addr.sin_port = htons(MESH_PORT);
    
    if (bind(mesh_sock, (sockaddr*) &bind_addr, sizeof(sockaddr_in)) < 0) {
        std::cerr << "[Mesh::setup_mesh_server] failed to bind the socket to mesh port." << std::endl;
        close(mesh_sock);
        exit(1);
    }
    
    if (listen(mesh_sock, SERVER_COUNT) < 0) {
        std::cerr << "[Mesh::setup_mesh_server] failed to listen the port." << std::endl;
        close(mesh_sock);
        exit(1);
    }

    wait_conn_thread = std::thread(&Mesh::wait_conn_handler, this);
}

void Mesh::append_server_trans_queue(int replica_id, trans_queue_item_t* trans_item) {    
    servers[replica_id].trans_queue_lock.lock();
    servers[replica_id].trans_queue.push_back(trans_item);
    servers[replica_id].trans_queue_lock.unlock();
}

trans_queue_item_t* Mesh::pop_server_trans_queue(int replica_id) {
    if (servers[replica_id].trans_queue.size() == 0)
        return NULL;
    trans_queue_item_t* trans_item = servers[replica_id].trans_queue.at(0);
    servers[replica_id].trans_queue.pop_front();
    return trans_item;
}

void Mesh::flush_server_trans_queue(int replica_id) {
    int count = servers[replica_id].trans_queue.size();
    trans_queue_item_t *trans_item = NULL;
    for (int i = 0; i < count; i++) {
        trans_item = pop_server_trans_queue(replica_id);
        delete trans_item->msg;
        delete trans_item;
    }
}

void Mesh::wait_conn_handler() {
    std::cout << "[Mesh::wait_conn_handler] waiting for replicas to connect." << std::endl;
    while (!is_stopped) {
        sockaddr_in replica_addr = {0};
        socklen_t addr_size = sizeof(replica_addr);
        
        int replica_sock = accept(mesh_sock, (sockaddr*) &replica_addr, &addr_size);
        if (replica_sock < 0) {
            std::cout << "[Mesh::wait_conn_handler] failed to accept client." << std::endl;
            continue;
        }

        // by subtracting the client base port, we can get the client id here
        int replica_id = ntohs(replica_addr.sin_port) - REPLICA_CLIENT_BASE_PORT;
        
        if (replica_id > 2 || replica_id < 0) {
            std::cerr << "[Mesh::wait_conn_handler] received invalid replica_id: " << replica_id << std::endl;
            close(replica_sock);
            continue;
        }

        if (servers[replica_id].connected == true) {
            std::cerr << "[Mesh::wait_conn_handler] replica: " << replica_id << " is already connected." << std::endl;
            close(replica_sock);
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
        flush_server_trans_queue(replica_id);
        servers[replica_id].connected = true;
        servers[replica_id].partitioned = false;
        servers[replica_id].sock = replica_sock;
        servers[replica_id].recv_task = new std::thread(&Mesh::recv_handler, this, replica_id);
        servers[replica_id].send_task = new std::thread(&Mesh::send_handler, this, replica_id);
    }
}

void Mesh::recv_handler(int replica_id) {
    std::cout << "[Network::recv_handler] listening server: " << replica_id << " for messages." << std::endl;
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

        msg_bytes = ntohl(msg_bytes);
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

        replica_msg_t *replica_msg = new replica_msg_t();
        replica_msg->ParseFromArray(msg, msg_bytes);
        delete [] msg;

        uint32_t receiver_id = replica_msg->receiver_id();
        if (servers[receiver_id].connected && !servers[receiver_id].partitioned) {
            trans_queue_item_t *trans_item = new trans_queue_item_t();
            trans_item->enqueue_time = clock_t::now();
            trans_item->msg = replica_msg;
            append_server_trans_queue(receiver_id, trans_item);      
        } else {
            delete replica_msg;
        }
    }
    std::cout << "[Network::recv_handler] server: " << replica_id << " disconnected." << std::endl;
    servers[replica_id].connected = false;
    close(servers[replica_id].sock);
}

void Mesh::send_handler(int replica_id) {
    int replica_sock = servers[replica_id].sock;
    while (!is_stopped && servers[replica_id].connected) {
         trans_queue_item_t *trans_item = pop_server_trans_queue(replica_id);
        if (trans_item == NULL) {
            std::this_thread::sleep_for(milliseconds_t(100));
            continue;
        }
        
        // simulate the network delay
        auto diff_ms = std::chrono::duration_cast<milliseconds_t>(clock_t::now() - trans_item->enqueue_time);
        if (diff_ms.count() < MESH_NETWORK_DELAY_MS) {
            std::this_thread::sleep_for(milliseconds_t(MESH_NETWORK_DELAY_MS - diff_ms.count()));
        }
        
        // fetch the replica message from the trans_item and then trans_item is useless
        replica_msg_t *msg = trans_item->msg;
        delete trans_item;
        
        // the the replica is partitioned, then the message shouldn't reach
        if (servers[replica_id].partitioned) {
            delete msg;
            continue;
        }

        // transfer the replica message
        // 1. transfer the header first
        COMM_HEADER_TYPE trans_bytes = htonl(msg->ByteSizeLong());
        write(servers[replica_id].sock, &trans_bytes, sizeof(trans_bytes));

        // 2. transfer the body next
        std::string trans_str = msg->SerializeAsString();
        write(servers[replica_id].sock, trans_str.c_str(), msg->ByteSizeLong());
        delete msg;
    }
    servers[replica_id].connected = false;
}

int main(int argc, char* argv[]) {
    Mesh mesh;
    while(true);
}