#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include "blockchain.h"

using namespace std;

int main() {

    Transaction txn1(1,2,25.5);
    Transaction txn2(2,3,60.9);
    Transaction txn3(4,5,90.32);
    Transaction txn4(4,2,110.0);
    Transaction txn5(1,5,20.59);
    Transaction txn6(3,4,290.0);
    Transaction txn7(4,2,45.4);

    cout<<"Test: print_transaction(): "<<endl;
    txn1.print_transaction();
    txn2.print_transaction();
    txn3.print_transaction();
    txn4.print_transaction();
    txn5.print_transaction();
    txn6.print_transaction();
    txn7.print_transaction();
    cout<<endl;

    vector<Transaction> l1,l2,l3;
    l1.push_back(txn1);
    l1.push_back(txn2);
    l1.push_back(txn3);

    l2.push_back(txn4);
    l2.push_back(txn5);

    l3.push_back(txn6);
    l3.push_back(txn7);


    Block b1(0, l1);
    Block b2(0, l2);
    Block b3(0, l3);

    cout<<"Test: print_block(): "<<endl;
    b1.print_block();
    b2.print_block();
    b3.print_block();
    cout<<endl;

    // TODO
    cout<<"Test: print_block_chain(): "<<endl;
    

    return 0;
}