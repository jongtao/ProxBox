################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Compiler'
	"c:/ti/ccsv6/tools/compiler/gcc_msp430_4.9.14r1_98/bin/msp430-elf-gcc.exe" -c -mmcu=msp430f5529 -I"c:/ti/ccsv6/tools/compiler/gcc_msp430_4.9.14r1_98/msp430-elf/include" -I"c:/ti/ccsv6/ccs_base/msp430/include_gcc" -O3 -g -gstrict-dwarf -Wall -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


