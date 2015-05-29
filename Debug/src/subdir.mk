################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/debug.c \
../src/hash.c \
../src/label.c \
../src/line.c \
../src/myvicar.c 

OBJS += \
./src/debug.o \
./src/hash.o \
./src/label.o \
./src/line.o \
./src/myvicar.o 

C_DEPS += \
./src/debug.d \
./src/hash.d \
./src/label.d \
./src/line.d \
./src/myvicar.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


