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

class Transaction {
    public:
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
        Block(uint32_t term_num, std::vector<Transaction> v) {
            term = term_num;
            txns = v;
            phash = "NULL";
            nonce = find_nonce();
        }

        void set_phash(std::string prev_hash) {phash = prev_hash;};
        uint32_t get_term() {return term;};
        std::string get_phash() {return phash;};
        std::string get_nonce() {return nonce;};
        std::vector<Transaction> get_txns() {return txns;};

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
            for(int i = 0; i < txns.size(); i++){
                hashInfo += txns[i].serialize_transaction();
            }
            hashInfo += nonce;
            return sha256(hashInfo);
        }

        void print_block() {
            std::cout<<"Print Block: "<<std::endl;
            for(int i = 0; i < txns.size(); i++){
                std::cout<<"	";
                txns[i].print_transaction();
            }
	        std::cout<<"    term = "<<term<<"; phash = "<<phash<<"; nonce = "<<nonce<<"; current_hash = "<<find_hash()<<std::endl;
        }

    private:
        uint32_t term;          // The current term number
        std::string phash;      // The hash of previous block
        std::string nonce;      // The nonce of current block
        std::vector<Transaction> txns;

        std::string find_nonce() {
            std::string txns_hash = "";
            std::string tempNounce;
            std::string hashInfo;
            srand(time(NULL));
            for(int i = 0; i < txns.size(); i++){
                txns_hash += txns[i].serialize_transaction();
            }
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
        Blockchain(std::string s) {
            committed_index = 0;
            filename = s;
            parse_file_to_bc();
        }

        // Helper function
        void parse_file_to_bc() {
            std::ifstream infile;
            infile.open(filename);
            // TODO
            // Deserialize the file into a blockchain
            infile.close();
        }

        void write_bc_to_file() {
            std::ofstream outfile;
            outfile.open(filename);
            // TODO
            // Serialize the bc into the file
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
