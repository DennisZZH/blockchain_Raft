#include <chrono>
#include <ctime>
#include "state.h"
#include "raft.h"

void CandidateState::run() {
    auto election_timestamp = std::chrono::system_clock::now();
    
    while(true) {
        // Get the time difference first for checking the timeout.
        auto curr_timestamp = std::chrono::system_clock::now();
        auto dt = curr_timestamp - election_timestamp;
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dt);

        // if the election is not finished within the timeout, need to end the current election.
        if (ms.count() > ELECTION_TIMEOUT_MS) {
    
            break;
        }

    }
}

void FollowerState::run() {

}

void LeaderState::run() {

}
