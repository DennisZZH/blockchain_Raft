#pragma once
#include <stdint.h>

typedef enum {
    TRANSACTION_RESPONSE,
    BALANCE_RESPONSE,
    LEADER_CHANGE
} message_type_t;

struct response_t {
    message_type_t type;
    uint64_t request_id;
    bool succeed;
    uint32_t balance;
    uint32_t leader_id;
}