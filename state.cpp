#include <chrono>
#include <ctime>
#include "state.h"
#include "raft.h"
#include "server.h"

void CandidateState::run() {
    Network* network = get_context()->get_network();
    
    // Need to reset the vote count before others vote for me
    vote_count = 0;

    // Increment the current term, and vote for myself
    term_t term = get_context()->increment_term();
    get_context()->set_voted_candidate(get_context()->get_id());

    // Reset the election timeout timer
    auto election_timestamp = std::chrono::system_clock::now();

    // Make the request vote rpc
    request_vote_rpc_t* rpc = new request_vote_rpc_t();
    // rpc->last_log_index = 
    // rpc->last_log_term = 
    rpc->term = term;
    
    msg_t msg;
    msg.type = REQ_VOTE_RPC;
    msg.payload = rpc;
    // Send the message to other servers
    network->send_message(msg);
    delete rpc;

    while(true) {
        // Get the time difference first for checking the timeout.
        auto curr_timestamp = std::chrono::system_clock::now();
        auto dt = curr_timestamp - election_timestamp;
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dt);

        // if the election is not finished within the timeout, need to end the current election.
        if (ms.count() > ELECTION_TIMEOUT_MS) {
            goto exit;
        }
        
        // if the message buffer is empty then do nothing, waiting for another round to check.
        if (network->get_message_count() == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(MSG_CHECK_SLEEP_MS));
            continue;
        }

        network->pop_message(msg);
        if (msg.type == REQ_VOTE_RPC) {
            // Check term
            // Compare the log completion
            // Give vote?
            // REVIEW: How should I deal with the candidate id.
            // REVIEW: What if there are same, break tie using pid?
            // REVIEW: If all of the people are in candidate mode, can they vote for other people? They already vote for themselves.
        } else if (msg.type == REQ_VOTE_RPL) {
            auto v_reply = (request_vote_reply_t*)msg.payload;
            // If the reply term is higher then the current term, it means I am slow so I need to step down.
            // REVIEW: Step down to be what, follower?
            if (v_reply->term > get_context()->get_curr_term()) {
                get_context()->set_state(new FollowerState(get_context()));
                goto exit;
            }
            else if (v_reply->term == get_context()->get_curr_term() && v_reply->vote_granted) {
                // If got the majority of votes, then proceed.
                if (++vote_count >= SERVER_COUNT / 2 + 1) {
                    get_context()->set_state(new LeaderState(get_context()));
                    goto exit;
                }
            }
        } else if (msg.type == APP_ENTR_RPC) {
            auto append_rpc = (append_entry_rpc_t*)msg.payload;
            // REVIEW: The new elected leader should have the same or larger term.
            // The new leader should send a empty heartbeat, so shouldn't need to append.
            if (append_rpc->term >= get_context()->get_curr_term()) {
                get_context()->set_state(new FollowerState(get_context()));
                goto exit;
            }
        } else {
            // For other cases, we can ignore. But need to free the dynamic memory taken by the msg.
            free(msg.payload);
            msg.payload = NULL;
        }
    }
exit:
    if (msg.payload != NULL) {
        free(msg.payload);
    }
    return; 
}

void FollowerState::run() {
    return;
}

void LeaderState::run() {
    return;
}
