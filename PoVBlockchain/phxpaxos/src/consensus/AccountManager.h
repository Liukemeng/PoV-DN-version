/*
 * AccountManager.h
 *
 *  Created on: 2018年4月3日
 *      Author: blackguess
 */

#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H
#include <vector>
#include "account.h"
#include <stdint.h>
#include "constants.h"
#include <string>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "utils.h"
#include <map>
//该类用于管理系统中的账号
class AccountManager {
public:
	AccountManager();
	virtual ~AccountManager();
	//判断是否给定公钥是否委员、管家候选或者管家
	bool is_commissioner(std::string pubkey);
	bool is_commissioner(NodeID ID);
	bool is_butler_candidate(std::string pubkey);
	bool is_butler(std::string pubkey);
	//把委员、管家候选、管家加入相应的列表中
	int pushback_commissioner(std::string pubkey);
	int pushback_commissioner(std::string pubkey,NodeID id);
	int pushback_butler_candidate(std::string pubkey);
	int pushback_butler_candidate(std::string pubkey,NodeID id);
	int pushback_butler(std::string pubkey);
	int pushback_butler(std::string pubkey,NodeID id);
	//从各个列表中移除相应的账号
	int pop_commissioner(std::string pubkey);
	int pop_butler_candidate(std::string pubkey);
	int pop_butler(std::string pubkey);
	// 清空各个账号列表
	void clear_butler();
	void clear_commissioner();
	void clear_butler_candidate();
	//根据某个账号的公钥获取其节点ID
	NodeID get_Commissioner_ID(std::string pubkey);
	NodeID get_Butler_Candidate_ID(std::string pubkey);
	NodeID get_Butler_ID(std::string pubkey);
	account* get_Commissioner_account(std::string pubkey);
	account* get_Butler_Candidate_account(std::string pubkey);
	account* get_Butler_account(std::string pubkey);
	//获取各类账号账号列表
	std::vector<account> get_Commissioner_List();
	std::vector<account>& get_Butler_Candidate_List();
	std::vector<account> get_Butler_List();
	//获取各类身份账号的数量
	uint32_t getCommissionerAmount();
	uint32_t getButlerCandidateAmount();
	uint32_t getButlerAmount();
	account& getCommissioner(uint32_t i);
	account& getCommissionerByNodeID(NodeID id);
	account& getButlerCandidate(uint32_t i);
	account& getButler(uint32_t i);
	//清空管家编号和管家公钥的映射
	void clearButlerPubkeyNumPairs();
	//设置管家编号和管家公钥的映射关系
	void setButlerPubkeyByNum(uint32_t num,std::string pubkey);
	//通过管家编号取管家的公钥
	std::string getButlerPubkeyByNum(uint32_t num);
	//设置各类账号节点ID
	void set_butler_ID(std::string pubkey,NodeID id);
	void set_commissioner_ID(std::string pubkey,NodeID id);
	void set_butler_candidate_ID(std::string pubkey,NodeID id);
	//设置自己账号的各项参数
	void setMyPubkey(std::string pubkey);
	void setMyNodeID(NodeID id);
	void setMyScore(uint32_t score);
	void setCommissioner();
	void setButlerCandidate();
	void setButler();
	void setNotCommissioner();
	void setNotButlerCandidate();
	void setNotButler();
	//下面是deprecated的函数，并没有使用到
	void setButlerWaitForApplyCommissionerResponse(std::string pubkey);
	void setButlerWaitForApplyButlerCandidateResponse(std::string pubkey);
	void setButlerWaitForQuitCommissionerResponse(std::string pubkey);
	void setButlerWaitForQuitButlerCandidateResponse(std::string pubkey);
	void setButlerNotWaitForApplyCommissionerResponse(std::string pubkey);
	void setButlerNotWaitForApplyButlerCandidateResponse(std::string pubkey);
	void setButlerNotWaitForQuitCommissionerResponse(std::string pubkey);
	void setButlerNotWaitForQuitButlerCandidateResponse(std::string pubkey);
	bool isButlerWaitForApplyCommissionerResponse(std::string pubkey);
	bool isButlerWaitForApplyButlerCandidateResponse(std::string pubkey);
	bool isButlerWaitForQuitCommissionerResponse(std::string pubkey);
	bool isButlerWaitForQuitButlerCandidateResponse(std::string pubkey);
	bool isAllButlerWaitForApplyCommissionerResponse(bool state);
	bool isAllButlerWaitForApplyButlerCandidateResponse(bool state);
	bool isAllButlerWaitForQuitCommissionerResponse(bool state);
	bool isAllButlerWaitForQuitButlerCandidateResponse(bool state);
	//获取本地节点的信息
	std::string getMyPubkey();
	NodeID getMyNodeID();
	uint32_t getMyScore();
	bool is_commissioner();
	bool is_butler_candidate();
	bool is_butler();
	rapidjson::Document& getBallot(uint32_t vote_amout);


//private:
	std::vector<account> commissioner_list; //委员列表
	std::vector<account> butler_candidate_list; //管家候选列表
	std::vector<account> butler_list;  //管家列表
	account m_account;	//节点账号
	account null_account;	//空节点，作为空返回值
	std::map<uint32_t,std::string> butler_number;  //管家编号和管家公钥的映射

};


#endif /* SRC_APPLICATIONS_MODEL_CONSENSUS_ACCOUNTMANAGER_H_ */
