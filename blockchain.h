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
        void set_flag(bool flag) {bal_txn_flag = flag;}
        uint32_t get_sender_id() {return sender_id;}
        uint32_t get_recver_id() {return recver_id;}
        float get_amount() {return amount;}
        bool get_bal_txn_flag() {return bal_txn_flag;}

        // review: why is this needed
        std::string serialize_transaction() {
            return std::to_string(sender_id) + "-" + std::to_string(recver_id) + "-" + std::to_string(amount);
        }

        void print_transaction() {
            std::string flag_str;
            if (bal_txn_flag) flag_str = "Balance";
            else flag_str = "Transfer";
            std::cout<< flag_str << " Transation : " << "Client " << sender_id << " send $" << amount << " To Client " << recver_id << std::endl;
        }

    private:
        uint32_t sender_id = 0;
        uint32_t recver_id = 0;
        float amount = 0;
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
            index = -1;
        }

        void set_term(uint32_t t) {term = t;}
        void set_nonce(std::string n) {nonce = n;}
        void set_txn(Transaction &T) {txn = T;}
        void set_phash(std::string h) {phash = h;}
        void set_index(int i) {index = i;}

        uint32_t get_term() {return term;}
        std::string get_phash() {return phash;}
        std::string get_nonce() {return nonce;}
        Transaction& get_txn() {return txn;}
        int get_index() {return index;}
    
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
            std::cout << "    ";
            txn.print_transaction();
	        std::cout << "    term = " << term << "; phash = " << phash << "; nonce = " 
                << nonce << "; current_hash = " << find_hash() << "; index = " << index << std::endl;
        }

    private:
        term_t term;          // The current term number
        std::string phash;      // The hash of previous block
        std::string nonce;      // The nonce of current block
        Transaction txn;        // A single transation
        int index;

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
            //std::cout<<"Found nonce = "<<tempNounce<<"; hashInfo = "<<hashInfo<<std::endl;
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

        /*
        *   note: blockchain saved format     
        *   line# 1: commited index value (total length BACKUP_COMMIX_INDEX_RESERVE_BYTES)
        *   line# 2: blockchain transaction 1
        *   line# 3: blockchain transaction 2
        *   line# n: ...
        * 
        *   note: each transaction has a flag to indicate either it's a balance or blockchain
        *   [4 + 2] =  [-/+][xxxx][\n][\0]
        */
        
        void load_file(std::string fname) {
            filename = fname;
            std::ifstream file(filename);
            // check if the path is existed
            // if not, need to create the file first.
            if(!file.good()) {
                std::fstream fs;
                std::cout << "[blockchain::load_file] couldn't find backup file, initialize it." << std::endl;
                fs.open(filename.c_str(), std::ios::out | std::ios::app);
                // construct the first line 
                int bytes = NUM_DIGITS_COMMITTED_INDEX + 2;
                char committed_index_str[NUM_DIGITS_COMMITTED_INDEX + 2];
                std::fill_n(committed_index_str, bytes, '0');
                committed_index_str[0] = '-';
                committed_index_str[NUM_DIGITS_COMMITTED_INDEX] = '1';
                committed_index_str[NUM_DIGITS_COMMITTED_INDEX + 1] = '\n';
                // committed_index_str[NUM_DIGITS_COMMITTED_INDEX + 2] = '\0';
                fs.write(committed_index_str, sizeof(committed_index_str));
                fs.close();
            } else {
                file.close();
            }
            parse_file_to_bc();
        }

        void parse_file_to_bc() {
            std::ifstream infile;
            infile.open(filename);

            std::string line;
            if (infile.is_open()) {
                getline(infile, line);
                std::cout << "line:" << line << std::endl;
                try {
                    committed_index = std::stoi(line);
                } catch(...) {
                    std::cout << "[blockchain::parse_file_to_bc] cannot convert line: " << line << std::endl;
                    exit(1);
                }

                // note: read the rest of the lines and parse each of them to transactions
                while (getline (infile, line)){
                    block_msg_t block_msg;
                    block_msg.ParseFromString(line);
                    Block blo;
                    Transaction txn;
                    txn.set_sender_id(block_msg.txn().sender_id());
                    txn.set_recver_id(block_msg.txn().recver_id());
                    txn.set_amount(block_msg.txn().amount());
                    txn.set_flag(block_msg.txn().bal_txn_flag());
                    blo.set_txn(txn);
                    blo.set_phash(block_msg.phash());
                    blo.set_nonce(block_msg.nonce());
                    blo.set_term(block_msg.term());
                    blo.set_index(block_msg.index());
                    blocks.push_back(blo);
                }
            } else {
                std::cerr << "[blockchain::parse_file_to_bc] unable to open file." << std::endl;
                exit(0);
            }
          
            infile.close();
        }

        void write_block_to_file(Block &newblo) {
            std::ofstream outfile(filename, std::ios::app);

            block_msg_t block_msg;
            txn_msg_t* txn_msg_ptr = new txn_msg_t();
            
            // because each block just have one transaction
            // so just parse the transaction and add to the block
            txn_msg_ptr->set_sender_id(newblo.get_txn().get_sender_id());
            txn_msg_ptr->set_recver_id(newblo.get_txn().get_recver_id());
            txn_msg_ptr->set_amount(newblo.get_txn().get_amount());
            txn_msg_ptr->set_bal_txn_flag(newblo.get_txn().get_bal_txn_flag());

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

        void set_committed_index(int index) {
            // Update the committed index on disk
            if (index < -1) {
                std::cerr << "[blockchain::set_committed_index] invalid index number. input index value: " << index << std::endl;
                return;
            }
            
            std::string line;
            std::fstream file;
            file.open(filename, std::ios::app);
            // move cursor to 0 and try to read the first line first
            // to make sure the file is not corrupted.
            file.seekp(0);
            getline(file, line);
            int curr_committed_index;
            try {
                curr_committed_index = std::stoi(line);
            } catch (...) {
                std::cerr << "[blockchain::set_committed_index] failed to read the current commited index. ";
                std::cerr << "file corrupted." << std::endl;
                file.close();
                exit(1);
            }
            if (index < curr_committed_index) {
                std::cerr << "[blockchain::set_committed_index] trying to commit a index that is smaller than the current committed index." << std::endl;
                file.close();
                return;
            }
            
            this->committed_index = index;
            // when saving to the file, will take 
            // allocate BACKUP_COMMIX_INDEX_RESERVE_BYTES + 2 bytes, 1 extra for sign, 1 extra for \n.
            char committed_index_str[NUM_DIGITS_COMMITTED_INDEX + 2] = {'0'};
            if (index < 0) {
                committed_index_str[0] = '-';
            } else {
                committed_index_str[0] = '+';
            }
            std::string index_string = std::to_string(abs(index));
            if (index_string.size() > NUM_DIGITS_COMMITTED_INDEX) {
                std::cerr << "[blockchain::set_committed_index] committed index has too many digits. digits count: " << index_string.size() << std::endl;
                file.close();
                return;
            }
            int first_char_offset = 1 + NUM_DIGITS_COMMITTED_INDEX - index_string.size();
            strcpy(committed_index_str + first_char_offset, index_string.c_str());
            committed_index_str[NUM_DIGITS_COMMITTED_INDEX + 1] = '\n';

            file.seekp(0);
            file.write(committed_index_str, sizeof(committed_index_str));
            file.close();
        }

        /**
         * @brief the erase will include the block specified by the index.
         * 
         * @param index 
         * @param ref 
         */
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
            
            // int idx = committed_index;
            // std::string index_str = "";
            // for (int i = 0; i < NUM_DIGITS_COMMITTED_INDEX; i++) {
            //     int r = idx % 10;
            //     idx /= 10;
            //     index_str = std::to_string(r) + index_str;
            // }
            // outfile << index_str << std::endl;
            int index = committed_index;
            char committed_index_str[NUM_DIGITS_COMMITTED_INDEX + 2] = {'0'};
            if (index < 0) {
                committed_index_str[0] = '-';
            } else {
                committed_index_str[0] = '+';
            }

            std::string index_string = std::to_string(abs(index));
            if (index_string.size() > NUM_DIGITS_COMMITTED_INDEX) {
                std::cerr << "[blockchain::set_committed_index] committed index has too many digits. digits count: " << index_string.size() << std::endl;
                outfile.close();
                exit(1);
            }

            int first_char_offset = 1 + NUM_DIGITS_COMMITTED_INDEX - index_string.size();
            strcpy(committed_index_str + first_char_offset, index_string.c_str());
            committed_index_str[NUM_DIGITS_COMMITTED_INDEX + 1] = '\n';
            
            outfile.close();

            for (auto& b : blocks) {
                write_block_to_file(b);
            }

        }

        Block& get_block_by_index(int index) {
            if (index < 0 || index >= blocks.size()) {
                std::cerr << "[blockchain::get_block_by_index] error: invalid index!" << std::endl;
                exit(0);
            }
            return blocks[index];
        }
        
        uint32_t get_blockchain_length() {return blocks.size();};
        int get_committed_index() {return committed_index;};
       
        term_t get_last_term() {
            if (blocks.size() == 0) {
                return 0;
            }
            return blocks.back().get_term();
        }

        int get_last_index() {
            if (blocks.size() == 0) {
                return -1;
            }
            return blocks.back().get_index();
        }
       
        void print_block_chain(){
            std::cout << "Print Block Chain: " << std::endl;
            std::cout << "    committed index = " << committed_index << std::endl;
            for(auto &i : blocks){
                std::cout<<"    ";
                i.print_block();
            }
        }

    private:
        std::vector<Block> blocks;
        int committed_index;
        std::string filename;
};
