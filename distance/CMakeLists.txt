file(GLOB distance_srcs "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")
add_library(distance 
    ${distance_srcs}
)
target_include_directories(distance
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
