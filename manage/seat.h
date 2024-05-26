#ifndef TICKET_SYSTEM_SEAT_H
#define TICKET_SYSTEM_SEAT_H

#include <iostream>
#include <fstream>
#include "Buffer.h"

namespace arima_kana {

    struct SeatInfo {
      int seat[100][100] = {0};

      SeatInfo() = default;

      SeatInfo(int date_num, int station_num, int seat_num) {
        for (int i = 0; i < date_num; i++) {
          for (int j = 0; j < station_num; j++) {
            seat[i][j] = seat_num;
          }
        }
      }
    };

    class Seat {
    public:
      int train_num = 0;
      std::fstream file;
      std::string name;
      List_Map_Buffer<SeatInfo, int, 1, 200> seat_list;

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
        SeatInfo tmp(d, s, n);
        seat_list[train_num] = tmp;
        file.open(name, std::ios::in | std::ios::out | std::ios::binary | std::ios::app);
        file.write(reinterpret_cast<char *>(&tmp), sizeof(SeatInfo));
        file.close();
      }

      SeatInfo &get_train(int pos) {
        return seat_list[pos];
      }

      int search_seat(int pos, int date_ind, int l, int r) {
        int ans = 2e5;
        SeatInfo &n = seat_list[pos];
        for (int i = l; i < r; i++) {
          int tmp = n.seat[date_ind][i];
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
        SeatInfo &tmp = seat_list[pos];
        bool flag = true;
        for (int i = l; i < r; i++) {
          if (tmp.seat[date_ind][i] < val) {
            flag = false;
            break;
          }
        }
        if (flag) {
          for (int i = l; i < r; i++) {
            tmp.seat[date_ind][i] -= val;
          }
        } else {
          error("seat not enough");
        }
      }

      void refund_seat(int pos, int date_ind, int l, int r, int val) {
        SeatInfo &tmp = seat_list[pos];
        for (int i = l; i < r; i++) {
          tmp.seat[date_ind][i] += val;
        }
      }
    };
}

#endif
