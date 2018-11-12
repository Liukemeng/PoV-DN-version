SRC_BASE_PATH=/home/wzx/Code/cplusplus/PoV-DN-version/PoVBlockchain/phxpaxos

all:lib_node test_propose_batch_bin 

include $(SRC_BASE_PATH)/makefile.mk

include Makefile.define
NODE_SRC=$(NODE_OBJ)
NODE_INCS=$(sort $(SRC_BASE_PATH)/src/communicate $(SRC_BASE_PATH)/src/checkpoint $(SRC_BASE_PATH)/src/algorithm $(SRC_BASE_PATH)/src/master $(SRC_BASE_PATH)/src/communicate $(SRC_BASE_PATH)/src/logstorage $(LEVELDB_INCLUDE_PATH) $(SRC_BASE_PATH)/src/sm-base $(SRC_BASE_PATH)/src/config $(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)/src/comm $(PROTOBUF_INCLUDE_PATH) $(SRC_BASE_PATH)/src/communicate/tcp $(SRC_BASE_PATH)/src/consensus /usr/local/include/mongocxx/v_noabi /usr/local/include/bsoncxx/v_noabi $(SRC_BASE_PATH)/src/consensus/FISCO-BCOS-GR-Sig $(SRC_BASE_PATH)/src/consensus/FISCO-BCOS-GR-Sig/algorithm/bbs04 $(SRC_BASE_PATH)/src/consensus/FISCO-BCOS-GR-Sig/algorithm $(SRC_BASE_PATH)/src/consensus/FISCO-BCOS-GR-Sig/algorithm/ring-sig $(SRC_BASE_PATH)/src/node)
NODE_FULL_LIB_PATH=$(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)/src/comm $(SRC_BASE_PATH)/src/logstorage $(SRC_BASE_PATH)/src/sm-base $(SRC_BASE_PATH)/src/config $(SRC_BASE_PATH)/src/communicate/tcp $(SRC_BASE_PATH)/src/communicate $(SRC_BASE_PATH)/src/checkpoint $(SRC_BASE_PATH)/src/algorithm $(SRC_BASE_PATH)/src/master $(SRC_BASE_PATH)/src/consensus $(SRC_BASE_PATH)//src/node
NODE_EXTRA_CPPFLAGS=-Wno-unused-variable -Wno-sign-compare -Wno-unused-but-set-variable -Wno-unused-result -Wall -Werror

CPPFLAGS+=$(patsubst %,-I%, $(NODE_INCS))
CPPFLAGS+=$(NODE_EXTRA_CPPFLAGS)

lib_node:$(SRC_BASE_PATH)/.lib/libnode.a

$(SRC_BASE_PATH)/.lib/libnode.a: $(NODE_SRC)
	ar -cvq $@ $(NODE_SRC)

TEST_PROPOSE_BATCH_SRC=$(TEST_PROPOSE_BATCH_OBJ)
TEST_PROPOSE_BATCH_INCS=$(sort $(SRC_BASE_PATH)/src/checkpoint $(SRC_BASE_PATH)/src/algorithm $(SRC_BASE_PATH)/src/master $(SRC_BASE_PATH)/src/communicate $(SRC_BASE_PATH)/src/logstorage $(LEVELDB_INCLUDE_PATH) $(SRC_BASE_PATH)/src/sm-base $(SRC_BASE_PATH)/src/config $(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)/src/comm $(PROTOBUF_INCLUDE_PATH) $(SRC_BASE_PATH)/src/communicate/tcp $(SRC_BASE_PATH)/src/consensus /usr/local/include/mongocxx/v_noabi /usr/local/include/bsoncxx/v_noabi $(SRC_BASE_PATH)/src/consensus/FISCO-BCOS-GR-Sig $(SRC_BASE_PATH)/src/consensus/FISCO-BCOS-GR-Sig/algorithm/bbs04 $(SRC_BASE_PATH)/src/consensus/FISCO-BCOS-GR-Sig/algorithm $(SRC_BASE_PATH)/src/consensus/FISCO-BCOS-GR-Sig/algorithm/ring-sig $(SRC_BASE_PATH)/src/node)
TEST_PROPOSE_BATCH_FULL_LIB_PATH=$(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)/src/comm $(SRC_BASE_PATH)/src/logstorage $(SRC_BASE_PATH)/src/sm-base $(SRC_BASE_PATH)/src/config $(SRC_BASE_PATH)/src/communicate/tcp $(SRC_BASE_PATH)/src/communicate $(SRC_BASE_PATH)/src/checkpoint $(SRC_BASE_PATH)/src/algorithm $(SRC_BASE_PATH)/src/master $(SRC_BASE_PATH)/src/consensus $(SRC_BASE_PATH)/src/node $(SRC_BASE_PATH)//src/node
TEST_PROPOSE_BATCH_EXTRA_CPPFLAGS=-Wno-unused-variable -Wno-sign-compare -Wno-unused-but-set-variable -Wno-unused-result -Wall -Werror

CPPFLAGS+=$(patsubst %,-I%, $(TEST_PROPOSE_BATCH_INCS))
CPPFLAGS+=$(TEST_PROPOSE_BATCH_EXTRA_CPPFLAGS)

TEST_PROPOSE_BATCH_LINK=node algorithm checkpoint master consensus communicate config logstorage smbase communicate_tcp comm utils
TEST_PROPOSE_BATCH_SYS_LIB=$(LEVELDB_LIB_PATH)/libleveldb.a $(PROTOBUF_LIB_PATH)/libprotobuf.a -lmongocxx -lbsoncxx -lcryptopp -lboost_thread -lpbc -lpbc_sig -lpthread -lboost_system -lboost_filesystem -lgmp
TEST_PROPOSE_BATCH_FLAGS+=$(LDFLAGS)

TEST_PROPOSE_BATCH_FLAGS+=$(patsubst %,-l%, $(TEST_PROPOSE_BATCH_LINK))
TEST_PROPOSE_BATCH_FLAGS+=$(TEST_PROPOSE_BATCH_SYS_LIB)
test_propose_batch_bin:test_propose_batch_dir test_propose_batch

test_propose_batch_dir:$(TEST_PROPOSE_BATCH_FULL_LIB_PATH)
	@for dir in $^;\
	do \
	current_dir=`readlink $$dir -m`;\
	pwd_dir=`pwd`;\
	pwd_dir=`readlink $$pwd_dir -m`;\
	if ([ "$$current_dir" != "$$pwd_dir" ]); then \
	make -C $$dir;\
	fi;\
	done

test_propose_batch:$(TEST_PROPOSE_BATCH_SRC)
	$(CXX) $^ -o $@ $(TEST_PROPOSE_BATCH_FLAGS)
	cp $@ $(SRC_BASE_PATH)/.sbin/

clean:
	rm -rf *.o *.pb.* $(SRC_BASE_PATH)/.lib/libnode.a test_propose_batch $(SRC_BASE_PATH)/.sbin/test_propose_batch test_propose_batch 

