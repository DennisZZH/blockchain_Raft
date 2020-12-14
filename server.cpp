#include "network.h"
#include "raft.h"
#include "server.h"
#include "state.h"

Server::Server(int server_id) {
    // Init network
    // 1) Establish connections between server through mesh
    // 2) Establish connections between clients
    id = server_id;
    network = new Network(this);

    // Init persistent state info
    curr_term = 0;
    curr_leader = 0;    // REVIEW: 0 for default
    voted_candidate = NULL_CANDIDATE_ID;
    bc_log.load_file("bc_file_" + std::to_string(id) + ".txt");  // Init bc_log by loading a file
    bal_tab.load_file("bal_tab_" + std::to_string(id) + ".txt"); // Init bal_tab by loading a file

    // Start with FollowerState
    curr_state = NULL;
    set_state(new FollowerState(this));
}

Server::~Server() {
    if (curr_state != NULL) {
        delete curr_state;
    }
    if (next_state != NULL) {
        delete next_state;
    }
    if (network != NULL) {
        delete network;
    }
}

void Server::set_state(State* state) {
    if (state == NULL) {
        return;
    }
    next_state = state;
}

void Server::run_state() {
    // if the next_state is not null, it means there should be a transition to a new state.
    if (next_state != NULL) {
        if (curr_state != NULL) {
            delete curr_state;
        }
        curr_state = next_state;
        next_state = NULL;
    }

    if (curr_state == NULL) {
        std::cerr << "[context::run_state] the current state is null." << std::endl;
        return;
    }

    curr_state->run();
}

void Server::run_state_machine() {
    while (true) {
        run_state();
    }
}

void Server::update_bal_tab_and_committed_index(uint32_t new_index) {
    int old_index = bc_log.get_committed_index();
    for (int bid = old_index + 1; bid <= new_index; bid++) {
        Transaction tmp = bc_log.get_block_by_index(bid).get_txn();
        bal_tab.update_balance(tmp.get_sender_id(), tmp.get_recver_id(), tmp.get_amount());
    }
    bc_log.set_committed_index(new_index);
}