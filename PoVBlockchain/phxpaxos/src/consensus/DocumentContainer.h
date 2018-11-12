/*
 * DocumentContainer.h
 *
 *  Created on: 2018年4月6日
 *      Author: blackguess
 */

#ifndef DOCUMENTCONTAINER_H
#define DOCUMENTCONTAINER_H
#include <iostream>
#include <string.h>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

class DocumentContainer {
public:
	DocumentContainer();
	virtual ~DocumentContainer();
	void saveDocument(rapidjson::Value& d);
	rapidjson::Document& getDocument();
	std::string getData();
	//void saveDocument(rapidjson::Value& d);
	//rapidjson::Value& getDocument();

private:
	std::string doc;
	//rapidjson::Value value;
};



#endif /* SRC_APPLICATIONS_MODEL_CONSENSUS_DOCUMENTCONTAINER_H_ */
