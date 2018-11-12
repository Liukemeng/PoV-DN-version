SRC_BASE_PATH=/home/wzx/Code/cplusplus/PoV-DN-version/PoVBlockchain/phxpaxos

all:lib_logger_google 

include $(SRC_BASE_PATH)/makefile.mk

include Makefile.define
LOGGER_GOOGLE_SRC=$(LOGGER_GOOGLE_OBJ)
LOGGER_GOOGLE_INCS=$(sort $(SRC_BASE_PATH)/plugin/include $(SRC_BASE_PATH)/plugin/logger_google $(PHXPAXOS_INCLUDE_PATH) $(LEVELDB_INCLUDE_PATH) $(PROTOBUF_INCLUDE_PATH))
LOGGER_GOOGLE_FULL_LIB_PATH=$(SRC_BASE_PATH)/plugin/include $(SRC_BASE_PATH)//plugin/logger_google
LOGGER_GOOGLE_EXTRA_CPPFLAGS=-Wall -Werror

CPPFLAGS+=$(patsubst %,-I%, $(LOGGER_GOOGLE_INCS))
CPPFLAGS+=$(LOGGER_GOOGLE_EXTRA_CPPFLAGS)

lib_logger_google:$(SRC_BASE_PATH)/.lib/liblogger_google.a

$(SRC_BASE_PATH)/.lib/liblogger_google.a: $(LOGGER_GOOGLE_SRC)
	ar -cvq $@ $(LOGGER_GOOGLE_SRC)

clean:
	rm -rf *.o *.pb.* $(SRC_BASE_PATH)/.lib/liblogger_google.a 

