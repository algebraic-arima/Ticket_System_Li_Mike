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
      date d; // origin departure date
      time s, t; // s and t may be larger than 24
      int tot_price = 0, ticket_num = 0;
      int l = 0, r = 0;
      int train_seat_index = 0;
      bool success = true;
      bool refunded = true;

      OrderInfo() = default;

      OrderInfo(const acc_id &buyer_id, size_t order_time, train_id tr_id, station_name from, station_name to,
                date d, time s, time t, int tot_price, int ticket_num,
                int a, int b, int c) :
              buyer_id(buyer_id), order_time(order_time), tr_id(tr_id), from(from), to(to), d(d), s(s), t(t),
              tot_price(tot_price), ticket_num(ticket_num),
              l(a), r(b), train_seat_index(c) {}

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
        time ss = info.s, tt = info.t;
        ss.h %= 24, tt.h %= 24;
        os << "] " << info.tr_id << " "
           << info.from << " "
           << info.d + info.s.h / 24 << " "
           << ss << " -> "
           << info.to << " "
           << info.d + info.t.h / 24 << " "
           << tt << " "
           << info.tot_price << " "
           << info.ticket_num;
        return os;
      }
    };

    class Order {
    public:
      BlockRiver<acc_id, OrderInfo, 20, 20, 8, 50, 50> list;
      //200kb

      explicit Order(const std::string &af = "4order") :
              list(af) {}

      void query_order(const acc_id &c_id) {
        vector<OrderInfo> orders;
        list.find(c_id, true, orders);
        std::cout << orders.size() << '\n';
        for (auto &order: orders) {
          std::cout << order << '\n';
        }
      }

      void add_order(const acc_id &c_id,
                     size_t order_time,
                     const train_id &train_id,
                     const station_name &from,
                     const station_name &to,
                     date d, time s, time t,
                     int tot_price, int ticket_num,
                     int a, int b, int c,
                     bool success = true) {
        OrderInfo order(c_id, order_time,
                        train_id, from, to,
                        d, s, t,
                        tot_price, ticket_num,
                        a, b, c);
        order.success = success;
        order.refunded = false;
        list.insert(order.buyer_id, order);
      }

      OrderInfo *refund_ticket(const acc_id &c_id,
                               int n, bool &is_success,
                               bool &is_pending,
                               bool &is_refunded) {
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
