#pragma once
#include <deque>
#include <thread>
#include <mutex>
#include <chrono>
#include "Msg.pb.h"
#include "parameter.h"

namespace RaftMesh {
    typedef std::chrono::system_clock clock_t;
    typedef std::chrono::milliseconds milliseconds_t;

    struct trans_queue_item_t {
        clock_t::time_point enqueue_time;
        replica_msg_t* msg;
    };

    struct server_info_t {
        bool partitioned;
        bool connected;
        int sock;
        std::thread *recv_task;
        std::thread *send_task;
        std::mutex trans_queue_lock;
        std::deque<trans_queue_item_t*> trans_queue;
    };

    class Mesh {
    public:
        Mesh();
        ~Mesh();
        int mesh_sock = 0;
        bool is_stopped = false;

        void server_partition_toggle(uint32_t server_id);

    private:
        std::thread wait_conn_thread;
        server_info_t servers[SERVER_COUNT] = {0};
        
        void setup_mesh_server();

        void append_server_trans_queue(int replica_id, trans_queue_item_t* trans_item);
        trans_queue_item_t* pop_server_trans_queue(int replica_id);
        void flush_server_trans_queue(int replica_id);
        // void flush_server_recv_queue(int replica_id);

        void wait_conn_handler();
        void recv_handler(int replica_id);
        void send_handler(int replica_id);
    };
}