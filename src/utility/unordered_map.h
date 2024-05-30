#ifndef TICKET_SYSTEM_UNORDERED_MAP_H
#define TICKET_SYSTEM_UNORDERED_MAP_H

#include <iostream>
#include "utility.h"

namespace arima_kana {

    template<size_t _cap>
    class m_hash {
    public:
      int operator()(const size_t &key) const {
        return int(key * key / 1000) % _cap;
      }
    };

    template<class T, size_t _cap, class hash = m_hash<_cap>>
    class unordered_map {
    public:
      struct Node {
        Node *next = nullptr;
        size_t key = 0;
        T value;
      };

      Node *data[_cap + 1];
      int size = 0;

      void insert(const size_t &key, const T &value) {
        int h = hash()(key);
        Node *p = new Node{data[h], key, value};
        data[h] = p;
        ++size;
      }

      T *find(const size_t &key) {
        int h = hash()(key);
        Node *p = data[h];
        while (p) {
          if (p->key == key) return &p->value;
          p = p->next;
        }
        return nullptr;
      }

      void erase(const size_t &key) {
        int h = hash()(key);
        Node *p = data[h];
        if (p->key == key) {
          data[h] = p->next;
          delete p;
          --size;
          return;
        }
        while (p->next) {
          if (p->next->key == key) {
            Node *q = p->next;
            p->next = q->next;
            delete q;
            --size;
            return;
          }
          p = p->next;
        }
      }

      void clear() {
        for (int i = 0; i < _cap; ++i) {
          Node *p = data[i];
          while (p) {
            Node *q = p;
            p = p->next;
            delete q;
          }
          data[i] = nullptr;
        }
        size = 0;
      }

      ~unordered_map() {
        clear();
      }

    };
}

#endif
