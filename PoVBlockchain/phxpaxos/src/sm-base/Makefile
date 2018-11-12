SRC_BASE_PATH=/home/wzx/Code/cplusplus/PoV-DN-version/PoVBlockchain/phxpaxos

all:lib_smbase 

include $(SRC_BASE_PATH)/makefile.mk

include Makefile.define
SMBASE_SRC=$(SMBASE_OBJ)
SMBASE_INCS=$(sort $(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)/src/comm $(PROTOBUF_INCLUDE_PATH) $(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/src/sm-base)
SMBASE_FULL_LIB_PATH=$(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)/src/comm $(SRC_BASE_PATH)//src/sm-base
SMBASE_EXTRA_CPPFLAGS=-Wall -Werror

CPPFLAGS+=$(patsubst %,-I%, $(SMBASE_INCS))
CPPFLAGS+=$(SMBASE_EXTRA_CPPFLAGS)

lib_smbase:$(SRC_BASE_PATH)/.lib/libsmbase.a

$(SRC_BASE_PATH)/.lib/libsmbase.a: $(SMBASE_SRC)
	ar -cvq $@ $(SMBASE_SRC)

clean:
	rm -rf *.o *.pb.* $(SRC_BASE_PATH)/.lib/libsmbase.a 

