CROSS=arm-linux-gnueabi-

all:
	$(CROSS)as -mcpu=cortex-m3  -o wtmi.o wtmi.S
	$(CROSS)gcc -nostdlib -T wtmi.ld -Xlinker "--build-id=none" wtmi.o -o wtmi.elf
	$(CROSS)objcopy -S -O binary wtmi.elf wtmi.bin
	$(CROSS)objdump -D -S wtmi.elf > wtmi.dis

clean:
	@rm *.elf *.bin *dis 2>/dev/null
