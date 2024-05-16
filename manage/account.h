#ifndef TICKET_SYSTEM_LI_MIKE_ACCOUNT_H
#define TICKET_SYSTEM_LI_MIKE_ACCOUNT_H

#include "BlockRiver.h"
#include <map>

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
      std::map<acc_id, pair<size_t, short>> login_list;

      explicit Account(const std::string &af = "account") :
              list(af) {
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

      void read_main(AccountInfo &t, const size_t pos) {
        acc_filer.open(acc_file, std::ios::in | std::ios::binary);
        acc_filer.seekg((pos - 1) * sizeof(AccountInfo) + sizeof(size_t));
        acc_filer.read(reinterpret_cast<char *>(&t), sizeof(AccountInfo));
        acc_filer.close();
      }

      void write_main(const AccountInfo &t, const size_t pos) {
        acc_filer.open(acc_file, std::ios::in | std::ios::out | std::ios::binary);
        acc_filer.seekp((pos - 1) * sizeof(AccountInfo) + sizeof(size_t));
        acc_filer.write(reinterpret_cast<const char *>(&t), sizeof(AccountInfo));
        acc_filer.close();
      }

      bool add_usr(const acc_id &c_id, const AccountInfo &usr) {
        if (num == 0) {//no usr
          AccountInfo root = usr;
          root.priv = 10;
          list.insert(usr.id, ++num);
          append_main(root);
          return true;
        }
        auto it = login_list.find(c_id);
        if (it == login_list.end())
          return false; //not logged in
        if (it->second.second <= usr.priv)
          return false; //privilege not enough
        if (!list.find(usr.id).empty())
          return false;//exists
        list.insert(usr.id, ++num);
        append_main(usr);
        return true;
      }

      bool login(const acc_id &id, const passwd &pw) {
        auto pos = list.find(id);
        if (pos.size() != 1)
          return false;//not found or duplicated
        if (login_list.find(id) != login_list.end())
          return false;
        AccountInfo tmp;
        read_main(tmp, pos[0]);
        if (tmp.pw != pw)
          return false;
        login_list[id] = pair(pos[0], tmp.priv);
        return true;
      }

      bool logout(const acc_id &id) {
        auto it = login_list.find(id);
        if (it == login_list.end())
          return false;
        login_list.erase(it);
        return true;
      }

      AccountInfo get_usr(const acc_id &c_id, const acc_id &id) {
        auto it = login_list.find(c_id);
        if (it == login_list.end()) {
          error("not logged in");
        }
        auto pos = list.find(id);
        if (pos.size() != 1) {
          error("User not found or duplicated");
        }
        AccountInfo tmp;
        read_main(tmp, pos[0]);
        if (c_id != id && it->second.second <= tmp.priv) {
          error("privilege not enough");
        }
        return tmp;
      }

      AccountInfo modify_usr(const acc_id &c_id, const acc_id &id, const passwd &pw) {
        auto it = login_list.find(c_id);
        if (it == login_list.end()) {
          error("not logged in");
        }
        auto pos = list.find(id);
        if (pos.size() != 1) {
          error("User not found or duplicated");
        }
        AccountInfo tmp;
        read_main(tmp, pos[0]);
        if (c_id != id && it->second.second <= tmp.priv) {
          error("privilege not enough");
        }
        tmp.pw = pw;
        write_main(tmp, pos[0]);
        return tmp;
      }

      AccountInfo modify_usr(const acc_id &c_id, const acc_id &id, const name &nm) {
        auto it = login_list.find(c_id);
        if (it == login_list.end()) {
          error("not logged in");
        }
        auto pos = list.find(id);
        if (pos.size() != 1) {
          error("User not found or duplicated");
        }
        AccountInfo tmp;
        read_main(tmp, pos[0]);
        if (c_id != id && it->second.second <= tmp.priv) {
          error("privilege not enough");
        }
        tmp.nm = nm;
        write_main(tmp, pos[0]);
        return tmp;
      }

      AccountInfo modify_usr(const acc_id &c_id, const acc_id &id, const mailaddr &mail) {
        auto it = login_list.find(c_id);
        if (it == login_list.end()) {
          error("not logged in");
        }
        auto pos = list.find(id);
        if (pos.size() != 1) {
          error("User not found or duplicated");
        }
        AccountInfo tmp;
        read_main(tmp, pos[0]);
        if (c_id != id && it->second.second <= tmp.priv) {
          error("privilege not enough");
        }
        tmp.mail = mail;
        write_main(tmp, pos[0]);
        return tmp;
      }

      AccountInfo modify_usr(const acc_id &c_id, const acc_id &id, const short &priv) {
        auto it = login_list.find(c_id);
        if (it == login_list.end()) {
          error("not logged in");
        }
        auto pos = list.find(id);
        if (pos.size() != 1) {
          error("User not found or duplicated");
        }
        AccountInfo tmp;
        read_main(tmp, pos[0]);
        if (c_id != id && it->second.second <= tmp.priv) {
          error("privilege not enough");
        }
        if (it->second.second <= priv) {
          error("modify: privilege not enough");
        }
        tmp.priv = priv;
        write_main(tmp, pos[0]);
        it->second.second = priv;
        return tmp;
      }
    };

}

#endif
