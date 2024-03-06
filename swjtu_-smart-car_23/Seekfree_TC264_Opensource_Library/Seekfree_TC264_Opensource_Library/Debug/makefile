################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include user/subdir.mk
-include libraries/zf_driver/subdir.mk
-include libraries/zf_device/subdir.mk
-include libraries/zf_common/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/_PinMap/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/_Lib/DataHandling/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/_Impl/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Vadc/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Vadc/Adc/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Stm/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Src/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Scu/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Qspi/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Qspi/SpiMaster/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Port/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Mtu/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Gtm/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Gtm/Atom/Pwm/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Gpt12/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Gpt12/IncrEnc/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Flash/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Dma/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Dma/Dma/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Cpu/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Cpu/Irq/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Cpu/CStart/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Ccu6/Timer/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Ccu6/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Asclin/Std/subdir.mk
-include libraries/infineon_libraries/iLLD/TC26B/Tricore/Asclin/Asc/subdir.mk
-include libraries/infineon_libraries/Service/CpuGeneric/SysSe/Math/subdir.mk
-include libraries/infineon_libraries/Service/CpuGeneric/SysSe/Bsp/subdir.mk
-include libraries/infineon_libraries/Service/CpuGeneric/StdIf/subdir.mk
-include libraries/infineon_libraries/Service/CpuGeneric/If/subdir.mk
-include libraries/infineon_libraries/Infra/Platform/Tricore/Compilers/subdir.mk
-include code/subdir.mk
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := Seekfree_TC264_Opensource_Library
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_SIZE += \
Seekfree_TC264_Opensource_Library.siz \


# All Target
all: main-build

# Main-build Target
main-build: Seekfree_TC264_Opensource_Library.elf secondary-outputs

# Tool invocations
Seekfree_TC264_Opensource_Library.elf: $(OBJS) makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: TASKING Linker'
	cctc --lsl-file="../Lcf_Tasking_Tricore_Tc.lsl" -Wl-Oc -Wl-OL -Wl-Ot -Wl-Ox -Wl-Oy "C:\Users\admin\Desktop\smartcar\smart_car\swjtu_-smart-car_23\Seekfree_TC264_Opensource_Library\Seekfree_TC264_Opensource_Library\libraries\zf_device\zf_device_config.a" -Wl--map-file="Seekfree_TC264_Opensource_Library.map" -Wl-mc -Wl-mf -Wl-mi -Wl-mk -Wl-ml -Wl-mm -Wl-md -Wl-mr -Wl-mu --no-warnings= -Wl--error-limit=42 --fp-model=3 -lcs_fpu -lfp_fpu -lrt --lsl-core=vtc --exceptions --strict --anachronisms --force-c++ -Ctc26xb -o"Seekfree_TC264_Opensource_Library.elf" -Wl-o"Seekfree_TC264_Opensource_Library.hex:IHEX" $(OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

Seekfree_TC264_Opensource_Library.siz: Seekfree_TC264_Opensource_Library.elf makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Invoking: Print Size'
	elfsize  "Seekfree_TC264_Opensource_Library.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	find . ! -name "*.mk" ! -name "makefile" ! -name "." -type f -delete 
	-@echo ' '

secondary-outputs: $(SECONDARY_SIZE)

.PHONY: all clean dependents main-build

-include ../makefile.targets
