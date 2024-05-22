#ifndef TICKET_SYSTEM_ORDER_H
#define TICKET_SYSTEM_ORDER_H

#include "account.h"
#include "train.h"

namespace arima_kana {
    class OrderInfo {
    public:
      acc_id buyer_id;
      size_t order_time = 0;
      train_id tr_id;
      station_name from, to;
      date d;
      time s, t; // t may be larger than 24
      int tot_price = 0, ticket_num = 0;
      bool success = true;
      bool refunded = true;

      OrderInfo() = default;

      OrderInfo(const acc_id &buyer_id, size_t order_time, train_id tr_id, station_name from, station_name to,
                date d, time s, time t, int tot_price, int ticket_num) :
              buyer_id(buyer_id), order_time(order_time), tr_id(tr_id), from(from), to(to), d(d), s(s), t(t),
              tot_price(tot_price), ticket_num(ticket_num) {}

      bool operator==(const OrderInfo &rhs) const {
        return buyer_id == rhs.buyer_id && order_time == rhs.order_time;
      }

      bool operator<(const OrderInfo &rhs) const {
        return buyer_id < rhs.buyer_id ||
               (buyer_id == rhs.buyer_id && order_time > rhs.order_time);
      }

      friend std::ostream &operator<<(std::ostream &os, const OrderInfo &info) {
        os << '[';
        if (info.success) {
          os << "success";
        } else {
          if (info.refunded) {
            os << "refunded";
          } else {
            os << "pending";
          }
        }
        os << "] " << info.tr_id << ' '
           << info.from << " "
           << info.d << " "
           << info.s << ' '
           << info.to << " ";
        if (info.t.h >= 24) {
          os << info.d + info.t.h / 24 << " " <<
             info.t.h % 24 << ":"
             << info.t.m << ":";
        } else {
          os << info.d << " " << info.t << " ";
        }
        os << info.tot_price << " " << info.ticket_num;
        return os;
      }
    };

    class Order {
    public:
      BlockRiver<acc_id, OrderInfo, 80> list;

      explicit Order(const std::string &af = "data/order") :
              list(af) {}

      void query_order(const acc_id &c_id) {
        vector<OrderInfo *> orders = list.find(c_id);
        std::cout << orders.size() << std::endl;
        for (auto &order: orders) {
          std::cout << *order << std::endl;
        }
      }

      void add_order(const acc_id &c_id,
                     size_t order_time,
                     train_id train_id,
                     station_name from,
                     station_name to,
                     date d, time s, time t,
                     int tot_price, int ticket_num,
                     bool success = true) {
        OrderInfo order(c_id, order_time,
                        train_id, from, to,
                        d, s, t,
                        tot_price, ticket_num);
        order.success = success;
        order.refunded = false;
        list.insert(order.buyer_id, order);
      }

      OrderInfo *refund_ticket(const acc_id &c_id, int n = 1) {
        vector<OrderInfo *> orders = list.find(c_id);
        orders[n - 1]->refunded = true;
        orders[n - 1]->success = false;
        return orders[n - 1];
      }

    };
}

#endif //TICKET_SYSTEM_ORDER_H
