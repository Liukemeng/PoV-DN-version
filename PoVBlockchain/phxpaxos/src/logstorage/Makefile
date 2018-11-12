SRC_BASE_PATH=/home/wzx/Code/cplusplus/PoV-DN-version/PoVBlockchain/phxpaxos

all:lib_logstorage 

include $(SRC_BASE_PATH)/makefile.mk

include Makefile.define
LOGSTORAGE_SRC=$(LOGSTORAGE_OBJ)
LOGSTORAGE_INCS=$(sort $(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)/src/comm $(PROTOBUF_INCLUDE_PATH) $(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/src/logstorage $(LEVELDB_INCLUDE_PATH))
LOGSTORAGE_FULL_LIB_PATH=$(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)/src/comm $(SRC_BASE_PATH)//src/logstorage
LOGSTORAGE_EXTRA_CPPFLAGS=-Wall -Werror

CPPFLAGS+=$(patsubst %,-I%, $(LOGSTORAGE_INCS))
CPPFLAGS+=$(LOGSTORAGE_EXTRA_CPPFLAGS)

lib_logstorage:$(SRC_BASE_PATH)/.lib/liblogstorage.a

$(SRC_BASE_PATH)/.lib/liblogstorage.a: $(LOGSTORAGE_SRC)
	ar -cvq $@ $(LOGSTORAGE_SRC)

clean:
	rm -rf *.o *.pb.* $(SRC_BASE_PATH)/.lib/liblogstorage.a 

