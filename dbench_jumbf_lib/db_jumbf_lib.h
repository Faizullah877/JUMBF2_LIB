// jumbf_lib.h : Include file for standard system include files,
// or project specific include files.

#pragma once

// TODO: Reference additional headers your program requires here.

#include "db_define.h"
#include "db_box.h"
#include "db_free_box.h"
#include "db_xml_box.h"
#include "db_json_box.h"
#include "db_cbor_box.h"
#include "db_uuid_box.h"
#include "db_codestream_box.h"
#include "db_bfdb_box.h"
#include "db_binary_data_box.h"
#include "db_jumbf_box.h"
#include "db_jumbf_desc_box.h"
#include "db_xml_jumbf_box.h"
#include "db_json_jumbf_box.h"
#include "db_cbor_jumbf_box.h"
#include "db_codestream_jumbf_box.h"
#include "db_uuid_jumbf_box.h"
#include "db_embedded_file_jumbf_box.h"

constexpr int major_ver = 2;  // compatibility changed
constexpr int minor_ver = 0;  // for backword compatible
constexpr int patch_ver = 0;  // for bug fixes

void print_lib_version();