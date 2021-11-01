cc = arm-none-eabi-gcc
ld = arm-none-eabi-ld
objcopy = arm-none-eabi-objcopy
lds = link.ld
target = $(patsubst %.c,%.o,$(wildcard *.c))
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






