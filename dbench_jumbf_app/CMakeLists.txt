# CMakeList.txt : CMake project for jumbf_lib, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable(dbench_jumbf_app "dbench_jumbf_app.h" "dbench_jumbf_app.cpp" "db_cmd_args.h" "db_cmd_args.cpp" "db_jumbf_encoder.h" "db_jumbf_encoder.cpp" "db_jumbf_parser.h" "db_jumbf_parser.cpp")

target_include_directories(dbench_jumbf_app PUBLIC "${CMAKE_SOURCE_DIR}/dbench_jumbf_lib/include")
target_link_libraries(dbench_jumbf_app dbench_jumbf_lib)
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET dbench_jumbf_app PROPERTY CXX_STANDARD 14)
endif()

# TODO: Add tests and install targets if needed.

