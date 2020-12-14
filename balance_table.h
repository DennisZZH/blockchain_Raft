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
            if (infile.is_open()) {
                getline(infile, line);
                int i, start = 0;
                for (i = 0; i < CLIENT_COUNT; i++) {
                    int index = line.find(" ", start);
                    if (index != std::string::npos) {
                        std::string bal_str = line.substr(start, index - start);
                        bal_tab[i] = stof(bal_str);
                        start = index + 1;
                    }
                }
                // bal_tab[CLIENT_COUNT - 1] = stof(line.substr(start, line.size() - start));
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