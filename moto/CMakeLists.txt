file(GLOB moto_srcs "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")
add_library(moto 
    ${moto_srcs}
)
target_include_directories(moto
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
