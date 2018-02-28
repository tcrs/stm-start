.SUFFIXES :

LIBOPENCM3 ?= libopencm3
DEVICE ?= STM32F407VG

CC = arm-none-eabi-gcc
AS = arm-none-eabi-as
LD = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy

CFLAGS += -mcpu=cortex-m4 -mlittle-endian -mthumb
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
CFLAGS += -g3 -Os -ffreestanding
CFLAGS += -I $(LIBOPENCM3)/include
CFLAGS += -fdata-sections -ffunction-sections

LDFLAGS += -g3 -mthumb --static -nostartfiles -Wl,--gc-sections
LDFLAGS += -L$(LIBOPENCM3)/lib

GENLINK = $(LIBOPENCM3)/scripts/genlink.awk

all : example.elf example.bin

%.o : %.c
	$(CC) $(CFLAGS) `awk -v PAT=$(DEVICE) -v MODE=CPPFLAGS -f $(GENLINK) $(LIBOPENCM3)/ld/devices.data` -c -o $@ $<

link.ld : | $(GENLINK)
	$(CC) `awk -v PAT=$(DEVICE) -v MODE=DEFS -f $(GENLINK) $(LIBOPENCM3)/ld/devices.data` -P -E $(LIBOPENCM3)/ld/linker.ld.S > $@

example.elf : main.o | link.ld
	$(LD) $(LDFLAGS) -T link.ld $< -lopencm3_stm32f4 -o $@

%.bin : %.elf
	$(OBJCOPY) -O binary $< $@

clean :
	rm -fv *.o example.elf example.bin link.ld
