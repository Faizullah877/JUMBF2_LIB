
#include "db_codestream_jumbf_box.h"



namespace dbench {
	DbCodestreamJumbBox::DbCodestreamJumbBox()
	{
		desc_box_.set_content_type(JumbfContentType::CODESTREAM);
		set_box_size();
	}

	DbCodestreamJumbBox::~DbCodestreamJumbBox()
	{
		delete[] desc_box_.get_type_16bytes();
	}

	DbCodestreamJumbBox::DbCodestreamJumbBox(DbCodestreamBox codestream_box)
	{
		desc_box_.set_content_type(JumbfContentType::CODESTREAM);
		codestream_content_box_ = codestream_box;
		set_box_size();
	}
	DbCodestreamJumbBox::DbCodestreamJumbBox(DbCodestreamBox codestream_box, DbFreeBox pading_box)
	{
		desc_box_.set_content_type(JumbfContentType::CODESTREAM);
		codestream_content_box_ = codestream_box;
		padding_box_ = pading_box;

		set_box_size();
	}
	DbCodestreamJumbBox::DbCodestreamJumbBox(DbJumbDescBox desc_box)
	{
		desc_box_.set_content_type(JumbfContentType::CODESTREAM);
		set_jumbf_description_box(desc_box);
		set_box_size();
	}
	DbCodestreamJumbBox::DbCodestreamJumbBox(DbJumbDescBox desc_box, DbCodestreamBox codestream_box)
	{
		desc_box_.set_content_type(JumbfContentType::CODESTREAM);
		set_jumbf_description_box(desc_box);
		codestream_content_box_ = codestream_box;
		set_box_size();
	}
	void DbCodestreamJumbBox::set_content_box(DbCodestreamBox codestream_box)
	{
		codestream_content_box_ = codestream_box;
		set_box_size();
	}
	void DbCodestreamJumbBox::set_description_box(bool requestable, std::string label, bool id_present, uint32_t id, unsigned char* hash, DbBox* private_box)
	{
		desc_box_.set_box(JumbfContentType::CODESTREAM, requestable, label, id_present, id, hash, private_box);
		set_box_size();
	}
	void DbCodestreamJumbBox::set_box_size()
	{
		box_size_ = 8;
		box_size_ += desc_box_.get_box_size();
		box_size_ += codestream_content_box_.get_box_size();
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
	void DbCodestreamJumbBox::serialize(unsigned char** out_buf, uint64_t* out_buf_size)
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

		// jp2c content box
		unsigned char* jp2c_box_buf{ nullptr };
		uint64_t jp2c_box_size{ 0 };
		codestream_content_box_.serialize(&jp2c_box_buf, &jp2c_box_size);
		//memcpy_s(dst_buf_position, box_size_, jp2c_box_buf, jp2c_box_size);
		memcpy(dst_buf_position, jp2c_box_buf, jp2c_box_size);
		dst_buf_position += jp2c_box_size;
		delete[] jp2c_box_buf;
		bytes_done += jp2c_box_size;

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
			throw std::runtime_error("Error: Serializing Codestream Content JUMBF, sizes not matches.");
		}
	}
}