SRC_BASE_PATH=/home/wzx/Code/cplusplus/PoV-DN-version/PoVBlockchain/phxpaxos

all:phx_paxos_bench_bin bench_db_bin 

include $(SRC_BASE_PATH)/makefile.mk

include Makefile.define
PHX_PAXOS_BENCH_SRC=$(PHX_PAXOS_BENCH_OBJ)
PHX_PAXOS_BENCH_INCS=$(sort $(SRC_BASE_PATH)/src/checkpoint $(SRC_BASE_PATH)/src/logstorage $(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)/src/comm $(SRC_BASE_PATH)/src/sm-base $(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/src/config $(SRC_BASE_PATH)/src/algorithm $(SRC_BASE_PATH)/src/benchmark $(PHXPAXOS_INCLUDE_PATH) $(LEVELDB_INCLUDE_PATH) $(PROTOBUF_INCLUDE_PATH))
PHX_PAXOS_BENCH_FULL_LIB_PATH=$(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/src/comm $(SRC_BASE_PATH)/src/logstorage $(SRC_BASE_PATH)/src/sm-base $(SRC_BASE_PATH)/src/config $(SRC_BASE_PATH)/src/checkpoint $(SRC_BASE_PATH)/src/algorithm $(SRC_BASE_PATH)//src/benchmark
PHX_PAXOS_BENCH_EXTRA_CPPFLAGS=-Wall -Werror

CPPFLAGS+=$(patsubst %,-I%, $(PHX_PAXOS_BENCH_INCS))
CPPFLAGS+=$(PHX_PAXOS_BENCH_EXTRA_CPPFLAGS)

PHX_PAXOS_BENCH_LINK=algorithm checkpoint config logstorage smbase comm utils
PHX_PAXOS_BENCH_SYS_LIB=$(PHXPAXOS_LIB_PATH)/libphxpaxos.a $(LEVELDB_LIB_PATH)/libleveldb.a $(PROTOBUF_LIB_PATH)/libprotobuf.a -lpthread
PHX_PAXOS_BENCH_FLAGS+=$(LDFLAGS)

PHX_PAXOS_BENCH_FLAGS+=$(patsubst %,-l%, $(PHX_PAXOS_BENCH_LINK))
PHX_PAXOS_BENCH_FLAGS+=$(PHX_PAXOS_BENCH_SYS_LIB)
phx_paxos_bench_bin:phx_paxos_bench_dir phx_paxos_bench

phx_paxos_bench_dir:$(PHX_PAXOS_BENCH_FULL_LIB_PATH)
	@for dir in $^;\
	do \
	current_dir=`readlink $$dir -m`;\
	pwd_dir=`pwd`;\
	pwd_dir=`readlink $$pwd_dir -m`;\
	if ([ "$$current_dir" != "$$pwd_dir" ]); then \
	make -C $$dir;\
	fi;\
	done

phx_paxos_bench:$(PHX_PAXOS_BENCH_SRC)
	$(CXX) $^ -o $@ $(PHX_PAXOS_BENCH_FLAGS)
	cp $@ $(SRC_BASE_PATH)/.sbin/

BENCH_DB_SRC=$(BENCH_DB_OBJ)
BENCH_DB_INCS=$(sort $(SRC_BASE_PATH)/src/benchmark $(PHXPAXOS_INCLUDE_PATH) $(LEVELDB_INCLUDE_PATH) $(PROTOBUF_INCLUDE_PATH))
BENCH_DB_FULL_LIB_PATH=$(SRC_BASE_PATH)//src/benchmark
BENCH_DB_EXTRA_CPPFLAGS=-Wall -Werror

CPPFLAGS+=$(patsubst %,-I%, $(BENCH_DB_INCS))
CPPFLAGS+=$(BENCH_DB_EXTRA_CPPFLAGS)

BENCH_DB_LINK=
BENCH_DB_SYS_LIB=$(PHXPAXOS_LIB_PATH)/libphxpaxos.a $(LEVELDB_LIB_PATH)/libleveldb.a $(PROTOBUF_LIB_PATH)/libprotobuf.a -lpthread
BENCH_DB_FLAGS+=$(LDFLAGS)

BENCH_DB_FLAGS+=$(patsubst %,-l%, $(BENCH_DB_LINK))
BENCH_DB_FLAGS+=$(BENCH_DB_SYS_LIB)
bench_db_bin:bench_db_dir bench_db

bench_db_dir:$(BENCH_DB_FULL_LIB_PATH)
	@for dir in $^;\
	do \
	current_dir=`readlink $$dir -m`;\
	pwd_dir=`pwd`;\
	pwd_dir=`readlink $$pwd_dir -m`;\
	if ([ "$$current_dir" != "$$pwd_dir" ]); then \
	make -C $$dir;\
	fi;\
	done

bench_db:$(BENCH_DB_SRC)
	$(CXX) $^ -o $@ $(BENCH_DB_FLAGS)
	cp $@ $(SRC_BASE_PATH)/.sbin/

clean:
	rm -rf *.o *.pb.* phx_paxos_bench $(SRC_BASE_PATH)/.sbin/phx_paxos_bench bench_db $(SRC_BASE_PATH)/.sbin/bench_db phx_paxos_bench bench_db 

