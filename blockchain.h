#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>
#include <iomanip>
#include <sstream>
#include <openssl/sha.h>
#include "Msg.pb.h"
#include "raft.h"

class Transaction {
    public:
        Transaction() {}
        Transaction(uint32_t sid, uint32_t rid, float amt) : sender_id(sid), recver_id(rid), amount(amt) {}
        Transaction(bool flag) {bal_txn_flag = true;}

        void set_sender_id(uint32_t sid) {sender_id = sid;}
        void set_recver_id(uint32_t rid) {recver_id = rid;}
        void set_amount(float amt) {amount = amt;}
        uint32_t get_sender_id() {return sender_id;}
        uint32_t get_recver_id() {return recver_id;}
        float get_amount() {return amount;}

        std::string serialize_transaction() {
            return std::to_string(sender_id) + "-" + std::to_string(recver_id) + "-" + std::to_string(amount);
        }
        void print_transaction() {std::cout<<"Print Transation : "<<"Client"<<sender_id<<" send $"<<amount<<" To Client"<<recver_id<<std::endl;}

    private:
        uint32_t sender_id = -1;
        uint32_t recver_id = -1;
        float amount = -1;
        bool bal_txn_flag = false;
};

class Block {
    public:
        Block() {}

        Block(uint32_t term_num, Transaction &t) {
            // These fields could be set when block being constructed
            term = term_num;
            txn = t;
            nonce = find_nonce();
            // These fields need to be set after this block is added to blockchain
            phash = "NULL";
            index = 0;
        }

        void set_term(uint32_t t) {term = t;}
        void set_nonce(std::string n) {nonce = n;}
        void set_txn(Transaction &T) {txn = T;}
        void set_phash(std::string h) {phash = h;}
        void set_index(uint32_t i) {index = i;}

        uint32_t get_term() {return term;}
        std::string get_phash() {return phash;}
        std::string get_nonce() {return nonce;}
        Transaction& get_txn() {return txn;}
        uint32_t get_index() {return index;}
    
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
            std::cout << "Print Block: " << std::endl;
            txn.print_transaction();
	        std::cout << "    term = " << term << "; phash = " << phash << "; nonce = " 
                << nonce << "; current_hash = " << find_hash() << "; index = " << index << std::endl;
        }

    private:
        term_t term;          // The current term number
        std::string phash;      // The hash of previous block
        std::string nonce;      // The nonce of current block
        Transaction txn;        // A single transation
        uint32_t index;

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
        *   1. When a server start and it initialize its blockchain from starter file.
        *   2. When a server reboot from failure, it reboot its blockchain from stored file.
        * 
        *   Precondition: Server need to provide the filename corresponding to its permanent storage on disk. That file starts with the committed index on the first line, then a serialized block on each line.
        *   Postcondistion: A blockchain is initialized on memory from the file. Internally, each block always only contain 1 transaction.
        */
        
        Blockchain() {
            committed_index = -1;
        }

        void load_file(std::string fname) {
            filename = fname;
            parse_file_to_bc();
        }

        void parse_file_to_bc() {
            std::ifstream infile;
            infile.open(filename);

            std::string block_str, line;
            if (infile.is_open()) {

                getline(infile, line);
                int index_val = 0;
                for (int i = 0; i < line.length(); i++) {
                    index_val *= 10;
                    index_val += line[i] - '0';
                }
                committed_index = index_val;

                while (getline (infile, line)){
                    block_str = line;
                    block_msg_t block_msg;
                    block_msg.ParseFromString(block_str);
                    Block blo;
                    Transaction txn;
                    txn.set_sender_id(block_msg.txn().sender_id());
                    txn.set_recver_id(block_msg.txn().recver_id());
                    txn.set_amount(block_msg.txn().amount());
                    blo.set_txn(txn);
                    blo.set_phash(block_msg.phash());
                    blo.set_nonce(block_msg.nonce());
                    blo.set_term(block_msg.term());
                    blo.set_index(block_msg.index());
                    blocks.push_back(blo);
                }
            }
            else {
                std::cerr << "Error: blockchain.h : parse_file_to_bc(): Unable to open file!" << std::endl;
                exit(0);
            }
          
            infile.close();
        }

        void write_block_to_file(Block &newblo) {
            std::ofstream outfile(filename, std::ios::app);

            block_msg_t block_msg;
            txn_msg_t* txn_msg_ptr = new txn_msg_t();
            txn_msg_ptr->set_sender_id(newblo.get_txn().get_sender_id());
            txn_msg_ptr->set_recver_id(newblo.get_txn().get_recver_id());
            txn_msg_ptr->set_amount(newblo.get_txn().get_amount());
            block_msg.set_allocated_txn(txn_msg_ptr);
            block_msg.set_term(newblo.get_term());
            block_msg.set_phash(newblo.get_phash());
            block_msg.set_nonce(newblo.get_nonce());
            block_msg.set_index(newblo.get_index());
            
            std::string block_str = block_msg.SerializeAsString();
        
            outfile << block_str << std::endl;

            outfile.close();
        }

        /*  
        *   Client send Server a single transction to add into blockchain.
        *   Because finding nonce is trival, we consider each block contains only one real transaction and two NULLs.
        */
        void add_transaction(uint32_t term, Transaction new_txn) {
            Block newblo(term, {new_txn});
            if (!blocks.empty()) {
                newblo.set_phash(blocks.back().find_hash());
            }
            newblo.set_index(blocks.size());
            blocks.push_back(newblo);
            write_block_to_file(newblo);
        }

        void set_committed_index(uint32_t new_index) {
            committed_index = new_index;
            // Update the committed index on disk
            std::string index_str = "";
            for (int i = 0; i < NUM_DIGITS_COMMITTED_INDEX; i++) {
                int r = new_index % 10;
                new_index /= 10;
                index_str = index_str + std::to_string(r);
            }
            FILE *pFile = fopen(filename.c_str(), "r+");
            fseek(pFile, 0, SEEK_SET);
            fputs(index_str.c_str(), pFile);
            fclose(pFile);
        }

        void clean_up_blocks(int index, std::vector<Block> ref) {
            // Leader clean up follower's logs up to index
            // Replace it with ref (In the case that no cleaning required, just append the new entries)
            for (int i = blocks.size() - 1; i >= index; i--) {
                blocks.pop_back();
            }
            for (auto &b : ref) blocks.push_back(b);
            write_bc_to_file();
        }

        void write_bc_to_file() {
            std::ofstream outfile(filename, std::ios::trunc);
            
            int idx = committed_index;
            std::string index_str = "";
            for (int i = 0; i < NUM_DIGITS_COMMITTED_INDEX; i++) {
                int r = idx % 10;
                idx /= 10;
                index_str = index_str + std::to_string(r);
            }
            outfile << index_str << std::endl;
            
            outfile.close();

            for (auto& b : blocks) {
                write_block_to_file(b);
            }

        }

        Block& get_block_by_index(uint32_t index) {
            if (index < 0 || index >= blocks.size()) {
                std::cerr << "Error: Blockchain: get_block_by_index: invalid index!" << std::endl;
            }
            return blocks[index];
        }
        
        uint32_t get_blockchain_length() {return blocks.size();};
        uint32_t get_committed_index() {return committed_index;};
        Block& get_last_block() {return blocks.back();};
       
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
