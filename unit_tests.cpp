#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include "blockchain.h"
#include "balance_table.h"
#include "Msg.pb.h"

using namespace std;

void run_test_bc() {

    // Test load file, parse_file_to_bc
    Blockchain bc1;
    bc1.load_file("bc_file_1.txt");
    bc1.print_block_chain();

    // Test add transaction, write_block_to_file
    Transaction t2(true);
    bc1.add_transaction(2, t2);
    Transaction t3(1,2,9.96);
    bc1.add_transaction(1, t3);

    // Test set_committed_index
    bc1.set_committed_index(2);

    // Test clean_up_blocks, write_bc_to_file
    bc1.clean_up_blocks(2, {});

    // Test get_block_by_index, get_last_block
    std::cout << "Block at index 0 : " << endl;
    bc1.get_block_by_index(0).print_block();
    std::cout << "Block at tail : " << endl;
    bc1.get_last_block().print_block();

    // Additional test
    Blockchain bc1b;
    bc1b.load_file("bc_file_1.txt");
    bc1b.print_block_chain();

}

void run_test_bal_tab() {
    
    // Test load_file, print_bal_tab
    BalanceTable bt1;
    bt1.load_file("bal_tab_1.txt");
    bt1.print_bal_tab();

    // Test set_balance
    bt1.set_balance(0, 11);
    bt1.print_bal_tab();

    // Test update_balance
    bt1.update_balance(0,1,1);
    bt1.print_bal_tab();

    // Test write_bal_tab_to_file
    BalanceTable bt1b;
    bt1b.load_file("bal_tab_1.txt");
    bt1b.print_bal_tab();
}

int main() {

    run_test_bc();
    run_test_bal_tab();

    return 0;
}