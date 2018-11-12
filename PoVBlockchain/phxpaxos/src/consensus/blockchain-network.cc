/*
 * blockchain-network.cpp
 *
 *  Created on: 2018年3月11日
 *      Author: blackguess
 */

#include "blockchain-network.h"
//#include "ns3/udp-socket-factory.h"
//#include "ns3/uinteger.h"
//#include "ns3/log.h"
//#include "ns3/socket.h"
//#include "ns3/address-utils.h"
//#include "ns3/udp-socket.h"
//#include "ns3/tcp-socket-factory.h"
//#include "ns3/simulator.h"
#include <time.h>
//#include "ns3/simulator.h"
#include "POV.h"
#include <memory>
//#include "ns3/ipv4-address.h"
#include "utils.h"
#include "constants.h"
#include "udp.h"
#include <iomanip>
#include "ConfigHelper.h"


blockchain_network::blockchain_network() //: m_oUDPRecv(this), m_oTcpIOThread(this)
{

}

blockchain_network::~blockchain_network()
{

}


void blockchain_network::init(std::string ip,uint32_t port)
{
	//初始化网络参数
	my_nodeinfo.ip=ip;
	std::cout<<"my_nodeinfo.ip："<<my_nodeinfo.ip<<"\n";
	my_nodeinfo.port=port;
	std::cout<<"my_nodeinfo.port"<<my_nodeinfo.port<<"\n";
	my_nodeinfo.id=getNodeId(ip,port);
	std::cout<<"my_nodeinfo.id："<<my_nodeinfo.id<<"\n";

	std::cout<<"init blockchain_network:1\n";
	index=0;
	std::cout<<"init blockchain_network:2\n";

	std::cout<<"init blockchain_network:4\n";
	msgCache=std::vector<cache>();
	std::cout<<"init blockchain_network:5\n";

	//初始化pov
	pov=new POV();
	pov->init(this);
}

void blockchain_network::StartApplication ()
{
	//初始化网络
	int ret=Init(my_nodeinfo.ip,my_nodeinfo.port,2);
	if(ret!=0)
	{
		std::cout<<"初始化失败："<<DNS_PORT<<std::endl;
	}
	//运行网络
	RunNetWork();
	program_start_time=getCurrentTime();

	PoV_runner=new std::thread(std::bind(&blockchain_network::run,this));
	PoV_runner->detach();
}

void blockchain_network::run()
{//运行主线程，循环执行节点发现协议以及POV算法
	while(1)
	{
		if(stop_time>0)
		{
			if(getCurrentTime()>stop_time+program_start_time)
			{
				std::cout<<"CurrentTime: "<<getCurrentTime()<<"\n";
				std::cout<<"stop_time: "<<stop_time<<"\n";
				std::cout<<"program_start_time: "<<program_start_time<<"\n";
				exit(0);
			}
		}
		mutex.lock();
		runDiscoverNode();
		if(NodeList.size()>=2)
		{
			pov->run();
		}
		mutex.unlock();
		sleep(interval);
	}
}

void blockchain_network::SendPacket(rapidjson::Document& message, std::string ip,uint32_t port)
{
	//发送json数据给指定的地址和端口
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	message.Accept(writer);
	std::string content= buffer.GetString();
	//根据数据量大小选择使用TCP还是UDP进行发送
	//if(content.size()>1000000)
		SendMessageTCP(0, ip, port,content);
	//else
		//SendMessageUDP(0, ip, port,content);
}

int blockchain_network::OnReceiveMessage(const char * pcMessage, const int iMessageLen)
{
	//处理接受到的消息，如果数pov的消息，转换成json对象后交由pov的消息处理函数进行进一步处理
	std::unique_lock<std::mutex> lock(mutex);
	std::string message(pcMessage,iMessageLen);
	rapidjson::Document d;
	d.Parse(pcMessage,iMessageLen);
	//节点发现协议在网络层处理
    if(!d.IsObject())
    {
    	std::cout<<"d.IsObject()!\n";
    	return -1;
    }
    if(d.HasMember("FIND_NODE"))
    {
    	handleDiscoverNode(d);
    }
    else if(d.HasMember("log"))
    {	//接收到外部发送来的日志由pov的handleLog函数进行处理
    	std::cout<<"接收到日志信息：\n";
    	print_document(d);
    	pov->handleLog(d);
    }
    else
    {
    	//检查消息合法性
    	if(!d.HasMember("sender") or !d.HasMember("index") or !d.HasMember("receiver"))
    	{

    		std::cout<<"incomplete document!\n";
    		return -2;
    	}
    	NodeID sender=d["sender"].GetUint64();
    	NodeID index=d["index"].GetUint64();

    	for(std::vector<cache>::const_iterator msg=msgCache.begin();msg !=msgCache.end();++msg)
    	{
    		if(sender==msg->sender and index==msg->index)
    		{
    			return -3;
    		}
    	}
    	cache c;
    	c.index=index;
    	c.sender=sender;
    	if(msgCache.size()>4000)
    		msgCache.erase(msgCache.begin());
    	msgCache.push_back(c);

    	//转发处理
    	NodeID _receiver=d["receiver"].GetUint64();
    	if(_receiver==0)
    	{
    		forwarding(d);
    		pov->handleMessage(d);
    	}
    	else
    	{
    		if(_receiver!=getNodeId())
    		{
    			forwarding(d);
    			return -4;
    		}
    		pov->handleMessage(d);
    	}
    }
	return 0;
}

NodeID blockchain_network::getNodeId()
{
	return my_nodeinfo.id;
}

NodeID blockchain_network::getNodeId(std::string ip,uint32_t port)
{
	NodeID id=IPToValue(ip);
	id=id<<32;
	id+=port;
	return id;
}

uint32_t blockchain_network::IPToValue(const string& strIP)
{
//IP转化为数值
//没有格式检查
//返回值就是结果

    int a[4];
    string IP = strIP;
    string strTemp;
    size_t pos;
    size_t i=3;

    do
    {
        pos = IP.find(".");

        if(pos != string::npos)
        {
            strTemp = IP.substr(0,pos);
            a[i] = atoi(strTemp.c_str());
            i--;
            IP.erase(0,pos+1);
        }
        else
        {
            strTemp = IP;
            a[i] = atoi(strTemp.c_str());
            break;
        }

    }while(1);

    int nResult = (a[3]<<24) + (a[2]<<16)+ (a[1]<<8) + a[0];
    return nResult;
}

//节点发现协议，由sendDiscoverNode和handleDiscoverNode两个函数组成，每个节点维护一个节点列表，该列表
//不包含本地节点。发送方先查找列表，如果列表中没有节点，则向DNS节点发送节点发现请求，否则随机从列表中随机挑
//选一个节点发送节点发送请求。接受到请求的节点则把自己节点列表中的所有节点发送给请求方，然后把请求节点加入到
//节点列表中。请求方把被请求方加入自己的节点列表，随后对比本地节点列表和接收到的节点列表，把本地列表中没有的
//节点添加到列表中，并向这些节点发送节点发现请求。
void blockchain_network::runDiscoverNode()
{
	double current_time=getCurrentTime();
	if(current_time-last_discover_node_time<discover_node_period)
	{
		return;
	}
	last_discover_node_time=current_time;
	if(NodeList.empty())
	{
		sendDiscoverNode(DNS_IP,DNS_PORT,true);
	}
	else
	{
		for(std::map<NodeID,NodeInfo>::const_iterator i=NodeList.begin();i!=NodeList.end();i++)
		{
			NodeInfo node=i->second;
			sendDiscoverNode(node.ip,node.port,true);
		}
	}
}

void blockchain_network::sendDiscoverNode(std::string receiver_ip,uint32_t receiver_port,bool is_request)
{
	//有两种发送情况，第一种是发送请求获取其他节点的节点列表，第二种是回复请求，把节点列表发送给请求节点
	if(is_request)
	{
		rapidjson::Document d;
		d.SetObject();
		rapidjson::Document::AllocatorType &allocator=d.GetAllocator();
		rapidjson::Value IntValue(0);
		rapidjson::Value receiver(getNodeId(receiver_ip,receiver_port));
		std::string myip=my_nodeinfo.ip;
		rapidjson::Value sender_ip(myip.c_str(),myip.size(),d.GetAllocator());
		rapidjson::Value sender_port(my_nodeinfo.port);
		rapidjson::Value sender_id(my_nodeinfo.id);
		d.AddMember("FIND_NODE",IntValue,allocator);
		d.AddMember("receiver",receiver,allocator);
		d.AddMember("sender_ip",sender_ip,allocator);
		d.AddMember("sender_port",sender_port,allocator);
		d.AddMember("sender_id",sender_id,allocator);
		SendPacket(d,receiver_ip,receiver_port);
	}
	else
	{
		rapidjson::Document d;
		d.SetObject();
		rapidjson::Document::AllocatorType &allocator=d.GetAllocator();
		rapidjson::Value IntValue(1);
		rapidjson::Value receiver(getNodeId(receiver_ip,receiver_port));
		std::string myip=my_nodeinfo.ip;
		rapidjson::Value sender_ip(myip.c_str(),myip.size(),d.GetAllocator());
		rapidjson::Value sender_port(my_nodeinfo.port);
		rapidjson::Value sender_id(my_nodeinfo.id);
		d.AddMember("FIND_NODE",IntValue,allocator);
		d.AddMember("receiver",receiver,allocator);
		d.AddMember("sender_ip",sender_ip,allocator);
		d.AddMember("sender_port",sender_port,allocator);
		d.AddMember("sender_id",sender_id,allocator);
		//发送节点列表
		rapidjson::Value node_list;
		node_list.SetArray();
		for(std::map<NodeID,NodeInfo>::const_iterator i=NodeList.begin();i!=NodeList.end();i++)
		{
			NodeInfo node=i->second;
			rapidjson::Value nodeinfo;
			nodeinfo.SetObject();
			rapidjson::Value node_ip(node.ip.c_str(),node.ip.size(),allocator);
			rapidjson::Value node_port(node.port);
			rapidjson::Value node_id(node.id);
			nodeinfo.AddMember("node_ip",node_ip,allocator);
			nodeinfo.AddMember("node_port",node_port,allocator);
			nodeinfo.AddMember("node_id",node_id,allocator);
			node_list.PushBack(nodeinfo,allocator);
		}
		d.AddMember("node_list",node_list,allocator);
		SendPacket(d,receiver_ip,receiver_port);
	}
}

void blockchain_network::handleDiscoverNode(rapidjson::Document& d)
{	//节点发送协议的消息处理
	if(!(d.HasMember("FIND_NODE")&&d.HasMember("sender_ip")&&d.HasMember("sender_port")&&d.HasMember("sender_id")))
	{
		return;
	}
	uint32_t type=d["FIND_NODE"].GetUint();
	std::string sender_ip=d["sender_ip"].GetString();
	uint32_t sender_port=d["sender_port"].GetUint();
	if(!d["sender_id"].IsUint64())
	{
		std::cout<<"sender_id!=uint64!\n";
		return;
	}
	NodeID sender_id=d["sender_id"].GetUint64();
	if(sender_id==my_nodeinfo.id)
		return;
	NodeInfo node;
	node.ip=sender_ip;
	node.port=sender_port;
	node.id=sender_id;
	NodeList[sender_id]=node;
	//相应的接受到的消息也有两种，FIND_NODE的value用来区分这两种消息，type=0表示接受到节点列表请求，否则表示接收到回复
	if(type==0)
	{
		sendDiscoverNode(sender_ip,sender_port,false);
	}
	else
	{
		rapidjson::Value& node_list=d["node_list"];
		for(uint32_t i=0;i<node_list.Size();i++)
		{
			rapidjson::Value& node=node_list[i];
			if(!node["node_id"].IsUint64())
			{
				std::cout<<"node_id!=uint64!\n";
				return;
			}
			NodeID node_id=node["node_id"].GetUint64();
			if(node_id!=my_nodeinfo.id && NodeList.find(node_id)==NodeList.end())
			{
				NodeInfo nodeinfo;
				nodeinfo.ip=node["node_ip"].GetString();
				nodeinfo.port=node["node_port"].GetUint();
				nodeinfo.id=node_id;
				NodeList[node_id]=nodeinfo;
				sendDiscoverNode(nodeinfo.ip,nodeinfo.port,true);
			}
		}
		std::cout<<"节点列表中的节点：\n";
		for(std::map<NodeID,NodeInfo>::iterator i=NodeList.begin();i!=NodeList.end();i++)
		{
			std::cout<<i->first<<"——ip="<<i->second.ip<<" && port="<<i->second.port<<"\n";
		}
	}
}

void blockchain_network::SendToAll(rapidjson::Document &d)
{
	//发送给相邻节点
	for(std::map<NodeID,NodeInfo>::const_iterator i=NodeList.begin();i!=NodeList.end();i++)
	{
		NodeInfo nodeinfo=i->second;
		SendPacket(d,nodeinfo.ip,nodeinfo.port);
	}
	//发送给自己
	//SendPacket(d,m_peersSockets[*m_ipv4]);
	SendPacket(d,my_nodeinfo.ip,my_nodeinfo.port);
}

void blockchain_network::SendToNeighbor(rapidjson::Document &d)
{
	//发送给相邻节点
	for (std::map<NodeID,NodeInfo>::const_iterator i=NodeList.begin();i!=NodeList.end();i++)
	{
		NodeInfo nodeinfo=i->second;
		SendPacket(d,nodeinfo.ip,nodeinfo.port);
	}
}

void blockchain_network::forwarding(rapidjson::Document &d)
{	//消息转发
	if(!d.HasMember("receiver") or !d.HasMember("sender") or !d.HasMember("last_sender"))
	{
		std::cout<<"error: document is not complete!";
		return;
	}
	NodeID label=d["receiver"].GetUint64();
	NodeID sender=d["sender"].GetUint64();
	NodeID last_sender=d["last_sender"].GetUint64();
	if(label==0)
	{
		//如果是发送给所有人，则发送给除了发送者和上个发送者以外的所有人
		for (std::map<NodeID,NodeInfo>::const_iterator i=NodeList.begin();i!=NodeList.end();i++)
		{
			if(i->first==sender or i->first==last_sender)
			{
				continue;
			}
			SendPacket(d,i->second.ip,i->second.port);
		}
	}
	else
	{
		//如果发送给某个节点，就先查找相邻节点有没有该节点,有就直接发送，没有发送给除了发送者和上个发送者以外的所有人
		NodeID receiver=d["receiver"].GetUint64();
		for (std::map<NodeID,NodeInfo>::const_iterator i=NodeList.begin();i!=NodeList.end();i++)
		{
			if(i->first==receiver)
			{
				SendPacket(d,i->second.ip,i->second.port);
				//std::cout<<"find=true\n";
				return;
			}
		}
		for (std::map<NodeID,NodeInfo>::const_iterator i=NodeList.begin();i!=NodeList.end();i++)
		{
			if(i->first==sender or i->first==last_sender)
			{
				continue;
			}
			SendPacket(d,i->second.ip,i->second.port);
		}
	}
}

void blockchain_network::SendToOne(rapidjson::Document &d,NodeID receiver)
{	//发送给指定节点
	if(!d.HasMember("receiver") or !d.HasMember("sender") or !d.HasMember("last_sender"))
	{
		std::cout<<"error: document is not complete!";
		return;
	}
	//先查找相邻节点有没有目标节点，没有就发送给所有相邻节点。
	if(receiver==my_nodeinfo.id)
	{
		SendPacket(d,my_nodeinfo.ip,my_nodeinfo.port);
		return;
	}
	for (std::map<NodeID,NodeInfo>::const_iterator i=NodeList.begin();i!=NodeList.end();i++)
	{
		if(i->first==receiver)
		{
			SendPacket(d,i->second.ip,i->second.port);
			return;
		}
	}
	SendToNeighbor(d);
}

void blockchain_network::SendMessage(rapidjson::Document &d)
{
	MessageType msg_type=MessageType(d["type"].GetUint());
	//如果一个消息需要有receiver、sender、last_sender用来做路由，这几个字段都用nodeid来来表示一个节点，包括这个函数在内的所有上层函数都使用nodeid而不是用address。
	if(!d.HasMember("receiver"))
	{
		return;
	}

	rapidjson::Document::AllocatorType &allocator=d.GetAllocator();
	//设置"sender"字段
	if(!d.HasMember("sender"))
	{
		rapidjson::Value sender(getNodeId());
		d.AddMember("sender",sender,allocator);
	}
	else
	{
		d["sender"].SetUint64(getNodeId());
	}
	//设置"last_sender"字段
	if(!d.HasMember("last_sender"))
	{
		rapidjson::Value lastsender(getNodeId());
		d.AddMember("last_sender",lastsender,allocator);
	}
	else
	{
		d["last_sender"].SetUint64(getNodeId());
	}
	//发送策略
	NodeID receiver=d["receiver"].GetUint64();
	if(receiver==0)
	{
		SendToAll(d);
		return;
	}
	else
	{
		NodeID true_receiver=d["receiver"].GetUint64();
		SendToOne(d,true_receiver);
	}
}

void blockchain_network::setPoVConfig(std::string file)
{
	pov->setConfig(file);
}

void blockchain_network::setNetworkConfig(std::string file)
{	//从配置文件中读取网络配置
	std::cout<<"配置网络参数：\n";
    ConfigHelper configSettings(file);
	std::cout<<"配置网络参数：start\n";
    interval=configSettings.Read("运行时间间隔", interval);
	std::cout<<"interval："<<interval<<"\n";
    DNS_IP=configSettings.Read("默认节点地址", DNS_IP);
	std::cout<<"DNS_IP："<<DNS_IP<<"\n";
    DNS_PORT=configSettings.Read("默认节点端口", DNS_PORT);
	std::cout<<"DNS_PORT："<<DNS_PORT<<"\n";
    stop_time=configSettings.Read("停止时间", stop_time);
	std::cout<<"stop_time："<<stop_time<<"\n";
}

std::map<NodeID,NodeInfo>& blockchain_network::getNodeList()
{
	return NodeList;
}

double blockchain_network::getStartTime()
{
	return start_time;
}

double blockchain_network::getInterval()
{
	return stop_time;
}

double blockchain_network::getSystemStartTime()
{
	return program_start_time;
}
