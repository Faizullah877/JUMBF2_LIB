#include "db_jumbf_encoder.h"

using namespace dbench;
void modify_lbox(unsigned char** jumbf_buf, uint64_t* jumbf_buf_size, string Lbox_s) {
	uint64_t ex_header{ 8 };
	uint64_t paylaod_size{ 0 };
	uint32_t lbox = db_get_4byte(jumbf_buf); // will be 1 or bigger value
	uint32_t tbox = db_get_4byte(jumbf_buf);
	uint64_t xl_box{ 0 };
	if (lbox == 1) {
		xl_box = db_get_8byte(jumbf_buf);
		ex_header += 8;
	}
	paylaod_size = (*jumbf_buf_size) - ex_header;

	if (Lbox_s == "NULL") {
		*jumbf_buf -= ex_header;
		return;
	}
	else if (Lbox_s == "0") {
		if (xl_box == 0) {// only updating lbox bytes to zero
			*jumbf_buf -= 8;
			db_put_4byte(jumbf_buf, 0);
			*jumbf_buf -= 4;
			return;
		}
		else { // remove xl_box field from old buf
			unsigned char* new_buf{ nullptr };
			uint64_t new_size = (*jumbf_buf_size) - 8;
			new_buf = new unsigned char[new_size];
			db_put_4byte(&new_buf, 0);
			db_put_4byte(&new_buf, tbox);
			//memcpy_s(new_buf, new_size, *jumbf_buf, size_t(paylaod_size));
			memcpy(new_buf,  *jumbf_buf, size_t(paylaod_size));
			*jumbf_buf -= ex_header;
			delete[] jumbf_buf;
			new_buf -= 8;
			*jumbf_buf = new_buf;
			*jumbf_buf_size = new_size;
		}
	}
	else if (Lbox_s == "1") { // xl_box is needed

		if (lbox == 1) // xl_box is already there 
			return;
		else { // change lbox to 1 and add xl_box 
			unsigned char* new_buf{ nullptr };
			uint64_t new_size{ (*jumbf_buf_size) };
			new_size += 8;
			new_buf = new unsigned char[new_size];
			db_put_4byte(&new_buf, 1);
			db_put_4byte(&new_buf, tbox);
			db_put_8byte(&new_buf, static_cast<uint64_t>(new_size));
			//memcpy_s(new_buf, new_size, *jumbf_buf, rsize_t(paylaod_size));
			memcpy(new_buf,  *jumbf_buf, size_t(paylaod_size));
			*jumbf_buf -= ex_header;
			delete[] * jumbf_buf;
			new_buf -= 16;
			*jumbf_buf = new_buf;
			*jumbf_buf_size = new_size;
		}
	}
}
int db_perform_encoding(CmdParams args)
{

	// make private box if required.
	DbBox* priv_box = nullptr;
	if (args.private_field_present || args.multiple_private_field) {
		uint64_t private_data_size{ 0 };
		// private field data is considered as already in single box bitstream or a single superbox bitstream.

		db_read_file_bitstream(args.private_field_data_file, &(args.private_field_data), &private_data_size);

		unsigned char* data = args.private_field_data;
		priv_box = new DbBox;
		uint64_t header_size{ 8 };

		uint32_t lbox = db_get_4byte(&data);
		uint32_t tbox = db_get_4byte(&data);
		uint64_t xl_box{ 0 };

		if (lbox == 1) {
			xl_box = db_get_8byte(&data);
			header_size += 8;
		}

		priv_box->set_box(tbox, data, private_data_size - header_size);
	}

	// make Jumb Description box
	DbJumbDescBox desc_box;
	desc_box.set_box(args.jumbf_content_type, args.is_requestable, args.label, args.id_present, args.id, args.hash, priv_box);

	//unsigned char* jumd_buf{ nullptr };
	//uint64_t jmd_size{ 0 };
	//desc_box.serialize(&jumd_buf, &jmd_size);

	//DbJumbDescBox rec_desc;
	//rec_desc.deserialize(jumd_buf, jmd_size);


	// make padding box if required
	DbFreeBox padding_box;
	bool padding_box_present{ false };
	if (args.free_bytes > 0) {
		padding_box.set_box(args.free_bytes);
		padding_box_present = true;
	}

	unsigned char* jumbf_buf{ nullptr };
	uint64_t jumbf_buf_size{ 0 };

	// make JUMBF and get serialized data.
	if (args.jumbf_content_type == JumbfContentType::CODESTREAM) {
		DbCodestreamBox codestream_box;
		unsigned char* codestream_payload{ nullptr };
		uint64_t payload_size{ 0 };
		db_read_file_bitstream(args.codestream_file, &codestream_payload, &payload_size);
		codestream_box.set_box(codestream_payload, payload_size);

		DbCodestreamJumbBox codestream_jumb(desc_box, codestream_box);
		if (padding_box_present)
			codestream_jumb.set_free_box(padding_box);
		codestream_jumb.serialize(&jumbf_buf, &jumbf_buf_size);
		delete[] codestream_payload;
	}
	else if (args.jumbf_content_type == JumbfContentType::XML) {
		unsigned char* xml_data = nullptr;
		uint64_t xml_size = 0;
		db_read_file_bitstream(args.xml_file, &xml_data, &xml_size);
		DbXmlBox xml_content_box((char*)xml_data, xml_size);
		DbXmlJumbBox xml_jumbf_box(desc_box, xml_content_box);

		if (padding_box_present)
			xml_jumbf_box.set_free_box(padding_box);

		xml_jumbf_box.serialize(&jumbf_buf, &jumbf_buf_size);
		delete[] xml_data;
	}
	else if (args.jumbf_content_type == JumbfContentType::JSON) {
		unsigned char* json_data = nullptr;
		uint64_t json_size = 0;
		db_read_file_bitstream(args.json_file, &json_data, &json_size);
		DbJsonBox json_content_box((char*)json_data, json_size);
		DbJsonJumbBox json_jumbf_box(desc_box, json_content_box);

		if (padding_box_present)
			json_jumbf_box.set_free_box(padding_box);

		json_jumbf_box.serialize(&jumbf_buf, &jumbf_buf_size);
		delete[] json_data;
	}
	else if (args.jumbf_content_type == JumbfContentType::UUID) {
		unsigned char* uuid_paylaod_data = nullptr;
		uint64_t uuid_paylaod_size = 0;
		db_read_file_bitstream(args.uuid_data_file, &uuid_paylaod_data, &uuid_paylaod_size);
		DbUuidBox uuid_content_box(args.uuid, uuid_paylaod_data, uuid_paylaod_size);
		DbUuidJumbBox uuid_jumbf_box(desc_box, uuid_content_box);

		if (padding_box_present)
			uuid_jumbf_box.set_free_box(padding_box);

		uuid_jumbf_box.serialize(&jumbf_buf, &jumbf_buf_size);
		delete[] uuid_paylaod_data;
	}
	else if (args.jumbf_content_type == JumbfContentType::EMBEDDED_File) {
		// first make file description box
		DbFileDescBox file_desc_box(args.media_type, args.file_name, args.reference_external);

		// binary file data box
		unsigned char* bfdb_paylaod_data = nullptr;
		uint64_t bfdb_paylaod_size = 0;
		db_read_file_bitstream(args.embed_file, &bfdb_paylaod_data, &bfdb_paylaod_size);
		DbBinaryDataBox binary_file_box(bfdb_paylaod_data, bfdb_paylaod_size);

		// prepared Embedded File content JUMBF box
		DbEmbdFileJumbBox embd_file_jumb_box(desc_box, file_desc_box, binary_file_box);

		if (padding_box_present)
			embd_file_jumb_box.set_free_box(padding_box);

		embd_file_jumb_box.serialize(&jumbf_buf, &jumbf_buf_size);
		delete[] bfdb_paylaod_data;
	}
	else if (args.jumbf_content_type == JumbfContentType::CBOR) {
		unsigned char* cbor_data = nullptr;
		uint64_t cbor_size = 0;
		db_read_file_bitstream(args.cbor_data_file, &cbor_data, &cbor_size);
		DbCborBox cbor_content_box((char*)cbor_data, cbor_size);
		DbCborJumbBox cbor_jumbf_box(desc_box, cbor_content_box);

		if (padding_box_present)
			cbor_jumbf_box.set_free_box(padding_box);

		cbor_jumbf_box.serialize(&jumbf_buf, &jumbf_buf_size);
		delete[] cbor_data;
	}


	//DbJumbBox jumb_box;
	//jumb_box.deserialize(jumbf_buf, jumbf_buf_size);

	modify_lbox(&jumbf_buf, &jumbf_buf_size, args.Lbox);
	if (args.output_file_ext == FileExtension::JUMBF) {
		db_write_file_bitstream(args.output_file, jumbf_buf, jumbf_buf_size);
	}
	else if (args.output_file_ext == FileExtension::JPG) {
		unsigned char* output_buf{ nullptr };
		uint64_t output_size{ 0 };
		unsigned char* input_jpg{ nullptr };
		uint64_t in_jpg_size{ 0 };
		db_read_file_bitstream(args.host_jpg_file, &input_jpg, &in_jpg_size);
		db_write_jumbf_buf_to_jpg_buf(jumbf_buf, jumbf_buf_size, input_jpg, in_jpg_size, &output_buf, &output_size);
		db_write_file_bitstream(args.output_file, output_buf, output_size);
		delete[] output_buf;
		delete[] jumbf_buf;
		delete[] input_jpg;
	}
	return 0;
}
