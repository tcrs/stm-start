Need to download and build [libopencm3][1]

```
git clone https://github.com/libopencm3/libopencm3.git
make -C libopencm3
```

[1]: http://libopencm3.org/

# Link-time Optimisation

The compiler and linker need to be passed the `-flto` flag

For lobopencm3:

```
make -C libopencm3 clean
CFLAGS=-flto make -C libopencm3
```

For the example:

```
make clean
CFLAGS=-flto LDFLAGS=-flto make
```

If you examine the resulting binary you should see that GCC has inlined most of
the code into `main` (as it's mostly small functions or only ever called once).

# Soft-float

The default build of libopencm3 is hard-float build, which is ABI-incompatible
with soft-float libraries. So if you want to link with any soft-float libraries
for any reason you will need to rebuild libopencm3:

```
make -C libopencm3 clean
make -C libopencm3 FP_FLAGS="-mfloat-abi=soft"
```
