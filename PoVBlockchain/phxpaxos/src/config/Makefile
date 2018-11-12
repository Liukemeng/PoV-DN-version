SRC_BASE_PATH=/home/wzx/Code/cplusplus/PoV-DN-version/PoVBlockchain/phxpaxos

all:lib_config 

include $(SRC_BASE_PATH)/makefile.mk

include Makefile.define
CONFIG_SRC=$(CONFIG_OBJ)
CONFIG_INCS=$(sort $(SRC_BASE_PATH)/src/logstorage $(LEVELDB_INCLUDE_PATH) $(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)/src/comm $(PROTOBUF_INCLUDE_PATH) $(SRC_BASE_PATH)/src/sm-base $(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/src/config)
CONFIG_FULL_LIB_PATH=$(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)/src/comm $(SRC_BASE_PATH)/src/logstorage $(SRC_BASE_PATH)/src/sm-base $(SRC_BASE_PATH)//src/config
CONFIG_EXTRA_CPPFLAGS=-Wall -Werror

CPPFLAGS+=$(patsubst %,-I%, $(CONFIG_INCS))
CPPFLAGS+=$(CONFIG_EXTRA_CPPFLAGS)

lib_config:$(SRC_BASE_PATH)/.lib/libconfig.a

$(SRC_BASE_PATH)/.lib/libconfig.a: $(CONFIG_SRC)
	ar -cvq $@ $(CONFIG_SRC)

clean:
	rm -rf *.o *.pb.* $(SRC_BASE_PATH)/.lib/libconfig.a 

