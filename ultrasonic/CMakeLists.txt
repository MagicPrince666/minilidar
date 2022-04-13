file(GLOB ultrasonic_srcs "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")
add_library(ultrasonic 
    ${ultrasonic_srcs}
)

include_directories(${CMAKE_SOURCE_DIR}/epoll
    ${CMAKE_SOURCE_DIR}
)

target_include_directories(ultrasonic
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
