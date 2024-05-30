#ifndef TICKET_SYSTEM_LI_MIKE_ACCOUNT_H
#define TICKET_SYSTEM_LI_MIKE_ACCOUNT_H

#include "unique_BlockRiver.h"
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

      AccountInfo(acc_id _id, passwd _pw, name nm, mailaddr _mail, short p) :
              id(_id), pw(_pw), nm(nm), mail(_mail), priv(p) {}

      bool operator==(const AccountInfo &rhs) const {
        return id == rhs.id;
      }

      bool operator<(const AccountInfo &rhs) const {
        return id < rhs.id;
      }

      friend std::ostream &operator<<(std::ostream &os, const AccountInfo &info) {
        os << info.id << " " << info.nm << " " << info.mail << " " << info.priv;
        //<< " " << info.pw;
        return os;
      }
    };

    class Account {
    public:
      unique_BlockRiver<acc_id, AccountInfo, 32, 180, 88, 50, 50> list;
      //tree: 22*180*100=400kb
      //data: (22+104)*32*100=400kb
      map<acc_id, short> login_list;

      explicit Account(const std::string &af = "1account") :
              list(af) {}

      bool add_usr(const acc_id &c_id, const AccountInfo &usr) {
        if (list.list.empty()) {//no usr
          AccountInfo root = usr;
          root.priv = 10;
          list.insert(usr.id, root);
          return true;
        }
        auto it = login_list.find(c_id);
        if (it == login_list.end())
          return false; //not logged in
        if (it->second <= usr.priv)
          return false; //privilege not enough
        if (list.find(usr.id))
          return false;//exists
        list.insert(usr.id, usr);
        return true;
      }

      bool login(const acc_id &id, const passwd &pw) {
        auto pos = list.find(id);
        if (!pos)
          return false;//not found or duplicated
        if (login_list.find(id) != login_list.end())
          return false;
        AccountInfo *tmp = list.find(id);
        if (!tmp)
          return false;
        if (tmp->pw != pw)
          return false;
        login_list.insert(pair(id, tmp->priv));
        return true;
      }

      bool logout(const acc_id &id) {
        auto it = login_list.find(id);
        if (it == login_list.end())
          return false;
        login_list.erase(it);
        return true;
      }

      AccountInfo *get_usr(const acc_id &c_id, const acc_id &id) {
        auto it = login_list.find(c_id);
        if (it == login_list.end()) {
//          error("get_usr: cur user not logged in");
          return nullptr;
        }
        AccountInfo *pos = list.find(id);
        if (!pos) {
//          error("User not found or duplicated");
          return nullptr;
        }
        if (c_id != id && it->second <= pos->priv) {
          return nullptr;
//          error("privilege not enough");
        }
        return pos;
      }

      short get_priv(const acc_id &c_id) {
        auto it = login_list.find(c_id);
        if (it == login_list.end()) {
          error("get privilege: not logged in");
        }
        return it->second;
      }

    };

}

#endif
