# 火车票管理系统

SJTU CS1951 课程大作业

![What is TS](/images/TS.png)

*当然是Ticket System啦！*

## 文件结构
``````
├── code
├── CMakeLists.txt
├── main.cpp
├── run-test
├── src
│   ├── BPtree
│   │   ├── BNode.h
│   │   ├── BPtree.h
│   │   ├── BlockRiver.h
│   │   └── DataNode.h
│   ├── account.h
│   ├── order.h
│   ├── parser.h
│   ├── pending.h
│   ├── seat.h
│   ├── train.h
│   ├── unique_BPtree
│   │   ├── unique_BNode.h
│   │   ├── unique_BPtree.h
│   │   ├── unique_BlockRiver.h
│   │   └── unique_DataNode.h
│   ├── unique_ind_ext_BPtree
│   │   └── unique_ind_ext_BPtree.h
│   └── utility
│       ├── Buffer.h
│       ├── error.h
│       ├── map.h
│       ├── timer.h
│       ├── unordered_map.h
│       └── utility.h
``````


## 文件内容

### 工具箱
- `/src/BPtree`键可重复，但是键值对不可重复的B+树，类名`BlockRiver`
- `/src/unique_BPtree`键值对不可重复的B+树，类名`unique_BlockRiver`
- `/src/unique_ind_ext_BPtree`键值对不可重复的外部B+树，将数据存储在另一个文件中，B+树存储
`Key`到数据物理外存位置的映射，类名`unique_ind_ext_BPtree`
- `/src/utility`一些工具类，如缓冲池`Buffer`，AA树`map`，数据对`pair`，
  错误处理`ErrorException`，动态存储表`vector`及其排序函数`sort()`，
  基于哈希表的无序映射`unordered_map`和用于调试的计时器`timer`。

### 火车票管理系统
- `/src/account.h`用户管理系统，包含1个不可重B+树，存储用户id->用户信息
- `/src/train.h`车次管理系统，包含
  - 1个不可重外部B+树，存储火车id->车次在Seat子系统中的存储位置->车次信息；
  - 1个可重B+树，存储车站->经过该站的车次信息；
  - 1个不可重B+树，存储（车站，火车id）->经过该站、编号为id的车次信息
- `/src/seat.h`座位管理系统，是1个带Buffer的文件管理系统，存储车次的剩余座位信息
- `/src/order.h`订单记录系统，包含1个可重B+树，存储用户id->订单信息
- `/src/pending.h`候补管理系统，包含1个可重B+树，存储火车->订单信息
- `/src/parser.h`解析器类，包含以上所有类的实例。负责处理输入，向各个子系统发出指令
- `main.cpp`主函数，负责读取每一行指令，并调用解析器类的`handle()`函数进行处理





