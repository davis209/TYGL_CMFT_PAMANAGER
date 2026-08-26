cmake_minimum_required(VERSION 3.10)
project(BroadcastScheduleService LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Threads REQUIRED)

add_executable(BroadcastScheduleService
    src/main.cpp
    src/BroadcastSchedulerService.cpp
    src/M44BroadcastExecutor.cpp
    src/ServiceLogger.cpp)

target_include_directories(BroadcastScheduleService PRIVATE include ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(BroadcastScheduleService PRIVATE Threads::Threads)

# The service uses TA_IRS_Core::PaBroadcastScheduleAccessFactory and must be
# linked with the existing TA_DAI/TA_PADAI libraries by the integrated build.

if(WIN32)
    target_link_libraries(BroadcastScheduleService PRIVATE ws2_32)
endif()
