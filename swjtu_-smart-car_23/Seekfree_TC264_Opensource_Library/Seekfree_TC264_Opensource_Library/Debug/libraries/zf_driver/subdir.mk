################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libraries/zf_driver/zf_driver_adc.c \
../libraries/zf_driver/zf_driver_delay.c \
../libraries/zf_driver/zf_driver_dma.c \
../libraries/zf_driver/zf_driver_encoder.c \
../libraries/zf_driver/zf_driver_exti.c \
../libraries/zf_driver/zf_driver_flash.c \
../libraries/zf_driver/zf_driver_gpio.c \
../libraries/zf_driver/zf_driver_pit.c \
../libraries/zf_driver/zf_driver_pwm.c \
../libraries/zf_driver/zf_driver_soft_iic.c \
../libraries/zf_driver/zf_driver_soft_spi.c \
../libraries/zf_driver/zf_driver_spi.c \
../libraries/zf_driver/zf_driver_timer.c \
../libraries/zf_driver/zf_driver_uart.c 

COMPILED_SRCS += \
./libraries/zf_driver/zf_driver_adc.src \
./libraries/zf_driver/zf_driver_delay.src \
./libraries/zf_driver/zf_driver_dma.src \
./libraries/zf_driver/zf_driver_encoder.src \
./libraries/zf_driver/zf_driver_exti.src \
./libraries/zf_driver/zf_driver_flash.src \
./libraries/zf_driver/zf_driver_gpio.src \
./libraries/zf_driver/zf_driver_pit.src \
./libraries/zf_driver/zf_driver_pwm.src \
./libraries/zf_driver/zf_driver_soft_iic.src \
./libraries/zf_driver/zf_driver_soft_spi.src \
./libraries/zf_driver/zf_driver_spi.src \
./libraries/zf_driver/zf_driver_timer.src \
./libraries/zf_driver/zf_driver_uart.src 

C_DEPS += \
./libraries/zf_driver/zf_driver_adc.d \
./libraries/zf_driver/zf_driver_delay.d \
./libraries/zf_driver/zf_driver_dma.d \
./libraries/zf_driver/zf_driver_encoder.d \
./libraries/zf_driver/zf_driver_exti.d \
./libraries/zf_driver/zf_driver_flash.d \
./libraries/zf_driver/zf_driver_gpio.d \
./libraries/zf_driver/zf_driver_pit.d \
./libraries/zf_driver/zf_driver_pwm.d \
./libraries/zf_driver/zf_driver_soft_iic.d \
./libraries/zf_driver/zf_driver_soft_spi.d \
./libraries/zf_driver/zf_driver_spi.d \
./libraries/zf_driver/zf_driver_timer.d \
./libraries/zf_driver/zf_driver_uart.d 

OBJS += \
./libraries/zf_driver/zf_driver_adc.o \
./libraries/zf_driver/zf_driver_delay.o \
./libraries/zf_driver/zf_driver_dma.o \
./libraries/zf_driver/zf_driver_encoder.o \
./libraries/zf_driver/zf_driver_exti.o \
./libraries/zf_driver/zf_driver_flash.o \
./libraries/zf_driver/zf_driver_gpio.o \
./libraries/zf_driver/zf_driver_pit.o \
./libraries/zf_driver/zf_driver_pwm.o \
./libraries/zf_driver/zf_driver_soft_iic.o \
./libraries/zf_driver/zf_driver_soft_spi.o \
./libraries/zf_driver/zf_driver_spi.o \
./libraries/zf_driver/zf_driver_timer.o \
./libraries/zf_driver/zf_driver_uart.o 


# Each subdirectory must supply rules for building sources it contributes
libraries/zf_driver/%.src: ../libraries/zf_driver/%.c libraries/zf_driver/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING C/C++ Compiler'
	cctc -cs --dep-file="$(basename $@).d" --misrac-version=2004 -D__CPU__=tc26xb "-fC:/Users/admin/Desktop/smartcar/smart_car/swjtu_-smart-car_23/Seekfree_TC264_Opensource_Library/Seekfree_TC264_Opensource_Library/Debug/TASKING_C_C___Compiler-Include_paths.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<" && \
	if [ -f "$(basename $@).d" ]; then sed.exe -r  -e 's/\b(.+\.o)\b/libraries\/zf_driver\/\1/g' -e 's/\\/\//g' -e 's/\/\//\//g' -e 's/"//g' -e 's/([a-zA-Z]:\/)/\L\1/g' -e 's/\d32:/@TARGET_DELIMITER@/g; s/\\\d32/@ESCAPED_SPACE@/g; s/\d32/\\\d32/g; s/@ESCAPED_SPACE@/\\\d32/g; s/@TARGET_DELIMITER@/\d32:/g' "$(basename $@).d" > "$(basename $@).d_sed" && cp "$(basename $@).d_sed" "$(basename $@).d" && rm -f "$(basename $@).d_sed" 2>/dev/null; else echo 'No dependency file to process';fi
	@echo 'Finished building: $<'
	@echo ' '

libraries/zf_driver/%.o: ./libraries/zf_driver/%.src libraries/zf_driver/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING Assembler'
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-libraries-2f-zf_driver

clean-libraries-2f-zf_driver:
	-$(RM) ./libraries/zf_driver/zf_driver_adc.d ./libraries/zf_driver/zf_driver_adc.o ./libraries/zf_driver/zf_driver_adc.src ./libraries/zf_driver/zf_driver_delay.d ./libraries/zf_driver/zf_driver_delay.o ./libraries/zf_driver/zf_driver_delay.src ./libraries/zf_driver/zf_driver_dma.d ./libraries/zf_driver/zf_driver_dma.o ./libraries/zf_driver/zf_driver_dma.src ./libraries/zf_driver/zf_driver_encoder.d ./libraries/zf_driver/zf_driver_encoder.o ./libraries/zf_driver/zf_driver_encoder.src ./libraries/zf_driver/zf_driver_exti.d ./libraries/zf_driver/zf_driver_exti.o ./libraries/zf_driver/zf_driver_exti.src ./libraries/zf_driver/zf_driver_flash.d ./libraries/zf_driver/zf_driver_flash.o ./libraries/zf_driver/zf_driver_flash.src ./libraries/zf_driver/zf_driver_gpio.d ./libraries/zf_driver/zf_driver_gpio.o ./libraries/zf_driver/zf_driver_gpio.src ./libraries/zf_driver/zf_driver_pit.d ./libraries/zf_driver/zf_driver_pit.o ./libraries/zf_driver/zf_driver_pit.src ./libraries/zf_driver/zf_driver_pwm.d ./libraries/zf_driver/zf_driver_pwm.o ./libraries/zf_driver/zf_driver_pwm.src ./libraries/zf_driver/zf_driver_soft_iic.d ./libraries/zf_driver/zf_driver_soft_iic.o ./libraries/zf_driver/zf_driver_soft_iic.src ./libraries/zf_driver/zf_driver_soft_spi.d ./libraries/zf_driver/zf_driver_soft_spi.o ./libraries/zf_driver/zf_driver_soft_spi.src ./libraries/zf_driver/zf_driver_spi.d ./libraries/zf_driver/zf_driver_spi.o ./libraries/zf_driver/zf_driver_spi.src ./libraries/zf_driver/zf_driver_timer.d ./libraries/zf_driver/zf_driver_timer.o ./libraries/zf_driver/zf_driver_timer.src ./libraries/zf_driver/zf_driver_uart.d ./libraries/zf_driver/zf_driver_uart.o ./libraries/zf_driver/zf_driver_uart.src

.PHONY: clean-libraries-2f-zf_driver

