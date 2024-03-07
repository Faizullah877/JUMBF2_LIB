// db_jumbf_app.cpp : This file contains the 'main' function. Program execution begins and ends there.


#include "dbench_jumbf_app.h"
using namespace std;
using namespace dbench;




int main(int argc, const char* argv[])
{
	if (argc < 2) {
		cout << "Error: input arguments are not enough." << endl;
		print_usage();
		return -1;
	}
	try
	{
		auto start1 = chrono::high_resolution_clock::now();
		auto start_time = chrono::system_clock::to_time_t(chrono::system_clock::now());
		WorkType task = WorkType::UNKNOWN;
		for (int i = 1; i < argc; i++) {
			std::string arg = argv[i];
			if (arg == opt_type_encoding) {
				task = WorkType::EMBED;
				break;
			}
			else if (arg == opt_type_decoding) {
				task = WorkType::EXTRACT;
				break;
			}
			else if (arg == opt_type_parse) {
				task = WorkType::PARSE;
				break;
			}
			else if (arg == "-h" || arg == "-help") {
				print_usage();
				exit(EXIT_SUCCESS);
			}
			else if (arg == "-v" || arg == "-version") {
				print_app_version();
				exit(EXIT_SUCCESS);
			}
		}
		//int retrn;
		if (task == WorkType::EMBED) {
			CmdParams enc_params;
			parse_enc_params(argc, argv, &enc_params);
			if (!enc_params.csv_file.empty()) {
				string csv_full_path;
				if ((!enc_params.input_dir.empty()) && (!contains_directory(enc_params.csv_file))) {
					csv_full_path = combineFilePathAndFileName(enc_params.input_dir, enc_params.csv_file);
				}
				else
					csv_full_path = enc_params.csv_file;

				int total_rows = countRowsInCSV(csv_full_path);
				if (enc_params.row_number != -1) {
					if (enc_params.row_number > total_rows) {
						std::cout << "Row Number " << enc_params.row_number << " is greater than Total Number of rows " << total_rows << ". " << std::endl;
						exit(EXIT_FAILURE);
					}
					std::ifstream file(csv_full_path);
					std::vector<std::string> row = getLineFromCSV(file, enc_params.row_number);
					parse_csv_row_to_enc_params(row, &enc_params);
					sanitize_enc_params(&enc_params);
					print_enc_params(&enc_params);
					int retrn = db_perform_encoding(enc_params);
					reset_enc_params(&enc_params);
				}
				else {
					for (int i = 2; i <= total_rows; i++) {
						cout << "Row Number : " << i << "   => ";
						std::ifstream file(csv_full_path);
						std::vector<std::string> row = getLineFromCSV(file, i);
						parse_csv_row_to_enc_params(row, &enc_params);
						sanitize_enc_params(&enc_params);
						print_enc_params(&enc_params);
						int retrn = db_perform_encoding(enc_params);
						reset_enc_params(&enc_params);

					}
				}
			}
			else {
				sanitize_enc_params(&enc_params);
				print_enc_params(&enc_params);
				int retrn = db_perform_encoding(enc_params);
				reset_enc_params(&enc_params);
			}


		}
		else if (task == WorkType::EXTRACT) {
			//retrn = db_perform_decoding(argc, argv);
			cout << "Extract Functionality is not implemented yet." << endl;
		}
		else if (task == WorkType::PARSE) {
			CmdParams parse_params;
			parse_enc_params(argc, argv, &parse_params);
			sanitize_parsing_params(&parse_params);
			db_parse(parse_params);
		}
		else {
			cout << "Error: Unkown worktype." << endl;
			cout << "       Please provide switch \"-encode\" or \"-extract\"." << endl;
			print_usage();
		}

		auto finish = chrono::high_resolution_clock::now();
		auto elapsed = finish - start1;
		cout << "Process Time : " << dec << elapsed / std::chrono::milliseconds(1) / 1000.0 << " seconds" << endl;
		exit(EXIT_SUCCESS);

	}
	catch (const std::exception& e)
	{
		std::cout << " a standard exception was caught, with message '"
			<< e.what() << "'\n";
	}
	return 0;

}
