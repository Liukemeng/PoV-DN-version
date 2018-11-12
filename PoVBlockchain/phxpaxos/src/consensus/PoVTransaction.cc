/*
 * PoVTransaction.cc
 *
 *  Created on: 2018年4月9日
 *      Author: blackguess
 */

#include "PoVTransaction.h"


PoVTransaction::PoVTransaction() {
	// TODO Auto-generated constructor stub

}

PoVTransaction::~PoVTransaction() {
	// TODO Auto-generated destructor stub
}

bool PoVTransaction::setData(rapidjson::Value& v)
{
	if(!v.IsObject())
		return false;
	metatype=v["metatype"].GetString();
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	v.Accept(writer);
	data=buffer.GetString();
	return true;
}

rapidjson::Document& PoVTransaction::getData()
{
	rapidjson::Document &d=*(new rapidjson::Document);
	d.Parse(data.c_str(),data.size());
	return d;
}

void PoVTransaction::setMetaType(std::string metatype)
{
	this->metatype=metatype;
}
std::string PoVTransaction::getMetaType()
{
	return metatype;
}


