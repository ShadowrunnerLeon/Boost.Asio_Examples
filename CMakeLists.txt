cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Boost COMPONENTS thread system REQUIRED)

add_executable(sync_thread_tcp_server sync_thread/sync_thread_tcp_server.cpp)
target_link_libraries(sync_thread_tcp_server ${Boost_LIBRARIES})

add_executable(sync_thread_tcp_client sync_thread/sync_thread_tcp_client.cpp)
target_link_libraries(sync_thread_tcp_client ${Boost_LIBRARIES})

add_executable(sync_server sync/sync_server.cpp)
target_link_libraries(sync_server ${Boost_LIBRARIES})

add_executable(sync_client sync/sync_client.cpp)
target_link_libraries(sync_client ${Boost_LIBRARIES})

add_executable(async_co_routine_udp_client async_thread/async_co_routine_udp_client.cpp)
target_link_libraries(async_co_routine_udp_client ${Boost_LIBRARIES})

add_executable(async_thread_tcp_client async_thread/async_thread_tcp_client.cpp)
target_link_libraries(async_thread_tcp_client ${Boost_LIBRARIES})

add_executable(async_thread_tcp_server async_thread/async_thread_tcp_server.cpp)
target_link_libraries(async_thread_tcp_server ${Boost_LIBRARIES})

add_executable(async_thread_udp_client async_thread/async_thread_udp_client.cpp)
target_link_libraries(async_thread_udp_client ${Boost_LIBRARIES})

add_executable(async_thread_udp_server async_thread/async_thread_udp_server.cpp)
target_link_libraries(async_thread_udp_server ${Boost_LIBRARIES}) 