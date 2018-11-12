SRC_BASE_PATH=/home/wzx/Code/cplusplus/PoV-DN-version/PoVBlockchain/phxpaxos

all:phxecho_bin 

include $(SRC_BASE_PATH)/makefile.mk

include Makefile.define
PHXECHO_SRC=$(PHXECHO_OBJ)
PHXECHO_INCS=$(sort $(SRC_BASE_PATH)/sample/phxecho $(PHXPAXOS_PLUGIN_PATH) $(PHXPAXOS_INCLUDE_PATH) $(LEVELDB_INCLUDE_PATH) $(PROTOBUF_INCLUDE_PATH))
PHXECHO_FULL_LIB_PATH=$(SRC_BASE_PATH)//sample/phxecho
PHXECHO_EXTRA_CPPFLAGS=-Wall -Werror

CPPFLAGS+=$(patsubst %,-I%, $(PHXECHO_INCS))
CPPFLAGS+=$(PHXECHO_EXTRA_CPPFLAGS)

PHXECHO_LINK=
PHXECHO_SYS_LIB=$(PHXPAXOS_LIB_PATH)/libphxpaxos_plugin.a $(PHXPAXOS_LIB_PATH)/libphxpaxos.a $(LEVELDB_LIB_PATH)/libleveldb.a $(PROTOBUF_LIB_PATH)/libprotobuf.a $(GLOG_LIB_PATH)/libglog.a $(GLOG_LIB_PATH)/libglog.a -lpthread
PHXECHO_FLAGS+=$(LDFLAGS)

PHXECHO_FLAGS+=$(patsubst %,-l%, $(PHXECHO_LINK))
PHXECHO_FLAGS+=$(PHXECHO_SYS_LIB)
phxecho_bin:phxecho_dir phxecho

phxecho_dir:$(PHXECHO_FULL_LIB_PATH)
	@for dir in $^;\
	do \
	current_dir=`readlink $$dir -m`;\
	pwd_dir=`pwd`;\
	pwd_dir=`readlink $$pwd_dir -m`;\
	if ([ "$$current_dir" != "$$pwd_dir" ]); then \
	make -C $$dir;\
	fi;\
	done

phxecho:$(PHXECHO_SRC)
	$(CXX) $^ -o $@ $(PHXECHO_FLAGS)
	cp $@ $(SRC_BASE_PATH)/.sbin/

clean:
	rm -rf *.o *.pb.* phxecho $(SRC_BASE_PATH)/.sbin/phxecho phxecho 

