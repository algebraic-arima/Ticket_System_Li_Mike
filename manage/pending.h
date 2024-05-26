#ifndef TICKET_SYSTEM_PENDING_H
#define TICKET_SYSTEM_PENDING_H

#include "train.h"
#include "account.h"

namespace arima_kana {

    class PendingInfo {
    public:

      PendingInfo() = default;

      PendingInfo(const acc_id mString, size_t i, const train_id mString1, const station_name mString2,
                  const station_name mString3, date date1, time time1, time time2, int i1, int i2) :
              buyer_id(mString), order_time(i), tr_id(mString1), from(mString2), to(mString3), d(date1), s(time1),
              t(time2), tot_price(i1), ticket_num(i2) {}


      acc_id buyer_id;
      size_t order_time = 0;
      train_id tr_id;
      station_name from, to;
      date d;
      time s, t; // t may be larger than 24
      int tot_price = 0, ticket_num = 0;

      bool operator==(const PendingInfo &rhs) const {
        return tr_id == rhs.tr_id
               && d == rhs.d
               && order_time == rhs.order_time;
      }

      bool operator!=(const PendingInfo &rhs) const {
        return !(*this == rhs);
      }

      bool operator<(const PendingInfo &rhs) const {
        if (tr_id != rhs.tr_id) return tr_id < rhs.tr_id;
        return order_time < rhs.order_time;
        // first: tr_id, second: order_time
      }
    };

    class PendingList {
      BlockRiver<train_id, PendingInfo, 80> pend_list;

    public:
      PendingList() : pend_list("6pending") {}

      void add_pending(const acc_id &c_id,
                       size_t order_time,
                       const train_id &tr_id,
                       const station_name &from,
                       const station_name &to,
                       date d, time s, time t,
                       int tot_price, int ticket_num) {
        PendingInfo order(c_id, order_time, tr_id, from, to,
                          d, s, t, tot_price, ticket_num);
        pend_list.insert(tr_id, order);
      }

      vector<PendingInfo *> get_pending(const train_id &tr_id, const date &d) {
        vector<PendingInfo *> res = pend_list.find(tr_id);
        vector<PendingInfo *> res1;
        for (auto &i: res) {
          if (i->d < d + 4 && d - 4 < i->d) {
            res1.push_back(i);
          }
        }//filter by date
        return res1;
      }

      void remove_pending(PendingInfo p) {
        pend_list.remove(p.tr_id, p);
      }

    };
}

#endif
