################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include sockets1/subdir.mk
-include parser/subdir.mk
-include generales/subdir.mk
-include commons/collections/subdir.mk
-include commons/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: liblibs.so

dependents:
	-cd /home/utnso/workspace/tp-2018-2c-keAprobo/libs/Release && $(MAKE) all

# Tool invocations
liblibs.so: $(OBJS) $(USER_OBJS) /home/utnso/workspace/tp-2018-2c-keAprobo/libs/Release/liblibs.so
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -shared -o "liblibs.so" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(LIBRARIES)$(OBJS)$(C_DEPS) liblibs.so
	-@echo ' '

.PHONY: all clean dependents
/home/utnso/workspace/tp-2018-2c-keAprobo/libs/Release/liblibs.so:

-include ../makefile.targets
