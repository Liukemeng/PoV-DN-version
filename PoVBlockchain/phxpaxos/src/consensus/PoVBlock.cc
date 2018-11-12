/*
 * PoVBlock.cc
 *
 *  Created on: 2018年4月9日
 *      Author: blackguess
 */

#include "PoVBlock.h"
#include "utils.h"


PoVBlock::PoVBlock() {
	// TODO Auto-generated constructor stub

}

PoVBlock::~PoVBlock() {
	// TODO Auto-generated destructor stub
}

void PoVBlock::copyfrom(PoVBlock &p)
{
	rapidjson::Document &header_doc=p.getHeader();
	rapidjson::Document &txs_doc=p.getTransactionsList();
	setHeader(header_doc);
	setTransactions(txs_doc);
	delete& header_doc;
	delete& txs_doc;
}

void PoVBlock::setHeader(rapidjson::Value& v)
{
	header.setData(v);
}

void PoVBlock::setRawHeader(rapidjson::Value& v)
{
	header.setRawData(v);
}

void PoVBlock::setHeader(PoVHeader v)
{
	header=v;
}

rapidjson::Document& PoVBlock::getHeader()
{
	return header.getData();
}

rapidjson::Document& PoVBlock::getRawHeader()
{
	return header.getRawData();
}

PoVHeader PoVBlock::getPoVHeader()
{
	return header;
}

uint32_t PoVBlock::getHeight()
{
	return header.getHeight();
}

void PoVBlock::PushBackTransaction(rapidjson::Value& v)
{
	PoVTransaction transaction;
	transaction.setData(v);
	transactions.push_back(transaction);
}

void PoVBlock::setTransactions(rapidjson::Value& v)
{
	transactions.clear();
	if(!v.IsArray())
		return;
	for(uint32_t i=0;i<v.Size();i++)
	{
		PushBackTransaction(v[i]);
	}
}

rapidjson::Document& PoVBlock::getTransaction(uint32_t i)
{
	return transactions.at(i).getData();
}

rapidjson::Document& PoVBlock::getTransactionsList()
{


	rapidjson::Document& txlist=*(new rapidjson::Document(rapidjson::kArrayType));
	txlist.SetArray();
	rapidjson::Document::AllocatorType& allocator=txlist.GetAllocator();
	for(std::vector<PoVTransaction>::iterator i=transactions.begin();i!=transactions.end();i++)
	{
		rapidjson::Document& doc=i->getData();
		rapidjson::Value& tx=*(new rapidjson::Value);
		tx.CopyFrom(doc,allocator);
		txlist.PushBack(tx,allocator);
		delete &doc;
	}
	return txlist;
}

uint32_t PoVBlock::getTransactionsAmout()
{
	return transactions.size();
}

void PoVBlock::getConstantsTransactions(std::vector<PoVTransaction>& txs)
{
	for(std::vector<PoVTransaction>::iterator i=transactions.begin();i!=transactions.end();i++)
	{
		if(i->getMetaType()=="Constants")
			txs.push_back(*i);
	}
}

void PoVBlock::getApplyCommissionerTransactions(std::vector<PoVTransaction>& txs)
{
	for(std::vector<PoVTransaction>::iterator i=transactions.begin();i!=transactions.end();i++)
	{
		if(i->getMetaType()=="ApplyCommissioner")
			txs.push_back(*i);
	}
}

void PoVBlock::getApplyButlerCandidateTransactions(std::vector<PoVTransaction>& txs)
{
	for(std::vector<PoVTransaction>::iterator i=transactions.begin();i!=transactions.end();i++)
	{
		if(i->getMetaType()=="ApplyButlerCandidate")
			txs.push_back(*i);
	}
}

void PoVBlock::getVoteTransactions(std::vector<PoVTransaction>& txs)
{
	for(std::vector<PoVTransaction>::iterator i=transactions.begin();i!=transactions.end();i++)
	{
		if(i->getMetaType()=="vote")
			txs.push_back(*i);
	}
}

void PoVBlock::getNormalTransactions(std::vector<PoVTransaction>& txs)
{
	for(std::vector<PoVTransaction>::iterator i=transactions.begin();i!=transactions.end();i++)
	{
		if(i->getMetaType()=="Normal")
			txs.push_back(*i);
	}
}

void PoVBlock::getQuitButlerCandateTransactions(std::vector<PoVTransaction>& txs)
{
	for(std::vector<PoVTransaction>::iterator i=transactions.begin();i!=transactions.end();i++)
	{
		if(i->getMetaType()=="QuitButlerCandidate")
			txs.push_back(*i);
	}
}

void PoVBlock::getQuitCommissionerTransactions(std::vector<PoVTransaction>& txs)
{
	for(std::vector<PoVTransaction>::iterator i=transactions.begin();i!=transactions.end();i++)
	{
		if(i->getMetaType()=="QuitCommissioner")
			txs.push_back(*i);
	}
}

void PoVBlock::setRawBlock(rapidjson::Value& d)
{
	setRawHeader(d["header"]);
	setTransactions(d["transactions"]);
}

void PoVBlock::setBlock(rapidjson::Value& d)
{
	setHeader(d["header"]);
	setTransactions(d["transactions"]);
}

rapidjson::Document& PoVBlock::getRawBlock()
{
	rapidjson::Document& block=*(new rapidjson::Document);
	block.SetObject();
	rapidjson::Document::AllocatorType& allocator=block.GetAllocator();
	rapidjson::Document& header_doc=header.getRawData();
	rapidjson::Document& txs_doc=getTransactionsList();
	rapidjson::Value& header_value=*(new rapidjson::Value);
	rapidjson::Value& txs_value=*(new rapidjson::Value);
	header_value.CopyFrom(header_doc,allocator);
	txs_value.CopyFrom(txs_doc,allocator);
	block.AddMember("header",header_value,allocator);
	block.AddMember("transactions",getTransactionsList(),allocator);
	delete &header_doc;
	delete &txs_doc;
	return block;
}

rapidjson::Document& PoVBlock::getBlock()
{
	rapidjson::Document& block=*(new rapidjson::Document);
	block.SetObject();
	rapidjson::Document::AllocatorType& allocator=block.GetAllocator();
	rapidjson::Document& header_doc=header.getData();
	rapidjson::Document& txs_doc=getTransactionsList();
	rapidjson::Value& header_value=*(new rapidjson::Value);
	rapidjson::Value& txs_value=*(new rapidjson::Value);
	header_value.CopyFrom(header_doc,allocator);
	txs_value.CopyFrom(txs_doc,allocator);
	block.AddMember("header",header_value,allocator);
	block.AddMember("transactions",txs_value,allocator);
	delete &header_doc;
	delete &txs_doc;
	return block;
}

void PoVBlock::setSigs(rapidjson::Value& sigs)
{
	header.setSignatures(sigs);
}

void PoVBlock::setSigs(std::vector<signature>& sigs)
{
	header.setSignatures(sigs);
}

rapidjson::Document& PoVBlock::getSignatures()
{
	return header.getSignatures();
}

void PoVBlock::setHash(std::string h)
{
	header.setHash(h);
}

std::string PoVBlock::getHash()
{
	return header.getHash();
}

void PoVBlock::setPreHash(std::string h)
{
	header.setPreviousHash(h);
}

std::string PoVBlock::getPreHash()
{
	return header.getPreviousHash();
}

void PoVBlock::setTe(double T)
{
	header.setTe(T);
}

double PoVBlock::getTe()
{
	return header.getTe();
}

void PoVBlock::setNextButler(uint32_t i)
{
	header.setNextButler(i);
}

uint32_t PoVBlock::getNextButler()
{
	return header.getNextButler();
}

uint32_t PoVBlock::getCycles()
{
	return header.getCycles();
}


