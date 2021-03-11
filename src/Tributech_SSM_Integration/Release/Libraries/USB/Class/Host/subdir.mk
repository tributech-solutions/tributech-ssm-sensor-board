################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libraries/USB/Class/Host/CDCClassHost.c \
../Libraries/USB/Class/Host/MassStorageClassHost.c 

OBJS += \
./Libraries/USB/Class/Host/CDCClassHost.o \
./Libraries/USB/Class/Host/MassStorageClassHost.o 

C_DEPS += \
./Libraries/USB/Class/Host/CDCClassHost.d \
./Libraries/USB/Class/Host/MassStorageClassHost.d 


# Each subdirectory must supply rules for building sources it contributes
Libraries/USB/Class/Host/%.o: ../Libraries/USB/Class/Host/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM-GCC C Compiler'
	"$(TOOLCHAIN_ROOT)/bin/arm-none-eabi-gcc" -MMD -MT "$@" -DXMC4700_F144x2048 -I"$(PROJECT_LOC)/Libraries/XMCLib/inc" -I"$(PROJECT_LOC)/Libraries/CMSIS/Include" -I"$(PROJECT_LOC)/Libraries/CMSIS/Infineon/XMC4700_series/Include" -I"$(PROJECT_LOC)" -I"$(PROJECT_LOC)/Libraries" -I"$(PROJECT_LOC)/Libraries/USB" -I"$(PROJECT_LOC)/Libraries/Tributech_SSM_USB_Host_Library" -I"$(PROJECT_LOC)/Libraries/RTC" -I"$(PROJECT_LOC)/Libraries/DPS310PressureSensor" -I"$(PROJECT_LOC)/Libraries/I2CMaster" -I"$(PROJECT_LOC)/Libraries/WATCHDOG" -I"$(PROJECT_LOC)/Libraries/USB/GLUE" -Os -ffunction-sections -fdata-sections -Wall -std=gnu99 -mfloat-abi=softfp -Wa,-adhlns="$@.lst" -pipe -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d) $@" -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mthumb -o "$@" "$<" 
	@echo 'Finished building: $<'
	@echo.

