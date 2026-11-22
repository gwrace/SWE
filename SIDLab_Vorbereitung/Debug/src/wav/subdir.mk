################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/wav/Wav.cpp \
../src/wav/tinywav.cpp 

CPP_DEPS += \
./src/wav/Wav.d \
./src/wav/tinywav.d 

OBJS += \
./src/wav/Wav.o \
./src/wav/tinywav.o 


# Each subdirectory must supply rules for building sources it contributes
src/wav/%.o: ../src/wav/%.cpp src/wav/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++2a -O0 -g3 -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-wav

clean-src-2f-wav:
	-$(RM) ./src/wav/Wav.d ./src/wav/Wav.o ./src/wav/tinywav.d ./src/wav/tinywav.o

.PHONY: clean-src-2f-wav

