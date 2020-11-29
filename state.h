#pragma once
#include "server.h"

class State {
private:
    Server* context;
public:
    State(Server* context) {this->context = context;};
    virtual void run() {};
};

class CandidateState : public State {
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
