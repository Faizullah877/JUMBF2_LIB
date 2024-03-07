# CMakeList.txt : CMake project for jumbf_lib, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_library (dbench_jumbf_lib "db_jumbf_lib.cpp" "db_jumbf_lib.h" "db_box.h" "db_box.cpp" "db_define.h" "db_define.cpp" "db_free_box.h" "db_free_box.cpp" "db_xml_box.h" "db_xml_box.cpp" "db_json_box.h" "db_json_box.cpp" "db_cbor_box.h" "db_cbor_box.cpp" "db_codestream_box.h" "db_codestream_box.cpp" "db_uuid_box.h" "db_uuid_box.cpp" "db_bfdb_box.h" "db_bfdb_box.cpp" "db_binary_data_box.h" "db_binary_data_box.cpp" "db_jumbf_desc_box.h" "db_jumbf_desc_box.cpp" "db_jumbf_box.h" "db_jumbf_box.cpp" "db_xml_jumbf_box.h" "db_xml_jumbf_box.cpp" "db_json_jumbf_box.h" "db_json_jumbf_box.cpp" "db_cbor_jumbf_box.h" "db_cbor_jumbf_box.cpp" "db_uuid_jumbf_box.h" "db_uuid_jumbf_box.cpp" "db_codestream_jumbf_box.h" "db_codestream_jumbf_box.cpp" "db_embedded_file_jumbf_box.h" "db_embedded_file_jumbf_box.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET dbench_jumbf_lib PROPERTY CXX_STANDARD 14)
endif()

# TODO: Add tests and install targets if needed.
