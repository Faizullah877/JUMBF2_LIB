
#include "db_codestream_box.h"

namespace dbench {

	DbCodestreamBox::DbCodestreamBox()
	{
		set_box_type(BoxType::JP2C);
	}

	DbCodestreamBox::~DbCodestreamBox()
	{
	}

	DbCodestreamBox::DbCodestreamBox(unsigned char* codestream, uint64_t codestream_size)
	{
		set_box(codestream, codestream_size);
	}

	void DbCodestreamBox::set_box(unsigned char* codestream, uint64_t codestream_size)
	{
		set_box_type(BoxType::JP2C);
		set_box_payload((unsigned char*)codestream, codestream_size);
	}
}