file(GLOB input_srcs "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")
add_library(input 
    ${input_srcs}
)

include_directories(${CMAKE_SOURCE_DIR}/epoll
    ${CMAKE_SOURCE_DIR}
)

target_include_directories(input
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
