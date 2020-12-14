#include <chrono>
#include <ctime>
#include "state.h"
#include "raft.h"
#include "server.h"

// Candidate State
void CandidateState::run() {
    std::cout<<"[State::CandidateState::run] Running a Candidate State!"<<std::endl;
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
    rpc.last_log_index = get_context()->get_bc_log().get_last_index(); 
    rpc.last_log_term = get_context()->get_bc_log().get_last_term();
    rpc.candidate_id = get_context()->get_id();
    rpc.term = term;
    
    replica_msg_wrapper_t send_msg;
    send_msg.type = REQ_VOTE_RPC;
    send_msg.payload = (void*) &rpc;
    // Send the message to other servers
    //std::cout<<"[State::CandidateState::run] Sending out requestVotePRCs!"<<std::endl;
    network->replica_send_message(send_msg);
     
    replica_msg_wrapper_t msg;

    while(true) {
        // Get the time difference first for checking the timeout.
        auto curr_timestamp = std::chrono::system_clock::now();
        auto dt = curr_timestamp - election_timestamp;
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dt);

        // if the election is not finished within the timeout, need to end the current election.
        if (ms.count() > curr_election_timeout) {
            std::cout<<"[State::CandidateState::run] Candidate Timeout!"<<std::endl;
            get_context()->set_state(new CandidateState(get_context()));
            return;
        }

        // Check if any client wrongly send request to a candidate
        // Redirect the client by sending leader id
        // REVIEW: a candidate can't know who is leader, no need to reply
        
        // if the message buffer is empty then do nothing, waiting for another round to check.
        if (network->replica_get_message_count() == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(MSG_CHECK_SLEEP_MS));
            continue;
        }

        // Receiving valid RPC
        network->replica_pop_message(msg);

        if (msg.type == REQ_VOTE_RPC) {
            //std::cout<<"[State::CandidateState::run] Received a requestVoteRPC!"<<std::endl;
            auto vote_rpc = (request_vote_rpc_t*)msg.payload;
            // If the term is lower or equal to the current term, then ignore.
            // If the term is higher than mine, then I should step down
            if (vote_rpc->term > get_context()->get_curr_term()) {
                 std::cout<<"[State::CandidateState::run] Step down to Follower State!"<<std::endl;
                get_context()->set_state(new FollowerState(get_context()));
                goto exit;
            }
        } else if (msg.type == REQ_VOTE_RPL) {
             //std::cout<<"[State::CandidateState::run] Recv a requestVoteRPC Reply!"<<std::endl;
            auto vote_reply = (request_vote_reply_t*)msg.payload;
            // If the reply term is higher then the current term, it means I am slow so I need to step down.
            // REVIEW: Step down to be what, follower?
            if (vote_reply->term > get_context()->get_curr_term()) {
                std::cout<<"[State::CandidateState::run] Step down to Follower State!"<<std::endl;
                get_context()->set_state(new FollowerState(get_context()));
                goto exit;
            }
            else if (vote_reply->term == get_context()->get_curr_term() && vote_reply->vote_granted) {
                // If got the majority of votes, then proceed.
                if (++vote_count >= SERVER_COUNT / 2 + 1) {
                    std::cout<<"[State::CandidateState::run] Recv majority vote, Step up to Leader State!"<<std::endl;
                    get_context()->set_state(new LeaderState(get_context()));
                    goto exit;
                }
            }
        } else if (msg.type == APP_ENTR_RPC) {
            //std::cout<<"[State::CandidateState::run] Received a appendEntryRPC!"<<std::endl;
            auto append_rpc = (append_entry_rpc_t*)msg.payload;
            // REVIEW: The new elected leader should have the same or larger term. Ignore if smaller
            // The new leader should send a empty heartbeat, so shouldn't need to append.
            if (append_rpc->term >= get_context()->get_curr_term()) {
                 std::cout<<"[State::CandidateState::run] Step down to Follower State!"<<std::endl;
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
    std::cout<<"[State::FollowerState::run] Running a Follower State!"<<std::endl;
    Network* network = get_context()->get_network();
    gen_election_timeout();
    auto last_time = std::chrono::system_clock::now();
    auto curr_time = last_time;

    while (true) {
        
        auto curr_time = std::chrono::system_clock::now();
        auto dt = curr_time - last_time;
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dt);

        if (ms.count() > curr_election_timeout) {
            std::cout<<"[State::FollowerState::run] Follower State Timeout, Step up to Candidate State!"<<std::endl;
            get_context()->set_state(new CandidateState(get_context()));
            return;
        }

        // Check if any client wrongly send request to a follower
        if (network->client_get_request_count() != 0) {
            //std::cout<<"[State::FollowerState::run] Recv wrong Request from Client, Redirecting!"<<std::endl;
            request_t* request = network->client_pop_request();
            response_t response;
            response.type = LEADER_CHANGE;
            response.leader_id = get_context()->get_curr_leader();
            response.request_id = request->request_id;
            response.succeed = false;
            response.balance = -1;
            network->client_send_message(response, request->client_id);
            if (request->payload != NULL) {
                free(request->payload);
            }
            free(request);
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
            // std::cout<<"[State::FollowerState::run] Received a appendEntryRPC!"<<std::endl;
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
                     // std::cout<<"[State::FollowerState::run] This appendEntryRPC is a HeartBeat!"<<std::endl;
                    // Comfirm leader
                    get_context()->set_curr_leader(append_rpc->leader_id);
                    // Advance balance table with newly committed entries (Also update committed index of the blockchain)
                    get_context()->update_bal_tab_and_committed_index(append_rpc->commit_index);
                    reply.term = reply.term = get_context()->get_curr_term();
                    reply.success = true;
                }
                // If the append RPC contains log entries
                else {
                     // std::cout<<"[State::FollowerState::run] This appendEntryRPC contains Logs!"<<std::endl;
                    // Return failure if log doesn't contain an entry at prevLogIndex whose term matches prevLogTerm
                    if (get_context()->get_bc_log().get_last_index() != append_rpc->prev_log_index
                        || get_context()->get_bc_log().get_last_term() != append_rpc->prev_log_term) {
                            std::cout<<"[State::FollowerState::run] AppendEntry failed due to log inconsistency!"<<std::endl;
                            reply.term = reply.term = get_context()->get_curr_term();
                            reply.success = false;
                    }
                    else {
                        // If existing entries conflict with new entries, delete all existing entries starting with first conflicting entry
                        // Append any new entries not already in the log
                         std::cout<<"[State::FollowerState::run] AppendEntry succeed, Fixing Log!"<<std::endl;
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
            //std::cout<<"[State::FollowerState::run] Received a requestVoteRPC!"<<std::endl;
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
                    if (get_context()->get_bc_log().get_last_term() < vote_rpc->last_log_term
                        || (get_context()->get_bc_log().get_last_term() == vote_rpc->last_log_term 
                            && get_context()->get_bc_log().get_last_index() <= vote_rpc->last_log_index)) {
                            // Grant vote and reset election timeout
                              std::cout<<"[State::FollowerState::run] Grant vote!"<<std::endl;
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
    heartbeat.commit_index = get_context()->get_bc_log().get_committed_index();
    // Heartbeat doesn't contain any log entries, prev log term or index.
    
    // Need to wrap the heartbeat with replica_msg_wrapper_t because it's the msg used by the network.
    replica_msg_wrapper_t msg;
    msg.type = APP_ENTR_RPC;
    msg.payload = (void*) &heartbeat;

    // Broadcast the heartbeat to all peers
    get_context()->get_network()->replica_send_message(msg);

    last_heartbeat_time = std::chrono::system_clock::now();
}


void LeaderState::run() {
    std::cout<<"[State::LeaderState::run] Running a Leader State!"<<std::endl;
    Network* network = get_context()->get_network();

    // Initialize nextIndex for each replica to last log index + 1
    int last_log_index = get_context()->get_bc_log().get_blockchain_length() - 1;
    for (int i = 0; i < SERVER_COUNT; i++) {
        nextIndex[i] = last_log_index + 1;
    }
    // Send the initial heartbeat to all replicas; Declear the fact the I am elected as leader
     std::cout<<"[State::LeaderState::run] Announce HeartBeat!"<<std::endl;
    send_heartbeat();
    // Send Announancement to all clients

    request_t *msg_ptr;
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

        // Fetch a client request, start the protocol
        // std::cout<<"[State::LeaderState::run] Recv a Client Request!"<<std::endl;
        msg_ptr = network->client_pop_request();

        // Get current block info, after append new block, current block will become prev block
        term_t prev_log_term = get_context()->get_bc_log().get_last_term();
        int prev_log_index = get_context()->get_bc_log().get_last_index();
        // Append new entry to local.
        if (msg_ptr->type == BALANCE_REQUEST) {
            get_context()->get_bc_log().add_transaction(get_context()->get_curr_term(), Transaction(true));
        }
        else if (msg_ptr->type == TRANSACTION_REQUEST) {
            get_context()->get_bc_log().add_transaction(get_context()->get_curr_term(), *((Transaction*)msg_ptr->payload));
        }
        else {
            // Ignore all other types of msg from client
            // Free msg ptr and payload
            if (msg_ptr->payload != NULL) {
                free(msg_ptr->payload);
            }
            if (msg_ptr != NULL) free(msg_ptr);
            continue;
        }
        // Whenever last log index >= netIndex for a follower, send AppendEntries PRC with log enetries starting at nextIndex,
        // Update nextIndex if successful
        // If AppendEntries fails because of log inconsistency, decrement nextIndex and retry
        for (int i = 0; i < SERVER_COUNT; i++) {
            if (i == get_context()->get_id()) continue;
            replica_msg_wrapper_t msg;
            msg.type = APP_ENTR_RPC;
            append_entry_rpc_t append_msg;
            append_msg.term = get_context()->get_curr_term();
            append_msg.leader_id = get_context()->get_id();
            append_msg.prev_log_term = prev_log_term;
            append_msg.prev_log_index = prev_log_index;
            append_msg.commit_index = get_context()->get_bc_log().get_committed_index();
            int next_index = nextIndex[i];
            std::vector<Block> entries;
            for (int j = next_index; j <= get_context()->get_bc_log().get_blockchain_length() - 1; j++) {
                entries.push_back(get_context()->get_bc_log().get_block_by_index(j));
            }
            append_msg.entries = entries;
            msg.payload = (void*) &append_msg;
             std::cout<<"[State::LeaderState::run] Sending appendEntryRPC!"<<std::endl;
            network->replica_send_message(msg, i);
        }
        int num_accept = 1;
        while (num_accept < SERVER_COUNT / 2 + 1) {
            if (network->replica_get_message_count() == 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(MSG_CHECK_SLEEP_MS));
                continue;
            }
            replica_msg_wrapper_t msg;
            network->replica_pop_message(msg);
            if (msg.type == REQ_VOTE_RPC) {
                 //std::cout<<"[State::LeaderState::run] Recv a requestVoteRPC!"<<std::endl;
                request_vote_rpc_t* vote_rpc = (request_vote_rpc_t*) msg.payload;
                if (vote_rpc->term > get_context()->get_curr_term()) {
                    // Step down
                    get_context()->set_state(new FollowerState(get_context()));
                    goto exit;
                }
            }
            else if (msg.type == APP_ENTR_RPC) {
                //std::cout<<"[State::LeaderState::run] Recv a appendEntryRPC!"<<std::endl;
                append_entry_rpc_t* append_rpc = (append_entry_rpc_t*) msg.payload;
                if (append_rpc->term > get_context()->get_curr_term()) {
                    // Step down
                    get_context()->set_state(new FollowerState(get_context()));
                    goto exit;
                }
            }
            else if (msg.type == APP_ENTR_RPL) {
                //std::cout<<"[State::LeaderState::run] Recv a requestVoteRPC Reply!"<<std::endl;
                append_entry_reply_t* reply = (append_entry_reply_t*) msg.payload;
                if (reply->term == get_context()->get_curr_term()) {
                    if (reply->success == true) {
                        // Append entry succeed
                        std::cout<<"[State::LeaderState::run] Append succeed!"<<std::endl;
                        num_accept++;
                        nextIndex[reply->sender_id] = get_context()->get_bc_log().get_blockchain_length() - 1;
                    }
                    else {
                        // Append failed due to log inconsistency, decrement nextIndex and retry
                        std::cout<<"[State::LeaderState::run] Append failde due to log inconsistency, Retry!"<<std::endl;
                        nextIndex[reply->sender_id]--;
                        replica_msg_wrapper_t msg;
                        msg.type = APP_ENTR_RPC;
                        append_entry_rpc_t append_msg;
                        append_msg.term = get_context()->get_curr_term();
                        append_msg.leader_id = get_context()->get_id();
                        append_msg.prev_log_term = prev_log_term;
                        append_msg.prev_log_index = prev_log_index;
                        append_msg.commit_index = get_context()->get_bc_log().get_committed_index();
                        int next_index = nextIndex[reply->sender_id];
                        std::vector<Block> entries;
                        for (int j = next_index; j <= get_context()->get_bc_log().get_blockchain_length() - 1; j++) {
                            entries.push_back(get_context()->get_bc_log().get_block_by_index(j));
                        }
                        append_msg.entries = entries;
                        msg.payload = (void*) &append_msg;
                        network->replica_send_message(msg, reply->sender_id);
                    }
                } 
            }
            else {
                // Leader should ignore all other type of message
            }
        }
        // Mark log committed if stored on a majority and at least one entry stored in the current term.
        // Execute the committed txn on balacne table, Also update committed index of the blockchain
        std::cout<<"[State::LeaderState::run] Enrty Committed, Update Balance Table!"<<std::endl;
        int curr_committed_index =  get_context()->get_bc_log().get_committed_index() + 1;
        get_context()->update_bal_tab_and_committed_index(curr_committed_index);
        // Reply to client
        response_t response;
        if (msg_ptr->type == TRANSACTION_REQUEST) {
            response.type = TRANSACTION_RESPONSE;
        }
        else if (msg_ptr->type == BALANCE_REQUEST) {
            response.type = BALANCE_RESPONSE;
        }
        response.request_id = msg_ptr->request_id;
        response.succeed = true;
        response.leader_id = get_context()->get_id();
        response.balance = get_context()->get_bal_tab().get_balance(msg_ptr->client_id);
        std::cout<<"[State::LeaderState::run] Reply to Client!"<<std::endl;
        network->client_send_message(response, msg_ptr->client_id);

        // Free msg ptr and payload
        if (msg_ptr->payload != NULL) {
            free(msg_ptr->payload);
        }
        if (msg_ptr != NULL) free(msg_ptr);
    }

exit:
    if (msg_ptr->payload != NULL) {
        free(msg_ptr->payload);
    }
    if (msg_ptr != NULL) free(msg_ptr);
    return; 
}
