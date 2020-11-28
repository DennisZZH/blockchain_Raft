#include "context.h"
#include <iostream>

Context::Context() {
    curr_state = NULL;          
    set_state(new CandidateState());    // Everyone starts from the candidate state.
    
}

Context::~Context() {
    if (curr_state != NULL) {
        delete curr_state;
    }

    if (next_state != NULL) {
        delete next_state;
    } 
}

/**
 * @brief set the state machine to transition to a new state in the following step.
 * 
 * @param state 
 */
void Context::set_state(State* state) {
    if (state == NULL) {
        return;
    }
    next_state = state;
}

/**
 * @brief run the state machine for one step.
 * 
 */
void Context::run_state() {
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


