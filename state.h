#pragma once

class State {
public:
    State();
    ~State();
    virtual void run();
};

class CandidateState : public State {
public:
    CandidateState();
    ~CandidateState();
    void run() override;
};

class FollowerState : public State {
public:
    FollowerState();
    ~FollowerState();
    void run() override;
};

class LeaderState : public State {
public:
    LeaderState();
    ~LeaderState();
    void run() override;
};
