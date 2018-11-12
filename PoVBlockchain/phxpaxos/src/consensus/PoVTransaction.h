/*
 * PoVTransaction.h
 *
 *  Created on: 2018年4月9日
 *      Author: blackguess
 */

#ifndef POVTRANSACTION_H
#define POVTRANSACTION_H
#include <iostream>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

class PoVTransaction {
public:
	PoVTransaction();
	virtual ~PoVTransaction();
	bool setData(rapidjson::Value& v);
	rapidjson::Document& getData();
	void setMetaType(std::string metatype);
	std::string getMetaType();
private:
	std::string metatype;
	std::string data;
};


#endif /* SRC_APPLICATIONS_MODEL_CONSENSUS_POVTRANSACTION_H_ */
