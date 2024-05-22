#ifndef TICKET_SYSTEM_SEAT_H
#define TICKET_SYSTEM_SEAT_H

#include <iostream>
#include <fstream>
#include "Buffer.h"

namespace arima_kana {

    struct Seat {
      int seat[100][100] = {0};
    };

    class SeatInfo {
    public:
      int train_num = 0;
      std::fstream file;
      std::string name;
      List_Map_Buffer<Seat, int, 1, 2000> seat_list;

      static constexpr int SIZE_INT = sizeof(int);

      explicit SeatInfo(const std::string &fn = "data/seat") :
              name(fn),
              seat_list(fn) {
        file.open(name, std::ios::in | std::ios::out | std::ios::binary);
        if (!file.is_open()) {
          init_data();
          file.close();
        } else {
          read_data();
          file.close();
        }
      }

      void init_data() {
        file.open(name, std::ios::out | std::ios::binary);
        file.write(reinterpret_cast<char *>(&train_num), SIZE_INT);
        file.close();
      }

      void read_data() {
        file.open(name, std::ios::in | std::ios::out | std::ios::binary);
        file.read(reinterpret_cast<char *>(&train_num), SIZE_INT);
        file.close();
      }

      void add_new_train() {
        train_num++;
        Seat tmp;
        seat_list[train_num] = tmp;
      }

      int search_seat(int pos, int date_ind, int l, int r) {
        int ans = 1e5;
        Seat &n = seat_list[pos];
        for (int i = l; i <= r; i++) {
          int tmp = n.seat[date_ind][i];
          if (tmp < ans) {
            ans = tmp;
          }
        }
        return ans;
      }

      void buy_seat(int pos, int date_ind, int l, int r, int val) {
        Seat &tmp = seat_list[pos];
        bool flag = true;
        for (int i = l; i <= r; i++) {
          if (tmp.seat[date_ind][i] < val) {
            flag = false;
            break;
          }
        }
        if (flag) {
          for (int i = l; i <= r; i++) {
            tmp.seat[date_ind][i] -= val;
          }
        } else {
          error("seat not enough");
        }
      }
    };
}

#endif
