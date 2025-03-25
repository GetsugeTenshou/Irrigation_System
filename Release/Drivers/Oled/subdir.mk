################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Oled/ssd1306.c \
../Drivers/Oled/ssd1306_fonts.c 

OBJS += \
./Drivers/Oled/ssd1306.o \
./Drivers/Oled/ssd1306_fonts.o 

C_DEPS += \
./Drivers/Oled/ssd1306.d \
./Drivers/Oled/ssd1306_fonts.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Oled/%.o Drivers/Oled/%.su Drivers/Oled/%.cyclo: ../Drivers/Oled/%.c Drivers/Oled/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Drivers-2f-Oled

clean-Drivers-2f-Oled:
	-$(RM) ./Drivers/Oled/ssd1306.cyclo ./Drivers/Oled/ssd1306.d ./Drivers/Oled/ssd1306.o ./Drivers/Oled/ssd1306.su ./Drivers/Oled/ssd1306_fonts.cyclo ./Drivers/Oled/ssd1306_fonts.d ./Drivers/Oled/ssd1306_fonts.o ./Drivers/Oled/ssd1306_fonts.su

.PHONY: clean-Drivers-2f-Oled

