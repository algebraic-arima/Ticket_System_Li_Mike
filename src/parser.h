#ifndef TICKET_SYSTEM_PARSER_H
#define TICKET_SYSTEM_PARSER_H

#include <iostream>
#include <sstream>
#include "account.h"
#include "train.h"
#include "error.h"
#include "order.h"
#include "pending.h"
#include "timer.h"

namespace arima_kana {

    Timer timer1("clean");
    Timer timer2("exit");
    Timer timer3("add_user");
    Timer timer4("login");
    Timer timer5("logout");
    Timer timer6("query_profile");
    Timer timer7("modify_profile");
    Timer timer8("add_train");
    Timer timer9("delete_train");
    Timer timer10("release_train");
    Timer timer11("query_train");
    Timer timer12("query_ticket");
    Timer timer13("query_transfer");
    Timer timer14("buy_ticket");
    Timer timer15("query_order");
    Timer timer16("refund_ticket");

    std::stringstream ss;
    Account acc;
    Train tr;
    Order ord;
    PendingList pend;

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
        std::cout << "-1\n";
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
        if (is_refunded) {
          std::cout << -1 << '\n';
          return;
        }
        std::cout << "0\n";
      } catch (const ErrorException &e) {
        std::cout << -1 << '\n';
      }
      if (is_success) {
        vector<PendingInfo> pending = pend.get_pending(it->tr_id, it->d);
        for (auto &p: pending) {
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

    bool handle(const std::string &str) {
      ss.clear();
      ss.str(str);
      std::string cmd;
      ss >> cmd;
      std::cout << cmd << ' ';
      timestamp = std::stoi(cmd.substr(1, cmd.size() - 2));
//        if (timestamp == 1729119) {
//          tr.query_train(train_id("DwellerinMannahatt"), date(6, 25));
//          tr.query_train(train_id("Andmenacingvoice"), date(6, 26));
//          tr.query_train(train_id("youngfellowsrobus"), date(6, 25));
//        }//debug
      ss >> cmd;
      if (cmd == "exit") {
        std::cout << "bye\n";
        return false;
      }
      auto len = cmd.size();
      char c1 = cmd[0], c2 = cmd[len - 3];
      if (c1 == 'a') {
        if (c2 == 's') {
          timer3.start();
          handle_add_user();
          timer3.stop();
        } else if (c2 == 'a') {
          timer8.start();
          handle_add_train();
          timer8.stop();
        }
      } else if (c1 == 'l') {
        if (c2 == 'g') {
          timer4.start();
          handle_login();
          timer4.stop();
        } else if (c2 == 'o') {
          timer5.start();
          handle_logout();
          timer5.stop();
        }
      } else if (c1 == 'q') {
        if (c2 == 'i') {
          timer6.start();
          handle_query_profile();
          timer6.stop();
        } else if (c2 == 'a') {
          timer11.start();
          handle_query_train();
          timer11.stop();
        } else if (c2 == 'k') {
          timer12.start();
          handle_query_ticket();
          timer12.stop();
        } else if (c2 == 'f') {
          timer13.start();
          handle_query_transfer();
          timer13.stop();
        } else if (c2 == 'd') {
          timer15.start();
          handle_query_order();
          timer15.stop();
        }
      } else if (c1 == 'm') {
        timer7.start();
        handle_modify_profile();
        timer7.stop();
      } else if (c1 == 'd') {
        timer9.start();
        handle_delete_train();
        timer9.stop();
      } else if (c1 == 'r') {
        if (c2 == 'a') {
          timer10.start();
          handle_release_train();
          timer10.stop();
        } else if (c2 == 'k') {
          timer16.start();
          handle_refund_ticket();
          timer16.stop();
        }
      } else if (c1 == 'b') {
        timer14.start();
        handle_buy_ticket();
        timer14.stop();
      } else if (c1 == 'c') {
        // clean
      }
      return true;
    }

}


#endif //TICKET_SYSTEM_PARSER_H