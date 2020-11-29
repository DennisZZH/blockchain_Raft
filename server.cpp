#include "network.h"
#include "raft.h"
#include "server.h"
#include "state.h"

Server::Server() {
    curr_term = 0;
    voted_candidate = -1;
    blockchain = Blockchain();
    curr_state = NULL;
    set_state(new CandidateState(this));
}

Server::~Server() {
    if (curr_state != NULL) {
        delete curr_state;
    }
    if (next_state != NULL) {
        delete next_state;
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

