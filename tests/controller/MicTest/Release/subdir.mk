################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Ema.cpp \
../MicTest.cpp 

LINK_OBJ += \
./Ema.cpp.o \
./MicTest.cpp.o 

CPP_DEPS += \
./Ema.cpp.d \
./MicTest.cpp.d 


# Each subdirectory must supply rules for building sources it contributes
Ema.cpp.o: ../Ema.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"/Applications/Arduino.app/Contents/Java/hardware/tools/arm/bin/arm-none-eabi-g++" -c -Os -g -Wall -ffunction-sections -fdata-sections -MMD -nostdlib -fno-exceptions -felide-constructors -std=gnu++0x -fno-rtti -mthumb -mcpu=cortex-m4 -fsingle-precision-constant -D__MK20DX256__ -DTEENSYDUINO=127 -DARDUINO=10605 -DF_CPU=96000000 -DARDUINO_ARCH_AVR -DUSB_SERIAL -DLAYOUT_US_ENGLISH   -I"/Applications/Arduino.app/Contents/Java/hardware/teensy/avr/cores/teensy3" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"   -Wall
	@echo 'Finished building: $<'
	@echo ' '

MicTest.cpp.o: ../MicTest.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"/Applications/Arduino.app/Contents/Java/hardware/tools/arm/bin/arm-none-eabi-g++" -c -Os -g -Wall -ffunction-sections -fdata-sections -MMD -nostdlib -fno-exceptions -felide-constructors -std=gnu++0x -fno-rtti -mthumb -mcpu=cortex-m4 -fsingle-precision-constant -D__MK20DX256__ -DTEENSYDUINO=127 -DARDUINO=10605 -DF_CPU=96000000 -DARDUINO_ARCH_AVR -DUSB_SERIAL -DLAYOUT_US_ENGLISH   -I"/Applications/Arduino.app/Contents/Java/hardware/teensy/avr/cores/teensy3" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"   -Wall
	@echo 'Finished building: $<'
	@echo ' '


