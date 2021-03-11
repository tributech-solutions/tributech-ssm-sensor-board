################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Startup/subdir.mk
-include Libraries/XMCLib/src/subdir.mk
-include Libraries/WATCHDOG/subdir.mk
-include Libraries/USBH_VCOM/subdir.mk
-include Libraries/USB/GLUE/subdir.mk
-include Libraries/USB/Core/subdir.mk
-include Libraries/USB/Class/Host/subdir.mk
-include Libraries/Tributech_SSM_USB_Host_Library/subdir.mk
-include Libraries/RTC/subdir.mk
-include Libraries/Newlib/subdir.mk
-include Libraries/I2CMaster/subdir.mk
-include Libraries/DPS310PressureSensor/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
Tributech_SSM_Integration.hex \

SECONDARY_LIST += \
Tributech_SSM_Integration.lst \

SECONDARY_SIZE += \
Tributech_SSM_Integration.siz \


LIST_FILES = $(addsuffix .o.lst, $(basename $(OBJS))) 
MAP_FILE = "Tributech_SSM_Integration.map"

TOOLCHAIN_ROOT=C:/Program Files/DAVE-IDE-4.4.2-64Bit/eclipse/ARM-GCC-49
PROJECT_LOC=C:/Users/AndreasAscher/Documents/USB Host XMC4700/src/Tributech_SSM_Integration

# All Target
all: Tributech_SSM_Integration.elf secondary-outputs

# Tool invocations
Tributech_SSM_Integration.elf: $(OBJS) ../linker_script.ld $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM-GCC C Linker'
	$(file >objects.rsp,$(OBJS))
	"$(TOOLCHAIN_ROOT)/bin/arm-none-eabi-gcc" -T"../linker_script.ld" -nostartfiles -Xlinker --gc-sections -specs=nosys.specs -Wl,-Map,$(MAP_FILE) -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -mcpu=cortex-m4 -mthumb -o "Tributech_SSM_Integration.elf" "@objects.rsp" $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo.
Tributech_SSM_Integration.hex: Tributech_SSM_Integration.elf
	@echo 'Invoking: ARM-GCC Create Flash Image'
	"$(TOOLCHAIN_ROOT)/bin/arm-none-eabi-objcopy" -O ihex "Tributech_SSM_Integration.elf" "Tributech_SSM_Integration.hex"
	@echo 'Finished building: $@'
	@echo.
Tributech_SSM_Integration.lst: Tributech_SSM_Integration.elf
	@echo 'Invoking: ARM-GCC Create Listing'
	"$(TOOLCHAIN_ROOT)/bin/arm-none-eabi-objdump" -h -S "Tributech_SSM_Integration.elf" > "Tributech_SSM_Integration.lst"
	@echo 'Finished building: $@'
	@echo.
Tributech_SSM_Integration.siz: Tributech_SSM_Integration.elf
	@echo 'Invoking: ARM-GCC Print Size'
	"$(TOOLCHAIN_ROOT)/bin/arm-none-eabi-size" --format=berkeley "Tributech_SSM_Integration.elf"
	@echo 'Finished building: $@'
	@echo.
# Other Targets
clean:
	-$(RM) $(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(ARM_GCC_FOR_XMC_LINKER_OUTPUT_TYPE_OUTPUTS)$(S_DEPS)$(S_UPPER_DEPS)$(C_DEPS)
	-$(RM) Libraries/DPS310PressureSensor/dps310_config.o Libraries/DPS310PressureSensor/dps310_ctrl.o Libraries/I2CMaster/i2c_master.o Libraries/I2CMaster/i2c_master_conf.o Libraries/I2CMaster/i2c_master_ctrl.o Libraries/Newlib/syscalls.o Libraries/RTC/rtc.o Libraries/RTC/rtc_conf.o Libraries/Tributech_SSM_USB_Host_Library/Base64.o Libraries/Tributech_SSM_USB_Host_Library/Tributech_Lib.o Libraries/Tributech_SSM_USB_Host_Library/Tributech_SSM_API.o Libraries/Tributech_SSM_USB_Host_Library/USB_Host.o Libraries/Tributech_SSM_USB_Host_Library/jsmn.o Libraries/USB/Class/Host/CDCClassHost.o Libraries/USB/Class/Host/MassStorageClassHost.o Libraries/USB/Core/ConfigDescriptors.o Libraries/USB/Core/DeviceStandardReq.o Libraries/USB/Core/Events.o Libraries/USB/Core/HostStandardReq.o Libraries/USB/Core/USBTask.o Libraries/USB/GLUE/USB_Glue.o Libraries/USBH_VCOM/USBVCH.o Libraries/WATCHDOG/watchdog.o Libraries/WATCHDOG/watchdog_conf.o Libraries/XMCLib/src/xmc4_eru.o Libraries/XMCLib/src/xmc4_flash.o Libraries/XMCLib/src/xmc4_gpio.o Libraries/XMCLib/src/xmc4_rtc.o Libraries/XMCLib/src/xmc4_scu.o Libraries/XMCLib/src/xmc_can.o Libraries/XMCLib/src/xmc_ccu4.o Libraries/XMCLib/src/xmc_ccu8.o Libraries/XMCLib/src/xmc_common.o Libraries/XMCLib/src/xmc_dac.o Libraries/XMCLib/src/xmc_dma.o Libraries/XMCLib/src/xmc_dsd.o Libraries/XMCLib/src/xmc_ebu.o Libraries/XMCLib/src/xmc_ecat.o Libraries/XMCLib/src/xmc_eru.o Libraries/XMCLib/src/xmc_eth_mac.o Libraries/XMCLib/src/xmc_eth_phy_dp83848.o Libraries/XMCLib/src/xmc_eth_phy_ksz8031rnl.o Libraries/XMCLib/src/xmc_eth_phy_ksz8081rnb.o Libraries/XMCLib/src/xmc_fce.o Libraries/XMCLib/src/xmc_gpio.o Libraries/XMCLib/src/xmc_hrpwm.o Libraries/XMCLib/src/xmc_i2c.o Libraries/XMCLib/src/xmc_i2s.o Libraries/XMCLib/src/xmc_ledts.o Libraries/XMCLib/src/xmc_posif.o Libraries/XMCLib/src/xmc_rtc.o Libraries/XMCLib/src/xmc_sdmmc.o Libraries/XMCLib/src/xmc_spi.o Libraries/XMCLib/src/xmc_uart.o 
	-$(RM) Libraries/XMCLib/src/xmc_usbd.o Libraries/XMCLib/src/xmc_usbh.o Libraries/XMCLib/src/xmc_usic.o Libraries/XMCLib/src/xmc_vadc.o Libraries/XMCLib/src/xmc_wdt.o Startup/startup_XMC4700.o Startup/system_XMC4700.o main.o uart.o  Tributech_SSM_Integration.elf $(LIST_FILES) $(MAP_FILE) objects.rsp
	-@echo.
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
