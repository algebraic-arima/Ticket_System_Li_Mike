#ifndef TICKET_SYSTEM_TRAIN_H
#define TICKET_SYSTEM_TRAIN_H

#include <iostream>
#include <fstream>
#include <sstream>
#include "utility.h"
#include "BlockRiver.h"

namespace arima_kana {

    typedef m_string<22> train_id;
    typedef m_string<44> station_name;

    class date {
    public:
      // year 2024
      int m, d;
      constexpr static int month_days[13] =
              {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

      explicit date(int mm = 0, int dd = 0) : m(mm), d(dd) {}

      explicit date(const std::string &s) {
        if (s.size() != 5 || s[2] != '-') {
          error("invalid_date");
        }
        m = (s[0] - '0') * 10 + s[1] - '0';
        d = (s[3] - '0') * 10 + s[4] - '0';
      }

      friend std::ostream &operator<<(std::ostream &os, const date &da) {
        if (da.m < 10) os << '0';
        os << da.m << '-' << da.d;
        return os;
      }

      friend std::istream &operator>>(std::istream &is, date &da) {
        std::string s;
        is >> s;
        if (s.size() != 5 || s[2] != '-') {
          error("invalid_date");
        }
        da.m = (s[0] - '0') * 10 + s[1] - '0';
        da.d = (s[3] - '0') * 10 + s[4] - '0';
        return is;
      }

      int operator-(const date &rhs) const {
        int res = 0;
        for (int i = rhs.m; i < m; i++) {
          res += month_days[i];
        }
        res += d - rhs.d;
        return res;
      }
    };

    class time {
    public:
      int h, m;

      explicit time(int hh = 0, int mm = 0) : h(hh), m(mm) {}

      friend std::ostream &operator<<(std::ostream &os, const time &t) {
        if (t.h < 10) os << '0';
        os << t.h << ':';
        if (t.m < 10) os << '0';
        os << t.m;
        return os;
      }

      friend std::istream &operator>>(std::istream &is, time &t) {
        std::string s;
        is >> s;
        if (s.size() != 5 || s[2] != ':') {
          error("invalid_time");
        }
        t.h = (s[0] - '0') * 10 + s[1] - '0';
        t.m = (s[3] - '0') * 10 + s[4] - '0';
        return is;
      }

      time &operator+=(int x) {
        h += x / 60;
        m += x % 60;
        if (m >= 60) {
          m -= 60;
          h++;
        }
        return *this;
      }

      time operator+(int x) {
        time tmp = *this;
        tmp += x;
        return tmp;
      }

      bool operator<(const time &rhs) const {
        return h < rhs.h || (h == rhs.h && m < rhs.m);
      }

      bool operator==(const time &rhs) const {
        return h == rhs.h && m == rhs.m;
      }

    };

    class TrainInfo {
    public:
      train_id t_id;
      short station_num = 0;
      station_name stations[20];
      int seat_num = 0;
      int occupied_seat[100][100] = {0};
      // on the i-th day, from the j-th to the j+1-th station
      // date_num * (station_num - 1)
      int price[100] = {0};
      time start_time;
      int travel_time[100] = {0};// station_num - 1
      int stopover_time[100] = {0};// station_num - 2
      date start_date;
      int date_num = 0;
      char type = '\0';
    public:
      TrainInfo() = default;

      bool operator==(const TrainInfo &rhs) const {
        return t_id == rhs.t_id;
      }

      bool operator!=(const TrainInfo &rhs) const {
        return t_id != rhs.t_id;
      }

      bool operator<(const TrainInfo &rhs) const {
        return t_id < rhs.t_id;
      }

      void print(date d) {
        std::cout << t_id << ' ' << type << '\n';
        time tmp = start_time;
        int price_sum = 0;
        for (int i = 0; i < station_num; i++) {
          std::cout << stations[i] << ' ';
          if (i == 0) {
            std::cout << "xx-xx xx:xx -> ";
          } else {
            std::cout << d << ' ' << tmp << " -> ";
            if (i != station_num - 1) {
              tmp += stopover_time[i - 1];
              if (tmp.h >= 24) {
                tmp.h -= 24;
                d.m++;
              }
            }
          }
          if (i == station_num - 1) {
            std::cout << "xx-xx xx:xx";
          } else {
            std::cout << d << ' ' << tmp;
            tmp += travel_time[i];
            if (tmp.h >= 24) {
              tmp.h -= 24;
              d.m++;
            }
          }
          std::cout << ' ' << price_sum << ' ';
          if (i != station_num - 1) {
            price_sum += price[i];
          }
          std::cout << seat_num -
                       occupied_seat[d - start_date][i] << '\n';
        }
      }
    };

    class EdgeInfo {
    public:
      station_name from, to;
      time start_time;
      int travel_time = 0;
      int price = 0;
      train_id t_id;

      EdgeInfo() = default;

      bool operator<(const EdgeInfo &rhs) const {
        if (from != rhs.from) return from < rhs.from;
        if (t_id != rhs.t_id) return t_id < rhs.t_id;
      }

      bool operator==(const EdgeInfo &rhs) const {
        return from == rhs.from
               && t_id == rhs.t_id;
      }
    };


    class Train {
    public:
      BlockRiver<train_id, TrainInfo, 80> train_list;
      BlockRiver<station_name, EdgeInfo, 80> edge_list;
      // insert into train_list to initialize
      // insert into edge_list when release

      Train() : train_list("train"), edge_list("edge") {}

      void add_train(const std::string &param) {
        TrainInfo tmp;
        std::stringstream ss(param);
        std::string op;
        std::string stations, prices, travel_times, stopover_times, sale_date;
        for (int i = 0; i < 10; i++) {
          ss >> op;
          switch (op[1]) {
            case 'i':
              ss >> tmp.t_id;
              break;
            case 'n':
              ss >> tmp.station_num;
              break;
            case 'm':
              ss >> tmp.seat_num;
              break;
            case 's':
              ss >> stations;
              break;
            case 'p':
              ss >> prices;
              break;
            case 't':
              ss >> travel_times;
              break;
            case 'd':
              ss >> sale_date;
              break;
            case 'o':
              ss >> stopover_times;
              break;
            case 'x':
              ss >> tmp.start_time;
              break;
            case 'y':
              ss >> tmp.type;
              break;
            default:
              error("invalid_parameter");
          }
        }
        ss.clear();
        ss << stations;
        int cnt = 0;
        while (getline(ss, op, '|')) {
          tmp.stations[cnt++] = station_name(op);
        }
        ss.clear();
        ss << prices;
        cnt = 0;
        while (getline(ss, op, '|')) {
          tmp.price[cnt++] = std::stoi(op);
        }
        ss.clear();
        ss << travel_times;
        cnt = 0;
        while (getline(ss, op, '|')) {
          tmp.travel_time[cnt++] = std::stoi(op);
        }
        ss.clear();
        ss << stopover_times;
        cnt = 0;
        while (getline(ss, op, '|')) {
          tmp.stopover_time[cnt++] = std::stoi(op);
        }
        ss.clear();
        ss << sale_date;
        if (sale_date.size() != 11 || sale_date[7] != '|') {
          error("invalid_date");
        }
        std::string s = sale_date.substr(0, 5);
        std::string e = sale_date.substr(6, 5);
        tmp.start_date = date(s);
        tmp.date_num = date(e) - tmp.start_date + 1;
        train_list.insert(tmp.t_id, tmp);
      }

      void release_train(const train_id &t_id) {
        vector<TrainInfo *> t = train_list.find(t_id);
        if (t.size() != 1) {
          error("train not found or duplicated");
        }
        TrainInfo &tmp = *t[0];
        for (int i = 0; i < tmp.station_num - 1; i++) {
          EdgeInfo edge;
          edge.from = tmp.stations[i];
          edge.to = tmp.stations[i + 1];
          edge.start_time = tmp.start_time;
          edge.travel_time = tmp.travel_time[i];
          edge.price = tmp.price[i];
          edge.t_id = t_id;
          edge_list.insert(edge.from, edge);
        }
      }

      void delete_train(const train_id &t_id) {
        vector<TrainInfo *> t = train_list.find(t_id);
        if (t.size() != 1) {
          error("train not found or duplicated");
        }
        train_list.remove(t_id, *t[0]);
      }

      void query_train(const train_id &id, const date &d) {
        vector<TrainInfo *> t = train_list.find(id);
        if (t.size() != 1) {
          error("train not found or duplicated");
        }
        t[0]->print(d);
      }

      struct query_info {
        train_id t_id;
        station_name from, to;
        date s_d, e_d;
        time s_t, e_t;
        int price;
        int seat;

        friend std::ostream &operator<<(std::ostream &os, const query_info &q) {
          os << q.t_id << ' ' << q.from <<' '<< q.s_d << ' ' << q.s_t << " -> " << q.to << ' ' << q.e_d << ' '
             << q.e_t << ' ' << q.price << ' ' << q.seat;
          return os;
        }
      };

      void query_ticket(const station_name &s, const station_name &t, const date &d, bool is_time) {
        vector<EdgeInfo *> e = edge_list.find(s);
        if (e.size() == 0) {
          error("no such station included in released trains");
        }
        train_id tmp_id = e[0]->t_id;
        int i = 0;
        while (i < e.size()) {
          vector<TrainInfo *> tr = train_list.find(e[i]->t_id);
          if (tr.size() != 1) {
            error("train not found or duplicated");
          }
          TrainInfo &tmp = *tr[0];
        }
      }

    };
}

#endif
