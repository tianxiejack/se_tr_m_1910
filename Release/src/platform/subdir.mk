################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/platform/Kalman.cpp \
../src/platform/deviceUser.cpp \
../src/platform/platformControl.cpp \
../src/platform/platformFilter.cpp \
../src/platform/sensorComp.cpp 

OBJS += \
./src/platform/Kalman.o \
./src/platform/deviceUser.o \
./src/platform/platformControl.o \
./src/platform/platformFilter.o \
./src/platform/sensorComp.o 

CPP_DEPS += \
./src/platform/Kalman.d \
./src/platform/deviceUser.d \
./src/platform/platformControl.d \
./src/platform/platformFilter.d \
./src/platform/sensorComp.d 


# Each subdirectory must supply rules for building sources it contributes
src/platform/%.o: ../src/platform/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-8.0/bin/nvcc -I/usr/local/include -I../src/OSA_CAP/inc -I../inc/ -I../src/ptz/inc -I../src/DxTimer/include -I../src/port -I../src/comm -I../src/platform -I../src/IPC -O3 -ccbin aarch64-linux-gnu-g++ -gencode arch=compute_53,code=sm_53 -m64 -odir "src/platform" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-8.0/bin/nvcc -I/usr/local/include -I../src/OSA_CAP/inc -I../inc/ -I../src/ptz/inc -I../src/DxTimer/include -I../src/port -I../src/comm -I../src/platform -I../src/IPC -O3 --compile -m64 -ccbin aarch64-linux-gnu-g++  -x c++ -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


