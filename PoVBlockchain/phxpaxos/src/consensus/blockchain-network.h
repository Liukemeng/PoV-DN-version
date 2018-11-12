/*
 * blockchain-network.h
 *
 *  Created on: 2018年3月11日
 *      Author: blackguess
 */

#ifndef BLOCKCHAIN_NETWORK_H
#define BLOCKCHAIN_NETWORK_H


//#include "POV.h"
#include "CallBackInstance.h"
#include <memory>
#include "constants.h"
#include "DocumentContainer.h"
#include <queue>
#include<ctime>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "dfnetwork.h"
#include <thread>
#include "udp.h"
#include "tcp.h"
#include <mutex>
//#include <boost/thread/thread.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

struct cache
{
	uint32_t index;
	NodeID sender;
	NodeID receiver;
};

struct DocumentSocketContainer
{
	DocumentContainer container;
	//Ptr<Socket> socket;
};

struct NodeInfo{
	std::string ip;
	uint32_t port;
	NodeID id;
};

class blockchain_network: public phxpaxos::DFNetWork {
public:
	//static TypeId GetTypeId (void);
	blockchain_network();
	virtual ~blockchain_network();
	//正式启动网络线程和PoV线程
	void StartApplication ();
	//发送json对象给指定IP和端口
	void SendPacket(rapidjson::Document& message, std::string ip,uint32_t port);
	//作为回调函数处理接收到的PoV协议信息及日志采集信息
	int OnReceiveMessage(const char * pcMessage, const int iMessageLen);
	//工具函数，把字符串IP转换为32位的整数
	uint32_t IPToValue(const string& strIP);
	//把消息发送给网络中的所有节点
	void SendToAll(rapidjson::Document &d);
	//把消息发送给除自己以外的所有节点
	void SendToNeighbor(rapidjson::Document &d);
	//把消息发送给指定NodeID的节点
	void SendToOne(rapidjson::Document &d,NodeID receiver);
	//把消息转发给除自己及发送方以外的所有节点
	void forwarding(rapidjson::Document &d);
	//提供给PoV调用的接口函数，根据json中的信息来决定发送目标
	void SendMessage(rapidjson::Document &d);
	//获取本节点ID
	NodeID getNodeId();
	//把IP和端口转换为一个64位的NodeID
	NodeID getNodeId(std::string ip,uint32_t port);
	//执行节点发现协议及PoV逻辑
	void run();
	//节点发现协议
	void runDiscoverNode();
	//发送节点发现协议消息
	void sendDiscoverNode(std::string receiver_ip,uint32_t receiver_port,bool is_request);
	//处理接收到的节点发现协议消息
	void handleDiscoverNode(rapidjson::Document& d);
	//系统初始化
	void init(std::string ip,uint32_t port);
	//根据配置文件设置POV相关参数
	void setPoVConfig(std::string file);
	//根据配置文件设置网络模块相关参数
	void setNetworkConfig(std::string file);
	//获取相邻节点信息
	std::map<NodeID,NodeInfo>& getNodeList();
	//统计工具
	double getStartTime();
	double getInterval();
	double getSystemStartTime();
private:
	std::mutex mutex;  //线程锁
	POV *pov;   //PoV对象
	std::thread *PoV_runner;   //PoV逻辑线程
	std::vector<cache> msgCache;  //缓存接收到的消息
	uint32_t index;		//从程序开始发送的消息编号
	//bool temp_switch;
	double interval=0.01;	//程序每次运行的时间间隔
	double start_time=2;    //程序开始运行的时间
	double last_discover_node_time=-1;	//上次进行节点发现的时间
	double program_start_time;   //程序开始的时间
	std::map<NodeID,NodeInfo> NodeList;  //把NodeID映射到具体的IP地址和端口
	NodeInfo my_nodeinfo;	//本节点的地址和端口
	std::string DNS_IP="127.0.0.1";  //用于节点发现协议的IP地址
	uint32_t DNS_PORT=5010;		//用于节点发现协议的端口
	double stop_time=0;		//程序停止时间，单位秒
	double discover_node_period=60;		//发节点发现协议运行周期

};


#endif /* SRC_APPLICATIONS_MODEL_CONSENSUS_BLOCKCHAINNETWORK_H_ */
