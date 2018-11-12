/*
 * PoVBlock.h
 *
 *  Created on: 2018年4月9日
 *      Author: blackguess
 */

#ifndef POVBLOCK_H
#define POVBLOCK_H
#include <iostream>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "PoVHeader.h"
#include "PoVTransaction.h"
#include <vector>
#include "KeyManager.h"


class PoVBlock {
public:
	PoVBlock();
	virtual ~PoVBlock();
	void setHeader(rapidjson::Value& v);
	void setHeader(PoVHeader v);
	void setRawHeader(rapidjson::Value& v);
	//void setRawHeader(KeyManager *key_manger);
	rapidjson::Document& getHeader();
	rapidjson::Document& getRawHeader();
	PoVHeader getPoVHeader();
	uint32_t getHeight();
	void setSigs(rapidjson::Value& sigs);
	void setSigs(std::vector<signature>& sigs);
	rapidjson::Document& getSignatures();
	void setHash(std::string h);
	std::string getHash();
	void setPreHash(std::string h);
	std::string getPreHash();
	void setTe(double T);
	double getTe();
	void setNextButler(uint32_t i);
	uint32_t getNextButler();
	uint32_t getCycles();

	void PushBackTransaction(rapidjson::Value& v);
	void setTransactions(rapidjson::Value& v);
	rapidjson::Document& getTransaction(uint32_t i);
	rapidjson::Document& getTransactionsList();
	uint32_t getTransactionsAmout();
	void getConstantsTransactions(std::vector<PoVTransaction>& txs);
	void getApplyCommissionerTransactions(std::vector<PoVTransaction>& txs);
	void getApplyButlerCandidateTransactions(std::vector<PoVTransaction>& txs);
	void getVoteTransactions(std::vector<PoVTransaction>& txs);
	void getNormalTransactions(std::vector<PoVTransaction>& txs);
	void getQuitButlerCandateTransactions(std::vector<PoVTransaction>& txs);
	void getQuitCommissionerTransactions(std::vector<PoVTransaction>& txs);

	void setRawBlock(rapidjson::Value& d);
	void setBlock(rapidjson::Value& d);
	rapidjson::Document& getRawBlock();
	rapidjson::Document& getBlock();
	void copyfrom(PoVBlock &p);

private:
	PoVHeader header;
	std::vector<PoVTransaction> transactions;

};


#endif /* SRC_APPLICATIONS_MODEL_CONSENSUS_POVBLOCK_H_ */
