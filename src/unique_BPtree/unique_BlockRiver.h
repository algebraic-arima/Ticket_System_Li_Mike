#ifndef BPTREE_UNIQUE_BLOCKRIVER_H
#define BPTREE_UNIQUE_BLOCKRIVER_H
#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <filesystem>
#include <utility>
#include "error.h"
#include "unique_BPtree.h"
#include "unique_DataNode.h"
#include "Buffer.h"

namespace arima_kana {
    template<class K, class V, size_t block, size_t bp_max, size_t bp_min, size_t buf_max, size_t bpt_buf_max>
    class unique_BlockRiver {
    public:

      typedef pair<K, V> KV;
      typedef unique_DataNode<K, V, block> DNode;
      typedef unique_BPTree<K, bp_max, bp_min, bpt_buf_max> map;
      typedef List_Map_Buffer<DNode, size_t, 1, buf_max> buffer;

      static constexpr int SIZE_DNODE = sizeof(DNode);
      static constexpr int SIZE_T = sizeof(size_t);

      size_t block_num = 0;
      std::fstream data_filer;
      std::string data_file;
      map list;
      buffer data_list;

      explicit unique_BlockRiver(const std::string &df) :
              data_file(df),
              list(df),
              data_list(df) {
        data_filer.open(data_file, std::ios::in);
        if (!data_filer.is_open()) {
          data_filer.close();
          init_data();
        } else {
          data_filer.close();
          read_data();
        }
      }

      void write_data() {
        data_filer.open(data_file, std::ios::in | std::ios::out | std::ios::binary);
        data_filer.write(reinterpret_cast<char *>(&block_num), SIZE_T);
        data_filer.close();
      }

      ~unique_BlockRiver() {
        write_data();
      }

      void init_data() {
        data_filer.open(data_file, std::ios::out | std::ios::binary);
        data_filer.write(reinterpret_cast<char *>(&block_num), SIZE_T);
        data_filer.close();
      }

      void read_data() {
        data_filer.open(data_file, std::ios::in | std::ios::binary);
        data_filer.read(reinterpret_cast<char *>(&block_num), SIZE_T);
        data_filer.close();
      }

      void append_main(DNode &t) {
        data_filer.open(data_file, std::ios::app);
        data_filer.write(reinterpret_cast<char *>(&t), SIZE_DNODE);
        data_filer.close();
        ++block_num;
      }

      void write_main(DNode &t, const int pos) {
        if (pos > block_num) return;
        data_filer.open(data_file, std::ios::out | std::ios::in | std::ios::binary);
        data_filer.seekp(sizeof(block_num) + (pos - 1) * SIZE_DNODE);
        data_filer.write(reinterpret_cast<char *> (&t), SIZE_DNODE);
        data_filer.close();
      }

      void read_main(DNode &t, const int pos) {
        if (pos > block_num) return;
        data_filer.open(data_file, std::ios::in | std::ios::binary);
        data_filer.seekg(sizeof(block_num) + (pos - 1) * SIZE_DNODE);
        data_filer.read(reinterpret_cast<char *> (&t), SIZE_DNODE);
        data_filer.close();
      }

      void insert(const K &k, const V &v) {
        KV kv = {k, v};
        //std::cout<<tv.key<<tv.pos;
        if (list.empty()) {
          //std::cout << "empty\n";
          DNode tmp(kv);
          append_main(tmp);
          list.insert(k, block_num);
          data_list[block_num] = tmp;
          return;
        }
        size_t it = list.block_lower_bound(k);

        if (it == 0) {
          list.adjust_max(k);
          // adjust the maximum to kv
          it = list.block_lower_bound(k);
          // it points to a min node
        }// kv is greater than the maximum
        DNode &tmp = data_list[it];
        try { tmp.insert_pair(k, v); }
        catch (...) {
          return;
        }
        if (tmp.size >= block) {
          DNode new_node;
          new_node.size = tmp.size / 2;
          for (int i = 0; i < tmp.size / 2; i++) {
            new_node._data[i] = tmp._data[i];
          }
          for (int i = tmp.size / 2; i < tmp.size; i++) {
            tmp._data[i - tmp.size / 2] = tmp._data[i];
          }
          tmp.size -= tmp.size / 2;
          list.insert(new_node._data[new_node.size - 1].first, block_num + 1);
          //std::cout << new_node.first.key << new_node.first.pos << block_num + 1 << '\n';
          append_main(new_node);
          data_list[block_num] = new_node;
        }
      }

      void remove(const K &k) {
        auto it = list.block_lower_bound(k);
        if (it == 0) return;
        DNode &tmp = data_list[it];

        if (k == tmp._data[tmp.size - 1].first) {
          list.remove(k);
          if (tmp.size != 1) {
            list.insert(tmp._data[tmp.size - 2].first, it);
          }
        }
        try { tmp.remove_pair(k); }
        catch (...) {
          return;
        }
      }

      /// unique
      V *find(const K &k) {
        vector<size_t> tmp = list.find(k);
        for (int i = 0; i < tmp.size(); i++) {
          DNode &t = data_list[tmp[i]];
          for (int j = 0; j < t.size; j++) {
            if (t._data[j].first == k) {
              return &t._data[j].second;
            }
          }
        }
        return nullptr;
      }

      V find(const K &k, bool flag) {
        vector<size_t> tmp = list.find(k);
        for (int i = 0; i < tmp.size(); i++) {
          DNode &t = data_list[tmp[i]];
          for (int j = 0; j < t.size; j++) {
            if (t._data[j].first == k) {
              return t._data[j].second;
            }
          }
        }
        return {};
      }


      void print() {
        list.print();
        for (int i = 1; i <= block_num; i++) {
          DNode &tmp = data_list[i];
          std::cout << i << '\n';
          tmp.print();
        }
      }

      void clear() {
        list.clear();
        block_num = 0;
        init_data();
      }

    };

}

#endif