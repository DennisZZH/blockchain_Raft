#pragma once
#include "server.h"
#include "parameter.h"

class State {
private:
    Server* context;

protected:
    const uint32_t MSG_CHECK_SLEEP_MS = 50;
    uint32_t curr_election_timeout;

public:
    State(Server* context) {this->context = context;};
    Server* get_context() {return context;};
    void gen_election_timeout() {curr_election_timeout = ELECTION_TIMEOUT_MS - ELECTION_TIMEOUT_MS / 4 + rand() % (ELECTION_TIMEOUT_MS / 2);};
    virtual void run() {};
};

class CandidateState : public State {
private:
    uint32_t vote_count;
public:
    CandidateState(Server* context) : State(context) {};
    void run() override;
};

class FollowerState : public State {    
public:
    FollowerState(Server* context) : State(context) {};
    void run() override;
};

class LeaderState : public State {
private:
    uint32_t nextIndex[CLIENT_COUNT];
    std::chrono::system_clock::time_point last_heartbeat_time;
    void send_heartbeat();
public:
    LeaderState(Server* context) : State(context) {};
    void run() override;
};
