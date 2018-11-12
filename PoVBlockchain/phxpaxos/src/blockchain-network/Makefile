SRC_BASE_PATH=/home/wzx/Code/cplusplus/PoV-DN-version/PoVBlockchain/phxpaxos

all:lib_blockchain-network 

include $(SRC_BASE_PATH)/makefile.mk

include Makefile.define
BLOCKCHAIN-NETWORK_SRC=$(BLOCKCHAIN-NETWORK_OBJ)
BLOCKCHAIN-NETWORK_INCS=$(sort $(SRC_BASE_PATH)/src/blockchain-network)
BLOCKCHAIN-NETWORK_FULL_LIB_PATH=$(SRC_BASE_PATH)//src/blockchain-network
BLOCKCHAIN-NETWORK_EXTRA_CPPFLAGS=

CPPFLAGS+=$(patsubst %,-I%, $(BLOCKCHAIN-NETWORK_INCS))
CPPFLAGS+=$(BLOCKCHAIN-NETWORK_EXTRA_CPPFLAGS)

lib_blockchain-network:$(SRC_BASE_PATH)/.lib/libblockchain-network.a

$(SRC_BASE_PATH)/.lib/libblockchain-network.a: $(BLOCKCHAIN-NETWORK_SRC)
	ar -cvq $@ $(BLOCKCHAIN-NETWORK_SRC)

clean:
	rm -rf *.o *.pb.* $(SRC_BASE_PATH)/.lib/libblockchain-network.a 

