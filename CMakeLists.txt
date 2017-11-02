cmake_minimum_required(VERSION 2.6)
project(check_zk)

add_executable(check_zk main.cpp)

install(TARGETS check_zk RUNTIME DESTINATION bin)
