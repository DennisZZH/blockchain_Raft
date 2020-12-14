#pragma once

// general network parameters
#define MESH_NETWORK_DELAY_MS   2000

// server client communication
#define COMM_HEADER_TYPE        uint32_t

// clients information
// note: the way to calculate client id on server:
// client id = (client_port - server_id - CLIENT_BASE_PORT) / CLIENT_PORT_MULT
#define CLIENT_COUNT            3
#define CLIENT_IP               "127.0.0.1"
#define CLIENT_BASE_PORT        11000
#define CLIENT_PORT_MULT        10
#define CLIENT_REQ_TIMEOUT_MS   5000

// servers information that the client connects to
#define SERVER_COUNT            3
#define SERVER_IP               "127.0.0.1"
#define SERVER_BASE_PORT        8020

// network simulator information
#define MESH_IP                 "127.0.0.1"
#define MESH_PORT               9000

// raft replica information
// replica sites bind to the following ip and port when connecting to the mesh
#define REPLICA_CLIENT_IP              "127.0.0.1"
#define REPLICA_CLIENT_BASE_PORT       8900

#define NULL_CANDIDATE_ID -1

#define ELECTION_TIMEOUT_MS     5000
#define HEARTBEAT_PERIOD_MS     1000

// The length of digits of blockchain's committed index
// ie. digit len = 4 means committed index range from 0 to 9999
#define NUM_DIGITS_COMMITTED_INDEX 4
