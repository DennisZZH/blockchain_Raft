#include "server.h"
#include "network.h"
#include "raft.h"
#include <thread>

const char* usage = "Run the program by typing ./main <server_id> where server_id is within range [0, 2].";
inline void print_usage() {
    printf("%s\n", usage);
}

void debug_run(int server_id) {
    // Network network = Network(cid);
    Server server(server_id);
    
    std::string input;
    while (true) {
        std::cout << "input command: ";
        
        input.clear();
        std::getline(std::cin, input);
        std::stringstream ss(input);
        std::vector<std::string> args;
        while (ss.good()) {
            std::string arg = "";
            ss >> arg;
            args.push_back(arg);
        }

        std::string &cmd = args[0];
        replica_msg_wrapper_t wrapper;
        if (cmd.compare("1") == 0) {
            wrapper.type = REQ_VOTE_RPC;
            request_vote_rpc_t rpc;
            rpc.candidate_id = 0;
            rpc.last_log_index = 1;
            rpc.last_log_term = 1;
            rpc.term = 1;
            wrapper.payload = &rpc;
            server.get_network()->replica_send_message(wrapper, 2);
        } else if (cmd.compare("2") == 0) {
            wrapper.type = REQ_VOTE_RPL;
            request_vote_reply_t rpl;
            rpl.term = 1;
            rpl.vote_granted = true;
            wrapper.payload = &rpl;
            server.get_network()->replica_send_message(wrapper, 1);
        } else if (cmd.compare("3") == 0) {
            wrapper.type = APP_ENTR_RPC;
            append_entry_rpc_t rpc;
            rpc.term = 2;
            rpc.prev_log_index = 3;
            rpc.prev_log_term = 1;
            rpc.leader_id = 2;
            rpc.commit_index = 1;
            auto t = Transaction(0, 2, 3.0);
            rpc.entries.push_back(Block(1, t));
            rpc.entries.push_back(Block(2, t));
            wrapper.payload = &rpc;
            server.get_network()->replica_send_message(wrapper, 0);
        } else if (cmd.compare("4") == 0) {
            wrapper.type = APP_ENTR_RPL;
            append_entry_reply_t rpl;
            rpl.sender_id = 1;
            rpl.success = true;
            rpl.term = 3;
            wrapper.payload = &rpl;
            server.get_network()->replica_send_message(wrapper, -1);
        } else if (cmd.compare("p") == 0) {
            // pop all messages
            while (server.get_network()->replica_get_message_count()) {
                replica_msg_wrapper_t wrapper;
                server.get_network()->replica_pop_message(wrapper);
                replica_msg_type_t type = wrapper.type;
                if (type == REQ_VOTE_RPC) {
                    auto msg = (request_vote_rpc_t*) wrapper.payload;
                    std::cout << "type: " << "REQ_VOTE_RPC" << std::endl;
                    std::cout << "term: " << msg->term << " candate id: " << msg->candidate_id << " last log term: " << msg->last_log_term << " last log index: " << msg->last_log_index << std::endl;
                } else if (type == REQ_VOTE_RPL) {
                    auto msg = (request_vote_reply_t*) wrapper.payload;
                    std::cout << "type: " << "REQ_VOTE_RPL" << std::endl;
                    std::cout << "term: " << msg->term << " granted: " << msg->vote_granted << std::endl;
                } else if (type == APP_ENTR_RPC) {
                    auto msg = (append_entry_rpc_t*) wrapper.payload;
                    std::cout << "type: " << "APP_ENTR_RPC" << std::endl;
                    std::cout << "term: " << msg->term << " leader id: " << msg->leader_id << " last log index: " << msg->prev_log_index << " prev log term: " << msg->prev_log_term << " commit index: " << msg->commit_index << std::endl;
                    std::cout << "entry count: " << msg->entries.size() << std::endl;
                    for (int i = 0; i < msg->entries.size(); i++) {
                        auto &block = msg->entries.at(i);
                        block.print_block();
                    }
                } else if (type == APP_ENTR_RPL) {
                    auto msg = (append_entry_reply_t*) wrapper.payload;
                    std::cout << "type: " << "APP_ENTR_RPL" << std::endl;
                    std::cout << "term: " << msg->term << " success: " << msg->success << " sender id: " << msg->sender_id << std::endl;
                }
                std::cout << "------------------------------------------------" << std::endl;
            }
        } else if (cmd.compare("leader") == 0) {
            response_t response;
            response.type = LEADER_CHANGE;
            response.succeed = true;
            response.leader_id = 1;
            server.get_network()->client_send_message(response);
        } else if (cmd.compare("trans_rpl") == 0) {
            response_t response;
            response.type = TRANSACTION_RESPONSE;
            response.succeed = true;
            response.request_id = 1;
            server.get_network()->client_send_message(response);
        } else if (cmd.compare("balance_rpl") == 0) {
            response_t response;
            response.type = BALANCE_RESPONSE;
            response.succeed = true;
            response.balance = 20;
            server.get_network()->client_send_message(response);
        } else if (cmd.compare("false_rpl") == 0) {
            response_t response;
            response.type = TRANSACTION_REQUEST;
            server.get_network()->client_send_message(response);
        }
    }
}


void normal_run(Server &s) {
    s.run_state_machine();
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        print_usage();
        exit(1);
    }
    
    int server_id = atoi(argv[1]);
    if (server_id < 0 || server_id >= SERVER_COUNT) {
        std::cout << "Your input cid is out of the accepted range." << std::endl;
        print_usage();
        exit(1);
    }

    // Spawn a thread to run server state machine
    Server server(server_id);
    std::thread server_thread(normal_run, server);

    // Main thread provide main UI interface
    std::string input;
    while (true) {
        std::cout << "input command: ";
        
        input.clear();
        std::getline(std::cin, input);
        std::stringstream ss(input);
        std::vector<std::string> args;
        while (ss.good()) {
            std::string arg = "";
            ss >> arg;
            args.push_back(arg);
        }

        std::string &cmd = args[0];
        if (cmd.compare("p")) {
            server.print_info();
        }
    }
}
