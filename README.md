# Getting Started using the STM32 platform

I have an [Arch Max][archmax] STM32F4 development board. Although it [supports
mbed][am-mbed] I like to understand how things work...

The [datasheet][stm-ds] and [reference manual][stm-ref] have all the required
low-level hardware info. This guide will get you started compiling C programs to
directly twiddle with the stuff from the reference manual.

I experimented with two ways to get started on the board: CMSIS and libopencm3.
I preferred libopencm3 and that's what I'm using now, but have included examples
using both. The toolchain and debugging stuff is the same between the two.

[archmax]: http://wiki.seeedstudio.com/wiki/Arch_Max
[am-mbed]: https://developer.mbed.org/platforms/Seeed-Arch-Max/
[stm-ds]: http://www.st.com/resource/en/datasheet/dm00037051.pdf
[stm-ref]: http://www.st.com/resource/en/reference_manual/dm00031020.pdf

## Toolchain

On Arch Linux, just install the `arm-none-eabi-gcc` and `arm-none-eabi-newlib`
packages. Most other distros probably offer similarly named packages.

# OpenOCD

An `openocd` config file is provided in the [Arch Max git repo][am-cfg-git].
Then running `openocd -f Arch_Max/arch_max.cfg` with the board connected to a
USB port should pick it up and provide a GDB server on port `3333`. I have to
run OpenOCD as root as it needs access to the `/dev/hidraw0` device node to
communicate with the board.

[am-cfg-git]: https://github.com/Seeed-Studio/Arch_Max.git

# GDB

Once OpenOCD is running and connected to the board via USB `gdb` can be
connected and used to run and debug binaries. 

```
arm-none-eabi-gdb -x load.gdb binary.elf
```

The gdb script `load.gdb` will connect to the OpenOCD gdb server, reset the
board and load the elf file into memory. You can continue (`c`) at the GDB
prompt to just run the program. For debugging you can set breakpoints etc before
running too.

TOOD:
 - how to program an image into flash

# Examples

Both the libopencm3 and the CMSIS examples will produce a binary which when run
on the Arch Max board will blink the LED next to the ethernet port (GPIO B3).

The Makefiles in both examples will link an elf executable file, which is
suitable for debugging and loading via GDB (see above). A further `objcopy -O
binary` step (also shown in the Makefiles) provides a file suitable for flashing
onto the chip's flash memory (I think - I've not got around to actually trying
this yet!).

## Using libopencm3

The [libopencm3][libopencm3] project provide an free software library which is
in many ways similar to CMSIS, but I found it easier to get started with and it
is what I use for projects now. Note that despite the name it also supports a
large range of Cortex-M4 devices!

There is an awk script (`libopencm3/scripts/gnlink.awk`) which can generate the
required C flags and linker script for a device, see
`example-libopencm3/Makefile` for usage.

The generated linker script sets the executable entry point to `reset_handler`
which can be found in `libopencm3/lib/cm3/vector.c`. This is pretty minimal:

 * clears the BSS
 * makes sure the stack is aligned correctly
 * calls pre_main, which may do chip-specific init. For the STM32F4 boards it
   just enabled access to the floating point unit
 * calls global constructors
 * calls main() - provided by you!
 * calls global destructors

Note that the clocks are left in their reset state. In the example I have
set them up (using the `rcc_clock_setup_hse_3v3` function) to run at 168MHz. The
settings there are correct for the Arch Max (I hope!) as it has a high-speed
external (hse) 8MHz oscillator.

Using the functions in libopencm3 to do simple things like toggling the GPIO is
probably rather wasteful as they each incur a function call overhead. If you are
worried about this then you can build the libopencm3 library and your binary
with link-time optimisation (LTO), which will inline any small functions the
compiler thinks is wise.

[libopencm3]: http://libopencm3.org/

## Using CMSIS

I found it very difficult to find the CMSIS code. It turns out the best way to
get hold of it for a platform with mbed support (which is quite a lot, including
the Arch Max) is to just grab the mbed source code from [mbed-os GitHub
repo][mbed-git]. The repository contains lots of stuff for mbed and mbed-os, but the
interesting CMSIS code is in the `targets` folder.

Each supported chip has a folder containing a chip-specific startup assembly
file. For the Arch Max this is in `mbed-os/targets/TARGET_STM/TARGET_STM32F4/TARGET_STM32F407xG/device/TOOLCHAIN_GCC_ARM/startup_stm32f407xx.S`
This assembly file contains the entry point (`Reset_Handler`) and quite well
documented with comments - it:

 * Sets up the stack
 * Copies data segment from flash to RAM
 * Calls `SystemInit` to init clock and other basic hardware init
 * Calls `_start` (which is the libc entry point that will call main)
 * Provides the interrupt vector table
   * Default handler just enters an infinite loop
   * Set up to have a weak symbol for each interrupt, aliased to the default one
   * Can easily provide an interrupt handler by just creating a function of the
	 correct name (e.g. `EXTI0_IRQHandler`)

In the same folder as the startup `.S` file there should be a linker script
(`.ld`) which should be passed to the linker with the `-T` option. This is also
quite well commented.

An implementation of `SystemInit` for the Arch Max is provided
(`mbed-os/targets/TARGET_STM/TARGET_STM32F4/TARGET_STM32F407xG/TARGET_ARCH_MAX/system_stm32f4xx.c`)
which sets up the system clock and various other bits and bobs. This
implementation calls into the HAL library (sources for which can be found at
`mbed-os/targets/TARGET_STM/TARGET_STM32F4/device/`). I have provided a chopped
down version `system_light.c` which does not depend on the HAL library.

The mbed files for your board should have some clues as to what pins are
connected to the odd LEDs etc. which are probably on your board. For the Arch
Max that was in
`mbed-os/targets/TARGET_STM/TARGET_STM32F4/TARGET_STM32F407xG/TARGET_ARCH_MAX/PinNames.h`
- at the bottom `LED1` is set to `PB_3`.

The provided `Makefile` shows the compiler/linker options and include paths required.

See example in the `example-cmsis` folder (note you'll need that checkout of
`mbed-os` - see the readme in the folder).

[Good overview of CMSIS on the LPC devices](https://sergeev.io/notes/cortex_cmsis/)

[Very useful tutorial](http://regalis.com.pl/en/arm-cortex-stm32-gnulinux/)

[Useful tutorial on making a blinky](http://jeremyherbert.net/get/stm32f4_getting_started)

[Tutorial not really checked out yet](http://theanine.io/notes/cortex_cmsis/)

[mbed-git]: https://github.com/ARMmbed/mbed-os.git

# License

mbed-os is licensed under the Apache 2.0 (I think). See their git repo for full
info. Any code which I've derived from this (I think that's just
`system_light.c`) also falls under this license.

libopencm3 is licensed under the LGPL3

The code I have written in the Makefiles and the two `main.c` files I release
under the MIT license.

```
Copyright 2018 Thomas Spurden <thomas@spurden.name>

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```
