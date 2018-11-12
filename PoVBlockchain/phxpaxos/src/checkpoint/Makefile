SRC_BASE_PATH=/home/wzx/Code/cplusplus/PoV-DN-version/PoVBlockchain/phxpaxos

all:lib_checkpoint 

include $(SRC_BASE_PATH)/makefile.mk

include Makefile.define
CHECKPOINT_SRC=$(CHECKPOINT_OBJ)
CHECKPOINT_INCS=$(sort $(SRC_BASE_PATH)/src/logstorage $(SRC_BASE_PATH)/src/logstorage $(LEVELDB_INCLUDE_PATH) $(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)/src/comm $(PROTOBUF_INCLUDE_PATH) $(SRC_BASE_PATH)/src/sm-base $(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/src/config $(SRC_BASE_PATH)/src/checkpoint)
CHECKPOINT_FULL_LIB_PATH=$(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)/src/comm $(SRC_BASE_PATH)/src/logstorage $(SRC_BASE_PATH)/src/sm-base $(SRC_BASE_PATH)/src/config $(SRC_BASE_PATH)//src/checkpoint
CHECKPOINT_EXTRA_CPPFLAGS=-Wall -Werror

CPPFLAGS+=$(patsubst %,-I%, $(CHECKPOINT_INCS))
CPPFLAGS+=$(CHECKPOINT_EXTRA_CPPFLAGS)

lib_checkpoint:$(SRC_BASE_PATH)/.lib/libcheckpoint.a

$(SRC_BASE_PATH)/.lib/libcheckpoint.a: $(CHECKPOINT_SRC)
	ar -cvq $@ $(CHECKPOINT_SRC)

clean:
	rm -rf *.o *.pb.* $(SRC_BASE_PATH)/.lib/libcheckpoint.a 

