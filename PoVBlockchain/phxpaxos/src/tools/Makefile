SRC_BASE_PATH=/home/wzx/Code/cplusplus/PoV-DN-version/PoVBlockchain/phxpaxos

all:system_variables_tools_bin paxos_log_tools_bin vfile_fetch_bin 

include $(SRC_BASE_PATH)/makefile.mk

include Makefile.define
SYSTEM_VARIABLES_TOOLS_SRC=$(SYSTEM_VARIABLES_TOOLS_OBJ)
SYSTEM_VARIABLES_TOOLS_INCS=$(sort $(SRC_BASE_PATH)/src/sm-base $(SRC_BASE_PATH)/src/config $(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)/src/comm $(PROTOBUF_INCLUDE_PATH) $(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/src/logstorage $(LEVELDB_INCLUDE_PATH) $(SRC_BASE_PATH)/src/tools)
SYSTEM_VARIABLES_TOOLS_FULL_LIB_PATH=$(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)/src/comm $(SRC_BASE_PATH)/src/logstorage $(SRC_BASE_PATH)/src/sm-base $(SRC_BASE_PATH)/src/config $(SRC_BASE_PATH)//src/tools
SYSTEM_VARIABLES_TOOLS_EXTRA_CPPFLAGS=-Wall -Werror

CPPFLAGS+=$(patsubst %,-I%, $(SYSTEM_VARIABLES_TOOLS_INCS))
CPPFLAGS+=$(SYSTEM_VARIABLES_TOOLS_EXTRA_CPPFLAGS)

SYSTEM_VARIABLES_TOOLS_LINK=config smbase logstorage comm utils
SYSTEM_VARIABLES_TOOLS_SYS_LIB=-lpthread $(PROTOBUF_LIB_PATH)/libprotobuf.a $(LEVELDB_LIB_PATH)/libleveldb.a
SYSTEM_VARIABLES_TOOLS_FLAGS+=$(LDFLAGS)

SYSTEM_VARIABLES_TOOLS_FLAGS+=$(patsubst %,-l%, $(SYSTEM_VARIABLES_TOOLS_LINK))
SYSTEM_VARIABLES_TOOLS_FLAGS+=$(SYSTEM_VARIABLES_TOOLS_SYS_LIB)
system_variables_tools_bin:system_variables_tools_dir system_variables_tools

system_variables_tools_dir:$(SYSTEM_VARIABLES_TOOLS_FULL_LIB_PATH)
	@for dir in $^;\
	do \
	current_dir=`readlink $$dir -m`;\
	pwd_dir=`pwd`;\
	pwd_dir=`readlink $$pwd_dir -m`;\
	if ([ "$$current_dir" != "$$pwd_dir" ]); then \
	make -C $$dir;\
	fi;\
	done

system_variables_tools:$(SYSTEM_VARIABLES_TOOLS_SRC)
	$(CXX) $^ -o $@ $(SYSTEM_VARIABLES_TOOLS_FLAGS)
	cp $@ $(SRC_BASE_PATH)/.sbin/

PAXOS_LOG_TOOLS_SRC=$(PAXOS_LOG_TOOLS_OBJ)
PAXOS_LOG_TOOLS_INCS=$(sort $(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)/src/comm $(PROTOBUF_INCLUDE_PATH) $(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/src/logstorage $(LEVELDB_INCLUDE_PATH) $(SRC_BASE_PATH)/src/tools)
PAXOS_LOG_TOOLS_FULL_LIB_PATH=$(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)/src/comm $(SRC_BASE_PATH)/src/logstorage $(SRC_BASE_PATH)//src/tools
PAXOS_LOG_TOOLS_EXTRA_CPPFLAGS=-Wall -Werror

CPPFLAGS+=$(patsubst %,-I%, $(PAXOS_LOG_TOOLS_INCS))
CPPFLAGS+=$(PAXOS_LOG_TOOLS_EXTRA_CPPFLAGS)

PAXOS_LOG_TOOLS_LINK=logstorage comm utils
PAXOS_LOG_TOOLS_SYS_LIB=-lpthread $(PROTOBUF_LIB_PATH)/libprotobuf.a $(LEVELDB_LIB_PATH)/libleveldb.a
PAXOS_LOG_TOOLS_FLAGS+=$(LDFLAGS)

PAXOS_LOG_TOOLS_FLAGS+=$(patsubst %,-l%, $(PAXOS_LOG_TOOLS_LINK))
PAXOS_LOG_TOOLS_FLAGS+=$(PAXOS_LOG_TOOLS_SYS_LIB)
paxos_log_tools_bin:paxos_log_tools_dir paxos_log_tools

paxos_log_tools_dir:$(PAXOS_LOG_TOOLS_FULL_LIB_PATH)
	@for dir in $^;\
	do \
	current_dir=`readlink $$dir -m`;\
	pwd_dir=`pwd`;\
	pwd_dir=`readlink $$pwd_dir -m`;\
	if ([ "$$current_dir" != "$$pwd_dir" ]); then \
	make -C $$dir;\
	fi;\
	done

paxos_log_tools:$(PAXOS_LOG_TOOLS_SRC)
	$(CXX) $^ -o $@ $(PAXOS_LOG_TOOLS_FLAGS)
	cp $@ $(SRC_BASE_PATH)/.sbin/

VFILE_FETCH_SRC=$(VFILE_FETCH_OBJ)
VFILE_FETCH_INCS=$(sort $(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)/src/comm $(PROTOBUF_INCLUDE_PATH) $(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/src/logstorage $(LEVELDB_INCLUDE_PATH) $(SRC_BASE_PATH)/src/tools)
VFILE_FETCH_FULL_LIB_PATH=$(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)/src/comm $(SRC_BASE_PATH)/src/logstorage $(SRC_BASE_PATH)//src/tools
VFILE_FETCH_EXTRA_CPPFLAGS=-Wall -Werror

CPPFLAGS+=$(patsubst %,-I%, $(VFILE_FETCH_INCS))
CPPFLAGS+=$(VFILE_FETCH_EXTRA_CPPFLAGS)

VFILE_FETCH_LINK=logstorage comm utils
VFILE_FETCH_SYS_LIB=-lpthread $(PROTOBUF_LIB_PATH)/libprotobuf.a $(LEVELDB_LIB_PATH)/libleveldb.a
VFILE_FETCH_FLAGS+=$(LDFLAGS)

VFILE_FETCH_FLAGS+=$(patsubst %,-l%, $(VFILE_FETCH_LINK))
VFILE_FETCH_FLAGS+=$(VFILE_FETCH_SYS_LIB)
vfile_fetch_bin:vfile_fetch_dir vfile_fetch

vfile_fetch_dir:$(VFILE_FETCH_FULL_LIB_PATH)
	@for dir in $^;\
	do \
	current_dir=`readlink $$dir -m`;\
	pwd_dir=`pwd`;\
	pwd_dir=`readlink $$pwd_dir -m`;\
	if ([ "$$current_dir" != "$$pwd_dir" ]); then \
	make -C $$dir;\
	fi;\
	done

vfile_fetch:$(VFILE_FETCH_SRC)
	$(CXX) $^ -o $@ $(VFILE_FETCH_FLAGS)
	cp $@ $(SRC_BASE_PATH)/.sbin/

clean:
	rm -rf *.o *.pb.* system_variables_tools $(SRC_BASE_PATH)/.sbin/system_variables_tools paxos_log_tools $(SRC_BASE_PATH)/.sbin/paxos_log_tools vfile_fetch $(SRC_BASE_PATH)/.sbin/vfile_fetch system_variables_tools paxos_log_tools vfile_fetch 

