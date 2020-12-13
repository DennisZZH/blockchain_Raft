#include "network.h"
#include "raft.h"
#include "server.h"
#include "state.h"

Server::Server(int id) {
    // Init network
    // TODO
    // 1) Establish connections between server through mesh
    // 2) Establish connections between clients
    this->id = id;
    network = new Network(id);

    // Init persistent state info
    curr_term = 0;
    voted_candidate = NULL_CANDIDATE_ID;
    bc_log.load_file("bc_file_" + std::to_string(id));  // Init bc_log by loading a file
    bal_tab.load_file("bal_tab_" + std::to_string(id)); // Init bal_tab by loading a file

    // Start with FollowerState
    curr_state = NULL;
    set_state(new FollowerState(this));

    // Start the state machine
    run_state_machine();
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
