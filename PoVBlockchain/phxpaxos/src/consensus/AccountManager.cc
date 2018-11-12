/*
 * AccountManager.cc
 *
 *  Created on: 2018年4月3日
 *      Author: blackguess
 */

#include "AccountManager.h"
#include <algorithm>
#include<iostream>


AccountManager::AccountManager() {
	// TODO Auto-generated constructor stub
	commissioner_list=std::vector<account>();
	butler_candidate_list=std::vector<account>();
	butler_list=std::vector<account>();
}

AccountManager::~AccountManager() {
	// TODO Auto-generated destructor stub
}

bool AccountManager::is_commissioner(std::string pubkey)
{
	for(std::vector<account>::iterator i=commissioner_list.begin();i!=commissioner_list.end();i++)
	{
		if(i->getPubKey()==pubkey)
			return true;
	}
	std::cout<<"is_commissioner verify pubkey:"<<pubkey<<"\n";
	for(std::vector<account>::iterator i=commissioner_list.begin();i!=commissioner_list.end();i++)
	{
		std::cout<<"is_commissioner commissioner pubkey:"<<i->getPubKey()<<"\n";
	}

	return false;
}

bool AccountManager::is_commissioner(NodeID ID)
{
	for(std::vector<account>::iterator i=commissioner_list.begin();i!=commissioner_list.end();i++)
	{
		if(i->getNodeId()==ID and i->getPubKey()!="")
			return true;
	}
	return false;
}

bool AccountManager::is_butler_candidate(std::string pubkey)
{
	for(std::vector<account>::iterator i=butler_candidate_list.begin();i!=butler_candidate_list.end();i++)
	{
		if(i->getPubKey()==pubkey)
			return true;
	}
	return false;
}
bool AccountManager::is_butler(std::string pubkey)
{
	for(std::vector<account>::iterator i=butler_list.begin();i!=butler_list.end();i++)
	{
		if(i->getPubKey()==pubkey)
			return true;
	}
	return false;
}

int AccountManager::pushback_commissioner(std::string pubkey)
{
	for(std::vector<account>::iterator i=commissioner_list.begin();i!=commissioner_list.end();i++)
	{
		if(i->getPubKey()==pubkey)
		{
			i->setNodeId(0);
			return 1;
		}
	}
	account com;
	com.setCommissioner();
	com.setPubKey(pubkey);
	com.setNodeId(0);
	commissioner_list.push_back(com);
	return 0;
}

int AccountManager::pushback_commissioner(std::string pubkey,NodeID id)
{
	for(std::vector<account>::iterator i=commissioner_list.begin();i!=commissioner_list.end();i++)
	{
		if(i->getPubKey()==pubkey)
		{
			i->setNodeId(id);
			return 1;
		}
	}
	account com;
	com.setCommissioner();
	com.setPubKey(pubkey);
	com.setNodeId(id);
	commissioner_list.push_back(com);
	return 0;
}

int AccountManager::pushback_butler_candidate(std::string pubkey)
{
	for(std::vector<account>::iterator i=butler_candidate_list.begin();i!=butler_candidate_list.end();i++)
	{
		if(i->getPubKey()==pubkey)
		{
			i->setNodeId(0);
			return 1;
		}
	}
	account bc;
	bc.setButlerCandidate();
	bc.setPubKey(pubkey);
	bc.setNodeId(0);
	butler_candidate_list.push_back(bc);
	return 0;
}

int AccountManager::pushback_butler_candidate(std::string pubkey,NodeID id)
{
	for(std::vector<account>::iterator i=butler_candidate_list.begin();i!=butler_candidate_list.end();i++)
	{
		if(i->getPubKey()==pubkey)
		{
			i->setNodeId(id);
			return 1;
		}
	}
	account bc;
	bc.setButlerCandidate();
	bc.setPubKey(pubkey);
	bc.setNodeId(id);
	butler_candidate_list.push_back(bc);
	return 0;
}

int AccountManager::pushback_butler(std::string pubkey)
{
	for(std::vector<account>::iterator i=butler_list.begin();i!=butler_list.end();i++)
	{
		if(i->getPubKey()==pubkey)
		{
			//i->setNodeId(0);
			return 1;
		}
	}
	account butler;
	butler.setButler();
	butler.setPubKey(pubkey);
	butler.setNodeId(0);
	butler_list.push_back(butler);
	return 0;
}

int AccountManager::pushback_butler(std::string pubkey,NodeID id)
{
	for(std::vector<account>::iterator i=butler_list.begin();i!=butler_list.end();i++)
	{
		if(i->getPubKey()==pubkey)
		{
			i->setNodeId(id);
			return 1;
		}
	}
	account butler;
	butler.setButler();
	butler.setPubKey(pubkey);
	butler.setNodeId(id);
	butler_list.push_back(butler);
	return 0;
}
int AccountManager::pop_commissioner(std::string pubkey)
{
	for(std::vector<account>::iterator i=commissioner_list.begin();i!=commissioner_list.end();i++)
	{
		if(i->getPubKey()==pubkey)
		{
			commissioner_list.erase(i);
			return 0;
		}
	}
	return 1;
}
int AccountManager::pop_butler_candidate(std::string pubkey)
{
	for(std::vector<account>::iterator i=butler_candidate_list.begin();i!=butler_candidate_list.end();i++)
	{
		if(i->getPubKey()==pubkey)
		{
			butler_candidate_list.erase(i);
			return 0;
		}
	}
	return 1;
}
int AccountManager::pop_butler(std::string pubkey)
{
	for(std::vector<account>::iterator i=butler_list.begin();i!=butler_list.end();i++)
	{
		if(i->getPubKey()==pubkey)
		{
			butler_list.erase(i);
			return 0;
		}
	}
	return 1;
}

void AccountManager::clear_butler()
{
	butler_list.clear();
}

void AccountManager::clear_commissioner()
{
	commissioner_list.clear();
}


void AccountManager::clear_butler_candidate()
{
	butler_candidate_list.clear();
}

NodeID AccountManager::get_Commissioner_ID(std::string pubkey)
{
	for(std::vector<account>::iterator i=commissioner_list.begin();i!=commissioner_list.end();i++)
	{
		if(i->getPubKey()==pubkey)
		{
			return i->getNodeId();
		}
	}
	return pov_null;
}


NodeID AccountManager::get_Butler_Candidate_ID(std::string pubkey)
{
	for(std::vector<account>::iterator i=butler_candidate_list.begin();i!=butler_candidate_list.end();i++)
	{
		if(i->getPubKey()==pubkey)
		{
			return i->getNodeId();
		}
	}
	return pov_null;
}

NodeID AccountManager::get_Butler_ID(std::string pubkey)
{
	for(std::vector<account>::iterator i=butler_list.begin();i!=butler_list.end();i++)
	{
		if(i->getPubKey()==pubkey)
		{
			return i->getNodeId();
		}
	}
	return pov_null;
}

account* AccountManager::get_Commissioner_account(std::string pubkey)
{
	for(std::vector<account>::iterator i=commissioner_list.begin();i!=commissioner_list.end();i++)
	{
		if(i->getPubKey()==pubkey)
		{
			return &(*i);
		}
	}
	return NULL;
}


account* AccountManager::get_Butler_Candidate_account(std::string pubkey)
{
	for(std::vector<account>::iterator i=butler_candidate_list.begin();i!=butler_candidate_list.end();i++)
	{
		if(i->getPubKey()==pubkey)
		{
			return &(*i);
		}
	}
	return NULL;
}
account* AccountManager::get_Butler_account(std::string pubkey)
{
	for(std::vector<account>::iterator i=butler_list.begin();i!=butler_list.end();i++)
	{
		if(i->getPubKey()==pubkey)
		{
			return &(*i);
		}
	}
	return NULL;
}

std::vector<account> AccountManager::get_Commissioner_List()
{
	return commissioner_list;
}

std::vector<account>& AccountManager::get_Butler_Candidate_List()
{
	return butler_candidate_list;
}

std::vector<account> AccountManager::get_Butler_List()
{
	return butler_list;
}

uint32_t AccountManager::getCommissionerAmount()
{
	return commissioner_list.size();
}

uint32_t AccountManager::getButlerCandidateAmount()
{
	return butler_candidate_list.size();
}

uint32_t AccountManager::getButlerAmount()
{
	return butler_list.size();
}

account& AccountManager::getCommissioner(uint32_t i)
{
	return commissioner_list.at(i);
}

account& AccountManager::getCommissionerByNodeID(NodeID id)
{
	for(std::vector<account>::iterator i=commissioner_list.begin();i!=commissioner_list.end();i++)
	{
		if(i->getNodeId()==id)
			return *i;
	}
	return null_account;
}

account& AccountManager::getButlerCandidate(uint32_t i)
{
	return butler_candidate_list.at(i);
}
account& AccountManager::getButler(uint32_t i)
{
	return butler_list.at(i);
}

rapidjson::Document& AccountManager::getBallot(uint32_t vote_amount)
{
	rapidjson::Document& ballot=*(new rapidjson::Document);
	ballot.SetObject();
	rapidjson::Document::AllocatorType& allocator=ballot.GetAllocator();
	//排序
	std::sort(butler_candidate_list.begin(),butler_candidate_list.end(),[](account x,account y){
		return x.getScore()>y.getScore();
	});
	//把最高分的成员pubkey加入选票列表中
	rapidjson::Value &selected_member=*(new rapidjson::Value(rapidjson::kArrayType));
	for(uint32_t i=0;i<vote_amount;i++)
	{
		std::string pubkey=butler_candidate_list.at(i).getPubKey();
		rapidjson::Value &member=*(new rapidjson::Value(rapidjson::kObjectType));
		member.SetObject();
		member.SetString(pubkey.c_str(),pubkey.size(),allocator);
		selected_member.PushBack(member,allocator);
	}
	ballot.AddMember("vote_list",selected_member,allocator);
	//这里的pubkey是从key_manager中获取的，可以获取pubkey的方法太多，以后需要统一改为使用某一固定方法。
	std::string str_voter=m_account.getPubKey();
	rapidjson::Value &voter=*(new rapidjson::Value(str_voter.c_str(),str_voter.size(),allocator));
	ballot.AddMember("voter",voter,allocator);
	rapidjson::Value &time=*(new rapidjson::Value(getCurrentTime()));
	ballot.AddMember("timestamp",time,allocator);
	return ballot;
}

void AccountManager::set_butler_ID(std::string pubkey,NodeID id)
{
	for(std::vector<account>::iterator i=butler_list.begin();i!=butler_list.end();i++)
	{
		if(i->getPubKey()==pubkey)
		{
			i->setNodeId(id);
		}
	}
}

void AccountManager::set_commissioner_ID(std::string pubkey,NodeID id)
{
	for(std::vector<account>::iterator i=commissioner_list.begin();i!=commissioner_list.end();i++)
	{
		if(i->getPubKey()==pubkey)
		{
			i->setNodeId(id);
		}
	}
}
void AccountManager::set_butler_candidate_ID(std::string pubkey,NodeID id)
{
	for(std::vector<account>::iterator i=butler_candidate_list.begin();i!=butler_candidate_list.end();i++)
	{
		if(i->getPubKey()==pubkey)
		{
			i->setNodeId(id);
		}
	}
}

void AccountManager::setMyPubkey(std::string pubkey)
{
	m_account.setPubKey(pubkey);
}

void AccountManager::setMyNodeID(NodeID id)
{
	m_account.setNodeId(id);
}

void AccountManager::setMyScore(uint32_t score)
{
	m_account.setScore(score);
}

void AccountManager::setCommissioner()
{
	m_account.setCommissioner();
}

void AccountManager::setButlerCandidate()
{
	m_account.setButlerCandidate();
}

void AccountManager::setButler()
{
	m_account.setButler();
}

void AccountManager::setNotCommissioner()
{
	m_account.setNotCommissioner();
}

void AccountManager::setNotButlerCandidate()
{
	m_account.setNotButlerCandidate();
}

void AccountManager::setNotButler()
{
	m_account.setNotButler();
}

std::string AccountManager::getMyPubkey()
{
	return m_account.getPubKey();
}
NodeID AccountManager::getMyNodeID()
{
	return m_account.getNodeId();
}
uint32_t AccountManager::getMyScore()
{
	return m_account.getScore();
}

bool AccountManager::is_commissioner()
{
	return m_account.isCommissioner();
}

bool AccountManager::is_butler_candidate()
{
	return m_account.isButlerCandidate();
}

bool AccountManager::is_butler()
{
	return m_account.isButler();
}

void AccountManager::clearButlerPubkeyNumPairs()
{
	butler_number.clear();
}


void AccountManager::setButlerPubkeyByNum(uint32_t num,std::string pubkey)
{
	butler_number[num]=pubkey;
}


std::string AccountManager::getButlerPubkeyByNum(uint32_t num)
{
	return butler_number[num];
}

void AccountManager::setButlerWaitForApplyCommissionerResponse(std::string pubkey)
{
	for(std::vector<account>::iterator i=butler_list.begin();i!=butler_list.end();i++)
	{
		if(i->getPubKey()==pubkey)
		{
			i->setWaitForApplyCommissionerResponse();
		}
	}
}
void AccountManager::setButlerWaitForApplyButlerCandidateResponse(std::string pubkey)
{
	for(std::vector<account>::iterator i=butler_list.begin();i!=butler_list.end();i++)
	{
		if(i->getPubKey()==pubkey)
		{
			i->setWaitForApplyButlerCandidateResponse();
		}
	}
}
void AccountManager::setButlerWaitForQuitCommissionerResponse(std::string pubkey)
{
	for(std::vector<account>::iterator i=butler_list.begin();i!=butler_list.end();i++)
	{
		if(i->getPubKey()==pubkey)
		{
			i->setWaitForQuitCommissionerResponse();
		}
	}
}
void AccountManager::setButlerWaitForQuitButlerCandidateResponse(std::string pubkey)
{
	for(std::vector<account>::iterator i=butler_list.begin();i!=butler_list.end();i++)
	{
		if(i->getPubKey()==pubkey)
		{
			i->setWaitForQuitButlerCandidateResponse();
		}
	}
}
void AccountManager::setButlerNotWaitForApplyCommissionerResponse(std::string pubkey)
{
	for(std::vector<account>::iterator i=butler_list.begin();i!=butler_list.end();i++)
	{
		if(i->getPubKey()==pubkey)
		{
			i->setNotWaitForApplyCommissionerResponse();
		}
	}
}
void AccountManager::setButlerNotWaitForApplyButlerCandidateResponse(std::string pubkey)
{
	for(std::vector<account>::iterator i=butler_list.begin();i!=butler_list.end();i++)
	{
		if(i->getPubKey()==pubkey)
		{
			i->setNotWaitForApplyButlerCandidateResponse();
		}
	}
}
void AccountManager::setButlerNotWaitForQuitCommissionerResponse(std::string pubkey)
{
	for(std::vector<account>::iterator i=butler_list.begin();i!=butler_list.end();i++)
	{
		if(i->getPubKey()==pubkey)
		{
			i->setNotWaitForQuitCommissionerResponse();
		}
	}
}
void AccountManager::setButlerNotWaitForQuitButlerCandidateResponse(std::string pubkey)
{
	for(std::vector<account>::iterator i=butler_list.begin();i!=butler_list.end();i++)
	{
		if(i->getPubKey()==pubkey)
		{
			i->setNotWaitForQuitButlerCandidateResponse();
		}
	}
}
bool AccountManager::isButlerWaitForApplyCommissionerResponse(std::string pubkey)
{
	for(std::vector<account>::iterator i=butler_list.begin();i!=butler_list.end();i++)
	{
		if(i->getPubKey()==pubkey)
		{
			return i->isWaitForApplyCommissionerResponse();
		}
	}
	return false;
}
bool AccountManager::isButlerWaitForApplyButlerCandidateResponse(std::string pubkey)
{
	for(std::vector<account>::iterator i=butler_list.begin();i!=butler_list.end();i++)
	{
		if(i->getPubKey()==pubkey)
		{
			return i->isWaitForApplyButlerCandidateResponse();
		}
	}
	return false;
}
bool AccountManager::isButlerWaitForQuitCommissionerResponse(std::string pubkey)
{
	for(std::vector<account>::iterator i=butler_list.begin();i!=butler_list.end();i++)
	{
		if(i->getPubKey()==pubkey)
		{
			return i->isWaitForQuitCommissionerResponse();
		}
	}
	return false;
}
bool AccountManager::isButlerWaitForQuitButlerCandidateResponse(std::string pubkey)
{
	for(std::vector<account>::iterator i=butler_list.begin();i!=butler_list.end();i++)
	{
		if(i->getPubKey()==pubkey)
		{
			return i->isWaitForQuitButlerCandidateResponse();
		}
	}
	return false;
}

bool AccountManager::isAllButlerWaitForApplyCommissionerResponse(bool state)
{
	bool result=true;
	for(std::vector<account>::iterator i=butler_list.begin();i!=butler_list.end();i++)
	{
		if(i->isWaitForApplyCommissionerResponse()!=state)
		{
			result=false;
			break;
		}
	}
	return result;
}
bool AccountManager::isAllButlerWaitForApplyButlerCandidateResponse(bool state)
{
	bool result=true;
	for(std::vector<account>::iterator i=butler_list.begin();i!=butler_list.end();i++)
	{
		if(i->isWaitForApplyButlerCandidateResponse()!=state)
		{
			result=false;
			break;
		}
	}
	return result;
}
bool AccountManager::isAllButlerWaitForQuitCommissionerResponse(bool state)
{
	bool result=true;
	for(std::vector<account>::iterator i=butler_list.begin();i!=butler_list.end();i++)
	{
		if(i->isWaitForQuitCommissionerResponse()!=state)
		{
			result=false;
			break;
		}
	}
	return result;
}
bool AccountManager::isAllButlerWaitForQuitButlerCandidateResponse(bool state)
{
	bool result=true;
	for(std::vector<account>::iterator i=butler_list.begin();i!=butler_list.end();i++)
	{
		if(i->isWaitForQuitButlerCandidateResponse()!=state)
		{
			result=false;
			break;
		}
	}
	return result;
}

