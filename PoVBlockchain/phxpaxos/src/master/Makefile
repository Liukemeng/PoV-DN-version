SRC_BASE_PATH=/home/wzx/Code/cplusplus/PoV-DN-version/PoVBlockchain/phxpaxos

all:lib_master 

include $(SRC_BASE_PATH)/makefile.mk

include Makefile.define
MASTER_SRC=$(MASTER_OBJ)
MASTER_INCS=$(sort $(LEVELDB_INCLUDE_PATH) $(SRC_BASE_PATH)/src/sm-base $(SRC_BASE_PATH)/src/config $(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)/src/comm $(PROTOBUF_INCLUDE_PATH) $(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/src/logstorage $(LEVELDB_INCLUDE_PATH) $(SRC_BASE_PATH)/src/master)
MASTER_FULL_LIB_PATH=$(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/src/comm $(SRC_BASE_PATH)/src/logstorage $(SRC_BASE_PATH)/src/sm-base $(SRC_BASE_PATH)/src/config $(SRC_BASE_PATH)//src/master
MASTER_EXTRA_CPPFLAGS=-Wall -Werror

CPPFLAGS+=$(patsubst %,-I%, $(MASTER_INCS))
CPPFLAGS+=$(MASTER_EXTRA_CPPFLAGS)

lib_master:$(SRC_BASE_PATH)/.lib/libmaster.a

$(SRC_BASE_PATH)/.lib/libmaster.a: $(MASTER_SRC)
	ar -cvq $@ $(MASTER_SRC)

clean:
	rm -rf *.o *.pb.* $(SRC_BASE_PATH)/.lib/libmaster.a 

