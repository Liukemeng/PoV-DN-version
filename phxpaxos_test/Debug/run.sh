#!/bin/bash
#gnome-terminal --name "node1" -x bash --tab -e "./phxpaxos_test /home/blackguess/workspace/C++/#phxpaxos_test/Debug/Config;exec bash;"
#gnome-terminal --name "node2" -x bash --tab -e "./phxpaxos_test /home/blackguess/workspace/C++/#phxpaxos_test/Debug/Config1;exec bash;"
#gnome-terminal --name "node3" -x bash --tab -e "./phxpaxos_test /home/blackguess/workspace/C++/#phxpaxos_test/Debug/Config2;exec bash;"
./phxpaxos_test Config;
./phxpaxos_test Config1;
./phxpaxos_test Config2;
