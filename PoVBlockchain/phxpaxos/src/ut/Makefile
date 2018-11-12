SRC_BASE_PATH=/home/wzx/Code/cplusplus/PoV-DN-version/PoVBlockchain/phxpaxos

all:phxpaxos_ut_bin 

include $(SRC_BASE_PATH)/makefile.mk

include Makefile.define
PHXPAXOS_UT_SRC=$(PHXPAXOS_UT_OBJ)
PHXPAXOS_UT_INCS=$(sort $(SRC_BASE_PATH)/src/config $(SRC_BASE_PATH)/src/config $(SRC_BASE_PATH)/src/checkpoint $(SRC_BASE_PATH)/src/config $(SRC_BASE_PATH)/src/algorithm $(SRC_BASE_PATH)/src/logstorage $(LEVELDB_INCLUDE_PATH) $(SRC_BASE_PATH)/src/sm-base $(SRC_BASE_PATH)/src/config $(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)/src/comm $(PROTOBUF_INCLUDE_PATH) $(SRC_BASE_PATH)/src/communicate/tcp $(SRC_BASE_PATH)/src/communicate $(SRC_BASE_PATH)/src/ut $(SRC_BASE_PATH)/third_party/gmock/include $(SRC_BASE_PATH)/third_party/gtest/include)
PHXPAXOS_UT_FULL_LIB_PATH=$(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)/src/comm $(SRC_BASE_PATH)/src/logstorage $(SRC_BASE_PATH)/src/sm-base $(SRC_BASE_PATH)/src/config $(SRC_BASE_PATH)/src/checkpoint $(SRC_BASE_PATH)/src/algorithm $(SRC_BASE_PATH)/src/communicate/tcp $(SRC_BASE_PATH)/src/communicate $(SRC_BASE_PATH)//src/ut
PHXPAXOS_UT_EXTRA_CPPFLAGS=-Wall -Werror

CPPFLAGS+=$(patsubst %,-I%, $(PHXPAXOS_UT_INCS))
CPPFLAGS+=$(PHXPAXOS_UT_EXTRA_CPPFLAGS)

PHXPAXOS_UT_LINK=algorithm checkpoint communicate config logstorage smbase communicate_tcp comm utils
PHXPAXOS_UT_SYS_LIB=$(LEVELDB_LIB_PATH)/libleveldb.a -lpthread $(PROTOBUF_LIB_PATH)/libprotobuf.a $(SRC_BASE_PATH)/third_party/gmock/lib/libgmock.a $(SRC_BASE_PATH)/third_party/gmock/lib/libgmock_main.a $(SRC_BASE_PATH)/third_party/gtest/lib/libgtest.a
PHXPAXOS_UT_FLAGS+=$(LDFLAGS)

PHXPAXOS_UT_FLAGS+=$(patsubst %,-l%, $(PHXPAXOS_UT_LINK))
PHXPAXOS_UT_FLAGS+=$(PHXPAXOS_UT_SYS_LIB)
phxpaxos_ut_bin:phxpaxos_ut_dir phxpaxos_ut

phxpaxos_ut_dir:$(PHXPAXOS_UT_FULL_LIB_PATH)
	@for dir in $^;\
	do \
	current_dir=`readlink $$dir -m`;\
	pwd_dir=`pwd`;\
	pwd_dir=`readlink $$pwd_dir -m`;\
	if ([ "$$current_dir" != "$$pwd_dir" ]); then \
	make -C $$dir;\
	fi;\
	done

phxpaxos_ut:$(PHXPAXOS_UT_SRC)
	$(CXX) $^ -o $@ $(PHXPAXOS_UT_FLAGS)
	cp $@ $(SRC_BASE_PATH)/.sbin/

clean:
	rm -rf *.o *.pb.* phxpaxos_ut $(SRC_BASE_PATH)/.sbin/phxpaxos_ut phxpaxos_ut 

