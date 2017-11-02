cmake_minimum_required(VERSION 2.6)
project(check_zk)

set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.45.0 COMPONENTS system program_options regex) 

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) 
    add_executable(check_zk main.cpp)
    target_link_libraries(check_zk ${Boost_LIBRARIES})
endif()


install(TARGETS check_zk RUNTIME DESTINATION bin)
