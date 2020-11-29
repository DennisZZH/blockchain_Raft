#pragma once
#include "network.h"
#include "blockchain.h"

#define SERVER_COUNT 3

// declare State class.
class State;

class Server {
private:
    int id;
    Network* network;

    // raft related
    term_t curr_term;
    int voted_candidate;
    Blockchain blockchain;

    // state related
    State* curr_state;
    State* next_state;

public:
    Server(int id);
    ~Server();

    int get_id() {return id;};
    Network* get_network() {return network;};

    // state related
    void run_state();
    void set_state(State* state);

    // raft related
    term_t increment_term() {return ++curr_term;};               // increse the term and return the new term value.
    term_t get_curr_term() {return curr_term;};                 
    void set_voted_candidate(int candidate_id) {voted_candidate = candidate_id;};
    int get_voted_candidate() {return voted_candidate;};
    Blockchain& get_blockchain() {return blockchain;}; 
};