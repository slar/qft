cmake_minimum_required(VERSION 3.18)
SET(CXX_STANDARD 14)
project(Quickfixtest)

include_directories(system /usr/local/include)

find_library(QUICKFIX quickfix)

add_executable (qfts app.h qfts.cc)
target_link_libraries(qfts ${QUICKFIX})


add_executable (qftc app.h qftc.cc)
target_link_libraries(qftc ${QUICKFIX})