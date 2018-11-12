/*
 * PoVHeader.h
 *
 *  Created on: 2018年4月9日
 *      Author: blackguess
 */

#ifndef POVHEADER_H
#define POVHEADER_H
#include <iostream>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "constants.h"

class PoVHeader {
public:
	PoVHeader();
	virtual ~PoVHeader();
	bool setData(rapidjson::Value& v);
	bool setRawData(rapidjson::Value& v);
	rapidjson::Document& getData();
	rapidjson::Document& getRawData();
	void setHeight(uint32_t h);
	uint32_t getHeight();
	void setNumOfTrans(uint32_t num);
	uint32_t getNumOfTrans();
	void setGenerator(std::string gen);
	std::string getGenerator();
	void setPreviousHash(std::string pre_hash);
	std::string getPreviousHash();
	void setTe(double T);
	double getTe();
	void setCycles(uint32_t num);
	uint32_t getCycles();
	void setMerkleRoot(std::string root);
	std::string getMerkleRoot();
	void setSignatures(rapidjson::Value& sigs);
	void setSignatures(std::vector<signature>& sigs);
	rapidjson::Document& getSignatures();
	void setHash(std::string h);
	std::string getHash();
	void setNextButler(uint32_t i);
	uint32_t getNextButler();
	void init();
private:
	uint32_t height;
	uint32_t num_of_trans;
	std::string generator;
	std::string previous_hash;
	double Te;
	uint32_t cycles;
	uint32_t next_butler;
	std::string merkle_root;
	std::string sigs_data;
	std::string hash;
};

#endif /* SRC_APPLICATIONS_MODEL_CONSENSUS_POVHEADER_H_ */
