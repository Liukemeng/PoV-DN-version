/*
 * utils.h
 *
 *  Created on: 2018年3月23日
 *      Author: blackguess
 */
#ifndef UTILS_H
#define UTILS_H

//#include "ns3/nstime.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <string>
#include <vector>
double getCurrentTime();	//获取当前系统时间
double getRandomNum();		//产生随机数
std::string getRandomString(uint32_t len);	//产生指定长度的随机字符串
void print_document(rapidjson::Document& d);	//打印json数据
void print_document(rapidjson::Value& d);	//打印json数据
std::string getDocumentString(rapidjson::Value& d);		//把json对象转换为字符串
std::vector<std::string> utils_split(std::string str,std::string pattern);	//字符串分割
long convertTime(std::string time);		//把日志中时间戳转换为标准的长整型时间

#endif /* SRC_APPLICATIONS_MODEL_CONSENSUS_UTILS_H_ */
