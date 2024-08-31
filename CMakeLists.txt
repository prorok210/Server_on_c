cmake_minimum_required(VERSION 3.2)
project (WebServer)

find_package(PkgConfig REQUIRED)
pkg_check_modules(MONGOC REQUIRED libmongoc-1.0)
pkg_check_modules(BSON REQUIRED libbson-1.0)

find_path(CJSON_INCLUDE_DIR cjson/cJSON.h)
find_library(CJSON_LIBRARY NAMES cjson libcjson)

if (NOT CJSON_INCLUDE_DIR OR NOT CJSON_LIBRARY)
    message(FATAL_ERROR "cJSON library not found. Install libcjson-dev or specify the paths manually.")
endif()

include_directories(${MONGOC_INCLUDE_DIRS} ${BSON_INCLUDE_DIRS} ${CJSON_INCLUDE_DIR})
link_directories(${MONGOC_LIBRARY_DIRS} ${BSON_LIBRARY_DIRS} ${CJSON_LIBRARY_DIR})

file(GLOB_RECURSE SOURCES "src/*.c")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -Wextra")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(server ${SOURCES})
set_target_properties(server PROPERTIES LANGUAGE C)
target_link_libraries(server ${MONGOC_LIBRARIES} ${BSON_LIBRARIES} ${CJSON_LIBRARY})

# add_executable(test tests/tests.c src/database.c src/http_pars.c src/register_views.c src/views.c)
# set_target_properties(test PROPERTIES LANGUAGE C)
# target_link_libraries(test ${MONGOC_LIBRARIES} ${BSON_LIBRARIES})

