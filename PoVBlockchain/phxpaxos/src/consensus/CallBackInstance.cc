/*
 * CallBackInstance.cpp
 *
 *  Created on: 2018年3月22日
 *      Author: blackguess
 */
#include "CallBackInstance.h"


CallBackInstance::CallBackInstance() {
	// TODO Auto-generated constructor stub
	index=0;
	start_time=getCurrentTime();
	wait_time=10;
	type=TEST;
	childtype=0;
	caller=NULL;
}

CallBackInstance::CallBackInstance(uint32_t index,callback_type type,NodeID childtype,double wait_time,Fun caller)
{
	this->index=index;
	this->type=type;
	this->childtype=childtype;
	this->wait_time=wait_time;

	this->caller=caller;
	this->start_time=getCurrentTime();
}

CallBackInstance::~CallBackInstance() {
	// TODO Auto-generated destructor stub

}

