#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <thread>
#include <iomanip>
#include <sstream>
#include <openssl/sha.h>
#include "Msg.pb.h"

class Transaction {
    public:
        Transaction() {};
        Transaction(uint32_t sid, uint32_t rid, float amt) : sender_id(sid), recver_id(rid), amount(amt) {};
        uint32_t get_sender_id() {return sender_id;};
        uint32_t get_recver_id() {return recver_id;};
        float get_amount() {return amount;};
        std::string serialize_transaction() {
            return std::to_string(sender_id) + "-" + std::to_string(recver_id) + "-" + std::to_string(amount);
        }
        void print_transaction() {std::cout<<"Print Transation : "<<"Client"<<sender_id<<" send $"<<amount<<" To Client"<<recver_id<<std::endl;}

    private:
        uint32_t sender_id;
        uint32_t recver_id;
        float amount;  
};

class Block {
    public:
        Block(uint32_t term_num, Transaction &t) {
            term = term_num;
            txn = t;
            phash = "NULL";
            nonce = find_nonce();
        }

        void set_phash(std::string prev_hash) {phash = prev_hash;};
        uint32_t get_term() {return term;};
        std::string get_phash() {return phash;};
        std::string get_nonce() {return nonce;};
        Transaction get_txn() {return txn;};

        std::string sha256(const std::string str){
            unsigned char hash[SHA256_DIGEST_LENGTH];
            SHA256_CTX sha256;
            SHA256_Init(&sha256);
            SHA256_Update(&sha256, str.c_str(), str.size());
            SHA256_Final(hash, &sha256);
            std::stringstream ss;
            for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
            {
                ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
            }
            return ss.str();    
        }

        std::string find_hash(){
            std::string hashInfo = "";
            hashInfo += txn.serialize_transaction();
            hashInfo += nonce;
            return sha256(hashInfo);
        }

        void print_block() {
            std::cout<<"Print Block: "<<std::endl;
            txn.print_transaction();
	        std::cout<<"    term = "<<term<<"; phash = "<<phash<<"; nonce = "<<nonce<<"; current_hash = "<<find_hash()<<std::endl;
        }

    private:
        uint32_t term;          // The current term number
        std::string phash;      // The hash of previous block
        std::string nonce;      // The nonce of current block
        Transaction txn;

        std::string find_nonce() {
            std::string txns_hash = "";
            std::string tempNounce;
            std::string hashInfo;
            srand(time(NULL));
            txns_hash += txn.serialize_transaction();
            do{
                tempNounce = std::string(1, char(rand()%26 + 97));
                hashInfo = sha256(txns_hash + tempNounce);
            }while( (*hashInfo.rbegin() != '0') && (*hashInfo.rbegin() != '1') && (*hashInfo.rbegin() != '2'));
            std::cout<<"Found nonce = "<<tempNounce<<"; hashInfo = "<<hashInfo<<std::endl;
            return tempNounce;
        }
};

class Blockchain {
    public:
        /*
        *   Two cases that a server will need to construct and initialize its blockchain from a file (permanent storage on disk)
        *   1. When a server start and it initialize its blockchain from starter file
        *   2. When a server reboot from failure, it reboot its blockchain from stored file
        * 
        *   Precondition: Server need to provide the filename corresponding to its permanent storage on disk, file stores a serialized blockchain.
        *   Postcondistion: A blockchain is initialized on memory from the file. Internally, each block is constructed in a way that it is fully filled,
        *   ie. each block contains maximum capacity (3) transactions, except maybe the last block contains fewer than 3
        */
        
        Blockchain(std::string fname) {
            committed_index = 0;
            filename = fname;
            parse_file_to_bc();
        }

        void parse_file_to_bc() {
            std::ifstream infile;
            infile.open(filename);
            std::string bc_str, line;
            if (infile.is_open()) {
                while (getline (infile, line)){
                    bc_str += line;
                }
            }
            else {
                std::cerr << "Error: blockchain.h : parse_file_to_bc(): Unable to open file!" << std::endl;
                exit(0);
            }
            bc_msg_t bc_msg;
            bc_msg.ParseFromString(bc_str);
            committed_index = bc_msg.committed_index();
            // TODO
            // get blocks    

            infile.close();
        }

        void write_bc_to_file() {
            std::ofstream outfile;
            outfile.open(filename);

            bc_msg_t bc_msg;
            bc_msg.set_committed_index(committed_index);
            for (auto &b : blocks) {
                block_msg_t* block_msg_ptr = bc_msg.add_blocks();
                txn_msg_t* txn_msg_ptr = new txn_msg_t();
                txn_msg_ptr->set_sender_id(b.get_txn().get_sender_id());
                txn_msg_ptr->set_recver_id(b.get_txn().get_recver_id());
                txn_msg_ptr->set_amount(b.get_txn().get_amount());
                block_msg_ptr->set_allocated_txn(txn_msg_ptr);
                block_msg_ptr->set_term(b.get_term());
                block_msg_ptr->set_phash(b.get_phash());
                block_msg_ptr->set_nonce(b.get_nonce());
            }
            std::string bc_str = bc_msg.SerializeAsString();
            // TODO
            // Need to over write the existed file, write from the begining
            outfile << bc_str;

            outfile.close();
        }

        /*  
        *   Client send Server a single transction to add into blockchain
        *   Because finding nonce is trival, we consider each block contains only one real transaction and two NULLs.
        */
        void add_transaction(uint32_t term, Transaction new_txn) {
            Block newblo(term, {new_txn});
            if (!blocks.empty()) {
                newblo.set_phash(blocks.back().find_hash());
            }
            blocks.push_back(newblo);
            write_bc_to_file();
        }

        void clean_up(int index, std::vector<Block> ref) {
            // TODO
            // Leader clean up follower's logs
        }

        uint32_t get_blockchain_length() {return blocks.size();};
        uint32_t get_committed_index() {return committed_index;};
        Block* get_last_block() {return &blocks.back();};
       
        void print_block_chain(){
            std::cout<<"Print Block Chain: "<<std::endl;
            for(auto &i : blocks){
                std::cout<<"	";
                i.print_block();
            }
        }

    private:
        std::vector<Block> blocks;
        uint32_t committed_index;
        std::string filename;
};
