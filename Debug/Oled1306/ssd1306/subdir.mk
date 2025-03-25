################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Oled1306/ssd1306/ssd1306.c \
../Oled1306/ssd1306/ssd1306_fonts.c \
../Oled1306/ssd1306/ssd1306_tests.c 

OBJS += \
./Oled1306/ssd1306/ssd1306.o \
./Oled1306/ssd1306/ssd1306_fonts.o \
./Oled1306/ssd1306/ssd1306_tests.o 

C_DEPS += \
./Oled1306/ssd1306/ssd1306.d \
./Oled1306/ssd1306/ssd1306_fonts.d \
./Oled1306/ssd1306/ssd1306_tests.d 


# Each subdirectory must supply rules for building sources it contributes
Oled1306/ssd1306/%.o Oled1306/ssd1306/%.su Oled1306/ssd1306/%.cyclo: ../Oled1306/ssd1306/%.c Oled1306/ssd1306/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"O:/STM/Workspace/AWP/project/Oled1306/ssd1306" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Oled1306-2f-ssd1306

clean-Oled1306-2f-ssd1306:
	-$(RM) ./Oled1306/ssd1306/ssd1306.cyclo ./Oled1306/ssd1306/ssd1306.d ./Oled1306/ssd1306/ssd1306.o ./Oled1306/ssd1306/ssd1306.su ./Oled1306/ssd1306/ssd1306_fonts.cyclo ./Oled1306/ssd1306/ssd1306_fonts.d ./Oled1306/ssd1306/ssd1306_fonts.o ./Oled1306/ssd1306/ssd1306_fonts.su ./Oled1306/ssd1306/ssd1306_tests.cyclo ./Oled1306/ssd1306/ssd1306_tests.d ./Oled1306/ssd1306/ssd1306_tests.o ./Oled1306/ssd1306/ssd1306_tests.su

.PHONY: clean-Oled1306-2f-ssd1306

