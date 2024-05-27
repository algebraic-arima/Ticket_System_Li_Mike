#ifndef BPTREE_UTILITY_H
#define BPTREE_UTILITY_H
#pragma once

#include <iostream>
#include <cstring>
#include <memory>

namespace arima_kana {

    template<int length>
    class m_string {
    public:
      char id[length]{};

      m_string() = default;

      explicit m_string(const char _key[]) {
        strcpy(id, _key);
      }

      explicit m_string(const std::string &rhs) {
        strcpy(id, rhs.c_str());
      }

      m_string<length> &operator=(const m_string<length> &rhs) {
        if (this == &rhs) return *this;
        strcpy(id, rhs.id);
        return *this;
      }

      m_string &operator=(const std::string &rhs) {
        strcpy(id, rhs.c_str());
        return *this;
      }

      m_string &operator=(const char _key[]) {
        strcpy(id, _key);
        return *this;
      }

      bool operator==(const m_string &rhs) const {
        return (strcmp(id, rhs.id) == 0);
      }

      bool operator!=(const m_string &rhs) const {
        return (strcmp(id, rhs.id) != 0);
      }

      bool operator<(const m_string &rhs) const {
        return (strcmp(id, rhs.id) < 0);
      }
    };

    template<int length>
    std::ostream &operator<<(std::ostream &os, const m_string<length> m) {
      os << m.id;
      return os;
    }

    template<int length>
    std::istream &operator>>(std::istream &is, m_string<length> &m) {
      is >> m.id;
      return is;
    }

    template<int length>
    unsigned long long hash(const m_string<length> &key) {
      unsigned long long h = 0;
      for (int i = 0; i < length; i++) {
        h = h * 1471 + key.id[i];
      }
      return h;
    }

    template<class T1, class T2>
    class pair {
    public:
      T1 first;   // first element
      T2 second;  // second element

      constexpr pair() = default;

      constexpr pair(const pair &other) = default;

      constexpr pair(pair &&other) = default;

      pair &operator=(const pair &other) = default;

      template<class U1 = T1, class U2 = T2>
      constexpr pair(U1 &&x, U2 &&y)
              : first(std::forward<U1>(x)), second(std::forward<U2>(y)) {}

      template<class U1, class U2>
      constexpr pair(const pair<U1, U2> &other)
              : first(other.first), second(other.second) {}

      template<class U1, class U2>
      constexpr pair(pair<U1, U2> &&other)
              : first(std::move(other.first)), second(std::move(other.second)) {}

      bool operator<(const pair &other) const {
        return first < other.first || (first == other.first && second < other.second);
      }

      bool operator==(const pair &other) const {
        return first == other.first && second == other.second;
      }

      bool operator!=(const pair &other) const {
        return first != other.first || second != other.second;
      }


    };

    template<class T1, class T2>
    std::ostream &operator<<(std::ostream &os, const pair<T1, T2> &p) {
      os << '(' << p.first << ',' << p.second << ')';
      return os;
    }

    template<class T1, class T2>
    pair(T1, T2) -> pair<T1, T2>;

    template<typename T>
    struct allocator {
      T *allocate(size_t size) {
        return (T *) malloc(sizeof(T) * size);
      }

      void deallocate(void *p) {
        free(p);
      }

      void construct(T *p, const T &val) {
        new(p) T(val);
      }

      void destroy(T *p) {
        p->~T();
      }
    };

    template<class T, class _alloc=std::allocator<T>>
    class vector {
      T *data;
      size_t _size;
      size_t _capacity;
      _alloc alloc;

      void double_space() {
        _capacity *= 2;
        T *tmp = alloc.allocate(_capacity);
        for (size_t i = 0; i < _size; i++) {
          alloc.construct(tmp + i, data[i]);
        }
        alloc.deallocate(data, _capacity / 2);
        data = tmp;
      }

    public:
      explicit vector(size_t cap = 32) : _size(0), _capacity(cap) {
        data = alloc.allocate(_capacity);
      }

      explicit vector(const T &val, size_t cap = 32) : _size(cap), _capacity(cap) {
        data = alloc.allocate(_capacity);
        for (size_t i = 0; i < _size; i++) {
          alloc.construct(data + i, val);
        }
      }

      void push_back(const T &val) {
        if (_size == _capacity) {
          double_space();
        }
        alloc.construct(data + _size, val);
        _size++;
      }

      size_t size() const {
        return _size;
      }

      T &operator[](size_t idx) {
        return data[idx];
      }

      T &back() {
        return data[_size - 1];
      }

      T &front() {
        return data[0];
      }

      bool empty() {
        return _size == 0;
      }

      void pop_back() {
        if (_size != 0) {
          alloc.destroy(data + _size - 1);
          _size--;
        }
//        if (_size < _capacity / 3) {
//          _capacity /= 2;
//          T *tmp = new T[_capacity];
//          for (size_t i = 0; i < _size; i++) {
//            tmp[i] = data[i];
//          }
//          delete[] data;
//          data = tmp;
//        }
      }

      void clear() {
        for (size_t i = 0; i < _size; i++) {
          alloc.destroy(data + i);
        }
        alloc.deallocate(data, _capacity);
        _size = 0;
        _capacity = 10;
        data = alloc.allocate(_capacity);
      }

      void resize(size_t new_size) {
        if (new_size > _size) {
          if (new_size > _capacity) {
            size_t old_capacity = _capacity;
            while (_capacity < new_size) {
              _capacity *= 2;
            }
            T *tmp = alloc.allocate(_capacity);
            for (size_t i = 0; i < _size; i++) {
              alloc.construct(tmp + i, data[i]);
            }
            alloc.deallocate(data, old_capacity);
            data = tmp;
          }
          _size = new_size;
        } else {
          for (size_t i = new_size; i < _size; i++) {
            alloc.destroy(data + i);
          }
          _size = new_size;
        }
      }

      ~vector() {
        for (size_t i = 0; i < _size; i++) {
          alloc.destroy(data + i);
        }
        alloc.deallocate(data, _capacity);
      }

      class iterator {
        T *ptr;
      public:
        iterator(T *p) : ptr(p) {}

        iterator operator++() {
          ptr++;
          return *this;
        }

        iterator operator++(int) {
          iterator tmp = *this;
          ptr++;
          return tmp;
        }

        iterator operator--() {
          ptr--;
          return *this;
        }

        iterator operator--(int) {
          iterator tmp = *this;
          ptr--;
          return tmp;
        }

        iterator operator+(int n) {
          return iterator(ptr + n);
        }

        iterator operator-(int n) {
          return iterator(ptr - n);
        }

        T &operator*() {
          return *ptr;
        }

        bool operator==(const iterator &rhs) {
          return ptr == rhs.ptr;
        }

        bool operator!=(const iterator &rhs) {
          return ptr != rhs.ptr;
        }

        bool operator<(const iterator &rhs) {
          return ptr < rhs.ptr;
        }
      };

      iterator begin() {
        return iterator(data);
      }

      iterator end() {
        return iterator(data + _size);
      }

      friend void sort(iterator l, iterator r, bool (*cmp)(const T &, const T &)) {
        if (r < l) return;
        iterator i = l, j = r;
        T mid = *l;
        while (i != j) {
          while (i != j && !cmp(*j, mid)) j--;
          if (i != j) *i = *j;
          while (i != j && cmp(*i, mid)) i++;
          if (i != j) *j = *i;
        }
        *i = mid;
        sort(l, i - 1, cmp);
        sort(i + 1, r, cmp);
      }

    };
}


#endif