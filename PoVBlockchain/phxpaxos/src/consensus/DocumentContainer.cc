/*
 * DocumentContainer.cc
 *
 *  Created on: 2018年4月6日
 *      Author: blackguess
 */

#include "DocumentContainer.h"

DocumentContainer::DocumentContainer() {
	// TODO Auto-generated constructor stub
	doc="";
	//value="";
}

DocumentContainer::~DocumentContainer() {
	// TODO Auto-generated destructor stub
}

void DocumentContainer::saveDocument(rapidjson::Value& d)
{
	if(!d.IsObject())
		return;
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	d.Accept(writer);
	doc=buffer.GetString();
}
rapidjson::Document& DocumentContainer::getDocument()
{
	rapidjson::Document &d=*(new rapidjson::Document);
	d.Parse(doc.c_str(),doc.size());
	return d;
}

std::string DocumentContainer::getData()
{
	return doc;
}
/*
void DocumentContainer::saveDocument(rapidjson::Value& d)
{
	if(!d.IsObject())
		return;
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	d.Accept(writer);
	doc=buffer.GetString();
}
rapidjson::Value& DocumentContainer::getDocument();
*/

