#pragma once
#include <deque>
#include <thread>
#include "Msg.pb.h"
#include "parameter.h"

namespace RaftMesh {
    struct server_info_t {
        bool partitioned;
        bool connected;
        int sock;
        std::thread *recv_task;
        std::thread *send_task;
        std::deque<raft_msg_t*> trans_queue;
    };

    class Mesh {
    public:
        Mesh();
        ~Mesh();
        int mesh_sock = 0;
        bool is_stopped = false;

    private:
        std::thread wait_conn_thread;
        server_info_t servers[SERVER_COUNT];

        void setup_mesh_server();
        void wait_conn_handler();
        void recv_handler(int replica_id);
        void send_handler(int replica_id);
    };
}