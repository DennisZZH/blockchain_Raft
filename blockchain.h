#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <openssl/sha.h>

class transaction {
    public:
        transaction(uint32_t sid, uint32_t rid, float amt) : sender_id(sid), recver_id(rid), amount(amt) {};
        uint32_t get_sender_id() {return sender_id;};
        uint32_t get_recver_id() {return recver_id;};
        float get_amount() {return amount;};
        std::string serialize_transaction() {
            return std::to_string(sender_id) + std::to_string(recver_id) + std::to_string(amount);
        }
        void print_transaction() {std::cout<<"Print Transation: "<<"P"<<sender_id<<" send $"<<amount<<" To P"<<recver_id<<std::endl;}

    private:
        int sender_id;
        int recver_id;
        float amount;  
};

class block {
    public:
        block(uint32_t term_num, const std::vector<transaction> &txns) {
            term = term_num;
            if (txns.size() > 3) {
                std::cerr<<"Block's Maximum capacity exceed!"<<std::endl;
                exit(0);
            }
            trans = txns;
            phash = "";
            nonce = find_nonce();
        }

        uint32_t get_term() {return term;};
        std::string get_phash() {return phash;};
        std::string get_nonce() {return nonce;};
        std::vector<transaction> get_trans() {return trans;};

        void set_phash(std::string prev_hash) {phash = prev_hash;};

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
            for(int i = 0; i < trans.size(); i++){
                hashInfo += trans[i].serialize_transaction();
            }
            hashInfo += nonce;
            return sha256(hashInfo);
        }

        void print_block() {
            std::cout<<"Print Block: "<<std::endl;
            for(int i = 0; i < trans.size(); i++){
                std::cout<<"	";
                trans[i].print_transaction();
            }
	        std::cout<<"    term = "<<term<<"; phash = "<<phash<<"; nonce = "<<nonce<<"; chash = "<<find_hash()<<std::endl;
        }

    private:
        uint32_t term;          // The current term number
        std::string phash;      // The hash of previous block
        std::string nonce;      // The nonce of current block
        std::vector<transaction> trans;

        std::string find_nonce() {
            std::string txns_hash = "";
            std::string tempNounce;
            std::string hashInfo;
            srand(time(NULL));
            for(int i = 0; i < trans.size(); i++){
                txns_hash += trans[i].serialize_transaction();
            }
            do{
                tempNounce = std::string(1, char(rand()%26 + 97));
                hashInfo = sha256(txns_hash + tempNounce);
            }while( (*hashInfo.rbegin() != '0') && (*hashInfo.rbegin() != '1') && (*hashInfo.rbegin() != '2'));
            std::cout<<"Found nonce = "<<tempNounce<<"; hashInfo = "<<hashInfo<<std::endl;
            return tempNounce;
        }
};

class blockchain {
    public:
        blockchain() {
            
        }
        
        void add_block(block &newblo) {
            if (!blos.empty()) {
                newblo.set_phash(blos.back().find_hash());
            }
            blos.push_back(newblo);
        }
       
        void print_block_chain(){
            std::cout<<"Print Block Chain: "<<std::endl;
            for(auto &i : blos){
                std::cout<<"	";
                i.print_block();
            }
        }
    private:
        std::vector<block> blos;
};
