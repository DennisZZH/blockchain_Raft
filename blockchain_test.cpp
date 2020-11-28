#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include "blockchain.cpp"

using namespace std;

int main() {

    transaction txn1(1,2,25.5);
    transaction txn2(2,3,60.9);
    transaction txn3(4,5,90.32);
    transaction txn4(4,2,110.0);
    transaction txn5(1,5,20.59);
    transaction txn6(3,4,290.0);
    transaction txn7(4,2,45.4);

    cout<<"Test: print_transaction(): "<<endl;
    txn1.print_transaction();
    txn2.print_transaction();
    txn3.print_transaction();
    txn4.print_transaction();
    txn5.print_transaction();
    txn6.print_transaction();
    txn7.print_transaction();
    cout<<endl;

    vector<transaction> l1,l2,l3;
    l1.push_back(txn1);
    l1.push_back(txn2);
    l1.push_back(txn3);

    l2.push_back(txn4);
    l2.push_back(txn5);

    l3.push_back(txn6);
    l3.push_back(txn7);


    block b1(0, l1);
    block b2(0, l2);
    block b3(0, l3);

    cout<<"Test: print_block(): "<<endl;
    b1.print_block();
    b2.print_block();
    b3.print_block();
    cout<<endl;

    blockchain bc;
    bc.add_block(b1);
    bc.add_block(b2);
    bc.add_block(b3);
    cout<<"Test: print_block_chain(): "<<endl;
    bc.print_block_chain();

    return 0;
}