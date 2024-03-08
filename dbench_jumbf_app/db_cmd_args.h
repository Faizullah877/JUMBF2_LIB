#pragma once
#include <string>
#include <sstream>
#include <fstream>

#include "db_jumbf_lib.h"

using namespace std;


constexpr unsigned int version_major = 1;
constexpr unsigned int version_minor = 1;
constexpr unsigned int version_patch = 0;

static const string opt_csv_file = "-csv_file";
static const string opt_row_number = "-row_number";
static const string opt_in_dir = "-input_dir";
static const string opt_output_dir = "-output_dir";


static const string opt_lbox = "-lbox";


static const string opt_host_file = "-host_file";
static const string opt_requestable = "-requestable";
static const string opt_label = "-label";
static const string opt_id = "-id";
static const string opt_hash_file = "-hash_file";
static const string opt_hash = "-hash";

static const string opt_private = "-private_data_file";
static const string opt_multiple_private = "-multiple_private";

static const string opt_codestream_file = "-codestream_file";

static const string opt_xml_file = "-xml_file";

static const string opt_json_file = "-json_file";

static const string opt_uuid_data_file = "-uuid_data_file";
static const string opt_uuid = "-uuid";

static const string opt_embed_file = "-embed_file";
static const string opt_embed_filename = "-include_filename";
static const string opt_media_type = "-media_type";
static const string opt_reference_external = "-reference_external";
static const string opt_external_link = "-external_link";

static const string opt_cbor_data = "-cbor_file";

static const string opt_free_bytes = "-free_bytes";

static const string opt_content_type = "-content_type";


static const std::string opt_input_file = "-i";
static const std::string opt_output = "-o";

static const std::string opt_type_encoding = "-encode";
static const std::string opt_type_decoding = "-extract";
static const std::string opt_type_parse = "-parse";

static const string opt_h = "-h";
static const string opt_help = "-help";
static const string opt_v = "-v";
static const string opt_version = "-version";


enum class WorkType {
	EMBED = 0,
	EXTRACT,
	PARSE,
	UNKNOWN
};
enum class FileExtension {
	JPG = 0,
	JUMBF,
	XML,
	TXT,
	JSON,
	CBOR,
	EMPTY,
	OBJ,
	UNKNOWN,
};

struct CmdParams
{
	string host_jpg_file;
	bool is_requestable = false;

	bool label_present = false;
	string label;

	bool id_present = false;
	uint32_t id{ 0 };

	bool hash_present = false;
	unsigned char* hash{ nullptr };
	string hash_file;

	bool private_field_present = false;
	string private_field_data_file;
	unsigned char* private_field_data = nullptr;
	uint64_t private_data_size{ 0 };
	bool multiple_private_field = false;

	string codestream_file;
	string xml_file;
	string json_file;

	string uuid_data_file;
	unsigned char* uuid{ nullptr };

	string embed_file;
	bool embed_file_name{ false };
	string file_name;
	string media_type;
	bool reference_external{ false };
	string external_link;

	string cbor_data_file;
	string output_file{ "db_output.jumbf" };

	dbench::JumbfContentType jumbf_content_type{ dbench::JumbfContentType::UNDEF };
	dbench::BoxType content_box_type{ dbench::BoxType::UNDEF };

	uint64_t no_of_content_box{ 1 };

	uint64_t free_bytes = 0; // Number of padding bytes;

	FileExtension content_data_file_ext = FileExtension::UNKNOWN;
	FileExtension host_file_ext = FileExtension::UNKNOWN;
	FileExtension output_file_ext = FileExtension::UNKNOWN;
	FileExtension input_file_ext = FileExtension::UNKNOWN;

	string csv_file;
	bool write_to_csv = false;
	int row_number{ -1 };
	string input_dir;
	string output_dir;
	string input_file;

	string Lbox = "NULL";

};


void print_app_version();
void print_usage();
void reset_enc_params(CmdParams* enc_params);
void print_uuid_as_str(unsigned char* uuid);
void print_enc_params(CmdParams* enc_params);
bool contains_directory(const string& path);
string get_jumbf_content_type_str(dbench::JumbfContentType type);
std::string combineFilePathAndFileName(const std::string& filePath, const std::string& fileName);
int countRowsInCSV(const std::string& filePath);
std::vector<std::string> getLineFromCSV(std::istream& str, int lineNumber);
void parse_csv_row_to_enc_params(vector<string> row, CmdParams* enc_params);
FileExtension get_file_extension(string filename);
std::string extract_file_name(const std::string& full_path);
void parse_enc_params(int argc, const char* argv[], CmdParams* enc_params);

void sanitize_enc_params(CmdParams* parms);
void sanitize_parsing_params(CmdParams* params);
