#include <iostream>
#include <cstdlib>
#include "blockchain.h"
#include "balance_table.h"
#include "Msg.pb.h"

using namespace std;

int main() {
    // This is a initial txn
    Transaction t(true);
    Block b(0, t);
    block_msg_t block_msg;
    txn_msg_t* txn_msg_ptr = new txn_msg_t();
    txn_msg_ptr->set_sender_id(b.get_txn().get_sender_id());
    txn_msg_ptr->set_recver_id(b.get_txn().get_recver_id());
    txn_msg_ptr->set_amount(b.get_txn().get_amount());
    txn_msg_ptr->set_bal_txn_flag(b.get_txn().get_bal_txn_flag());
    block_msg.set_allocated_txn(txn_msg_ptr);
    block_msg.set_term(b.get_term());
    block_msg.set_phash(b.get_phash());
    block_msg.set_nonce(b.get_nonce());
    block_msg.set_index(b.get_index());
    std::string block_str = block_msg.SerializeAsString();

    // Create an bc_file_0 on disk
    string filename0a = "bc_file_0.txt";
    std::ofstream outfile0a(filename0a);
    outfile0a << "0000" << endl;
    // Write initial txn to bc
    outfile0a << block_str << endl;
    outfile0a.close();
    // Create a bal_tab_file_1 on disk
    string filename0b = "bal_tab_0.txt";
    std::ofstream outfile0b(filename0b);
    outfile0b << "10 10 10 " << endl;
    outfile0b.close();


     // Create an bc_file_1 on disk
    string filename1a = "bc_file_1.txt";
    std::ofstream outfile1a(filename1a);
    outfile1a << "0000" << endl;
    // Write initial txn to bc
    outfile1a << block_str << endl;
    outfile1a.close();
     // Create a bal_tab_file_1 on disk
    string filename1b = "bal_tab_1.txt";
    std::ofstream outfile1b(filename1b);
    outfile1b << "10 10 10 " << endl;
    outfile1b.close();

    // Create an bc_file_2 on disk
    string filename2a = "bc_file_2.txt";
    std::ofstream outfile2a(filename2a);
    outfile2a << "0000" << endl;
    // Write initial txn to bc
    outfile2a << block_str << endl;
    outfile2a.close();
     // Create a bal_tab_file_2 on disk
    string filename2b = "bal_tab_2.txt";
    std::ofstream outfile2b(filename2b);
    outfile2b << "10 10 10 " << endl;
    outfile2b.close();

}