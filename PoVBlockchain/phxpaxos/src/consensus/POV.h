/*
 * POV.h
 *
 *  Created on: 2018年3月16日
 *      Author: blackguess
 */

#ifndef POV_H
#define POV_H

#include <memory>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "KeyManager.h"
#include "account.h"
#include "MessageManager.h"
#include "AccountManager.h"
#include "constants.h"
#include "CallBackInstance.h"
#include "DocumentContainer.h"
#include "PoVBlock.h"
#include "PoVHeader.h"
#include "PoVBlockChain.h"
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include<boost/array.hpp>
#include<boost/system/system_error.hpp>
#include <boost/asio.hpp>
#include <thread>
#include <queue>
#include <cryptopp/integer.h>
#include "algorithm/RingSig.h"
//#include "ns3/blockchain-network.h"

//这里使用enum来控制程序运行状态

//程序总体共三个阶段，有同步区块，产生创世区块，正常运行阶段
enum system_state
{
	sync_block,
	generate_first_block,
	normal,
};
//退出管家候选状态
enum QuitButlerCandidateState
{
	QuitButlerCandidate_apply,
	QuitButlerCandidate_finish,
};

//退出委员状态
enum QuitCommissionerState
{
	QuitCommissioner_apply,
	QuitCommissioner_finish,
};

//申请管家候选状态
enum ApplyButlerCandidateState
{
	ApplyButlerCandidate_apply_recommendation_letter,
	ApplyButlerCandidate_apply_signatures,
	ApplyButlerCandidate_final_apply,
	ApplyButlerCandidate_finish,
};

//申请委员状态
enum ApplyCommissionerState
{
	ApplyCommissioner_apply_signatures,
	ApplyCommissioner_final_apply,
	ApplyCommissioner_finish,
};

//投票状态
enum VoteState
{
	Vote_voting,
	Vote_finish,
};

//生成区块状态
enum GenerateBlockState
{
	GenerateBlock_sign,
	GenerateBlock_publish,
	GenerateBlock_finish,
};

enum SyncBlockMethod
{
	SequenceSync,
	ConcurrentSync,
};

//用于收集重要的统计数据
struct StatContainer
{
	uint32_t block_num;
	double Te;
	uint32_t blocksize;
	uint32_t tx_amout;
	uint32_t normal_tx_amout;
	double* norm_txs_timestamp;
	uint32_t cycles;
};

//用于sequencesync
struct block_syncer
{
	NodeID id=0;
	int height=-1;
	double start_time=0;
	double wait_time=10;
	bool valid=false;
};
//用于concurrentsync
struct concurrent_block_syncer
{
	int height;
	std::string hash;
	std::string prehash;
	bool prepared;
	PoVBlock block;
};
//群参数
struct group_param
{
	std::string group_name;
	std::string gmsk_pass; //manager's password
	std::string algorithm_method = "bbs04"; //the only choice
	std::string pbc_param="{\"linear_type\":\"a\", \"q_bits_len\": 256, \"r_bits_len\":256}";//an example of parameter

    std::string gmsk;//group manager's psk
	std::string gpk;//group's pbk
	std::string gamma;// gamma information, a secert number
	std::string gsk_info;
};
class blockchain_network;
class POV {
public:
	POV();
	virtual ~POV();
	//序列同步区块
	void syncBlockSequence();
	//并发同步区块
	void syncBlockConcurrency();
	//PoV一致协议入口，会一致循环运行
	void run();
	//处理接收到的区块链消息
	void handleMessage(rapidjson::Document &d);
	//初始化POV状态
	void init(blockchain_network* bn);
	//在生成创世区块阶段申请委员
	void Initial_ApplyCommissioner();
	//在生成创世区块阶段申请管家候选
	void Initial_ApplyButlerCandidate();
	//在生成创世区块阶段发送投票选举下一任管家
	void Initial_Vote();
	//生成创世区块
	void Initial_GenerateBlock();
	//创建群
	void Initial_Group();
	//正常阶段获取群密钥
	void Normal_Get_Group_Key(std::string pubkey);
	//正常阶段投票过程
	void Normal_Vote();
	//正常阶段产生区块
	void Normal_GenerateBlock();
	//正常阶段随机发布普通事务
	void Normal_PublishTransaction();
	//正常阶段申请委员
	void Normal_ApplyCommissioner();
	//正常阶段申请管家候选
	void Normal_ApplyButlerCandidate();
	//正常阶段退出委员
	void Normal_QuitCommissioner();
	//正常阶段退出管家候选
	void Normal_QuitButlerCandidate();
	//处理接收到的日志
	void handleLog(rapidjson::Document &d);
	//处理请求委员公钥的回复
	void handleResponseCommissionerPubkey(rapidjson::Document &d);
	//处理申请委员签名的回复
	void handleResponseApplyCommissionerSignature(rapidjson::Document &d);
	//处理申请委员的回复
	void handleResponseApplyCommissioner(rapidjson::Document &d);
	//处理请求推荐信的回复
	void handleResponseRecommendationLetter(rapidjson::Document &d);
	//处理申请管家候选签名的回复
	void handleResponseApplyButlerCandidateSignature(rapidjson::Document &d);
	//处理申请管家候选的回复
	void handleResponseApplyButlerCandidate(rapidjson::Document &d);
	//处理投票的回复
	void handleResponseVote(rapidjson::Document &d);
	//处理请求区块签名的回复
	void handleResponseBlockSignature(rapidjson::Document &d);
	//处理请求节点ID的回复
	void handleResponseNodeID(rapidjson::Document &d);
	//处理发布正常事务的回复
	void handleResponseNormal(rapidjson::Document &d);
	//处理请求退出管家候选的回复
	void handleResponseQuitButlerCandidate(rapidjson::Document &d);
	//处理请求退出委员的回复
	void handleResponseQuitCommissioner(rapidjson::Document &d);
	//处理区块请求
	void handleResponseBlock(rapidjson::Document &d);
	//处理群密钥请求
	void handleResponseGroupKey(rapidjson::Document &d);
	//发送消息
	void sendMessage(rapidjson::Document &d,bool setCallBack,double wait_time,callback_type type,NodeID childtype,handler_ptr caller);
	//验证申请委员事务
	bool validateApplyCommissionerTransaction(rapidjson::Value &data);
	//验证推荐信
	bool validateRecommendationLetter(rapidjson::Value &data);
	//验证申请管家候选签名
	bool validateApplyButlerCandidateSignature(rapidjson::Value &letter,rapidjson::Value &data);
	//验证申请管家候选的数据
	bool validateApplyButlerCandidateData(rapidjson::Value &data);
	//验证投票是否正确
	bool validateBallot(rapidjson::Value &data);
	//验证生区块是否正确
	bool validateRawBlock(rapidjson::Value &data);
	//验证完整区块是否正确
	bool validateBlock(rapidjson::Value &data);
	//验证区块签名是否合法
	bool validateBlockSignature(rapidjson::Document& header,rapidjson::Value &data);
	//验证投票事务是否正确
	bool validateVoteTransaction(rapidjson::Value &data);
	//验证退出管家候选事务
	bool validateQuitButlerCandidateTransaction(rapidjson::Value &data);
	//验证退出委员事务
	bool validateQuitCommissionerTransaction(rapidjson::Value &data);
	//产生创世区块
	PoVBlock generateFirstBlock();
	//产生普通区块和特殊区块
	PoVBlock generateBlock();
	//统计投票
	rapidjson::Document& BallotStatistic(std::vector<DocumentContainer> &ballots);
	//根据区块签名计算下一个产生区块的管家
	uint32_t getNextButler(rapidjson::Value& sigs);
	//根据区块签名获取区块发布时间
	uint32_t getTe(rapidjson::Value& sigs);
	//根据区块内容更新系统变量
	void updateVariables(rapidjson::Value& block);
	//根据配置文件设置系统参数
	void setConfig(std::string path);
	//判断buff中的内容是否读完整，和handleLogQuery配合使用
	static size_t read_complete(char * buff, const boost::system::error_code & err, size_t bytes);
	//处理日志查询请求
	void handleLogQuery();
	//更新节点ID，deprecated
	void updateNodeId();
	//统计信息生成一个文本文件
	void statisticData();
	//void setClient(mongocxx::client &client);
	//rapidjson::Document& generateBallot(std::vector<account> &butler_candidate_list);

private:
	blockchain_network* network; 	//网络模块
	KeyManager key_manager;		//秘钥管理器，用于签名，验证，做哈希等功能
	MessageManager msg_manager;		//消息管理器，用于生成系统运行所需的各种类型的消息
	//mongocxx::client &client;
	account my_account;		//本节点账号
	system_state state;		//系统运行状态
	AccountManager account_manager;		//账号管理器
	std::vector<CallBackInstance> CallBackList; 	//回调实例列表
	NodeID m_ID;	//本地节点ID
	PoVBlockChain blockchain; 	//区块链数据库，用于存放区块
	bool I_am_commissioner=false;		//本账号是否委员
	bool I_am_butler_candidate=false;	//本节点是否管家候选




	//系统变量
	uint32_t period_generated_block_num;//在一个任职周期内已经生成的区块数
	double start_time=0;//上个区块发布时间
	int init_duty_butler_num=-1;//上个区块指定的管家
	int current_duty_butler_num=-1; //当前任职管家编号
	std::string current_duty_butler_pubkey=""; //当前任职管家公钥



	//交易池
	std::vector<DocumentContainer> QuitButlerCandidatePool; //退出管家候选交易池
	std::vector<DocumentContainer> QuitCommissionerPool; //退出委员交易池
	std::vector<DocumentContainer> ApplyCommissionerPool;//申请委员交易缓存池
	std::vector<DocumentContainer> ApplyButlerCandidatePool; //申请管家候选交易缓存池
	std::vector<DocumentContainer> BallotPool; //投票交易缓存池
	std::vector<DocumentContainer> NormalPool; //正常交易缓存池
	uint32_t NormalPoolCapacity=4000; //正常交易缓存池的容量大小

	//以下为元数据及签名等数据
	rapidjson::Document Apply_Commissioner_Metadata;	//申请委员的元数据
	std::vector<signature> Apply_Commissioner_Signatures;	//申请委员签名容器
	rapidjson::Document Apply_Commissioner_Transaction;		//申请委员的事务
	rapidjson::Document RecommendationLetter;		//推荐信
	std::vector<signature> Apply_Butler_Candidate_Signatures;	//申请管家候选的签名
	PoVBlock *cache_block;		//缓存的生区块，等待委员验证后加入委员签名，最后发布
	std::vector<signature> raw_block_signatures;	//生区块签名集合
	rapidjson::Document Quit_Butler_Candidate_Metadata;		//退出管家候选的元数据
	rapidjson::Document Quit_Commissioner_Metadata;		//退出管家的元数据

	//以下为一些控制程序运行的状态标识
	bool GenerateFirstBlock_ApplyCommissioner=true;		//用于控制生成创世区块阶段是否执行申请委员过程,deprecated
	ApplyCommissionerState gegenerate_first_bolck_apply_commissioner_state;		//生成创世区块阶段申请管家候选的控制状态
	ApplyButlerCandidateState generate_first_bolck_apply_butler_candidate_state;		//生成创世区块阶段申请管家候选的控制状态
	VoteState GenerateFirstBlock_vote;		//生成创世区块阶段的投票控制状态
	GenerateBlockState GenerateFirstBlock;		//生成创世区块阶段的总控制状态
	GenerateBlockState GenerateBlock;		//正常阶段的总体控制状态
	VoteState Normal_vote=Vote_voting;		//正常阶段的投票状态控制，初始化为Vote_voting
	ApplyCommissionerState normal_apply_commissioner_state;		//正常阶段的申请委员状态控制
	ApplyButlerCandidateState normal_apply_butler_candidate_state;		//正常阶段申请管家候选状态控制
	QuitButlerCandidateState normal_quit_butler_candidate_state;		//正常阶段退出管家候选状态控制
	QuitCommissionerState normal_quit_commissioner_state;		//正常阶段退出管家候选状态控制

	//系统固定参数
	uint32_t max_normal_trans_per_block=2000; //每个区块最多存放的普通交易数量
	NodeID AgentCommissioner;
	double prob_generate_normal_tx=0.001; //每个节点产生交易的概率
	double prob_apply_commissioner=0.0001; //非委员节点申请成为委员的概率
	double prob_apply_butler_candidate=0.0001; //非管家候选申请成为管家候选的概率
	double prob_quit_commissioner=0.00001; //委员节点申请退出委员的概率
	double prob_quit_butler_candidate=0.001; //管家候选申请退出管家候选的概率
	double defer_time=5; //每个管家开始打包区块前等待的时间
	//同步区块变量
	SyncBlockMethod sync_block_method=SequenceSync;
	//bool send_request_height=true;
	double fetch_height_start_time=0;  //同步区块的开始时间
	double fetch_height_wait_time=10;	//同步区块的等待时间
	double fetch_height_wait_time_last_encore=1;	//发送高度请求的时间间隔
	std::queue<block_syncer> syncers;	//区块同步器列表
	block_syncer current_syncer;	//正在进行同步的区块同步器
	double syncer_wait_time=15;		//同步器失效的等待时间
	//并发同步所需变量
	int max_height; //最大高度
	std::vector<concurrent_block_syncer> con_syncers; // 并发同步器

	//预定义的初始委员节点,
	uint32_t agent_commissioner_num=0;//这里固定代理委员为第一个委员

	uint32_t init_commissioner_size=5; //委员数量
	NodeID *Initial_Commissioner_Nodes; //委员名单

	uint32_t init_butler_candidate_size=10;	//系统初始阶段管家候选人数
	NodeID *Initial_Butler_Candidate_Nodes; //管家候选名单
	uint32_t butler_amount=10;//管家人数
	uint32_t vote_amount=10; //投票人数
	uint32_t num_blocks=10; //每个任职周期产生的区块数
	double Tcut=20; //每个区块产生的截止时间
	uint32_t tx_len=10; //普通交易内容长度
	uint32_t end_height=10; //程序结束高度
	std::shared_ptr<POV> ptr;	//指向POV的指针
	std::thread *query_thread;	//日志查询线程
	int query_port;		//日志查询端口

	//环签名相关变量
	std::vector<std::string> ring_pubkeys;
	std::vector<std::string> ring_prikeys;
	std::string ring_prikey;
	std::string ring_param;
	uint32_t ring_num;
	LinkableRingSigImpl lrsi;
	vector<std::string> ring_sigs_pool;

	//群签名变量
	std::string group_name="#";
	std::string gmsk_pass; //manager's password
	const std::string algorithm_method = "bbs04"; //the only choice
	std::string pbc_param;//an example of parameter

	std::string gmsk;//group manager's psk
	std::string gpk;//group's pbk
	std::string gamma;// gamma information, a secert number
	//群主需要每个群成员公钥和其秘钥的map
	std::map<std::string,std::string> gsk_info_list;
	//群成员需要所属的群的群主和群的map——publickey<->group
	std::map<std::string,group_param> group_list;
	//域名列表
	std::vector<std::string> domain_names_list;
	double reading_interval;
	std::string file_path;
	double last_reading_time=0;
};



#endif /* POV_H */
