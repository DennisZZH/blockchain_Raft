#pragma once
#include <stdint.h>

typedef uint32_t term_t;

typedef struct {
    term_t term;                    // candidate's term
    term_t last_log_term;           // the term of the last log
    int last_log_index;             // last log's index
} request_vote_rpc_t;

typedef struct {
    term_t term;                    // the term of the replier
    int candidate_id;               // the candidate the replier voted for
} request_vote_reply_t;

typedef struct {
    term_t term;                    // the leader's term
    int leader_id;                  // the leader's id
    term_t prev_log_term;           // the term of the log before the one to append
    int prev_log_index;             // the index of the log before the one to append
    int commit_index;               // the index of the commited last entry
    size_t entry_count;             // the number of entries commited
    // TODO: entries[];
} append_entry_rpc_t;

typedef struct {
    term_t term;                    // the term of the replier
    bool success;                   // indicates wether the append is successful.
} append_entry_reply_t;              




