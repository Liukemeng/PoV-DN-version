SRC_BASE_PATH=/home/wzx/Code/cplusplus/PoV-DN-version/PoVBlockchain/phxpaxos

all:phx_paxos_test_bin 

include $(SRC_BASE_PATH)/makefile.mk

include Makefile.define
PHX_PAXOS_TEST_SRC=$(PHX_PAXOS_TEST_OBJ)
PHX_PAXOS_TEST_INCS=$(sort $(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)/src/comm $(SRC_BASE_PATH)/src/test $(PHXPAXOS_INCLUDE_PATH) $(LEVELDB_INCLUDE_PATH) $(PROTOBUF_INCLUDE_PATH))
PHX_PAXOS_TEST_FULL_LIB_PATH=$(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/src/comm $(SRC_BASE_PATH)//src/test
PHX_PAXOS_TEST_EXTRA_CPPFLAGS=-Wall -Werror

CPPFLAGS+=$(patsubst %,-I%, $(PHX_PAXOS_TEST_INCS))
CPPFLAGS+=$(PHX_PAXOS_TEST_EXTRA_CPPFLAGS)

PHX_PAXOS_TEST_LINK=comm utils
PHX_PAXOS_TEST_SYS_LIB=$(PHXPAXOS_LIB_PATH)/libphxpaxos.a $(LEVELDB_LIB_PATH)/libleveldb.a $(PROTOBUF_LIB_PATH)/libprotobuf.a -lpthread
PHX_PAXOS_TEST_FLAGS+=$(LDFLAGS)

PHX_PAXOS_TEST_FLAGS+=$(patsubst %,-l%, $(PHX_PAXOS_TEST_LINK))
PHX_PAXOS_TEST_FLAGS+=$(PHX_PAXOS_TEST_SYS_LIB)
phx_paxos_test_bin:phx_paxos_test_dir phx_paxos_test

phx_paxos_test_dir:$(PHX_PAXOS_TEST_FULL_LIB_PATH)
	@for dir in $^;\
	do \
	current_dir=`readlink $$dir -m`;\
	pwd_dir=`pwd`;\
	pwd_dir=`readlink $$pwd_dir -m`;\
	if ([ "$$current_dir" != "$$pwd_dir" ]); then \
	make -C $$dir;\
	fi;\
	done

phx_paxos_test:$(PHX_PAXOS_TEST_SRC)
	$(CXX) $^ -o $@ $(PHX_PAXOS_TEST_FLAGS)
	cp $@ $(SRC_BASE_PATH)/.sbin/

clean:
	rm -rf *.o *.pb.* phx_paxos_test $(SRC_BASE_PATH)/.sbin/phx_paxos_test phx_paxos_test 

