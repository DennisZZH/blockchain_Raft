#pragma once
#include <stdint.h>
#include "parameter.h"

// declarations
class Block;

typedef uint32_t term_t;

typedef enum {
    NONE,
    REQ_VOTE_RPC,               // request vote RPC
    REQ_VOTE_RPL,               // request vote reply
    APP_ENTR_RPC,               // append entry RPC
    APP_ENTR_RPL                // append entry reply
} replica_msg_type_t;

struct replica_msg_wrapper_t{
    replica_msg_type_t type;        // type determines which struct we should cast the message payload to.
    void* payload;                  // cast this one to one of the message struct.
};

struct request_vote_rpc_t{
    int candidate_id;               // the candidate's id who is requesting votes
    term_t term;                    // candidate's term
    term_t last_log_term;           // the term of the last log
    int last_log_index;             // last log's index
};

struct request_vote_reply_t{
    term_t term;                    // the term of the replier
    bool vote_granted;              // the flag indicates if candidate got the vote
};

struct append_entry_rpc_t{
    term_t term;                    // the leader's term
    int leader_id;                  // the leader's id
    term_t prev_log_term;           // the term of the log before the one to append
    int prev_log_index;             // the index of the log before the one to append
    int commit_index;               // the index of the commited last entry
    // size_t entry_count;             // the number of entries commited
    std::vector<Block> entries;
};

struct append_entry_reply_t{
    term_t term;                    // the term of the replier
    int sender_id;                  // who send the message
    bool success;                   // indicates wether the append is successful.
    bool reply_hearbeat;
};              




