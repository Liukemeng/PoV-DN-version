SRC_BASE_PATH=/home/wzx/Code/cplusplus/PoV-DN-version/PoVBlockchain/phxpaxos

all:lib_monitor 

include $(SRC_BASE_PATH)/makefile.mk

include Makefile.define
MONITOR_SRC=$(MONITOR_OBJ)
MONITOR_INCS=$(sort $(SRC_BASE_PATH)/plugin/include $(SRC_BASE_PATH)/plugin/monitor $(PHXPAXOS_INCLUDE_PATH) $(LEVELDB_INCLUDE_PATH) $(PROTOBUF_INCLUDE_PATH))
MONITOR_FULL_LIB_PATH=$(SRC_BASE_PATH)/plugin/include $(SRC_BASE_PATH)//plugin/monitor
MONITOR_EXTRA_CPPFLAGS=-Wall -Werror

CPPFLAGS+=$(patsubst %,-I%, $(MONITOR_INCS))
CPPFLAGS+=$(MONITOR_EXTRA_CPPFLAGS)

lib_monitor:$(SRC_BASE_PATH)/.lib/libmonitor.a

$(SRC_BASE_PATH)/.lib/libmonitor.a: $(MONITOR_SRC)
	ar -cvq $@ $(MONITOR_SRC)

clean:
	rm -rf *.o *.pb.* $(SRC_BASE_PATH)/.lib/libmonitor.a 

