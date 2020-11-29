#pragma once
#include "server.h"

class State {
private:
    Server* context;
public:
    State(Server* context) {this->context = context;};
    Server* get_context() {return context;};
    virtual void run() {};
};

class CandidateState : public State {
private:
    const uint32_t MSG_CHECK_SLEEP_MS = 50;
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
public:
    LeaderState(Server* context) : State(context) {};
    void run() override;
};
