cmake_minimum_required(VERSION 3.22)
project(Ticket_System)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=leak -fsanitize=undefined")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")

include_directories(src)
include_directories(manage)

add_executable(code
        src/BNode.h
        src/BlockRiver.h
        src/BPtree.h
        src/DataNode.h
        src/error.h
        src/utility.h
        src/Buffer.h
        src/map.h
        manage/account.h
        main.cpp
        manage/train.h
        manage/parser.h
        manage/seat.h
        manage/order.h
)
