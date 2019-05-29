################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/ptz/src/ExtensionFormat.cpp \
../src/ptz/src/PTZ_control.cpp \
../src/ptz/src/PTZ_speedTransfer.cpp \
../src/ptz/src/pelcoDformat.cpp \
../src/ptz/src/pelcoPformat.cpp 

OBJS += \
./src/ptz/src/ExtensionFormat.o \
./src/ptz/src/PTZ_control.o \
./src/ptz/src/PTZ_speedTransfer.o \
./src/ptz/src/pelcoDformat.o \
./src/ptz/src/pelcoPformat.o 

CPP_DEPS += \
./src/ptz/src/ExtensionFormat.d \
./src/ptz/src/PTZ_control.d \
./src/ptz/src/PTZ_speedTransfer.d \
./src/ptz/src/pelcoDformat.d \
./src/ptz/src/pelcoPformat.d 


# Each subdirectory must supply rules for building sources it contributes
src/ptz/src/%.o: ../src/ptz/src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-8.0/bin/nvcc -I/usr/local/include -I../src/OSA_CAP/inc -I../inc/ -I../src/ptz/inc -I../src/DxTimer/include -I../src/port -I../src/comm -I../src/platform -I../src/IPC -G -g -O0 -std=c++11 -ccbin aarch64-linux-gnu-g++ -gencode arch=compute_53,code=sm_53 -m64 -odir "src/ptz/src" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-8.0/bin/nvcc -I/usr/local/include -I../src/OSA_CAP/inc -I../inc/ -I../src/ptz/inc -I../src/DxTimer/include -I../src/port -I../src/comm -I../src/platform -I../src/IPC -G -g -O0 -std=c++11 --compile -m64 -ccbin aarch64-linux-gnu-g++  -x c++ -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


