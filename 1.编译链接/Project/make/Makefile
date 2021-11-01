object = main.o first.o second.o
export MAKE = gcc -c

main: $(object)
	gcc -o main $(object)
$(object): %.o: %.c
	gcc -c $<

bin: $(object)
	objcopy -O binary main.out main.bin

map: $(object)
	ld -Map main.map $(object) -o main


.PHONY : clean sub

sub:
	cd /c/Users/Administrator/Desktop/make/abc ; make

clean:
	-rm *.o main
	cd /c/Users/Administrator/Desktop/make/abc && make clean

cleanbin:
	-rm *.o main *.bin
	cd /c/Users/Administrator/Desktop/make/abc && make clean


cleanmap:
	-rm *.o main *.map
	cd /c/Users/Administrator/Desktop/make/abc && make clean




	


#		参数传递（子文件夹）与指令传递
#sub:	
#	cd /c/Users/Administrator/Desktop/make/abc ; make
#sub1:
#	#cd /c/Users/Administrator/Desktop/make/abc ; make; pwd
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









