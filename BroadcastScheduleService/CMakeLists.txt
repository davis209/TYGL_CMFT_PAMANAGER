cmake_minimum_required(VERSION 3.10)
project(BroadcastScheduleService LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)
find_path(MYSQL_INCLUDE_DIR mysql.h PATH_SUFFIXES mysql mariadb)
find_library(MYSQL_LIBRARY NAMES libmysql mysqlclient mariadb)

if(NOT MYSQL_INCLUDE_DIR OR NOT MYSQL_LIBRARY)
    message(FATAL_ERROR "MySQL Connector/C is required. Set MYSQL_INCLUDE_DIR and MYSQL_LIBRARY if it is installed in a non-standard location.")
endif()

add_executable(BroadcastScheduleService
    src/main.cpp
    src/BroadcastSchedulerService.cpp
    src/M44BroadcastExecutor.cpp
    src/ServiceLogger.cpp)

target_include_directories(BroadcastScheduleService PRIVATE include ${MYSQL_INCLUDE_DIR})
target_link_libraries(BroadcastScheduleService PRIVATE ${MYSQL_LIBRARY} Threads::Threads)

if(WIN32)
    target_link_libraries(BroadcastScheduleService PRIVATE ws2_32)
endif()
