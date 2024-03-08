
#include "db_xml_jumbf_box.h"

namespace dbench {
	DbXmlJumbBox::DbXmlJumbBox()
	{
		desc_box_.set_content_type(JumbfContentType::XML);
		set_box_size();
	}

	DbXmlJumbBox::~DbXmlJumbBox()
	{
		delete[] desc_box_.get_type_16bytes();
	}

	DbXmlJumbBox::DbXmlJumbBox(DbXmlBox xml_box)
	{
		desc_box_.set_content_type(JumbfContentType::XML);
		xml_content_box_ = xml_box;
		set_box_size();
	}
	DbXmlJumbBox::DbXmlJumbBox(DbXmlBox xml_box, DbFreeBox pading_box)
	{
		desc_box_.set_content_type(JumbfContentType::XML);
		xml_content_box_ = xml_box;
		padding_box_ = pading_box;
		set_box_size();
	}
	DbXmlJumbBox::DbXmlJumbBox(DbJumbDescBox desc_box)
	{
		desc_box_.set_content_type(JumbfContentType::XML);
		set_jumbf_description_box(desc_box);
		set_box_size();
	}
	DbXmlJumbBox::DbXmlJumbBox(DbJumbDescBox desc_box, DbXmlBox xml_box)
	{
		desc_box_.set_content_type(JumbfContentType::XML);
		set_jumbf_description_box(desc_box);
		xml_content_box_ = xml_box;
		set_box_size();
	}
	void DbXmlJumbBox::set_content_box(DbXmlBox xml_box)
	{
		xml_content_box_ = xml_box;
		set_box_size();
	}
	void DbXmlJumbBox::set_description_box(bool requestable, std::string label, bool id_present, uint32_t id, unsigned char* hash, DbBox* private_box)
	{
		desc_box_.set_box(JumbfContentType::XML, requestable, label, id_present, id, hash, private_box);
		set_box_size();
	}
	void DbXmlJumbBox::set_box_size()
	{
		box_size_ = 8;
		box_size_ += desc_box_.get_box_size();
		box_size_ += xml_content_box_.get_box_size();
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
	void DbXmlJumbBox::serialize(unsigned char** out_buf, uint64_t* out_buf_size)
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
		memcpy(dst_buf_position,  jumd_buf, desc_box_buf_size);
		dst_buf_position += desc_box_buf_size;
		delete[] jumd_buf;
		bytes_done += desc_box_buf_size;

		// xml content box
		unsigned char* xml_box_buf{ nullptr };
		uint64_t xml_box_size{ 0 };
		xml_content_box_.serialize(&xml_box_buf, &xml_box_size);
		//memcpy_s(dst_buf_position, box_size_, xml_box_buf, xml_box_size);
		memcpy(dst_buf_position, xml_box_buf, xml_box_size);
		dst_buf_position += xml_box_size;
		delete[] xml_box_buf;
		bytes_done += xml_box_size;

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
			throw std::runtime_error("Error: Serializing XML JUMBF, sizes not matches.");
		}
	}
}