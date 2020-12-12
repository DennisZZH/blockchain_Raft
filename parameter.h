#pragma once

// server client communication
#define COMM_HEADER_TYPE    uint32_t

// clients informations
#define CLIENT_COUNT        3
#define CLIENT_IP           "127.0.0.1"
#define CLIENT_BASE_PORT    8030

// servers information that the client connects to
#define SERVER_COUNT        3
#define SERVER_IP           "127.0.0.1"
#define SERVER_BASE_PORT    8020


// raft replica information
#define REPLICA_SERVER_BASE_PORT

#define NULL_CANDIDATE_ID -1

#define ELECTION_TIMEOUT_MS     500
#define HEARTBEAT_PERIOD_MS     100

// The length of digits of blockchain's committed index
// ie. digit len = 4 means committed index range from 0 to 9999
#define NUM_DIGITS_COMMITTED_INDEX 4
