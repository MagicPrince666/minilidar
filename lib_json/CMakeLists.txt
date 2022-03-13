set(JSONCPP_INCLUDE_DIR ../include)

set(PUBLIC_HEADERS
    ${JSONCPP_INCLUDE_DIR}/json/config.h
    ${JSONCPP_INCLUDE_DIR}/json/forwards.h
    ${JSONCPP_INCLUDE_DIR}/json/json_features.h
    ${JSONCPP_INCLUDE_DIR}/json/value.h
    ${JSONCPP_INCLUDE_DIR}/json/reader.h
    ${JSONCPP_INCLUDE_DIR}/json/version.h
    ${JSONCPP_INCLUDE_DIR}/json/writer.h
    ${JSONCPP_INCLUDE_DIR}/json/assertions.h
)

set(JSONCPP_SOURCES
    json_tool.h
    json_reader.cpp
    json_valueiterator.inl
    json_value.cpp
    json_writer.cpp
)

add_library(json
    ${JSONCPP_SOURCES}
)
target_include_directories(json
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})