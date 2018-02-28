.SUFFIXES : 

MBED ?= mbed-os

CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

CFLAGS += -mcpu=cortex-m4 -mlittle-endian -mthumb -g3 -Os

CFLAGS += -I $(MBED)/targets/TARGET_STM/TARGET_STM32F4/device
CFLAGS += -I $(MBED)/targets/TARGET_STM/TARGET_STM32F4/TARGET_STM32F407xG/device
CFLAGS += -I $(MBED)/cmsis
CFLAGS += -I $(MBED)/cmsis/TARGET_CORTEX_M
CFLAGS += -I $(MBED)/cmsis/TARGET_CORTEX_M/TOOLCHAIN_GCC

CFLAGS += -DTARGET_STM32F407VG

LDFLAGS += -T STM32F407XG.ld -Wl,--gc-sections -mcpu=cortex-m4 -mlittle-endian -mthumb -g3

all : example.bin

%.o : %.S
	$(CC) $(CFLAGS) -c -o $@ $^

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $^

example.elf : main.o startup_stm32f407xx.o system_light.o
	$(LD) $(LDFLAGS) $^ -o $@

%.bin : %.elf
	$(OBJCOPY) -O binary $^ $@

clean :
	rm -fv *.o example.elf example.bin
