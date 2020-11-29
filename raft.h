#pragma once
#include <stdint.h>

#define ELECTION_TIMEOUT_MS     2000


typedef uint32_t term_t;

struct request_vote_rpc_t{
    term_t term;                    // candidate's term
    term_t last_log_term;           // the term of the last log
    int last_log_index;             // last log's index
};

struct request_vote_reply_t{
    term_t term;                    // the term of the replier
    int vote_granted;               // the candidate the replier voted for
};

struct append_entry_rpc_t{
    term_t term;                    // the leader's term
    int leader_id;                  // the leader's id
    term_t prev_log_term;           // the term of the log before the one to append
    int prev_log_index;             // the index of the log before the one to append
    int commit_index;               // the index of the commited last entry
    size_t entry_count;             // the number of entries commited
    // TODO: entries[];
};

struct append_entry_reply_t{
    term_t term;                    // the term of the replier
    bool success;                   // indicates wether the append is successful.
};              




