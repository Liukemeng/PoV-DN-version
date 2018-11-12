/*
 * POV.cpp
 *
 *  Created on: 2018年3月16日
 *      Author: blackguess
 */

#include "POV.h"
#include <memory>
#include "constants.h"
#include <vector>
#include "blockchain-network.h"
#include "utils.h"
#include <stdlib.h>
//#include "simulator.h"
#include <fstream>
#include <cstdio>
#include<ctime>
#include "ConfigHelper.h"

#include <functional>
#include <boost/bind.hpp>
#include "boost/function.hpp"
#include <string.h>
#include "algorithm/GroupSig.h"
#include <fstream>

using namespace GroupSigApi;

//#include <sstream>

POV::POV() {
	// TODO Auto-generated constructor stub

}

POV::~POV() {
	// TODO Auto-generated destructor stub
}

void POV::run()
{
	//std::cout<<"run:start!\n";
	//处理超时的CallBackInstance
	for(std::vector<CallBackInstance>::iterator i=CallBackList.begin();i!=CallBackList.end();++i)
	{
		double current_time=getCurrentTime();
		if(i->start_time+i->wait_time<=current_time)
		{
			CallBackList.erase(i);
			i--;
		}
	}
	switch(state)
	{
	case sync_block:
	{
		if(sync_block_method==SequenceSync)
			syncBlockSequence();
		else
			syncBlockConcurrency();
		break;

	}
	case generate_first_block:
	{
		//在创世区块阶段，系统先获取所有委员的公钥
		bool have_all_commissoner_pubkey=true;
		for(uint32_t i=0;i<init_commissioner_size;i++)
		{
			//uint32_t myid=network->getNodeId();
			//std::cout<<"generate_first_block——collect commissioner pubkey：1\n";
			if(!account_manager.is_commissioner(Initial_Commissioner_Nodes[i]))
			{
				rapidjson::Document &d=msg_manager.make_Request_Commissioner_PublicKey(Initial_Commissioner_Nodes[i]);
				sendMessage(d,true,10,Recall_RequestCommissionerPubkey,Initial_Commissioner_Nodes[i],&POV::handleResponseCommissionerPubkey);
				have_all_commissoner_pubkey=false;
				delete &d;
			}
			//std::cout<<"generate_first_block——collect commissioner pubkey：2\n";
		}
		//如果有所有委员的公钥，则进入申请成为委员的流程。
		if(have_all_commissoner_pubkey)
		{
			//std::cout<<"have_all_commissoner_pubkey!\n";
			if(I_am_butler_candidate)
			{
				//申请管家候选流程从这里开始
				//std::cout<<"申请管家候选!\n";
				Initial_ApplyButlerCandidate();
			}/*I_am_butler_candidate*/

			if(I_am_commissioner)
			{
				//std::cout<<"申请委员!\n";
				//std::cout<<network->getNodeId()<<"收集到所有委员公钥!\n";
				Initial_ApplyCommissioner();
				//当管家候选人数达到或超过预定的管家数，开始生成投票并发送给代理委员。
				//std::cout<<"投票!\n";
				Initial_Vote();

				//代理委员执行策略，这里的代理委员用节点ID判断
				if(network->getNodeId()==AgentCommissioner)
				{
					//std::cout<<"生成创世区块!\n";
					Initial_GenerateBlock();
				}/*值班委员执行策略*/
			}/*I_am_commissioner*/
		}/*have_all_commissoner_pubkey*/
		break;
	}
	case normal:
	{
		//std::cout<<"在"<<network->getNodeId()<<"的Normal状态中：start!\n";
		current_duty_butler_num=((int)((getCurrentTime()-start_time)/Tcut)+init_duty_butler_num)%butler_amount;
		current_duty_butler_pubkey=account_manager.getButlerPubkeyByNum(current_duty_butler_num);
		Normal_PublishTransaction();
		//std::cout<<"在"<<network->getNodeId()<<"的Normal状态中：2!\n";
		//如果是委员，则在任期结束时投票，否则进入申请委员阶段
		if(account_manager.is_commissioner())
		{
			if(period_generated_block_num==0)
			{
				Normal_Vote();
			}
			//Normal_QuitCommissioner();
		}
		else
		{
			//Normal_ApplyCommissioner();
		}
		if(account_manager.is_butler_candidate())
		{
			//Normal_QuitButlerCandidate();
		}
		else
		{
			//Normal_ApplyButlerCandidate();
		}
		//std::cout<<"在"<<network->getNodeId()<<"的Normal状态中：3!\n";
		if(account_manager.is_butler())
		{
			if(account_manager.getMyPubkey()==current_duty_butler_pubkey)
			{
				double passed_time=(getCurrentTime()-start_time)-((int)((getCurrentTime()-start_time)/Tcut))*Tcut;
				//std::cout<<"passed_time:"<<passed_time<<"\n";
				if(passed_time>defer_time)
				{
					//std::cout<<network->getNodeId()<<"节点在值班管家处理中：\n";
					//std::cout<<"start time="<<start_time<<"\n";
					//std::cout<<"getCurrentTime()="<<getCurrentTime()<<"\n";
					//std::cout<<"init_duty_butler_num="<<init_duty_butler_num<<"\n";
					//std::cout<<"current_duty_butler_num="<<current_duty_butler_num<<"\n";
					Normal_GenerateBlock();
				}/*defer time*/
				else
				{
					//GenerateBlock=GenerateBlock_sign;
				}
			}
			else //如果不是值班管家
			{
				GenerateBlock=GenerateBlock_sign;
				if(cache_block!=NULL)
				{
					delete cache_block;
					cache_block=NULL;
				}
				raw_block_signatures.clear();
			}
		}
		//std::cout<<"在"<<network->getNodeId()<<"的Normal状态中：4!\n";
		break;
	}
	}/*switch(state)*/
	//std::cout<<"run:end!\n";

}

void POV::syncBlockSequence()
{
	//同步区块，该算法的区块高度等待时间必须比区块产生周期短，否则一个节点永远无法加入区块链网络
	//向所有节点发送高度请求，这个请求只发送一次
	double now=getCurrentTime();
	if(fetch_height_start_time<=0)
	{
		fetch_height_start_time=now;
		//rapidjson::Document& msg=msg_manager.make_Request_Height();
		//sendMessage(msg,false,10,Recall_RequestCommissionerPubkey,0,&POV::handleResponseCommissionerPubkey);
		//delete &msg;
		//break;
	}

	//根据时间判断当前的syncer是否有效，即正在进行区块同步
	bool valid=false;
	if(now-current_syncer.start_time<current_syncer.wait_time and current_syncer.height>blockchain.getHeight())
	{
		valid=true;
	}
	//在syncers为空且current_syncer无效的情况下，若等待区块头时间超过设定的等待时间，则跳转到生成创世区块或者普通阶段
	if(syncers.empty() && !valid)
	{
		//std::cout<<"sync block: 1\n";
		if(now-fetch_height_start_time>fetch_height_wait_time)
		{
			//std::cout<<"sync block: 2\n";
			int height= blockchain.getHeight();
			if(height==-1)
			{
				state=generate_first_block;
			}
			else
			{
				for(int i=0;i<height;i++)
				{
					PoVBlock block=blockchain.getBlock(i);
					rapidjson::Document& d=block.getBlock();
					updateVariables(d);
					delete &d;
				}
				state=normal;
			}
			return;
		}
	}

	if(syncers.empty())
	{
		rapidjson::Document& msg=msg_manager.make_Request_Height();
		//std::cout<<"cbi数量："<<CallBackList.size()<<"\n";
		sendMessage(msg,true,fetch_height_wait_time_last_encore,Recall_RequestCommissionerPubkey,0,&POV::handleResponseCommissionerPubkey);
		//std::cout<<"cbi数量："<<CallBackList.size()<<"\n";
		delete &msg;
	}
	while(!syncers.empty() && !valid)
	{//如果syncer不为空且当前syncer无效，就从syncers队列中取出一个有效的syncer来进行同步。
		std::cout<<"sync block: 3\n";
		current_syncer=syncers.front();
		syncers.pop();
		if(current_syncer.height>blockchain.getHeight())
		{
			valid=true;
			current_syncer.start_time=now;
			current_syncer.wait_time=syncer_wait_time;
			NodeID receiver=current_syncer.id;
			rapidjson::Document& msg=msg_manager.make_Request_Block(receiver,blockchain.getHeight()+1);
			sendMessage(msg,true,5,Recall_RequestBlock,blockchain.getHeight()+1,&POV::handleResponseBlock);
			delete &msg;
		}
		/*
		if(syncers.empty())
		{
			rapidjson::Document& msg=msg_manager.make_Request_Height();
			std::cout<<"cbi数量："<<CallBackList.size()<<"\n";
			sendMessage(msg,true,fetch_height_wait_time_last_encore,Recall_RequestCommissionerPubkey,0,&POV::handleResponseCommissionerPubkey);
			std::cout<<"cbi数量："<<CallBackList.size()<<"\n";
			delete &msg;
		}
		*/
	}
}

void POV::syncBlockConcurrency()
{
	double now=getCurrentTime();
	if(fetch_height_start_time<=0)
	{
		fetch_height_start_time=now;
		//rapidjson::Document& msg=msg_manager.make_Request_Height();
		//sendMessage(msg,false,10,Recall_RequestCommissionerPubkey,0,&POV::handleResponseCommissionerPubkey);
		//delete &msg;
		//break;
	}
	int height= blockchain.getHeight();
	if(con_syncers.empty() && now-fetch_height_start_time>fetch_height_wait_time)
	{
		//std::cout<<"sync block: 2\n";
		if(height==-1)
		{
			state=generate_first_block;
		}
		else
		{
			for(int i=0;i<height;i++)
			{
				PoVBlock block=blockchain.getBlock(i);
				rapidjson::Document& d=block.getBlock();
				updateVariables(d);
				delete &d;
			}
			state=normal;
		}
		return;
	}
	else
	{
		rapidjson::Document& msg=msg_manager.make_Request_Height();
		//std::cout<<"cbi数量："<<CallBackList.size()<<"\n";
		sendMessage(msg,true,fetch_height_wait_time_last_encore,Recall_RequestCommissionerPubkey,0,&POV::handleResponseCommissionerPubkey);
		//std::cout<<"cbi数量："<<CallBackList.size()<<"\n";
		delete &msg;
	}
	std::map<NodeID,NodeInfo>& nodelist=network->getNodeList();
	int nums=nodelist.size();
	std::vector<NodeID> list;
	for(auto i=nodelist.begin();i!=nodelist.end();i++)
	{
		list.push_back(i->first);
	}
	for(int i=0;i<con_syncers.size();i++)
	{
		if(!con_syncers[i].prepared)
		{
			int request_height=con_syncers[i].height;
			NodeID receiver=list[request_height%nums];
			rapidjson::Document& msg=msg_manager.make_Request_Block(receiver,request_height);
			sendMessage(msg,true,5,Recall_RequestBlock,request_height,&POV::handleResponseBlock);
			delete &msg;
		}
	}
	while((!con_syncers.empty()) && con_syncers[0].prepared)
	{
		int next_height=con_syncers[0].height;
		if(next_height>0)
		{
			std::string prehash=con_syncers[0].prehash;
			PoVBlock b=blockchain.getBlock(height);
			std::string true_hash=key_manager.getHash256(b.getHeader());
			if(true_hash!=prehash)
			{
				std::cout<<"验证区块头错误——前一个区块的hash错误！\n";
				std::cout<<"我的区块链高度="<<height<<"\n";
				std::cout<<"我的hash="<<true_hash<<"\n";
				std::cout<<"同步器的区块链高度="<<next_height<<"\n";
				std::cout<<"同步器的prehash="<<prehash<<"\n";
				//验证区块不通过的情况下随机选取节点进行区块同步
				NodeID receiver=list[rand()%nums];
				rapidjson::Document& msg=msg_manager.make_Request_Block(receiver,next_height);
				sendMessage(msg,true,5,Recall_RequestBlock,next_height,&POV::handleResponseBlock);
				con_syncers[0].prepared=false;
				delete &msg;
				break;
			}
			blockchain.pushbackBlockToDatabase(con_syncers[0].block);
			con_syncers.erase(con_syncers.begin());
		}
		else
		{
			blockchain.pushbackBlockToDatabase(con_syncers[0].block);
			con_syncers.erase(con_syncers.begin());
		}
	}
}

void POV::updateVariables(rapidjson::Value& b)
{
	//std::cout<<"更新变量：1\n";
	PoVBlock block;
	block.setBlock(b);
	//清空环签名池中的签名
	if(!ring_sigs_pool.empty())
	{
		ring_sigs_pool.clear();
		std::cout<<"清空环签名池！\n";
	}

	//处理普通交易,仅仅是清除缓存池中的相同交易而已
	std::vector<PoVTransaction> Normals;
	block.getNormalTransactions(Normals);
	//std::cout<<"更新变量：2\n";
	for(std::vector<PoVTransaction>::iterator i=Normals.begin();i!=Normals.end();i++)
	{
		//std::cout<<"更新变量：21\n";
		rapidjson::Document& tx=i->getData();
		double timestamp=tx["timestamp"].GetDouble();
		rapidjson::Value &content=tx["content"];
		//DocumentContainer container;
		//container.saveDocument(tx);
		for(std::vector<DocumentContainer>::iterator i=NormalPool.begin();i!=NormalPool.end();i++)
		{
			rapidjson::Document& cache_tx=i->getDocument();
			double cache_timestamp=cache_tx["timestamp"].GetDouble();
			rapidjson::Value& cache_content=cache_tx["content"];
			//普通交易的content有两种类型，分别是字符串或者json对象
			if(content.IsObject() && cache_content.IsObject()
					&&content.HasMember("log") &&cache_content.HasMember("log")
					&&content["log"].GetInt()==cache_content["log"].GetInt())
			{
				NormalPool.erase(i);
				i--;
			}
			else if(content.IsString())
			{
				if(cache_content.IsString())
				{
					if(std::string(content.GetString())==std::string(cache_content.GetString()))
					{
						NormalPool.erase(i);
						i--;
					}
				}
			}
			delete &cache_tx;

		}

		delete &tx;

	}
	//std::cout<<"更新变量：3\n";
	//处理常量交易
	std::vector<PoVTransaction> constants;
	block.getConstantsTransactions(constants);
	for(std::vector<PoVTransaction>::iterator i=constants.begin();i!=constants.end();i++)
	{
		rapidjson::Document& tx=i->getData();
		//print_document(tx);
		butler_amount=tx["metadata"]["butler_amout"].GetInt();
		num_blocks=tx["metadata"]["blocks_per_cycle"].GetInt();
		Tcut=tx["metadata"]["Tcut"].GetDouble();
		vote_amount=tx["metadata"]["vote_amount"].GetInt();
		delete &tx;
	}
	//std::cout<<"更新变量：4\n";
	//处理申请委员交易交易
	std::vector<PoVTransaction> ApplyCommissioners;
	block.getApplyCommissionerTransactions(ApplyCommissioners);
	for(std::vector<PoVTransaction>::iterator i=ApplyCommissioners.begin();i!=ApplyCommissioners.end();i++)
	{
		rapidjson::Document& tx=i->getData();
		std::string applicant=tx["metadata"]["pubkey"].GetString();
		account_manager.pushback_commissioner(applicant);
		bool existed=false;
		for(std::vector<DocumentContainer>::iterator j=ApplyCommissionerPool.begin();j!=ApplyCommissionerPool.end();j++)
		{
			rapidjson::Document& _tx=j->getDocument();
			if(_tx["metadata"]["pubkey"].GetString()==applicant)
			{
				ApplyCommissionerPool.erase(j);
				delete &_tx;
				existed=true;
				break;
			}
			delete &_tx;
		}
		delete &tx;
	}
	//std::cout<<"更新变量：5\n";
	//处理申请管家候选交易
	std::vector<PoVTransaction> ApplyButlerCandidates;
	block.getApplyButlerCandidateTransactions(ApplyButlerCandidates);
	for(std::vector<PoVTransaction>::iterator i=ApplyButlerCandidates.begin();i!=ApplyButlerCandidates.end();i++)
	{
		rapidjson::Document& tx=i->getData();
		std::string applicant=tx["recommendation_letter"]["metadata"]["refferal"].GetString();
		account_manager.pushback_butler_candidate(applicant);
		for(std::vector<DocumentContainer>::iterator j=ApplyButlerCandidatePool.begin();j!=ApplyButlerCandidatePool.end();j++)
		{
			rapidjson::Document& _tx=j->getDocument();
			if(_tx["recommendation_letter"]["metadata"]["refferal"].GetString()==applicant)
			{
				ApplyButlerCandidatePool.erase(j);
				delete &_tx;
				break;
			}
			delete &_tx;
		}
		delete &tx;
	}
	//处理退出委员交易
	std::vector<PoVTransaction> QuitCommissioners;
	block.getQuitCommissionerTransactions(QuitCommissioners);
	for(std::vector<PoVTransaction>::iterator i=QuitCommissioners.begin();i!=QuitCommissioners.end();i++)
	{
		rapidjson::Document& tx=i->getData();
		std::string applicant=tx["metadata"]["pubkey"].GetString();
		std::cout<<applicant<<"退出委员\n";
		account_manager.pop_commissioner(applicant);
		for(std::vector<DocumentContainer>::iterator j=QuitCommissionerPool.begin();j!=QuitCommissionerPool.end();j++)
		{
			rapidjson::Document& _tx=j->getDocument();
			if(_tx["metadata"]["pubkey"].GetString()==applicant)
			{
				QuitCommissionerPool.erase(j);
				delete &_tx;
				break;
			}
			delete &_tx;
		}
		delete &tx;
	}
	//处理退出管家候选交易
	std::vector<PoVTransaction> QuitButlerCandidates;
	block.getQuitButlerCandateTransactions(QuitButlerCandidates);
	for(std::vector<PoVTransaction>::iterator i=QuitButlerCandidates.begin();i!=QuitButlerCandidates.end();i++)
	{
		rapidjson::Document& tx=i->getData();
		std::string applicant=tx["metadata"]["pubkey"].GetString();
		std::cout<<applicant<<"退出管家候选\n";
		account_manager.pop_butler_candidate(applicant);
		for(std::vector<DocumentContainer>::iterator j=QuitButlerCandidatePool.begin();j!=QuitButlerCandidatePool.end();j++)
		{
			rapidjson::Document& _tx=j->getDocument();
			if(_tx["metadata"]["pubkey"].GetString()==applicant)
			{
				QuitButlerCandidatePool.erase(j);
				delete &_tx;
				break;
			}
			delete &_tx;
		}
		delete &tx;
	}
	//std::cout<<"更新变量：6\n";
	//处理投票交易
	BallotPool.clear();
	std::vector<PoVTransaction> Votes;
	block.getVoteTransactions(Votes);
	for(std::vector<PoVTransaction>::iterator i=Votes.begin();i!=Votes.end();i++)
	{
		rapidjson::Document& tx=i->getData();
		rapidjson::Value& result=tx["metadata"]["result"];
		//account_manager.clear_butler();
		for(uint32_t i=0;i<result.Size();i++)
		{
			std::string butler=result[i].GetString();
			account_manager.pushback_butler(butler);
			account_manager.setButlerPubkeyByNum(i,butler);
		}
		delete &tx;
	}
	//std::cout<<"更新变量：7\n";
	//更新自己的账号身份
	std::string my_pubkey=account_manager.getMyPubkey();
	if(account_manager.is_commissioner(my_pubkey))
	{
		account_manager.setCommissioner();
	}
	else
	{
		account_manager.setNotCommissioner();
	}

	if(account_manager.is_butler_candidate(my_pubkey))
	{
		account_manager.setButlerCandidate();
	}
	else
	{
		account_manager.setNotButlerCandidate();
	}

	if(account_manager.is_butler(my_pubkey))
	{
		account_manager.setButler();
	}
	else
	{
		account_manager.setNotButler();
	}
	//std::cout<<"更新变量：8\n";
	//更新系统变量
	if(cache_block!=NULL)
	{
		delete cache_block;
		cache_block=NULL;
	}
	raw_block_signatures.clear();
	init_duty_butler_num=block.getNextButler();
	start_time=block.getTe();
	period_generated_block_num=(period_generated_block_num+1)%num_blocks;
	GenerateBlock=GenerateBlock_sign;
	Normal_vote=Vote_voting;
	std::cout<<"管家候选人数："<<account_manager.getButlerCandidateAmount()<<"\n";
	std::cout<<"委员人数："<<account_manager.getCommissionerAmount()<<"\n";
	//std::cout<<"更新变量：9\n";
}

PoVBlock POV::generateFirstBlock()
{
	PoVBlock block;
	//生成初始化交易
	rapidjson::Document system_data;
	system_data.SetObject();
	//把系统常量加入system_data_metadata中，即生成常量交易
	rapidjson::Value system_data_metadata;
	system_data_metadata.SetObject();
	rapidjson::Value butler_num(butler_amount);
	system_data_metadata.AddMember("butler_amout",butler_num,system_data.GetAllocator());
	rapidjson::Value _vote_amount(vote_amount);
	system_data_metadata.AddMember("vote_amount",_vote_amount,system_data.GetAllocator());
	//rapidjson::Value vote_num(vote_amount,system_data.GetAllocator());
	//system_data.AddMember("butler_amout",vote_num,system_data.GetAllocator());
	rapidjson::Value blocks_num(num_blocks);
	system_data_metadata.AddMember("blocks_per_cycle",blocks_num,system_data.GetAllocator());
	rapidjson::Value _Tcut(Tcut);
	system_data_metadata.AddMember("Tcut",_Tcut,system_data.GetAllocator());
	//把system_data_metadata，system_data_metatype等加入system_data中
	system_data.AddMember("metadata",system_data_metadata,system_data.GetAllocator());
	rapidjson::Value system_data_metatype("Constants",system_data.GetAllocator());
	system_data.AddMember("metatype",system_data_metatype,system_data.GetAllocator());
	block.PushBackTransaction(system_data);
	//print_document(system_data);
	//生成申请委员交易
	for(std::vector<DocumentContainer>::iterator i=ApplyCommissionerPool.begin();i!=ApplyCommissionerPool.end();i++)
	{
		rapidjson::Document& temp=i->getDocument();
		block.PushBackTransaction(temp);
		delete& temp;
	}
	//生成申请管家候选交易
	for(std::vector<DocumentContainer>::iterator i=ApplyButlerCandidatePool.begin();i!=ApplyButlerCandidatePool.end();i++)
	{
		rapidjson::Document& temp=i->getDocument();
		block.PushBackTransaction(temp);
		delete& temp;
	}
	//生成投票交易
	rapidjson::Document &vote_data=*(new rapidjson::Document);
	vote_data.SetObject();
	rapidjson::Document::AllocatorType& vote_data_allocator=vote_data.GetAllocator();
	//打包metatype
	rapidjson::Value &vote_metatype=*(new rapidjson::Value("vote",vote_data_allocator));
	vote_data.AddMember("metatype",vote_metatype,vote_data_allocator);
	//打包metadata
	rapidjson::Value vote_metadata;
	vote_metadata.SetObject();
	rapidjson::Document& result=BallotStatistic(BallotPool);
	rapidjson::Value& ballots=*(new rapidjson::Value(rapidjson::kArrayType));
	for(std::vector<DocumentContainer>::iterator i=BallotPool.begin();i!=BallotPool.end();i++)
	{
		ballots.PushBack(i->getDocument(),vote_data_allocator);
	}
	vote_metadata.AddMember("result",result,vote_data_allocator);
	vote_metadata.AddMember("ballots",ballots,vote_data_allocator);
	vote_data.AddMember("metadata",vote_metadata,vote_data_allocator);
	block.PushBackTransaction(vote_data);
	delete &vote_data;
	//print_document(vote_data);
	//生成区块头
	rapidjson::Document& txs=block.getTransactionsList();
	PoVHeader header;
	header.setHeight(0);	//创世区块高度为0
	header.setCycles(0);
	header.setNumOfTrans(txs.Size());
	header.setGenerator(account_manager.getMyPubkey());
	header.setPreviousHash(""); //创世区块的前一个区块hash为空
	header.setMerkleRoot(key_manager.getHash256(txs));  //这里用hash值代替merkle根
	//print_document(txs);
	block.setHeader(header);
	return block;
}

PoVBlock POV::generateBlock()
{
	PoVBlock block;
	//生成普通区块
	if(period_generated_block_num!=0)
	{
		//生成申请委员交易
		for(std::vector<DocumentContainer>::iterator i=ApplyCommissionerPool.begin();i!=ApplyCommissionerPool.end();i++)
		{
			rapidjson::Document& temp=i->getDocument();
			block.PushBackTransaction(temp);
			delete& temp;
		}
		//生成申请管家候选交易
		for(std::vector<DocumentContainer>::iterator i=ApplyButlerCandidatePool.begin();i!=ApplyButlerCandidatePool.end();i++)
		{
			rapidjson::Document& temp=i->getDocument();
			block.PushBackTransaction(temp);
			delete& temp;
		}
		//生成退出管家候选交易
		//std::cout<<"退出管家候选交易的数量="<<QuitButlerCandidatePool.size()<<"\n";
		for(auto i=QuitButlerCandidatePool.begin();i!=QuitButlerCandidatePool.end();i++)
		{

			rapidjson::Document& temp=i->getDocument();
			//这里加入限制现任管家不能申请退出管家候选，防止避免管家候选人数少于系统预定人数的情况发生
			std::string pubkey=temp["metadata"]["pubkey"].GetString();
			std::cout<<pubkey<<"打算退出管家候选\n";
			if(!account_manager.is_butler(pubkey))
			{
				block.PushBackTransaction(temp);
				std::cout<<"把"<<pubkey<<"的退出管家候选交易放进生区块中\n";
			}
			delete& temp;
		}
		//生成退出委员交易
		//std::cout<<"退出委员交易的数量="<<QuitCommissionerPool.size()<<"\n";
		for(auto i=QuitCommissionerPool.begin();i!=QuitCommissionerPool.end();i++)
		{

			rapidjson::Document& temp=i->getDocument();
			//这里再确认一次申请退出委员的账号是委员
			std::string pubkey=temp["metadata"]["pubkey"].GetString();
			std::cout<<pubkey<<"打算退出委员\n";
			if(account_manager.is_commissioner(pubkey))
			{
				block.PushBackTransaction(temp);
				std::cout<<"把"<<pubkey<<"的退出委员交易放进生区块中\n";
			}
			delete& temp;
		}
		//生成普通交易
		if(NormalPool.size()<max_normal_trans_per_block)
		{
			for(std::vector<DocumentContainer>::iterator i=NormalPool.begin();i!=NormalPool.end();i++)
			{
				rapidjson::Document& temp=i->getDocument();
				block.PushBackTransaction(temp);
				delete& temp;
			}
		}
		else
		{
			for(uint32_t i=0;i<max_normal_trans_per_block;i++)
			{
				uint32_t num=rand()%NormalPool.size();
				rapidjson::Document& temp=NormalPool.at(num).getDocument();
				block.PushBackTransaction(temp);
				NormalPool.erase(NormalPool.begin()+num);
				delete& temp;
			}
		}
	}
	else //生成特殊区块
	{
		//生成投票交易
		rapidjson::Document &vote_data=*(new rapidjson::Document);
		vote_data.SetObject();
		rapidjson::Document::AllocatorType& vote_data_allocator=vote_data.GetAllocator();
		//打包metatype
		rapidjson::Value &vote_metatype=*(new rapidjson::Value("vote",vote_data_allocator));
		vote_data.AddMember("metatype",vote_metatype,vote_data_allocator);
		//打包metadata
		rapidjson::Value vote_metadata;
		vote_metadata.SetObject();
		rapidjson::Document& result=BallotStatistic(BallotPool);
		rapidjson::Value& ballots=*(new rapidjson::Value(rapidjson::kArrayType));
		for(std::vector<DocumentContainer>::iterator i=BallotPool.begin();i!=BallotPool.end();i++)
		{
			ballots.PushBack(i->getDocument(),vote_data_allocator);
		}
		vote_metadata.AddMember("result",result,vote_data_allocator);
		vote_metadata.AddMember("ballots",ballots,vote_data_allocator);
		vote_data.AddMember("metadata",vote_metadata,vote_data_allocator);
		block.PushBackTransaction(vote_data);
		delete &vote_data;
	}

	//print_document(vote_data);
	//生成区块头
	rapidjson::Document& txs=block.getTransactionsList();
	PoVHeader header;
	header.setHeight(blockchain.getHeight()+1);
	//std::cout<<"set_height:"<<blockchain.getHeight()<<"\n";
	header.setCycles((int)((getCurrentTime()-start_time)/Tcut));
	header.setNumOfTrans(txs.Size());
	header.setGenerator(account_manager.getMyPubkey());
	PoVBlock pre_block=blockchain.getBlock(blockchain.getHeight());
	rapidjson::Document& h=pre_block.getHeader();
	std::string pre_hash=key_manager.getHash256(h);
	header.setPreviousHash(pre_hash);
	delete &h;
	header.setMerkleRoot(key_manager.getHash256(txs));
	//print_document(txs);
	block.setHeader(header);
	return block;
}

void POV::setConfig(std::string path)
{
	//读取配置文件并设置系统变量
    ConfigHelper configSettings(path);
    //设置初始状态的委员账号
    std::string commissioner_ids;
	commissioner_ids=configSettings.Read("委员节点", commissioner_ids);
	std::vector<std::string> com_ids_str=utils_split(commissioner_ids,",");
	delete[] Initial_Commissioner_Nodes;
	init_commissioner_size=com_ids_str.size();
	Initial_Commissioner_Nodes=new NodeID[init_commissioner_size];
	for(uint32_t i=0;i<com_ids_str.size();i++)
	{
		std::vector<std::string> addr=utils_split(com_ids_str[i],":");
		NodeID id=network->getNodeId(addr[0],atoi(addr[1].c_str()));
		//std::stringstream ss(com_ids_str.at(i));
		//ss>>id;
		//std::cout<<"id:"<<id<<std::endl;
		Initial_Commissioner_Nodes[i]=id;
	}
	//设置管家候选节点ID
    std::string butler_candidate_ids;
	butler_candidate_ids=configSettings.Read("管家候选节点", butler_candidate_ids);
	std::vector<std::string> butler_candidate_ids_str=utils_split(butler_candidate_ids,",");
	delete[] Initial_Butler_Candidate_Nodes;
	init_butler_candidate_size=butler_candidate_ids_str.size();
	Initial_Butler_Candidate_Nodes=new NodeID[init_butler_candidate_size];
	for(uint32_t i=0;i<butler_candidate_ids_str.size();i++)
	{
		std::vector<std::string> addr=utils_split(butler_candidate_ids_str[i],":");
		NodeID id=network->getNodeId(addr[0],atoi(addr[1].c_str()));
		//std::stringstream ss(butler_candidate_ids_str.at(i));
		//ss>>id;
		//std::cout<<"id:"<<id<<std::endl;
		Initial_Butler_Candidate_Nodes[i]=id;
	}
	//管家节点数
	butler_amount=configSettings.Read("管家节点数",butler_amount);
	//投票人数
	vote_amount=configSettings.Read("投票节点数",vote_amount);
	//每个任职周期产生区块数
	num_blocks=configSettings.Read("每个周期产生区块数",num_blocks);
	//每个区块产生的截止时间
	Tcut=configSettings.Read("区块产生截止时间",Tcut);
	//正常交易缓存池的容量大小
	NormalPoolCapacity=configSettings.Read("交易缓存池容量",NormalPoolCapacity);
	//每个区块最多存放的普通交易数量
	max_normal_trans_per_block=configSettings.Read("区块最大交易量",max_normal_trans_per_block);
	//每个管家开始打包区块前等待的时间
	defer_time=configSettings.Read("生成区块延迟",defer_time);
	//设置代理委员
	AgentCommissioner=Initial_Commissioner_Nodes[agent_commissioner_num];
	//设置初始节点身份
	for(uint32_t i=0;i<init_commissioner_size;i++)
	{
		if(Initial_Commissioner_Nodes[i]==network->getNodeId())
		{
			I_am_commissioner=true;
		}
	}
	for(uint32_t i=0;i<init_butler_candidate_size;i++)
	{
		if(Initial_Butler_Candidate_Nodes[i]==network->getNodeId())
		{
			I_am_butler_candidate=true;
		}
	}
	//设置密钥
	std::string pubkey;
	pubkey=configSettings.Read("公钥",pubkey);
	std::string prikey;
	prikey=configSettings.Read("私钥",prikey);
	bool ret1=key_manager.setPrivateKey(prikey);
	bool ret2=key_manager.setPublicKey(pubkey);
	if(ret1 && ret2)
	{
		my_account.setPubKey(pubkey);
		account_manager.setMyPubkey(pubkey);
		msg_manager.setPubkey(pubkey);
		blockchain.setPubkey(pubkey);
	}
	//清空区块链数据库
	std::string is_clear_database;
	is_clear_database=configSettings.Read("清空数据库",is_clear_database);
	if(is_clear_database=="yes")
	{
		blockchain.deleteBlockChainFromDatabase();
	}

	//加载区块链
	blockchain.loadBlockChain();
	//设置查询日志端口
	query_port=configSettings.Read("查询端口",query_port);
	query_thread=new std::thread(std::bind(&POV::handleLogQuery,this));
	query_thread->detach();
	//设置同步区块的开始时间
	fetch_height_wait_time=configSettings.Read("同步区块的等待时间",fetch_height_wait_time);
	fetch_height_wait_time_last_encore=configSettings.Read("发送高度请求的时间间隔",fetch_height_wait_time_last_encore);
	syncer_wait_time=configSettings.Read("同步器失效的等待时间",syncer_wait_time);
	//设置程序结束的区块高度
	end_height=configSettings.Read("结束高度",end_height);
	//设置每个节点产生交易的概率
	prob_generate_normal_tx=configSettings.Read("节点产生交易的概率",prob_generate_normal_tx);
	//设置环签名参数
	std::string ring_pubkeys_str;
	ring_pubkeys_str=configSettings.Read("ring_public_keys", ring_pubkeys_str);
	ring_pubkeys=utils_split(ring_pubkeys_str,",");
	std::string ring_prikeys_str;
	ring_prikeys_str=configSettings.Read("ring_private_keys", ring_prikeys_str);
	std::vector<std::string> ring_prikeys_vec=utils_split(ring_prikeys_str,",");
	ring_num=configSettings.Read("ring_number", ring_num);
	std::cout<<"ring_pubkey="<<ring_pubkeys[ring_num]<<"\n";
	//构造环私钥json字符串
	if(ring_num>=0)
	{
		std::map<std::string,std::string> private_key_map;
		private_key_map["pos"]=to_string(ring_num);
		private_key_map["prk_x"]=ring_prikeys_vec[ring_num];
		JsonConfigParser::convert_to_json_str(ring_prikey,private_key_map);
	}
	else
		ring_prikey="";
	std::cout<<"ring_prikey="<<ring_prikey<<"\n";
	//构造环参数的json字符串
	std::string g;
	std::string p;
	std::string q;
	g=configSettings.Read("g", g);
	p=configSettings.Read("p", p);
	q=configSettings.Read("q", q);
	std::map<std::string,std::string> ring_param_map;
	ring_param_map["g"]=g;
	ring_param_map["p"]=p;
	ring_param_map["q"]=q;
	JsonConfigParser::convert_to_json_str(ring_param,ring_param_map);
	std::cout<<"ring_param="<<ring_param<<"\n";
	key_manager.setRingParam(ring_pubkeys,ring_prikey,ring_param);
	/*
	//环签名验证测试
	//签名
	std::string message="sb lihui";
	std::string sig;
	bool result;
	LinkableRingSigImpl lrsi;
	lrsi.linkable_ring_sig(sig,message,ring_pubkeys,ring_prikey,ring_param);
	//验证
	lrsi.linkable_ring_verify(result,sig,message,ring_param);
	std::cout<<"result="<<result<<"\n";
	*/
	//域名文件读取信息
	reading_interval=configSettings.Read("reading_interval", reading_interval);
	file_path=configSettings.Read("file_path", file_path);
}

//deprecated
void POV::updateNodeId()
{
	account_manager.setMyNodeID(network->getNodeId());
	m_ID=network->getNodeId();

}

//初始化POV模块，注意一些对象的初始化顺序不能打乱
void POV::init(blockchain_network* bn)
{
	//std::cout<<"init POV:1\n";
	network=bn;
	key_manager.init();
	key_manager.setKeyPair();//使用随机生成的一对秘钥，在实际使用中建立存储恢复秘钥功能
	std::cout<<"生成的private key:"<<key_manager.getPrivateKey()<<"\n";
	std::cout<<"生成的public key:"<<key_manager.getPublicKey()<<"\n";
	//std::cout<<"init POV:2\n";
	//设置自己账号的公钥
	my_account.setPubKey(key_manager.getPublicKey());
	account_manager.setMyPubkey(key_manager.getPublicKey());
	account_manager.setMyNodeID(network->getNodeId());
	//std::cout<<"init POV:3\n";
	msg_manager.init(key_manager.getPublicKey(),network,&key_manager); 	//初始化消息管理器
	//std::cout<<"init POV:4\n";
	state=sync_block; //设置系统的初始状态为同步区块状态。
	CallBackList=std::vector<CallBackInstance>();
	m_ID=network->getNodeId();
	//std::cout<<"init POV:5\n";
	//申请成为委员的相关元数据
	Apply_Commissioner_Metadata.SetNull();
	Apply_Commissioner_Signatures=std::vector<signature>();
	Apply_Commissioner_Transaction.SetNull();
	ApplyCommissionerPool=std::vector<DocumentContainer>();
	//std::cout<<"init POV:6\n";
	gegenerate_first_bolck_apply_commissioner_state=ApplyCommissioner_apply_signatures;		//初始化申请委员过程的状态控制
	generate_first_bolck_apply_butler_candidate_state=ApplyButlerCandidate_apply_recommendation_letter;		//初始化申请管家候选过程的控制状态
	Apply_Butler_Candidate_Signatures=std::vector<signature>();
	ApplyButlerCandidatePool=std::vector<DocumentContainer>();
	BallotPool=std::vector<DocumentContainer>();
	Quit_Butler_Candidate_Metadata.SetNull();
	Quit_Commissioner_Metadata.SetNull();
	GenerateFirstBlock_vote=Vote_voting;	//初始化投票过程的控制状态
	GenerateFirstBlock=GenerateBlock_sign;	//初始化生成区块过程的控制状态
	//初始化正常阶段的各个过程控制状态
	normal_apply_commissioner_state=ApplyCommissioner_finish;
	normal_apply_butler_candidate_state=ApplyButlerCandidate_finish;
	normal_quit_butler_candidate_state=QuitButlerCandidate_finish;
	normal_quit_commissioner_state=QuitCommissioner_finish;
	//std::cout<<"init POV:7\n";
	//vote_number=vote_amount;
	cache_block=NULL;
	//std::cout<<"init POV:71\n"
	//setClient(client);
	//blockchain=PoVBlockChain(client);
	blockchain.clear();
	blockchain.setPubkey(key_manager.getPublicKey());
	//blockchain.setCollection(client,my_account.getPubKey());
	//std::cout<<"init POV:72\n";
	AgentCommissioner=0;
	raw_block_signatures=std::vector<signature>();
	NormalPool=std::vector<DocumentContainer>();
	GenerateBlock=GenerateBlock_sign;
	Initial_Commissioner_Nodes=new NodeID[init_commissioner_size];
	Initial_Butler_Candidate_Nodes=new NodeID[init_butler_candidate_size];
	ptr=std::shared_ptr<POV>(this);
	//std::cout<<"init POV:8\n";
	/*
	for(uint32_t i=0;i<init_commissioner_size;i++)
	{
		if(Initial_Commissioner_Nodes[i]==network->getNodeId())
		{
			I_am_commissioner=true;
		}
	}
	for(uint32_t i=0;i<init_butler_candidate_size;i++)
	{
		if(Initial_Butler_Candidate_Nodes[i]==network->getNodeId())
		{
			I_am_butler_candidate=true;
		}
	}
	*/
	//std::cout<<"init POV:9\n";
}

void POV::Initial_ApplyCommissioner()
{
	switch(gegenerate_first_bolck_apply_commissioner_state)
	{
		case ApplyCommissioner_apply_signatures:
		{
			std::cout<<"申请委员：1!\n";
			//把metadata 存放到Apply_Commissioner_Metadata中
			if(Apply_Commissioner_Metadata.IsNull())
			{
				//如果Apply_Commissioner_Metadata为空则把metadata复制到Apply_Commissioner_Metadata中保存起来。
				rapidjson::Document& metadata=msg_manager.get_Apply_Commissioner_Metadata();
				rapidjson::Document::AllocatorType& a=metadata.GetAllocator();
				Apply_Commissioner_Metadata.GetAllocator().Clear();
				Apply_Commissioner_Metadata.CopyFrom(metadata,Apply_Commissioner_Metadata.GetAllocator());
				delete &metadata;
			}

			uint32_t commissioners_num=account_manager.getCommissionerAmount();
			for(uint32_t i=0;i<commissioners_num;i++)
			{
				account& com=account_manager.getCommissioner(i);

				rapidjson::Document& d=msg_manager.make_Request_Apply_Commissioner_Signature((NodeID)com.getNodeId(),Apply_Commissioner_Metadata);
				//NodeID id=com.getNodeId();
				sendMessage(d,true,10,Recall_RequestApplyCommissionerSignature,(NodeID)com.getNodeId(),&POV::handleResponseApplyCommissionerSignature);
				delete &d;
			}
			break;
		}
		case ApplyCommissioner_final_apply:
		{
			std::cout<<"申请委员：2!\n";
			std::cout<<network->getNodeId()<<"收集到所有委员的签名\n";
			//设置Apply_Commissioner_Transaction，并发送给代理委员
			if(Apply_Commissioner_Transaction.IsNull())
			{
				Apply_Commissioner_Transaction.GetAllocator().Clear();
				std::cout<<network->getNodeId()<<"设置Apply_Commissioner_Transaction\n";
				Apply_Commissioner_Transaction.SetObject();
				rapidjson::Document::AllocatorType &allocator = Apply_Commissioner_Transaction.GetAllocator();
				rapidjson::Document::AllocatorType &a = Apply_Commissioner_Metadata.GetAllocator();
				//统一每个交易的json数据中的第一层都加上metatype，方便后面代码处理
				rapidjson::Value _metatype("ApplyCommissioner",a);
				Apply_Commissioner_Transaction.AddMember("metatype",_metatype,a);
				rapidjson::Value metadata(Apply_Commissioner_Metadata,a);
				Apply_Commissioner_Transaction.AddMember("metadata",metadata,allocator);
				//print_document(Apply_Commissioner_Transaction);
				rapidjson::Value sig_array(rapidjson::kArrayType);
				for(uint32_t i=0;i<Apply_Commissioner_Signatures.size();i++)
				{
					std::string str_pubkey=Apply_Commissioner_Signatures.at(i).pubkey;
					std::string str_sig=Apply_Commissioner_Signatures.at(i).sig;
					rapidjson::Value sig;
					sig.SetObject();
					rapidjson::Value pubkeyValue(str_pubkey.c_str(),str_pubkey.size(),allocator);
					sig.AddMember("pubkey",pubkeyValue,allocator);
					rapidjson::Value sigValue(str_sig.c_str(),str_sig.size(),allocator);
					sig.AddMember("sig",sigValue,allocator);
					sig_array.PushBack(sig,allocator);
				}
				Apply_Commissioner_Transaction.AddMember("sigs",sig_array,allocator);
				std::cout<<"完成设置Apply_Commissioner_Transaction！\n";
				//print_document(Apply_Commissioner_Transaction);
			}
			//rapidjson::Document::AllocatorType &allocator = Apply_Commissioner_Transaction.GetAllocator();
			//rapidjson::Document temp_doc;
			//temp_doc.CopyFrom(Apply_Commissioner_Transaction,allocator);
			//print_document(temp_doc);
			rapidjson::Document& msg=msg_manager.make_Request_Apply_Commissioner(AgentCommissioner,Apply_Commissioner_Transaction);
			sendMessage(msg,true,10,Recall_RequestApplyCommissioner,AgentCommissioner,&POV::handleResponseApplyCommissioner);
			delete &msg;
			break;
		}
		case ApplyCommissioner_finish:
		{
			//std::cout<<"申请委员：3!\n";
			break;
		}
	}
	/*GenerateFirstBlock_ApplyCommissioner*/
}

void POV::Initial_ApplyButlerCandidate()
{
	switch(generate_first_bolck_apply_butler_candidate_state)
	{
		std::cout<<network->getNodeId()<<"申请管家候选：1!\n";
		case ApplyButlerCandidate_apply_recommendation_letter:
		{
			//生成创世区块阶段的请求推荐信是发送给随机的一个委员
			uint32_t comm_num=rand()%init_commissioner_size;
			NodeID comm_ID=Initial_Commissioner_Nodes[comm_num];

			rapidjson::Document &letter_request=msg_manager.make_Request_Recommend_Letter(comm_ID);
			sendMessage(letter_request,true,10,Recall_RequestApplyRecommendationLetter,comm_ID,&POV::handleResponseRecommendationLetter);
			delete &letter_request;
			break;
		}
		case ApplyButlerCandidate_apply_signatures:
		{
			std::cout<<network->getNodeId()<<"申请管家候选：2!\n";
			//std::cout<<"申请管家候选阶段：ApplyButlerCandidate_apply_signatures\n";
			//rapidjson::Value &letter=RecommendationLetter["data"];
			//申请管家候选签名发送给所有委员
			uint32_t commissioners_num=account_manager.getCommissionerAmount();
			for(uint32_t i=0;i<commissioners_num;i++)
			{

				account comm=account_manager.getCommissioner(i);
				rapidjson::Document& msg=msg_manager.make_Request_Apply_Butler_Candidate_Signature((NodeID)comm.getNodeId(),RecommendationLetter);
				//print_document(msg);
				sendMessage(msg,true,10,Recall_RequestApplyButlerCandidateSignature,(NodeID)comm.getNodeId(),&POV::handleResponseApplyButlerCandidateSignature);
				delete &msg;
			}

			break;
		}
		case ApplyButlerCandidate_final_apply:
		{
			//最终的管家候选申请交易包含一个推荐信，以及超过一半委员的签名，最终发送给所有管家
			std::cout<<"申请管家候选：3!\n";
			rapidjson::Document &data=*(new rapidjson::Document);
			data.SetObject();
			rapidjson::Document::AllocatorType &allocator=data.GetAllocator();

			rapidjson::Value& _metatype=*(new rapidjson::Value("ApplyButlerCandidate",allocator));
			data.AddMember("metatype",_metatype,allocator);

			rapidjson::Value &recommendation_letter=*(new rapidjson::Value);
			recommendation_letter.CopyFrom(RecommendationLetter,allocator);
			data.AddMember("recommendation_letter",recommendation_letter,allocator);

			rapidjson::Value sigs(rapidjson::kArrayType);
			for(uint32_t i=0;i<Apply_Butler_Candidate_Signatures.size()/2;i++)
			{
				std::string pubkey=Apply_Butler_Candidate_Signatures[i].pubkey;
				std::string sig=Apply_Butler_Candidate_Signatures[i].sig;
				rapidjson::Value &doc=*(new rapidjson::Value);
				doc.SetObject();
				//rapidjson::Document::AllocatorType &a=doc.GetAllocator();
				rapidjson::Value _pubkey(pubkey.c_str(),pubkey.size(),allocator);
				rapidjson::Value _sig(sig.c_str(),sig.size(),allocator);
				doc.AddMember("pubkey",_pubkey,allocator);
				doc.AddMember("sig",_sig,allocator);
				sigs.PushBack(doc,allocator);
			}
			data.AddMember("sigs",sigs,allocator);
			for(uint32_t i=0;i<account_manager.getCommissionerAmount();i++)
			{
				NodeID id=account_manager.getCommissioner(i).getNodeId();
				//rapidjson::Document& copy_data=*(new rapidjson::Document);
				//copy_data.CopyFrom(data,copy_data.GetAllocator());
				rapidjson::Document& msg=msg_manager.make_Request_Apply_Butler_Candidate(id,data);
				//print_document(msg);
				sendMessage(msg,true,20,Recall_RequestApplyButlerCandidate,id,&POV::handleResponseApplyButlerCandidate);
				delete &msg;
			}
			delete &data;
			break;
		}
		case ApplyButlerCandidate_finish:
		{
			//std::cout<<"申请管家候选：4!\n";
			break;
		}
	}/*switch(generate_first_bolck_apply_butler_candidate_state)*/
}

void POV::Initial_Vote()
{
	switch(GenerateFirstBlock_vote)
	{
		case Vote_voting:
		{
			//std::cout<<network->getNodeId()<<"投票：1!\n";
			//只有管家候选人数等于预设的管家人数才可以进行投票
			if(account_manager.getButlerCandidateAmount()==init_butler_candidate_size)
			{
				//std::cout<<network->getNodeId()<<"节点的委员人数满足要求，管家候选人数满足要求！\n";
				std::vector<account>& butler_candidate_list=account_manager.get_Butler_Candidate_List();
				rapidjson::Document &ballot=account_manager.getBallot(vote_amount);
				rapidjson::Document& msg=msg_manager.make_Request_Vote(AgentCommissioner,ballot);
				//print_document(msg);
				sendMessage(msg,true,10,Recall_RequestVote,AgentCommissioner,&POV::handleResponseVote);
				delete &ballot;
				delete &msg;
			}
			break;
		}
		case Vote_finish:
		{
			//std::cout<<"投票：2!\n";
			break;
		}
	}
}

void POV::Initial_GenerateBlock()
{
	switch(GenerateFirstBlock)
	{
		case GenerateBlock_sign:
		{
			//生成区块并获取委员的签名
			//std::cout<<"生成创世区块：1!\n";
			if(ApplyCommissionerPool.size()<init_commissioner_size)
			{
				break;
			}
			if(ApplyButlerCandidatePool.size()<init_butler_candidate_size)
			{
				break;
			}
			if(BallotPool.size()<init_commissioner_size)
			{
				break;
			}
			if(cache_block==NULL)
			{
				PoVBlock povblock=generateFirstBlock();
				cache_block=new PoVBlock();
				cache_block->copyfrom(povblock);
			}

			//print_document(cache_block->getBlock());
			uint32_t commissioners_num=account_manager.getCommissionerAmount();
			//std::cout<<"委员数量为："<<commissioners_num<<"!\n";
			for(uint32_t i=0;i<commissioners_num;i++)
			{
				//这里倒序发送是因为顺序发送时发现最后一个节点收不到消息，倒序发送又可以，原因不明
				account comm=account_manager.getCommissioner(i);
				//std::cout<<"发送生区块给"<<comm.getNodeId()<<"\n";
				rapidjson::Document& raw_zero_block=cache_block->getRawBlock();
				rapidjson::Document& msg=msg_manager.make_Request_Block_Signature((NodeID)comm.getNodeId(),raw_zero_block);
				sendMessage(msg,true,100,Recall_RequestBlockSignature,(NodeID)comm.getNodeId(),&POV::handleResponseBlockSignature);
				delete &raw_zero_block;
				delete &msg;
			}
			break;
		}
		case GenerateBlock_publish:
		{
			//发布生成的区块
			std::cout<<"生成创世区块：2!\n";
			if(raw_block_signatures.size()!=account_manager.getCommissionerAmount())
			{
				break;
			}
			cache_block->setSigs(raw_block_signatures);
			uint32_t next_butler_num=getNextButler(cache_block->getPoVHeader().getSignatures());
			//std::cout<<"butler_num="<<next_butler_num<<"\n";
			cache_block->setNextButler(next_butler_num);
			cache_block->setTe(getTe(cache_block->getPoVHeader().getSignatures()));
			rapidjson::Document& complete_block=cache_block->getBlock();
			rapidjson::Document& msg=msg_manager.make_Request_Publish_Block(complete_block);
			sendMessage(msg,false,30,Recall_RequestBlockSignature,0,&POV::handleResponseBlockSignature);
			GenerateFirstBlock=GenerateBlock_finish;
			delete &complete_block;
			delete &msg;
			break;
		}
		case GenerateBlock_finish:
		{
			//std::cout<<"生成创世区块：3!\n";
			delete cache_block;
			cache_block=NULL;
			raw_block_signatures.clear();
			break;
		}
	}
}

void POV::Normal_Vote()
{
	switch(Normal_vote)
	{
		case Vote_voting:
		{
			if(account_manager.getButlerCandidateAmount()>=butler_amount)
			{
				/*
				std::cout<<network->getNodeId()<<"管家候选人数满足要求！\n";
				for(uint32_t i=0;i<butler_amount;i++)
				{
					std::string butler_pubkey=account_manager.getButlerPubkeyByNum(i);
					//std::cout<<"和管家编号对应的管家公钥："<<butler_pubkey<<"\n";
				}
				for(uint32_t i=0;i<butler_amount;i++)
				{
					NodeID butler_id=account_manager.butler_list[i].getNodeId();
					//std::cout<<"管家ID："<<butler_id<<"\n";
				}
				*/
				for(uint32_t i=0;i<butler_amount;i++)
				{
					std::string butler_pubkey=account_manager.getButlerPubkeyByNum(i);
					NodeID butler_id=account_manager.get_Butler_ID(butler_pubkey);
					if(butler_id==0)
					{
						//id为0表示没有id，需要请求ID
						//std::cout<<"发送NodeID请求!\n";
						char* ch=const_cast<char*>(butler_pubkey.c_str());
						uint32_t *map_id=(uint32_t*)ch;
						rapidjson::Document& msg=msg_manager.make_Request_NodeID_By_Pubkey(butler_pubkey);
						sendMessage(msg,true,10,Recall_RequestNodeID,*map_id,&POV::handleResponseNodeID);
						delete &msg;
					}
					else
					{
						//std::cout<<account_manager.getMyNodeID()<<"发送投票给"<<butler_id<<"\n";
						std::vector<account>& butler_candidate_list=account_manager.get_Butler_Candidate_List();
						rapidjson::Document &ballot=account_manager.getBallot(vote_amount);
						rapidjson::Document& msg=msg_manager.make_Request_Vote(butler_id,ballot);
						//print_document(msg);
						sendMessage(msg,true,10,Recall_RequestVote,butler_id,&POV::handleResponseVote);
						delete &ballot;
						delete &msg;
					}
				}

			}
			break;
		}
		case Vote_finish:
		{
			break;
		}
	}
}

void POV::Normal_GenerateBlock()
{
	switch(GenerateBlock)
	{
		case GenerateBlock_sign:
		{
			//std::cout<<"在区块签名阶段\n";
			//period_generated_block_num为0表示下个生成的区块是特殊区块
			if(period_generated_block_num==0)
			{
				if(BallotPool.size()<account_manager.getCommissionerAmount()/2.0)
				{
					//std::cout<<"period_generated_block_num="<<period_generated_block_num<<"\n";
					//std::cout<<"BallotPool.size()="<<BallotPool.size()<<"\n";
					//std::cout<<"account_manager.getCommissionerAmount()/2.0="<<account_manager.getCommissionerAmount()/2.0<<"\n";
					break;
				}
			}
			//std::cout<<"在区块签名阶段——1\n";
			if(cache_block==NULL)
			{
				PoVBlock povblock=generateBlock();
				//std::cout<<"generate block:1\n";
				cache_block=new PoVBlock();
				cache_block->copyfrom(povblock);
				//print_document(cache_block->getBlock());
			}

			//std::cout<<"在区块签名阶段——2\n";
			uint32_t commissioners_num=account_manager.getCommissionerAmount();
			if(raw_block_signatures.size()>account_manager.getCommissionerAmount()/2.0)
			{
				GenerateBlock=GenerateBlock_publish;
			}
			for(uint32_t i=0;i<commissioners_num;i++)
			{
				account comm=account_manager.getCommissioner(i);
				if(comm.getNodeId()==0)
				{
					//std::cout<<"发送NodeID请求!\n";
					char* ch=const_cast<char*>(comm.getPubKey().c_str());
					uint32_t *map_id=(uint32_t*)ch;
					rapidjson::Document& msg=msg_manager.make_Request_NodeID_By_Pubkey(comm.getPubKey());
					sendMessage(msg,true,10,Recall_RequestNodeID,*map_id,&POV::handleResponseNodeID);
					delete &msg;
				}
				else
				{
					//先获取群签名，再用群签名对区块头签名发送给相应的委员节点
					NodeID comm_ID=(NodeID)comm.getNodeId();
					std::string pubkey=comm.getPubKey();
					auto iter=group_list.find(pubkey);
					if(iter==group_list.end())
					{
						//std::cout<<"发送群签名：1\n";
						rapidjson::Document& msg=msg_manager.make_Request_Group_Key(comm_ID);
						//std::cout<<"发送群签名：2\n";
						//print_document(msg);
						sendMessage(msg,true,10,Recall_RequestGroupKey,comm_ID,&POV::handleResponseGroupKey);
						break;
					}
					//std::cout<<"发送生区块给"<<comm.getNodeId()<<"\n";
					rapidjson::Document& raw_zero_block=cache_block->getRawBlock();
					bool sig_existed=false;
					for(int i=0;i<raw_block_signatures.size();i++)
					{
						std::string key=raw_block_signatures[i].pubkey;
						if(comm.getPubKey()==key)
						{
							sig_existed=true;
							break;
						}
					}
					if(sig_existed)
					{
						delete &raw_zero_block;
					}
					else
					{
						//rapidjson::Document& msg=msg_manager.make_Request_Block_Signature((NodeID)comm.getNodeId(),raw_zero_block);
						group_param param=iter->second;
						//std::cout<<"向NodeID为"<<comm_ID<<"的节点发送区块签名请求:\n";
						rapidjson::Document& msg=msg_manager.make_Request_Block_Signature_With_Group_Sig(comm_ID,raw_zero_block,
								param.algorithm_method,
								param.gpk,
								param.gsk_info,
								param.pbc_param);
						//print_document(msg);
						sendMessage(msg,true,10,Recall_RequestBlockSignature,comm_ID,&POV::handleResponseBlockSignature);

						delete &raw_zero_block;
						delete &msg;
					}
				}

			}
			//std::cout<<"在区块签名阶段——3\n";
			break;
		}
		case GenerateBlock_publish:
		{
			//std::cout<<"在区块发布阶段\n";
			if(raw_block_signatures.size()<=account_manager.getCommissionerAmount()/2.0)
			{
				break;
			}
			cache_block->setSigs(raw_block_signatures);
			uint32_t next_butler_num=getNextButler(cache_block->getPoVHeader().getSignatures());
			//std::cout<<"butler_num="<<next_butler_num<<"\n";
			cache_block->setNextButler(next_butler_num);
			cache_block->setTe(getTe(cache_block->getPoVHeader().getSignatures()));
			rapidjson::Document& complete_block=cache_block->getBlock();
			rapidjson::Document& msg=msg_manager.make_Request_Publish_Block(complete_block);
			sendMessage(msg,false,30,Recall_RequestBlockSignature,0,&POV::handleResponseBlockSignature);
			GenerateBlock=GenerateBlock_finish;
			delete &complete_block;
			delete &msg;
			break;
		}
		case GenerateBlock_finish:
		{
			//std::cout<<"在区块生成结束阶段\n";
			delete cache_block;
			cache_block=NULL;
			raw_block_signatures.clear();
			break;
		}
	}/*switch(Normal_vote)*/
}

void POV::Normal_PublishTransaction()
{
	if(getCurrentTime()-last_reading_time>reading_interval)
	{
	    char buffer[1024];
	    std::fstream outFile;
	    outFile.open(file_path,ios::in);
	    //std::cout<<file_path<<"--- all file is as follows:---"<<endl;
	    while(!outFile.eof())
	    {
	    	std::memset(buffer,0,sizeof(buffer));
	        outFile.getline(buffer,1024,'\n');//getline(char *,int,char) 表示该行字符达到256个或遇到换行就结束
	        std::string domain_name(buffer);
	        bool existed=false;
	        for(int i=0;i<domain_names_list.size();i++)
	        {
	        	if(domain_name==domain_names_list[i])
	        	{
	        		existed=true;
	        		break;
	        	}
	        }
	        if(!existed && domain_name!="")
	        {
	        	domain_names_list.push_back(domain_name);
		        std::cout<<"生成域名："<<domain_name<<endl;
	        }
	        //else
		        //std::cout<<"域名"<<buffer<<"已存在"<<endl;
	    }
	    outFile.close();
	    last_reading_time=getCurrentTime();
	}
	//随机发布正常交易，content字段只有一个自定义长度的随机字符串，测试用
	if(prob_generate_normal_tx<1)
	{
		double r=getRandomNum();
		//std::cout<<"在"<<network->getNodeId()<<"的Normal状态中：1!\n";

		if(r<prob_generate_normal_tx)
		{
			rapidjson::Document& msg=msg_manager.make_Request_Normal(tx_len);
			sendMessage(msg,false,100,Recall_RequestNormal,0,&POV::handleResponseNormal);
			delete &msg;
			/*
			//uint32_t i=rand()%account_manager.getButlerAmount();
			for(uint32_t i=0;i<account_manager.getButlerAmount();i++)
			{
				account& butler=account_manager.getButler(i);
				NodeID id=butler.getNodeId();
				if(id==0)
				{
					std::cout<<"发送NodeID请求!\n";
					char* ch=const_cast<char*>(butler.getPubKey().c_str());
					uint32_t *map_id=(uint32_t*)ch;
					rapidjson::Document& msg=msg_manager.make_Request_NodeID_By_Pubkey(butler.getPubKey());
					sendMessage(msg,true,100,Recall_RequestNodeID,*map_id,&POV::handleResponseNodeID);
					//delete map_id;
					delete &msg;
				}
				else
				{
					std::cout<<"发送Normal交易请求!\n";
					rapidjson::Document& msg=msg_manager.make_Request_Normal(id,tx_len);
					sendMessage(msg,true,100,Recall_RequestNormal,id,&POV::handleResponseNormal);
					delete &msg;
				}
			}
			*/
		}

	}
	else
	{
		for(int i=0;i<prob_generate_normal_tx;i++)
		{
			rapidjson::Document& msg=msg_manager.make_Request_Normal(tx_len);
			sendMessage(msg,false,100,Recall_RequestNormal,0,&POV::handleResponseNormal);
			delete &msg;
		}
	}

}

void POV::Normal_ApplyCommissioner()
{
	switch(normal_apply_commissioner_state)
	{
		case ApplyCommissioner_apply_signatures:
		{
			//把metadata 存放到Apply_Commissioner_Metadata中
			if(Apply_Commissioner_Metadata.IsNull())
			{
				//如果Apply_Commissioner_Metadata为空则把metadata复制到Apply_Commissioner_Metadata中保存起来。
				rapidjson::Document& metadata=msg_manager.get_Apply_Commissioner_Metadata();
				rapidjson::Document::AllocatorType& a=metadata.GetAllocator();
				Apply_Commissioner_Metadata.GetAllocator().Clear();
				Apply_Commissioner_Metadata.CopyFrom(metadata,Apply_Commissioner_Metadata.GetAllocator());
				delete &metadata;
			}

			uint32_t commissioners_num=account_manager.getCommissionerAmount();
			for(uint32_t i=0;i<commissioners_num;i++)
			{
				account& com=account_manager.getCommissioner(i);
				std::string comm_pubkey=com.getPubKey();
				NodeID comm_id=com.getNodeId();
				if(comm_id==0)
				{
					std::cout<<"发送NodeID请求!\n";
					char* ch=const_cast<char*>(comm_pubkey.c_str());
					uint32_t *map_id=(uint32_t*)ch;
					rapidjson::Document& msg=msg_manager.make_Request_NodeID_By_Pubkey(comm_pubkey);
					sendMessage(msg,true,10,Recall_RequestNodeID,*map_id,&POV::handleResponseNodeID);
					delete &msg;
				}
				else
				{
					/*
					//把Apply_Commissioner_Metadata复制到新建的metadata中.
					rapidjson::Document metadata;
					rapidjson::Document::AllocatorType& a=Apply_Commissioner_Metadata.GetAllocator();
					metadata.SetNull();
					metadata.GetAllocator().Clear();
					metadata.CopyFrom(Apply_Commissioner_Metadata,metadata.GetAllocator());
					//创建消息请求签名

					rapidjson::Document& d=msg_manager.make_Request_Apply_Commissioner_Signature((NodeID)com.getNodeId(),metadata);
					*/
					rapidjson::Document& d=msg_manager.make_Request_Apply_Commissioner_Signature((NodeID)com.getNodeId(),Apply_Commissioner_Metadata);
					//NodeID id=com.getNodeId();
					sendMessage(d,true,10,Recall_RequestApplyCommissionerSignature,(NodeID)com.getNodeId(),&POV::handleResponseApplyCommissionerSignature);
					delete &d;
				}
			}
			break;
		}
		case ApplyCommissioner_final_apply:
		{
			std::cout<<network->getNodeId()<<"收集到所有委员的签名\n";
			//设置Apply_Commissioner_Transaction，并发送给代理委员
			if(Apply_Commissioner_Transaction.IsNull())
			{
				Apply_Commissioner_Transaction.GetAllocator().Clear();
				std::cout<<network->getNodeId()<<"设置Apply_Commissioner_Transaction\n";
				Apply_Commissioner_Transaction.SetObject();
				rapidjson::Document::AllocatorType &allocator = Apply_Commissioner_Transaction.GetAllocator();
				rapidjson::Document::AllocatorType &a = Apply_Commissioner_Metadata.GetAllocator();
				//统一每个交易的json数据中的第一层都加上metatype，方便后面代码处理
				rapidjson::Value _metatype("ApplyCommissioner",a);
				Apply_Commissioner_Transaction.AddMember("metatype",_metatype,a);
				rapidjson::Value metadata(Apply_Commissioner_Metadata,allocator);
				Apply_Commissioner_Transaction.AddMember("metadata",metadata,allocator);
				//print_document(Apply_Commissioner_Transaction);
				rapidjson::Value sig_array(rapidjson::kArrayType);
				for(uint32_t i=0;i<Apply_Commissioner_Signatures.size();i++)
				{
					std::string str_pubkey=Apply_Commissioner_Signatures.at(i).pubkey;
					std::string str_sig=Apply_Commissioner_Signatures.at(i).sig;
					rapidjson::Value sig;
					sig.SetObject();
					rapidjson::Value pubkeyValue(str_pubkey.c_str(),str_pubkey.size(),allocator);
					sig.AddMember("pubkey",pubkeyValue,allocator);
					rapidjson::Value sigValue(str_sig.c_str(),str_sig.size(),allocator);
					sig.AddMember("sig",sigValue,allocator);
					sig_array.PushBack(sig,allocator);
				}
				Apply_Commissioner_Transaction.AddMember("sigs",sig_array,allocator);
				std::cout<<"完成设置Apply_Commissioner_Transaction！\n";
				//print_document(Apply_Commissioner_Transaction);
			}

			//print_document(temp_doc);
			for(uint32_t i=0;i<account_manager.getButlerAmount();i++)
			{
				std::string butler_pubkey=account_manager.getButlerPubkeyByNum(i);
				NodeID butler_id=account_manager.get_Butler_ID(butler_pubkey);
				if(butler_id==0)
				{
					std::cout<<"发送NodeID请求!\n";
					char* ch=const_cast<char*>(butler_pubkey.c_str());
					uint32_t *map_id=(uint32_t*)ch;
					rapidjson::Document& msg=msg_manager.make_Request_NodeID_By_Pubkey(butler_pubkey);
					sendMessage(msg,true,10,Recall_RequestNodeID,*map_id,&POV::handleResponseNodeID);
					delete &msg;
				}
				else
				{
					std::cout<<"发送申请委员交易"<<1<<"\n";
					//rapidjson::Document::AllocatorType &allocator = Apply_Commissioner_Transaction.GetAllocator();
					//rapidjson::Document &temp_doc=*(new rapidjson::Document());
					//rapidjson::Document::AllocatorType &allocator =temp_doc.GetAllocator();
					//temp_doc.CopyFrom(Apply_Commissioner_Transaction,allocator);
					rapidjson::Document& msg=msg_manager.make_Request_Apply_Commissioner(butler_id,Apply_Commissioner_Transaction);
					sendMessage(msg,true,10,Recall_RequestApplyCommissioner,butler_id,&POV::handleResponseApplyCommissioner);
					delete &msg;
					std::cout<<"发送申请委员交易"<<2<<"\n";

				}
			}
			break;
		}
		case ApplyCommissioner_finish:
		{
			double r=getRandomNum();
			if(r<prob_apply_commissioner)
			{
				std::cout<<"in applycommissioner_finish state\n";
				if(!Apply_Commissioner_Transaction.IsNull())
				{
					std::cout<<"in applycommissioner_finish state 1\n";
					Apply_Commissioner_Transaction.SetNull();
					std::cout<<"in applycommissioner_finish state 2\n";
					Apply_Commissioner_Transaction.GetAllocator().Clear();
				}
				std::cout<<"in applycommissioner_finish state 3\n";
				Apply_Commissioner_Signatures.clear();
				std::cout<<"in applycommissioner_finish state 4\n";
				normal_apply_commissioner_state=ApplyCommissioner_apply_signatures;
			}
			break;
		}
	}
	/*Normal_ApplyCommissioner*/
}

void POV::Normal_ApplyButlerCandidate()
{
	switch(normal_apply_butler_candidate_state)
	{
		case ApplyButlerCandidate_apply_recommendation_letter:
		{
			uint32_t comm_num=rand()%account_manager.getCommissionerAmount();
			account comm=account_manager.getCommissioner(comm_num);
			NodeID comm_ID=comm.getNodeId();
			if(comm_ID==0)
			{
				std::string pubkey=comm.getPubKey();
				char* ch=const_cast<char*>(pubkey.c_str());
				uint32_t *map_id=(uint32_t*)ch;
				rapidjson::Document& msg=msg_manager.make_Request_NodeID_By_Pubkey(pubkey);
				sendMessage(msg,true,10,Recall_RequestNodeID,*map_id,&POV::handleResponseNodeID);
				delete &msg;
			}
			else
			{
				rapidjson::Document &letter_request=msg_manager.make_Request_Recommend_Letter(comm_ID);
				sendMessage(letter_request,true,10,Recall_RequestApplyRecommendationLetter,comm_ID,&POV::handleResponseRecommendationLetter);
				delete &letter_request;
			}
			break;
		}
		case ApplyButlerCandidate_apply_signatures:
		{
			//std::cout<<"申请管家候选阶段：ApplyButlerCandidate_apply_signatures\n";
			//rapidjson::Value &letter=RecommendationLetter["data"];
			uint32_t commissioners_num=account_manager.getCommissionerAmount();
			for(uint32_t i=0;i<commissioners_num;i++)
			{
				account comm=account_manager.getCommissioner(i);
				NodeID comm_ID=comm.getNodeId();
				if(comm_ID==0)
				{
					std::string pubkey=comm.getPubKey();
					char* ch=const_cast<char*>(pubkey.c_str());
					uint32_t *map_id=(uint32_t*)ch;
					rapidjson::Document& msg=msg_manager.make_Request_NodeID_By_Pubkey(pubkey);
					sendMessage(msg,true,10,Recall_RequestNodeID,*map_id,&POV::handleResponseNodeID);
					delete &msg;
				}
				else
				{
					//account comm=account_manager.getCommissioner(i);
					rapidjson::Document& msg=msg_manager.make_Request_Apply_Butler_Candidate_Signature((NodeID)comm.getNodeId(),RecommendationLetter);
					//print_document(msg);
					sendMessage(msg,true,10,Recall_RequestApplyButlerCandidateSignature,(NodeID)comm.getNodeId(),&POV::handleResponseApplyButlerCandidateSignature);
					delete &msg;
				}
			}

			break;
		}
		case ApplyButlerCandidate_final_apply:
		{
			rapidjson::Document &data=*(new rapidjson::Document);
			data.SetObject();
			rapidjson::Document::AllocatorType &allocator=data.GetAllocator();

			rapidjson::Value& _metatype=*(new rapidjson::Value("ApplyButlerCandidate",allocator));
			data.AddMember("metatype",_metatype,allocator);

			rapidjson::Document &recommendation_letter=*(new rapidjson::Document);
			recommendation_letter.CopyFrom(RecommendationLetter,recommendation_letter.GetAllocator());
			data.AddMember("recommendation_letter",recommendation_letter,allocator);

			rapidjson::Value sigs(rapidjson::kArrayType);
			for(uint32_t i=0;i<Apply_Butler_Candidate_Signatures.size()/2;i++)
			{
				std::string pubkey=Apply_Butler_Candidate_Signatures[i].pubkey;
				std::string sig=Apply_Butler_Candidate_Signatures[i].sig;
				rapidjson::Document &doc=*(new rapidjson::Document);
				doc.SetObject();
				rapidjson::Document::AllocatorType &a=doc.GetAllocator();
				rapidjson::Value _pubkey(pubkey.c_str(),pubkey.size(),a);
				rapidjson::Value _sig(sig.c_str(),sig.size(),a);
				doc.AddMember("pubkey",_pubkey,allocator);
				doc.AddMember("sig",_sig,allocator);
				sigs.PushBack(doc,allocator);
			}
			data.AddMember("sigs",sigs,allocator);
			for(uint32_t i=0;i<account_manager.getButlerAmount();i++)
			{
				account butler=account_manager.getButler(i);
				NodeID id=butler.getNodeId();
				if(id==0)
				{
					std::string pubkey=butler.getPubKey();
					char* ch=const_cast<char*>(pubkey.c_str());
					uint32_t *map_id=(uint32_t*)ch;
					rapidjson::Document& msg=msg_manager.make_Request_NodeID_By_Pubkey(pubkey);
					sendMessage(msg,true,10,Recall_RequestNodeID,*map_id,&POV::handleResponseNodeID);
					delete &msg;
				}
				else
				{
					rapidjson::Document& copy_data=*(new rapidjson::Document);
					copy_data.CopyFrom(data,copy_data.GetAllocator());
					rapidjson::Document& msg=msg_manager.make_Request_Apply_Butler_Candidate(id,copy_data);
					//print_document(msg);
					sendMessage(msg,true,20,Recall_RequestApplyButlerCandidate,id,&POV::handleResponseApplyButlerCandidate);
					delete &msg;
				}
			}
			delete &data;
			break;
		}
		case ApplyButlerCandidate_finish:
		{
			double r=getRandomNum();
			if(r<prob_apply_commissioner)
			{
				std::cout<<"applybutlercandidate_finish\n";
				if(!RecommendationLetter.IsNull())
				{
					RecommendationLetter.Clear();
					RecommendationLetter.SetNull();
				}
				Apply_Butler_Candidate_Signatures.clear();
				normal_apply_butler_candidate_state=ApplyButlerCandidate_apply_recommendation_letter;
			}

			break;
		}
	}/*switch(normal_apply_butler_candidate_state)*/
}

void POV::Normal_QuitCommissioner()
{
	//std::cout<<"在退出委员交易中！\n";
	switch(normal_quit_commissioner_state)
	{
		case QuitCommissioner_apply:
		{
			//std::cout<<"Normal_QuitButlerCandidate 1\n";
			if(Quit_Commissioner_Metadata.IsNull())
			{
				//std::cout<<"Normal_QuitButlerCandidate 11\n";
				Quit_Commissioner_Metadata.SetObject();
				//std::cout<<"Normal_QuitButlerCandidate 12\n";
				rapidjson::Document& metadata=msg_manager.get_Quit_Commissioner_Metadata();
				//std::cout<<"Normal_QuitButlerCandidate 13\n";
				Quit_Commissioner_Metadata.CopyFrom(metadata,Quit_Commissioner_Metadata.GetAllocator());
				//std::cout<<"Normal_QuitButlerCandidate 14\n";
				delete &metadata;
			}
			//std::cout<<"Normal_QuitButlerCandidate 2\n";
			//rapidjson::Document& metadata=*(new rapidjson::Document());
			//metadata.CopyFrom(Quit_Butler_Candidate_Metadata,metadata.GetAllocator());
			//发送给所有管家
			for(uint32_t i=0;i<account_manager.getButlerAmount();i++)
			{
				std::string butler_pubkey=account_manager.getButlerPubkeyByNum(i);
				NodeID butler_id=account_manager.get_Butler_ID(butler_pubkey);
				if(butler_id==0)
				{
					//std::cout<<"在退出委员交易中——请求NodeID！\n";
					//std::cout<<"发送NodeID请求!\n";
					char* ch=const_cast<char*>(butler_pubkey.c_str());
					uint32_t *map_id=(uint32_t*)ch;
					rapidjson::Document& msg=msg_manager.make_Request_NodeID_By_Pubkey(butler_pubkey);
					sendMessage(msg,true,10,Recall_RequestNodeID,*map_id,&POV::handleResponseNodeID);
					delete &msg;
				}
				else
				{
					//std::cout<<"发送退出管家后选交易"<<1<<"\n";
					//rapidjson::Document::AllocatorType &allocator = Apply_Commissioner_Transaction.GetAllocator();
					/*
					rapidjson::Document &temp_doc=*(new rapidjson::Document());
					rapidjson::Document::AllocatorType &allocator =temp_doc.GetAllocator();
					temp_doc.CopyFrom(Quit_Butler_Candidate_Metadata,allocator);
					*/
					rapidjson::Document& msg=msg_manager.make_Request_Quit_Commissioner(butler_id,Quit_Commissioner_Metadata);
					sendMessage(msg,true,10,Recall_RequestQuitCommissioner,butler_id,&POV::handleResponseQuitCommissioner);
					//std::cout<<"发送退出委员请求\n";
					delete &msg;
					//std::cout<<"发送退出管家候选交易"<<2<<"\n";
				}
				//std::cout<<"Normal_QuitButlerCandidate 3\n";
			}
			break;
		}
		case QuitCommissioner_finish:
		{
			double r=getRandomNum();
			if(r<prob_quit_commissioner)
			{
				normal_quit_commissioner_state=QuitCommissioner_apply;
				if(Quit_Commissioner_Metadata.IsObject())
				{
					Quit_Commissioner_Metadata.Clear();
					Quit_Commissioner_Metadata.SetNull();
				}
			}
			break;
		}

	}
}

void POV::Normal_QuitButlerCandidate()
{
	switch(normal_quit_butler_candidate_state)
	{
		case QuitButlerCandidate_apply:
		{
			//std::cout<<"Normal_QuitButlerCandidate 1\n";
			if(Quit_Butler_Candidate_Metadata.IsNull())
			{
				//std::cout<<"Normal_QuitButlerCandidate 11\n";
				Quit_Butler_Candidate_Metadata.SetObject();
				//std::cout<<"Normal_QuitButlerCandidate 12\n";
				rapidjson::Document& metadata=msg_manager.get_Quit_Butler_Candidate_Metadata();
				//std::cout<<"Normal_QuitButlerCandidate 13\n";
				Quit_Butler_Candidate_Metadata.CopyFrom(metadata,Quit_Butler_Candidate_Metadata.GetAllocator());
				//std::cout<<"Normal_QuitButlerCandidate 14\n";
				delete &metadata;
			}
			//std::cout<<"Normal_QuitButlerCandidate 2\n";
			//rapidjson::Document& metadata=*(new rapidjson::Document());
			//metadata.CopyFrom(Quit_Butler_Candidate_Metadata,metadata.GetAllocator());
			//发送给所有管家
			for(uint32_t i=0;i<account_manager.getButlerAmount();i++)
			{
				std::string butler_pubkey=account_manager.getButlerPubkeyByNum(i);
				NodeID butler_id=account_manager.get_Butler_ID(butler_pubkey);
				if(butler_id==0)
				{
					//std::cout<<"发送NodeID请求!\n";
					char* ch=const_cast<char*>(butler_pubkey.c_str());
					uint32_t *map_id=(uint32_t*)ch;
					rapidjson::Document& msg=msg_manager.make_Request_NodeID_By_Pubkey(butler_pubkey);
					sendMessage(msg,true,10,Recall_RequestNodeID,*map_id,&POV::handleResponseNodeID);
					delete &msg;
				}
				else
				{
					//std::cout<<"发送退出管家后选交易"<<1<<"\n";
					//rapidjson::Document::AllocatorType &allocator = Apply_Commissioner_Transaction.GetAllocator();
					/*
					rapidjson::Document &temp_doc=*(new rapidjson::Document());
					rapidjson::Document::AllocatorType &allocator =temp_doc.GetAllocator();
					temp_doc.CopyFrom(Quit_Butler_Candidate_Metadata,allocator);
					*/
					rapidjson::Document& msg=msg_manager.make_Request_Quit_Butler_Candidate(butler_id,Quit_Butler_Candidate_Metadata);
					sendMessage(msg,true,10,Recall_RequestQuitButlerCandidate,butler_id,&POV::handleResponseQuitButlerCandidate);
					delete &msg;
					//std::cout<<"发送退出管家候选交易"<<2<<"\n";
				}
				//std::cout<<"Normal_QuitButlerCandidate 3\n";
			}
			break;
		}
		case QuitButlerCandidate_finish:
		{
			double r=getRandomNum();
			if(r<prob_quit_butler_candidate)
			{
				normal_quit_butler_candidate_state=QuitButlerCandidate_apply;
				if(Quit_Butler_Candidate_Metadata.IsObject())
				{
					Quit_Butler_Candidate_Metadata.Clear();
					Quit_Butler_Candidate_Metadata.SetNull();
				}
			}

			break;
		}
	}
}

void POV::handleMessage(rapidjson::Document &d)
{
	//处理接收到的消息PoV协议层面的消息在这里进行处理
	//std::cout<<"in pov->handleMessage\n";
	rapidjson::Document::AllocatorType &allocator=d.GetAllocator();
	uint32_t index=d["index"].GetUint();
	//std::cout<<"in pov->handleMessage 1\n";
	NodeID sender=d["sender"].GetUint64();
	//std::cout<<"in pov->handleMessage 2\n";
	NodeID receiver=d["receiver"].GetUint64();
	//std::cout<<"in pov->handleMessage 3\n";
	std::string pubkey(d["pubkey"].GetString(),d["pubkey"].GetStringLength());
	MessageType type=MessageType(d["type"].GetUint());
	rapidjson::Value &data=d["data"];
	uint32_t respond_index=d["respond_to"].GetUint();
	//std::cout<<"in pov->handleMessage 4\n";
	std::string sig(d["signature"].GetString(),d["signature"].GetStringLength());
	callback_type callbacktype=callback_type(d["callback_type"].GetUint());
	//std::cout<<"in pov->handleMessage 5\n";
	NodeID callback_childtype=d["child_type"].GetUint64();
	//std::cout<<"in pov->handleMessage 6\n";
	//如果是对某个请求的回复，则从CallBackList查找该请求并调用相应的回调函数进行处理
	if(respond_index!=0)
	{
		//std::cout<<"CallBackList在handleMessage时的size为"<<CallBackList.size()<<"\n";
		for(std::vector<CallBackInstance>::iterator i=CallBackList.begin();i!=CallBackList.end();i++)
		{
			//std::cout<<"i->index="<<i->index <<"&& i->type="<<int(i->type)<<" && i->childtype="<<i->childtype<<"\n";
			if(i->index==respond_index and i->type==callbacktype and i->childtype==callback_childtype)
			{
				//std::cout<<"respond_index="<<respond_index <<"&& callbacktype"<<int(callbacktype)<<" && callback_childtype="<<callback_childtype<<"\n";
				//这里的顺序不可更改，否则会出错
				CallBackInstance cbi=*i;
				CallBackList.erase(i);
				(cbi.caller)(d);
				//std::cout<<"callbackList delete:1\n";
				//std::cout<<"CallBackList在handleMessage的erase后的size为"<<CallBackList.size()<<"\n";
				//std::cout<<"callbackList delete:2\n";
				return;
			}
		}
	}
	//如果不是某个请求的回复，则根据消息的类型进行相应的处理
	switch(type)
	{
		case RequestCommissionerPubkey:
		{
			rapidjson::Document &response_pubkey_request=msg_manager.make_Response_Commissioner_PublicKey(sender,index);
			sendMessage(response_pubkey_request,false,30,callbacktype,callback_childtype,NULL);
			delete &response_pubkey_request;
			break;
		}
		case RequestApplyCommissionerSignature:
		{
			std::cout<<"处理申请委员签名请求\n";
			//std::cout<<"收到委员公钥！:"<<pubkey<<"\n";
			//rapidjson::StringBuffer buffer;
			//rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
			//d.Accept(writer);
			//std::cout<<"response文档信息："<<buffer.GetString()<<"\n";
			//if(account_manager.is_commissioner(pubkey))
			rapidjson::Document &response=msg_manager.make_Response_Apply_Commissioner_Signature(sender,index,data);

			/*
			rapidjson::Value &_data=response["data"];
			std::string pk=_data["pubkey"].GetString();
			std::string s=_data["sig"].GetString();
			bool result=key_manager.verifyDocument(data,pk,s);
			std::cout<<"验证签名结果："<<result<<"\n";
			*/
			sendMessage(response,false,10,Recall_RequestApplyCommissionerSignature,callback_childtype,NULL);
			delete &response;
			break;
		}
		case RequestApplyCommissioner:
		{
			std::cout<<"处理申请委员请求1\n";

			if(validateApplyCommissionerTransaction(data))
			{
				bool existed=false;
				std::string pubkey=data["metadata"]["pubkey"].GetString();
				for(std::vector<DocumentContainer>::iterator i=ApplyCommissionerPool.begin();i!=ApplyCommissionerPool.end();i++)
				{
					rapidjson::Document& doc=i->getDocument();
					if(doc["metadata"]["pubkey"].GetString()==pubkey)
					{
						existed=true;
						break;
					}
					delete &doc;
				}
				if(!existed)
				{
					//在生成创世区块阶段，只接受委员的申请，其他阶段则只接受非委员的申请。
					if(state==generate_first_block)
					{
						if(account_manager.is_commissioner(pubkey))
						{
							DocumentContainer container;
							container.saveDocument(data);
							ApplyCommissionerPool.push_back(container);
						}
					}
					else
					{
						if(!account_manager.is_commissioner(pubkey))
						{
							DocumentContainer container;
							container.saveDocument(data);
							ApplyCommissionerPool.push_back(container);
						}
					}
				}
				std::cout<<"处理申请委员请求2\n";

				//std::cout<<"申请委员消息：\n";
				//print_document(d);
				//简单起见，对于申请成为委员，默认同意
				rapidjson::Document &response=msg_manager.make_Response_Apply_Commissioner(sender,index);
				sendMessage(response,false,10,Recall_RequestApplyCommissioner,callback_childtype,NULL);
				delete &response;
				std::cout<<"处理申请委员请求3\n";

			}
			else
				std::cout<<"申请委员交易验证不通过!\n";
			break;
		}
		case RequestRecommendationLetter:
		{
			std::cout<<"接收到推荐信请求\n";
			rapidjson::Document &msg=msg_manager.make_Response_Recommend_Letter(sender,index,pubkey);
			sendMessage(msg,false,10,Recall_RequestApplyRecommendationLetter,callback_childtype,NULL);
			delete &msg;
			break;
		}
		case RequestApplyButlerCandidateSignature:
		{
			std::cout<<"接收到申请管家候选签名请求\n";
			if(!validateRecommendationLetter(data))
			{
				std::cout<<"推荐信验证不通过\n";
				break;
			}
			rapidjson::Document &response=msg_manager.make_Response_Apply_Butler_Candidate_Signature(sender,index,data);
			sendMessage(response,false,10,Recall_RequestApplyButlerCandidateSignature,callback_childtype,NULL);
			delete &response;
			break;
		}
		case RequestApplyButlerCandidate:
		{
			std::cout<<"接收到申请管家候选请求\n";
			int num=13;
			if(sender==num)
			{
				//std::cout<<network->getNodeId()<<"节点接收到来自"<<sender<<"RequestApplyButlerCandidate请求：start!\n";
			}
			//rapidjson::Document& msg=
			if(!validateApplyButlerCandidateData(data))
			{
				std::cout<<"验证申请管家候选不通过！\n";
				break;
			}
			//判断是否已经有这个申请了，如果没有就加入到ApplyButlerCandidatePool中。
			bool existed=false;
			std::string new_refferal=data["recommendation_letter"]["metadata"]["refferal"].GetString();
			for(std::vector<DocumentContainer>::iterator i=ApplyButlerCandidatePool.begin();i!=ApplyButlerCandidatePool.end();i++)
			{
				rapidjson::Document& doc=i->getDocument();
				//rapidjson::Value& letter=doc["recommendation_letter"];
				std::string refferal=doc["recommendation_letter"]["metadata"]["refferal"].GetString();

				delete &doc;
				if(refferal==new_refferal)
				{
					existed=true;
					break;
				}
			}
			if(!existed)
			{
				rapidjson::Document temp;
				rapidjson::Document::AllocatorType &allocator=temp.GetAllocator();
				temp.CopyFrom(data,allocator);
				DocumentContainer container;
				container.saveDocument(temp);
				ApplyButlerCandidatePool.push_back(container);
				std::string pubkey=data["recommendation_letter"]["metadata"]["refferal"].GetString();
				if(state==generate_first_block)
				account_manager.pushback_butler_candidate(pubkey,sender);//这里直接把发送方的ID作为申请人的ID会不会有安全问题？这是发送节点ID可篡改带来的问题，可以在底层再加一层签名解决，这里不实现。
			}
			//发送回复表示已经收到
			rapidjson::Document &response=msg_manager.make_Response_Apply_Butler_Candidate(sender,index);
			sendMessage(response,false,10,Recall_RequestApplyButlerCandidate,callback_childtype,NULL);
			if(sender==num)
			{
				//std::cout<<network->getNodeId()<<"节点接收到来自"<<sender<<"RequestApplyButlerCandidate请求：end!\n";
			}
			delete &response;
			break;
		}
		case RequestVote:
		{
			//std::cout<<"接收到Vote请求：start!\n";
			//验证投票正确性
			if(!validateBallot(data))
			{
				std::cout<<"验证投票错误！\n";
				break;
			}
			//检查投票池中是否已经有该委员的投票
			bool existed=false;
			std::string new_voter=data["metadata"]["voter"].GetString();
			for(std::vector<DocumentContainer>::iterator i=BallotPool.begin();i!=BallotPool.end();i++)
			{
				rapidjson::Document& doc=i->getDocument();
				//rapidjson::Value& letter=doc["recommendation_letter"];
				std::string voter=doc["metadata"]["voter"].GetString();
				delete &doc;
				if(voter==new_voter)
				{
					existed=true;
					break;
				}
			}
			if(!existed)
			{
				//rapidjson::Document& Ballot=*(new rapidjson::Document);
				//Ballot.CopyFrom(data,Ballot.GetAllocator());
				DocumentContainer container;
				container.saveDocument(data);
				BallotPool.push_back(container);
			}
			//发送回复表示已经收到
			rapidjson::Document &response=msg_manager.make_Response_Vote(sender,index);
			sendMessage(response,false,10,Recall_RequestVote,callback_childtype,NULL);
			delete &response;
			break;
		}

		case RequestBlockSignature:
		{
			//std::cout<<network->getNodeId()<<"节点接收到"<<network->getNodeId()<<"节点的区块签名请求：start!\n";
			//std::cout<<"收到区块签名请求！\n";
			rapidjson::Value* block_pointer;
			if(state==generate_first_block)
			{
				block_pointer=&data;
			}
			else
			{

				block_pointer=&(data["block"]);
				std::cout<<"对接收的预区块群签名进行验证!\n";
				std::string group_sig=data["group_sig"].GetString();
				std::cout<<"群签名="<<group_sig<<"\n";
				if(key_manager.group_verifyDocument(*block_pointer,group_sig,algorithm_method,gpk,pbc_param))
				{
					std::cout<<"\n区块群签名验证通过!\n";
				}
				else
				{
					std::cout<<"\n区块群签名验证失败！\n";
					print_document(*block_pointer);

					std::cout<<"algorithm_method="<<algorithm_method<<"\n";
					std::cout<<"gpk="<<gpk<<"\n";
					std::cout<<"pbc_param="<<pbc_param<<"\n";
				}

			}
			rapidjson::Value& doc_block=*block_pointer;
			if(!validateRawBlock(doc_block))
			{
				std::cout<<"区块验证不通过\n";
				break;
			}
			else
			{
				std::cout<<"区块验证通过\n";
			}
			PoVBlock block;
			block.setRawBlock(doc_block);
			uint32_t height=block.getHeight();
			std::cout<<network->getNodeId()<<"节点收到"<<sender<<"节点发送来的高度为"<<height<<"的生区块\n";
			//验证高度
			if(height!=blockchain.getHeight()+1)
			{
				std::cout<<"请求区块签名中验证区块头错误——高度错误！\n";
				//std::cout<<"height:"<<height<<"\n";
				//std::cout<<"my height:"<<blockchain.getHeight()<<"\n";
				//print_document(data);
				break;
			}
			//blockchain.pushbackBlock(block);
			rapidjson::Value& header=doc_block["header"];
			//NodeInfo node=network->NodeList[sender];
			rapidjson::Document &response=msg_manager.make_Response_Block_Signature(sender,index,header);
			//print_document(response);
			sendMessage(response,false,10,Recall_RequestBlockSignature,callback_childtype,NULL);
			delete &response;
			//std::cout<<"接收到区块签名请求：end!\n";
			break;
		}
		case PublishBlock:
		{
			std::cout<<network->getNodeId()<<"节点接收到新发布的区块！\n";
			if(!validateBlock(data))
			{
				break;
			}
			PoVBlock block;
			block.setBlock(data);
			uint32_t height=block.getHeight();
			//验证高度
			if(height!=blockchain.getHeight()+1)
			{
				std::cout<<"publish block 中验证区块头错误——高度错误！\n";
				std::cout<<"接收到的区块高度:"<<height<<"\n";
				std::cout<<"本地区块链高度:"<<blockchain.getHeight()<<"\n";
				//print_document(data);
				break;
			}
			//std::cout<<"发布区块:1\n";
			updateVariables(data);
			//std::cout<<"发布区块:2\n";
			//blockchain.pushbackBlock(block);
			blockchain.pushbackBlockToDatabase(block);
			std::cout<<network->getNodeId()<<"把高度为"<<height<<"的区块加入到区块链中!\n";
			//print_document(data);
			if(height==0)
			{
				state=normal;
			}
			if(height==end_height+1)
			{
				print_document(data);
				statisticData();
				exit(0);
			}
			break;
		}
		case RequestNodeID:
		{
			std::string receiver_pubkey=data["pubkey"].GetString();
			if(receiver_pubkey==account_manager.getMyPubkey())
			{
				//std::cout<<"in pubkey equal!\n";
				rapidjson::Document &response=msg_manager.make_Response_NodeID_By_Pubkey(sender,index);
				sendMessage(response,false,10,Recall_RequestNodeID,callback_childtype,NULL);
				delete &response;
			}
			break;
		}
		case RequestNormal:
		{
			/*查找有是否已经接受过该交易，是的话抛弃，为了减低算法复杂度，该部分不使用
			for(std::vector<DocumentContainer>::iterator i=NormalPool.begin();i!=NormalPool.end();i++)
			{
				rapidjson::Document& tx=i->getDocument();
				if(data["content"].GetString()==tx["content"].GetString() and data["timestamp"].GetString()==tx["timestamp"].GetString())
				{
					break;
				}
			}
			*/
			//std::cout<<"接收到正常交易请求：start!\n";
			//std::cout<<"正常交易缓存数量："<<NormalPool.size()<<"\n";
			if(account_manager.is_butler())
			{
				//防止接收相同的交易
				rapidjson::Value& content=data["content"];
				if(content.IsObject() && content.HasMember("log"))
				{
					int hash=content["log"].GetInt();
					for(uint32_t i=0;i<NormalPool.size();i++)
					{
						rapidjson::Document& tx=NormalPool.at(i).getDocument();
						rapidjson::Value& _content=tx["content"];
						 if(_content.IsObject() && _content.HasMember("log") && _content["log"].GetInt()==hash)
						 {
							 delete &tx;
							 return;
						 }
					}
				}
				DocumentContainer container;
				container.saveDocument(data);
				if(NormalPool.size()>=NormalPoolCapacity)
				{
					NormalPool.erase(NormalPool.begin());
				}
				NormalPool.push_back(container);
				//if(account_manager.getMyNodeID()==2)
				//{
					//print_document(data);
				//}
			}

			//rapidjson::Document &response=msg_manager.make_Response_Normal(sender,index);
			//sendMessage(response,false,10,Recall_RequestNormal,callback_childtype,NULL);
			//delete &response;
			//std::cout<<"接收到正常交易请求：end!\n";
			break;
		}
		case RequestQuitButlerCandidate:
		{
			//std::cout<<"收到退出管家候选请求！\n";
			//print_document(data);
			if(!validateQuitButlerCandidateTransaction(data))
			{
				std::cout<<"退出管家候选请求验证失败！";
			}
			std::string applicant=data["metadata"]["pubkey"].GetString();
			if(account_manager.is_butler(applicant))
			{
				std::cout<<"申请退出管家候选的申请者是管家!\n";
				break;
			}
			//判断该交易是否已经存在
			bool exist=false;
			for(auto i=QuitButlerCandidatePool.begin();i!=QuitButlerCandidatePool.end();i++)
			{
				rapidjson::Document &tx=i->getDocument();
				std::string _app=tx["metadata"]["pubkey"].GetString();
				if(_app==applicant)
				{
					delete &tx;
					exist=true;
					break;
				}
				delete &tx;
			}
			if(exist)
			{
				break;
			}
			DocumentContainer container;
			container.saveDocument(data);
			QuitButlerCandidatePool.push_back(container);
			rapidjson::Document &response=msg_manager.make_Response_Quit_Butler_Candidate(sender,index);
			sendMessage(response,false,10,Recall_RequestQuitButlerCandidate,callback_childtype,NULL);
			delete &response;
			break;
		}
		case RequestQuitCommissioner:
		{
			std::cout<<"收到退出委员请求！\n";
			if(!validateQuitCommissionerTransaction(data))
			{
				std::cout<<"退出委员请求验证失败！";
			}
			std::string applicant=data["metadata"]["pubkey"].GetString();
			//判断该交易是否已经存在
			bool exist=false;
			for(auto i=QuitCommissionerPool.begin();i!=QuitCommissionerPool.end();i++)
			{
				rapidjson::Document &tx=i->getDocument();
				std::string _app=tx["metadata"]["pubkey"].GetString();
				if(_app==applicant)
				{
					delete &tx;
					exist=true;
					break;
				}
				delete &tx;
			}
			if(exist)
			{
				break;
			}
			std::cout<<"把退出委员请求存放进交易池中！\n";
			DocumentContainer container;
			container.saveDocument(data);
			QuitCommissionerPool.push_back(container);
			rapidjson::Document &response=msg_manager.make_Response_Quit_Commissioner(sender,index);
			sendMessage(response,false,10,Recall_RequestQuitCommissioner,callback_childtype,NULL);
			delete &response;
			break;
		}
		case RequestHeight:
		{
			rapidjson::Document &response=msg_manager.make_Response_Height(sender,blockchain.getHeight());
			sendMessage(response,false,10,Recall_RequestNodeID,callback_childtype,NULL);
			delete &response;
			break;
		}
		case ResponseHeight:
		{
			int height=data["height"].GetInt();
			std::cout<<receiver<<"收到高度为"<<height<<"的高度请求回复\n";
			if(sync_block_method==SequenceSync)
			{
				if(height>blockchain.getHeight())
				{
					block_syncer s;
					s.id=sender;
					s.height=height;
					syncers.push(s);
				}
			}
			else
			{
				int current_height=blockchain.getHeight();
				if(height>current_height)
				{
					int start_height=current_height+1;
					if((!con_syncers.empty()) && con_syncers[con_syncers.size()-1].height<height)
					{
						start_height=con_syncers[con_syncers.size()-1].height+1;
					}
					for(int h=start_height;h<=height;h++)
					{
						concurrent_block_syncer s;
						s.height=h;
						s.prepared=false;
						con_syncers.push_back(s);
					}
				}
			}

			break;
		}
		case RequestBlock:
		{
			int height=data["height"].GetInt();
			std::cout<<receiver<<"收到高度为"<<height<<"的区块请求\n";
			if(blockchain.getHeight()<height)
				break;
			std::cout<<account_manager.getMyNodeID()<<"返回高度为"<<height<<"的区块, childtype为"<<callback_childtype<<"\n";
			PoVBlock block=blockchain.getBlock(height);
			rapidjson::Document& doc=block.getBlock();
			rapidjson::Document &response=msg_manager.make_Response_Block(sender,index,doc);
			sendMessage(response,false,10,Recall_RequestBlock,callback_childtype,NULL);
			//print_document(response);
			delete &doc;
			delete &response;
			break;
		}
		case RequestGroupKey:
		{
			std::string pubkey= data["pubkey"].GetString();
			if(group_name=="#")
				Initial_Group();
			std::string gsk_info;
			auto it=gsk_info_list.find(pubkey);
			//std::cout<<"group sig test:1\n";
			if(it!=gsk_info_list.end())
			{
				//std::cout<<"group sig test:2\n";
				gsk_info=it->second;
			}
			else
			{
				//std::cout<<"group sig test:3\n";
				std::cout<<"群主"<<group_name<<"添加新的群成员\n";
				group_member_join(gsk_info,algorithm_method,
							pbc_param,gmsk,gpk,gamma);
				/*
				std::cout<<"algorithm_method="<<algorithm_method<<"\n";
				std::cout<<"pbc_param="<<pbc_param<<"\n";
				std::cout<<"gmsk="<<gmsk<<"\n";
				std::cout<<"gpk="<<gpk<<"\n";
				std::cout<<"gamma="<<gamma<<"\n";
				std::cout<<"group sig test:4\n";
				*/
				gsk_info_list[pubkey]=gsk_info;
			}
			//std::cout<<"group sig test:5\n";
			rapidjson::Document &response=msg_manager.make_Response_Group_Key(sender,index,gsk_info,algorithm_method,pbc_param,gpk);
			sendMessage(response,false,10,Recall_RequestGroupKey,callback_childtype,NULL);
			//std::cout<<"发送请求群密钥的回复\n";
			break;
		}
		/*
		case ResponseBlock:
		{
			print_document(data);
			int height=data["response_height"].GetInt();
			std::cout<<"在handleMessage中接受到ResponseBlock的消息，高度为"<<height<<"\n";
			break;
		}
		*/
		default:
			break;
	}
}

rapidjson::Document& POV::BallotStatistic(std::vector<DocumentContainer> &ballots)
{
	//对投票进行统计
	rapidjson::Document& result=*(new rapidjson::Document);
	//result.SetObject();
	result.SetArray();
	rapidjson::Document::AllocatorType& allocator=result.GetAllocator();
	/*
	rapidjson::Value& result_metatype=*(new rapidjson::Value("result",allocator));
	result.AddMember("metatype",result_metatype,allocator);

	rapidjson::Value& ballot_result=*(new rapidjson::Value(rapidjson::kArrayType));
	*/
	std::map<std::string,uint32_t> rank_list;
	//统计每个账号的得票数，结果用一个map数据结构存储
	for(std::vector<DocumentContainer>::iterator i=ballots.begin();i!=ballots.end();i++)
	{
		rapidjson::Document& ballot=i->getDocument();
		rapidjson::Value& metadata=ballot["metadata"];
		//std::string sig=ballot["sig"].GetString();
		//std::string pubkey=metadata["voter"].GetString();
		rapidjson::Value& vote_list=metadata["vote_list"];
		for(uint32_t j=0;j<vote_list.Size();j++)
		{
			std::string pubkey=vote_list[j].GetString();
			if(rank_list.find(pubkey)==rank_list.end())
			{
				rank_list[pubkey]=1;
			}
			else
			{
				rank_list[pubkey]=rank_list[pubkey]+1;
			}
		}
	}
	//取出最高票数的账号作为投票结果生成选举交易
	for(uint32_t i=0;i<vote_amount;i++)
	{
		uint32_t max=0;
		std::string max_pubkey;
		for(std::map<std::string,uint32_t>::iterator j=rank_list.begin();j!=rank_list.end();j++)
		{
			if(j->second>max)
			{
				max=j->second;
				max_pubkey=j->first;
			}
		}
		rapidjson::Value& new_butler=*(new rapidjson::Value(rapidjson::kObjectType));
		new_butler.SetObject();
		new_butler.SetString(max_pubkey.c_str(),max_pubkey.size(),allocator);
		result.PushBack(new_butler,allocator);
		rank_list[max_pubkey]=0;
	}
	//result.AddMember("metadata",ballot_result,allocator);
	return result;
}

uint32_t POV::getNextButler(rapidjson::Value& sigs)
{
	//根据委员的签名计算下一任管家编号
	//std::cout<<"debug: 3\n";
	uint32_t num=sigs.Size();
	uint32_t butler_nums[num];
	std::string max_sig="";
	double max_time=0;
	//print_document(sigs);
	for(uint32_t i=0;i<num;i++)
	{
		double time=sigs[i]["timestamp"].GetDouble();
		std::string sig=sigs[i]["sig"].GetString();
		if(time>max_time)
		{
			max_time=time;
			max_sig=sig;
		}
	}
	//std::cout<<"debug: 4\n";
	//std::string hash=key_manager.getHash256(max_sig);
	unsigned char hex_sig[100];
	size_t len;
	key_manager.Str2Hex(max_sig,hex_sig,&len);

	//std::cout<<"debug: 5\n";
	unsigned char *timestamp=(unsigned char *)(&max_time);
	size_t double_len=sizeof(double);
	char *p;
	size_t p_len;
	//std::cout<<"debug: 6\n";
	if(len>double_len)
	{
		p_len=len;
		p=new char[len];
		for(size_t i=0;i<len;i++)
		{
			if(i<double_len)
			{
				p[i]=timestamp[i]^hex_sig[i];
			}
			else
				p[i]=hex_sig[i];
		}
	}
	else
	{
		p_len=double_len;
		p=new char[double_len];
		for(size_t i=0;i<double_len;i++)
		{
			if(i<len)
			{
				p[i]=timestamp[i]^hex_sig[i];
			}
			else
				p[i]=hex_sig[i];
		}
	}
	//std::cout<<"debug: 7\n";
	uint32_t* butler_num=(uint32_t*)&(p[p_len-4]);
	//std::cout<<"debug: 8\n";
	uint32_t ret_num=(*butler_num)%butler_amount;
	delete[] p;
	return ret_num;
}

uint32_t POV::getTe(rapidjson::Value& sigs)
{
	//把签名中最后时间戳最大值作为区块的时间戳
	uint32_t num=sigs.Size();
	double max_time=0;
	for(uint32_t i=0;i<num;i++)
	{
		double time=sigs[i]["timestamp"].GetDouble();
		if(time>max_time)
		{
			max_time=time;
		}
	}
	return max_time;
}

bool POV::validateBlock(rapidjson::Value &data)
{
	//验证区块是否合法
	if(!validateRawBlock(data))
	{
		std::cout<<"区块数据验证不通过\n";
		return false;
	}
	else
	{
		std::cout<<"区块数据验证通过\n";
	}
	PoVBlock block;
	block.setBlock(data);
	uint32_t height=block.getHeight();

	//验证区块头签名
	rapidjson::Document& sigs=block.getSignatures();
	rapidjson::Document& raw_header=block.getRawHeader();
	if(sigs.Size()<=account_manager.getCommissionerAmount()/2.0)
	{
		std::cout<<"验证区块头错误——区块签名没有超过委员数量的一半！\n";
		return false;
	}
	else
	{
		std::cout<<"验证区块头通过，环签名数量超过委员数量的一半！";
	}
	for(uint32_t i=0;i<sigs.Size();i++)
	{
		rapidjson::Value& sig=sigs[i];
		if(!validateBlockSignature(raw_header,sig))
		{
			std::cout<<"验证区块头错误——签名错误!\n";
			return false;
		}
	}
	//验证下一任管家
	uint32_t next_butler=getNextButler(sigs);
	if(block.getNextButler()!=next_butler)
	{
		std::cout<<"验证区块头错误——下一任管家错误!\n";
		return false;
	}
	//验证区块截止时间
	double Te=getTe(sigs);
	delete &sigs;
	delete &raw_header;
	if(block.getTe()!=Te)
	{
		std::cout<<"验证区块头错误——Te错误!\n";
		return false;
	}
	return true;
}

bool POV::validateBlockSignature(rapidjson::Document& header,rapidjson::Value &data)
{
	//std::cout<<"header info:\n";
	//print_document(header);
	//std::cout<<"data info:\n";
	//print_document(data);
	std::string pubkey=data["pubkey"].GetString();
	std::string sig=data["sig"].GetString();
	double timestamp=data["timestamp"].GetDouble();
	header["Te"].SetDouble(timestamp);

	if(data.HasMember("ring_sig"))
	{
		std::cout<<"对环签名进行验证\n";
		std::string ring_sig=data["ring_sig"].GetString();
		std::cout<<"环签名="<<ring_sig<<"\n";
		if(!key_manager.ring_verifyDocument(header,ring_sig))
		{
			std::cout<<network->getNodeId()<<"验证环签名不通过！\n";
			std::cout<<"ring_sig:"<<ring_sig<<"\n";
			return false;
		}
		else
		{
			std::cout<<"环签名验证通过\n";
		}
	}

	if(!key_manager.verifyDocument(header,pubkey,sig))
	{
		std::cout<<network->getNodeId()<<"验证签名不通过！\n";
		std::cout<<"pubkey:"<<pubkey<<"\n";
		std::cout<<"sig:"<<sig<<"\n";
		return false;
	}
	//std::cout<<"received pubkey:"<<pubkey<<"\n";
	//std::string sig=data["sig"].GetString();
	//判断是否是委员
	if(!account_manager.is_commissioner(pubkey))
	{
		std::cout<<" not commissioner\n";
		return false;
	}
	return true;
}

bool POV::validateRawBlock(rapidjson::Value &data)
{
	//print_document(data);
	rapidjson::Value& header=data["header"];
	rapidjson::Value& transactions=data["transactions"];
	if(!transactions.IsArray())
	{
		std::cout<<"验证交易错误——transactions不是array！\n";
		print_document(transactions);
	}
	//验证交易是否正确
	for(uint32_t i=0;i<transactions.Size();i++)
	{
		rapidjson::Value& tx=transactions[i];
		std::string metatype=tx["metatype"].GetString();
		if(metatype=="Constants")
		{

		}
		else if(metatype=="ApplyCommissioner")
		{
			if(!validateApplyCommissionerTransaction(tx))
			{
				std::cout<<"验证交易错误——申请委员交易错误！\n";
				return false;
			}
		}
		else if(metatype=="vote")
		{
			if(!validateVoteTransaction(tx))
			{
				std::cout<<"验证交易错误——投票交易错误！\n";
				return false;
			}
		}
		else if(metatype=="ApplyButlerCandidate")
		{
			if(!validateApplyButlerCandidateData(tx))
			{
				std::cout<<"验证交易错误——申请管家候选交易错误！\n";
				return false;
			}
		}
	}
	//验证区块头信息是否正确
	uint32_t height=header["height"].GetInt();
	uint32_t num_of_trans=header["num_of_trans"].GetInt();
	std::string generator=header["generator"].GetString();
	uint32_t cycles=header["cycles"].GetInt();
	std::string merkle_root=header["merkle_root"].GetString();
	std::string hash=header["previous_hash"].GetString();
	int myheight=blockchain.getHeight();
	if(height!=myheight+1)
	{
		std::cout<<"验证区块头错误——高度错误！\n";
		return false;
	}
	//std::cout<<"验证区块头:my height="<<myheight<<"\n";
	if(myheight>0)
	{
		PoVBlock b=blockchain.getBlock(myheight);
		std::string true_hash=key_manager.getHash256(b.getHeader());
		if(true_hash!=hash)
		{
			std::cout<<"验证区块头错误——前一个区块的hash错误！\n";
			//std::cout<<"true_hash="<<true_hash<<"\n";
			//std::cout<<"hash="<<hash<<"\n";
			return false;
		}
	}

	if(num_of_trans!=transactions.Size())
	{
		std::cout<<"验证区块头错误——交易数错误！\n";
		return false;
	}
	if(state==generate_first_block)
	{
		//NodeID AgentCommissioner=Initial_Commissioner_Nodes[agent_commissioner_num];
		if(generator!=account_manager.getCommissionerByNodeID(AgentCommissioner).getPubKey())
		{
			std::cout<<"验证区块头错误——公钥错误！\n";
			return false;
		}
		if(cycles!=0)
		{
			std::cout<<"验证区块头错误——周期数错误！\n";
			return false;
		}
	}
	else
	{

	}
	std::string root=key_manager.getHash256(transactions);
	if(merkle_root!=root)
	{
		return false;
	}
	return true;
}

bool POV::validateVoteTransaction(rapidjson::Value &data)
{
	std::string metatype=data["metatype"].GetString();
	rapidjson::Value& metadata=data["metadata"];
	rapidjson::Value& result=metadata["result"];
	rapidjson::Value& ballots=metadata["ballots"];
	if(state==generate_first_block)
	{
		if(ballots.Size()!=account_manager.getCommissionerAmount())
		{
			std::cout<<"验证投票交易错误——创世区块阶段投票数错误！\n";
		}
	}
	else
	{
		if(ballots.Size()<account_manager.getCommissionerAmount()/2)
		{
			std::cout<<"验证投票交易错误——创世区块阶段投票数错误！\n";
		}
	}
	//验证ballot是否正确
	std::vector<DocumentContainer> ballot_containers;
	for(uint32_t i=0;i<ballots.Size();i++)
	{
		rapidjson::Value& ballot=ballots[i];
		if(!validateBallot(ballot))
		{
			std::cout<<"验证投票交易错误——投票错误！\n";
			return false;
		}
		DocumentContainer container;
		container.saveDocument(ballot);
		ballot_containers.push_back(container);
	}
	rapidjson::Document& stat_result=BallotStatistic(ballot_containers);
	for(uint32_t i=0;i<result.Size();i++)
	{
		bool equal=false;
		for(uint32_t j=0;j<stat_result.Size();j++)
		{
			if(result[i].GetString()!=stat_result[i].GetString())
			{
				equal=true;
				break;
			}
		}

		if(!equal)
		{
			std::cout<<"验证投票交易错误——投票结果错误！\n";
			delete &stat_result;
			return false;
		}
	}
	delete &stat_result;
	return true;
}

bool POV::validateBallot(rapidjson::Value &data)
{
	std::string metatype=data["metatype"].GetString();
	rapidjson::Value& metadata=data["metadata"];
	std::string sig=data["sig"].GetString();
	std::string pubkey=metadata["voter"].GetString();
	rapidjson::Value& vote_list=metadata["vote_list"];
	//检查metatype是否Ballot
	if(metatype!="Ballot")
	{
		std::cout<<"验证投票——metatype错误！\n";
		return false;
	}
	//检查投票人是否委员
	if(!account_manager.is_commissioner(pubkey))
	{
		std::cout<<"验证投票——投票人不是委员错误！\n";
		return false;
	}
	//检查投票人数是否正确
	if(vote_list.Size()!=vote_amount)
	{
		std::cout<<"验证投票——投票人数不等于规定的人数错误！\n";
		return false;
	}
	//检查被投票的公钥是否管家候选
	if(state!=generate_first_block)
	{
		for(rapidjson::SizeType i=0;i<vote_list.Size();i++)
		{
			std::string voted_pubkey=vote_list[i].GetString();
			if(!account_manager.is_butler_candidate(voted_pubkey))
			{
				std::cout<<"验证投票——被投票的公钥不是管家候选错误！\n";
				return false;
			}
		}
	}

	//检查签名是否正确
	if(!key_manager.verifyDocument(metadata,pubkey,sig))
	{
		std::cout<<"验证投票——签名错误！\n";
		return false;
	}
	return true;
}

bool POV::validateApplyCommissionerTransaction(rapidjson::Value &data)
{
	//rapidjson::Value &data=d["data"];
	if(!data.IsObject())
	{
		std::cout<<"验证委员交易——data不是object\n";
		print_document(data);
	}
	rapidjson::Value &metadata=data["metadata"];
	rapidjson::Value &sigs=data["sigs"];
	//检查metatype是否正确
	std::string first_metatype=data["metatype"].GetString();
	if(first_metatype!="ApplyCommissioner")
	{
		std::cout<<"验证委员交易——第一个metatype错误!\n";
		return false;
	}
	if(!metadata.IsObject())
	{
		std::cout<<"验证委员交易——metadata不是object\n";
		print_document(metadata);
	}
	std::string metatype=metadata["metatype"].GetString();
	if(metatype!="ApplyCommissioner")
	{
		std::cout<<"验证委员交易——第二个metatype错误!\n";
		return false;
	}
	//检验是否pubkey是否委员,在生成创世区块的阶段，只有委员才能申请成为委员，在其他阶段委员不能申请成为委员
	if(state==generate_first_block)
	{
		std::string com_pubkey=metadata["pubkey"].GetString();
		if(!account_manager.is_commissioner(com_pubkey))
		{
			std::cout<<"验证委员交易——创世区块阶段不是委员错误!\n";
			return false;
		}
	}
	else
	{
		std::string com_pubkey=metadata["pubkey"].GetString();
		if(account_manager.is_commissioner(com_pubkey))
		{
			std::cout<<"验证委员交易——正常阶段已经是委员错误!\n";
			return false;
		}
	}
	//检验时间——仿真不检验
	//校验签名是否正确
	if(!sigs.IsArray())
	{
		std::cout<<"验证委员交易——签名不是数组错误!\n";
		return false;
	}
	for(rapidjson::SizeType i=0;i<sigs.Size();i++)
	{
		rapidjson::Value &sig=sigs[i];
		if(sig.IsObject() and sig.HasMember("pubkey") and sig.HasMember("sig"))
		{
			std::string pubkey=sig["pubkey"].GetString();
			std::string str_sig=sig["sig"].GetString();
			if(!account_manager.is_commissioner(pubkey))
			{
				std::cout<<"验证委员交易——签名人不是委员错误!\n";
				return false;
			}
			if(!key_manager.verifyDocument(metadata,pubkey,str_sig))
			{
				std::cout<<"验证委员交易——签名不正确错误!\n";
				return false;
			}
		}
		else
		{
			std::cout<<"验证委员交易——缺少pubkey或者sig错误\n";
			return false;
		}
	}
	return true;
}

bool POV::validateRecommendationLetter(rapidjson::Value &data)
{
	//rapidjson::Value &data=d["data"];
	rapidjson::Value &metadata=data["metadata"];
	std::string sig=data["sig"].GetString();
	std::string metatype=metadata["metatype"].GetString();
	std::string refferal=metadata["refferal"].GetString();
	std::string refferer=metadata["refferer"].GetString();
	double timestamp=metadata["timestamp"].GetDouble();
	//检查metatype是否正确
	if(metatype!="RecommendationLetter")
	{
		std::cout<<"验证推荐信错误——推荐信类型错误！\n";
		return false;
	}
	//判断refferal是否已经是管家
	if(state!=generate_first_block)
	{
		if(account_manager.is_butler_candidate(refferal))
		{
			std::cout<<"验证推荐信错误——被推荐人已经是管家候选错误！\n";
			return false;
		}
	}

	//判断refferer是否委员
	if(!account_manager.is_commissioner(refferer))
	{
		std::cout<<"验证推荐信错误——推荐人不是委员！\n";
		return false;
	}
	//判断时间是否合法，这个暂时不写
	//校验签名是否正确
	if(!key_manager.verifyDocument(metadata,refferer,sig))
	{
		std::cout<<"验证推荐信错误——签名错误！\n";
		return false;
	}
	return true;
}

bool POV::validateApplyButlerCandidateSignature(rapidjson::Value &letter,rapidjson::Value &data)
{
	//rapidjson::Value &data=d["data"];
	//print_document(data);
	std::string pubkey=data["pubkey"].GetString();
	std::string sig=data["sig"].GetString();
	if(!account_manager.is_commissioner(pubkey))
	{
		std::cout<<"验证申请管家候选的签名——不是委员！\n";
		return false;
	}
	if(!key_manager.verifyDocument(letter,pubkey,sig))
	{
		std::cout<<"验证申请管家候选的签名——签名错误！\n";
		std::cout<<"pubkey="<<pubkey<<"\n";
		std::cout<<"sig="<<sig<<"\n";
		std::cout<<"letter:\n";
		print_document(letter);
		return false;
	}
	return true;
}

bool POV::validateApplyButlerCandidateData(rapidjson::Value &data)
{
	if(!data.IsObject())
	{
		std::cout<<"验证申请管家候选交易——data不是Object错误！\n";
		print_document(data);
	}
	std::string metatype=data["metatype"].GetString();
	rapidjson::Value& letter=data["recommendation_letter"];
	rapidjson::Value& sigs=data["sigs"];
	if(metatype!="ApplyButlerCandidate")
	{
		std::cout<<"验证申请管家候选交易——消息类型错误！\n";
		return false;
	}
	if(!letter.IsObject())
	{
		std::cout<<"验证申请管家候选交易——letter不是Object错误！\n";
		print_document(letter);
		return false;
	}
	if(!validateRecommendationLetter(letter))
	{
		std::cout<<"验证申请管家候选交易——验证推荐信错误！\n";
		return false;
	}
	if(!sigs.IsArray() or sigs.Empty())
	{
		std::cout<<"验证申请管家候选交易——sigs不是数组或者为空！\n";
		return false;
	}
	for(rapidjson::SizeType i=0;i<sigs.Size();i++)
	{
		rapidjson::Value& sig=sigs[i];
		if(!validateApplyButlerCandidateSignature(letter,sig))
		{
			std::cout<<"验证申请管家候选交易——验证签名错误！\n";
			return false;
		}
	}
	return true;
}

bool POV::validateQuitButlerCandidateTransaction(rapidjson::Value &data)
{
	std::string metatype=data["metatype"].GetString();
	rapidjson::Value& metadata=data["metadata"];
	std::string sig=data["sig"].GetString();
	std::string type=metadata["type"].GetString();
	std::string pubkey=metadata["pubkey"].GetString();
	double timestamp=metadata["timestamp"].GetDouble();

	if(metatype!="QuitButlerCandidate")
	{
		std::cout<<"验证退出管家候选交易错误——metatype错误！\n";
		return false;
	}
	if(type!="QuitButlerCandidateMetadata")
	{
		std::cout<<"验证退出管家候选交易错误——type错误！\n";
		return false;
	}
	if(!account_manager.is_butler_candidate(pubkey))
	{
		std::cout<<"验证退出管家候选交易错误——pubkey错误！\n";
		return false;
	}
	if(!key_manager.verifyDocument(metadata,pubkey,sig))
	{
		std::cout<<"验证退出管家候选交易错误——签名错误！\n";
		return false;
	}
	return true;
}

bool POV::validateQuitCommissionerTransaction(rapidjson::Value &data)
{
	return true;
}

void POV::sendMessage(rapidjson::Document &d,bool setCallBack=false,double wait_time=100,callback_type type=TEST,NodeID childtype=0,handler_ptr caller=NULL)
{

	//设置回调字段
	rapidjson::Document::AllocatorType &allocator=d.GetAllocator();
	if(d.HasMember("callback_type"))
	{
		d["callback_type"].SetUint(type);
	}
	else
	{
		rapidjson::Value _type(type);
		d.AddMember("callback_type",_type,allocator);
	}

	if(d.HasMember("child_type"))
	{
		d["child_type"].SetUint64(childtype);
	}
	else
	{
		rapidjson::Value _childtype(childtype);
		d.AddMember("child_type",_childtype,allocator);
	}
	uint32_t index=d["index"].GetUint();
	//设置CallBack

	if(setCallBack)
	{
		//std::cout<<"in setCallBack\n";
		bool existed=false;
		if(CallBackList.size()>0)
		{
			//std::cout<<"回调instance数量："<<CallBackList.size()<<"!\n";
			for(std::vector<CallBackInstance>::const_iterator i=CallBackList.begin();i!=CallBackList.end();++i)
			{
				if(i->type==type and i->childtype==childtype)
				{
					//std::cout<<"该消息已发送！\n";
					if(type==Recall_RequestBlock)
					{
						//std::cout<<"type="<<int(Recall_RequestBlock)<<" && childtype="<<childtype<<"have existed!\n";
					}
					existed=true;
					break;
				}
				/*
				else
				{
					std::cout<<"i->type="<<i->type<<"\n";
					std::cout<<"type="<<type<<"\n";
					std::cout<<"i->childtype="<<i->childtype<<"\n";
					std::cout<<"childtype="<<childtype<<"\n";
				}
				*/
			}
		}
		if(!existed){
			//std::cout<<"CallBackList size="<<CallBackList.size()<<" 1\n";
			//std::shared_ptr<POV> ptr(this);
			Fun f=std::bind(caller,ptr,std::placeholders::_1);
			CallBackInstance cbi(index,type,childtype,wait_time,f);
			CallBackList.push_back(cbi);
			//std::cout<<"CallBackList size="<<CallBackList.size()<<" 2\n";
		}
		else
			return;
	}
	network->SendMessage(d);
}

void POV::handleResponseCommissionerPubkey(rapidjson::Document &d)
{
	//std::cout<<"在handleResponseCommissionerPubkey中\n";
	NodeID sender=d["sender"].GetUint64();
	std::string pubkey(d["pubkey"].GetString(),d["pubkey"].GetStringLength());
	//std::cout<<network->getNodeId()<<"节点 received pubkey in handleResponseCommissionerPubkey:"<<pubkey<<" from "<<sender<<"\n";
	account_manager.pushback_commissioner(pubkey,sender);
}

void POV::handleResponseApplyCommissionerSignature(rapidjson::Document &d)
{
	std::cout<<network->getNodeId()<<" in handleResponseApplyCommissionerSignature\n";
	//print_document(d);
	NodeID sender=d["sender"].GetUint64();
	if(d.HasMember("data"))
	{
		rapidjson::Value &data=d["data"];
		rapidjson::Document::AllocatorType& a = d.GetAllocator();
		std::string pubkey=data["pubkey"].GetString();
		std::string sig=data["sig"].GetString();
		if(!key_manager.verifyDocument(Apply_Commissioner_Metadata,pubkey,sig))
		{
			std::cout<<network->getNodeId()<<"验证"<<sender<<"签名不通过！\n";
			std::cout<<"pubkey:"<<pubkey<<"\n";
			std::cout<<"sig:"<<sig<<"\n";
			return;
		}
		//std::cout<<"received pubkey:"<<pubkey<<"\n";
		//std::string sig=data["sig"].GetString();
		//判断是否是委员
		if(!account_manager.is_commissioner(pubkey))
		{
			std::cout<<" not commissioner\n";
			return;
		}
		//判断该签名是否已经存在
		for(std::vector<signature>::iterator i=Apply_Commissioner_Signatures.begin();i!=Apply_Commissioner_Signatures.end();i++)
		{
			if(i->pubkey==pubkey)
			{
				//std::cout<<" loop in acs 2\n";
				return;
			}
		}
		signature _sig;
		_sig.pubkey=pubkey;
		_sig.sig=sig;
		Apply_Commissioner_Signatures.push_back(_sig);
		if(Apply_Commissioner_Signatures.size()>=account_manager.getCommissionerAmount())
		{
			gegenerate_first_bolck_apply_commissioner_state=ApplyCommissioner_final_apply;
			normal_apply_commissioner_state=ApplyCommissioner_final_apply;
		}

	}
	else
	{
		std::cout<<" not have data member\n";
		print_document(d);
	}

}

void POV::handleResponseApplyCommissioner(rapidjson::Document &d)
{
	//为简便起见，这里代理委员或值班管家返回的回复没有加入同意接受或拒绝信息。
	std::cout<<"在处理ResponseApplyCommissioner中\n";
	//print_document(d);
	GenerateFirstBlock_ApplyCommissioner=false;
	gegenerate_first_bolck_apply_commissioner_state=ApplyCommissioner_finish;
	normal_apply_commissioner_state=ApplyCommissioner_finish;
}

void POV::handleResponseRecommendationLetter(rapidjson::Document &d)
{
	std::cout<<"接收到推荐信：\n";
	//print_document(d);
	if(state==generate_first_block)
	{
		if(generate_first_bolck_apply_butler_candidate_state==ApplyButlerCandidate_apply_recommendation_letter and RecommendationLetter.IsNull())
		{
			//验证签名
			if(!validateRecommendationLetter(d["data"]))
			{
				std::cout<<"验证推荐信不通过！\n";
				return;
			}
			rapidjson::Document::AllocatorType &allocator=d.GetAllocator();
			//RecommendationLetter.SetNull();
			RecommendationLetter.GetAllocator().Clear();
			rapidjson::Value &letter=d["data"];
			RecommendationLetter.CopyFrom(d["data"],RecommendationLetter.GetAllocator());
			//print_document(RecommendationLetter);
			generate_first_bolck_apply_butler_candidate_state=ApplyButlerCandidate_apply_signatures;
		}
	}
	else
	{
		if(normal_apply_butler_candidate_state==ApplyButlerCandidate_apply_recommendation_letter and RecommendationLetter.IsNull())
		{
			//验证签名
			if(!validateRecommendationLetter(d["data"]))
			{
				std::cout<<"验证推荐信不通过！\n";
				return;
			}
			rapidjson::Document::AllocatorType &allocator=d.GetAllocator();
			//RecommendationLetter.SetNull();
			RecommendationLetter.GetAllocator().Clear();
			rapidjson::Value &letter=d["data"];
			RecommendationLetter.CopyFrom(d["data"],RecommendationLetter.GetAllocator());
			//print_document(RecommendationLetter);
			normal_apply_butler_candidate_state=ApplyButlerCandidate_apply_signatures;
		}
	}
}

void POV::handleResponseApplyButlerCandidateSignature(rapidjson::Document &d)
{
	std::cout<<"收到申请管家候选的签名！\n";
	rapidjson::Value &sig=d["data"];
	std::string pubkey=sig["pubkey"].GetString();
	if(!validateApplyButlerCandidateSignature(RecommendationLetter,sig))
	{
		std::cout<<"验证管家候选签名不正确！\n";
	}
	for(std::vector<signature>::iterator i=Apply_Butler_Candidate_Signatures.begin();i!=Apply_Butler_Candidate_Signatures.end();i++)
	{
		if(i->pubkey==pubkey)
		{
			//std::cout<<" loop in acs 2\n";
			return;
		}
	}
	signature _sig;
	_sig.pubkey=pubkey;
	_sig.sig=sig["sig"].GetString();
	Apply_Butler_Candidate_Signatures.push_back(_sig);
	if(state==generate_first_block)
	{
		//在生成创世区块阶段，只有获得所有委员的签名才能通过
		if(Apply_Butler_Candidate_Signatures.size()==account_manager.getCommissionerAmount())
		{
			generate_first_bolck_apply_butler_candidate_state=ApplyButlerCandidate_final_apply;
			std::cout<<"进入ApplyButlerCandidate_final_apply阶段！\n";
		}
	}
	else
	{
		//在其他阶段，只要获得超过委员数量一半的签名就能验证通过
		if(Apply_Butler_Candidate_Signatures.size()>=account_manager.getCommissionerAmount()/2)
		{
			//generate_first_bolck_apply_butler_candidate_state=ApplyButlerCandidate_final_apply;
			std::cout<<"进入ApplyButlerCandidate_final_apply阶段！\n";
			normal_apply_butler_candidate_state=ApplyButlerCandidate_final_apply;
		}
	}

}

void POV::handleResponseApplyButlerCandidate(rapidjson::Document &d)
{
	std::cout<<account_manager.getMyNodeID()<<"节点接收到最终申请管家候选的响应！\n";
	//清空推荐信和签名

	RecommendationLetter.SetNull();
	RecommendationLetter.GetAllocator().Clear();
	Apply_Butler_Candidate_Signatures.clear();
	//设置申请管家候选阶段为结束阶段。
	if(state==generate_first_block)
		generate_first_bolck_apply_butler_candidate_state=ApplyButlerCandidate_finish;
	else
		normal_apply_butler_candidate_state=ApplyButlerCandidate_finish;
}

void POV::handleResponseVote(rapidjson::Document &d)
{
	//std::cout<<"在投票回复中！\n";
	GenerateFirstBlock_vote=Vote_finish;
	//Normal_vote=Vote_finish;
}

void POV::handleResponseBlockSignature(rapidjson::Document &d)
{
	//GenerateFirstBlock=GenerateBlock_publish;
	//for(std::vector<signature>)
	//发送生区块时，生区块头Te=0，在签名时把Te设置为委员节点的当前时间然后签名，最后把签名、公钥和时间放在一起发送给打包区块的管家，在这里验证的时候也要在区块头中加入签名中的时间再验证。
	//std::cout<<"在处理区块签名的回复中：start!\n";
	NodeID sender=d["sender"].GetUint64();
	if(d.HasMember("data"))
	{
		//std::cout<<"在处理区块签名的回复中:"<<1<<"\n";
		rapidjson::Value &data=d["data"];
		//rapidjson::Document::AllocatorType& a = d.GetAllocator();
		std::string pubkey=data["pubkey"].GetString();
		std::string sig=data["sig"].GetString();
		double timestamp=data["timestamp"].GetDouble();
		//std::cout<<"在处理区块签名的回复中:"<<2<<"\n";
		//在管家发布新的区块后，就会把cache_block清空，这时如果有接收到迟到的签名回复，就会出错，因此要判断是否为空。
		if(cache_block==NULL)
		{
			std::cout<<"缓存的生区块错误:cache_block==NULL!\n";
			return;
		}
		//std::cout<<"在处理区块签名的回复中:"<<3<<"\n";

		//验证签名正确性
		if(!validateBlockSignature(cache_block->getRawHeader(),data))
		{
			std::cout<<"返回的区块签名验证错误！\n";
			return;
		}
		//std::cout<<"在处理区块签名的回复中:"<<4<<"\n";

		//判断该签名是否已经存在
		for(std::vector<signature>::iterator i=raw_block_signatures.begin();i!=raw_block_signatures.end();i++)
		{
			if(i->pubkey==pubkey)
			{
				//std::cout<<" loop in acs 2\n";
				return;
			}
		}
		//std::cout<<"在处理区块签名的回复中:"<<5<<"\n";

		signature _sig;
		_sig.pubkey=pubkey;
		_sig.sig=sig;
		_sig.timestamp=timestamp;
		raw_block_signatures.push_back(_sig);
		if(data.HasMember("ring_sig"))
		{
			std::string ring_sig=data["ring_sig"].GetString();
			JsonConfigParser sig_paser(ring_sig,JsonString);
			std::string Y=sig_paser.get_value<string>("Y");
			for(int i=0;i<ring_sigs_pool.size();i++)
			{
				JsonConfigParser old_sig_paser(ring_sigs_pool[i],JsonString);
				std::string old_Y=old_sig_paser.get_value<string>("Y");
				if(old_Y==Y)
				{
					std::cout<<"已经有该委员的环签名！";
					return ;
				}
			}
			std::cout<<"环签名不重复，将环签名加入到环签名池！";
			ring_sigs_pool.push_back(ring_sig);
		}
	}
	else
	{
		std::cout<<" not have data member\n";
		print_document(d);
	}
	//std::cout<<"在处理区块签名的回复中：7!\n";
	std::cout<<"收到"<<d["sender"].GetUint64()<<"节点区块签名！\n";
	if(state==generate_first_block)
	{
		if(raw_block_signatures.size()==account_manager.getCommissionerAmount())
		{
			GenerateFirstBlock=GenerateBlock_publish;
		}
	}
	else
	{
		if(raw_block_signatures.size()>=account_manager.getCommissionerAmount()/2.0)
		{
			GenerateBlock=GenerateBlock_publish;
		}
	}

	//std::cout<<"在处理区块签名的回复中：end!\n";
}

void POV::handleResponseNodeID(rapidjson::Document &d)
{
	//std::cout<<"获得了NodeID！\n";
	NodeID sender=d["sender"].GetUint64();
	std::string pubkey=d["pubkey"].GetString();
	account_manager.set_commissioner_ID(pubkey,sender);
	account_manager.set_butler_candidate_ID(pubkey,sender);
	account_manager.set_butler_ID(pubkey,sender);
}

void POV::handleResponseNormal(rapidjson::Document &d)
{
	//std::cout<<"接收到了正常交易！\n";
}

void POV::handleResponseQuitButlerCandidate(rapidjson::Document &d)
{

}

void POV::handleResponseQuitCommissioner(rapidjson::Document &d)
{

}

void POV::handleLog(rapidjson::Document &d)
{
	//std::cout<<"接受到日志:\n";
	//print_document(d);

	for(uint32_t i=0;i<account_manager.getButlerAmount();i++)
	{
		account butler=account_manager.getButler(i);
		NodeID id=butler.getNodeId();
		//std::cout<<"发送的管家id为："<<id<<"\n";
		if(id==0)
		{
			std::string pubkey=butler.getPubKey();
			char* ch=const_cast<char*>(pubkey.c_str());
			uint32_t *map_id=(uint32_t*)ch;
			rapidjson::Document& msg=msg_manager.make_Request_NodeID_By_Pubkey(pubkey);
			sendMessage(msg,true,10,Recall_RequestNodeID,*map_id,&POV::handleResponseNodeID);
			delete &msg;
		}
		else
		{
			rapidjson::Document& msg=msg_manager.make_Request_Normal(id,d);
			//print_document(msg);
			sendMessage(msg,false,20,Recall_RequestNormal,id,&POV::handleResponseNormal);
			delete &msg;
		}
	}
}

size_t POV::read_complete(char * buff, const boost::system::error_code & err, size_t bytes)
{
    if ( err) return 0;
    bool found = std::find(buff, buff + bytes, '\n') < buff + bytes;
    // 我们一个一个读取直到读到回车，不缓存
    return found ? 0 : 1;
}

void POV::handleLogQuery()
{
	//std::cout<<"handleLogQuery: 1\n";
	boost::asio::io_service service;
	//std::cout<<"handleLogQuery: 2\n";
	boost::asio::ip::tcp::acceptor acceptor(service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),query_port));
	//std::cout<<"查询端口为："<<query_port<<"\n";
	//std::cout<<"handleLogQuery: 3\n";
	//service.run();

    while ( true) {
        char buff[65535];
        memset(buff,0,65535);
    	std::cout<<"处理日志查询任务中...\n";
    	boost::asio::ip::tcp::socket sock(service);
    	//std::cout<<"handleLogQuery: 4\n";
        acceptor.accept(sock);
    	//std::cout<<"handleLogQuery: 5\n";
    	sock.read_some(boost::asio::buffer(buff));
        //int bytes = boost::asio::read(sock, boost::asio::buffer(buff), boost::bind(&POV::read_complete,buff,_1,_2));
    	//std::cout<<"handleLogQuery: 6\n";

    	std::string msg(buff);
    	rapidjson::Document doc;
    	doc.Parse(msg.c_str(),msg.size());
        std::cout<<"接收到的信息："<<msg<<"\n";

    	if(doc.IsObject()&&doc.HasMember("log"))
    	{
    		std::string command=doc["log"].GetString();
    		if(command=="fetch_height")
    		{
    			std::cout<<"请求区块链高度\n";
    			int height=blockchain.queryHeight();
        		rapidjson::Document d;
        		d.SetObject();
        		rapidjson::Document::AllocatorType& allocator=d.GetAllocator();
        		rapidjson::Value _height(height);
        		d.AddMember("height",_height,allocator);
    	        sock.write_some(boost::asio::buffer(getDocumentString(d)));
    		}
    		else if(command=="fetch_block")
    		{
    			std::cout<<"请求根据高度获取区块\n";
    			if(doc.HasMember("height"))
    			{
    				int height=doc["height"].GetInt();
    				PoVBlock block=blockchain.getBlockFromDatabase(height);
    				rapidjson::Document& d=block.getBlock();
        	        sock.write_some(boost::asio::buffer(getDocumentString(d)));
        	        delete &d;
    			}
    		}
    		else if(command=="fetch_log_time")
    		{
    			std::cout<<"请求根据时间范围获取日志\n";
    			if(doc.HasMember("start_time"))
    			{
    				//std::cout<<"接收到日志请求:1\n";
    				long start_time=doc["start_time"].GetInt64();
    				//std::cout<<"接收到日志请求:2\n";
    				long end_time=9999999999;
    				if(doc.HasMember("end_time"))
    				{
    					end_time=doc["end_time"].GetInt64();
    				}
    				//std::cout<<"接收到日志请求:3\n";
    				rapidjson::Document& d=blockchain.queryLogByTime(start_time,end_time);
    				//std::cout<<"接收到日志请求:4\n";
    				std::string buff=getDocumentString(d);
    				//std::cout<<"接收到日志请求:5\n";
        	        sock.write_some(boost::asio::buffer(buff));
    				//std::cout<<"接收到日志请求:6\n";
        	        delete &d;
    			}
    		}
    		else if(command=="fetch_log_errorCode" && doc.HasMember("errorCode"))
    		{
    			std::cout<<"请求根据错误码获取日志\n";
				//std::cout<<"接收到fetch_log_errorCode请求:1\n";
    			std::string err_code=doc["errorCode"].GetString();
				//std::cout<<"接收到fetch_log_errorCode请求:2\n";
				rapidjson::Document& d=blockchain.queryLogByErrorCode(err_code);
				//std::cout<<"接收到fetch_log_errorCode请求:3\n";
				std::string buff=getDocumentString(d);
				//std::cout<<"接收到fetch_log_errorCode请求:4\n";
    	        sock.write_some(boost::asio::buffer(buff));
				//std::cout<<"接收到fetch_log_errorCode请求:5\n";
    		}
    	}
    	else if(!doc.IsObject())
    	{
    		std::cout<<"不是object错误\n";
    		rapidjson::Document d;
    		d.SetObject();
    		rapidjson::Document::AllocatorType& allocator=d.GetAllocator();
    		std::string error_msg="字符串不是json文档";
    		rapidjson::Value _error_msg(error_msg.c_str(),error_msg.size(),allocator);
    		d.AddMember("error",_error_msg,allocator);
	        sock.write_some(boost::asio::buffer(getDocumentString(d)));
    	}
    	else
    	{
    		std::cout<<"没有log错误\n";
    		rapidjson::Document d;
    		d.SetObject();
    		rapidjson::Document::AllocatorType& allocator=d.GetAllocator();
    		std::string error_msg="不含log类型";
    		rapidjson::Value _error_msg(error_msg.c_str(),error_msg.size(),allocator);
    		d.AddMember("error",_error_msg,allocator);
	        sock.write_some(boost::asio::buffer(getDocumentString(d)));
    	}
        //std::string msg(buff, bytes);
        sock.close();
    }
}

void POV::handleResponseBlock(rapidjson::Document &d)
{
	//std::cout<<"收到请求回复的区块\n";
	NodeID sender=d["sender"].GetUint64();
	rapidjson::Value& block=d["data"]["block"];
	PoVBlock b;
	b.setBlock(block);
	if(sync_block_method==SequenceSync)
	{
		if(b.getHeight()==blockchain.getHeight()+1)
		{
			//std::cout<<account_manager.getMyNodeID()<<"收到高度为"<<b.getHeight()<<"的区块！\n";
			//blockchain.pushbackBlock(b);
			blockchain.pushbackBlockToDatabase(b);
			if(blockchain.getHeight()<current_syncer.height)
			{
				NodeID receiver=current_syncer.id;
				std::cout<<"向"<<receiver<<"请求高度为"<<blockchain.getHeight()+1<<"的区块\n";
				rapidjson::Document& msg=msg_manager.make_Request_Block(receiver,blockchain.getHeight()+1);
				//std::cout<<"CallBackList.size()="<<CallBackList.size()<<"\n";
				sendMessage(msg,true,10,Recall_RequestBlock,blockchain.getHeight()+1,&POV::handleResponseBlock);
				//std::cout<<"CallBackList.size()="<<CallBackList.size()<<"\n";
				//print_document(msg);
				delete &msg;
				current_syncer.start_time=getCurrentTime();
			}
			/*
			else
			{
				std::cout<<"blockchain.getHeight()="<<blockchain.getHeight()<<"\n";
				std::cout<<"current_syncer.height="<<current_syncer.height<<"\n";
			}
			*/
		}
		else
		{
			std::cout<<"b.getHeight()="<<b.getHeight()<<"\n";
			std::cout<<"blockchain.getHeight()"<<blockchain.getHeight()<<"\n";
		}
	}
	else
	{
		int height=b.getHeight();
		for(int i=0;i<con_syncers.size();i++)
		{
			if(con_syncers[i].height==height)
			{
				con_syncers[i].block=b;
				con_syncers[i].prepared=true;
				con_syncers[i].prehash=b.getPreHash();
				//con_syncers[i].hash=key_manager.getHash256(block);
			}
		}
	}

}

void POV::statisticData()
{
	//统计数据
	std::cout<<network->getNodeId()<<"节点结束运行!\n";
	StatContainer *staters=new StatContainer[end_height];
	for(uint32_t i=0;i<end_height;i++)
	{
		PoVBlock block=blockchain.getBlock(i);
		//区块高度
		staters[i].block_num=block.getHeight();
		//生成时间
		staters[i].Te=block.getTe();
		//获取区块大小
		rapidjson::Document& json_block=block.getBlock();
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		json_block.Accept(writer);
		staters[i].blocksize=buffer.GetSize();
		delete& json_block;
		//交易总数
		staters[i].tx_amout=block.getTransactionsAmout();
		//普通交易数
		std::vector<PoVTransaction> norm_txs;
		block.getNormalTransactions(norm_txs);
		staters[i].normal_tx_amout=norm_txs.size();
		//所有交易的时间戳
		staters[i].norm_txs_timestamp=new double[norm_txs.size()];
		for(uint32_t j=0;j<norm_txs.size();j++)
		{
			rapidjson::Document& tx=norm_txs.at(j).getData();
			//print_document(tx);
			staters[i].norm_txs_timestamp[j]=tx["timestamp"].GetDouble();
			delete &tx;
		}
		staters[i].cycles=block.getCycles();
	}
	std::cout<<"统计完成\n";
	//区块数据统计
	std::ofstream block_data;
	block_data.open("data/block_data.txt");
	block_data<<"区块高度, ";
	block_data<<"区块大小, ";
	block_data<<"生成时间, ";
	block_data<<"交易数量, ";
	block_data<<"正常交易数量, ";
	block_data<<"周期数, ";
	block_data<<"平均正常交易生成速率, ";
	block_data<<"每个区块交易打包速率;\n";
	double last_Te=0;
	double first_block_Te=staters[0].Te;
	uint32_t time_norm_txs_amout=0;
	for(uint32_t i=0;i<end_height;i++)
	{
		uint32_t num=staters[i].block_num;
		uint32_t size=staters[i].blocksize;
		double Te=staters[i].Te;
		uint32_t tx_amout=staters[i].tx_amout;
		uint32_t normal_tx_amout=staters[i].normal_tx_amout;
		uint32_t cycles=staters[i].cycles;


		block_data<<num<<", ";
		block_data<<size<<", ";
		//std::string Te(staters[i].Te,3);
		block_data<<std::to_string(Te)<<", ";
		block_data<<tx_amout<<", ";
		block_data<<normal_tx_amout<<", ";
		block_data<<cycles<<", ";
		time_norm_txs_amout=time_norm_txs_amout+normal_tx_amout;
		block_data<<std::to_string(time_norm_txs_amout/(Te-first_block_Te))<<", ";
		block_data<<std::to_string(normal_tx_amout/(Te-last_Te))<<";\n";
		last_Te=Te;
	}
	block_data.close();
	double total_defer_time=0;
	uint32_t total_normal_txs=0;
	std::cout<<"区块统计数据写入文件完成\n";
	//交易数据统计
	std::cout<<"locate: 1\n";
	std::ofstream transaction_data;
	std::cout<<"locate: 2\n";
	transaction_data.open("data/transaction_data.txt");
	std::cout<<"locate: 3\n";
	transaction_data<<"交易生成时间, ";
	std::cout<<"locate: 4\n";
	transaction_data<<"所在区块, ";
	std::cout<<"locate: 5\n";
	transaction_data<<"所在区块普通交易数, ";
	std::cout<<"locate: 6\n";
	transaction_data<<"所在区块生成时间;\n";
	std::cout<<"locate: 7\n";
	for(uint32_t i=0;i<end_height;i++)
	{
		//std::string Te(staters[i].Te,3);
		std::cout<<"第"<<i<<"区块交易数："<<staters[i].normal_tx_amout<<"\n";
		total_normal_txs+=staters[i].normal_tx_amout;
		for(uint32_t j=0;j<staters[i].normal_tx_amout;j++)
		{
			double Te=staters[i].Te;
			double norm_tx_timestamp=staters[i].norm_txs_timestamp[j];
			uint32_t num=staters[i].block_num;
			uint32_t amout=staters[i].normal_tx_amout;
			//std::cout<<staters[i].norm_txs_timestamp[j]<<",";
			//std::string tx_Te(staters[i].norm_txs_timestamp[j],3);
			transaction_data<<std::to_string(norm_tx_timestamp)<<", ";
			transaction_data<<num<<", ";
			transaction_data<<amout<<", ";
			transaction_data<<std::to_string(Te)<<";\n";
			total_defer_time=total_defer_time+Te-norm_tx_timestamp;
		}
		//std::cout<<"\n";
	}
	transaction_data.close();
	std::cout<<"交易统计数据写入文件完成\n";
	//记录系统设置参数
	std::ofstream settings;
	settings.open("data/settings.txt");
	settings<<"程序开始时间："<<network->getStartTime()<<";\n";
	settings<<"程序运行间隔："<<network->getInterval()<<";\n";
	settings<<"正常交易缓存池的容量大小："<<NormalPoolCapacity<<";\n";
	settings<<"区块最大普通交易数量："<<max_normal_trans_per_block<<";\n";
	settings<<"每个节点产生交易的概率："<<prob_generate_normal_tx<<";\n";
	settings<<"每个管家开始生成区块前等待的时间："<<defer_time<<";\n";
	settings<<"代理委员编号："<<agent_commissioner_num<<";\n";
	settings<<"委员数量："<<init_commissioner_size<<";\n";
	settings<<"委员名单：{";
	for(uint32_t i=0;i<init_commissioner_size;i++)
	{
		settings<<Initial_Commissioner_Nodes[i]<<", ";
	}
	settings<<"};\n";
	settings<<"管家候选数量："<<init_butler_candidate_size<<";\n";
	settings<<"管家候选名单：{";
	for(uint32_t i=0;i<init_butler_candidate_size;i++)
	{
		settings<<Initial_Butler_Candidate_Nodes[i]<<", ";
	}
	settings<<"};\n";
	settings<<"管家人数："<<butler_amount<<";\n";
	settings<<"投票人数："<<vote_amount<<";\n";
	settings<<"每个任职周期产生的区块数："<<num_blocks<<";\n";
	settings<<"每个区块产生的截止时间："<<Tcut<<";\n";
	settings<<"普通交易内容长度："<<tx_len<<";\n";
	settings<<"程序结束高度："<<end_height<<";\n";
	settings<<"平均交易时延："<<std::to_string(total_defer_time/total_normal_txs)<<";\n";
	std::time_t now_time=time(NULL);
	std::time_t running_time=now_time-network->getSystemStartTime();
	settings<<"运行时间："<<running_time<<";\n";
	settings.close();
	std::cout<<"设置数据写入文件完成;\n";
}

void POV::handleResponseGroupKey(rapidjson::Document &d)
{
	std::cout<<"接收到群密钥\n";
	//print_document(d);
	rapidjson::Document::AllocatorType &allocator=d.GetAllocator();
	NodeID sender=d["sender"].GetUint64();

	rapidjson::Value &data=d["data"];
	std::string gsk_info=data["gsk_info"].GetString();
	std::string algorithm_method=data["algorithm_method"].GetString();
	std::string pbc_param=data["pbc_param"].GetString();
	std::string gpk=data["gpk"].GetString();
	std::string pubkey=data["pubkey"].GetString();
	group_param param;
	param.gsk_info=gsk_info;
	param.algorithm_method=algorithm_method;
	param.pbc_param=pbc_param;
	param.gpk=gpk;
	group_list[pubkey]=param;
}

void POV::Initial_Group()
{
	std::cout<<"群初始化\n";
	group_name = key_manager.getPublicKey();
	gmsk_pass = key_manager.getPrivateKey(); //manager's password
	std::cout<<"群名字："<<group_name<<"\n";
	std::cout<<"群密码："<<gmsk_pass<<"\n";
	std::string prefix = "group "+ group_name;
	pbc_param="{\"linear_type\":\"a\", \"q_bits_len\": 256, \"r_bits_len\":256}";//an example of parameter
	std::string result;
	std::cout<<"创建群\n";
	int internal_ret = create_group(result , algorithm_method ,pbc_param);
	vector<string> result_vec = dev::eth::split(result, dev::eth::BBSGroupSig::RET_DELIM);
	   //vector[0]: gpk --return and store
	   //vector[1]: gmsk --store
	   //vector[2]: gamma --store
	   //vector[3]: pbc_param --store
	gmsk = result_vec[1];//group manager's psk
	gpk = result_vec[0];//group's pbk
	gamma = result_vec[2];// gamma information, a secert number
	pbc_param = result_vec[3];
	/*
	std::cout<<"gmsk："<<gmsk<<"\n";
	std::cout<<"gpk："<<gpk<<"\n";
	std::cout<<"gamma："<<gamma<<"\n";
	std::cout<<"pbc_param："<<pbc_param<<"\n";
	*/
	std::cout<<"\n初始化群完成\n";


}
/*

void POV::setDatabase(mongocxx::client &client)
{
	this->client=client;
}
*/
/*
rapidjson::Document& POV::generateBallot(std::vector<account> &butler_candidate_list)
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
	for(uint32_t i=0;i<vote_amout;i++)
	{
		std::string pubkey=butler_candidate_list.at(i).getPubKey();
		rapidjson::Value &member=*(new rapidjson::Value(rapidjson::kObjectType));
		member.SetObject();
		member.SetString(pubkey.c_str(),pubkey.size());
		selected_member.PushBack(member,allocator);
	}
	ballot.AddMember("list",selected_member,allocator);
	//这里的pubkey是从key_manager中获取的，可以获取pubkey的方法太多，以后需要统一改为使用某一固定方法。
	std::string str_voter=key_manager.getPublicKey();
	rapidjson::Value &voter=*(new rapidjson::Value(str_voter.c_str(),str_voter.size(),allocator));
	ballot.AddMember("voter",voter,allocator);
	return ballot;
}
*/

