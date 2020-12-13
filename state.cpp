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
    request_vote_rpc_t rpc;
    rpc.last_log_index = get_context()->get_bc_log().get_last_block().get_index(); 
    rpc.last_log_term = get_context()->get_bc_log().get_last_block().get_term();
    rpc.term = term;
    
    replica_msg_wrapper_t send_msg;
    send_msg.type = REQ_VOTE_RPC;
    send_msg.payload = (void*) &rpc;
    // Send the message to other servers
    network->replica_send_message(send_msg);
     
    replica_msg_wrapper_t msg;

    while(true) {
        // Get the time difference first for checking the timeout.
        auto curr_timestamp = std::chrono::system_clock::now();
        auto dt = curr_timestamp - election_timestamp;
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dt);

        // if the election is not finished within the timeout, need to end the current election.
        if (ms.count() > curr_election_timeout) {
            get_context()->set_state(new CandidateState(get_context()));
            goto exit;
        }
        
        // if the message buffer is empty then do nothing, waiting for another round to check.
        if (network->replica_get_message_count() == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(MSG_CHECK_SLEEP_MS));
            continue;
        }

        // Receiving valid RPC
        network->replica_pop_message(msg);

        if (msg.type == REQ_VOTE_RPC) {
            auto vote_rpc = (request_vote_rpc_t*)msg.payload;
            // If the term is lower or equal to the current term, then ignore.
            // If the term is higher than mine, then I should step down
            if (vote_rpc->term > get_context()->get_curr_term()) {
                get_context()->set_state(new FollowerState(get_context()));
                goto exit;
            }
        } else if (msg.type == REQ_VOTE_RPL) {
            auto vote_reply = (request_vote_reply_t*)msg.payload;
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
        } else if (msg.type == APP_ENTR_RPC) {
            auto append_rpc = (append_entry_rpc_t*)msg.payload;
            // REVIEW: The new elected leader should have the same or larger term. Ignore if smaller
            // The new leader should send a empty heartbeat, so shouldn't need to append.
            if (append_rpc->term >= get_context()->get_curr_term()) {
                get_context()->set_state(new FollowerState(get_context()));
                goto exit;
            }
        }

        // Free payload after done using message
        if (msg.payload != NULL) {
            free(msg.payload);
        }
    }

exit:
    if (msg.payload != NULL) {
        free(msg.payload);
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

        // Receiving valid RPC
        replica_msg_wrapper_t msg;
        network->replica_pop_message(msg);

        // Handle received RPC
        if (msg.type == APP_ENTR_RPC) {
            auto append_rpc = (append_entry_rpc_t*) msg.payload;
            append_entry_reply_t reply;
            
            // Return failure if term is outdated (leader invalid)
            if (append_rpc->term < get_context()->get_curr_term()) { 
                reply.term = get_context()->get_curr_term();
                reply.success = false;
            }
            else {
                // Update the timestamp if the term is the lastest term.
                if (append_rpc->term > get_context()->get_curr_term()) {
                    get_context()->set_curr_term(append_rpc->term);
                    get_context()->clear_voted_candidate();
                }
                // Reset timeout
                last_time = std::chrono::system_clock::now();
                // If the append RPC is just a heartbeat.
                if (append_rpc->entries.size() == 0) {
                    // Comfirm leader
                    get_context()->set_curr_leader(append_rpc->leader_id);
                    // Advance balance table with newly committed entries (Also update committed index of the blockchain)
                    get_context()->update_bal_tab(append_rpc->commit_index);
                    reply.term = reply.term = get_context()->get_curr_term();
                    reply.success = true;
                }
                // If the append RPC contains log entries
                else {
                    // Return failure if log doesn't contain an entry at prevLogIndex whose term matches prevLogTerm
                    if (get_context()->get_bc_log().get_last_block().get_index() != append_rpc->prev_log_index
                        || get_context()->get_bc_log().get_last_block().get_term() != append_rpc->prev_log_term) {
                            reply.term = reply.term = get_context()->get_curr_term();
                            reply.success = false;
                    }
                    else {
                        // If existing entries conflict with new entries, delete all existing entries starting with first conflicting entry
                        // Append any new entries not already in the log
                        get_context()->get_bc_log().clean_up_blocks(append_rpc->prev_log_index + 1, append_rpc->entries);
                        get_context()->get_bc_log().set_committed_index(append_rpc->commit_index);
                        reply.term = reply.term = get_context()->get_curr_term();
                        reply.success = true;
                    }
                }
            }
            // Reply is ready; Prepare a message
            replica_msg_wrapper_t reply_msg;
            reply_msg.type = replica_msg_type_t::APP_ENTR_RPL;
            reply_msg.payload = (void*) &reply;
            network->replica_send_message(reply_msg, append_rpc->leader_id);          
        } 
        else if (msg.type == REQ_VOTE_RPC) {
            auto vote_rpc = (request_vote_rpc_t*) msg.payload;
            request_vote_reply_t reply;
            reply.vote_granted = false;
            
            // Discover larger term
            if (vote_rpc->term > get_context()->get_curr_term()) {
                get_context()->set_curr_term(vote_rpc->term);
                get_context()->clear_voted_candidate();  
            }
            reply.term = get_context()->get_curr_term();

            if (vote_rpc->term == get_context()->get_curr_term()) {
                if (get_context()->get_voted_candidate() == NULL_CANDIDATE_ID || get_context()->get_voted_candidate() == vote_rpc->candidate_id) {
                    if (get_context()->get_bc_log().get_last_block().get_term() < vote_rpc->last_log_term
                        || (get_context()->get_bc_log().get_last_block().get_term() == vote_rpc->last_log_term 
                            && get_context()->get_bc_log().get_last_block().get_index() <= vote_rpc->last_log_index)) {
                            // Grant vote and reset election timeout
                            reply.vote_granted = true;
                            get_context()->set_voted_candidate(vote_rpc->candidate_id);
                            last_time = std::chrono::system_clock::now();
                        }
                }
            }
            // Reply is ready; Prepare a message
            replica_msg_wrapper_t reply_msg;
            reply_msg.type = replica_msg_type_t::REQ_VOTE_RPL;
            reply_msg.payload = (void*) &reply;
            network->replica_send_message(reply_msg, vote_rpc->candidate_id);           
        }
        else {
            // REVIEW: A follower simply ignore all other messages
        }
        // Free payload after done using a msg
        if (msg.payload != NULL) {
            free(msg.payload);
        }  
    }
exit:
    return;
}

// Leader State 
void LeaderState::send_heartbeat() {
    append_entry_rpc_t heartbeat;
    heartbeat.term = get_context()->get_curr_term();
    heartbeat.leader_id = get_context()->get_id();
    // Heartbeat doesn't contain any log.
    
    // Need to wrap the heartbeat with replica_msg_wrapper_t because it's the msg used by the network.
    replica_msg_wrapper_t msg;
    msg.type = APP_ENTR_RPC;
    msg.payload = (void*) &heartbeat;

    // Broadcast the heartbeat to all peers
    get_context()->get_network()->replica_send_message(msg);

    last_heartbeat_time = std::chrono::system_clock::now();
}


void LeaderState::run() {
    Network* network = get_context()->get_network();

    // Initialize nextIndex for each replica to last log index + 1
    int last_log_index = get_context()->get_bc_log().get_last_block().get_index();
    for (int i = 0; i < CLIENT_COUNT; i++) {
        nextIndex[i] = last_log_index + 1;
    }
    // Send the initial heartbeat to all; Declear the fact the I am elected as leader
    send_heartbeat();

    while (true) {
        auto curr_time = std::chrono::system_clock::now();
        auto dt = curr_time - last_heartbeat_time;
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dt);
        
        if (ms.count() >= HEARTBEAT_PERIOD_MS) {
            send_heartbeat();
            continue;
        }

        // If the request buffer is empty then do nothing, waiting for another round to check.
        if (network->client_get_request_count() == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(MSG_CHECK_SLEEP_MS));
            continue;
        }

        // Fetch the client command.
        request_t *msg_ptr =  network->client_pop_request();
        // Append new entry to local.
        

        // TODO: Do the sync if the last log index >= next_index. If fails decrement next_index aand retry.
        // TODO: Mark log commited if stored on a majority and at least one entry stored in the current term.
        // TODO: Step down if the current term changes.
    }
    return;
}
