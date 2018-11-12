/*
 * utils.cc
 *
 *  Created on: 2018年3月23日
 *      Author: blackguess
 */
#include "utils.h"
//#include "simulator.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include<string>
#include<vector>

double getCurrentTime()//统一使用单位为秒来表示时间
{
	struct timeval tv;
	gettimeofday(&tv,NULL);
	double usec=tv.tv_usec / 1000000.0;
	double sec=tv.tv_sec;
	return sec+usec;
	//return Simulator::Now().ToDouble(Time::S);
}

double getRandomNum()
{
	double r=((double)rand())/RAND_MAX;
	return r;
}

std::string getRandomString(uint32_t len)
{
	char ch[len];
	for(uint32_t i=0;i<len;i++)
	{
		int r1=rand()%3;
		switch(r1)
		{
		case 0:
			ch[i]='A'+rand()%26;
			break;
		case 1:
			ch[i]='a'+rand()%26;
			break;
		case 2:
			ch[i]='0'+rand()%10;
			break;
		}
	}
	std::string str(ch,len);
	return str;
}

void print_document(rapidjson::Document& d)
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	d.Accept(writer);
	std::cout<<"Document info:"<<buffer.GetString()<<"\n";
}

void print_document(rapidjson::Value& d)
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	d.Accept(writer);
	std::cout<<"Document info:"<<buffer.GetString()<<"\n";
}

//字符串分割函数
std::vector<std::string> utils_split(std::string str,std::string pattern)
{
    std::string::size_type pos;
    std::vector<std::string> result;
    str+=pattern;//扩展字符串以方便操作
    int size=str.size();

    for(int i=0; i<size; i++)
    {
        pos=str.find(pattern,i);
        if(pos<size)
        {
            std::string s=str.substr(i,pos-i);
            result.push_back(s);
            i=pos+pattern.size()-1;
        }
    }
    return result;
}

std::string getDocumentString(rapidjson::Value& d)
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	d.Accept(writer);
	std::string str=buffer.GetString();
	return str;
}

long convertTime(std::string time)
{
	tm tm_;
	std::vector<std::string> segs=utils_split(time," ");
	for(int i=0;i<segs.size();i++)
	{
		std::cout<<i<<": "<<segs[i]<<"\n";
	}
	for(std::vector<std::string>::iterator i=segs.begin();i<segs.end();i++)
	{
		if(*i=="\0")
		{
			segs.erase(i);
			i--;
		}
	}
	for(int i=0;i<segs.size();i++)
	{
		std::cout<<i<<": "<<segs[i]<<"\n";
	}
	//设置星期
	if(segs[0]=="Sun")
	{
		tm_.tm_wday=0;
	}
	else if(segs[0]=="Mon")
	{
		tm_.tm_wday=1;
	}
	else if(segs[0]=="Tue")
	{
		tm_.tm_wday=2;
	}
	else if(segs[0]=="Wed")
	{
		tm_.tm_wday=3;
	}
	else if(segs[0]=="Thu")
	{
		tm_.tm_wday=4;
	}
	else if(segs[0]=="Fri")
	{
		tm_.tm_wday=5;
	}
	else if(segs[0]=="Sat")
	{
		tm_.tm_wday=6;
	}

	if(segs[1]=="Jan")
	{
		tm_.tm_mon=0;
	}
	else if(segs[1]=="Feb")
	{
		tm_.tm_mon=1;
	}
	else if(segs[1]=="Mar")
	{
		tm_.tm_mon=2;
	}
	else if(segs[1]=="Apr")
	{
		tm_.tm_mon=3;
	}
	else if(segs[1]=="May")
	{
		tm_.tm_mon=4;
	}
	else if(segs[1]=="Jun")
	{
		tm_.tm_mon=5;
	}
	else if(segs[1]=="Jul")
	{
		tm_.tm_mon=6;
	}
	else if(segs[1]=="Aug")
	{
		tm_.tm_mon=7;
	}
	else if(segs[1]=="Sep")
	{
		tm_.tm_mon=8;
	}
	else if(segs[1]=="Oct")
	{
		tm_.tm_mon=9;
	}
	else if(segs[1]=="Nov")
	{
		tm_.tm_mon=10;
	}
	else if(segs[1]=="Dec")
	{
		tm_.tm_mon=11;
	}
	//day
	tm_.tm_mday=atoi(segs[2].c_str());
	//hour：minute：second
	std::vector<std::string> daytimes=utils_split(segs[3],":");

	tm_.tm_hour=atoi(daytimes[0].c_str());
	tm_.tm_min=atoi(daytimes[1].c_str());
	tm_.tm_sec=atoi(daytimes[2].c_str());
	//int final=segs[4].find("n");
	//segs[4]=segs[4].substr(0,final);
	tm_.tm_year=atoi(segs[4].c_str())-1900;

    //设置月份
	time_t t=mktime(&tm_);
	std::string str(ctime(&t));
	std::cout<<"转换后的字符串："<<str<<"\n";
	return t;
}

/*
int main()
{
	std::string time="Wed Jun  16 19:46:59 2018";
	long digit_time=convertTime(time);
	std::cout<<"时间转换为："<<digit_time<<"\n";
	return 0;
}
*/
