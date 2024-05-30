#ifndef TICKET_SYSTEM_PENDING_H
#define TICKET_SYSTEM_PENDING_H

#include "train.h"
#include "account.h"

namespace arima_kana {

    class PendingInfo {
    public:


      acc_id buyer_id;
      size_t train_seat_index = 0;
      int from = 0, to = 0;
      size_t order_time = 0;
      date d;// origin start date
      int ticket_num = 0;


      PendingInfo() = default;

      PendingInfo(const acc_id &c_id, size_t tsi,
                  int f, int t,
                  size_t ot, date _d,
                  int tn) :
              buyer_id(c_id),
              train_seat_index(tsi),
              from(f), to(t),
              order_time(ot), d(_d),
              ticket_num(tn) {}

      bool operator==(const PendingInfo &rhs) const {
        return train_seat_index == rhs.train_seat_index
               && d == rhs.d
               && order_time == rhs.order_time;
      }

      bool operator!=(const PendingInfo &rhs) const {
        return !(*this == rhs);
      }

      bool operator<(const PendingInfo &rhs) const {
        if (train_seat_index != rhs.train_seat_index)
          return train_seat_index < rhs.train_seat_index;
        return order_time < rhs.order_time;
        // first: tr_id, second: order_time
      }
    };

    class PendingList {
      BlockRiver<size_t, PendingInfo, 64, 64, 30, 100, 100> pend_list;

    public:
      PendingList() : pend_list("6pending") {}

      void add_pending(const acc_id &c_id,
                       size_t order_time,
                       size_t tsi,
                       const int &from,
                       const int &to,
                       date d, int ticket_num) {
        PendingInfo order(c_id, tsi, from, to, order_time,
                          d, ticket_num);
        pend_list.insert(tsi, order);
      }

      void get_pending(const int &tr_id, const date &d, vector<PendingInfo> &res1) {
        vector<PendingInfo> res;
        pend_list.find(tr_id, true, res);
        for (auto &i: res) {
          if (i.d < d + 4 && d - 4 < i.d) {
            res1.push_back(i);
          }
        }//filter by date
      }

      void remove_pending(PendingInfo p) {
        pend_list.remove(p.train_seat_index, p);
      }

    };
}

#endif
