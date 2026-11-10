################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/sid/Filter.cpp \
../src/sid/FilterBank.cpp \
../src/sid/SID.cpp \
../src/sid/Voice.cpp \
../src/sid/Waveform.cpp \
../src/sid/sid_types.cpp 

CPP_DEPS += \
./src/sid/Filter.d \
./src/sid/FilterBank.d \
./src/sid/SID.d \
./src/sid/Voice.d \
./src/sid/Waveform.d \
./src/sid/sid_types.d 

OBJS += \
./src/sid/Filter.o \
./src/sid/FilterBank.o \
./src/sid/SID.o \
./src/sid/Voice.o \
./src/sid/Waveform.o \
./src/sid/sid_types.o 


# Each subdirectory must supply rules for building sources it contributes
src/sid/%.o: ../src/sid/%.cpp src/sid/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++2a -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-sid

clean-src-2f-sid:
	-$(RM) ./src/sid/Filter.d ./src/sid/Filter.o ./src/sid/FilterBank.d ./src/sid/FilterBank.o ./src/sid/SID.d ./src/sid/SID.o ./src/sid/Voice.d ./src/sid/Voice.o ./src/sid/Waveform.d ./src/sid/Waveform.o ./src/sid/sid_types.d ./src/sid/sid_types.o

.PHONY: clean-src-2f-sid

