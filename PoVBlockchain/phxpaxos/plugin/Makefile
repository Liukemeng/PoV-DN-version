SRC_BASE_PATH=/home/wzx/Code/cplusplus/PoV-DN-version/PoVBlockchain/phxpaxos

all:lib_phxpaxos_plugin 

include $(SRC_BASE_PATH)/makefile.mk

include Makefile.define
PHXPAXOS_PLUGIN_SRC=$(PHXPAXOS_PLUGIN_OBJ)
PHXPAXOS_PLUGIN_INCS=$(sort $(SRC_BASE_PATH)/plugin/logger_google $(SRC_BASE_PATH)/plugin/include $(SRC_BASE_PATH)/plugin/monitor $(PHXPAXOS_INCLUDE_PATH) $(LEVELDB_INCLUDE_PATH) $(PROTOBUF_INCLUDE_PATH) $(SRC_BASE_PATH)/plugin)
PHXPAXOS_PLUGIN_FULL_LIB_PATH=$(SRC_BASE_PATH)/plugin/include $(SRC_BASE_PATH)/plugin/logger_google $(SRC_BASE_PATH)/plugin/monitor $(SRC_BASE_PATH)//plugin
PHXPAXOS_PLUGIN_EXTRA_CPPFLAGS=-Wall -Werror

CPPFLAGS+=$(patsubst %,-I%, $(PHXPAXOS_PLUGIN_INCS))
CPPFLAGS+=$(PHXPAXOS_PLUGIN_EXTRA_CPPFLAGS)

lib_phxpaxos_plugin: phxpaxos_plugin_dir $(SRC_BASE_PATH)/.lib/libphxpaxos_plugin.a $(SRC_BASE_PATH)/.lib/extlib/libphxpaxos_plugin.a

phxpaxos_plugin_dir:$(PHXPAXOS_PLUGIN_FULL_LIB_PATH)
	@for dir in $^;\
	do \
	current_dir=`readlink $$dir -m`;\
	pwd_dir=`pwd`;\
	pwd_dir=`readlink $$pwd_dir -m`;\
	if ([ "$$current_dir" != "$$pwd_dir" ]); then \
	make -C $$dir;\
	fi;\
	done

$(SRC_BASE_PATH)/.lib/libphxpaxos_plugin.a: $(PHXPAXOS_PLUGIN_SRC)
	ar -cvq $@ $(PHXPAXOS_PLUGIN_SRC)

PHXPAXOS_PLUGIN_LIB_OBJ=$(patsubst %, $(SRC_BASE_PATH)/%, plugin/logger_google/logger_google.o plugin/monitor/monitor.o plugin/monitor/monitor_bp.o)
$(SRC_BASE_PATH)/.lib/extlib/libphxpaxos_plugin.a: $(PHXPAXOS_PLUGIN_LIB_OBJ)
	ar -cvq $@ $(PHXPAXOS_PLUGIN_LIB_OBJ)

SUBDIRS=monitor include logger_google

.PHONY:sub_dir
sub_dir:$(SUBDIRS)
	@for sub_dir in $^; do \
	make -C $$sub_dir; \
	done

.PHONY:clean
clean:$(SUBDIRS)
	@for sub_dir in $^; do \
	make -C $$sub_dir clean;\
	done
	rm -rf *.o *.pb.* $(SRC_BASE_PATH)/.lib/libphxpaxos_plugin.a $(SRC_BASE_PATH)/.lib/extlib/libphxpaxos_plugin.a libphxpaxos_plugin.a $(SRC_BASE_PATH)/.lib/libphxpaxos_plugin.a 

