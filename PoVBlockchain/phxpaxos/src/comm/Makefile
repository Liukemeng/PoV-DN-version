SRC_BASE_PATH=/home/wzx/Code/cplusplus/PoV-DN-version/PoVBlockchain/phxpaxos

all:lib_comm 

include $(SRC_BASE_PATH)/makefile.mk

include Makefile.define
COMM_SRC=$(COMM_OBJ)
COMM_INCS=$(sort $(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)/src/comm $(PROTOBUF_INCLUDE_PATH))
COMM_FULL_LIB_PATH=$(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)//src/comm
COMM_EXTRA_CPPFLAGS=-Wall -Werror

CPPFLAGS+=$(patsubst %,-I%, $(COMM_INCS))
CPPFLAGS+=$(COMM_EXTRA_CPPFLAGS)

lib_comm:$(SRC_BASE_PATH)/.lib/libcomm.a

$(SRC_BASE_PATH)/.lib/libcomm.a: $(COMM_SRC)
	ar -cvq $@ $(COMM_SRC)

clean:
	rm -rf *.o *.pb.* $(SRC_BASE_PATH)/.lib/libcomm.a 

