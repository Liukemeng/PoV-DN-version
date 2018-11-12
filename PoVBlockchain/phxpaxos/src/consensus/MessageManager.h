/*
 * MessageManager.h
 *
 *  Created on: 2018年3月31日
 *      Author: blackguess
 */

#ifndef MESSAGEMANAGER_H
#define MESSAGEMANAGER_H
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "constants.h"
#include<string>
#include<iostream>
#include "KeyManager.h"
#include "utils.h"

class blockchain_network;
class MessageManager {
public:
	MessageManager();
	virtual ~MessageManager();
	//初始化消息管理器
	void init(std::string public_key,blockchain_network *bn,KeyManager *manager);
	//设置公钥
	void setPubkey(std::string pubkey);
	//获取当前的index
	uint32_t getIndex();
	//生成消息的基础函数
	rapidjson::Document& make_Message(uint32_t index,NodeID sender,NodeID receiver,
			std::string pubkey,MessageType type,rapidjson::Document &data,uint32_t respond_to,std::string signature);
	//稍加封装的消息生成基础函数
	rapidjson::Document& make_Message(NodeID receiver,
			MessageType type,rapidjson::Document &data,uint32_t respond_to,std::string signature);
	//生成申请委员公钥请求
	rapidjson::Document& make_Request_Commissioner_PublicKey(NodeID receiver);
	//生成申请委员公钥回复
	rapidjson::Document& make_Response_Commissioner_PublicKey(NodeID receiver,uint32_t response_index);
	//生成申请委员交易元数据
	rapidjson::Document& get_Apply_Commissioner_Metadata();
	//生成申请委员签名请求
	rapidjson::Document& make_Request_Apply_Commissioner_Signature(NodeID receiver,rapidjson::Document& data);
	//生成申请委员签名回复
	rapidjson::Document& make_Response_Apply_Commissioner_Signature(NodeID receiver,uint32_t response_index,rapidjson::Value& metadata);
	//生成申请委员请求
	rapidjson::Document& make_Request_Apply_Commissioner(NodeID receiver,rapidjson::Document& data);
	//生成申请委员回复
	rapidjson::Document& make_Response_Apply_Commissioner(NodeID receiver,uint32_t response_index);
	//生成申请推荐信的请求
	rapidjson::Document& make_Request_Recommend_Letter(NodeID receiver);
	//生成推荐信元数据
	rapidjson::Document& get_Recommend_Letter_Metadata(std::string pubkey);
	//生成申请推荐信回复
	rapidjson::Document& make_Response_Recommend_Letter(NodeID receiver,uint32_t response_index,std::string refferal_pubkey);
	//生成申请管家候选签名请求
	rapidjson::Document& make_Request_Apply_Butler_Candidate_Signature(NodeID receiver,rapidjson::Document& d);
	//生成申请管家候选签名回复
	rapidjson::Document& make_Response_Apply_Butler_Candidate_Signature(NodeID receiver,uint32_t response_index,rapidjson::Value& metadata);
	//生成申请管家候选请求
	rapidjson::Document& make_Request_Apply_Butler_Candidate(NodeID receiver,rapidjson::Document& data);
	//生成申请管家候选回复
	rapidjson::Document& make_Response_Apply_Butler_Candidate(NodeID receiver,uint32_t response_index);
	//生成投票请求
	rapidjson::Document& make_Request_Vote(NodeID receiver,rapidjson::Document& ballot);
	//生成投票回复
	rapidjson::Document& make_Response_Vote(NodeID receiver,uint32_t response_index);
	//生成申请区块签名请求
	rapidjson::Document& make_Request_Block_Signature(NodeID receiver,rapidjson::Document& block);
	//生成附带群签名的申请区块签名请求
	rapidjson::Document& make_Request_Block_Signature_With_Group_Sig(NodeID receiver,rapidjson::Document& block,
			const std::string& algorithm_method,
	        const std::string& gpk_info,
	        const std::string& gsk_info,
	        const std::string& pbc_param_info);
	//生成申请区块签名回复
	rapidjson::Document& make_Response_Block_Signature(NodeID receiver,uint32_t response_index,rapidjson::Value& header);
	//生成发布区块请求
	rapidjson::Document& make_Request_Publish_Block(rapidjson::Document& block);
	//生成普通交易
	rapidjson::Document& make_Request_Normal(uint32_t len);
	//生成普通交易请求
	rapidjson::Document& make_Request_Normal(NodeID receiver,rapidjson::Document& content);
	//生成普通交易回复
	rapidjson::Document& make_Response_Normal(NodeID receiver,uint32_t response_index);
	//生成NodeID请求
	rapidjson::Document& make_Request_NodeID_By_Pubkey(std::string pubkey);
	//生成NodeID回复
	rapidjson::Document& make_Response_NodeID_By_Pubkey(NodeID receiver,uint32_t response_index);
	//生成退出管家候选交易
	rapidjson::Document& get_Quit_Butler_Candidate_Metadata();
	//生成退出委员交易
	rapidjson::Document& get_Quit_Commissioner_Metadata();
	//生成退出管家候选请求
	rapidjson::Document& make_Request_Quit_Butler_Candidate(NodeID receiver,rapidjson::Document& data);
	//生成退出管家候选回复
	rapidjson::Document& make_Response_Quit_Butler_Candidate(NodeID receiver,uint32_t response_index);
	//生成退出委员请求
	rapidjson::Document& make_Request_Quit_Commissioner(NodeID receiver,rapidjson::Document& data);
	//生成退出委员回复
	rapidjson::Document& make_Response_Quit_Commissioner(NodeID receiver,uint32_t response_index);
	//生成获取高度请求
	rapidjson::Document& make_Request_Height();
	//生成获取高度回复
	rapidjson::Document& make_Response_Height(NodeID receiver,int height);
	//生成获取区块请求
	rapidjson::Document& make_Request_Block(NodeID receiver,int height);
	//生成获取区块回复
	rapidjson::Document& make_Response_Block(NodeID receiver,uint32_t response_index,rapidjson::Value& block);
	//生成获取区块请求
	rapidjson::Document& make_Request_Group_Key(NodeID receiver);
	//生成获取区块回复
	rapidjson::Document& make_Response_Group_Key(NodeID receiver,uint32_t response_index,std::string gsk_info,std::string algorithm_method,std::string pbc_param,std::string gpk);
private:
	uint32_t index;
	std::string pubkey;
	blockchain_network *network;
	KeyManager *key_manager;
};



#endif /* SRC_APPLICATIONS_MODEL_CONSENSUS_MESSAGEMANAGER_H_ */
