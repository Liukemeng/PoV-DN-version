SRC_BASE_PATH=/home/wzx/Code/cplusplus/PoV-DN-version/PoVBlockchain/phxpaxos

all:lib_communicate_tcp 

include $(SRC_BASE_PATH)/makefile.mk

include Makefile.define
COMMUNICATE_TCP_SRC=$(COMMUNICATE_TCP_OBJ)
COMMUNICATE_TCP_INCS=$(sort $(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)/src/comm $(PROTOBUF_INCLUDE_PATH) $(SRC_BASE_PATH)/src/communicate/tcp)
COMMUNICATE_TCP_FULL_LIB_PATH=$(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/src/comm $(SRC_BASE_PATH)//src/communicate/tcp
COMMUNICATE_TCP_EXTRA_CPPFLAGS=-Wall -Werror

CPPFLAGS+=$(patsubst %,-I%, $(COMMUNICATE_TCP_INCS))
CPPFLAGS+=$(COMMUNICATE_TCP_EXTRA_CPPFLAGS)

lib_communicate_tcp:$(SRC_BASE_PATH)/.lib/libcommunicate_tcp.a

$(SRC_BASE_PATH)/.lib/libcommunicate_tcp.a: $(COMMUNICATE_TCP_SRC)
	ar -cvq $@ $(COMMUNICATE_TCP_SRC)

clean:
	rm -rf *.o *.pb.* $(SRC_BASE_PATH)/.lib/libcommunicate_tcp.a 

