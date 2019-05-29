################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/state/StateManger.cpp \
../src/state/state.cpp \
../src/state/state_AutoMtd.cpp \
../src/state/state_Box.cpp \
../src/state/state_manualMtd.cpp \
../src/state/state_ptz.cpp \
../src/state/state_scene.cpp 

OBJS += \
./src/state/StateManger.o \
./src/state/state.o \
./src/state/state_AutoMtd.o \
./src/state/state_Box.o \
./src/state/state_manualMtd.o \
./src/state/state_ptz.o \
./src/state/state_scene.o 

CPP_DEPS += \
./src/state/StateManger.d \
./src/state/state.d \
./src/state/state_AutoMtd.d \
./src/state/state_Box.d \
./src/state/state_manualMtd.d \
./src/state/state_ptz.d \
./src/state/state_scene.d 


# Each subdirectory must supply rules for building sources it contributes
src/state/%.o: ../src/state/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-8.0/bin/nvcc -I/usr/local/include -I../src/OSA_CAP/inc -I../inc/ -I../src/ptz/inc -I../src/DxTimer/include -I../src/port -I../src/comm -I../src/platform -I../src/IPC -G -g -O0 -ccbin aarch64-linux-gnu-g++ -gencode arch=compute_53,code=sm_53 -m64 -odir "src/state" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-8.0/bin/nvcc -I/usr/local/include -I../src/OSA_CAP/inc -I../inc/ -I../src/ptz/inc -I../src/DxTimer/include -I../src/port -I../src/comm -I../src/platform -I../src/IPC -G -g -O0 --compile -m64 -ccbin aarch64-linux-gnu-g++  -x c++ -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


