
#include "db_xml_box.h"

namespace dbench {

	DbXmlBox::DbXmlBox()
	{
		set_box_type(BoxType::XML);
	}

	DbXmlBox::~DbXmlBox()
	{
	}

	DbXmlBox::DbXmlBox(char* xml, uint64_t xml_size)
	{
		set_box(xml, xml_size);
	}

	void DbXmlBox::set_box(char* xml, uint64_t xml_size)
	{
		set_box_type(BoxType::XML);
		set_box_payload((unsigned char*)xml, xml_size);
	}


}
