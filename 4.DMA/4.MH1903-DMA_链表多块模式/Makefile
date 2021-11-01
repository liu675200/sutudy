cc = arm-none-eabi-gcc -ffunction-sections -fdata-sections -mthumb -mcpu=cortex-m4 -mfloat-abi=softfp -Wall
lds = my.ld
ld = arm-none-eabi-ld -nostdlib --gc-sections
objcopy = arm-none-eabi-objcopy

target = $(patsubst %.c,%.o,$(wildcard *.c))

main.hex : main.exe
	$(objcopy)  --output-target=ihex $< $@
	$(objcopy)  --output-target=binary $< $(patsubst %.hex,%.bin,$@)

main.exe : $(target)
#	echo $(target)
	$(ld) -T $(lds)  -Map main.map $^ -o $@
#	$(cc) -Wl,-T,$(lds),-Map,main.map,--gc-sections,-nostdlib $^ -o $@

$(target): %.o : %.c
	$(cc) -c $< -o $@


.pHONY : clean
clean :
	rm *.o *.exe *.map *.bin *.hex