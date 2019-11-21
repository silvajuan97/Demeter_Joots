################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c 

OBJS += \
./Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.o 

C_DEPS += \
./Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/%.o: ../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32F407VGTx -DSTM32F4 -DSTM32F4DISCOVERY -DSTM32 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -DUSE_RTOS_SYSTICK -I"C:/Users/juanj/Downloads/SE/stm32f4discovery_hal_lib" -I"C:/Users/juanj/Downloads/SE/LIB_MSE-SE/inc" -I"C:/Users/juanj/Downloads/DE/modulo-central/inc" -I"C:/Users/juanj/Downloads/SE/stm32f4discovery_hal_lib/CMSIS/core" -I"C:/Users/juanj/Downloads/SE/stm32f4discovery_hal_lib/CMSIS/device" -I"C:/Users/juanj/Downloads/SE/stm32f4discovery_hal_lib/HAL_Driver/Inc/Legacy" -I"C:/Users/juanj/Downloads/SE/stm32f4discovery_hal_lib/HAL_Driver/Inc" -I"C:/Users/juanj/Downloads/SE/stm32f4discovery_hal_lib/Utilities/Components/ampire480272" -I"C:/Users/juanj/Downloads/SE/stm32f4discovery_hal_lib/Utilities/Components/ampire640480" -I"C:/Users/juanj/Downloads/SE/stm32f4discovery_hal_lib/Utilities/Components/Common" -I"C:/Users/juanj/Downloads/SE/stm32f4discovery_hal_lib/Utilities/Components/cs43l22" -I"C:/Users/juanj/Downloads/SE/stm32f4discovery_hal_lib/Utilities/Components/exc7200" -I"C:/Users/juanj/Downloads/SE/stm32f4discovery_hal_lib/Utilities/Components/ft6x06" -I"C:/Users/juanj/Downloads/SE/stm32f4discovery_hal_lib/Utilities/Components/ili9325" -I"C:/Users/juanj/Downloads/SE/stm32f4discovery_hal_lib/Utilities/Components/ili9341" -I"C:/Users/juanj/Downloads/SE/stm32f4discovery_hal_lib/Utilities/Components/l3gd20" -I"C:/Users/juanj/Downloads/SE/stm32f4discovery_hal_lib/Utilities/Components/lis302dl" -I"C:/Users/juanj/Downloads/SE/stm32f4discovery_hal_lib/Utilities/Components/lis3dsh" -I"C:/Users/juanj/Downloads/SE/stm32f4discovery_hal_lib/Utilities/Components/lsm303dlhc" -I"C:/Users/juanj/Downloads/SE/stm32f4discovery_hal_lib/Utilities/Components/mfxstm32l152" -I"C:/Users/juanj/Downloads/SE/stm32f4discovery_hal_lib/Utilities/Components/n25q128a" -I"C:/Users/juanj/Downloads/SE/stm32f4discovery_hal_lib/Utilities/Components/n25q256a" -I"C:/Users/juanj/Downloads/SE/stm32f4discovery_hal_lib/Utilities/Components/n25q512a" -I"C:/Users/juanj/Downloads/SE/stm32f4discovery_hal_lib/Utilities/Components/otm8009a" -I"C:/Users/juanj/Downloads/SE/stm32f4discovery_hal_lib/Utilities/Components/ov2640" -I"C:/Users/juanj/Downloads/SE/stm32f4discovery_hal_lib/Utilities/Components/s25fl512s" -I"C:/Users/juanj/Downloads/SE/stm32f4discovery_hal_lib/Utilities/Components/s5k5cag" -I"C:/Users/juanj/Downloads/SE/stm32f4discovery_hal_lib/Utilities/Components/st7735" -I"C:/Users/juanj/Downloads/SE/stm32f4discovery_hal_lib/Utilities/Components/stmpe1600" -I"C:/Users/juanj/Downloads/SE/stm32f4discovery_hal_lib/Utilities/Components/stmpe811" -I"C:/Users/juanj/Downloads/SE/stm32f4discovery_hal_lib/Utilities/Components/ts3510" -I"C:/Users/juanj/Downloads/SE/stm32f4discovery_hal_lib/Utilities/Components/wm8994" -I"C:/Users/juanj/Downloads/SE/stm32f4discovery_hal_lib/Utilities" -I"C:/Users/juanj/Downloads/SE/stm32f4discovery_hal_lib/Utilities/STM32F4-Discovery" -I"C:/Users/juanj/Downloads/DE/modulo-central/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"C:/Users/juanj/Downloads/DE/modulo-central/Middlewares/Third_Party/FreeRTOS/Source/include" -I"C:/Users/juanj/Downloads/DE/modulo-central/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


