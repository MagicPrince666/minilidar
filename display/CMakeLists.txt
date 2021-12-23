file(GLOB display_srcs "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")
add_library(display
    ${display_srcs}
)
target_include_directories(display
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
