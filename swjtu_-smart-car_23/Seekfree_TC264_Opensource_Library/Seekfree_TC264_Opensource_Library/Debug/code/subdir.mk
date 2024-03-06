################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../code/Adc_Em.c \
../code/In_out_bound.c \
../code/In_perspective.c \
../code/balance.c \
../code/current.c \
../code/debug.c \
../code/element.c \
../code/encoder.c \
../code/error.c \
../code/filtering.c \
../code/image.c \
../code/imu.c \
../code/init.c \
../code/key.c \
../code/motor.c \
../code/pico_GY.c \
../code/pico_link_ii.c \
../code/pid.c \
../code/protocol.c \
../code/ring.c \
../code/time.c \
../code/tracing.c 

COMPILED_SRCS += \
./code/Adc_Em.src \
./code/In_out_bound.src \
./code/In_perspective.src \
./code/balance.src \
./code/current.src \
./code/debug.src \
./code/element.src \
./code/encoder.src \
./code/error.src \
./code/filtering.src \
./code/image.src \
./code/imu.src \
./code/init.src \
./code/key.src \
./code/motor.src \
./code/pico_GY.src \
./code/pico_link_ii.src \
./code/pid.src \
./code/protocol.src \
./code/ring.src \
./code/time.src \
./code/tracing.src 

C_DEPS += \
./code/Adc_Em.d \
./code/In_out_bound.d \
./code/In_perspective.d \
./code/balance.d \
./code/current.d \
./code/debug.d \
./code/element.d \
./code/encoder.d \
./code/error.d \
./code/filtering.d \
./code/image.d \
./code/imu.d \
./code/init.d \
./code/key.d \
./code/motor.d \
./code/pico_GY.d \
./code/pico_link_ii.d \
./code/pid.d \
./code/protocol.d \
./code/ring.d \
./code/time.d \
./code/tracing.d 

OBJS += \
./code/Adc_Em.o \
./code/In_out_bound.o \
./code/In_perspective.o \
./code/balance.o \
./code/current.o \
./code/debug.o \
./code/element.o \
./code/encoder.o \
./code/error.o \
./code/filtering.o \
./code/image.o \
./code/imu.o \
./code/init.o \
./code/key.o \
./code/motor.o \
./code/pico_GY.o \
./code/pico_link_ii.o \
./code/pid.o \
./code/protocol.o \
./code/ring.o \
./code/time.o \
./code/tracing.o 


# Each subdirectory must supply rules for building sources it contributes
code/%.src: ../code/%.c code/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING C/C++ Compiler'
	cctc -cs --dep-file="$(basename $@).d" --misrac-version=2004 -D__CPU__=tc26xb "-fC:/Users/admin/Desktop/smartcar/smart_car/swjtu_-smart-car_23/Seekfree_TC264_Opensource_Library/Seekfree_TC264_Opensource_Library/Debug/TASKING_C_C___Compiler-Include_paths.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<" && \
	if [ -f "$(basename $@).d" ]; then sed.exe -r  -e 's/\b(.+\.o)\b/code\/\1/g' -e 's/\\/\//g' -e 's/\/\//\//g' -e 's/"//g' -e 's/([a-zA-Z]:\/)/\L\1/g' -e 's/\d32:/@TARGET_DELIMITER@/g; s/\\\d32/@ESCAPED_SPACE@/g; s/\d32/\\\d32/g; s/@ESCAPED_SPACE@/\\\d32/g; s/@TARGET_DELIMITER@/\d32:/g' "$(basename $@).d" > "$(basename $@).d_sed" && cp "$(basename $@).d_sed" "$(basename $@).d" && rm -f "$(basename $@).d_sed" 2>/dev/null; else echo 'No dependency file to process';fi
	@echo 'Finished building: $<'
	@echo ' '

code/%.o: ./code/%.src code/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING Assembler'
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-code

clean-code:
	-$(RM) ./code/Adc_Em.d ./code/Adc_Em.o ./code/Adc_Em.src ./code/In_out_bound.d ./code/In_out_bound.o ./code/In_out_bound.src ./code/In_perspective.d ./code/In_perspective.o ./code/In_perspective.src ./code/balance.d ./code/balance.o ./code/balance.src ./code/current.d ./code/current.o ./code/current.src ./code/debug.d ./code/debug.o ./code/debug.src ./code/element.d ./code/element.o ./code/element.src ./code/encoder.d ./code/encoder.o ./code/encoder.src ./code/error.d ./code/error.o ./code/error.src ./code/filtering.d ./code/filtering.o ./code/filtering.src ./code/image.d ./code/image.o ./code/image.src ./code/imu.d ./code/imu.o ./code/imu.src ./code/init.d ./code/init.o ./code/init.src ./code/key.d ./code/key.o ./code/key.src ./code/motor.d ./code/motor.o ./code/motor.src ./code/pico_GY.d ./code/pico_GY.o ./code/pico_GY.src ./code/pico_link_ii.d ./code/pico_link_ii.o ./code/pico_link_ii.src ./code/pid.d ./code/pid.o ./code/pid.src ./code/protocol.d ./code/protocol.o ./code/protocol.src ./code/ring.d ./code/ring.o ./code/ring.src ./code/time.d ./code/time.o ./code/time.src ./code/tracing.d ./code/tracing.o ./code/tracing.src

.PHONY: clean-code

