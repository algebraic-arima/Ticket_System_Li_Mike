#ifndef TICKET_SYSTEM_TRAIN_H
#define TICKET_SYSTEM_TRAIN_H

#include <iostream>
#include <fstream>
#include <sstream>
#include "utility.h"
#include "BlockRiver.h"
#include "unique_BlockRiver.h"
#include "seat.h"
#include "unique_ind_ext_BPtree.h"


namespace arima_kana {
    extern int timestamp;

    typedef m_string<22> train_id;
    typedef m_string<44> station_name;

    class date {
    public:
      // year 2024
      int dd; // date_index from 2024-06-01
      constexpr static int month_days[13] =
              {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

      explicit date() : dd(-1) {}

      explicit date(int mm, int dd) : dd(dd) {
        for (int i = 6; i < mm; i++) {
          dd += month_days[i];
        }
      }

      explicit date(const std::string &s) {
        if (s.size() != 5 || s[2] != '-') {
          error("invalid_date");
        }
        int m = (s[0] - '0') * 10 + s[1] - '0';
        dd = (s[3] - '0') * 10 + s[4] - '0';
        if (m == 6) {
          --dd;
        } else if (m == 7) {
          dd += 29;
        } else if (m == 8) {
          dd += 60;
        }
      }

      friend std::ostream &operator<<(std::ostream &os, const date &da) {
        if (da.dd >= 92) {
          os << "09-";
          if (da.dd - 91 < 10) os << '0';
          os << da.dd - 91;
          return os;
        } else if (da.dd >= 61 && da.dd <= 91) {
          os << "08-";
          if (da.dd - 60 < 10) os << '0';
          os << da.dd - 60;
        } else if (da.dd >= 30 && da.dd <= 60) {
          os << "07-";
          if (da.dd - 29 < 10) os << '0';
          os << da.dd - 29;
        } else {
          os << "06-";
          if (da.dd + 1 < 10) os << '0';
          os << da.dd + 1;
        }
        return os;
      }

      friend std::istream &operator>>(std::istream &is, date &da) {
        std::string s;
        is >> s;
        if (s.size() != 5 || s[2] != '-') {
          error("invalid_date");
        }
        int m = (s[0] - '0') * 10 + s[1] - '0';
        da.dd = (s[3] - '0') * 10 + s[4] - '0';
        if (m == 6) {
          --da.dd;
        } else if (m == 7) {
          da.dd += 29;
        } else if (m == 8) {
          da.dd += 60;
        }
        return is;
      }

      bool operator<(const date &rhs) const {
        return dd < rhs.dd;
      }

      int operator-(const date &rhs) const {
        return dd - rhs.dd;
      }

      date &operator+=(int x) {
        dd += x;
        return *this;
      }

      date operator+(int x) const {
        date tmp = *this;
        tmp += x;
        return tmp;
      }

      date &operator-=(int x) {
        dd -= x;
        return *this;
      }

      date operator-(int x) const {
        date tmp = *this;
        tmp -= x;
        return tmp;
      }

      bool operator==(const date &rhs) const {
        return dd == rhs.dd;
      }

      bool operator!=(const date &rhs) const {
        return dd != rhs.dd;
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
      // on the day i, from the j-th to the j+1-th station
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
      station_name from;
      time start_time{};
      int station_ind = 0;
      int stop_time = 0;
      date start_date{};
      int duration_date = 0;
      int price_till_now = 0;
      int occupied_seat_index = 0;
      int max_possible_seat = 0;
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
      // block_num, bp_max, bp_min, block_buffer_size
      unique_ind_ext_BPtree<train_id, TrainInfo, 136, 186, 92, 60, 20> train_list;
      BlockRiver<station_name, EdgeInfo, 26, 26, 12, 200, 200> edge_list;
      //tree: (44+108)*26*100=400kb
      //data: (44+108)*26*100=400kb
      unique_BlockRiver<pair<station_name, train_id>, EdgeInfo, 24, 62, 30, 200, 200> station_train_to_ind;
      //tree: (44+22)*62*100=400kb
      //data: (44+22+108)*24*100=400kb

      Seat seat_list;
      // insert into train_list and seat_list to initialize
      // insert into edge_list when release

      Train() : train_list("2train"), edge_list("3edge"),
                station_train_to_ind("7statrtoi") {}

      bool add_train(const std::string &param) {
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
//          error("train id duplicated");
          return false;
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
//          error("invalid_date");
          return false;
        }
        std::string s = sale_date.substr(0, 5);
        std::string e = sale_date.substr(6, 5);
        tmp.start_date = date(s);
        tmp.date_num = date(e) - tmp.start_date + 1;
        tmp.occupied_seat_index = seat_list.train_num + 1;
        train_list.insert(tmp.t_id, tmp);
        seat_list.add_new_train(tmp.start_date.dd, tmp.start_date.dd + tmp.date_num,
                                tmp.station_num - 1, tmp.seat_num);
        return true;
      }

      bool release_train(const train_id &t_id) {
        if (!train_list.release(t_id)) {
//          error("train has been released");
          return false;
        }
        TrainInfo *t = train_list.find(t_id);
        TrainInfo &tmp = *t;
        time cur_time = tmp.start_time;
        EdgeInfo edge;
        edge.start_date = tmp.start_date;
        edge.duration_date = tmp.date_num;
        edge.max_possible_seat = tmp.seat_num;
        edge.occupied_seat_index = tmp.occupied_seat_index;
        int i;
        for (i = 0; i < tmp.station_num - 1; i++) {
          edge.from = tmp.stations[i];
          edge.station_ind = i;
          if (i != 0) {
            edge.stop_time = tmp.stopover_time[i - 1];
          } else {
            edge.stop_time = 0;
          }
          edge.start_time = cur_time;
          edge.t_id = t_id;
          edge.price_till_now = tmp.price[i];
          edge_list.insert(edge.from, edge);
          cur_time += tmp.travel_time[i] + tmp.stopover_time[i];
          station_train_to_ind.insert(pair(tmp.stations[i], t_id), edge);
        }
        edge.from = tmp.stations[i];
        edge.start_time = cur_time;
        edge.station_ind = i;
        edge.stop_time = 0;
        edge.price_till_now = tmp.price[i];
        edge.t_id = t_id;
        edge_list.insert(edge.from, edge);
        station_train_to_ind.insert(pair(tmp.stations[i], t_id), edge);
        return true;
      }

      bool delete_train(const train_id &t_id) {
        if (train_list.is_released(t_id)) {
//          error("train has been released");
          return false;
        }
        train_list.remove(t_id);
        return true;
      }

      void query_train(const train_id &id, const date &d) {
        TrainInfo *t = train_list.find(id);
        print_train(t, d);
      }

      void print_train(TrainInfo *tr, date d) {
        if (d < tr->start_date || tr->start_date + tr->date_num - 1 < d) {
          std::cout << "-1\n";
          return;
        }
        std::cout << tr->t_id << ' ' << tr->type << '\n';
        int date_index = d.dd;
        time tmp = tr->start_time;
        SeatInfo &tr_seat = seat_list.get_train_date(tr->occupied_seat_index, date_index);
        for (int i = 0; i < tr->station_num; i++) {
          std::cout << tr->stations[i] << ' ';
          if (i == 0) {
            std::cout << "xx-xx xx:xx -> ";
          } else {
            std::cout << d << ' ' << tmp << " -> ";
            if (i != tr->station_num - 1) {
              tmp += tr->stopover_time[i - 1];
              if (tmp.h >= 24) {
                tmp.h -= 24;
                d += 1;
              }
            }
          }
          if (i == tr->station_num - 1) {
            std::cout << "xx-xx xx:xx";
          } else {
            std::cout << d << ' ' << tmp;
            tmp += tr->travel_time[i];
            if (tmp.h >= 24) {
              tmp.h -= 24;
              d += 1;
            }
          }
          std::cout << ' ' << tr->price[i] << ' ';
          if (i != tr->station_num - 1) {
            std::cout << tr_seat.seat[i] << '\n';
          } else {
            std::cout << 'x' << '\n';
          }
        }
      }

      struct query_info {
        train_id t_id;
        station_name from, to;
        date s_d, e_d;
        time s_t, e_t;
        int interval = 0;
        int price = 0;
        int vacant_seat = 0;

        friend std::ostream &operator<<(std::ostream &os, const query_info &q) {
          os << q.t_id << ' ' << q.from << ' ' << q.s_d << ' ' << q.s_t << " -> " << q.to << ' ' << q.e_d << ' '
             << q.e_t << ' ' << q.price << ' ' << q.vacant_seat;
          return os;
        }

      };

      struct transfer_info {
        query_info q1, q2;
        int il = 0;
        int price = 0;

        friend std::ostream &operator<<(std::ostream &os, const transfer_info &q) {
          os << q.q1 << '\n' << q.q2;
          return os;
        }

        inline void get() {
          int tmp = q2.e_d - q1.s_d;
          time t1 = q1.s_t, t2 = q2.e_t;
          if (t2 < t1) {
            --tmp;
            t2.h += 24;
          }
          il = tmp * 24 * 60 + (t2 - t1);
          price = q1.price + q2.price;
        }
      };

      void query_ticket(const station_name &s,
                        const station_name &t,
                        const date &d,
                        bool is_time) {
        vector<pair<int, int>> res_train;

        vector<EdgeInfo> e;
        edge_list.find(s, true, e);
        vector<EdgeInfo> f;
        edge_list.find(t, true, f);
        if (e.size() == 0 || f.size() == 0) {
          std::cout << "0\n";
          return;
        }
        int p1 = 0, p2 = 0;

        while (p1 < e.size() && p2 < f.size()) {
          while (p1 < e.size() && e[p1].t_id < f[p2].t_id) {
            ++p1;
          }
          if (p1 >= e.size()) break;
          if (e[p1].t_id == f[p2].t_id) {
            if (e[p1].station_ind < f[p2].station_ind) res_train.push_back(pair(p1, p2));
            ++p1, ++p2;
          }
          if (p1 >= e.size() || p2 >= f.size()) break;
          while (p2 < f.size() && f[p2].t_id < e[p1].t_id) {
            ++p2;
          }
          if (p1 == e.size() || p2 >= f.size()) break;
          if (e[p1].t_id == f[p2].t_id) {
            if (e[p1].station_ind < f[p2].station_ind) res_train.push_back(pair(p1, p2));
            ++p1, ++p2;
          }
        }


        int i = 0;
        vector<query_info> res;
        while (i < res_train.size()) {
          EdgeInfo &tmpe = e[res_train[i].first];
          date origin_start_date = d - tmpe.start_time.h / 24;
          if (origin_start_date < tmpe.start_date || tmpe.start_date + tmpe.duration_date - 1 < origin_start_date) {
            i++;
            continue;
          }
          query_info q;
          EdgeInfo &tmpf = f[res_train[i].second];
          int start = tmpe.station_ind, end = tmpf.station_ind;
          int seat_num = seat_list.search_seat(tmpe.occupied_seat_index, origin_start_date.dd, start, end);

          q.t_id = tmpe.t_id;
          q.from = s;
          q.to = t;
          q.s_d = d;
          q.s_t = tmpe.start_time;
          q.s_t.h %= 24;
          q.e_t = tmpf.start_time - tmpf.stop_time;
          q.e_d = origin_start_date + q.e_t.h / 24;
          q.e_t.h %= 24;
          q.price = tmpf.price_till_now - tmpe.price_till_now;
          q.interval = (tmpf.start_time - tmpe.start_time) - tmpf.stop_time;
          q.vacant_seat = seat_num;
          res.push_back(q);
          ++i;
        }
        if (is_time) {
          sort(res.begin(), res.end() - 1, [](const query_info &a, const query_info &b) {
            return a.interval < b.interval
                   || (a.interval == b.interval && a.t_id < b.t_id);
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
        int occupied_seat_index = 0;
      };

      // local departure date
      void query_transfer(const station_name &s,
                          const station_name &t,
                          const date &d,
                          bool is_time) {
        vector<EdgeInfo> e;
        edge_list.find(s, true, e);
        vector<EdgeInfo> f;
        edge_list.find(t, true, f);
        if (e.size() == 0 || f.size() == 0) {
          std::cout << "0\n";
          return;
        }

        vector<transfer_info> result;
        vector<candidate> pos_1;

        TrainInfo *tr1 = train_list.find(e[0].t_id);
        TrainInfo &tmp1 = *tr1;
        for (auto &i: e) {
          train_id &t1 = i.t_id;
          if (i.t_id != tr1->t_id) {
            tr1 = train_list.find(t1);
            tmp1 = *tr1;
          }
          date origin_start_date = d - i.start_time.h / 24;
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
              tmp.s_t = i.start_time;
              tmp.e_t = i.start_time + tmp1.travel_time[j];
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
        TrainInfo *tr2 = train_list.find(f[0].t_id);
        TrainInfo &tmp2 = *tr2;
        for (auto &j: f) {
          train_id &t2 = j.t_id;
          if (j.t_id != tr2->t_id) {
            tr2 = train_list.find(t2);
            tmp2 = *tr2;
          }
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
              tmp.e_t = j.start_time - tmp2.stopover_time[k - 1];
              tmp.s_t = j.start_time - (tmp2.stopover_time[k - 1] + tmp2.travel_time[k - 1]);
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
            q1.vacant_seat = seat_list.search_seat(i.occupied_seat_index,
                                                   origin_date_1.dd, i.from, i.to);
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
            q2.vacant_seat = seat_list.search_seat(j.occupied_seat_index,
                                                   origin_date_2.dd, j.from, j.to);
            res.get();
            result.push_back(res);
          }
        }

        if (result.empty()) {
          std::cout << "0\n";
          return;
        }

        if (is_time) {
          sort(result.begin(), result.end() - 1, [](const transfer_info &a, const transfer_info &b) {
            return a.il < b.il
                   || (a.il == b.il && a.price < b.price)
                   || (a.il == b.il && a.price == b.price && a.q1.t_id < b.q1.t_id)
                   || (a.il == b.il && a.price == b.price && a.q1.t_id == b.q1.t_id &&
                       a.q2.t_id < b.q2.t_id);
          });
        } else {
          sort(result.begin(), result.end() - 1, [](const transfer_info &a, const transfer_info &b) {
            return a.price < b.price
                   || (a.price == b.price && a.il < b.il)
                   || (a.price == b.price && a.il == b.il && a.q1.t_id < b.q1.t_id)
                   || (a.price == b.price && a.il == b.il && a.q1.t_id == b.q1.t_id &&
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

      //local departure date
      pair<int, int> buy_ticket(time &st, time &ed,
                                int &l, int &r,
                                int &tsi,
                                date &origin_d,
                                const train_id &t_id,
                                const station_name &from,
                                const station_name &to,
                                const date &d,
                                int num, bool queue = false) {
        EdgeInfo e = station_train_to_ind.find(pair(from, t_id), true);
        EdgeInfo f = station_train_to_ind.find(pair(to, t_id), true);
        if (e.t_id == train_id() || f.t_id == train_id()) {
          error("no such station included in released trains");
        }
        if (e.t_id != f.t_id) {
          error("no such station included in released trains");
        }
        if (num > e.max_possible_seat) {
          error("seat insatisfiable");
        }

        int date_index = d.dd;
        int start = e.station_ind, end = f.station_ind;
        if (start >= end) {
          error("end station not in the train route");
        }
        date_index -= e.start_time.h / 24;
        if (date_index < e.start_date.dd || date_index >= e.start_date.dd + e.duration_date) {
          error("invalid date");
        }
        st = e.start_time;
        ed = f.start_time - f.stop_time;
        l = start, r = end;
        tsi = e.occupied_seat_index;
        origin_d.dd = date_index;
        int price = f.price_till_now - e.price_till_now;

        try {
          seat_list.buy_seat(e.occupied_seat_index, date_index, start, end, num);
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
//        if (t_id == train_id("LeavesofGrass") && origin_date == date{6, 17}) {
//          std::ofstream log("log.txt", std::ios::app);
//          log << timestamp << ": buy success\n";
//          log_train(log, tmp, origin_date);
//          log.close();
//        }
        return pair(price, 0);
      }

      /// return true if the train is satisfiable
      /// and then the ticket is bought
      bool is_satisfiable(const size_t &t_id,
                          const int &from,
                          const int &to,
                          const date &d,
                          int num) {
        try {
          seat_list.buy_seat(t_id, d.dd, from, to, num);
        } catch (const ErrorException &e) {
          return false;
        }
//        if (t_id == train_id("LeavesofGrass") && origin_date == date{6, 17}) {
//          std::ofstream log("log.txt", std::ios::app);
//          log << timestamp << ": pending->success\n";
//          log_train(log, tmp, origin_date);
//          log.close();
//        }
        return true;
      }

      // origin departure date
      void refund_ticket(const int &t_id,
                         const int &from,
                         const int &to,
                         const date &d,
                         int num) {
        seat_list.refund_seat(t_id, d.dd, from, to, num);
//        if (t_id == train_id("LeavesofGrass") && origin_date == date{6, 17}) {
//          std::ofstream log("log.txt", std::ios::app);
//          log << timestamp << ": refund\n";
//          log_train(log, tmp, origin_date);
//          log.close();
//        }
      }

    };
}

#endif