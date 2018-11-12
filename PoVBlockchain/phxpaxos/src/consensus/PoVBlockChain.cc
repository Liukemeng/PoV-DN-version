/*
 * PoVBlockChain.cc
 *
 *  Created on: 2018年4月11日
 *      Author: blackguess
 */

#include "PoVBlockChain.h"
#include <thread>
#include "utils.h"
//#include <experimental/optional>


PoVBlockChain::PoVBlockChain() {
	// TODO Auto-generated constructor stub

}

PoVBlockChain::~PoVBlockChain() {
	// TODO Auto-generated destructor stub
}

void PoVBlockChain::setPubkey(std::string pubkey)
{
	this->pubkey=pubkey.substr(0,40);
}

bool PoVBlockChain::pushbackBlock(PoVBlock block)
{
	PoVHeader header=block.getPoVHeader();
	if(header.getHeight()!=getHeight()+1)
	{
		return false;
	}
	blockchain.push_back(block);
	return true;
}

PoVBlock PoVBlockChain::getBlock(uint32_t i)
{
	//return blockchain.at(i);
	return getBlockFromDatabase(i);
}

int PoVBlockChain::getHeight()
{
	/*
	uint32_t num=blockchain.size();
	if(num==0)
		return -1;
	return blockchain.at(num-1).getHeight();
	*/
	return queryHeight();
}

uint32_t PoVBlockChain::getAmout()
{
	//return blockchain.size();
	return queryHeight()+1;
}

bool PoVBlockChain::validateBlockChain()
{
	if(getHeight()>=0)
	{
		for(uint32_t i=0;i<blockchain.size()-1;i++)
		{
			if(getBlock(i).getHeight()!=getBlock(i+1).getHeight())
			{
				return false;
			}
		}
	}
	return true;
}

void PoVBlockChain::clear()
{
	blockchain.clear();
}

/*
void PoVBlockChain::setCollection(mongocxx::client &client,std::string pubkey)
{
	coll=client["blockchain"][pubkey];
}
*/
bool PoVBlockChain::pushbackBlockToDatabase(PoVBlock block)
{
	//std::cout<<"before get coll\n";
	//mongocxx::client c{mongocxx::uri{}};
	mongocxx::client client{mongocxx::uri{}};
	auto coll=client["blockchain"][pubkey];
	//std::cout<<"after get find\n";
	rapidjson::Document& doc=block.getBlock();
	uint32_t height=block.getHeight();
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	doc.Accept(writer);
	std::string json=buffer.GetString();
	bsoncxx::builder::stream::document temp_builder{};
	bsoncxx::document::value value=bsoncxx::from_json(json);
	temp_builder<<"height"<<static_cast<int>(height)
				<<"block"<<bsoncxx::builder::stream::open_document <<bsoncxx::builder::concatenate_doc{value.view()}<<bsoncxx::builder::stream::close_document;
	//std::cout<<"before find\n";
	mongocxx::stdx::optional<bsoncxx::document::value> result = coll.find_one(
			bsoncxx::builder::stream::document{}<<"height"<<static_cast<int>(height)
			<<bsoncxx::builder::stream::finalize);
	//std::cout<<"after find\n";
	delete &doc;
	if(!result)
	{
		//std::cout<<"before insert\n";
		//std::string tojson=bsoncxx::to_json(temp_builder.view());
		//std::cout<<"bson string:"<<tojson<<"\n";
		/*
		auto threadfunc = [](std::string pubkey,bsoncxx::builder::stream::document &doc) {
			std::cout<<"thread:1\n";
			mongocxx::client c{mongocxx::uri{}};
			std::cout<<"thread:2\n";
			auto coll=c["blockchain"][pubkey];
			std::cout<<"thread:3\n";
			//std::unique_lock<std::mutex> lock(mtx);
			coll.insert_one(doc.view());
			std::cout<<"thread:4\n";
		};
		std::thread t([&]() { threadfunc(pubkey,temp_builder);});
		t.detach();
		*/
		coll.insert_one(temp_builder.view());
		//bsoncxx::stdx::optional<mongocxx::result::insert_one> insert_result=coll.insert_one(temp_builder.view());
		//std::cout<<"after insert\n";
		return true;
	}
	std::cout<<"this block has existed!\n";
	return false;
	/*
	else
	{
		bsoncxx::document::view view = (*result).view();
		std::string oid=view["_id"].get_oid().value.to_string();

	}
	*/
}
/*
bool PoVBlockChain::updateBlockToDatabase(PoVBlock block)
{
	rapidjson::Document& doc=block.getBlock();
	uint32_t height=block.getHeight();
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	doc.Accept(writer);
	std::string json=buffer.GetString();
	bsoncxx::builder::stream::document temp_builder{};
	bsoncxx::document::value value=bsoncxx::from_json(json);
	stdx::optional<mongocxx::result::update> result = coll.update_one(bsoncxx::builder::stream::document{} << "height" << static_cast<int>(height) << bsoncxx::builder::stream::finalize,
							bsoncxx::builder::stream::document{} << "block" << bsoncxx::builder::stream::open_document <<
							bsoncxx::builder::concatenate_doc{value.view()}<< bsoncxx::builder::stream::close_document << bsoncxx::builder::stream::finalize);
	if(result)
	{
		return true;
	}
	return false;
}
*/
PoVBlock PoVBlockChain::getBlockFromDatabase(uint32_t height)
{
	mongocxx::client c{mongocxx::uri{}};
	auto coll=c["blockchain"][pubkey];
	PoVBlock block;
	mongocxx::stdx::optional<bsoncxx::document::value> result = coll.find_one(
			bsoncxx::builder::stream::document{}<<"height"<<static_cast<int>(height)
			<<bsoncxx::builder::stream::finalize);
	if(result)
	{
		bsoncxx::types::b_document document=(*result).view()["block"].get_document();
		bsoncxx::document::view view = document.view();
		std::string json=bsoncxx::to_json(view);
		rapidjson::Document doc;
		doc.Parse(json.c_str(),json.size());
		block.setBlock(doc);
		return block;
	}
	return block;
}
void PoVBlockChain::deleteBlockChainFromDatabase()
{
	mongocxx::client c{mongocxx::uri{}};
	auto coll=c["blockchain"][pubkey];
	coll.delete_many(bsoncxx::builder::stream::document{}<<bsoncxx::builder::stream::finalize);
	//if(result) {
	//  std::cout <<"删除区块数量:"<< result->deleted_count(bsoncxx::builder::stream::document{}<<bsoncxx::builder::stream::finalize) << "\n";
	//}
}
void PoVBlockChain::saveBlockChain()
{
	for(std::vector<PoVBlock>::const_iterator i=blockchain.begin();i<blockchain.end();i++)
	{
		bool result=pushbackBlockToDatabase(*i);
	}
}
void PoVBlockChain::loadBlockChain()
{
	mongocxx::client c{mongocxx::uri{}};
	auto coll=c["blockchain"][pubkey];
	blockchain.clear();
	int height=0;
	mongocxx::stdx::optional<bsoncxx::document::value> result = coll.find_one(
			bsoncxx::builder::stream::document{}<<"height"<<height
			<<bsoncxx::builder::stream::finalize);
	while(result)
	{
		std::string json=bsoncxx::to_json((*result).view());
		rapidjson::Document json_doc;
		json_doc.Parse(json.c_str(),json.size());
		rapidjson::Value& value=json_doc["block"];
		PoVBlock block;
		block.setBlock(value);
		blockchain.push_back(block);
		height++;
		result = coll.find_one(
					bsoncxx::builder::stream::document{}<<"height"<<height
					<<bsoncxx::builder::stream::finalize);
	}
}

int PoVBlockChain::queryHeight()
{
	mongocxx::client c{mongocxx::uri{}};
	auto coll=c["blockchain"][pubkey];
	mongocxx::cursor cursor = coll.find({});
	int height=-1;
	for(auto doc : cursor) {
	  //std::cout << bsoncxx::to_json(doc) << "\n";
	  rapidjson::Document d;
	  std::string json_doc=bsoncxx::to_json(doc);
	  d.Parse(json_doc.c_str(),json_doc.size());
	  if(d.IsObject()&&d.HasMember("height"))
	  {
		  int h=d["height"].GetInt();
		  if(h>height)
			  height=h;
	  }
	}
	return height;
}

rapidjson::Document& PoVBlockChain::queryLogByTime(long minT,long maxT)
{
	mongocxx::client c{mongocxx::uri{}};
	auto coll=c["blockchain"][pubkey];
	mongocxx::cursor cursor = coll.find({});
	int height=0;
	rapidjson::Document& log_array=*(new rapidjson::Document);
	log_array.SetArray();
	rapidjson::Document::AllocatorType& allocator=log_array.GetAllocator();
	for(auto doc : cursor) {
	  //std::cout << bsoncxx::to_json(doc) << "\n";
	  rapidjson::Document d;
	  std::string json_doc=bsoncxx::to_json(doc);
	  d.Parse(json_doc.c_str(),json_doc.size());
	  if(d.IsObject()&&d.HasMember("block"))
	  {
		  rapidjson::Value& txs=d["block"]["transactions"];
		  for(int i=0;i<txs.Size();i++)
		  {
			  //print_document(txs[i]);
			  if(txs[i].HasMember("metatype"))
			  {
				  if(!txs[i]["metatype"].IsString())
				  {
					  std::cout<<"metatype不是字符串\n";
					  //print_document(txs[i]["metatype"]);
				  }
				  std::string metatype=txs[i]["metatype"].GetString();
				  if(metatype=="Normal")
				  {
					  rapidjson::Value& content=txs[i]["content"];
					  if(content.HasMember("detail"))
					  {
						  if(content["detail"].IsObject()&& content["detail"].HasMember("Timestamp"))
						  {
							  //std::cout<<"遍历的日志：\n";
							  //print_document(content);
							  std::string timestamp=content["detail"]["Timestamp"].GetString();
							  int final=timestamp.find("\n");
							  timestamp=timestamp.substr(0,final);
							  long t=convertTime(timestamp);
							  if(t>=minT&&t<maxT)
							  {
								  rapidjson::Value& v=*(new rapidjson::Value);
								  v.CopyFrom(content,allocator);
								  log_array.PushBack(v,allocator);
							  }
							  else
							  {
								  //std::cout<<"minT="<<minT<<"\n";
								  //std::cout<<"maxT="<<maxT<<"\n";
								  //std::cout<<"t="<<t<<"\n";
								  std::cout<<"时间范围错误\n";
							  }
						  }//timestamp
						  else
						  {
							  std::cout<<"查询日志错误：没有timestamp\n";
						  }
					  }//detail
					  else
					  {
						  std::cout<<"查询日志错误：没有detail\n";
					  }
				  }//normal
				  else
				  {
					  std::cout<<"查询日志错误：没有normal\n";
				  }
			  }//metatype
			  else
			  {
				  std::cout<<"查询日志错误：没有metatype\n";
			  }
		  }//txs loop
	  }//block
	  else
	  {
		  std::cout<<"查询日志错误：没有block\n";
	  }
	}//blocks loop
	return log_array;
}

rapidjson::Document& PoVBlockChain::queryLogByErrorCode(std::string err_code)
{
	mongocxx::client c{mongocxx::uri{}};
	auto coll=c["blockchain"][pubkey];
	mongocxx::cursor cursor = coll.find({});
	int height=0;
	rapidjson::Document& log_array=*(new rapidjson::Document);
	log_array.SetArray();
	rapidjson::Document::AllocatorType& allocator=log_array.GetAllocator();
	for(auto doc : cursor) {
	  //std::cout << bsoncxx::to_json(doc) << "\n";
	  rapidjson::Document d;
	  std::string json_doc=bsoncxx::to_json(doc);
	  d.Parse(json_doc.c_str(),json_doc.size());
	  if(d.IsObject()&&d.HasMember("block"))
	  {
		  rapidjson::Value& txs=d["block"]["transactions"];
		  for(int i=0;i<txs.Size();i++)
		  {
			  print_document(txs[i]);
			  if(txs[i].HasMember("metatype"))
			  {
				  if(!txs[i]["metatype"].IsString())
				  {
					  std::cout<<"metatype不是字符串\n";
					  print_document(txs[i]["metatype"]);
				  }
				  std::string metatype=txs[i]["metatype"].GetString();
				  if(metatype=="Normal")
				  {
					  rapidjson::Value& content=txs[i]["content"];
					  if(content.HasMember("detail"))
					  {
						  if(content["detail"].IsObject()&& content["detail"].HasMember("errorCode"))
						  {
							  //std::cout<<"遍历的日志：\n";
							  //print_document(content);
							  std::string errorCode=content["detail"]["errorCode"].GetString();
							  if(errorCode==err_code)
							  {
								  rapidjson::Value& v=*(new rapidjson::Value);
								  v.CopyFrom(content,allocator);
								  log_array.PushBack(v,allocator);
							  }
							  else
							  {
								  std::cout<<"没有该时间短范围内的日志\n";
							  }
						  }//timestamp
						  else
						  {
							  std::cout<<"查询日志错误：没有errorCode\n";
						  }
					  }//detail
					  else
					  {
						  std::cout<<"查询日志错误：没有detail\n";
					  }
				  }//normal
				  else
				  {
					  std::cout<<"查询日志错误：没有normal\n";

				  }
			  }//metatype
			  else
			  {
				  std::cout<<"查询日志错误：没有metatype\n";
			  }
		  }//txs loop
	  }//block
	  else
	  {
		  std::cout<<"查询日志错误：没有block\n";
	  }
	}//blocks loop
	return log_array;
}
/*
rapidjson::Document& PoVBlockChain::queryBlock(int height)
{

}
*/

