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
           << info.s << " -> "
           << info.to << " ";
        if (info.t.h >= 24) {
          int hh = info.t.h % 24, mm = info.t.m;
          os << info.d + info.t.h / 24 << " " << (hh < 10 ? "0" : "") <<
             hh << ":" << (mm < 10 ? "0" : "")
             << info.t.m << " ";
        } else {
          os << info.d << " " << info.t << " ";
        }
        os << info.tot_price << " " << info.ticket_num;
        return os;
      }
    };

    class Order {
    public:
      BlockRiver<acc_id, OrderInfo, 16, 16, 6, 60> list;

      explicit Order(const std::string &af = "4order") :
              list(af) {}

      void query_order(const acc_id &c_id) {
        vector<OrderInfo *> orders = list.find(c_id);
        std::cout << orders.size() << '\n';
        for (auto &order: orders) {
          std::cout << *order << '\n';
        }
      }

      void add_order(const acc_id &c_id,
                     size_t order_time,
                     const train_id &train_id,
                     const station_name &from,
                     const station_name &to,
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

      OrderInfo *refund_ticket(const acc_id &c_id, int n, bool &is_success, bool &is_pending, bool &is_refunded) {
        vector<OrderInfo *> orders = list.find(c_id);
        if (orders.size() < n) {
          error("No such order");
        }// RE check
        is_success = orders[n - 1]->success;
        is_pending = !orders[n - 1]->success && !orders[n - 1]->refunded;
        is_refunded = orders[n - 1]->refunded;
        orders[n - 1]->refunded = true;
        orders[n - 1]->success = false;
        return orders[n - 1];
      }

      OrderInfo *alt_ticket(const acc_id &id, size_t o_t) {
        vector<OrderInfo *> orders = list.find(id);
        for (auto &order: orders) {
          if (order->order_time == o_t) {
            order->success = true;
            order->refunded = false;
            return order;
          }
        }
        return nullptr;
      }

    };
}

#endif //TICKET_SYSTEM_ORDER_H
