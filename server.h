#pragma once
#include "network.h"
#include "blockchain.h"
#include "balance_table.h"
#include "parameter.h"
#include <vector>

// declare State class.
class State;

class Server {
private:
    int id;
    Network* network;

    // raft persistent state info
    uint32_t curr_leader;           // Current leader id
    term_t curr_term;               // Current term on this server
    int voted_candidate;            // The candidate this server has voted in the current term   
    Blockchain bc_log;              // The log of this server
    BalanceTable bal_tab;           // The state machine of this server

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
    void run_state_machine();

    // raft related
    void print_info() {
        std::cout << "server id: " << id << std::endl;
        std::cout << "current leader: " << curr_leader << std::endl;
        std::cout << "voted candidate: " << voted_candidate << std::endl;
        bal_tab.print_bal_tab();
        bc_log.print_block_chain();
    }

    uint32_t get_curr_leader() {return curr_leader;}
    term_t get_curr_term() {return curr_term;}
    int get_voted_candidate() {return voted_candidate;}
    Blockchain& get_bc_log() {return bc_log;}
    BalanceTable& get_bal_tab() {return bal_tab;} 

    void set_curr_leader(uint32_t id) {curr_leader = id;}
    term_t increment_term() {return ++curr_term;}
    void set_curr_term(term_t newterm) {curr_term = newterm;}
    void clear_voted_candidate() {voted_candidate = NULL_CANDIDATE_ID;};                       
    void set_voted_candidate(int candidate_id) {voted_candidate = candidate_id;}
    void update_bal_tab_and_committed_index(uint32_t new_index);
    
};