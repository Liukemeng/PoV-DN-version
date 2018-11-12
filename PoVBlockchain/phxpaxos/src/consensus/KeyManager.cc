/*
 * KeyManager.cc
 *
 *  Created on: 2018年3月27日
 *      Author: blackguess
 */

#include "KeyManager.h"


KeyManager::KeyManager() {
	// TODO Auto-generated constructor stub

}

KeyManager::~KeyManager() {
	// TODO Auto-generated destructor stub
}
void KeyManager::init(){
    mbedtls_ecdsa_init( &ctx_sign );
    mbedtls_ecdsa_init( &ctx_verify );
    mbedtls_ctr_drbg_init( &ctr_drbg );
    mbedtls_entropy_init( &entropy );
    pers="ecdsa";
    radix=16;
} /* namespace ns3 */

int KeyManager::setKeyPair()
{
	int ret=mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy,
            (const unsigned char *) pers,
            strlen( pers ) );
	ret = mbedtls_ecdsa_genkey( &ctx_sign, MBEDTLS_ECP_DP_SECP256K1,
	                              mbedtls_ctr_drbg_random, &ctr_drbg );
	//复制ctx_sign的椭圆曲线基点grp和公钥Q到ctx_verify
	ret = mbedtls_ecp_group_copy( &ctx_verify.grp, &ctx_sign.grp );
	ret = mbedtls_ecp_copy( &ctx_verify.Q, &ctx_sign.Q );
	return ret;
}

std::string KeyManager::getHash256(std::string message)
{
	//conunsigned char *content=new unsigned char[message.size()];
	const unsigned char *content=(const unsigned char*)message.c_str();
	unsigned char *hash=new unsigned char[32];
	mbedtls_sha256_ret( content, message.size(), hash, 0 );
	std::string hash_str=Hex2Str(hash,32);
	//delete[] content;
	delete[] hash;
	return hash_str;
}

std::string KeyManager::getHash256(rapidjson::Value& v)
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	v.Accept(writer);
	std::string str_buffer=buffer.GetString();
	std::string hash=getHash256(str_buffer);
	return hash;
}

std::string KeyManager::sign(std::string hash)
{
	unsigned char *hash_char=new unsigned char[hash.size()/2];
	size_t hash_size;
	Str2Hex(hash,hash_char,&hash_size);
	unsigned char *sig=new unsigned char[MBEDTLS_ECDSA_MAX_LEN];
	size_t sig_len;
	mbedtls_ecdsa_write_signature( &ctx_sign, MBEDTLS_MD_SHA256,
            hash_char, hash_size,
            sig, &sig_len,
            mbedtls_ctr_drbg_random, &ctr_drbg );
	std::string sig_str=Hex2Str(sig,sig_len);
	delete[] hash_char;
	delete[] sig;
	return sig_str;
}

std::string KeyManager::signDocument(rapidjson::Document& d)
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	d.Accept(writer);
	std::string str_buffer=buffer.GetString();
	std::string hash=getHash256(str_buffer);
	std::string sig=sign(hash);
	return sig;
}

std::string KeyManager::signDocument(rapidjson::Value& d)
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	d.Accept(writer);
	std::string str_buffer=buffer.GetString();
	std::string hash=getHash256(str_buffer);
	std::string sig=sign(hash);
	return sig;
}

bool KeyManager::verify(std::string hash,std::string sig)
{

	unsigned char *hash_char=new unsigned char[hash.size()/2];
	size_t hash_size;
	Str2Hex(hash,hash_char,&hash_size);
	//test
	/*
	std::string test=Hex2Str(hash_char,hash_size);
	std::cout<<test<<"\n";

	unsigned char *test_hash_char=new unsigned char[test.size()/2];
	size_t test_hash_size;
	Str2Hex(test,test_hash_char,&test_hash_size);

	std::cout<<"test_hash_size==hash_size:"<<(test_hash_size==hash_size)<<"\n";
	bool equal=true;
	for(size_t i=0;i<hash_size;i++)
	{
		if(hash_char[i]!=test_hash_char[i])
		{
			equal=false;
		}
	}
	std::cout<<"hash_char==test_hash_char:"<<equal<<"\n";
	*/

	unsigned char *sig_char=new unsigned char[sig.size()/2];
	size_t sig_size;
	Str2Hex(sig,sig_char,&sig_size);

	int ret = mbedtls_ecdsa_read_signature( &ctx_verify,
            hash_char, hash_size,
            sig_char, sig_size );
	//std::cout<<"error code:"<<ret<<"\n";
	/*
	 * MBEDTLS_ERR_ECP_ALLOC_FAILED  -0x4D80  -19840	Memory allocation failed.
	 * MBEDTLS_ERR_ECP_BAD_INPUT_DATA   -0x4F80  -20352		Bad input parameters to function.
	 * MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL   -0x4F00  -20224	The buffer is too small to write to.
	 * MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE   -0x4E80  -20096	Requested curve not available.
	 * MBEDTLS_ERR_ECP_HW_ACCEL_FAILED   -0x4B80  -19328	ECP hardware accelerator failed.
	 * MBEDTLS_ERR_ECP_INVALID_KEY   -0x4C80  -19584	Invalid private or public key.
	 * MBEDTLS_ERR_ECP_RANDOM_FAILED   -0x4D00  -19712	Generation of random value, such as (ephemeral) key, failed.
	 * MBEDTLS_ERR_ECP_SIG_LEN_MISMATCH   -0x4C00  -19456	Signature is valid but shorter than the user-supplied length.
	 * MBEDTLS_ERR_ECP_VERIFY_FAILED   -0x4E00  -19968	The signature is not valid.
	 */
	delete[] hash_char;
	delete[] sig_char;
	if(ret !=0)
		return false;
	return true;
}

bool KeyManager::verifyDocument(rapidjson::Document& d,std::string pubkey, std::string sig)
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	d.Accept(writer);
	std::string str_buffer=buffer.GetString();
	std::string hash=getHash256(str_buffer);
	return verify(pubkey,hash,sig);
}

void KeyManager::setRingParam(std::vector<std::string> public_keys,std::string private_key,std::string ring_param)
{
	this->ring_pubkeys=public_keys;
	this->ring_prikey=private_key;
	this->ring_param=ring_param;
}

std::string KeyManager::ring_signDocument(rapidjson::Value& d)
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	d.Accept(writer);
	std::string str_buffer=buffer.GetString();
	std::string hash=getHash256(str_buffer);
	std::string sig;
	lrsi.linkable_ring_sig(sig,hash,ring_pubkeys,ring_prikey,ring_param);
	return sig;
}

bool KeyManager::ring_verifyDocument(rapidjson::Value& d,std::string sig)
{
	bool result=false;
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	d.Accept(writer);
	std::string str_buffer=buffer.GetString();
	std::string hash=getHash256(str_buffer);
	lrsi.linkable_ring_verify(result,sig,hash,ring_param);
	return result;
}

std::string KeyManager::group_signDocument(rapidjson::Value& d,
        const std::string& algorithm_method,
        const std::string& gpk_info,
        const std::string& gsk_info,
        const std::string& pbc_param_info)
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	d.Accept(writer);
	std::string str_buffer=buffer.GetString();
	std::string hash=getHash256(str_buffer);
	std::string sig;
	group_sig(sig,algorithm_method,
			gpk_info,gsk_info,pbc_param_info,hash);
	return sig;
}

bool KeyManager::group_verifyDocument(rapidjson::Value& d,std::string sig,
        const std::string& algorithm_method,
        const std::string& gpk_info,
        const std::string& pbc_param_info)
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	d.Accept(writer);
	std::string str_buffer=buffer.GetString();
	std::string hash=getHash256(str_buffer);
	int is_valid;
	group_verify(is_valid, sig,
			hash, algorithm_method, gpk_info, pbc_param_info);
	if(is_valid==1)
		return true;
	return false;
}

bool KeyManager::verifyDocument(rapidjson::Value& d,std::string pubkey, std::string sig)
{
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	d.Accept(writer);
	std::string str_buffer=buffer.GetString();
	std::string hash=getHash256(str_buffer);
	return verify(pubkey,hash,sig);
}

bool KeyManager::verify(std::string pubkey,std::string hash,std::string sig)
{
	mbedtls_ecdsa_context context=getContextFromPublicKey(pubkey);
	unsigned char *hash_char=new unsigned char[hash.size()/2];
	size_t hash_size;
	Str2Hex(hash,hash_char,&hash_size);
	//test
	/*
	std::string test=Hex2Str(hash_char,hash_size);
	std::cout<<test<<"\n";

	unsigned char *test_hash_char=new unsigned char[test.size()/2];
	size_t test_hash_size;
	Str2Hex(test,test_hash_char,&test_hash_size);

	std::cout<<"test_hash_size==hash_size:"<<(test_hash_size==hash_size)<<"\n";
	bool equal=true;
	for(size_t i=0;i<hash_size;i++)
	{
		if(hash_char[i]!=test_hash_char[i])
		{
			equal=false;
		}
	}
	std::cout<<"hash_char==test_hash_char:"<<equal<<"\n";
	*/

	unsigned char *sig_char=new unsigned char[sig.size()/2];
	size_t sig_size;
	Str2Hex(sig,sig_char,&sig_size);
	int ret = mbedtls_ecdsa_read_signature( &context,
            hash_char, hash_size,
            sig_char, sig_size );
	//std::cout<<"error code:"<<ret<<"\n";
	/*
	 * MBEDTLS_ERR_ECP_ALLOC_FAILED  -0x4D80  -19840	Memory allocation failed.
	 * MBEDTLS_ERR_ECP_BAD_INPUT_DATA   -0x4F80  -20352		Bad input parameters to function.
	 * MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL   -0x4F00  -20224	The buffer is too small to write to.
	 * MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE   -0x4E80  -20096	Requested curve not available.
	 * MBEDTLS_ERR_ECP_HW_ACCEL_FAILED   -0x4B80  -19328	ECP hardware accelerator failed.
	 * MBEDTLS_ERR_ECP_INVALID_KEY   -0x4C80  -19584	Invalid private or public key.
	 * MBEDTLS_ERR_ECP_RANDOM_FAILED   -0x4D00  -19712	Generation of random value, such as (ephemeral) key, failed.
	 * MBEDTLS_ERR_ECP_SIG_LEN_MISMATCH   -0x4C00  -19456	Signature is valid but shorter than the user-supplied length.
	 * MBEDTLS_ERR_ECP_VERIFY_FAILED   -0x4E00  -19968	The signature is not valid.
	 */
	delete[] hash_char;
	delete[] sig_char;
	if(ret !=0)
		return false;
	return true;
}

std::string KeyManager::getPublicKey()
{
	size_t olen;
	size_t buflen=100;
	unsigned char buf[buflen];
	//unsigned char *buf=new unsigned char[buflen];
	mbedtls_ecp_point_write_binary( &ctx_verify.grp, &ctx_verify.Q,
			MBEDTLS_ECP_PF_UNCOMPRESSED, &olen,
	                            buf, buflen );
	//std::cout<<"buflen="<<buflen<<"\n";
	//std::cout<<"olen="<<olen<<"\n";
	std::string ret=Hex2Str(buf,olen);
	return ret;
}

mbedtls_ecdsa_context KeyManager::getContextFromPublicKey(std::string pubkey)
{
	mbedtls_ecdsa_context context;
	mbedtls_ecdsa_init(&context);
	int ret=mbedtls_ecp_group_load( &context.grp, MBEDTLS_ECP_DP_SECP256K1 );
	if(ret!=0)
	{
		std::cout<<"mbedtls_ecp_group_load error code:"<<ret<<"\n";
	}
	unsigned char public_key[pubkey.size()/2];
	size_t pubkey_len;
	Str2Hex(pubkey,public_key,&pubkey_len);
	ret=mbedtls_ecp_point_read_binary( &context.grp, &context.Q,
			public_key, pubkey_len );
	if(ret!=0)
	{
		std::cout<<"getContext Wrong!\n";
	}
	return context;
}

std::string KeyManager::getPrivateKey()
{
	//int radix=16;
	size_t buflen=1024;
	size_t olen;
	char *buf=new char[buflen];
	int ret=mbedtls_mpi_write_string(&ctx_sign.d, radix,
                              buf, buflen, &olen);
	if(ret!=0)
	{
		std::cout<<"ret=:"<<ret<<"\n";
	}
	unsigned char *buf_unsigned=(unsigned char*)buf;

	std::string str=Hex2Str(buf_unsigned,olen);
	delete[] buf;
	//delete[] buf_unsigned;
	return str;
}

bool KeyManager::setPublicKey(std::string pubkey)
{
	unsigned char public_key[pubkey.size()/2];
	size_t pubkey_len;
	Str2Hex(pubkey,public_key,&pubkey_len);
	int ret=mbedtls_ecp_point_read_binary( &ctx_sign.grp, &ctx_sign.Q,
			public_key, pubkey_len );
	ret = mbedtls_ecp_copy( &ctx_verify.Q, &ctx_sign.Q );
	if(ret!=0)
		return false;
	return true;
}

bool KeyManager::setPrivateKey(std::string privkey)
{
	//int radix=16;
	size_t temp_len;
	unsigned char temp[privkey.size()/2];
	Str2Hex(privkey,temp,&temp_len);
	char *s=(char *)temp;
	int ret=mbedtls_mpi_read_string( &ctx_sign.d, radix, s );
	if(ret!=0)
	{
		std::cout<<"mbedtls_mpi_read_string result:"<<ret<<"\n";
		return false;
	}
	return true;
}

bool KeyManager::checkPublicKey(std::string pubkey)
{
	mbedtls_ecdsa_context context= getContextFromPublicKey(pubkey);
	size_t temp_len;
	unsigned char temp[pubkey.size()/2];
	Str2Hex(pubkey,temp,&temp_len);

	int ret=mbedtls_ecp_point_read_binary( &context.grp, &context.Q,
			temp, temp_len );
	if(ret!=0)
	{
		std::cout<<"checkPublicKey mbedtls_ecp_point_read_binary ret:"<<ret<<"\n";
	}
	ret=mbedtls_ecp_check_pubkey( &context.grp, &context.Q );
	if(ret!=0)
	{
		std::cout<<"checkPublicKey mbedtls_ecp_check_pubkey ret:"<<ret<<"\n";
		return false;
	}
	return true;
}

bool KeyManager::checkPrivateKey(std::string privkey)
{
	//构建context
	mbedtls_ecdsa_context context;
	mbedtls_ecdsa_init(&context);
	int ret=mbedtls_ecp_group_load( &context.grp, MBEDTLS_ECP_DP_SECP256K1 );
	if(ret!=0)
	{
		std::cout<<"mbedtls_ecp_group_load error code:"<<ret<<"\n";
	}
	//配置privkey
	//int radix=16;
	size_t temp_len;
	unsigned char temp[privkey.size()/2];
	Str2Hex(privkey,temp,&temp_len);
	char *s=(char *)temp;
	ret=mbedtls_mpi_read_string( &context.d, radix, s );
	//检验privkey 是不是在群中
	ret=mbedtls_ecp_check_privkey( &context.grp, &context.d );
	if(ret!=0)
	{
		std::cout<<"mbedtls_ecp_check_privkey error code:"<<ret<<"\n";
		return false;
	}
	return true;
}

std::string KeyManager::Hex2Str( unsigned char *hex, size_t hex_size)
{
	std::string str;
	for(size_t i=0;i<hex_size;i++)
	{
		str=str+"0123456789ABCDEF" [hex[i] / 16];
		str=str+"0123456789ABCDEF" [hex[i] % 16];
		//str[2*i]="0123456789ABCDEF" [hex[i] / 16];
		//str[2*i+1]="0123456789ABCDEF" [hex[i] % 16];
	}
	return str;
}



void KeyManager::Str2Hex( std::string str,unsigned char *hex,size_t *hex_size )
{
	size_t str_size=str.size();
	if(str_size%2==1)
	{
		std::cout<<"Char2Hex长度错误\n";
		return;
	}
	for(size_t i=0;i<str_size/2;i++)
	{
		if('0'<=str[2*i] and str[2*i]<='9')
			hex[i]=16*(str[2*i]-'0');
		else if('A'<=str[2*i] and str[2*i]<='F')
			hex[i]=16*(str[2*i]-'A'+10);
		else
		{
			std::cout<<"转换错误！\n";
			return;
		}

		if('0'<=str[2*i+1] and str[2*i+1]<='9')
			hex[i]=hex[i]+(str[2*i+1]-'0');
		else if('A'<=str[2*i+1] and str[2*i+1]<='F')
			hex[i]=hex[i]+(str[2*i+1]-'A'+10);
		else
		{
			std::cout<<"转换错误！\n";
			return;
		}
	}
	*hex_size=str_size/2;
}



