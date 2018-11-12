SRC_BASE_PATH=/home/wzx/Code/cplusplus/PoV-DN-version/PoVBlockchain/phxpaxos

all:lib_algorithm 

include $(SRC_BASE_PATH)/makefile.mk

include Makefile.define
ALGORITHM_SRC=$(ALGORITHM_OBJ)
ALGORITHM_INCS=$(sort $(SRC_BASE_PATH)/src/config $(SRC_BASE_PATH)/src/checkpoint $(SRC_BASE_PATH)/src/logstorage $(LEVELDB_INCLUDE_PATH) $(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)/src/comm $(PROTOBUF_INCLUDE_PATH) $(SRC_BASE_PATH)/src/sm-base $(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/src/config $(SRC_BASE_PATH)/src/algorithm)
ALGORITHM_FULL_LIB_PATH=$(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)/src/comm $(SRC_BASE_PATH)/src/logstorage $(SRC_BASE_PATH)/src/sm-base $(SRC_BASE_PATH)/src/config $(SRC_BASE_PATH)/src/checkpoint $(SRC_BASE_PATH)//src/algorithm
ALGORITHM_EXTRA_CPPFLAGS=-Wall -Werror

CPPFLAGS+=$(patsubst %,-I%, $(ALGORITHM_INCS))
CPPFLAGS+=$(ALGORITHM_EXTRA_CPPFLAGS)

lib_algorithm:$(SRC_BASE_PATH)/.lib/libalgorithm.a

$(SRC_BASE_PATH)/.lib/libalgorithm.a: $(ALGORITHM_SRC)
	ar -cvq $@ $(ALGORITHM_SRC)

clean:
	rm -rf *.o *.pb.* $(SRC_BASE_PATH)/.lib/libalgorithm.a 

