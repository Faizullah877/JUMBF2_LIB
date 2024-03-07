
#include "db_uuid_box.h"

namespace dbench {

	DbUuidBox::DbUuidBox()
	{
		set_box_type(BoxType::UUID);
	}

	DbUuidBox::~DbUuidBox()
	{
	}

	DbUuidBox::DbUuidBox(unsigned char* uuid, unsigned char* uuid_payload, uint64_t uuid_payload_size)
	{
		set_box(uuid, uuid_payload, uuid_payload_size);
	}

	void DbUuidBox::set_box(unsigned char* uuid, unsigned char* uuid_payload, uint64_t uuid_payload_size)
	{
		set_box_type(BoxType::UUID);
		set_uuid(uuid);
		set_box_payload(uuid_payload, uuid_payload_size);
		set_box_size();
	}

	void DbUuidBox::set_uuid(unsigned char* uuid)
	{
		for (auto i = 0; i < 16; i++)
			uuid_[i] = uuid[i];
	}

	void DbUuidBox::set_uuid_paylaod(unsigned char* paylaod_data, uint64_t payload_size)
	{
		set_box_payload(paylaod_data, payload_size);
		set_box_size();
	}

	void DbUuidBox::set_box_size()
	{
		box_size_ = 8 + 16 + payload_size_;
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

	uint64_t DbUuidBox::get_box_size()
	{
		set_box_size();
		return box_size_;
	}

	void DbUuidBox::serialize(unsigned char** out_buf, uint64_t* out_buf_size)
	{
		set_box_size();
		*out_buf_size = box_size_;
		*out_buf = new unsigned char[box_size_];

		uint64_t header_size = 8;
		unsigned char* buf = *out_buf;
		db_put_4byte(&buf, lbox_);
		db_put_4byte(&buf, tbox_);

		// putting xl_box field
		if (xl_box_present_) {
			db_put_8byte(&buf, xl_box_);
			header_size += 8;
		}
		//memcpy_s(buf, box_size_, uuid_, 16);
		memcpy(buf, uuid_, 16);
		buf += 16;
		header_size += 16;
		//memcpy_s(buf, box_size_, payload_, static_cast<size_t>(box_size_) - header_size);
		memcpy(buf, payload_, static_cast<size_t>(box_size_) - header_size);
	}

	void DbUuidBox::deserialize(unsigned char* buf, uint64_t in_buf_size)
	{
		uint64_t header_size{ 8 };
		lbox_ = db_get_4byte(&buf);
		tbox_ = db_get_4byte(&buf);
		box_type_ = db_identify_box_type(tbox_);
		if (lbox_ == 1) {
			xl_box_ = db_get_8byte(&buf);
			xl_box_present_ = true;
			header_size += 8;
			box_size_ = xl_box_;
		}
		else if (lbox_ == 0)
			box_size_ = in_buf_size;
		else
			box_size_ = lbox_;

		for (auto i = 0; i < 16; i++)
			uuid_[i] = db_get_byte(&buf);

		payload_ = buf;
		set_payload(buf);
		set_payload_size(in_buf_size - header_size - 16);
	}

	unsigned char* DbUuidBox::get_uuid()
	{
		return uuid_;
	}

}
