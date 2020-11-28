#pragma once
#include "state.h"

class Context {
public:
    Context();
    ~Context();
    void run_state();
    void set_state(State* state);
private:
    State* curr_state;
    State* next_state;
};
