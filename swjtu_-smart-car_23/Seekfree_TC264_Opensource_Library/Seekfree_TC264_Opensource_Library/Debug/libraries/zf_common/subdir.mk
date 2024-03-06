################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libraries/zf_common/zf_common_clock.c \
../libraries/zf_common/zf_common_debug.c \
../libraries/zf_common/zf_common_fifo.c \
../libraries/zf_common/zf_common_font.c \
../libraries/zf_common/zf_common_function.c \
../libraries/zf_common/zf_common_interrupt.c 

COMPILED_SRCS += \
./libraries/zf_common/zf_common_clock.src \
./libraries/zf_common/zf_common_debug.src \
./libraries/zf_common/zf_common_fifo.src \
./libraries/zf_common/zf_common_font.src \
./libraries/zf_common/zf_common_function.src \
./libraries/zf_common/zf_common_interrupt.src 

C_DEPS += \
./libraries/zf_common/zf_common_clock.d \
./libraries/zf_common/zf_common_debug.d \
./libraries/zf_common/zf_common_fifo.d \
./libraries/zf_common/zf_common_font.d \
./libraries/zf_common/zf_common_function.d \
./libraries/zf_common/zf_common_interrupt.d 

OBJS += \
./libraries/zf_common/zf_common_clock.o \
./libraries/zf_common/zf_common_debug.o \
./libraries/zf_common/zf_common_fifo.o \
./libraries/zf_common/zf_common_font.o \
./libraries/zf_common/zf_common_function.o \
./libraries/zf_common/zf_common_interrupt.o 


# Each subdirectory must supply rules for building sources it contributes
libraries/zf_common/%.src: ../libraries/zf_common/%.c libraries/zf_common/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING C/C++ Compiler'
	cctc -cs --dep-file="$(basename $@).d" --misrac-version=2004 -D__CPU__=tc26xb "-fC:/Users/admin/Desktop/smartcar/smart_car/swjtu_-smart-car_23/Seekfree_TC264_Opensource_Library/Seekfree_TC264_Opensource_Library/Debug/TASKING_C_C___Compiler-Include_paths.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Ctc26xb -Y0 -N0 -Z0 -o "$@" "$<" && \
	if [ -f "$(basename $@).d" ]; then sed.exe -r  -e 's/\b(.+\.o)\b/libraries\/zf_common\/\1/g' -e 's/\\/\//g' -e 's/\/\//\//g' -e 's/"//g' -e 's/([a-zA-Z]:\/)/\L\1/g' -e 's/\d32:/@TARGET_DELIMITER@/g; s/\\\d32/@ESCAPED_SPACE@/g; s/\d32/\\\d32/g; s/@ESCAPED_SPACE@/\\\d32/g; s/@TARGET_DELIMITER@/\d32:/g' "$(basename $@).d" > "$(basename $@).d_sed" && cp "$(basename $@).d_sed" "$(basename $@).d" && rm -f "$(basename $@).d_sed" 2>/dev/null; else echo 'No dependency file to process';fi
	@echo 'Finished building: $<'
	@echo ' '

libraries/zf_common/%.o: ./libraries/zf_common/%.src libraries/zf_common/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: TASKING Assembler'
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-libraries-2f-zf_common

clean-libraries-2f-zf_common:
	-$(RM) ./libraries/zf_common/zf_common_clock.d ./libraries/zf_common/zf_common_clock.o ./libraries/zf_common/zf_common_clock.src ./libraries/zf_common/zf_common_debug.d ./libraries/zf_common/zf_common_debug.o ./libraries/zf_common/zf_common_debug.src ./libraries/zf_common/zf_common_fifo.d ./libraries/zf_common/zf_common_fifo.o ./libraries/zf_common/zf_common_fifo.src ./libraries/zf_common/zf_common_font.d ./libraries/zf_common/zf_common_font.o ./libraries/zf_common/zf_common_font.src ./libraries/zf_common/zf_common_function.d ./libraries/zf_common/zf_common_function.o ./libraries/zf_common/zf_common_function.src ./libraries/zf_common/zf_common_interrupt.d ./libraries/zf_common/zf_common_interrupt.o ./libraries/zf_common/zf_common_interrupt.src

.PHONY: clean-libraries-2f-zf_common

