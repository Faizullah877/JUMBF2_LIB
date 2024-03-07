
#include "db_json_box.h"

namespace dbench {

	DbJsonBox::DbJsonBox()
	{
		set_box_type(BoxType::JSON);
	}

	DbJsonBox::~DbJsonBox()
	{
	}

	DbJsonBox::DbJsonBox(char* json, uint64_t json_size)
	{
		set_box(json, json_size);
	}

	void DbJsonBox::set_box(char* json, uint64_t json_size)
	{
		set_box_type(BoxType::JSON);
		set_box_payload((unsigned char*)json, json_size);
	}
}