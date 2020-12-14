#include <cstdlib>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "network.h"
#include "message.h"
#include "server.h"

#define DEBUG_MODE

Network::Network(Server *context) {
    this->context = context;
    setup_replica_server();
    setup_client_server();
}

/**
 * @brief Initialize the server and connections between replicas.
 * 
 */
void Network::setup_replica_server() {
    replica_conn_thread = std::thread(&Network::replica_conn_handler, this);
}

/**
 * @brief thread function that connects the current the current replica to the mesh
 * 
 */
void Network::replica_conn_handler() {
    std::cout << "[Network::replica_conn_handler] trying to connect to other replica through network mesh." << std::endl;
    while(!stop_flag) {
        if (mesh_connected) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }
        
        replica_socket = socket(AF_INET, SOCK_STREAM, 0);
        int flag;
        if (setsockopt(replica_socket, SOL_SOCKET, SO_REUSEPORT, &flag, sizeof(flag)) < 0) {
            std::cerr << "[Network::replica_conn_handler] failed to set the socket options." << std::endl;
            close(replica_socket);
            continue;
        }

        // bind to replica specific ip and port
        // the mesh can identify the replica id based on the port number.
        sockaddr_in bind_addr;
        bind_addr.sin_family = AF_INET;
        bind_addr.sin_addr.s_addr = inet_addr(REPLICA_CLIENT_IP);
        bind_addr.sin_port = htons(REPLICA_CLIENT_BASE_PORT + get_context()->get_id());

        if (bind(replica_socket, (sockaddr*) &bind_addr, sizeof(bind_addr)) < 0) {
            std::cerr << "[Network::replica_conn_handler] failed to bind self address." << std::endl;
            close(replica_socket);
            replica_socket = 0;
            std::this_thread::sleep_for(std::chrono::milliseconds(3000));
            continue;
        }

        // indicate the mesh address and port
        sockaddr_in mesh_addr;
        mesh_addr.sin_family = AF_INET;
        mesh_addr.sin_addr.s_addr = inet_addr(MESH_IP);
        mesh_addr.sin_port = htons(MESH_PORT);
        
        if (connect(replica_socket, (sockaddr*) &mesh_addr, sizeof(mesh_addr)) < 0) {
            std::cerr << "[Network::replica_conn_handler] failed to connect the mesh." << std::endl;
            close(replica_socket);
            std::this_thread::sleep_for(std::chrono::milliseconds(3000));
            continue;
        }

        // just in case the previous receive thread is still running
        // need to join the previous thread
        try {
            replica_recv_thread.join();
        } catch (...) {}
        
        // set the mesh status to be connected and start a new thread.
        mesh_connected = true;
        replica_recv_thread = std::thread(&Network::replica_recv_handler, this);

    }
}

void Network::replica_recv_handler() {
    std::cout << "[Network::replica_recv_handler] start receiving mesh messages." << std::endl;
    while (!stop_flag && mesh_connected) {
        int count = 0;
        COMM_HEADER_TYPE msg_bytes = 0;
        count = read(replica_socket, &msg_bytes, sizeof(msg_bytes));
        if (count <= 0) {
            break;
        } else if (count < sizeof(msg_bytes)) {
            std::cerr << "[Network::replica_recv_handler] received broken header." << std::endl;
            continue;
        }

        msg_bytes = ntohl(msg_bytes);
        uint8_t *msg = new uint8_t[msg_bytes];
        count = read(replica_socket, msg, msg_bytes);
        if (count <= 0) {
            delete [] msg;
            break;
        } else if (count < msg_bytes) {
            delete [] msg;
            std::cerr << "[Network::replica_recv_handler] received broken body." << std::endl;
            continue;
        }

        replica_msg_t replica_msg;
        replica_msg.ParseFromArray(msg, msg_bytes);
        delete [] msg; 

        // based on the message type, parse the information and save to the wrapper object
        replica_msg_wrapper_t *wrapper = new replica_msg_wrapper_t();
        wrapper->type = (replica_msg_type_t) replica_msg.type();
        if (wrapper->type == REQ_VOTE_RPC) {
            request_vote_rpc_t *vote_rpc = new request_vote_rpc_t();
            const request_vote_rpc_msg_t &vote_rpc_msg = replica_msg.request_vote_rpc_msg();
            vote_rpc->candidate_id = vote_rpc_msg.candidate_id();
            vote_rpc->term = vote_rpc_msg.term();
            vote_rpc->last_log_term = vote_rpc_msg.last_log_term();
            vote_rpc->last_log_index = vote_rpc_msg.last_log_index();
            wrapper->payload = (void*) vote_rpc;
        } else if (wrapper->type == REQ_VOTE_RPL) {
            request_vote_reply_t *vote_reply = new request_vote_reply_t();
            const request_vote_reply_msg_t &vote_reply_msg = replica_msg.request_vote_reply_msg();
            vote_reply->term = vote_reply_msg.term();
            vote_reply->vote_granted = vote_reply_msg.vote_granted();
            wrapper->payload = (void*) vote_reply;
        } else if (wrapper->type == APP_ENTR_RPC) {
            append_entry_rpc_t *append_rpc = new append_entry_rpc_t();
            const append_entry_rpc_msg_t &append_rpc_msg = replica_msg.append_entry_rpc_msg();
            append_rpc->term = append_rpc_msg.term();
            append_rpc->leader_id = append_rpc_msg.leader_id();
            append_rpc->prev_log_index = append_rpc_msg.prev_log_index();
            append_rpc->prev_log_term = append_rpc_msg.prev_log_term();
            append_rpc->commit_index = append_rpc_msg.commit_index();
            for (int i = 0; i < append_rpc_msg.entries_size(); i++) {
                Block block;
                Transaction txn;
                const block_msg_t &block_msg = replica_msg.append_entry_rpc_msg().entries(i);
                txn.set_sender_id(block_msg.txn().sender_id());
                txn.set_recver_id(block_msg.txn().recver_id());
                txn.set_amount(block_msg.txn().amount());

                block.set_term(block_msg.term());
                block.set_phash(block_msg.phash());
                block.set_nonce(block_msg.nonce());
                block.set_index(block_msg.index());
                block.set_txn(txn);

                append_rpc->entries.push_back(block);
            }
            wrapper->payload = (void*) append_rpc;
        } else if (wrapper->type == APP_ENTR_RPL) {
            append_entry_reply_t *append_reply = new append_entry_reply_t();
            const append_entry_reply_msg_t &append_reply_msg = replica_msg.append_entry_reply_msg();
            append_reply->term = append_reply_msg.term();
            append_reply->sender_id = append_reply_msg.sender_id();
            append_reply->success = append_reply_msg.success();
            wrapper->payload = (void*) append_reply;
        } else {
            std::cout << "[Network::replica_recv_handler] received unknown type." << std::endl;
        }
        replica_msg_queue.push_back(wrapper);
        std::cout << "[Network::replica_recv_handler] received and saved." << std::endl;
    }
    std::cout << "[Network]::replica_recv_handler] the mesh connection is lost." << std::endl;
    close(replica_socket);
}

void Network::replica_send_message(replica_msg_wrapper_t &msg, int id) {
    if (id < -1 || id >= SERVER_COUNT) {
        std::cout << "[Network::replica_send_message] invalid server id number." << std::endl;
        return;
    }
    if (id == -1) {
        for (int i = 0; i < SERVER_COUNT; i++) {
            if (i == get_context()->get_id())
                continue;
            replica_send_message(msg, i);
        }
        return;
    }
    replica_msg_type_t type = msg.type;
    replica_msg_t send_msg;
    send_msg.set_type(type);
    send_msg.set_receiver_id(id);
    // need to construct the send_msg based on the input msg before sending it.
    if (type == REQ_VOTE_RPC) {
        auto vote_rpc = (request_vote_rpc_t*) msg.payload;
        auto vote_rpc_msg = new request_vote_rpc_msg_t();
        vote_rpc_msg->set_term(vote_rpc->last_log_term);
        vote_rpc_msg->set_candidate_id(vote_rpc->candidate_id);
        vote_rpc_msg->set_last_log_index(vote_rpc->last_log_index);
        vote_rpc_msg->set_last_log_term(vote_rpc->last_log_term);
        send_msg.set_allocated_request_vote_rpc_msg(vote_rpc_msg);
    } else if (type == REQ_VOTE_RPL) {
        auto vote_rpl = (request_vote_reply_t*) msg.payload;
        auto vote_rpl_msg = new request_vote_reply_msg_t();
        vote_rpl_msg->set_term(vote_rpl->term);
        vote_rpl_msg->set_vote_granted(vote_rpl->vote_granted);
        send_msg.set_allocated_request_vote_reply_msg(vote_rpl_msg);
    } else if (type == APP_ENTR_RPC) {
        auto append_rpc = (append_entry_rpc_t*) msg.payload;
        auto append_rpc_msg = new append_entry_rpc_msg_t();
        append_rpc_msg->set_term(append_rpc->term);
        append_rpc_msg->set_leader_id(append_rpc->leader_id);
        append_rpc_msg->set_prev_log_index(append_rpc->prev_log_index);
        append_rpc_msg->set_prev_log_term(append_rpc->prev_log_term);
        append_rpc_msg->set_commit_index(append_rpc->commit_index);
        for (int i = 0; i < append_rpc->entries.size(); i++) {
            auto block_msg = append_rpc_msg->add_entries();
            Block &block = append_rpc->entries.at(i);
            // allocate phash, nonce and txn for the block_msg
            std::string* phash = new std::string(block.get_phash());
            std::string* nonce = new std::string(block.get_nonce());
            auto txn_msg = new txn_msg_t();
            txn_msg->set_sender_id(block.get_txn().get_sender_id());
            txn_msg->set_recver_id(block.get_txn().get_recver_id());
            txn_msg->set_amount(block.get_txn().get_amount());
            // construct the block_msg
            block_msg->set_term(block.get_term());
            block_msg->set_allocated_phash(phash);
            block_msg->set_allocated_nonce(nonce);
            block_msg->set_allocated_txn(txn_msg);
            block_msg->set_index(block.get_index());
        }
        send_msg.set_allocated_append_entry_rpc_msg(append_rpc_msg);
    } else if (type == APP_ENTR_RPL) {
        auto append_reply = (append_entry_reply_t*) msg.payload;
        auto append_reply_msg = new append_entry_reply_msg_t();
        append_reply_msg->set_term(append_reply->term);
        append_reply_msg->set_sender_id(append_reply->sender_id);
        append_reply_msg->set_success(append_reply->success);
        send_msg.set_allocated_append_entry_reply_msg(append_reply_msg);
    } else {
        std::cout << "[Network::replica_send_message] try to send unknown type." << std::endl;
        return;
    }
    
    // send the header first
    COMM_HEADER_TYPE msg_bytes = htonl(send_msg.ByteSizeLong());
    write(replica_socket, &msg_bytes, sizeof(msg_bytes));
    // send the message next
    std::string msg_string = send_msg.SerializeAsString();
    write(replica_socket, msg_string.c_str(), send_msg.ByteSizeLong());
    // no need to free dynamically allocated data because they will be freed by send_msg.
    return;
}

/**
 * @brief This function should fill the msg using the info of the first msg in the buffer and delete
 *        the first msg in the buffer.
 * 
 * @param msg 
 */
void Network::replica_pop_message(replica_msg_wrapper_t &msg) {
    if (replica_get_message_count() == 0) {
        msg.type = NONE;
        return;
    }
    replica_msg_wrapper_t* wrapper = replica_msg_queue.at(0);
    replica_msg_queue.pop_front();
    
    msg.type = wrapper->type;
    msg.payload = wrapper->payload;
    delete wrapper;
}

size_t Network::replica_get_message_count() {
    return replica_msg_queue.size();
}

/* Clients */
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
    addr.sin_port = htons(SERVER_BASE_PORT + get_context()->get_id());
    
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
    // client_recycle_thread = std::thread(&Network::client_recycle_handler, this);
}


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
        int client_id = (ntohs(client_addr.sin_port) - CLIENT_BASE_PORT - get_context()->get_id()) / CLIENT_PORT_MULT;
        
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
    std::cout << "[client_recv_handler] start listening messages from client: " << client_id << std::endl;
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
        msg_bytes = ntohl(msg_bytes);
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
        delete [] msg;
        
        request_t *request = new request_t();
        bzero(request, sizeof(request_t));
        request->type = (message_type_t)request_msg.type();
        request->client_id = get_context()->get_id();
        request->request_id = request_msg.request_id();
        if (request->type == TRANSACTION_REQUEST) {
            uint32_t sid = request_msg.transaction().sender_id();
            uint32_t rid = request_msg.transaction().recver_id();
            float amount = request_msg.transaction().amount();
            request->payload = new Transaction(sid, rid, amount);
        }

        client_push_request(request);
        std::cout << "[Network::client_recv_handler] received request from client: " << client_id;
        std::cout << " req# " << request->request_id << std::endl; 
    }
    // The client connection is lost. Need to free the dynamically allocated client information.
    std::cout << "[Network::client_recv_handler] disconnected from client: " << client_id << std::endl;
    clients[client_id].connected = false;
    close(sock);
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


void Network::client_send_message(response_t& response, int client_id) {
    if (client_id == -1) {
        for (int i = 0; i < CLIENT_COUNT; i++)
            client_send_message(response, i);
        return;
    }

    if (!clients[client_id].connected) {
        std::cout << "[Network::client_send_message] client: " << client_id << " is not connected." << std::endl;
        return;
    }

    response_msg_t response_msg;
    response_msg.set_type(response.type);
    response_msg.set_request_id(response.request_id);
    response_msg.set_succeed(response.succeed);
    // the following values might be not valid depends on the type.
    response_msg.set_balance(response.balance);
    response_msg.set_leader_id(response.leader_id);
    
    COMM_HEADER_TYPE msg_bytes = htonl(response_msg.ByteSizeLong());
    write(clients[client_id].sock, &msg_bytes, sizeof(msg_bytes));
    write(clients[client_id].sock, response_msg.SerializeAsString().c_str(), response_msg.ByteSizeLong());
}

size_t Network::client_get_request_count() {
    return client_req_queue.size();
}








