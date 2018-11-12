/*
 * constants.h
 *
 *  Created on: 2018年3月16日
 *      Author: blackguess
 */


#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <cstdint>
#include <vector>
#include <string>

#define NodeID uint64_t
#define pov_null 99999  //作为获取账号ID找不到时的返回值

//数据发送方式
enum Destination
{
	SenToOne,
	SendToAll,
	Forwading,
};

//回调的类型
enum callback_type
{
	TEST,
	Echo,
	Recall_RequestCommissionerPubkey,
	Recall_RequestApplyCommissionerSignature,
	Recall_RequestApplyCommissioner,
	Recall_RequestApplyRecommendationLetter,
	Recall_RequestApplyButlerCandidateSignature,
	Recall_RequestApplyButlerCandidate,
	Recall_RequestVote,
	Recall_RequestBlockSignature,
	Recall_RequestNormal,
	Recall_RequestNodeID,
	Recall_RequestQuitButlerCandidate,
	Recall_RequestQuitCommissioner,
	Recall_RequestBlock,
	Recall_RequestGroupKey,
};

//消息类型
enum MessageType
{
	RequestCommissionerPubkey,
	ResponseCommissionerPubkey,
	RequestApplyCommissionerSignature,
	ResponseApplyCommissionerSignature,
	RequestApplyCommissioner,
	ResponseApplyCommissioner,
	RequestRecommendationLetter,
	ResponseRecommendationLetter,
	RequestApplyButlerCandidateSignature,
	ResponseApplyButlerCandidateSignature,
	RequestApplyButlerCandidate,
	ResponseApplyButlerCandidate,
	RequestVote,
	ResponseVote,
	RequestBlockSignature,
	ResponseBlockSignature,
	PublishBlock,
	RequestNormal,
	ResponseNormal,
	RequestNodeID,
	ResponseNodeID,
	RequestQuitButlerCandidate,
	ResponseQuitButlerCandidate,
	RequestQuitCommissioner,
	ResponseQuitCommissioner,
	RequestHeight,
	ResponseHeight,
	RequestBlock,
	ResponseBlock,
	RequestGroupKey,
	ResponseGroupKey,
};

//deprecated
enum MetadataType
{
	ApplyCommissioner,
};

//存储签名信息
struct signature
{
	std::string pubkey="";
	std::string sig="";
	double timestamp=0.0;
};

#endif /* SRC_APPLICATIONS_MODEL_CONSENSUS_CONSTANTS_H_ */
