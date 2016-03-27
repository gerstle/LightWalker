################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -f

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include arduino/core/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
AR += \
arduino.ar \

ELF += \
MicTest.elf \

EEP += \
MicTest.eep \

FLASH_IMAGE += \
MicTest.hex \


# All Target
all: MicTest

# Tool invocations
MicTest: $(FLASH_IMAGE)
	@echo 'Building target: $@'
	@echo 'Printing size:'
	"/Applications/Arduino.app/Contents/Java/hardware/tools/arm/bin/arm-none-eabi-size" -A "/Users/cgerstle/Documents/workspace/MicTest/Release/MicTest.elf"
	@echo 'Finished building target: $@'
	@echo ' '

arduino.ar: $(AR_OBJ)
	@echo 'Starting archiver'
	"/Applications/Arduino.app/Contents/Java/hardware/tools/arm/bin/arm-none-eabi-ar" rcs "/Users/cgerstle/Documents/workspace/MicTest/Release/arduino.ar"   $(AR_OBJ)
	@echo 'Finished building: $@'
	@echo ' '

MicTest.elf: $(LINK_OBJ) $(AR)
	@echo 'Starting combiner'
	"/Applications/Arduino.app/Contents/Java/hardware/tools/arm/bin/arm-none-eabi-gcc" -Os -Wl,--gc-sections,--relax,--defsym=__rtc_localtime=1459011771 "-T/Applications/Arduino.app/Contents/Java/hardware/teensy/avr/cores/teensy3/mk20dx256.ld" --specs=nano.specs -mthumb -mcpu=cortex-m4 -fsingle-precision-constant -o "/Users/cgerstle/Documents/workspace/MicTest/Release/MicTest.elf"   $(LINK_OBJ) $(AR)  "/Users/cgerstle/Documents/workspace/MicTest/Release/arduino.ar" "-L/Users/cgerstle/Documents/workspace/MicTest/Release" -larm_cortexM4l_math -lm
	@echo 'Finished building: $@'
	@echo ' '

MicTest.eep: $(ELF)
	@echo 'Create eeprom image'
	"/Applications/Arduino.app/Contents/Java/hardware/tools/arm/bin/arm-none-eabi-objcopy" -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0 "/Users/cgerstle/Documents/workspace/MicTest/Release/MicTest.elf" "/Users/cgerstle/Documents/workspace/MicTest/Release/MicTest.eep"
	@echo 'Finished building: $@'
	@echo ' '

MicTest.hex: $(EEP)
	@echo 'Create Flash image (ihex format)'
	"/Applications/Arduino.app/Contents/Java/hardware/tools/arm/bin/arm-none-eabi-objcopy" -O ihex -R .eeprom "/Users/cgerstle/Documents/workspace/MicTest/Release/MicTest.elf" "/Users/cgerstle/Documents/workspace/MicTest/Release/MicTest.hex"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C_UPPER_DEPS)$(C_DEPS)$(AR)$(EEP)$(FLASH_IMAGE)$(CC_DEPS)$(AR_OBJ)$(C++_DEPS)$(LINK_OBJ)$(CXX_DEPS)$(ASM_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(ELF)$(CPP_DEPS) MicTest
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
