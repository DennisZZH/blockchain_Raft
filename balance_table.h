#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Msg.pb.h"
#include "parameter.h"

class BalanceTable {
    public:
        BalanceTable () {}

        void load_file(std::string fname) {
            filename = fname;
            
            std::ifstream infile;
            infile.open(filename);

            std::string line;
            int start = 0;
            if (infile.is_open()) {
                getline(infile, line);
                for (int i = 0; i < CLIENT_COUNT; i++) {
                    int index = line.find(" ", start);
                    if (index != std::string::npos) {
                        std::string bal_str = line.substr(start, index - start);
                        bal_tab[i] = stoi(bal_str);
                        start = index + 1;
                    }
                }
            }
            else {
                std::cerr << "Error: blockchain.h : parse_file_to_bc(): Unable to open file!" << std::endl;
                exit(0);
            }

            infile.close();
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

        void update_balance(uint32_t id, float bal_change) {
            if (id >= CLIENT_COUNT || id < 0) {
                std::cerr << "Error: balance_table.h: << get_balance() : Invalid id!" << std::endl;
                exit(0);
            }
            bal_tab[id] += bal_change;
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

    private:
        float bal_tab[CLIENT_COUNT];
        std::string filename;
};