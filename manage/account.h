#ifndef TICKET_SYSTEM_LI_MIKE_ACCOUNT_H
#define TICKET_SYSTEM_LI_MIKE_ACCOUNT_H

#include "BlockRiver.h"

namespace arima_kana {

    typedef m_string<21> acc_id;
    typedef m_string<17> name;
    typedef m_string<31> passwd;
    typedef m_string<32> mailaddr;

    class AccountInfo {
    public:
      acc_id id;
      passwd pw;
      name nm;
      mailaddr mail;
      short priv = -1;

      AccountInfo() = default;

      AccountInfo(acc_id _id, passwd _pw, mailaddr _mail, short p) :
              id(_id), pw(_pw), mail(_mail), priv(p) {}

      bool operator==(const AccountInfo &rhs) const {
        return id == rhs.id;
      }
    };

    class Account {

      std::string acc_file;
      std::fstream acc_filer;
      size_t num = 0;
      BlockRiver<acc_id, size_t, 80> list;
      BlockRiver<acc_id, short, 80> priv_list;
      vector<acc_id> login_list;

      explicit Account(const std::string &af = "account") :
              list(af), priv_list(af + "_priv") {
        acc_file = af + "_raw";
        acc_filer.open(acc_file, std::ios::in);
        if (!acc_filer.is_open()) {
          acc_filer.close();
          init_num();
        } else {
          acc_filer.close();
          read_num();
        }
      }

      void init_num() {
        acc_filer.open(acc_file, std::ios::out | std::ios::binary);
        acc_filer.write(reinterpret_cast<char *>(&num), sizeof(size_t));
        acc_filer.close();
      }

      void read_num() {
        acc_filer.open(acc_file, std::ios::in | std::ios::binary);
        acc_filer.read(reinterpret_cast<char *>(&num), sizeof(size_t));
        acc_filer.close();
      }

      void append_main(const AccountInfo &t) {
        acc_filer.open(acc_file, std::ios::in | std::ios::out | std::ios::binary);
        acc_filer.seekp(0, std::ios::end);
        acc_filer.write(reinterpret_cast<const char *>(&t), sizeof(AccountInfo));
        acc_filer.close();
      }

      bool add_usr(const acc_id &c_id, const AccountInfo &usr) {
        if (num == 0) {//no usr
          AccountInfo root = usr;
          root.priv = 10;
          list.insert(usr.id, ++num);
          priv_list.insert(usr.id, 10);
          append_main(root);
          return true;
        }
        if (!login_list.find(c_id)) return false;//not logged in
        auto pos = priv_list.find(c_id);
        if (pos.size() != 1) return false;//not found or duplicated
        if (pos[0] <= usr.priv) return false;//privilege not enough
        if (!list.find(usr.id).empty()) return false;//exists
        list.insert(usr.id, ++num);
        priv_list.insert(usr.id, usr.priv);
        append_main(usr);
        return true;
      }

      bool login(const acc_id &id, const passwd &pw) {
        auto pos = list.find(id);
        if (pos.size() != 1) return false;
        if (login_list.find(id)) return false;
        acc_filer.open(acc_file, std::ios::in | std::ios::binary);
        acc_filer.seekg((pos[0] - 1) * sizeof(AccountInfo) + sizeof(size_t));
        AccountInfo tmp;
        acc_filer.read(reinterpret_cast<char *>(&tmp), sizeof(AccountInfo));
        acc_filer.close();
        if (tmp.pw != pw) return false;
        login_list.push_back(id);
        return true;
      }

      bool logout(const acc_id &id) {
        if (!login_list.find(id)) return false;
        login_list.erase(id);
        return true;
      }

      AccountInfo get_usr(const acc_id &c_id, const acc_id &id) {
        if (!login_list.find(c_id)) {
          error("not logged in");
        }
        auto pos = list.find(id);
        if (pos.size() != 1) {
          error("User not found or duplicated");
        }
        acc_filer.open(acc_file, std::ios::in | std::ios::binary);
        acc_filer.seekg((pos[0] - 1) * sizeof(AccountInfo) + sizeof(size_t));
        AccountInfo tmp;
        acc_filer.read(reinterpret_cast<char *>(&tmp), sizeof(AccountInfo));
        acc_filer.close();
        auto priv_pos = priv_list.find(c_id);
        if (c_id != id && priv_list.find(c_id)[0] <= tmp.priv) {
          error("privilege not enough");
        }
        return tmp;
      }

      bool modify_usr(const acc_id &id, const passwd &pw) {
        auto pos = list.find(id);
        if (pos.size() != 1) {
          return false;
          error("User not found or duplicated");
        }
        acc_filer.open(acc_file, std::ios::out | std::ios::in | std::ios::binary);
        acc_filer.seekg((pos[0] - 1) * sizeof(AccountInfo) + sizeof(size_t));
        AccountInfo tmp;
        acc_filer.read(reinterpret_cast<char *>(&tmp), sizeof(AccountInfo));
        acc_filer.seekp((pos[0] - 1) * sizeof(AccountInfo) + sizeof(size_t));
        tmp.pw = pw;
        acc_filer.write(reinterpret_cast<const char *>(&tmp), sizeof(AccountInfo));
        acc_filer.close();
        return true;
      }
    };

}

#endif
