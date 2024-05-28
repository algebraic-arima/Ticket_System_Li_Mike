#ifndef TICKET_SYSTEM_UNIQUE_IND_EXT_BPTREE_H
#define TICKET_SYSTEM_UNIQUE_IND_EXT_BPTREE_H

#include <iostream>
#include <fstream>
#include "unique_BlockRiver.h"

namespace arima_kana {


    template<class K, class V, size_t block, size_t bp_max, size_t bp_min, size_t buf_max>

    class unique_ind_ext_BPtree {
    public:
      std::string data_name;
      std::fstream data_filer;
      size_t num = 0;
      unique_BlockRiver<K, size_t, block, bp_max, bp_min, buf_max> index_list;
      V *buffer;
      size_t buffer_pos = 0;

      explicit unique_ind_ext_BPtree(const std::string &df) :
              data_name(df + "_data"),
              index_list(df) {
        data_filer.open(data_name, std::ios::in | std::ios::out | std::ios::binary);
        if (!data_filer.is_open()) {
          data_filer.close();
          init_data();
        } else {
          data_filer.close();
          read_data();
        }
        buffer = new V;
      }

      inline void init_data() {
        data_filer.open(data_name, std::ios::out | std::ios::binary);
        data_filer.write(reinterpret_cast<char *>(&num), sizeof(size_t));
        data_filer.close();
      }

      inline void read_data() {
        data_filer.open(data_name, std::ios::in | std::ios::out | std::ios::binary);
        data_filer.read(reinterpret_cast<char *>(&num), sizeof(size_t));
        data_filer.close();
      }

      void insert(const K &key, const V &val) {
        index_list.insert(key, ++num);
        data_filer.open(data_name, std::ios::app | std::ios::binary);
        data_filer.write(reinterpret_cast<const char *>(&val), sizeof(V));
        data_filer.close();
      }

      void remove(const K &key) {
        index_list.remove(key);
      }

      V *find(const K &key) {
        size_t pos = index_list.find(key, true);
        if (pos == 0) {
          return nullptr;
        }
        data_filer.open(data_name, std::ios::in | std::ios::out | std::ios::binary);
        data_filer.seekg((pos - 1) * sizeof(V) + sizeof(size_t), std::ios::beg);
        data_filer.read(reinterpret_cast<char *>(buffer), sizeof(V));
        data_filer.close();
        buffer_pos = pos;
        return buffer;
      }

      void update() {
        data_filer.open(data_name, std::ios::in | std::ios::out | std::ios::binary);
        data_filer.seekp((buffer_pos - 1) * sizeof(V) + sizeof(size_t), std::ios::beg);
        data_filer.write(reinterpret_cast<char *>(buffer), sizeof(V));
        data_filer.close();
      }

      ~unique_ind_ext_BPtree() {
        data_filer.open(data_name, std::ios::in | std::ios::out | std::ios::binary);
        data_filer.write(reinterpret_cast<char *>(&num), sizeof(size_t));
        data_filer.seekp((buffer_pos - 1) * sizeof(V) + sizeof(size_t), std::ios::beg);
        data_filer.write(reinterpret_cast<char *>(buffer), sizeof(V));
        data_filer.close();
        delete buffer;
      }

    };

}

#endif
