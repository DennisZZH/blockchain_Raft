#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Msg.pb.h"
#include "parameter.h"

class BalanceTable {
    /**
     * note: balance table file format
     * line #1: amount_client_0 amount_client_1 amount_client_2 <- note: there is a space at the end.
     */
    public:
        // BalanceTable () {}
        
        void load_file(std::string fname) {
            filename = fname;
            
            std::ifstream file(filename);
            // check if the open is failed (file doesn't exist)
            // if failed, initialize the balance table by giving all clients 10.0
            if (!file.good()) {
                std::fstream fs;
                fs.open(filename, std::ios::out | std::ios::app);
                std::stringstream ss;
                for (int i = 0; i < CLIENT_COUNT; i++) {
                    ss << "10.0 ";
                }
                fs.write(ss.str().c_str(), ss.str().size());
                fs.close();
            }

            if (!file.is_open()) {
                file.open(filename);
            }

            std::string line;
            if (file.is_open()) {
                getline(file, line);
                std::stringstream ss(line);
                std::vector<std::string> clients_balance_strs;
                while (ss.good()) {
                    std::string amount_str = "";
                    ss >> amount_str;
                    clients_balance_strs.push_back(amount_str);
                }
                
                for (int client_id = 0; client_id < clients_balance_strs.size(); client_id++) {
                    float amount;
                    try {
                        amount = stof(clients_balance_strs.at(client_id));
                    } catch(...) {
                        // std::cout << "[balancetable::load_file] failed to convert. check file text." << std::endl;
                        // exit(1);
                        continue;
                    }
                    bal_tab[client_id] = amount; 
                }
            }
            else {
                std::cerr << "[BalanceTable::load_file] error: unable to open file!" << std::endl;
                exit(0);
            }

            file.close();
        }

        float get_balance(uint32_t id) {
            if (id >= CLIENT_COUNT || id < 0) {
                std::cerr << "Error: balance_table.h: << get_balance() : Invalid id!" << std::endl;
                exit(0);
            }
            return bal_tab[id];
        }

        void set_balance(uint32_t id, float new_bal) {
            if (id >= CLIENT_COUNT || id < 0) {
                std::cerr << "Error: balance_table.h: << get_balance() : Invalid id!" << std::endl;
                exit(0);
            }
            bal_tab[id] = new_bal;
            write_bal_tab_to_file();
        }

        void update_balance(uint32_t sid, uint32_t rid, float bal_change) {
            if (sid >= CLIENT_COUNT || sid < 0 || rid >= CLIENT_COUNT || sid < 0) {
                std::cerr << "Error: balance_table.h: << get_balance() : Invalid id!" << std::endl;
                exit(0);
            }
            bal_tab[sid] -= bal_change;
            bal_tab[rid] += bal_change;
            write_bal_tab_to_file();
        }

        void write_bal_tab_to_file() {
            // Disregard all the existing context and write from the start
            std::ofstream outfile(filename, std::ios::trunc);

            std::string bal_tab_str = "";
            for (int i = 0; i < CLIENT_COUNT; i++) {
                bal_tab_str += std::to_string(bal_tab[i]) + " ";
            }

            outfile << bal_tab_str;

            outfile.close();
        }

        void print_bal_tab() {
            std::cout << "client 0 : $" << bal_tab[0] << "; ";
            std::cout << "client 1 : $" << bal_tab[1] << "; ";
            std::cout << "client 2 : $" << bal_tab[2] << std::endl;
        }
    private:
        float bal_tab[CLIENT_COUNT];
        std::string filename;
};