/*
 * account.h
 *
 *  Created on: 2018年3月25日
 *      Author: blackguess
 */

#ifndef ACCOUNT_H
#define ACCOUNT_H
#include <string>
#include "constants.h"

enum authority{
	None,
	User,
	ButlerCandidate,
};
class account {
public:
	account();
	virtual ~account();

	void setNodeName(NodeID node_name);
	NodeID getNodeName();
	void setScore(int score);
	int getScore();
	void setPubKey(std::string pubkey);
	std::string getPubKey();
	void setCommissioner();
	void setNotCommissioner();
	void setButler();
	void setNotButler();
	void setButlerCandidate();
	void setNotButlerCandidate();
	void setNodeId(NodeID id);
	void setWaitForApplyCommissionerResponse();
	void setWaitForApplyButlerCandidateResponse();
	void setWaitForQuitCommissionerResponse();
	void setWaitForQuitButlerCandidateResponse();
	void setNotWaitForApplyCommissionerResponse();
	void setNotWaitForApplyButlerCandidateResponse();
	void setNotWaitForQuitCommissionerResponse();
	void setNotWaitForQuitButlerCandidateResponse();
	bool isWaitForApplyCommissionerResponse();
	bool isWaitForApplyButlerCandidateResponse();
	bool isWaitForQuitCommissionerResponse();
	bool isWaitForQuitButlerCandidateResponse();
	NodeID getNodeId();
	bool isCommissioner();
	bool isButlerCandidate();
	bool isButler();

private:
	NodeID node_name;
	int score;
	bool is_Commissioner;
	bool is_Butler;
	bool is_Butler_Candidate;
	std::string pubkey;
	NodeID node_id;
	bool wait_for_apply_commissioner_response=false;
	bool wait_for_apply_butler_candidate_response=false;
	bool wait_for_quit_commissioner_response=false;
	bool wait_for_quit_butler_candidate_response=false;

};


#endif /* SRC_APPLICATIONS_MODEL_CONSENSUS_ACCOUNT_H_ */
