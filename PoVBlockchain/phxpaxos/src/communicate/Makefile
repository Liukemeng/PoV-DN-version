SRC_BASE_PATH=/home/wzx/Code/cplusplus/PoV-DN-version/PoVBlockchain/phxpaxos

all:lib_communicate 

include $(SRC_BASE_PATH)/makefile.mk

include Makefile.define
COMMUNICATE_SRC=$(COMMUNICATE_OBJ)
COMMUNICATE_INCS=$(sort $(SRC_BASE_PATH)/src/logstorage $(LEVELDB_INCLUDE_PATH) $(SRC_BASE_PATH)/src/sm-base $(SRC_BASE_PATH)/src/config $(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)/src/comm $(PROTOBUF_INCLUDE_PATH) $(SRC_BASE_PATH)/src/communicate/tcp $(SRC_BASE_PATH)/src/communicate)
COMMUNICATE_FULL_LIB_PATH=$(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/src/comm $(SRC_BASE_PATH)/src/logstorage $(SRC_BASE_PATH)/src/sm-base $(SRC_BASE_PATH)/src/config $(SRC_BASE_PATH)/src/communicate/tcp $(SRC_BASE_PATH)//src/communicate
COMMUNICATE_EXTRA_CPPFLAGS=-Wall -Werror

CPPFLAGS+=$(patsubst %,-I%, $(COMMUNICATE_INCS))
CPPFLAGS+=$(COMMUNICATE_EXTRA_CPPFLAGS)

lib_communicate:$(SRC_BASE_PATH)/.lib/libcommunicate.a

$(SRC_BASE_PATH)/.lib/libcommunicate.a: $(COMMUNICATE_SRC)
	ar -cvq $@ $(COMMUNICATE_SRC)

SUBDIRS=tcp

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
	rm -rf *.o *.pb.* $(SRC_BASE_PATH)/.lib/libcommunicate.a 

