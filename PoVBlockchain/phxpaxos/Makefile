SRC_BASE_PATH=/home/wzx/Code/cplusplus/PoV-DN-version/PoVBlockchain/phxpaxos

all:lib_phxpaxos 

include $(SRC_BASE_PATH)/makefile.mk

include Makefile.define
PHXPAXOS_SRC=$(PHXPAXOS_OBJ)
PHXPAXOS_INCS=$(sort $(SRC_BASE_PATH)/src/checkpoint $(SRC_BASE_PATH)/src/algorithm $(SRC_BASE_PATH)/src/master $(SRC_BASE_PATH)/src/communicate $(SRC_BASE_PATH)/src/logstorage $(LEVELDB_INCLUDE_PATH) $(SRC_BASE_PATH)/src/sm-base $(SRC_BASE_PATH)/src/config $(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)/src/comm $(PROTOBUF_INCLUDE_PATH) $(SRC_BASE_PATH)/src/communicate/tcp $(SRC_BASE_PATH)/src/consensus /usr/local/include/mongocxx/v_noabi /usr/local/include/bsoncxx/v_noabi $(SRC_BASE_PATH)/src/consensus/FISCO-BCOS-GR-Sig $(SRC_BASE_PATH)/src/consensus/FISCO-BCOS-GR-Sig/algorithm/bbs04 $(SRC_BASE_PATH)/src/consensus/FISCO-BCOS-GR-Sig/algorithm $(SRC_BASE_PATH)/src/consensus/FISCO-BCOS-GR-Sig/algorithm/ring-sig $(SRC_BASE_PATH)/src/node $(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/)
PHXPAXOS_FULL_LIB_PATH=$(SRC_BASE_PATH)/include $(SRC_BASE_PATH)/src/utils $(SRC_BASE_PATH)/src/comm $(SRC_BASE_PATH)/src/logstorage $(SRC_BASE_PATH)/src/sm-base $(SRC_BASE_PATH)/src/config $(SRC_BASE_PATH)/src/communicate/tcp $(SRC_BASE_PATH)/src/communicate $(SRC_BASE_PATH)/src/checkpoint $(SRC_BASE_PATH)/src/algorithm $(SRC_BASE_PATH)/src/master $(SRC_BASE_PATH)/src/consensus $(SRC_BASE_PATH)/src/node $(SRC_BASE_PATH)/
PHXPAXOS_EXTRA_CPPFLAGS=-Wno-unused-variable -Wno-sign-compare -Wno-unused-but-set-variable -Wno-unused-result -Wall -Werror

CPPFLAGS+=$(patsubst %,-I%, $(PHXPAXOS_INCS))
CPPFLAGS+=$(PHXPAXOS_EXTRA_CPPFLAGS)

lib_phxpaxos: phxpaxos_dir $(SRC_BASE_PATH)/.lib/libphxpaxos.a $(SRC_BASE_PATH)/.lib/extlib/libphxpaxos.a

phxpaxos_dir:$(PHXPAXOS_FULL_LIB_PATH)
	@for dir in $^;\
	do \
	current_dir=`readlink $$dir -m`;\
	pwd_dir=`pwd`;\
	pwd_dir=`readlink $$pwd_dir -m`;\
	if ([ "$$current_dir" != "$$pwd_dir" ]); then \
	make -C $$dir;\
	fi;\
	done

$(SRC_BASE_PATH)/.lib/libphxpaxos.a: $(PHXPAXOS_SRC)
	ar -cvq $@ $(PHXPAXOS_SRC)

PHXPAXOS_LIB_OBJ=$(patsubst %, $(SRC_BASE_PATH)/%, src/node/group.o src/node/pnode.o src/node/node.o src/node/propose_batch.o src/algorithm/base.o src/algorithm/proposer.o src/algorithm/acceptor.o src/algorithm/learner.o src/algorithm/learner_sender.o src/algorithm/instance.o src/algorithm/ioloop.o src/algorithm/commitctx.o src/algorithm/committer.o src/algorithm/checkpoint_sender.o src/algorithm/checkpoint_receiver.o src/algorithm/msg_counter.o src/checkpoint/cp_mgr.o src/checkpoint/replayer.o src/checkpoint/cleaner.o src/master/master_sm.pb.o src/master/master_sm.o src/master/master_mgr.o src/master/master_variables_store.o src/consensus/test.o src/consensus/blockchain-network.o src/consensus/DocumentContainer.o src/consensus/utils.o src/consensus/KeyManager.o src/consensus/MessageManager.o src/consensus/account.o src/consensus/AccountManager.o src/consensus/CallBackInstance.o src/consensus/PoVBlock.o src/consensus/PoVHeader.o src/consensus/PoVTransaction.o src/consensus/user.o src/consensus/PoVBlockChain.o src/consensus/ConfigHelper.o src/consensus/POV.o src/consensus/FISCO-BCOS-GR-Sig/devcore/SHA3.o src/consensus/FISCO-BCOS-GR-Sig/devcore/easylog.o src/consensus/FISCO-BCOS-GR-Sig/devcore/easylogging++.o src/consensus/FISCO-BCOS-GR-Sig/algorithm/ring-sig/LinkableRingSig_Impl.o src/consensus/FISCO-BCOS-GR-Sig/algorithm/bbs04/GroupSig_BBS.o src/consensus/FISCO-BCOS-GR-Sig/algorithm/bbs04/GroupSig_BBS_Impl.o src/consensus/FISCO-BCOS-GR-Sig/algorithm/GroupSig.o src/consensus/FISCO-BCOS-GR-Sig/algorithm/KeyLoaderDumper.o src/consensus/FISCO-BCOS-GR-Sig/algorithm/RingSig.o src/communicate/dfnetwork.o src/communicate/udp.o src/communicate/network.o src/communicate/communicate.o src/config/config.o src/config/system_v_sm.o src/logstorage/db.o src/logstorage/paxos_log.o src/logstorage/log_store.o src/logstorage/system_variables_store.o src/sm-base/sm_base.o src/sm-base/sm.o src/communicate/tcp/event_base.o src/communicate/tcp/message_event.o src/communicate/tcp/event_loop.o src/communicate/tcp/tcp_client.o src/communicate/tcp/tcp_acceptor.o src/communicate/tcp/notify.o src/communicate/tcp/tcp.o src/comm/paxos_msg.pb.o src/comm/breakpoint.o src/comm/options.o src/comm/inside_options.o src/comm/logger.o src/utils/concurrent.o src/utils/socket.o src/utils/util.o src/utils/crc32.o src/utils/timer.o src/utils/bytes_buffer.o src/utils/serial_lock.o src/utils/wait_lock.o src/utils/notifier_pool.o)
$(SRC_BASE_PATH)/.lib/extlib/libphxpaxos.a: $(PHXPAXOS_LIB_OBJ)
	ar -cvq $@ $(PHXPAXOS_LIB_OBJ)

SUBDIRS=src plugin include sample

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
	rm -f $(SRC_BASE_PATH)/.lib/*.a $(SRC_BASE_PATH)/.lib/extlib/*.a $(SRC_BASE_PATH)/.sbin/*.a $(SRC_BASE_PATH)/lib/*.a
	rm -rf *.o *.pb.* $(SRC_BASE_PATH)/.lib/libphxpaxos.a $(SRC_BASE_PATH)/.lib/extlib/libphxpaxos.a libphxpaxos.a $(SRC_BASE_PATH)/.lib/libphxpaxos.a 

