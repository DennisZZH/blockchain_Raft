#pragma once
// #define SERVER_COUNT 3
// #define CLIENT_COUNT 3
#define NULL_CANDIDATE_ID -1

#define ELECTION_TIMEOUT_MS     500
#define HEARTBEAT_PERIOD_MS     100

// The length of digits of blockchain's committed index
// ie. digit len = 4 means committed index range from 0 to 9999
#define NUM_DIGITS_COMMITTED_INDEX 4
