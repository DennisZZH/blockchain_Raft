#pragma once

class Client {
private:
    int leader_id;              // estimate leader id

public:
    void connect_server();      
    void send_transaction();    
    void send_balance();
};