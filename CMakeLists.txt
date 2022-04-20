CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
option(EXAMPLE "shall we complie example" on)
INCLUDE(build_for_host)

include_directories(${CMAKE_SOURCE_DIR}/include)

PROJECT(CppDemo)

# SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_BUILD_TYPE "Release")

SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} -O0 -Wall -g")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} -Os -Wall")

SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -std=c++14 -O0 -Wall -g")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -std=c++14 -Os -Wall")

MESSAGE(STATUS ${CMAKE_SYSTEM_NAME})

IF (WIN32)
	MESSAGE(STATUS "Now is windows.")
    add_subdirectory(iocp)
ELSEIF (APPLE)
	MESSAGE(STATUS "Now is Apple systens.")
    add_subdirectory(kqueue)
ELSEIF (UNIX)
	MESSAGE(STATUS "Now is UNIX-like OS's.")
    add_subdirectory(epoll)
ENDIF ()

#add_subdirectory(imu)
# add_subdirectory(input)
#add_subdirectory(distance)
# add_subdirectory(ultrasonic)
#add_subdirectory(moto)
# add_subdirectory(display)
add_subdirectory(lib_json)

set(EXEC_NAME minilidar)

add_executable(${EXEC_NAME} main.cc
#    uart.cc
#    inotify.cc
#    timerfd.cc
    utils.cc
    socket.cc
)

target_link_libraries(${EXEC_NAME} 
    multiplexing
#    distance
#    imu
#    moto
#    display
#    ultrasonic
    json
#    input
)

