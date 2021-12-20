file(GLOB imu_srcs "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")
add_library(imu 
    ${imu_srcs}
)
target_include_directories(imu
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
