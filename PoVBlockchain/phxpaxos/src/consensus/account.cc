/*
 * account.cc
 *
 *  Created on: 2018年3月25日
 *      Author: blackguess
 */

#include "account.h"

account::account() {
	// TODO Auto-generated constructor stub
	node_name=0;
	node_id=0;
	score=0;
	is_Commissioner=false;
	is_Butler=false;
	is_Butler_Candidate=false;
	pubkey="";
}

account::~account() {
	// TODO Auto-generated destructor stub
}

 /* namespace ns3 */

void account::setNodeName(NodeID node_name)
{
	this->node_name=node_name;
}

NodeID account::getNodeName()
{
	return node_name;
}

void account::setScore(int score)
{
	this->score=score;
}

int account::getScore()
{
	return score;
}


void account::setPubKey(std::string pubkey)
{
	this->pubkey=pubkey;
}

std::string account::getPubKey()
{
	return pubkey;
}
void account::setCommissioner()
{
	is_Commissioner=true;
}
void account::setNotCommissioner()
{
	is_Commissioner=false;
}
void account::setButler()
{
	is_Butler=true;
}
void account::setNotButler()
{
	is_Butler=false;
}
void account::setButlerCandidate()
{
	is_Butler_Candidate=true;
}
void account::setNotButlerCandidate()
{
	is_Butler_Candidate=false;
}
void account::setNodeId(NodeID id)
{
	node_id=id;
}
NodeID account::getNodeId()
{
	return node_id;
}
bool account::isCommissioner()
{
	return is_Commissioner;
}
bool account::isButlerCandidate()
{
	return is_Butler_Candidate;
}
bool account::isButler()
{
	return is_Butler;
}

void account::setWaitForApplyCommissionerResponse()
{
	wait_for_apply_commissioner_response=true;
}
void account::setWaitForApplyButlerCandidateResponse()
{
	wait_for_apply_butler_candidate_response=true;
}
void account::setWaitForQuitCommissionerResponse()
{
	wait_for_quit_commissioner_response=true;
}
void account::setWaitForQuitButlerCandidateResponse()
{
	wait_for_quit_butler_candidate_response=true;
}
void account::setNotWaitForApplyCommissionerResponse()
{
	wait_for_apply_commissioner_response=false;
}
void account::setNotWaitForApplyButlerCandidateResponse()
{
	wait_for_apply_butler_candidate_response=false;
}
void account::setNotWaitForQuitCommissionerResponse()
{
	wait_for_quit_commissioner_response=false;
}
void account::setNotWaitForQuitButlerCandidateResponse()
{
	wait_for_quit_butler_candidate_response=false;
}
bool account::isWaitForApplyCommissionerResponse()
{
	return wait_for_apply_commissioner_response;
}
bool account::isWaitForApplyButlerCandidateResponse()
{
	return wait_for_apply_butler_candidate_response;
}
bool account::isWaitForQuitCommissionerResponse()
{
	return wait_for_quit_commissioner_response;
}
bool account::isWaitForQuitButlerCandidateResponse()
{
	return wait_for_quit_butler_candidate_response;
}

