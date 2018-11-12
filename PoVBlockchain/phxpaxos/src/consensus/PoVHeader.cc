/*
 * PoVHeader.cc
 *
 *  Created on: 2018年4月9日
 *      Author: blackguess
 */

#include "PoVHeader.h"

PoVHeader::PoVHeader() {
	// TODO Auto-generated constructor stub
	init();
}

PoVHeader::~PoVHeader() {
	// TODO Auto-generated destructor stub
}

void PoVHeader::init()
{
	height=0;
	num_of_trans=0;
	generator="";
	previous_hash="";
	Te=0;
	cycles=0;
	next_butler=0;
	merkle_root="";
	sigs_data="";
	hash="";

}

bool PoVHeader::setData(rapidjson::Value& v)
{
	if(!v.IsObject())
		return false;
	if(!v.HasMember("height"))
		return false;
	if(!v.HasMember("num_of_trans"))
		return false;
	if(!v.HasMember("generator"))
		return false;
	if(!v.HasMember("previous_hash"))
		return false;
	if(!v.HasMember("Te"))
		return false;
	if(!v.HasMember("cycles"))
		return false;
	if(!v.HasMember("next_butler"))
		return false;
	if(!v.HasMember("merkle_root"))
		return false;
	if(!v.HasMember("sigs"))
		return false;
	height=v["height"].GetInt();
	num_of_trans=v["num_of_trans"].GetInt();
	generator=v["generator"].GetString();
	previous_hash=v["previous_hash"].GetString();
	Te=v["Te"].GetDouble();
	cycles=v["cycles"].GetInt();
	next_butler=v["next_butler"].GetInt();
	merkle_root=v["merkle_root"].GetString();
	setSignatures(v["sigs"]);
	return true;
}

bool PoVHeader::setRawData(rapidjson::Value& v)
{
	init();
	if(!v.IsObject())
		return false;
	if(!v.HasMember("height"))
		return false;
	if(!v.HasMember("num_of_trans"))
		return false;
	if(!v.HasMember("generator"))
		return false;
	if(!v.HasMember("previous_hash"))
		return false;
	if(!v.HasMember("cycles"))
		return false;
	if(!v.HasMember("merkle_root"))
		return false;
	height=v["height"].GetInt();
	num_of_trans=v["num_of_trans"].GetInt();
	generator=v["generator"].GetString();
	previous_hash=v["previous_hash"].GetString();
	cycles=v["cycles"].GetInt();
	merkle_root=v["merkle_root"].GetString();
	return true;
}

rapidjson::Document& PoVHeader::getData()
{
	//顺序最好不要变,产生的区块头的字段顺序会影响最后签名
	rapidjson::Document &d=*(new rapidjson::Document);
	d.SetObject();
	rapidjson::Document::AllocatorType& allocator=d.GetAllocator();
	rapidjson::Value &_height=*(new rapidjson::Value(height));
	d.AddMember("height",_height,allocator);
	rapidjson::Value &_num_of_trans=*(new rapidjson::Value(num_of_trans));
	d.AddMember("num_of_trans",_num_of_trans,allocator);
	rapidjson::Value &_generator=*(new rapidjson::Value(generator.c_str(),generator.size(),allocator));
	d.AddMember("generator",_generator,allocator);
	rapidjson::Value &_previous_hash=*(new rapidjson::Value(previous_hash.c_str(),previous_hash.size(),allocator));
	d.AddMember("previous_hash",_previous_hash,allocator);
	rapidjson::Value &_cycles=*(new rapidjson::Value(cycles));
	d.AddMember("cycles",_cycles,allocator);
	rapidjson::Value &_merkle_root=*(new rapidjson::Value(merkle_root.c_str(),merkle_root.size(),allocator));
	d.AddMember("merkle_root",_merkle_root,allocator);
	d.AddMember("sigs",getSignatures(),allocator);
	rapidjson::Value &_next_butler=*(new rapidjson::Value(next_butler));
	d.AddMember("next_butler",_next_butler,allocator);
	rapidjson::Value &_Te=*(new rapidjson::Value(Te));
	d.AddMember("Te",_Te,allocator);
	return d;
}

//就是去掉Te、next_butler、sigs的区块头
rapidjson::Document& PoVHeader::getRawData()
{
	rapidjson::Document &d=*(new rapidjson::Document);
	d.SetObject();
	rapidjson::Document::AllocatorType& allocator=d.GetAllocator();
	rapidjson::Value &_height=*(new rapidjson::Value(height));
	d.AddMember("height",_height,allocator);
	rapidjson::Value &_num_of_trans=*(new rapidjson::Value(num_of_trans));
	d.AddMember("num_of_trans",_num_of_trans,allocator);
	rapidjson::Value &_generator=*(new rapidjson::Value(generator.c_str(),generator.size(),allocator));
	d.AddMember("generator",_generator,allocator);
	rapidjson::Value &_previous_hash=*(new rapidjson::Value(previous_hash.c_str(),previous_hash.size(),allocator));
	d.AddMember("previous_hash",_previous_hash,allocator);
	rapidjson::Value &_cycles=*(new rapidjson::Value(cycles));
	d.AddMember("cycles",_cycles,allocator);
	rapidjson::Value &_merkle_root=*(new rapidjson::Value(merkle_root.c_str(),merkle_root.size(),allocator));
	d.AddMember("merkle_root",_merkle_root,allocator);
	rapidjson::Value &_Te=*(new rapidjson::Value(0.0));
	d.AddMember("Te",_Te,allocator);
	//d.AddMember("sigs",getSignatures(),allocator);
	//rapidjson::Value &_next_butler=*(new rapidjson::Value(next_butler));
	//d.AddMember("next_butler",_next_butler,allocator);
	return d;
}

void PoVHeader::setHeight(uint32_t h)
{
	height=h;
}
uint32_t PoVHeader::getHeight()
{
	return height;
}
void PoVHeader::setNumOfTrans(uint32_t num)
{
	num_of_trans=num;
}
uint32_t PoVHeader::getNumOfTrans()
{
	return num_of_trans;
}
void PoVHeader::setGenerator(std::string gen)
{
	generator=gen;
}
std::string PoVHeader::getGenerator()
{
	return generator;
}
void PoVHeader::setPreviousHash(std::string pre_hash)
{
	previous_hash=pre_hash;
}
std::string PoVHeader::getPreviousHash()
{
	return previous_hash;
}
void PoVHeader::setTe(double T)
{
	Te=T;
}
double PoVHeader::getTe()
{
	return Te;
}
void PoVHeader::setCycles(uint32_t num)
{
	cycles=num;
}
uint32_t PoVHeader::getCycles()
{
	return cycles;
}
void PoVHeader::setMerkleRoot(std::string root)
{
	merkle_root=root;
}
std::string PoVHeader::getMerkleRoot()
{
	return merkle_root;
}
void PoVHeader::setSignatures(rapidjson::Value& sigs)
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	sigs.Accept(writer);
	sigs_data=buffer.GetString();
}

void PoVHeader::setSignatures(std::vector<signature>& sigs)
{
	rapidjson::Document& d=*(new rapidjson::Document);
	d.SetArray();
	rapidjson::Document::AllocatorType& allocator=d.GetAllocator();
	for(std::vector<signature>::iterator i=sigs.begin();i!=sigs.end();i++)
	{
		rapidjson::Value& sig_value=*(new rapidjson::Value(rapidjson::kObjectType));
		std::string pubkey=i->pubkey;
		std::string sig=i->sig;
		double timestamp=i->timestamp;
		rapidjson::Value& _pubkey=*(new rapidjson::Value(pubkey.c_str(),pubkey.size(),allocator));
		rapidjson::Value& _sig=*(new rapidjson::Value(sig.c_str(),sig.size(),allocator));
		//rapidjson::Value& _timestamp(timestamp);
		sig_value.AddMember("pubkey",_pubkey,allocator);
		sig_value.AddMember("sig",_sig,allocator);
		sig_value.AddMember("timestamp",timestamp,allocator);
		d.PushBack(sig_value,allocator);
	}
	setSignatures(d);
	delete &d;
}

rapidjson::Document& PoVHeader::getSignatures()
{
	rapidjson::Document &d=*(new rapidjson::Document);
	d.Parse(sigs_data.c_str(),sigs_data.size());
	return d;
}
void PoVHeader::setHash(std::string h)
{
	hash=h;
}
std::string PoVHeader::getHash()
{
	return hash;
}

void PoVHeader::setNextButler(uint32_t i)
{
	next_butler=i;
}
uint32_t PoVHeader::getNextButler()
{
	return next_butler;
}

