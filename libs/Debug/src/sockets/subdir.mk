################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/sockets/sockets.c 

OBJS += \
./src/sockets/sockets.o 

C_DEPS += \
./src/sockets/sockets.d 


# Each subdirectory must supply rules for building sources it contributes
src/sockets/%.o: ../src/sockets/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


