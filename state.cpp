#include <chrono>
#include <ctime>
#include "state.h"
#include "raft.h"
#include "server.h"

// Candidate State
void CandidateState::run() {
    Network* network = get_context()->get_network();
        
    gen_election_timeout();

    // Increment the current term, and vote for myself
    term_t term = get_context()->increment_term();
    get_context()->set_voted_candidate(get_context()->get_id());
    vote_count = 1;

    // Reset the election timeout timer
    auto election_timestamp = std::chrono::system_clock::now();

    // Make the request vote rpc
    request_vote_rpc_t* rpc = new request_vote_rpc_t();
    // rpc->last_log_index = 
    // rpc->last_log_term = 
    rpc->term = term;
    
    msg_t send_msg;
    send_msg.type = REQ_VOTE_RPC;
    send_msg.payload = rpc;
    // Send the message to other servers
    network->replica_send_message(send_msg);
    delete rpc;

    msg_t recv_msg;
    while(true) {
        // Get the time difference first for checking the timeout.
        auto curr_timestamp = std::chrono::system_clock::now();
        auto dt = curr_timestamp - election_timestamp;
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dt);

        // if the election is not finished within the timeout, need to end the current election.
        if (ms.count() > curr_election_timeout) {
            goto exit;
        }
        
        // if the message buffer is empty then do nothing, waiting for another round to check.
        if (network->replica_get_message_count() == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(MSG_CHECK_SLEEP_MS));
            continue;
        }

        network->replica_pop_message(recv_msg);
        if (recv_msg.type == REQ_VOTE_RPC) {
            auto vote_rpc = (request_vote_rpc_t*)recv_msg.payload;
            // If the term is lower then the current term, then ignore.
            if (vote_rpc->term < get_context()->get_curr_term()) {
                continue;
            }
            // If the term is higher than mine,  then I should step down.
            if (vote_rpc->term > get_context()->get_curr_term()) {
                get_context()->set_state(new FollowerState(get_context()));
                goto exit;
            }
            // Reject the vote request because I already voted for myself.
            request_vote_reply_t vote_rpl = {0};
            vote_rpl.term = get_context()->get_curr_term();
            vote_rpl.vote_granted = false;
            // Wrap the reply with the msg_t for sending.
            send_msg.type = REQ_VOTE_RPL;
            send_msg.payload = &vote_rpl;
            network->replica_send_message(send_msg, vote_rpc->candidate_id);

        } else if (recv_msg.type == REQ_VOTE_RPL) {
            auto vote_reply = (request_vote_reply_t*)recv_msg.payload;
            // If the reply term is higher then the current term, it means I am slow so I need to step down.
            // REVIEW: Step down to be what, follower?
            if (vote_reply->term > get_context()->get_curr_term()) {
                get_context()->set_state(new FollowerState(get_context()));
                goto exit;
            }
            else if (vote_reply->term == get_context()->get_curr_term() && vote_reply->vote_granted) {
                // If got the majority of votes, then proceed.
                if (++vote_count >= SERVER_COUNT / 2 + 1) {
                    get_context()->set_state(new LeaderState(get_context()));
                    goto exit;
                }
            }
        } else if (recv_msg.type == APP_ENTR_RPC) {
            auto append_rpc = (append_entry_rpc_t*)recv_msg.payload;
            // REVIEW: The new elected leader should have the same or larger term.
            // The new leader should send a empty heartbeat, so shouldn't need to append.
            if (append_rpc->term >= get_context()->get_curr_term()) {
                get_context()->set_state(new FollowerState(get_context()));
                goto exit;
            }
        } else {
            // For other cases, we can ignore. But need to free the dynamic memory taken by the recv_msg.
            free(recv_msg.payload);
            recv_msg.payload = NULL;
        }
    }

exit:
    if (recv_msg.payload != NULL) {
        free(recv_msg.payload);
    }
    return; 
}

// Follower State
void FollowerState::run() {
    Network* network = get_context()->get_network();

    gen_election_timeout();
    auto last_time = std::chrono::system_clock::now();
    auto curr_time = last_time;

    while (true) {
        
        auto curr_time = std::chrono::system_clock::now();
        auto dt = curr_time - last_time;
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dt);
        
        if (ms.count() > curr_election_timeout) {
            get_context()->set_state(new CandidateState(get_context()));
            goto exit;
        }

        if (network->replica_get_message_count() == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(MSG_CHECK_SLEEP_MS));
            continue;
        }

        // Receiving valid PRC
        msg_t msg;
        network->replica_pop_message(msg);
        if (msg.type == APP_ENTR_RPC) {
            auto append_rpc = (append_entry_rpc_t*) msg.payload;
            if (append_rpc->term < get_context()->get_curr_term()) {
                continue;
            }
            // Update the timestamp if the term is the lastest term.
            last_time = std::chrono::system_clock::now();
            
            // REVIEW: Check if the append RPC is just a heartbeat.
            // REVIEW: Don't need to reply?
            if (append_rpc->entry_count == 0) {
                continue;
            }

            // TODO: Implement the raft sync algorithm.


        } else if (msg.type == REQ_VOTE_RPC) {
            auto vote_rpc = (request_vote_rpc_t*) msg.payload;
            // Make sure the candidate has the newer term.
            if (vote_rpc->term <= get_context()->get_curr_term()) {
                continue;
            }

            // TODO: Need to compare with the latest term I got.
        } 
        // REVIEW: Simply ignore other messages
    }
exit:
    return;
}

// Leader State 
void LeaderState::send_heartbeat() {
    append_entry_rpc_t* heartbeat = new append_entry_rpc_t();
    heartbeat->term = get_context()->get_curr_term();
    heartbeat->leader_id = get_context()->get_id();
    heartbeat->entry_count = 0;                                     // Heartbeat doesn't contain any log.
    
    // Need to wrap the heartbeat with msg_t because it's the msg used by the network.
    msg_t msg;
    msg.type = APP_ENTR_RPC;
    msg.payload = heartbeat;

    // Transmit the heartbeat.
    get_context()->get_network()->replica_send_message(msg);
    delete heartbeat;

    last_heartbeat_time = std::chrono::system_clock::now();
}


void LeaderState::run() { 
    // Need to send the initial heartbeat first.
    send_heartbeat();
    while (true) {
        auto curr_time = std::chrono::system_clock::now();
        auto dt = curr_time - last_heartbeat_time;
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dt);
        
        if (ms.count() >= HEARTBEAT_PERIOD_MS) {
            send_heartbeat();
            continue;
        }

        // TODO: Calculate next_index for each entry.
        // REVIEW: How to get the lastest log entry from the servers?

        // TODO: Fetch the client command.
        // TODO: Append new entry to local.
        // TODO: Do the sync if the last log index >= next_index. If fails decrement next_index aand retry.
        // TODO: Mark log commited if stored on a majority and at least one entry stored in the current term.
        // TODO: Step down if the current term changes.
    }
    return;
}
