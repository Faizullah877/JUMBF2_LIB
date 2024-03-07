#include <iostream>
#include <algorithm>
#include <cctype>

#include "db_jumbf_parser.h"
#include <string>
#include "../dbench_jumbf_lib/db_define.h"
#include <iomanip>

using namespace std;
using namespace dbench;

#ifdef WIN32
#include <windows.h> // For Windows-specific directory handling
#else
#include <dirent.h>  // For POSIX directory handling
#endif

std::string toUpper(const std::string& input) {

	string type_str_l = input;
	std::transform(type_str_l.begin(), type_str_l.end(), type_str_l.begin(), [](unsigned char c) { return std::toupper(c); });
	return type_str_l;
}
bool has_extension(const std::string& filename, const std::string& ext) {
	size_t pos = filename.find_last_of('.');
	if (pos != std::string::npos) {
		std::string file_ext = filename.substr(pos + 1);
		std::transform(file_ext.begin(), file_ext.end(), file_ext.begin(), ::tolower);
		return (file_ext == ext);
	}
	return false;
}
void writeVectorToCsv(const std::vector<std::string>& data, const std::string& filename) {
	std::ofstream outputFile(filename, std::ios::app); // Open in append mode

	if (!outputFile.is_open()) {
		std::cerr << "Error opening file: " << filename << std::endl;
		return;
	}

	// Write data rows
	for (const auto& str : data) {
		outputFile << str;
		if (&str != &data.back()) {
			outputFile << ",";
		}
	}
	outputFile << std::endl;

	outputFile.close();
}

string get_uuid_as_str(unsigned char* uuid) {
	stringstream str;
	for (auto i = 0; i < 16; i++) {
		str << setw(2) << setfill('0') << hex << int(uuid[i]);
	}
	return str.str();
}
string get_hash_as_str(unsigned char* hash) {
	stringstream str;
	for (auto i = 0; i < 32; i++) {
		str << setw(2) << setfill('0') << hex << int(hash[i]);
	}
	return str.str();
}
string get_box_type_str(dbench::BoxType type)
{
	switch (type)
	{
	case dbench::BoxType::JUMB:
		return "jumb";
		break;
	case dbench::BoxType::JUMD:
		return "jumd";
		break;
	case dbench::BoxType::JP2C:
		return "jp2c";
		break;
	case dbench::BoxType::XML:
		return "xml";
		break;
	case dbench::BoxType::JSON:
		return "json";
		break;
	case dbench::BoxType::UUID:
		return "uuid";
		break;
	case dbench::BoxType::BIDB:
		return "bidb";
		break;
	case dbench::BoxType::BFDB:
		return "bfdb";
		break;
	case dbench::BoxType::CBOR:
		return "cbor";
		break;
	case dbench::BoxType::FREE:
		return "free";
		break;
	case dbench::BoxType::PRIV:
		return "priv";
		break;
	case dbench::BoxType::UNDEF:
		return "undefined";
		break;
	default:
		return "undefined";
		break;
	}
}

void print_on_screen(DbJumbBox jumb, CmdParams params);
void parse_to_csv_file(DbJumbBox jumb, CmdParams params, string fname);


std::vector<std::string> get_files_in_directory(const std::string& directory) {
	std::vector<std::string> file_paths;

#ifdef WIN32
	// Windows-specific code
	WIN32_FIND_DATA file_data;
	HANDLE dir = FindFirstFile((directory + "\\*").c_str(), &file_data);
	if (dir != INVALID_HANDLE_VALUE) {
		do {
			if (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				continue; // Skip directories
			std::string filename = file_data.cFileName;
			if (has_extension(filename, "jpg") || has_extension(filename, "jpeg") || has_extension(filename, "jumbf")) {
				file_paths.push_back(directory + "\\" + filename);
			}
		} while (FindNextFile(dir, &file_data));
		FindClose(dir);
	}
#else
	// POSIX (Linux) code
	DIR* dir = opendir(directory.c_str());
	if (dir) {
		struct dirent* entry;
		while ((entry = readdir(dir)) != nullptr) {
			if (entry->d_type == DT_REG) { // Regular file
				std::string filename = entry->d_name;
				if (has_extension(filename, "jpg") || has_extension(filename, "jpeg") || has_extension(filename, "jumbf")) {
					file_paths.push_back(directory + "/" + filename);
				}
			}
		}
		closedir(dir);
	}
#endif

	return file_paths;
}


void parse_file(string input_file, CmdParams params) {

	FileExtension file_ext = get_file_extension(input_file);

	unsigned char* jumbf_buf{ nullptr };
	uint64_t jumbf_size{ 0 };


	vector<unsigned char*> jumb_buffers;
	vector<uint64_t> jumb_buf_sizes;

	if (file_ext == FileExtension::JUMBF) {
		db_read_file_bitstream(input_file, &jumbf_buf, &jumbf_size);
		jumb_buffers.push_back(jumbf_buf);
		jumb_buf_sizes.push_back(jumbf_size);
	}
	else {
		unsigned char* jpg_buf = nullptr;
		uint64_t jpg_buf_size = 0;
		db_read_file_bitstream(input_file, &jpg_buf, &jpg_buf_size);
		int x = db_extract_jumbfs_from_jpg1(jpg_buf, jpg_buf_size, jumb_buffers, jumb_buf_sizes);
		if (x == -1) {
			cout << "Problem encountered in extracting JUMBF bitstream." << endl;
			delete[] jpg_buf;
			return;
		}
		delete[] jpg_buf;
	}
	size_t no_of_jumbfs = jumb_buffers.size();
	if (no_of_jumbfs == 0)
		cout << "JUMBF box not present in input file." << endl;

	for (auto i = 0; i < no_of_jumbfs; i++) {
		DbJumbBox* jumb_box = new DbJumbBox;
		jumb_box->deserialize(jumb_buffers[i], jumb_buf_sizes[i]);

		if (params.write_to_csv) {
			parse_to_csv_file(*jumb_box, params, extract_file_name(input_file));
		}
		else
			print_on_screen(*jumb_box, params);

		delete[] jumb_buffers[i];
	}
}

void db_parse(CmdParams const params)
{
	if (params.write_to_csv) {
		vector<string> header_row{ "File Name", "Is Standalone?", "Lbox", "XLbox", "Content Type UUID", "Description Box Toggle", "Is requestable?", "Label", "ID", "SHA256 Hash", "Private Field Exits?", "UUID box UUID field", "Embedded File Description Box Toggle",
	"Embedded File Description Box Media Type", "Embedded File Description Box File Name", "Embedded File Description box Content Referenced Externally","Content Box TBox field","Content Box Size", "JUMBF Box Padding box Size" };
		writeVectorToCsv(header_row, params.csv_file);
	}
	if (params.input_file.empty()) {// full directory
		vector<string> files_list = get_files_in_directory(params.input_dir);
		for (int fno = 0; fno < files_list.size(); fno++)
		{
			string filename = combineFilePathAndFileName(params.input_dir, files_list[fno]);
			cout << "Parsing File No " << setw(6) << fno << "  => File : " << filename << endl;
			parse_file(filename, params);
		}
	}
	else {
		parse_file(params.input_file, params);
	}



}

void print_on_screen(DbJumbBox jumb_box, CmdParams params) {
	cout << "Input File  :               [" << params.input_file << "]" << endl;

	cout << "JUMBF Box." << endl;
	cout << "   Lbox :                   [" << to_string(jumb_box.get_lbox()) << "]" << endl;
	cout << "   Tbox :                   [0x" << jumb_box.get_tbox() << "]   [" << get_box_type_str(jumb_box.get_box_type()) << "]" << endl;
	if (jumb_box.is_xl_box_present()) {
		cout << "   XLbox :                  [" << jumb_box.get_xl_box() << "]" << endl;
	}

	cout << "   JUMBF Description Box." << endl;
	cout << "      Lbox :                [" << to_string(jumb_box.desc_box_.get_lbox()) << "]" << endl;
	cout << "      Tbox :                [0x" << hex << jumb_box.desc_box_.get_tbox() << dec << "]   [" << get_box_type_str(jumb_box.desc_box_.get_box_type()) << "]" << endl;
	if (jumb_box.desc_box_.is_xl_box_present()) {
		cout << "      XLbox :                [" << jumb_box.desc_box_.get_xl_box() << "]" << endl;
	}

	cout << "      Content Type UUID :   [";
	print_uuid_as_str(jumb_box.desc_box_.get_type_16bytes());
	JumbfContentType jumb_content_type = jumb_box.desc_box_.get_jumb_content_type();
	cout << "]   [" << get_jumbf_content_type_str(jumb_content_type) << "]" << endl;

	cout << "      Toogles Byte :        [0x" << setw(2) << setfill('0') << hex << int(jumb_box.desc_box_.get_toggles_byte()) << dec << "]" << endl;
	cout << "      Is Requestable :      [" << (jumb_box.desc_box_.is_requestable() ? "TRUE" : "FALSE") << "]" << endl;
	cout << "      Label :               [" << (jumb_box.desc_box_.is_label_present() ? jumb_box.desc_box_.get_label() : "NOT PRESENT") << "]" << endl;
	cout << "      ID :                  [" << (jumb_box.desc_box_.is_id_present() ? to_string(jumb_box.desc_box_.get_id()) : "NOT PRESENT") << "]" << endl;

	string hash_str = "NOT PRESENT";
	if (jumb_box.desc_box_.is_hash_present()) {
		hash_str = get_hash_as_str(jumb_box.desc_box_.get_hash());
		hash_str = toUpper(hash_str);
	}
	cout << "      Hash/Signature :      [" << hash_str << "]" << endl;

	bool private_field = jumb_box.desc_box_.is_private_box_present();
	cout << "      Private Field  :      [" << (private_field ? "PRESENT" : "NOT PRESENT") << "]" << endl;
	if (private_field) {
		DbBox* private_box = jumb_box.desc_box_.get_private_box();
		cout << "        Lbox :              [" << private_box->get_lbox() << "]" << endl;
		cout << "        Tbox :              [0x" << hex << private_box->get_tbox() << dec << "]   [" << get_box_type_str(private_box->get_box_type()) << "]" << endl;
		if (private_box->is_xl_box_present()) {
			cout << "        XLbox :              [" << private_box->get_xl_box() << "]" << endl;
		}
		cout << "        Payload Data Size   [" << private_box->get_payload_size() << "]" << endl;
		delete private_box;
	}

	cout << "   JUMBF Content Boxes      [" << jumb_box.content_boxes_.size() << "]" << endl;
	int no = 0;
	//cout << "     No of Content Boxes    [" << jumb_box.content_boxes_.size() << "]" << endl;
	for (auto& box : jumb_box.content_boxes_) {

		cout << "     Content Box No :       [" << ++no << "]" << endl;
		cout << "      Lbox :                [" << box.get_lbox() << "]" << endl;
		cout << "      Tbox :                [0x" << hex << box.get_tbox() << dec << "   [" << get_box_type_str(box.get_box_type()) << "]" << endl;
		if (box.is_xl_box_present()) {
			cout << "      XLbox :                  [" << box.get_xl_box() << "]" << endl;
		}
		cout << "      Payload Data Size     [" << box.get_payload_size() << "]" << endl;
		if (jumb_content_type == JumbfContentType::UUID) {
			cout << "     Content Data UUID         [";
			print_uuid_as_str(box.get_payload());
			cout << "]" << endl;
		}
		if (jumb_content_type == JumbfContentType::EMBEDDED_File && box.get_box_type() == BoxType::BFDB) {
			string embedd_file_toggle_str;
			string media_type_str = "NULL";
			string embedded_file_name_str = "NOT PRESENT";
			string embeddd_external_str = "NULL";

			DbFileDescBox desc_box;
			desc_box.deserialize(box.get_payload() - (box.is_xl_box_present() ? 16 : 8), box.get_box_size());

			embedd_file_toggle_str = to_string(desc_box.get_toggle_byte());

			media_type_str = desc_box.get_media_type();

			if (!desc_box.get_file_name().empty())
				embedded_file_name_str = desc_box.get_file_name();

			if (desc_box.is_externally_referenced())
				embeddd_external_str = "TRUE";
			else
				embeddd_external_str = "FALSE";

			cout << "      Toggles               [0x" << setfill('0') << int(desc_box.get_toggle_byte()) << "]" << endl;
			cout << "      Media Type            [" << media_type_str << "]" << endl;
			cout << "      File Name             [" << embedded_file_name_str << "]" << endl;
			cout << "      External referrenced? [" << embeddd_external_str << "]" << endl;
		}
	}

	cout << "   JUMBF Padding Box" << endl;
	bool padding_box_present = jumb_box.padding_box_present_;
	cout << "      Padding  Box  :       [" << (padding_box_present ? "PRESENT" : "NOT PRESENT") << "]" << endl;
	if (padding_box_present) {
		cout << "      Lbox :                [" << jumb_box.padding_box_.get_lbox() << "]" << endl;
		cout << "      Tbox :                [0x" << hex << jumb_box.padding_box_.get_tbox() << dec << "]   [" << get_box_type_str(jumb_box.desc_box_.get_box_type()) << "]" << endl;
		if (jumb_box.padding_box_.is_xl_box_present()) {
			cout << "      XLbox :                [" << jumb_box.padding_box_.get_xl_box() << "]" << endl;
		}
		cout << "      Payload Data Size     [" << jumb_box.padding_box_.get_payload_size() << "]" << endl;
	}

}

void parse_to_csv_file(DbJumbBox jumb_box, CmdParams params, string fname) {
	vector<string> csv_row;
	// Column A => file Name
	csv_row.push_back(extract_file_name(fname));

	FileExtension ext = get_file_extension(fname);

	// Column B => is_standalone
	csv_row.push_back(ext == FileExtension::JUMBF ? "TRUE" : "FALSE");

	// Column C => Lbox
	csv_row.push_back(to_string(jumb_box.get_lbox()));

	// Column D => XLbox
	csv_row.push_back(jumb_box.is_xl_box_present() ? to_string(jumb_box.get_xl_box()) : "NULL");

	// Column E => Content Type UUID
	csv_row.push_back(toUpper(get_uuid_as_str(jumb_box.desc_box_.get_type_16bytes())));

	// Column F => toggle 
	csv_row.push_back(to_string(jumb_box.desc_box_.get_toggles_byte()));

	// Column G => is Requestable
	csv_row.push_back(jumb_box.desc_box_.is_requestable() ? "TRUE" : "FALSE");

	// Column H => Label
	csv_row.push_back(jumb_box.desc_box_.is_label_present() ? jumb_box.desc_box_.get_label() : "NULL");

	// Column I => ID
	csv_row.push_back(jumb_box.desc_box_.is_id_present() ? to_string(jumb_box.desc_box_.get_id()) : "NULL");

	// Column J => SHA256 Hash
	if (jumb_box.desc_box_.is_hash_present()) {
		string hash_str = get_hash_as_str(jumb_box.desc_box_.get_hash());
		csv_row.push_back(toUpper(hash_str));
	}
	else
		csv_row.push_back("NULL");

	// Column K => Private field present?
	csv_row.push_back(jumb_box.desc_box_.is_private_box_present() ? "TRUE" : "FALSE");

	// Column L => UUID box UUID field
	DbBox content_box = jumb_box.content_boxes_.front();

	if (jumb_box.desc_box_.get_jumb_content_type() == JumbfContentType::UUID) {
		string uuid_str = get_uuid_as_str(content_box.get_payload());
		csv_row.push_back(toUpper(uuid_str));
	}
	else
		csv_row.push_back("NULL");

	string embedd_file_toggle_str = "NULL";
	string media_type_str = "NULL";
	string embedded_file_name_str = "NULL";
	string embeddd_external_str = "NULL";
	string content_tbox_str = "NULL";
	string content_box_size_str = "";

	if (jumb_box.desc_box_.get_jumb_content_type() == JumbfContentType::EMBEDDED_File) {
		content_box = jumb_box.content_boxes_.front();

		unsigned char* buf = content_box.get_payload() - 8;
		DbFileDescBox desc_box;
		desc_box.deserialize(buf, content_box.get_box_size());

		embedd_file_toggle_str = to_string(desc_box.get_toggle_byte());

		media_type_str = desc_box.get_media_type();

		if (!desc_box.get_file_name().empty())
			embedded_file_name_str = desc_box.get_file_name();

		if (desc_box.is_externally_referenced())
			embeddd_external_str = "TRUE";
		else
			embeddd_external_str = "FALSE";
		DbBox box = jumb_box.content_boxes_.back();
		if (box.get_box_type() == BoxType::BIDB) {
			content_tbox_str = "bidb";
			content_box_size_str = to_string(box.get_box_size());
		}
	}
	else {
		content_tbox_str = get_box_type_str(content_box.get_box_type());
		content_box_size_str = to_string(content_box.get_box_size());
	}
	csv_row.push_back(embedd_file_toggle_str);
	csv_row.push_back(media_type_str);
	csv_row.push_back(embedded_file_name_str);
	csv_row.push_back(embeddd_external_str);
	csv_row.push_back(content_tbox_str);
	csv_row.push_back(content_box_size_str);

	csv_row.push_back(jumb_box.padding_box_present_ ? to_string(jumb_box.padding_box_.get_box_size()) : "NULL");
	writeVectorToCsv(csv_row, params.csv_file);
}
