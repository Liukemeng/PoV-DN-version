################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../main.cpp 

OBJS += \
./main.o 

CPP_DEPS += \
./main.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -std=c++11 -g -I/home/blackguess/workspace/C++/PoVBlockchain/phxpaxos -I/usr/local/include/mongocxx/v_noabi -I/usr/local/include/bsoncxx/v_noabi -I/home/blackguess/workspace/C++/PoVBlockchain/phxpaxos/src/consensus -I/home/blackguess/workspace/C++/PoVBlockchain/phxpaxos/third_party/protobuf/include -I/home/blackguess/workspace/C++/PoVBlockchain/phxpaxos/src/communicate/tcp -I/home/blackguess/workspace/C++/PoVBlockchain/phxpaxos/include -I/home/blackguess/workspace/C++/PoVBlockchain/phxpaxos/src -I/home/blackguess/workspace/C++/PoVBlockchain/phxpaxos/src/comm -I/home/blackguess/workspace/C++/PoVBlockchain/phxpaxos/src/communicate -I/home/blackguess/workspace/C++/PoVBlockchain/phxpaxos/third_party/protobuf -I/home/blackguess/workspace/C++/PoVBlockchain/phxpaxos/src/utils -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


