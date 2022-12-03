CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
option(EXAMPLE "shall we complie example" on)
# INCLUDE(build_for_host)

include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/utils)

PROJECT(CppDemo)

set(CMAKE_CXX_STANDARD 17)

SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb ")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

MESSAGE(STATUS ${CMAKE_SYSTEM_NAME})

IF (CMAKE_HOST_WIN32)
	MESSAGE(STATUS "Now is windows.")
    add_subdirectory(iocp)
#ELSEIF (CMAKE_HOST_APPLE)
#	MESSAGE(STATUS "Now is Apple systens.")
#    add_subdirectory(kqueue)
ELSEIF (UNIX)
	MESSAGE(STATUS "Now is UNIX-like OS's.")
    add_subdirectory(epoll)
ENDIF ()

add_subdirectory(imu)
add_subdirectory(input)
add_subdirectory(distance)
add_subdirectory(ultrasonic)
add_subdirectory(moto)
add_subdirectory(display)
add_subdirectory(lib_json)

set(EXEC_NAME minilidar)

add_executable(${EXEC_NAME} main.cc
    utils/uart.cc
    utils/inotify.cc
    utils/timerfd.cc
    utils/utils.cc
    utils/socket.cc
    utils/udp_server.cc
)

target_link_libraries(${EXEC_NAME} 
    multiplexing
    json
    input
    distance
    imu
    moto
    display
    ultrasonic
)

