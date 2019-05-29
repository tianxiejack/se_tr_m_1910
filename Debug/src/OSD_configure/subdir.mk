################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/OSD_configure/osd.cpp 

OBJS += \
./src/OSD_configure/osd.o 

CPP_DEPS += \
./src/OSD_configure/osd.d 


# Each subdirectory must supply rules for building sources it contributes
src/OSD_configure/%.o: ../src/OSD_configure/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: NVCC Compiler'
	/usr/local/cuda-8.0/bin/nvcc -I/usr/local/include -I../src/OSA_CAP/inc -I../inc/ -I../src/ptz/inc -I../src/DxTimer/include -I../src/port -I../src/comm -I../src/platform -I../src/IPC -I../src/algorithm_configure -I../src/display_configure -I../src/OSD_configure -G -g -O0 -std=c++11 -ccbin aarch64-linux-gnu-g++ -gencode arch=compute_53,code=sm_53 -m64 -odir "src/OSD_configure" -M -o "$(@:%.o=%.d)" "$<"
	/usr/local/cuda-8.0/bin/nvcc -I/usr/local/include -I../src/OSA_CAP/inc -I../inc/ -I../src/ptz/inc -I../src/DxTimer/include -I../src/port -I../src/comm -I../src/platform -I../src/IPC -I../src/algorithm_configure -I../src/display_configure -I../src/OSD_configure -G -g -O0 -std=c++11 --compile -m64 -ccbin aarch64-linux-gnu-g++  -x c++ -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


