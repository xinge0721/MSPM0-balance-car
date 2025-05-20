################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
MPU6050/%.o: ../MPU6050/%.cpp $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"D:/ti/ccs/tools/compiler/ti-cgt-armllvm_4.0.2.LTS/bin/tiarmclang.exe" -c @"device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O0 -I"C:/Users/xinge/workspace_ccstheia/MSPM0 balance car" -I"C:/Users/xinge/workspace_ccstheia/MSPM0 balance car/Debug" -I"C:/TI/mspm0_sdk_2_04_00_06/source/third_party/CMSIS/Core/Include" -I"C:/TI/mspm0_sdk_2_04_00_06/source" -I"C:/Users/xinge/workspace_ccstheia/MSPM0 balance car/Hardware/PID" -I"C:/Users/xinge/workspace_ccstheia/MSPM0 balance car/OLED" -I"C:/Users/xinge/workspace_ccstheia/MSPM0 balance car/MPU6050" -I"C:/Users/xinge/workspace_ccstheia/MSPM0 balance car/Hardware/HTS221" -DMOTION_DRIVER_TARGET_MSPM0 -DMPU6050 -gdwarf-3 -MMD -MP -MF"MPU6050/$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


