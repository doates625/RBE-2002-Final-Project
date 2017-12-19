################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ..\makefile.init

RM := del 

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include libraries\Wire\src\utility\subdir.mk
-include libraries\Wire\src\subdir.mk
-include libraries\TimerOne\subdir.mk
-include libraries\Timer\subdir.mk
-include libraries\Servo\src\stm32f4\subdir.mk
-include libraries\Servo\src\samd\subdir.mk
-include libraries\Servo\src\sam\subdir.mk
-include libraries\Servo\src\nrf52\subdir.mk
-include libraries\Servo\src\avr\subdir.mk
-include libraries\PidController\subdir.mk
-include libraries\OpenLoopServo\subdir.mk
-include libraries\LinearAtmel\VecExpr\VecScalarExpr\subdir.mk
-include libraries\LinearAtmel\VecExpr\VecMatProd\subdir.mk
-include libraries\LinearAtmel\VecExpr\VecBinaryExpr\subdir.mk
-include libraries\LinearAtmel\VecExpr\Vec\subdir.mk
-include libraries\LinearAtmel\VecExpr\subdir.mk
-include libraries\LinearAtmel\MatExpr\MatUnaryExpr\subdir.mk
-include libraries\LinearAtmel\MatExpr\MatSimple\subdir.mk
-include libraries\LinearAtmel\MatExpr\MatScalarExpr\subdir.mk
-include libraries\LinearAtmel\MatExpr\MatRref\subdir.mk
-include libraries\LinearAtmel\MatExpr\MatIndex\subdir.mk
-include libraries\LinearAtmel\MatExpr\MatBinaryExpr\MatVcat\subdir.mk
-include libraries\LinearAtmel\MatExpr\MatBinaryExpr\MatProd\subdir.mk
-include libraries\LinearAtmel\MatExpr\MatBinaryExpr\MatHcat\subdir.mk
-include libraries\LinearAtmel\MatExpr\MatBinaryExpr\subdir.mk
-include libraries\LinearAtmel\MatExpr\Mat\subdir.mk
-include libraries\LinearAtmel\MatExpr\subdir.mk
-include libraries\LinearAtmel\subdir.mk
-include libraries\Led\subdir.mk
-include libraries\ISquaredC\subdir.mk
-include libraries\HcSr04Array\subdir.mk
-include libraries\HcSr04\subdir.mk
-include libraries\Hc06\subdir.mk
-include libraries\DcMotor\subdir.mk
-include libraries\BrushlessMotor\subdir.mk
-include libraries\Bno055\subdir.mk
-include libraries\BinarySerial\subdir.mk
-include core\core\subdir.mk
-include Namespaces\WallFollower\subdir.mk
-include Namespaces\Sonar\subdir.mk
-include Namespaces\RobotDims\subdir.mk
-include Namespaces\PanTilt\subdir.mk
-include Namespaces\Odometer\subdir.mk
-include Namespaces\MotorR\subdir.mk
-include Namespaces\MotorL\subdir.mk
-include Namespaces\MatlabComms\subdir.mk
-include Namespaces\IndicatorLed\subdir.mk
-include Namespaces\FireBot\subdir.mk
-include Namespaces\DriveSystem\subdir.mk
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
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
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

-include ..\makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
AR += \
C:\Users\Dan_2\Desktop\RBE-2002-Final-Project\Software\MainBoard\Release\arduino.ar \

ELF += \
MainBoard.elf \

HEX += \
MainBoard.hex \


# All Target
all: MainBoard

# Tool invocations
MainBoard: $(HEX)
	@echo 'Building target: $@'
	@echo 'Printing size:'
	"C:\sloeber\/arduinoPlugin/packages/arduino/tools/avr-gcc/4.9.2-atmel3.5.4-arduino2/bin/avr-size" -A "C:/Users/Dan_2/Desktop/RBE-2002-Final-Project/Software/MainBoard/Release/MainBoard.elf"
	@echo 'Finished building target: $@'
	@echo ' '

C:\Users\Dan_2\Desktop\RBE-2002-Final-Project\Software\MainBoard\Release\arduino.ar: $(AR_OBJ)
	@echo 'Starting archiver'
	"C:\sloeber\/arduinoPlugin/packages/arduino/tools/avr-gcc/4.9.2-atmel3.5.4-arduino2/bin/avr-gcc-ar" rcs    C:/Users/Dan_2/Desktop/RBE-2002-Final-Project/Software/MainBoard/Release/arduino.ar $(AR_OBJ)
	@echo 'Finished building: $@'
	@echo ' '

MainBoard.elf: $(LINK_OBJ) $(AR)
	@echo 'Starting combiner'
	"C:\sloeber\/arduinoPlugin/packages/arduino/tools/avr-gcc/4.9.2-atmel3.5.4-arduino2/bin/avr-gcc" -Wall -Wextra -Os -Wl,--gc-sections,--relax -mmcu=atmega2560 -o "C:\Users\Dan_2\Desktop\RBE-2002-Final-Project\Software\MainBoard/Release/MainBoard.elf"   $(LINK_OBJ) $(AR)   C:/Users/Dan_2/Desktop/RBE-2002-Final-Project/Software/MainBoard/Release/arduino.ar  "-LC:\Users\Dan_2\Desktop\RBE-2002-Final-Project\Software\MainBoard/Release" -lm
	@echo 'Finished building: $@'
	@echo ' '

MainBoard.hex: $(ELF)
	@echo 'Create eeprom image'
	${A.RECIPE.OBJCOPY.EEP.PATTERN}
	${A.RECIPE.OBJCOPY.HEX.PATTERN}
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C_UPPER_DEPS)$(PDE_DEPS)$(C_DEPS)$(AR)$(CC_DEPS)$(AR_OBJ)$(C++_DEPS)$(LINK_OBJ)$(CXX_DEPS)$(ASM_DEPS)$(HEX)$(INO_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(ELF)$(CPP_DEPS)
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ..\makefile.targets
