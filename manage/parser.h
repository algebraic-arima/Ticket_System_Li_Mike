#ifndef TICKET_SYSTEM_PARSER_H
#define TICKET_SYSTEM_PARSER_H

#include <iostream>
#include <sstream>
#include "account.h"
#include "train.h"
#include "error.h"
#include "order.h"
#include "pending.h"

namespace arima_kana {

    class Parser {
    public:
      std::stringstream ss;
      Account acc;
      Train tr;
      Order ord;
      PendingList pend;

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
            handle_add_train();
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
            handle_query_train();
          } else if (cmd[len - 3] == 'k') {
            handle_query_ticket();
          } else if (cmd[len - 3] == 'f') {
            handle_query_transfer();
          } else if (cmd[len - 3] == 'd') {
            handle_query_order();
          }
        } else if (cmd[0] == 'm') {
          handle_modify_profile();
        } else if (cmd[0] == 'd') {
          handle_delete_train();
        } else if (cmd[0] == 'r') {
          if (cmd[len - 3] == 'a') {
            handle_release_train();
          } else if (cmd[len - 3] == 'k') {
            handle_refund_ticket();
          }
        } else if (cmd[0] == 'b') {
          handle_buy_ticket();
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
          std::cout << -1 << '\n';
        }
      }

      inline void handle_modify_profile() {
        acc_id c_id, id;
        std::string param;
        passwd pw;
        name nm;
        mailaddr mail;
        short priv;
        bool flag[4] = {false, false, false, false};
        while (ss >> param) {
          if (param[1] == 'c') {
            ss >> c_id;
          } else if (param[1] == 'u') {
            ss >> id;
          } else if (param[1] == 'p') {
            ss >> pw;
            flag[0] = true;
          } else if (param[1] == 'n') {
            ss >> nm;
            flag[1] = true;
          } else if (param[1] == 'm') {
            ss >> mail;
            flag[2] = true;
          } else if (param[1] == 'g') {
            ss >> priv;
            flag[3] = true;
          }
        }

        try {
          AccountInfo &tmp = acc.get_usr(c_id, id);
          short c_priv = acc.get_priv(c_id);
          if (flag[3] && c_priv <= priv) {
            error("modify-priv: privilege not enough");
          }
          if (flag[0]) {
            tmp.pw = pw;
          }
          if (flag[1]) {
            tmp.nm = nm;
          }
          if (flag[2]) {
            tmp.mail = mail;
          }
          if (flag[3]) {
            tmp.priv = priv;
          }
          std::cout << tmp << '\n';
        } catch (const ErrorException &e) {
          std::cout << -1 << '\n';
        }
      }

      inline void handle_add_train() {
        std::string train_param;
        getline(ss, train_param);
        try {
          tr.add_train(train_param);
          std::cout << "0\n";
        } catch (const ErrorException &e) {
          std::cout << -1 << '\n';
        }
      }

      inline void handle_delete_train() {
        train_id tr_id;
        std::string param;
        ss >> param;
        if (param[1] == 'i') {
          ss >> tr_id;
        }
        try {
          tr.delete_train(tr_id);
          std::cout << "0\n";
        } catch (const ErrorException &e) {
          std::cout << -1 << '\n';
        }
      }

      inline void handle_release_train() {
        train_id tr_id;
        std::string param;
        ss >> param;
        if (param[1] == 'i') {
          ss >> tr_id;
        }
        try {
          tr.release_train(tr_id);
          std::cout << "0\n";
        } catch (const ErrorException &e) {
          std::cout << -1 << '\n';
        }
      }

      inline void handle_query_order() {
        acc_id c_id;
        std::string param;
        ss >> param;
        if (param[1] == 'u') {
          ss >> c_id;
        }
        if (acc.login_list.find(c_id) == acc.login_list.end()) {
          std::cout << "-1\n"; // Not logged in
          return;
        }
        try {
          ord.query_order(c_id);
        } catch (const ErrorException &e) {
          std::cout << -1 << '\n';
        }
      }

      inline void handle_refund_ticket() {
        std::string param;
        acc_id c_id;
        int n = 1;
        while (ss >> param) {
          if (param[1] == 'u') {
            ss >> c_id;
          } else if (param[1] == 'n') {
            ss >> n;
          }
        }
        if (acc.login_list.find(c_id) == acc.login_list.end()) {
          std::cout << "-1\n";// Not logged in
          return;
        }
        OrderInfo *it;
        bool is_success = false;
        bool is_pending = false;
        bool is_refunded = false;
        try {
          it = ord.refund_ticket(c_id, n, is_success, is_pending, is_refunded);
          if (is_success) {
            tr.refund_ticket(it->tr_id, it->from, it->to, it->d, it->ticket_num);
          }
          if (is_pending) {
            PendingInfo p(c_id, it->order_time, it->tr_id,
                          it->from, it->to, it->d, it->s, it->t,
                          it->tot_price, it->ticket_num);
            pend.remove_pending(p);// time cost may be high
          }
          if (is_refunded) {}
          std::cout << "0\n";
        } catch (const ErrorException &e) {
          std::cout << -1 << '\n';
        }
        if (is_success) {
          vector<PendingInfo *> pending = pend.get_pending(it->tr_id, it->d);
          vector<PendingInfo> tmp;
          for (auto &i: pending) {
            tmp.push_back(*i);
          }
          for (auto &p: tmp) {
            if (tr.is_satisfiable(p.tr_id, p.from, p.to, p.d, p.ticket_num)) {
              pend.remove_pending(p);// p, a pointer, cannot be used after this line
              ord.alt_ticket(p.buyer_id, p.order_time);
            }
          }
        }
      }

      inline void handle_buy_ticket() {
        acc_id c_id;
        train_id tr_id;
        station_name from, to;
        date d;
        int n;
        bool queue = false;
        std::string q;
        std::string param;
        while (ss >> param) {
          if (param[1] == 'u') {
            ss >> c_id;
          } else if (param[1] == 'i') {
            ss >> tr_id;
          } else if (param[1] == 'f') {
            ss >> from;
          } else if (param[1] == 't') {
            ss >> to;
          } else if (param[1] == 'd') {
            ss >> d;
          } else if (param[1] == 'n') {
            ss >> n;
          } else if (param[1] == 'q') {
            ss >> q;
            if (q == "true") queue = true;
          }
        }
        if (acc.login_list.find(c_id) == acc.login_list.end()) {
          std::cout << "-1\n";//Not logged in
          return;
        }
        try {
          time s, t;
          auto price = tr.buy_ticket(s, t, tr_id, from, to, d, n, queue);
          if (price.first == -1) {
            std::cout << "-1\n";
            return;
          } else if (price.second == -1) {
            pend.add_pending(c_id, timestamp, tr_id, from, to, d, s, t, price.first, n);
            ord.add_order(c_id, timestamp, tr_id, from, to, d, s, t, price.first, n, false);
            std::cout << "queue\n";
            return;
          } else {
            std::cout << price.first * n << '\n';
            ord.add_order(c_id, timestamp, tr_id, from, to, d, s, t, price.first, n, true);
          }
        } catch (const ErrorException &e) {
          std::cout << -1 << '\n';
        }
      }

      inline void handle_query_train() {
        std::string param;
        train_id tr_id;
        date d;
        for (int i = 0; i < 2; i++) {
          ss >> param;
          if (param[1] == 'i') {
            ss >> tr_id;
          } else if (param[1] == 'd') {
            ss >> d;
          }
        }
        try {
          tr.query_train(tr_id, d);
        } catch (const ErrorException &e) {
          std::cout << -1 << '\n';
        }
      }

      inline void handle_query_ticket() {
        std::string param;
        station_name from, to;
        date d;
        std::string p;
        bool is_time = true;
        while (ss >> param) {
          if (param[1] == 's') {
            ss >> from;
          } else if (param[1] == 't') {
            ss >> to;
          } else if (param[1] == 'd') {
            ss >> d;
          } else if (param[1] == 'p') {
            ss >> p;
            if (p == "cost") is_time = false;
          }
        }
        try {
          tr.query_ticket(from, to, d, is_time);
        } catch (const ErrorException &e) {
          std::cout << 0 << '\n';
        }
      }

      inline void handle_query_transfer() {
        std::string param;
        station_name from, to;
        date d;
        std::string p;
        bool is_time = true;
        while (ss >> param) {
          if (param[1] == 's') {
            ss >> from;
          } else if (param[1] == 't') {
            ss >> to;
          } else if (param[1] == 'd') {
            ss >> d;
          } else if (param[1] == 'p') {
            ss >> p;
            if (p == "cost") is_time = false;
          }
        }
        try {
          tr.query_transfer(from, to, d, is_time);
        } catch (const ErrorException &e) {
          std::cout << 0 << '\n';
        }
      }

    };
}

#endif //TICKET_SYSTEM_PARSER_H
