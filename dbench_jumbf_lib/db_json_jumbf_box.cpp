
#include "db_json_jumbf_box.h"



namespace dbench {
	DbJsonJumbBox::DbJsonJumbBox()
	{
		desc_box_.set_content_type(JumbfContentType::JSON);
		set_box_size();
	}

	DbJsonJumbBox::~DbJsonJumbBox()
	{
		delete[] desc_box_.get_type_16bytes();
	}

	DbJsonJumbBox::DbJsonJumbBox(DbJsonBox json_box)
	{
		desc_box_.set_content_type(JumbfContentType::JSON);
		json_content_box_ = json_box;
		set_box_size();
	}
	DbJsonJumbBox::DbJsonJumbBox(DbJsonBox json_box, DbFreeBox pading_box)
	{
		desc_box_.set_content_type(JumbfContentType::JSON);
		json_content_box_ = json_box;
		padding_box_ = pading_box;
		set_box_size();
	}
	DbJsonJumbBox::DbJsonJumbBox(DbJumbDescBox desc_box)
	{
		desc_box_.set_content_type(JumbfContentType::JSON);
		set_jumbf_description_box(desc_box);
		set_box_size();
	}
	DbJsonJumbBox::DbJsonJumbBox(DbJumbDescBox desc_box, DbJsonBox json_box)
	{
		desc_box_.set_content_type(JumbfContentType::JSON);
		set_jumbf_description_box(desc_box);
		json_content_box_ = json_box;
		set_box_size();
	}
	void DbJsonJumbBox::set_content_box(DbJsonBox json_box)
	{
		json_content_box_ = json_box;
		set_box_size();
	}
	void DbJsonJumbBox::set_description_box(bool requestable, std::string label, bool id_present, uint32_t id, unsigned char* hash, DbBox* private_box)
	{
		desc_box_.set_box(JumbfContentType::JSON, requestable, label, id_present, id, hash, private_box);
		set_box_size();
	}
	void DbJsonJumbBox::set_box_size()
	{
		box_size_ = 8;
		box_size_ += desc_box_.get_box_size();
		box_size_ += json_content_box_.get_box_size();
		if (padding_box_present_) {
			box_size_ += padding_box_.get_box_size();
		}

		if (box_size_ > MAX_32BIT_UINT_VALUE)
			box_size_ += (static_cast<uint64_t>(8)); // 8 xl_box


		if (box_size_ > MAX_32BIT_UINT_VALUE)
		{
			lbox_ = 1;
			xl_box_ = box_size_;
			xl_box_present_ = true;
		}
		else {
			lbox_ = static_cast<uint32_t>(box_size_);
			xl_box_present_ = false;
		}
	}
	void DbJsonJumbBox::serialize(unsigned char** out_buf, uint64_t* out_buf_size)
	{

		set_box_size();

		*out_buf = new unsigned char[box_size_];
		*out_buf_size = box_size_;
		unsigned char* dst_buf_position = *out_buf;

		uint64_t bytes_done = 0;

		db_put_4byte(&dst_buf_position, lbox_);
		db_put_4byte(&dst_buf_position, tbox_);
		bytes_done += 8;
		if (xl_box_present_) {
			db_put_8byte(&dst_buf_position, xl_box_);
			bytes_done += 8;
		}

		// description box
		uint64_t desc_box_buf_size = 0;
		unsigned char* jumd_buf = nullptr;
		desc_box_.serialize(&jumd_buf, &desc_box_buf_size);
		//memcpy_s(dst_buf_position, box_size_, jumd_buf, desc_box_buf_size);
		memcpy(dst_buf_position, jumd_buf, desc_box_buf_size);
		dst_buf_position += desc_box_buf_size;
		delete[] jumd_buf;
		bytes_done += desc_box_buf_size;

		// json content box
		unsigned char* json_box_buf{ nullptr };
		uint64_t json_box_size{ 0 };
		json_content_box_.serialize(&json_box_buf, &json_box_size);
		//memcpy_s(dst_buf_position, box_size_, json_box_buf, json_box_size);
		memcpy(dst_buf_position, json_box_buf, json_box_size);
		dst_buf_position += json_box_size;
		delete[] json_box_buf;
		bytes_done += json_box_size;

		if (padding_box_present_) {
			unsigned char* free_box_buf{ nullptr };
			uint64_t free_box_size{ 0 };
			padding_box_.serialize(&free_box_buf, &free_box_size);
			//memcpy_s(dst_buf_position, box_size_, free_box_buf, free_box_size);
			memcpy(dst_buf_position, free_box_buf, free_box_size);
			dst_buf_position += free_box_size;
			delete[] free_box_buf;
			bytes_done += free_box_size;
		}

		if (bytes_done != box_size_) {
			throw std::runtime_error("Error: Serializing JSON JUMBF, sizes not matches.");
		}
	}
}