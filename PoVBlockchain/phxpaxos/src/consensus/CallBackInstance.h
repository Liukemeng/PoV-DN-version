/*
 * CallBackInstance.h
 *
 *  Created on: 2018年3月22日
 *      Author: blackguess
 */

#ifndef CALLBACKINSTANCE_H
#define CALLBACKINSTANCE_H
#include "constants.h"
//#include "nstime.h"
#include "rapidjson/document.h"
#include <cstdint>
#include "utils.h"
#include <functional>

class POV;
typedef void(POV::*handler_ptr)(rapidjson::Document &d);
typedef std::function<void(rapidjson::Document&)> Fun;
//该类用于存储发送的消息的信息，用于回调以及超时限制
class CallBackInstance {
public:
	CallBackInstance();
	CallBackInstance(uint32_t index,callback_type type,NodeID childtype,double wait_time,Fun caller);
	virtual ~CallBackInstance();
	/*
	void setIndex(uint32_t index);
	void setCallBackType(callback_type type);
	void setChildType(uint32_t childtype);
	void setStartTime(double start_time);
	void setWaitTime(double wait_time);
	void setCalledFunction(handler_ptr caller);
private:*/
	uint32_t index; //消息编号
	callback_type type;	//回调类型
	NodeID childtype;	//回调子类型
	double start_time;	//消息开始时间
	double wait_time;	//超时时间
	Fun caller;		//回调函数
};



#endif /* SRC_APPLICATIONS_MODEL_CONSENSUS_CALLBACKINSTANCE_H_ */
