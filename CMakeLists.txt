CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
option(EXAMPLE "shall we complie example" on)
# INCLUDE(build_for_host)

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

include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/utils)
include_directories(
"${CMAKE_SOURCE_DIR}/include"
"${CMAKE_SOURCE_DIR}/utils"
"${CMAKE_SOURCE_DIR}/imu"
"${CMAKE_SOURCE_DIR}/drivers"
"${CMAKE_SOURCE_DIR}/ultrasonic"
"${CMAKE_SOURCE_DIR}/moto"
"${CMAKE_SOURCE_DIR}/distance"
"${CMAKE_SOURCE_DIR}/display"
"${CMAKE_SOURCE_DIR}/input"
"${CMAKE_SOURCE_DIR}/imu/mpu6050"
)

add_subdirectory(lib_json)

set(EXEC_NAME minilidar)

file(GLOB main_srcs 
"${CMAKE_CURRENT_SOURCE_DIR}/*.cc"
"${CMAKE_CURRENT_SOURCE_DIR}/utils/*.cc"
"${CMAKE_CURRENT_SOURCE_DIR}/imu/*.cc"
"${CMAKE_CURRENT_SOURCE_DIR}/drivers/*.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/ultrasonic/*.cc"
"${CMAKE_CURRENT_SOURCE_DIR}/moto/*.cc"
"${CMAKE_CURRENT_SOURCE_DIR}/distance/*.cc"
"${CMAKE_CURRENT_SOURCE_DIR}/display/*.cc"
"${CMAKE_CURRENT_SOURCE_DIR}/input/*.cc"
"${CMAKE_CURRENT_SOURCE_DIR}/imu/mpu6050/*.cpp"
)

add_executable(${EXEC_NAME}
${main_srcs}
)

target_link_libraries(${EXEC_NAME} 
    multiplexing
    json
)

install(TARGETS ${EXEC_NAME}
        RUNTIME DESTINATION /usr/sbin)