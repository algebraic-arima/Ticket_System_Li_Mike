#ifndef TICKET_SYSTEM_PARSER_H
#define TICKET_SYSTEM_PARSER_H

#include <iostream>
#include <sstream>
#include "account.h"
#include "train.h"
#include "error.h"

namespace arima_kana {
    class Parser {
    public:
      std::stringstream ss;
      Account acc;
      Train tr;
      int timestamp = 0;

      Parser() = default;

      void handle(const std::string &str) {
        ss.clear();
        ss.str(str);
        std::string cmd;
        ss >> cmd;
        std::cout << cmd << ' ';
        timestamp = std::stoi(cmd.substr(1, cmd.size() - 2));
        ss >> cmd;
        auto len = cmd.size();
        if (cmd[0] == 'a') {
          if (cmd[len - 3] == 's') {
            handle_add_user();
          } else if (cmd[len - 3] == 'a') {
            // add_train
          }
        } else if (cmd[0] == 'l') {
          if (cmd[len - 3] == 'g') {
            handle_login();
          } else if (cmd[len - 3] == 'o') {
            handle_logout();
          }
        } else if (cmd[0] == 'q') {
          if (cmd[len - 3] == 'i') {
            handle_query_profile();
          } else if (cmd[len - 3] == 'a') {
            // query_train
          } else if (cmd[len - 3] == 'k') {
            // query_ticket
          } else if (cmd[len - 3] == 'f') {
            // query_transfer
          } else if (cmd[len - 3] == 'd') {
            // query_order
          }
        } else if (cmd[0] == 'm') {
          handle_modify_profile();
        } else if (cmd[0] == 'd') {
          // delete_train
        } else if (cmd[0] == 'r') {
          if (cmd[len - 3] == 'a') {
            // release_train
          } else if (cmd[len - 3] == 'k') {
            // refund_ticket
          }
        } else if (cmd[0] == 'b') {
          // buy_ticket
        } else if (cmd[0] == 'c') {
          // clean
        }

      }

      inline void handle_add_user() {
        acc_id c_id, id;
        passwd pw;
        short priv;
        mailaddr mail;
        name nm;
        std::string param;
        for (int i = 0; i < 6; ++i) {
          ss >> param;
          if (param[1] == 'u') {
            ss >> id;
          } else if (param[1] == 'p') {
            ss >> pw;
          } else if (param[1] == 'n') {
            ss >> nm;
          } else if (param[1] == 'm') {
            ss >> mail;
          } else if (param[1] == 'g') {
            ss >> priv;
          } else if (param[1] == 'c') {
            ss >> c_id;
          }
        }
        if (acc.add_usr(c_id, AccountInfo(id, pw, nm, mail, priv))) {
          std::cout << "0\n";
        } else {
          std::cout << "-1\n";
        }
      }

      inline void handle_login() {
        acc_id c_id;
        passwd pw;
        std::string param;
        for (int i = 0; i < 2; ++i) {
          ss >> param;
          if (param[1] == 'u') {
            ss >> c_id;
          } else if (param[1] == 'p') {
            ss >> pw;
          }
        }
        if (acc.login(c_id, pw)) {
          std::cout << "0\n";
        } else {
          std::cout << "-1\n";
        }
      }

      inline void handle_logout() {
        acc_id c_id;
        std::string param;
        ss >> param;
        if (param[1] == 'u') {
          ss >> c_id;
        }
        if (acc.logout(c_id)) {
          std::cout << "0\n";
        } else {
          std::cout << "-1\n";
        }
      }

      inline void handle_query_profile() {
        acc_id c_id, id;
        std::string param;
        for (int i = 0; i < 2; ++i) {
          ss >> param;
          if (param[1] == 'c') {
            ss >> c_id;
          } else if (param[1] == 'u') {
            ss >> id;
          }
        }
        try {
          AccountInfo &tmp = acc.get_usr(c_id, id);
          std::cout << tmp << '\n';
        } catch (const ErrorException &e) {
          std::cout << e.getMessage() << '\n';
        }
      }

      inline void handle_modify_profile() {
        acc_id c_id, id;
        std::string param;
        for (int i = 0; i < 2; ++i) {
          ss >> param;
          if (param[1] == 'c') {
            ss >> c_id;
          } else if (param[1] == 'u') {
            ss >> id;
          }
        }
        try {
          AccountInfo &tmp = acc.get_usr(c_id, id);
          short c_priv = acc.get_priv(c_id);
          while (ss >> param) {
            if (param[1] == 'p') {
              passwd pw;
              ss >> pw;
            } else if (param[1] == 'n') {
              name nm;
              ss >> nm;
            } else if (param[1] == 'm') {
              mailaddr mail;
              ss >> mail;
            } else if (param[1] == 'g') {
              short priv;
              ss >> priv;
              if (c_priv <= priv) {
                error("modify-priv: privilege not enough");
              }
            }
          }
        } catch (const ErrorException &e) {
          std::cout << e.getMessage() << '\n';
        }
      }


    };
}

#endif //TICKET_SYSTEM_PARSER_H
