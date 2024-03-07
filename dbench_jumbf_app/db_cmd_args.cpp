#include <iostream>
#include <string>
#include <cstring>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef _WIN32 // Windows-specific code
#include <direct.h>
#define MKDIR _mkdir
#else // Linux-specific code
#include <sys/stat.h>
#define MKDIR(path) mkdir(path, 0777)
#endif

#include "db_cmd_args.h"
void print_app_version()

{
	cout << "dbench_jumbf version : v" << version_major << "." << version_minor << "." << version_patch << endl;
}

void print_usage() {
	print_app_version();
	cout << "Usage : db_jumbf_app.exe <options>" << endl;
	cout << "Options: " << endl;
	cout << "\t-encode                        Encoding/Generating JUMBF (default)" << endl;
	cout << "\t-parse                         Parse a .jumbf or .jpg file" << endl;
	cout << "\t-host_file     INPUTFILENAME   Specify input file name" << endl;
	cout << "\t                               .jpg, .jpeg" << endl;
	cout << "\t-o             OUTPUTFILENAME  Specify output file name. default = \"db_output.jumbf\"" << endl;
	cout << "\t                               .jpg, .jpeg, or .jumbf file." << endl;
	cout << "\t-content_type  OUTPUTFILENAME  Specify JUMBF content type." << endl;
	cout << "\t                               Possible values are XML, JSON, JP2C, UUID, CBOR, EMBEDDED_FILE" << endl;
	cout << "\t-lbox          0, 1 or NULL    Specify Lbox field ." << endl;
	cout << "\t                               0 = box size will be file size i.e Box will continue till EOF." << endl;
	cout << "\t                               1 = box size will be present in XLbox field." << endl;
	cout << "\t                               NULL = box size will be present in Lbox or XLbox field." << endl;
	cout << "\t-requestable                   Set requestable field to true in JUMBF Description box" << endl;
	cout << "\t-label         LABEL STRING    Specify Label for JUMB Description box." << endl;
	cout << "\t-id            ID              Specify ID for JUMB Description box." << endl;
	cout << "\t-hash          HASH            Specify hash/signature for JUMB Description box." << endl;
	cout << "\t                               HASH should be string of 32 charachters. each 2 characters represent a HEX byte" << endl;
	cout << "\t-hash_file     HASH_FILE       Specify file name for hash/signature to put in JUMB Description box." << endl;
	cout << "\t                               HASH_FILE should contain only string of 32 charachters. each 2 characters represent a HEX byte" << endl;
	cout << "\t-private_data_file FILE_NAME   Specify file name for private box data to put in JUMB Description box." << endl;
	cout << "\t                               FILE_NAME should contain bitstream in accordance with Box definition" << endl;
	cout << "\t-codestream_file FILE_NAME     Specify file name for codestream data to put in JUMBF Content box." << endl;
	cout << "\t-xml_file        FILE_NAME     Specify file name (.xml) for xml data to put in JUMBF Content box." << endl;
	cout << "\t-json_file       FILE_NAME     Specify file name (.json) for json data to put in JUMBF Content box." << endl;
	cout << "\t-uuid            UUID          Specify UUID for UUID field to put in JUMBF Content box." << endl;
	cout << "\t                               UUID should be string of charachters. each 2 characters represent a HEX byte" << endl;
	cout << "\t                               character \"-\" will ignored by the software." << endl;
	cout << "\t                               e.g -uuid 663295b0-2158-42de-b16f-a4e458237885 " << endl;
	cout << "\t                               e.g -uuid 663295b0215842deb16fa4e458237885 " << endl;
	cout << "\t-uuid_data_file  FILE_NAME     Specify file name for UUID box paylaod data to put in JUMBF Content box." << endl;
	cout << "\t-embed_file      FILE_NAME     Specify file name to embed in JUMBF Content box." << endl;
	cout << "\t-media_type      MEDIA_TYPE    Specify media type to put in File Description box." << endl;
	cout << "\t-include_filename              Specify flag for file name to put in File Description box." << endl;
	cout << "\t                               If this flag is provided then file name will be extracted from -embed_file." << endl;
	cout << "\t-reference_external            Specify flag reference as external flag in File Description box." << endl;
	cout << "\t                               If this flag is provided then -embed_file wil be used as external link and the flag will on." << endl;
	cout << "\t-cbor_file       FILE_NAME     Specify file name (.cbor) for CBOR data to put in JUMBF Content box." << endl;
	cout << "\t-free_bytes      NUMBER        Specify number of padding bytes for JUMBF padding box." << endl;
	cout << "\t                               If NUMBER is provided then padding box with Tbox = FREE wil be generated and included in JUMBF." << endl;

	cout << "\t-csv_file       DATASET_FILE   Specify input dataset file name" << endl;
	cout << "\t                               .csv file the data should be in accordance with JPEG JUMBF Dataset" << endl;
	cout << "\t-row_number     ROW_NUMBER     Specify row number in dataset file name" << endl;
	cout << "\t                               specified row number data will be used and single output file will genereated" << endl;
	cout << "\t-input_dir      INPUT_FOLDER   Specify input directory name" << endl;
	cout << "\t                               the directory should exist and contain all in input files." << endl;
	cout << "\t-output_dir     OUTPUT_FOLDER  Specify directory name for output files" << endl;

	cout << "\t-h                  Print usage." << endl;
	cout << "\t-help               Print usage." << endl;
	cout << "\t-v                  Print version." << endl;
	cout << "\t-version            Print version." << endl;


	cout << endl;
}
void reset_enc_params(CmdParams* enc_params)
{
	enc_params->input_file.clear();
	enc_params->host_jpg_file.clear();
	enc_params->host_file_ext = FileExtension::UNKNOWN;
	enc_params->is_requestable = false;
	enc_params->label_present = false;
	enc_params->label.clear();
	enc_params->id = 0;
	enc_params->id_present = false;
	enc_params->hash_present = false;
	enc_params->hash = nullptr;
	enc_params->hash_file.clear();
	enc_params->private_field_present = false;
	enc_params->private_field_data_file = "";
	enc_params->private_field_data = nullptr;
	enc_params->private_data_size = 0;
	enc_params->multiple_private_field = false;
	enc_params->codestream_file = "";
	enc_params->xml_file = "";
	enc_params->json_file = "";
	enc_params->uuid_data_file = "";
	enc_params->uuid = nullptr;
	enc_params->embed_file = "";
	enc_params->embed_file_name = false;
	enc_params->file_name = "";
	enc_params->media_type = "";
	enc_params->reference_external = false;
	enc_params->external_link = "";
	enc_params->cbor_data_file = "";
	enc_params->output_file = "";
	enc_params->jumbf_content_type = dbench::JumbfContentType::UNDEF;
	enc_params->content_box_type = dbench::BoxType::UNDEF;
	enc_params->no_of_content_box = 1;
	enc_params->free_bytes = 0;
	enc_params->Lbox = "NULL";
}
string get_jumbf_content_type_str(dbench::JumbfContentType type) {
	switch (type)
	{
	case dbench::JumbfContentType::CODESTREAM:
		return "CODESTREAM";
		break;
	case dbench::JumbfContentType::XML:
		return "XML";
		break;
	case dbench::JumbfContentType::JSON:
		return "JSON";
		break;
	case dbench::JumbfContentType::UUID:
		return "UUID";
		break;
	case dbench::JumbfContentType::EMBEDDED_File:
		return "EMBEDDED FILE";
		break;
	case dbench::JumbfContentType::CBOR:
		return "CBOR";
		break;
	case dbench::JumbfContentType::JPEG360:
		return "JPEG 360";
		break;
	case dbench::JumbfContentType::UNDEF:
		return "UNKNOWN";
		break;
	default:
		return "UNKNOWN";
		break;
	}
}

void print_uuid_as_str(unsigned char* uuid) {
	for (auto i = 0; i < 16; i++) {
		if (i == 4 || i == 6 || i == 8 || i == 10)
			cout << "-";
		cout << setw(2) << setfill('0') << hex << int(uuid[i]);
	}
}
bool create_folder(const std::string& folderPath) {

	if (MKDIR(folderPath.c_str()) == 0 || errno == EEXIST) {
		std::cout << "Folder exists or created successfully: " << folderPath << std::endl;
		return true;
	}
	else {
		std::cerr << "Failed to create folder: " << folderPath << std::endl;
		return false;
	}
}


void print_enc_params(CmdParams* enc_params)
{
	cout << "Encoding Configurations" << endl;
	if (!enc_params->input_dir.empty()) {
		cout << "   Input Files directory  :       [" << enc_params->input_dir << "]" << endl;
	}
	if (!enc_params->csv_file.empty()) {
		cout << "   Input CSV File Name :          [" << enc_params->csv_file << "]" << endl;
		if (enc_params->row_number != -1) {
			cout << "   Row No. in CSV File :          [" << dec << enc_params->row_number << "]" << endl;
		}
	}
	if (!enc_params->host_jpg_file.empty()) {
		cout << "\tHost File Name :          [" << enc_params->host_jpg_file << "]" << endl;
	}

	cout << "   JUMBF Description Box Configuration" << endl;

	dbench::JumbfContentType type = enc_params->jumbf_content_type;
	unsigned char* uuid = dbench::db_get_jumbf_content_type_uuid(enc_params->jumbf_content_type);

	cout << "\tJUMBF Content Type UUID : [";
	print_uuid_as_str(uuid);
	cout << "]";
	cout << "\t[" << get_jumbf_content_type_str(type) << "]" << endl;
	string requestable = enc_params->is_requestable ? "TRUE" : "FALSE";
	cout << "\tIs Requestable :          [" << requestable << "]" << endl;

	if (enc_params->label_present)
		cout << "\tLABEL :                   [" << enc_params->label << "]" << endl;
	else
		cout << "\tLABEL :                   [NOT PRESENT]" << endl;

	if (enc_params->id_present)
		cout << "\tID :                      [" << dec << enc_params->id << "]" << endl;
	else
		cout << "\tID :                      [NOT PRESENT]" << endl;

	if (enc_params->hash_present) {
		cout << "\tHASH :                    [0x";
		for (auto i = 0; i < 32; i++)
			cout << hex << int(enc_params->hash[i]);
		cout << "]" << endl;
	}
	else
		cout << "\tHASH :                    [NOT PRESENT]" << endl;

	if (enc_params->private_field_present || enc_params->multiple_private_field)
	{
		cout << "\tPRIVATE FIELD :           [PRESENT]" << endl;
		cout << "\tPRIVATE FIELD data file : [" << enc_params->private_field_data_file << "]" << endl;
		cout << "\tSize of Private Data :    [" << enc_params->private_data_size << "]" << endl;
	}
	else
		cout << "\tPRIVATE FIELD :           [NOT PRESENT]" << endl;


	cout << "   JUMBF Content Boxes Configuration" << endl;

	switch (type)
	{
	case dbench::JumbfContentType::CODESTREAM:
		cout << "\t   Codestream File :      [" << enc_params->codestream_file << "]" << endl;
		break;
	case dbench::JumbfContentType::XML:
		cout << "\t   XML Data File :        [" << enc_params->xml_file << "]" << endl;
		break;
	case dbench::JumbfContentType::JSON:
		cout << "\t   JSON Data File :       [" << enc_params->json_file << "]" << endl;
		break;
	case dbench::JumbfContentType::UUID:
		cout << "\t   Content Data UUID :    [";
		print_uuid_as_str(enc_params->uuid);
		cout << "]" << endl;
		cout << "\t   UUID Data File :       [" << enc_params->uuid_data_file << "]" << endl;
		break;
	case dbench::JumbfContentType::EMBEDDED_File:
		cout << "\t   Media Type :           [" << enc_params->media_type << "]" << endl;
		cout << "\t   Embedded File :        [" << enc_params->embed_file << "]" << endl;
		if (enc_params->embed_file_name) {
			cout << "\t   Filename Present :     [YES] =>    [" << enc_params->file_name << "]" << endl;
		}
		else {
			cout << "\t   Filename Present :     [NO]" << endl;
		}
		if (enc_params->reference_external) {
			cout << "\t   Referenced Externally: [YES]" << endl;
		}
		else {
			cout << "\t   Referenced Externally: [NO]" << endl;
		}
		break;
	case dbench::JumbfContentType::CBOR:
		cout << "\t   CBOR Data File :        [" << enc_params->cbor_data_file << "]" << endl;
		break;
	case dbench::JumbfContentType::JPEG360:
		break;
	case dbench::JumbfContentType::UNDEF:
		break;
	default:
		break;
	}
	if (enc_params->free_bytes != 0) {
		cout << "\tPadding Box :             [PRESENT]" << endl;
		cout << "\t   Number of Free Bytes : [" << dec << enc_params->free_bytes << "]" << endl;
	}
	else
		cout << "\tPadding Box :             [NOT PRESENT]" << endl;


	cout << "   Output File :                  [" << enc_params->output_file << "]" << endl;
}
FileExtension get_file_extension(string file) {
	if (file.empty())
		return FileExtension::EMPTY;
	string::size_type n;
	n = file.rfind(".");
	string::size_type ext_size = file.length() - n;
	string ext = file.substr(n);

	if (ext == ".jpg" || ext == ".JPG")
		return FileExtension::JPG;
	if (ext == ".jpeg" || ext == ".JPEG")
		return FileExtension::JPG;
	else if (ext == ".jumbf" || ext == ".JUMBF")
		return FileExtension::JUMBF;
	else if (ext == ".xml" || ext == ".XML")
		return FileExtension::XML;
	else if (ext == ".txt" || ext == ".TXT")
		return FileExtension::TXT;
	else if (ext == ".json" || ext == ".JSON")
		return FileExtension::JSON;
	else if (ext == ".cbor" || ext == ".CBOR")
		return FileExtension::CBOR;
	else if (ext == ".obj" || ext == ".OBJ")
		return FileExtension::OBJ;
	else
		return FileExtension::UNKNOWN;
}
dbench::JumbfContentType  recognise_content_type(string type_str);

std::string extract_file_name(const std::string& full_path) {
	size_t last_slash_index = full_path.find_last_of("/\\");
	if (last_slash_index == std::string::npos) {
		return full_path;
	}
	return full_path.substr(last_slash_index + 1);
}
int countRowsInCSV(const std::string& filePath)
{
	std::ifstream file(filePath);
	int rowCount = 0;
	std::string line;
	while (std::getline(file, line))
	{
		rowCount++;
	}
	return rowCount;
}

std::vector<std::string> getLineFromCSV(std::istream& str, int lineNumber)
{
	std::vector<std::string> result;
	std::string line;
	int currentLineNumber = 0;
	while (std::getline(str, line))
	{
		currentLineNumber++;
		if (currentLineNumber == lineNumber)
		{
			std::stringstream lineStream(line);
			std::string cell;
			while (std::getline(lineStream, cell, ','))
			{
				result.push_back(cell);
			}
			break;
		}
	}
	return result;
}


void parse_enc_params(int argc, const char* argv[], CmdParams* enc_params)
{
	for (int i = 2; i < argc; i++) {
		string arg = argv[i++];

		if (arg == opt_csv_file) {
			enc_params->csv_file = argv[i];
		}
		else if (arg == opt_row_number) {
			enc_params->row_number = stoi(argv[i]);
		}
		else if (arg == opt_input_file) {
			enc_params->input_file = argv[i];
		}
		else if (arg == opt_in_dir) {
			enc_params->input_dir = argv[i];
		}
		else if (arg == opt_output_dir) {
			enc_params->output_dir = argv[i];
		}
		else if (arg == opt_host_file) {
			enc_params->host_jpg_file = argv[i];
			enc_params->host_file_ext = get_file_extension(argv[i]);
		}
		else if (arg == opt_lbox) {
			enc_params->Lbox = argv[i];
		}
		else if (arg == opt_requestable) {
			enc_params->is_requestable = true;
			i--;
		}
		else if (arg == opt_label) {
			string label = argv[i];
			while (true)
			{
				string next_word = argv[++i];
				if (next_word.front() == '-') {
					i--;
					break;
				}
				else {
					label.append(" ");
					label.append(next_word);
				}
			}
			enc_params->label = label;
			enc_params->label_present = true;
		}
		else if (arg == opt_id) {
			enc_params->id = stoi(argv[i]);
			enc_params->id_present = true;
		}
		else if (arg == opt_hash) {
			string hash_str = argv[i];
			if (hash_str.size() != 32) {
				cout << "Error: \"" << hash_str << "\" Number of characters must be 32." << endl;
				exit(EXIT_FAILURE);
			}
			else {
				enc_params->hash = new unsigned char[32];
				//memcpy_s(enc_params->hash, 32, hash_str.c_str(), 32);
				memcpy(enc_params->hash, hash_str.c_str(), 32);
				enc_params->hash_present = true;
			}
		}
		else if (arg == opt_hash_file) {
			enc_params->hash_file = argv[i];
		}
		else if (arg == opt_private) {
			enc_params->private_field_present = true;
			enc_params->private_field_data_file = argv[i];
		}
		else if (arg == opt_multiple_private) {
			enc_params->multiple_private_field = true;
		}
		else if (arg == opt_codestream_file) {
			enc_params->codestream_file = argv[i];
			enc_params->jumbf_content_type = dbench::JumbfContentType::CODESTREAM;
			enc_params->content_box_type = dbench::BoxType::JP2C;
		}
		else if (arg == opt_xml_file) {
			enc_params->xml_file = argv[i];
			enc_params->jumbf_content_type = dbench::JumbfContentType::XML;
			enc_params->content_box_type = dbench::BoxType::XML;
			enc_params->content_data_file_ext = FileExtension::XML;
		}
		else if (arg == opt_json_file) {
			enc_params->json_file = argv[i];
			enc_params->jumbf_content_type = dbench::JumbfContentType::JSON;
			enc_params->content_box_type = dbench::BoxType::JSON;
			enc_params->content_data_file_ext = FileExtension::JSON;
		}
		else if (arg == opt_uuid_data_file) {
			enc_params->uuid_data_file = argv[i];
			enc_params->jumbf_content_type = dbench::JumbfContentType::UUID;
			enc_params->content_box_type = dbench::BoxType::UUID;
		}
		else if (arg == opt_uuid) {
			string uuid_str = argv[i];
			uuid_str.erase(std::remove(uuid_str.begin(), uuid_str.end(), '-'), uuid_str.end());

			if (uuid_str.size() != 32) {
				cout << "Error: \"" << uuid_str << "\" Number of characters must be 32." << endl;
				exit(EXIT_FAILURE);
			}
			else {
				enc_params->uuid = new unsigned char[16];

				for (int i = 0; i < 16; i++) {
					std::string hexByte = uuid_str.substr(2 * static_cast<std::basic_string<char, std::char_traits<char>, std::allocator<char>>::size_type>(i), 2);
					enc_params->uuid[i] = std::stoi(hexByte, nullptr, 16);
				}
			}
		}
		else if (arg == opt_embed_file) {
			string embed_file_full = argv[i];
			enc_params->embed_file = embed_file_full;
			enc_params->jumbf_content_type = dbench::JumbfContentType::EMBEDDED_File;
		}
		else if (arg == opt_embed_filename) {
			enc_params->embed_file_name = true;
		}
		else if (arg == opt_cbor_data) {
			enc_params->cbor_data_file = argv[i];
			enc_params->jumbf_content_type = dbench::JumbfContentType::CBOR;
			enc_params->content_data_file_ext = get_file_extension(argv[i]);
		}
		else if (arg == opt_media_type)
			enc_params->media_type = argv[i];
		else if (arg == opt_output) {
			enc_params->output_file = argv[i];
			enc_params->output_file_ext = get_file_extension(argv[i]);
		}
		else if (arg == opt_content_type) {
			string type_str = argv[i];
			enc_params->jumbf_content_type = recognise_content_type(type_str);
		}
		else if (arg == opt_free_bytes) {
			enc_params->free_bytes = stoi(argv[i]);
		}
		else if (arg == opt_reference_external) {
			enc_params->reference_external = true;
			i--;
		}
		else if (arg == opt_external_link) {
			enc_params->external_link = argv[i];
		}
		else
		{
			cout << "Un - recongnized parameter : \"" << arg << "\"" << endl;
			exit(EXIT_FAILURE);
		}
	}
}
bool fileExists(const std::string& filePath)
{
	std::ifstream file(filePath);
	return file.good();
}

std::string combineFilePathAndFileName(const std::string& filePath, const std::string& fileName)
{
	char separator = '/';
#ifdef _WIN32
	separator = '\\';
#endif
	if (filePath.back() != separator)
	{
		return filePath + separator + fileName;
	}
	else
	{
		return filePath + fileName;
	}
}
bool contains_directory(const string& path) {
	// Check if the path contains any directory separator
	return path.find('/') != string::npos || path.find('\\') != string::npos;
}
void sanitize_enc_params(CmdParams* params)
{
	bool input_dir_present = params->input_dir.empty() ? false : true;

	if (!params->host_jpg_file.empty()) {
		if (!contains_directory(params->host_jpg_file) && input_dir_present)
			params->host_jpg_file = combineFilePathAndFileName(params->input_dir, params->host_jpg_file);

		if (!fileExists(params->host_jpg_file))
		{
			std::cout << "Host File : \"" << params->host_jpg_file << "\" does not exist." << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	if (!params->codestream_file.empty()) {
		if (!contains_directory(params->codestream_file) && input_dir_present)
			params->codestream_file = combineFilePathAndFileName(params->input_dir, params->codestream_file);
		if (!fileExists(params->codestream_file))
		{
			std::cout << "Codestream File : \"" << params->codestream_file << "\" does not exist." << std::endl;
			exit(EXIT_FAILURE);
		}
		if (params->jumbf_content_type != dbench::JumbfContentType::CODESTREAM) {
			std::cout << "JUMBF Content Type should be set to JP2C." << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	if (!params->hash_file.empty()) {
		if (!contains_directory(params->hash_file) && input_dir_present) {
			params->hash_file = combineFilePathAndFileName(params->input_dir, params->hash_file);
		}
		uint64_t hash_file_size{ 0 };
		dbench::db_read_file_bitstream(params->hash_file, &params->hash, &hash_file_size);
		if (hash_file_size != 32) {
			cout << "Error: Hash Data should be 32 bytes." << endl;
			cout << params->hash_file << " => contains " << hash_file_size << " bytes" << endl;
			exit(EXIT_FAILURE);
		}
	}
	if (!params->xml_file.empty()) {
		if (!contains_directory(params->xml_file) && input_dir_present)
			params->xml_file = combineFilePathAndFileName(params->input_dir, params->xml_file);
		if (!fileExists(params->xml_file))
		{
			std::cout << "XML File : \"" << params->xml_file << "\" does not exist." << std::endl;
			exit(EXIT_FAILURE);
		}
		if (params->jumbf_content_type != dbench::JumbfContentType::XML) {
			std::cout << "JUMBF Content Type should be set to XML." << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	if (!params->json_file.empty()) {
		if (!contains_directory(params->json_file) && input_dir_present)
			params->json_file = combineFilePathAndFileName(params->input_dir, params->json_file);
		if (!fileExists(params->json_file))
		{
			std::cout << "JSON File : \"" << params->json_file << "\" does not exist." << std::endl;
			exit(EXIT_FAILURE);
		}
		if (params->jumbf_content_type != dbench::JumbfContentType::JSON) {
			std::cout << "JUMBF Content Type should be set to JSON." << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	if (!params->uuid_data_file.empty()) {
		if ((!contains_directory(params->uuid_data_file)) && input_dir_present)
			params->uuid_data_file = combineFilePathAndFileName(params->input_dir, params->uuid_data_file);
		if (!fileExists(params->uuid_data_file))
		{
			std::cout << "UUID Data File : \"" << params->uuid_data_file << "\" does not exist." << std::endl;
			exit(EXIT_FAILURE);
		}
		if (params->jumbf_content_type != dbench::JumbfContentType::UUID) {
			std::cout << "JUMBF Content Type should be set to UUID." << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	if (!params->embed_file.empty()) {
		if ((!contains_directory(params->embed_file)) && input_dir_present)
			params->embed_file = combineFilePathAndFileName(params->input_dir, params->embed_file);
		if (!fileExists(params->embed_file))
		{
			std::cout << "File to embed : \"" << params->embed_file << "\" does not exist." << std::endl;
			exit(EXIT_FAILURE);
		}
		if (params->jumbf_content_type != dbench::JumbfContentType::EMBEDDED_File) {
			std::cout << "JUMBF Content Type should be set to EMBEDDED_FILE." << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	if (!params->cbor_data_file.empty()) {
		if ((!contains_directory(params->cbor_data_file)) && input_dir_present)
			params->cbor_data_file = combineFilePathAndFileName(params->input_dir, params->cbor_data_file);
		if (!fileExists(params->cbor_data_file))
		{
			std::cout << "CBOR File : \"" << params->cbor_data_file << "\" does not exist." << std::endl;
			exit(EXIT_FAILURE);
		}
		if (params->jumbf_content_type != dbench::JumbfContentType::CBOR) {
			std::cout << "JUMBF Content Type should be set to CBOR." << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	if (params->private_field_present || params->multiple_private_field) {
		if (!params->private_field_data_file.empty()) {
			if ((!contains_directory(params->private_field_data_file)) && input_dir_present)
				params->private_field_data_file = combineFilePathAndFileName(params->input_dir, params->private_field_data_file);
			if (!fileExists(params->private_field_data_file))
			{
				std::cout << "Private Field data File : \"" << params->private_field_data_file << "\" does not exist." << std::endl;
				exit(EXIT_FAILURE);
			}
		}
		else {
			std::cout << "Private Field data File should be provided." << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	if (params->multiple_private_field) {
		dbench::db_read_file_bitstream(params->private_field_data_file, &params->private_field_data, &params->private_data_size);
		uint32_t lbox = dbench::db_get_4byte(&params->private_field_data);
		uint32_t tbox = dbench::db_get_4byte(&params->private_field_data);
		params->private_field_data -= 8;
		if (tbox != uint32_t(dbench::BoxType::PRIV)) {
			std::cout << "Private Field data should have Tbox = PRIV." << std::endl;
			delete[] params->private_field_data;
			exit(EXIT_FAILURE);
		}
	}
	if (params->private_field_present || params->multiple_private_field) {
		dbench::db_read_file_bitstream(params->private_field_data_file, &params->private_field_data, &params->private_data_size);
	}
	if (!params->hash_file.empty()) {
		//params->hash_file = combineFilePathAndFileName(params->input_dir, params->hash_file);
		if (!fileExists(params->hash_file))
		{
			std::cout << "File containing Hash/Signature : \"" << params->hash_file << "\" does not exist." << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	if (params->jumbf_content_type == dbench::JumbfContentType::EMBEDDED_File) {
		if (params->media_type.empty()) {
			std::cout << "Media Type should be provided." << std::endl;
			exit(EXIT_FAILURE);
		}
		if (params->embed_file_name) {
			if (params->file_name.empty()) {
				params->file_name = extract_file_name(params->embed_file);
			}
		}
		if (params->reference_external) {
			if (params->external_link.empty()) {
				params->external_link = params->embed_file;
			}
		}
	}

	if (params->jumbf_content_type == dbench::JumbfContentType::UUID) {
		if (params->uuid == nullptr) {
			std::cout << "16 bytes UUID should be provided." << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	if (!(params->Lbox == "NULL" || params->Lbox == "1" || params->Lbox == "0")) {
		std::cout << "Value of Lbox provided should be NULL or 1 or 0." << std::endl;
		exit(EXIT_FAILURE);
	}
	if (!params->output_dir.empty()) {
		if (create_folder(params->output_dir)) {
			//std::cout << "Output Folder created successfully!" << std::endl;
		}
		else {
			std::cout << "Failed to create output folder." << std::endl;
			exit(EXIT_FAILURE);
		}
		params->output_file = combineFilePathAndFileName(params->output_dir, params->output_file);
	}

}

void sanitize_parsing_params(CmdParams* params) {
	bool input_dir_present = params->input_dir.empty() ? false : true;

	if (!params->input_file.empty()) {
		if (!contains_directory(params->input_file) && input_dir_present)
			params->input_file = combineFilePathAndFileName(params->input_dir, params->input_file);

		if (!fileExists(params->input_file))
		{
			std::cout << "Host File : \"" << params->input_file << "\" does not exist." << std::endl;
			exit(EXIT_FAILURE);
		}
		params->input_file_ext = get_file_extension(params->input_file);
	}
	if (!params->csv_file.empty()) {
		params->write_to_csv = true;
		//if (!contains_directory(params->csv_file) && input_dir_present)
		//	params->csv_file = combineFilePathAndFileName(params->input_dir, params->csv_file);
	}

}

dbench::JumbfContentType  recognise_content_type(string type_str) {
	string type_str_l = type_str;
	std::transform(type_str_l.begin(), type_str_l.end(), type_str_l.begin(), [](unsigned char c) { return std::tolower(c); });
	if (type_str_l == "xml")
		return dbench::JumbfContentType::XML;
	else if (type_str_l == "json")
		return dbench::JumbfContentType::JSON;
	else if (type_str_l == "cbor")
		return dbench::JumbfContentType::CBOR;
	else if (type_str_l == "embedded_file" || type_str_l == "embedded file")
		return dbench::JumbfContentType::EMBEDDED_File;
	else if (type_str_l == "jp2c")
		return dbench::JumbfContentType::CODESTREAM;
	else if (type_str_l == "uuid")
		return dbench::JumbfContentType::UUID;
	else
		return dbench::JumbfContentType::UNDEF;
}
void parse_csv_row_to_enc_params(vector<string> row, CmdParams* enc_params) {

	string value = row[1];
	// Column B
	enc_params->jumbf_content_type = recognise_content_type(value);

	// Column C
	value = row[2];
	if (value != "NULL") {
		enc_params->host_jpg_file = value;
		enc_params->host_file_ext = get_file_extension(value);
	}

	// Column D
	enc_params->Lbox = row[3];

	// Column E
	value = row[4];
	if (value == "YES" || value == "yes")
		enc_params->is_requestable = true;
	else
		enc_params->is_requestable = false;

	// Column F
	value = row[5];
	if (value != "NULL") {
		enc_params->label = value;
		enc_params->label_present = true;
	}

	// Column G
	value = row[6];
	if (value != "NULL") {
		enc_params->id = stoi(value);
		enc_params->id_present = true;
	}

	// Column H
	value = row[7];
	if (value != "NULL") {
		enc_params->hash_file = value;
		enc_params->hash_present = true;
	}

	// Column I
	value = row[8];
	if (value == "YES" || value == "yes") {
		enc_params->multiple_private_field = true;
	}

	// Column J
	value = row[9];
	if (value != "NULL") {
		enc_params->private_field_data_file = value;
		if (enc_params->multiple_private_field == false)
			enc_params->private_field_present = true;
	}

	// Column K
	value = row[10];
	enc_params->free_bytes = stoi(value);

	// Column L
	enc_params->no_of_content_box = stoi(row[11]);

	// Column M
	value = row[12];
	if (enc_params->jumbf_content_type == dbench::JumbfContentType::XML) {
		enc_params->xml_file = value;
	}
	else if (enc_params->jumbf_content_type == dbench::JumbfContentType::JSON) {
		enc_params->json_file = value;
	}
	else if (enc_params->jumbf_content_type == dbench::JumbfContentType::UUID) {
		enc_params->uuid_data_file = value;
	}
	else if (enc_params->jumbf_content_type == dbench::JumbfContentType::EMBEDDED_File) {
		enc_params->embed_file = value;
	}
	else if (enc_params->jumbf_content_type == dbench::JumbfContentType::CODESTREAM) {
		enc_params->codestream_file = value;
	}
	else if (enc_params->jumbf_content_type == dbench::JumbfContentType::CBOR) {
		enc_params->cbor_data_file = value;
	}

	// column N
	value = row[13];
	if (value != "NULL") {
		string uuid_str = value;
		uuid_str.erase(std::remove(uuid_str.begin(), uuid_str.end(), '-'), uuid_str.end());

		if (uuid_str.size() != 32) {
			cout << "Error: \"" << uuid_str << "\" Number of characters must be 32." << endl;
			exit(EXIT_FAILURE);
		}
		else {
			enc_params->uuid = new unsigned char[16];

			for (int i = 0; i < 16; i++) {
				std::string hexByte = uuid_str.substr(2 * static_cast<std::basic_string<char, std::char_traits<char>, std::allocator<char>>::size_type>(i), 2);
				enc_params->uuid[i] = std::stoi(hexByte, nullptr, 16);
			}
		}
	}

	// Column O
	value = row[14];
	if (value != "NULL") {
		enc_params->uuid_data_file = value;
	}

	// Column P
	value = row[15];
	if (value != "NULL") {
		enc_params->media_type = value;
	}

	// Column Q
	value = row[16];
	if (value != "NULL") {
		enc_params->file_name = value;
		enc_params->embed_file_name = true;
	}

	// Column R
	value = row[17];
	if (value == "YES" || value == "yes") {
		enc_params->reference_external = true;
	}

	// Column S

	value = row[18];
	enc_params->output_file = value;
	enc_params->output_file_ext = get_file_extension(value);

}