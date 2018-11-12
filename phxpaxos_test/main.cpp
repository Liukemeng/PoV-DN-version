#include<stdio.h>
#include<iostream>
#include<string>

#include "consensus/blockchain-network.h"
#include "consensus/ConfigHelper.h"

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include "consensus/FISCO-BCOS-GR-Sig/devcore/easylogging++.h"

INITIALIZE_EASYLOGGINGPP;
using namespace phxpaxos;
using namespace std;


void run_single_process(int argc, char ** argv)
{
    mongocxx::instance inst{};

    std::mutex db1_mtx{};
    //mongocxx::client conn1{mongocxx::uri{}};
    //mongocxx::database db1 = conn1["blockchain"];
	blockchain_network dn;

	std::string file1(argv[1]);
	std::cout<<"读取文件地址为："<<file1<<"\n";
    ConfigHelper configSettings(file1);
    std::string ip1="";
    int port1=5000;
    ip1=configSettings.Read("本地地址", ip1);
    port1=configSettings.Read("本地端口", port1);
	dn.init(ip1,port1);
	dn.setNetworkConfig(file1);
	dn.setPoVConfig(file1);
	dn.StartApplication();
	while(1)
	{

	}
}

void run_multi_process()
{
    mongocxx::instance inst{};

    std::mutex db1_mtx{};
    //mongocxx::client conn1{mongocxx::uri{}};
    //mongocxx::database db1 = conn1["blockchain"];
	blockchain_network dn1;
	std::string file1="Config";
	std::cout<<"读取文件地址为："<<file1<<"\n";
    ConfigHelper configSettings1(file1);
    std::string ip1="";
    int port1=5000;
    ip1=configSettings1.Read("本地地址", ip1);
    port1=configSettings1.Read("本地端口", port1);
	dn1.init(ip1,port1);
	dn1.setNetworkConfig(file1);
	dn1.setPoVConfig(file1);
	dn1.StartApplication();

	blockchain_network dn2;
	std::string file2="Config1";
	std::cout<<"读取文件地址为："<<file2<<"\n";
    ConfigHelper configSettings2(file2);
    std::string ip2="";
    int port2=5000;
    ip2=configSettings2.Read("本地地址", ip2);
    port2=configSettings2.Read("本地端口", port2);
	dn2.init(ip2,port2);
	dn2.setNetworkConfig(file2);
	dn2.setPoVConfig(file2);
	dn2.StartApplication();

	blockchain_network dn3;
	std::string file3="Config2";
	std::cout<<"读取文件地址为："<<file3<<"\n";
    ConfigHelper configSettings3(file3);
    std::string ip3="";
    int port3=5000;
    ip3=configSettings3.Read("本地地址", ip3);
    port3=configSettings3.Read("本地端口", port3);
    dn3.init(ip3,port3);
    dn3.setNetworkConfig(file3);
    dn3.setPoVConfig(file3);
    dn3.StartApplication();
	while(1)
	{

	}
}

int main(int argc, char ** argv)
{
	el::Configurations conf("my_log.conf");
	el::Loggers::reconfigureAllLoggers(conf);

	LOG(TRACE)   << "***** trace log  *****";
	LOG(DEBUG)   << "***** debug log  *****";
	LOG(ERROR)   << "***** error log  *****";
	LOG(WARNING) << "***** warning log  *****";
	LOG(INFO)    << "***** info log  *****";

	//run_multi_process();
	run_single_process(argc, argv);
	/*
    std::mutex db2_mtx{};
	blockchain_network dn2;
    //mongocxx::client conn2{mongocxx::uri{}};
    //mongocxx::database db2 = conn1["blockchain"];
	dn2.init("127.0.0.1",5009);
	std::string file2="/home/blackguess/workspace/C++/phxpaxos_test/Debug/Config1";
	dn2.setNetworkConfig(file2);
	dn2.setPoVConfig(file2);
	dn2.StartApplication();

    std::mutex db3_mtx{};
    //mongocxx::client conn3{mongocxx::uri{}};
    //mongocxx::database db3 = conn1["blockchain"];
	blockchain_network dn3;
	dn3.init("127.0.0.1",5010);
	std::string file3="/home/blackguess/workspace/C++/phxpaxos_test/Debug/Config2";
	dn3.setNetworkConfig(file3);
	dn3.setPoVConfig(file3);
	dn3.StartApplication();
	*/

	return 0;
}

