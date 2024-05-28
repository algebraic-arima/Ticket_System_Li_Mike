#ifndef TICKET_SYSTEM_SEAT_H
#define TICKET_SYSTEM_SEAT_H

#include <iostream>
#include <fstream>
#include "Buffer.h"

namespace arima_kana {

    struct SeatInfo {
      int seat[100] = {0};

      SeatInfo() = default;

      SeatInfo(int station_num, int seat_num) {
        for (int j = 0; j < station_num; j++) {
          seat[j] = seat_num;
        }
      }
    };

    class Seat {
    public:
      int train_num = 0;
      std::fstream file;
      std::string name;
      List_Map_Buffer<SeatInfo, int, 1, 1> seat_list;

      static constexpr int SIZE_INT = sizeof(int);

      explicit Seat(const std::string &fn = "5seat") :
              name(fn),
              seat_list(fn) {
        file.open(name, std::ios::in | std::ios::out | std::ios::binary);
        if (!file.is_open()) {
          file.close();
          init_data();
        } else {
          file.close();
          read_data();// close the file first
        }
      }

      ~Seat() {
        file.open(name, std::ios::in | std::ios::out | std::ios::binary);
        file.write(reinterpret_cast<char *>(&train_num), SIZE_INT);
        file.close();
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

      void add_new_train(int d, int s, int n) {
        train_num++;
        SeatInfo tmp(s, n), e;
        int cnt = 100;
        file.open(name, std::ios::in | std::ios::out | std::ios::binary | std::ios::app);
        while (d--) {
          file.write(reinterpret_cast<char *>(&tmp), sizeof(SeatInfo));
          cnt--;
        }
        while (cnt--)
          file.write(reinterpret_cast<char *>(&e), sizeof(SeatInfo));
        file.close();
      }

      SeatInfo &get_train_date(int pos, int date_ind) {
        return seat_list[(pos - 1) * 100 + date_ind + 1];
      }

      int search_seat(int pos, int date_ind, int l, int r) {
        int ans = 2e5;
        SeatInfo &n = get_train_date(pos, date_ind);
        for (int i = l; i < r; i++) {
          int tmp = n.seat[i];
          if (tmp < ans) {
            ans = tmp;
          }
        }
        return ans;
      }

      /// buy val seats from l to r
      /// start from the l-th station, end at the r-th station
      /// so int i = l; i < r; i++
      void buy_seat(int pos, int date_ind, int l, int r, int val) {
        SeatInfo &tmp = get_train_date(pos, date_ind);
        bool flag = true;
        for (int i = l; i < r; i++) {
          if (tmp.seat[i] < val) {
            flag = false;
            break;
          }
        }
        if (flag) {
          for (int i = l; i < r; i++) {
            tmp.seat[i] -= val;
          }
        } else {
          error("seat not enough");
        }
      }

      void refund_seat(int pos, int date_ind, int l, int r, int val) {
        SeatInfo &tmp = get_train_date(pos, date_ind);
        for (int i = l; i < r; i++) {
          tmp.seat[i] += val;
        }
      }
    };
}

#endif
