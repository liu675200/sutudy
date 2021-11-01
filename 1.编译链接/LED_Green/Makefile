cc = arm-none-eabi-gcc
ld = arm-none-eabi-ld
objcopy = arm-none-eabi-objcopy


object = main.o start.o

main.hex: main
#$(objcopy) -O ihex main main.hex
#$(objcopy) -O binary main main.bin
	$(objcopy)  --output-target=ihex $< $@


main: $(object)
#$(ld) -T link.ld $(object) -L/usr/lib/gcc/x86_64-linux-gnu/7/../../../x86_64-linux-gnu -lc -o main -Map main.map 
#$(ld) -Tlink.ld $(object) -o main
#$(cc) $(object) -o main -Wl,-T,link.ld,-Map,main.map
	arm-none-eabi-ld -nostdlib --gc-section -T link.ld -Map main.map $^ -o main


$(object): %.o: %.c
#$(cc) -c $<
	$(cc) -mthumb -mcpu=cortex-m3 -fdata-sections -ffunction-sections -O1 -mabi=aapcs -c $< -o $@



.PHONY : clean
clean:
	-rm *.o main *.bin *.map *.hex






#elf ---->> bin
#bin: main
#	$(objcopy) -O ihex main main.hex
#	$(objcopy) -O binary main main.bin

#		参数传递（子文件夹）与指令传递
#sub:	
#	cd /c/Users/Administrator/Desktop/make/abc ; make
#sub1:
#	cd /c/Users/Administrator/Desktop/make/abc ; make; pwd
#sub2:
#	cd /c/Users/Administrator/Desktop/make/abc && pwd && make



#		命令包
#main : $(object)
#	$(run-main)
#
#define run-main
#	gcc -c *.c
#	gcc -o main *.o
#endef









