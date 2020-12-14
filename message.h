/**
 * @file message.h
 * @brief shared by clients and servers
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#pragma once
#include <stdint.h>
#include "Msg.pb.h"

typedef enum {
    TRANSACTION_REQUEST,
    BALANCE_REQUEST,
    TRANSACTION_RESPONSE,
    BALANCE_RESPONSE,
    LEADER_CHANGE
} message_type_t;

struct response_t {
    message_type_t type;
    uint64_t request_id;
    bool succeed;
    float balance;
    uint32_t leader_id;
};

struct request_t {
    message_type_t type;
    uint32_t client_id;
    uint64_t request_id;
    void* payload;
};