SRC_BASE_PATH=/home/wzx/Code/cplusplus/PoV-DN-version/PoVBlockchain/phxpaxos

all:lib_utils 

include $(SRC_BASE_PATH)/makefile.mk

include Makefile.define
UTILS_SRC=$(UTILS_OBJ)
UTILS_INCS=$(sort $(SRC_BASE_PATH)/src/utils)
UTILS_FULL_LIB_PATH=$(SRC_BASE_PATH)//src/utils
UTILS_EXTRA_CPPFLAGS=-Wall -Werror

CPPFLAGS+=$(patsubst %,-I%, $(UTILS_INCS))
CPPFLAGS+=$(UTILS_EXTRA_CPPFLAGS)

lib_utils:$(SRC_BASE_PATH)/.lib/libutils.a

$(SRC_BASE_PATH)/.lib/libutils.a: $(UTILS_SRC)
	ar -cvq $@ $(UTILS_SRC)

clean:
	rm -rf *.o *.pb.* $(SRC_BASE_PATH)/.lib/libutils.a 

