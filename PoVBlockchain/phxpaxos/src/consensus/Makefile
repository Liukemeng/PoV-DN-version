SRC_BASE_PATH=/home/wzx/Code/cplusplus/PoV-DN-version/PoVBlockchain/phxpaxos

all:lib_consensus 

include $(SRC_BASE_PATH)/makefile.mk

include Makefile.define
CONSENSUS_SRC=$(CONSENSUS_OBJ)
CONSENSUS_INCS=$(sort $(SRC_BASE_PATH)/src/communicate $(SRC_BASE_PATH)/src/logstorage $(LEVELDB_INCLUDE_PATH) $(SRC_BASE_PATH)/src/sm-base $(SRC_BASE_PATH)/src/config $(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)/src/comm $(PROTOBUF_INCLUDE_PATH) $(SRC_BASE_PATH)/src/communicate/tcp $(SRC_BASE_PATH)/src/consensus /usr/local/include/mongocxx/v_noabi /usr/local/include/bsoncxx/v_noabi $(SRC_BASE_PATH)/src/consensus/FISCO-BCOS-GR-Sig $(SRC_BASE_PATH)/src/consensus/FISCO-BCOS-GR-Sig/algorithm/bbs04 $(SRC_BASE_PATH)/src/consensus/FISCO-BCOS-GR-Sig/algorithm $(SRC_BASE_PATH)/src/consensus/FISCO-BCOS-GR-Sig/algorithm/ring-sig)
CONSENSUS_FULL_LIB_PATH=$(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/src/comm $(SRC_BASE_PATH)/src/logstorage $(SRC_BASE_PATH)/src/sm-base $(SRC_BASE_PATH)/src/config $(SRC_BASE_PATH)/src/communicate/tcp $(SRC_BASE_PATH)/src/communicate $(SRC_BASE_PATH)//src/consensus
CONSENSUS_EXTRA_CPPFLAGS=-Wall -Werror -Wno-unused-variable -Wno-sign-compare -Wno-unused-but-set-variable -Wno-unused-result

CPPFLAGS+=$(patsubst %,-I%, $(CONSENSUS_INCS))
CPPFLAGS+=$(CONSENSUS_EXTRA_CPPFLAGS)

lib_consensus:$(SRC_BASE_PATH)/.lib/libconsensus.a

$(SRC_BASE_PATH)/.lib/libconsensus.a: $(CONSENSUS_SRC)
	ar -cvq $@ $(CONSENSUS_SRC)

SUBDIRS=Linkable-Ring-Signature-master FISCO-BCOS-GR-Sig rapidjson Group-Signature-CL-Scheme

.PHONY:sub_dir
sub_dir:$(SUBDIRS)
	@for sub_dir in $^; do \
	make -C $$sub_dir; \
	done

.PHONY:clean
clean:$(SUBDIRS)
	@for sub_dir in $^; do \
	make -C $$sub_dir clean;\
	done
	rm -rf *.o *.pb.* $(SRC_BASE_PATH)/.lib/libconsensus.a 

