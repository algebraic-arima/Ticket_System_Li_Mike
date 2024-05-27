#ifndef TICKET_SYSTEM_TRAIN_H
#define TICKET_SYSTEM_TRAIN_H

#include <iostream>
#include <fstream>
#include <sstream>
#include "utility.h"
#include "BlockRiver.h"
#include "unique_BlockRiver.h"
#include "seat.h"


namespace arima_kana {
    extern int timestamp;

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
        os << da.m << '-';
        if (da.d < 10) os << '0';
        os << da.d;
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

      bool operator<(const date &rhs) const {
        return m < rhs.m || (m == rhs.m && d < rhs.d);
      }

      int operator-(const date &rhs) const {
        int res = 0;
        for (int i = rhs.m; i < m; i++) {
          res += month_days[i];
        }
        res += d - rhs.d;
        return res;
      }

      date &operator+=(int x) {
        while (x > 0) {
          if (month_days[m] - d >= x) {
            d += x;
            x = 0;
          } else {
            x -= month_days[m] - d;
            d = 0;
            m++;
          }
          if (m > 12) {
            m = 1;
          }
        }
        return *this;
      }

      date operator+(int x) const {
        date tmp = *this;
        tmp += x;
        return tmp;
      }

      date &operator-=(int x) {
        while (x > 0) {
          if (d > x) {
            d -= x;
            x = 0;
          } else {
            x -= d;
            d = month_days[--m];
          }
        }
        return *this;
      }

      date operator-(int x) const {
        date tmp = *this;
        tmp -= x;
        return tmp;
      }

      bool operator==(const date &rhs) const {
        return m == rhs.m && d == rhs.d;
      }

      bool operator!=(const date &rhs) const {
        return m != rhs.m || d != rhs.d;
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

      time &operator-=(int x) {
        h -= x / 60;
        m -= x % 60;
        if (m < 0) {
          m += 60;
          h--;
        }
        return *this;
      }

      time operator-(int x) {
        time tmp = *this;
        tmp -= x;
        return tmp;
      }

      int operator-(const time &rhs) const {
        return (h - rhs.h) * 60 + m - rhs.m;
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
      int occupied_seat_index = -1;
      // on the i-th day, from the j-th to the j+1-th station
      // date_num * (station_num - 1)
      // stored in another file
      // occupied_seat[date_index][station_index] =
      // (occupied_seat_index * 100 * 100 + date_index * 100 + station_index) * sizeof(int)
      int price[100] = {0};
      time start_time;
      int travel_time[100] = {0};// station_num - 1
      int stopover_time[100] = {0};// station_num - 2
      date start_date;
      int date_num = 0;
      char type = '\0';
      bool released = false;
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

    };

    class EdgeInfo {
    public:
      station_name from, to;
      time start_time;
      time arrive_time;
      // may exceed 24, and means the next day or later
      int travel_time = 0;
      int price = 0;
      train_id t_id;

      EdgeInfo() = default;

      bool operator<(const EdgeInfo &rhs) const {
        if (from != rhs.from) return from < rhs.from;
        return t_id < rhs.t_id;
      }

      bool operator==(const EdgeInfo &rhs) const {
        return from == rhs.from
               && t_id == rhs.t_id;
      }
    };


    class Train {
    public:
      unique_BlockRiver<train_id, TrainInfo, 16, 128, 56, 2000> train_list;
      BlockRiver<station_name, EdgeInfo, 22, 22, 10, 100> edge_list;
      Seat seat_list;
      // insert into train_list and seat_list to initialize
      // insert into edge_list when release

      Train() : train_list("2train"), edge_list("3edge") {}

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
          }
        }
        TrainInfo *t = train_list.find(tmp.t_id);
        if (t != nullptr) {
          error("train id duplicated");
        }
        ss.str(stations);
        ss.clear();
        int cnt = 0;
        while (getline(ss, op, '|')) {
          tmp.stations[cnt++] = op;
        }
        ss.str(prices);
        ss.clear();
        cnt = 0;
        tmp.price[0] = 0;
        while (getline(ss, op, '|')) {
          tmp.price[cnt + 1] = tmp.price[cnt] + std::stoi(op);
          ++cnt;
        }
        ss.str(travel_times);
        ss.clear();
        cnt = 0;
        while (getline(ss, op, '|')) {
          tmp.travel_time[cnt++] = std::stoi(op);
        }
        ss.str(stopover_times);
        ss.clear();
        if (tmp.station_num > 2) {
          cnt = 0;
          while (getline(ss, op, '|')) {
            tmp.stopover_time[cnt++] = std::stoi(op);
          }
        }
        ss.str(sale_date);
        ss.clear();
        if (sale_date.size() != 11 || sale_date[5] != '|') {
          error("invalid_date");
        }
        std::string s = sale_date.substr(0, 5);
        std::string e = sale_date.substr(6, 5);
        tmp.start_date = date(s);
        tmp.date_num = date(e) - tmp.start_date + 1;
        tmp.occupied_seat_index = seat_list.train_num + 1;
        train_list.insert(tmp.t_id, tmp);
        seat_list.add_new_train(tmp.date_num, tmp.station_num - 1, tmp.seat_num);
      }

      void release_train(const train_id &t_id) {
        TrainInfo *t = train_list.find(t_id);
        TrainInfo &tmp = *t;
        if (tmp.released) {
          error("train has been released");
        }
        tmp.released = true;
        time cur_time = tmp.start_time;
        for (int i = 0; i < tmp.station_num - 1; i++) {
          EdgeInfo edge;
          edge.from = tmp.stations[i];
          edge.to = tmp.stations[i + 1];
          edge.start_time = cur_time;
          edge.arrive_time = cur_time + tmp.travel_time[i];
          edge.travel_time = tmp.travel_time[i];
          edge.price = tmp.price[i + 1] - tmp.price[i];
          edge.t_id = t_id;
          edge_list.insert(edge.from, edge);
          cur_time += tmp.travel_time[i] + tmp.stopover_time[i];
        }
        EdgeInfo edge;
        edge.from = tmp.stations[tmp.station_num - 1];
        edge.start_time = cur_time;
        edge.t_id = t_id;
        edge_list.insert(edge.from, edge);
      }

      void delete_train(const train_id &t_id) {
        TrainInfo *t = train_list.find(t_id);
        if (t->released) {
          error("train has been released");
        }
        train_list.remove(t_id);
      }

      void query_train(const train_id &id, const date &d) {
        TrainInfo *t = train_list.find(id);
        print_train(*t, d);
      }

      void print_train(const TrainInfo &tr, date d) {
        if (d < tr.start_date || tr.start_date + tr.date_num - 1 < d) {
          error("train not available on this date");
        }
        std::cout << tr.t_id << ' ' << tr.type << '\n';
        time tmp = tr.start_time;
        int date_index = d - tr.start_date;
        SeatInfo &tr_seat = seat_list.get_train_date(tr.occupied_seat_index, date_index);

        for (int i = 0; i < tr.station_num; i++) {
          std::cout << tr.stations[i] << ' ';
          if (i == 0) {
            std::cout << "xx-xx xx:xx -> ";
          } else {
            std::cout << d << ' ' << tmp << " -> ";
            if (i != tr.station_num - 1) {
              tmp += tr.stopover_time[i - 1];
              if (tmp.h >= 24) {
                tmp.h -= 24;
                d += 1;
              }
            }
          }
          if (i == tr.station_num - 1) {
            std::cout << "xx-xx xx:xx";
          } else {
            std::cout << d << ' ' << tmp;
            tmp += tr.travel_time[i];
            if (tmp.h >= 24) {
              tmp.h -= 24;
              d += 1;
            }
          }
          std::cout << ' ' << tr.price[i] << ' ';
          if (i != tr.station_num - 1) {
            std::cout << tr_seat.seat[i] << '\n';
          } else {
            std::cout << 'x' << '\n';
          }
        }
      }

      void log_train(std::ofstream &os, const TrainInfo &tr, date d) {
        if (d < tr.start_date || tr.start_date + tr.date_num - 1 < d) {
          error("train not available on this date");
        }
        os << tr.t_id << ' ' << tr.type << '\n';
        time tmp = tr.start_time;
        int date_index = d - tr.start_date;
        SeatInfo &tr_seat = seat_list.get_train_date(tr.occupied_seat_index, date_index);
        for (int i = 0; i < tr.station_num; i++) {
          os << tr.stations[i] << ' ';
          if (i == 0) {
            os << "xx-xx xx:xx -> ";
          } else {
            os << d << ' ' << tmp << " -> ";
            if (i != tr.station_num - 1) {
              tmp += tr.stopover_time[i - 1];
              if (tmp.h >= 24) {
                tmp.h -= 24;
                d += 1;
              }
            }
          }
          if (i == tr.station_num - 1) {
            os << "xx-xx xx:xx";
          } else {
            os << d << ' ' << tmp;
            tmp += tr.travel_time[i];
            if (tmp.h >= 24) {
              tmp.h -= 24;
              d += 1;
            }
          }
          os << ' ' << tr.price[i] << ' ';
          if (i != tr.station_num - 1) {
            os << tr_seat.seat[i] << '\n';
          } else {
            os << 'x' << '\n';
          }
        }
      }

      struct query_info {
        train_id t_id;
        station_name from, to;
        date s_d, e_d;
        time s_t, e_t;
        int price = 0;
        int time_interval = 0;
        int vacant_seat = 0;

        friend std::ostream &operator<<(std::ostream &os, const query_info &q) {
          os << q.t_id << ' ' << q.from << ' ' << q.s_d << ' ' << q.s_t << " -> " << q.to << ' ' << q.e_d << ' '
             << q.e_t << ' ' << q.price << ' ' << q.vacant_seat;
          return os;
        }

        inline int interval() const {
          int tmp = e_d - s_d;
          time t1 = s_t, t2 = e_t;
          if (t2 < t1) {
            --tmp;
            t2.h += 24;
          }
          return tmp * 24 * 60 + (t2 - t1);
        }
      };

      struct transfer_info {
        query_info q1, q2;

        friend std::ostream &operator<<(std::ostream &os, const transfer_info &q) {
          os << q.q1 << '\n' << q.q2;
          return os;
        }

        inline int price() const {
          return q1.price + q2.price;
        }

        inline int interval() const {
          int tmp = q2.e_d - q1.s_d;
          time t1 = q1.s_t, t2 = q2.e_t;
          if (t2 < t1) {
            --tmp;
            t2.h += 24;
          }
          return tmp * 24 * 60 + (t2 - t1);
        }
      };

      void query_ticket(const station_name &s,
                        const station_name &t,
                        const date &d,
                        bool is_time) {
        vector<pair<train_id, time>> res_train;
        {
          vector<EdgeInfo *> e = edge_list.find(s);
          vector<EdgeInfo *> f = edge_list.find(t);
          if (e.size() == 0 || f.size() == 0) {
            error("no such station included in released trains");
          }
          int p1 = 0, p2 = 0;

          while (p1 < e.size() && p2 < f.size()) {
            while (p1 < e.size() && e[p1]->t_id < f[p2]->t_id) {
              ++p1;
            }
            if (p1 >= e.size()) break;
            if (e[p1]->t_id == f[p2]->t_id) {
              res_train.push_back(pair(e[p1]->t_id, e[p1]->start_time));
              ++p1, ++p2;
            }
            if (p1 >= e.size() || p2 >= f.size()) break;
            while (p2 < f.size() && f[p2]->t_id < e[p1]->t_id) {
              ++p2;
            }
            if (p1 == e.size() || p2 >= f.size()) break;
            if (e[p1]->t_id == f[p2]->t_id) {
              res_train.push_back(pair(e[p1]->t_id, e[p1]->start_time));
              ++p1, ++p2;
            }
          }
        }

        int i = 0;
        vector<query_info> res;
        while (i < res_train.size()) {
          TrainInfo *tr = train_list.find(res_train[i].first);

          TrainInfo &tmp = *tr;
          date origin_start_date = d - res_train[i].second.h / 24;
          if (origin_start_date < tmp.start_date || tmp.start_date + tmp.date_num - 1 < origin_start_date) {
            i++;
            continue;
          }// not in the sale date

          query_info q;
          int start = -1, end = -1;
          int time_interval = 0;
          int seat_num = 0x3fffffff;
          int date_index = origin_start_date - tmp.start_date;
          SeatInfo &tr_seat = seat_list.get_train_date(tmp.occupied_seat_index, date_index);

          for (int j = 0; j < tmp.station_num - 1; j++) {
            if (tmp.stations[j] == s) {
              start = j;
            } else if (tmp.stations[j] == t) {
              end = j;
            }// match
            if (start != -1 && end == -1) {
              time_interval += tmp.travel_time[j] + tmp.stopover_time[j];
              seat_num = std::min(seat_num, tr_seat.seat[j]);
            }// mount the time, price and min seat_num
            if (start != -1 && end != -1) {
              break;
            }
          }
          if (tmp.stations[tmp.station_num - 1] == t) {
            end = tmp.station_num - 1;
          }
          int price = tmp.price[end] - tmp.price[start];

          if (start == -1 || end == -1 || start >= end) {
            i++;
            continue;
          } else {
            time_interval -= tmp.stopover_time[end - 1];
            q.t_id = tmp.t_id;
            q.from = s;
            q.to = t;
            q.s_d = d;
            q.s_t = res_train[i].second;
            q.s_t.h %= 24;
            q.e_t = res_train[i].second + time_interval;
            q.e_d = origin_start_date + q.e_t.h / 24;
            q.e_t.h %= 24;

            q.e_t.h %= 24;
            q.price = price;
            q.time_interval = time_interval;
            q.vacant_seat = seat_num;
            res.push_back(q);
            ++i;
          }
        }
        if (is_time) {
          sort(res.begin(), res.end() - 1, [](const query_info &a, const query_info &b) {
            return a.interval() < b.interval()
                   || (a.interval() == b.interval() && a.t_id < b.t_id);
          });
        } else {
          sort(res.begin(), res.end() - 1, [](const query_info &a, const query_info &b) {
            return a.price < b.price ||
                   (a.price == b.price && a.t_id < b.t_id);
          });
        }
        std::cout << res.size() << '\n';
        for (auto &it: res) {
          std::cout << it << '\n';
        }
      }

      struct candidate {
        train_id t_id;
        station_name sta;
        int from = -1, to = -1;
        date s_d, e_d;
        time s_t, e_t;
        int price = 0;
        int time_interval = 0;
        int occupied_seat_index;
      };

      void query_transfer(const station_name &s,
                          const station_name &t,
                          const date &d,
                          bool is_time) {
        vector<EdgeInfo *> e = edge_list.find(s);
        vector<EdgeInfo *> f = edge_list.find(t);
        if (e.size() == 0 || f.size() == 0) {
          error("no such station included in released trains");
        }

        vector<transfer_info> result;
        vector<candidate> pos_1;

        for (auto &i: e) {
          train_id t1 = i->t_id;
          TrainInfo *tr1 = train_list.find(t1);
          TrainInfo &tmp1 = *tr1;
          date origin_start_date = d - i->start_time.h / 24;
          if (origin_start_date < tmp1.start_date || tmp1.start_date + tmp1.date_num - 1 < origin_start_date) {
            continue;
          }// not in the sale date
          int st1 = -1;
          candidate tmp;
          for (int j = 0; j < tmp1.station_num; ++j) {
            if (st1 != -1) {
              pos_1.push_back(tmp);
              tmp.time_interval += tmp1.stopover_time[j - 1] + tmp1.travel_time[j];
              tmp.e_t += tmp1.stopover_time[j - 1] + tmp1.travel_time[j];
              ++tmp.to;
              tmp.sta = tmp1.stations[tmp.to];
              tmp.price = tmp1.price[j + 1] - tmp1.price[st1];
            }
            if (tmp1.stations[j] == s) {
              st1 = j;
              tmp.t_id = t1;
              tmp.s_d = tmp1.start_date;
              tmp.e_d = tmp1.start_date + (tmp1.date_num - 1);
              tmp.s_t = i->start_time;
              tmp.e_t = i->start_time + tmp1.travel_time[j];
              // from s_d to e_d, s_t to e_t every day
              tmp.time_interval = tmp1.travel_time[j];
              tmp.from = j;
              tmp.to = j + 1;
              tmp.occupied_seat_index = tmp1.occupied_seat_index;
              tmp.sta = tmp1.stations[tmp.to];
              tmp.price = tmp1.price[j + 1] - tmp1.price[j];
            }
          }
        }

        vector<candidate> pos_2;
        for (auto &j: f) {
          train_id t2 = j->t_id;
          TrainInfo *tr2 = train_list.find(t2);
          TrainInfo &tmp2 = *tr2;
          int st2 = -1;
          candidate tmp;
          for (int k = tmp2.station_num - 1; k >= 0; --k) {
            if (st2 != -1) {
              pos_2.push_back(tmp);
              if (k == 0) break;
              tmp.time_interval += tmp2.stopover_time[k - 1] + tmp2.travel_time[k - 1];
              tmp.s_t -= tmp2.stopover_time[k - 1] + tmp2.travel_time[k - 1];
              while (tmp.s_t.h < 0) {
                tmp.s_t.h += 24;
                tmp.s_d -= 1;
              }
              --tmp.from;
              tmp.sta = tmp2.stations[tmp.from];
              tmp.price = tmp2.price[st2] - tmp2.price[k - 1];
            }
            if (tmp2.stations[k] == t && k != 0) {
              st2 = k;
              tmp.t_id = t2;
              tmp.s_d = tmp2.start_date;
              tmp.e_d = tmp2.start_date + (tmp2.date_num - 1);
              tmp.e_t = j->start_time - tmp2.stopover_time[k - 1];
              tmp.s_t = j->start_time - (tmp2.stopover_time[k - 1] + tmp2.travel_time[k - 1]);
              // from s_d to e_d, s_t to e_t every day
              tmp.time_interval = tmp2.travel_time[k - 1];
              tmp.from = k - 1;
              tmp.to = k;
              tmp.occupied_seat_index = tmp2.occupied_seat_index;
              tmp.sta = tmp2.stations[tmp.from];
              tmp.price = tmp2.price[k] - tmp2.price[k - 1];
            }
          }
        }

        for (auto &i: pos_1) {
          for (auto &j: pos_2) {
            if (i.sta != j.sta || i.t_id == j.t_id)
              continue;
            date origin_date_1 = d - i.s_t.h / 24;
            int delta = 0;
            time s2 = j.s_t, e1 = i.e_t;
            while (s2 < e1) {
              ++delta;
              s2.h += 24;
            }
            while (!(s2 < e1)) {
              --delta;
              s2.h -= 24;
            }
            ++delta;
            date origin_date_2;
            if (delta > 0) origin_date_2 = origin_date_1 + delta;
            else origin_date_2 = origin_date_1 - (-delta);
            if (j.e_d < origin_date_2)
              continue;
            else if (origin_date_2 < j.s_d)
              origin_date_2 = j.s_d;
            // later than the first train is all possible
            transfer_info res;
            query_info &q1 = res.q1;
            q1.t_id = i.t_id;
            q1.from = s;
            q1.to = i.sta;
            q1.s_d = d;
            q1.s_t = i.s_t;
            q1.s_t.h %= 24;
            q1.e_d = origin_date_1 + i.e_t.h / 24;
            q1.e_t = i.e_t;
            q1.e_t.h %= 24;
            q1.price = i.price;
            q1.time_interval = i.time_interval;
            q1.vacant_seat = seat_list.search_seat(i.occupied_seat_index,
                                                   origin_date_1 - i.s_d, i.from, i.to);
            query_info &q2 = res.q2;
            q2.t_id = j.t_id;
            q2.from = j.sta;
            q2.to = t;
            q2.s_d = origin_date_2 + j.s_t.h / 24;
            q2.s_t = j.s_t;
            q2.s_t.h %= 24;
            q2.e_d = origin_date_2 + j.e_t.h / 24;
            q2.e_t = j.e_t;
            q2.e_t.h %= 24;
            q2.price = j.price;
            q2.time_interval = j.time_interval;
            q2.vacant_seat = seat_list.search_seat(j.occupied_seat_index,
                                                   origin_date_2 - j.s_d, j.from, j.to);
            result.push_back(res);
          }
        }

        if (result.empty()) {
          std::cout << "0\n";
          return;
        }

        if (is_time) {
          sort(result.begin(), result.end() - 1, [](const transfer_info &a, const transfer_info &b) {
            return a.interval() < b.interval()
                   || (a.interval() == b.interval() && a.price() < b.price())
                   || (a.interval() == b.interval() && a.price() == b.price() && a.q1.t_id < b.q1.t_id)
                   || (a.interval() == b.interval() && a.price() == b.price() && a.q1.t_id == b.q1.t_id &&
                       a.q2.t_id < b.q2.t_id);
          });
        } else {
          sort(result.begin(), result.end() - 1, [](const transfer_info &a, const transfer_info &b) {
            return a.price() < b.price()
                   || (a.price() == b.price() && a.interval() < b.interval())
                   || (a.price() == b.price() && a.interval() == b.interval() && a.q1.t_id < b.q1.t_id)
                   || (a.price() == b.price() && a.interval() == b.interval() && a.q1.t_id == b.q1.t_id &&
                       a.q2.t_id < b.q2.t_id);
          });
        }

//        if (timestamp == 1729119) {
//          for (auto &i: result) {
//            std::cout << i << '\n';
//          }
//        }
        std::cout << result[0] << '\n';
      }

      pair<int, int> buy_ticket(time &st, time &ed, const train_id &t_id,
                                const station_name &from,
                                const station_name &to,
                                const date &d,
                                int num, bool queue = false) {
        TrainInfo *t = train_list.find(t_id);
        TrainInfo &tmp = *t;
        if (!tmp.released) {
          error("train not released");
        }
        if (num > tmp.seat_num) {
          error("seat insatisfiable");
        }
        int date_index = d - tmp.start_date;
        int start = -1, end = -1;
        time cur_time = tmp.start_time;
        for (int i = 0; i < tmp.station_num - 1; i++) {
          if (tmp.stations[i] == from) {
            start = i;
            st = cur_time;
          }
          cur_time += tmp.travel_time[i];
          if (tmp.stations[i + 1] == to) {
            end = i + 1;
            ed = cur_time;
          }
          if (start != -1 && end != -1) {
            break;
          }
          cur_time += tmp.stopover_time[i];
        }
        int price = tmp.price[end] - tmp.price[start];
        date_index -= st.h / 24;
        if (date_index < 0 || date_index >= tmp.date_num) {
          error("invalid date");
        }
        date origin_date = d - st.h / 24;
        ed.h -= st.h / 24 * 24;
        st.h %= 24;
        if (start == -1 || end == -1 || start >= end) {
          error("end station not in the train route");
        }


        try {
          seat_list.buy_seat(tmp.occupied_seat_index, date_index, start, end, num);
        } catch (const ErrorException &e) {
          // seat not enough
          if (!queue) {
            return pair(-1, -1);
            // fail
          } else {
            return pair(price, -1);
            // queue
          }
        }
        if (t_id == train_id("LeavesofGrass") && origin_date == date{7, 9}) {
          std::ofstream log("log.txt", std::ios::app);
          if (log.fail()) {
            std::cout << "log file open failed" << std::endl;
          }
          log << timestamp << ": buy success\n";
          log_train(log, tmp, origin_date);
          log.close();
        }
        return pair(price, 0);
      }

      /// return true if the train is satisfiable
      /// and then the ticket is bought
      bool is_satisfiable(const train_id &t_id,
                          const station_name &from,
                          const station_name &to,
                          const date &d,
                          int num) {
        TrainInfo *t = train_list.find(t_id);
        time st, ed;
        TrainInfo &tmp = *t;
        if (!tmp.released) {
          return false;
        }
        int date_index = d - tmp.start_date;
        int start = -1, end = -1;
        time cur_time = tmp.start_time;
        for (int i = 0; i < tmp.station_num - 1; i++) {
          if (tmp.stations[i] == from) {
            start = i;
            st = cur_time;
          }
          cur_time += tmp.travel_time[i];
          if (tmp.stations[i] == to) {
            end = i;
            ed = cur_time;
          }
          cur_time += tmp.stopover_time[i];
          if (start != -1 && end != -1) {
            break;
          }
        }
        if (tmp.stations[tmp.station_num - 1] == to) {
          end = tmp.station_num - 1;
          ed = cur_time;
        }
        date_index -= st.h / 24;
        date origin_date = d - st.h / 24;
        if (start == -1 || end == -1 || start >= end) {
          error("end station not in the train route");
        }
        try {
          seat_list.buy_seat(tmp.occupied_seat_index, date_index, start, end, num);
        } catch (const ErrorException &e) {
          return false;
        }
        if (t_id == train_id("LeavesofGrass") && origin_date == date{7, 9}) {
          std::ofstream log("log.txt", std::ios::app);
          if (log.fail()) {
            std::cout << "log file open failed" << std::endl;
          }
          log << timestamp << ": pending->success\n";
          log_train(log, tmp, origin_date);
          log.close();
        }
        return true;
      }

      void refund_ticket(const train_id &t_id,
                         const station_name &from,
                         const station_name &to,
                         const date &d,
                         int num) {
        TrainInfo *t = train_list.find(t_id);
        TrainInfo &tmp = *t;
        int date_index = d - tmp.start_date;
        int start = -1, end = -1;
        time cur_time = tmp.start_time;
        time st;
        for (int i = 0; i < tmp.station_num - 1; i++) {
          if (tmp.stations[i] == from) {
            start = i;
            st = cur_time;
          } else if (tmp.stations[i] == to) {
            end = i;
          }
          cur_time += tmp.travel_time[i];
          if (start != -1 && end != -1) {
            break;
          }
          cur_time += tmp.stopover_time[i];
        }
        if (tmp.stations[tmp.station_num - 1] == to) {
          end = tmp.station_num - 1;
        }
        if (start == -1 || end == -1 || start >= end) {
          error("invalid_station");
        }
        date_index -= st.h / 24;// refund on the origin date
        date origin_date = d - st.h / 24;
        seat_list.refund_seat(tmp.occupied_seat_index, date_index, start, end, num);
        if (t_id == train_id("LeavesofGrass") && origin_date == date{7, 9}) {
          std::ofstream log("log.txt", std::ios::app);
          if (log.fail()) {
            std::cout << "log file open failed" << std::endl;
          }
          log << timestamp << ": refund\n";
          log_train(log, tmp, origin_date);
          log.close();
        }
      }

    };
}

#endif
