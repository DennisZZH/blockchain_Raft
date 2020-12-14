#include <iostream>
#include <cstdlib>
#include "blockchain.h"
#include "balance_table.h"
#include "Msg.pb.h"

using namespace std;

int main() {
    // Create an bc_file_1 on disk
    string filename1a = "bc_file_1.txt";
    std::ofstream outfile1a(filename1a);
    outfile1a << "0000" << endl;

    Transaction t1(0,1,13.14);
    Block b1(1, t1);
    block_msg_t block_msg1;
    txn_msg_t* txn_msg_ptr1 = new txn_msg_t();
    txn_msg_ptr1->set_sender_id(b1.get_txn().get_sender_id());
    txn_msg_ptr1->set_recver_id(b1.get_txn().get_recver_id());
    txn_msg_ptr1->set_amount(b1.get_txn().get_amount());
    txn_msg_ptr1->set_bal_txn_flag(b1.get_txn().get_bal_txn_flag());
    block_msg1.set_allocated_txn(txn_msg_ptr1);
    block_msg1.set_term(b1.get_term());
    block_msg1.set_phash(b1.get_phash());
    block_msg1.set_nonce(b1.get_nonce());
    block_msg1.set_index(b1.get_index());
    std::string block_str1 = block_msg1.SerializeAsString();
    outfile1a << block_str1 << endl;

    outfile1a.close();

    // Create a bal_tab_file_1 on disk
    string filename1b = "bal_tab_1.txt";
    std::ofstream outfile1b(filename1b);
    outfile1b << "10 10 10 " << endl;
    outfile1b.close();
}