cmake_minimum_required(VERSION 3.22)
project(Ticket_System)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=leak -fsanitize=undefined")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")

include_directories(src)
include_directories(src/BPtree)
include_directories(src/unique_BPtree)
include_directories(src/utility)

add_executable(code
        src/BPtree/BNode.h
        src/BPtree/BlockRiver.h
        src/BPtree/BPtree.h
        src/BPtree/DataNode.h
        src/unique_BPtree/unique_BNode.h
        src/unique_BPtree/unique_BlockRiver.h
        src/unique_BPtree/unique_BPtree.h
        src/unique_BPtree/unique_DataNode.h
        src/utility/error.h
        src/utility/utility.h
        src/utility/Buffer.h
        src/utility/map.h
        src/account.h
        main.cpp
        src/train.h
        src/parser.h
        src/seat.h
        src/order.h
        src/pending.h
)
