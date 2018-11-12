/*
 * KeyManager.h
 *
 *  Created on: 2018年3月27日
 *      Author: blackguess
 */

#ifndef KEYMANAGER_H
#define KEYMANAGER_H

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdio.h>
#define mbedtls_printf     printf
#endif

#if defined(MBEDTLS_ECDSA_C) && \
    defined(MBEDTLS_ENTROPY_C) && defined(MBEDTLS_CTR_DRBG_C)
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/ecdsa.h"
#include "mbedtls/sha256.h"
#include<iostream>
#include <string.h>
#endif
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "algorithm/RingSig.h"
#include "algorithm/GroupSig.h"
using namespace GroupSigApi;

class KeyManager {
public:
	KeyManager();
	virtual ~KeyManager();
	//初始化秘钥管理器
	void init();
	//随机生成秘钥对
	int setKeyPair();
	//对字符串进行哈希操作
	std::string getHash256(std::string message);
	//对json对象进行hash操作
	std::string getHash256(rapidjson::Value& v);
	//对字符串进行签名
	std::string sign(std::string hash);
	//验证签名
	bool verify(std::string hash,std::string sig);
	bool verify(std::string pubkey,std::string hash,std::string sig);
	//16进制转字符串
	std::string Hex2Str( unsigned char *hex, size_t hex_size);
	//字符串转16进制
	void Str2Hex( std::string str,unsigned char *hex,size_t *hex_size );
	//获取公钥
	std::string getPublicKey();
	//内部函数，获取context
	mbedtls_ecdsa_context getContextFromPublicKey(std::string pubkey);
	//获取私钥
	std::string getPrivateKey();
	//设置公钥
	bool setPublicKey(std::string pubkey);
	//设置私钥
	bool setPrivateKey(std::string privkey);
	//检查私钥合法性
	bool checkPrivateKey(std::string privkey);
	//检查公钥合法性
	bool checkPublicKey(std::string pubkey);
	//对json对象进行签名
	std::string signDocument(rapidjson::Document& d);
	std::string signDocument(rapidjson::Value& d);
	//对json对象的签名进行验证
	bool verifyDocument(rapidjson::Document& d,std::string pubkey, std::string sig);
	bool verifyDocument(rapidjson::Value& d,std::string pubkey, std::string sig);
	void setRingParam(std::vector<std::string> public_keys,std::string private_key,std::string ring_param);
	std::string ring_signDocument(rapidjson::Value& d);
	bool ring_verifyDocument(rapidjson::Value& d,std::string sig);
	std::string group_signDocument(rapidjson::Value& d,
	        const std::string& algorithm_method,
	        const std::string& gpk_info,
	        const std::string& gsk_info,
	        const std::string& pbc_param_info);
	bool group_verifyDocument(rapidjson::Value& d,std::string sig,
	        const std::string& algorithm_method,
	        const std::string& gpk_info,
	        const std::string& pbc_param_info);
private:
    mbedtls_ecdsa_context ctx_sign, ctx_verify;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    const char *pers;//个性化数据，用来获取个性化的随机seed
    int radix;
    LinkableRingSigImpl lrsi;
	std::vector<std::string> ring_pubkeys;
	std::string ring_prikey;
	std::string ring_param;
};


#endif /* SRC_APPLICATIONS_MODEL_CONSENSUS_KEYMANAGER_H_ */
