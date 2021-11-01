cc = arm-none-eabi-gcc
ld = arm-none-eabi-ld
objcopy = arm-none-eabi-objcopy
lds = link.ld
target = main.o start.o usart.o clock.o xmodem.o flash.o interrup.o FIFO.o
output = main.hex


$(output): main
	$(objcopy) -O ihex main $(output)

main: $(target)
	$(ld) -nostdlib --gc-section -T $(lds) -Map main.map $^ -o main

$(target): %.o :%.c
	$(cc) -mthumb -mcpu=cortex-m3 -fdata-sections -ffunction-sections -Wall -O1 -c $< -o $@

.PHONY : clean
clean:
	-rm *.o main *.map *.hex






