/*
 * PoVBlockChain.h
 *
 *  Created on: 2018年4月11日
 *      Author: blackguess
 */

#ifndef POVBLOCKCHAIN_H
#define POVBLOCKCHAIN_H
#include "PoVBlock.h"
#include <vector>

#include <iostream>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include<mutex>

//只是一个存储区块的的容器而已
class PoVBlockChain {
public:
	//PoVBlockChain();
	PoVBlockChain();
	virtual ~PoVBlockChain();
	bool pushbackBlock(PoVBlock block);
	PoVBlock getBlock(uint32_t i);
	int getHeight();
	uint32_t getAmout();
	bool validateBlockChain();
	void clear();
	//void setCollection(mongocxx::client &client,std::string pubkey);
	bool pushbackBlockToDatabase(PoVBlock block);
	//bool updateBlockToDatabase(PoVBlock block);
	PoVBlock getBlockFromDatabase(uint32_t i);
	void deleteBlockChainFromDatabase();
	void saveBlockChain();
	void loadBlockChain();
	void setPubkey(std::string pubkey);
	int queryHeight();
	rapidjson::Document& queryLogByTime(long minT,long maxT);
	rapidjson::Document& queryLogByErrorCode(std::string err_code);
	//rapidjson::Document& queryLogByT(std::string);
private:
	std::vector<PoVBlock> blockchain;
	//mongocxx::collection coll;
	std::string pubkey;
	//mongocxx::client &client;
	//std::mutex& mtx;
};



#endif /* SRC_APPLICATIONS_MODEL_CONSENSUS_POVBLOCKCHAIN_H_ */
