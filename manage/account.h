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

      bool operator<(const AccountInfo &rhs) const {
        return id < rhs.id;
      }
    };

    class Account {

      BlockRiver<acc_id, AccountInfo, 80> list;
      std::map<acc_id, short> login_list;

      explicit Account(const std::string &af = "account") :
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
        if (!list.find(usr.id).empty())
          return false;//exists
        list.insert(usr.id, usr);
        return true;
      }

      bool login(const acc_id &id, const passwd &pw) {
        auto pos = list.find(id);
        if (pos.size() != 1)
          return false;//not found or duplicated
        if (login_list.find(id) != login_list.end())
          return false;
        vector<AccountInfo *> tmp = list.find(id);
        if (tmp.size() != 1)
          return false;
        if (tmp[0]->pw != pw)
          return false;
        login_list[id] = tmp[0]->priv;
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
        vector<AccountInfo *> pos = list.find(id);
        if (pos.size() != 1) {
          error("User not found or duplicated");
        }
        if (c_id != id && it->second <= pos[0]->priv) {
          error("privilege not enough");
        }
        return *pos[0];
      }

      AccountInfo modify_usr(const acc_id &c_id, const acc_id &id, const passwd &pw) {
        auto it = login_list.find(c_id);
        if (it == login_list.end()) {
          error("not logged in");
        }
        vector<AccountInfo *> pos = list.find(id);
        if (pos.size() != 1) {
          error("User not found or duplicated");
        }
        if (c_id != id && it->second <= pos[0]->priv) {
          error("privilege not enough");
        }
        pos[0]->pw = pw;
        return *pos[0];
      }

      AccountInfo modify_usr(const acc_id &c_id, const acc_id &id, const name &nm) {
        auto it = login_list.find(c_id);
        if (it == login_list.end()) {
          error("not logged in");
        }
        vector<AccountInfo *> pos = list.find(id);
        if (pos.size() != 1) {
          error("User not found or duplicated");
        }
        if (c_id != id && it->second <= pos[0]->priv) {
          error("privilege not enough");
        }
        pos[0]->nm = nm;
        return *pos[0];
      }

      AccountInfo modify_usr(const acc_id &c_id, const acc_id &id, const mailaddr &mail) {
        auto it = login_list.find(c_id);
        if (it == login_list.end()) {
          error("not logged in");
        }
        vector<AccountInfo *> pos = list.find(id);
        if (pos.size() != 1) {
          error("User not found or duplicated");
        }
        if (c_id != id && it->second <= pos[0]->priv) {
          error("privilege not enough");
        }
        pos[0]->mail = mail;
        return *pos[0];
      }

      AccountInfo modify_usr(const acc_id &c_id, const acc_id &id, const short &priv) {
        auto it = login_list.find(c_id);
        if (it == login_list.end()) {
          error("not logged in");
        }
        vector<AccountInfo *> pos = list.find(id);
        if (pos.size() != 1) {
          error("User not found or duplicated");
        }
        if (c_id != id && it->second <= pos[0]->priv) {
          error("privilege not enough");
        }
        if (it->second <= priv) {
          error("modify-to: privilege not enough");
        }
        pos[0]->priv = priv;
        auto it_mod = login_list.find(id);
        if (it_mod != login_list.end()) {
          it_mod->second = priv;
        }
        return *pos[0];
      }
    };

}

#endif
