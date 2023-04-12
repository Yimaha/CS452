XDIR:=/u/cs452/public/xdev
ARCH=cortex-a72
TRIPLE=aarch64-none-elf
XBINDIR:=$(XDIR)/bin
CC:=$(XBINDIR)/$(TRIPLE)-g++
OBJCOPY:=$(XBINDIR)/$(TRIPLE)-objcopy
OBJDUMP:=$(XBINDIR)/$(TRIPLE)-objdump

# COMPILE OPTIONS
WARNINGS=-Wall -Wextra -Wpedantic -Wno-unused-const-variable
CFLAGS:=-g -pipe -static $(WARNINGS) -ffreestanding -nostartfiles\
	-mcpu=$(ARCH) -static-pie -mstrict-align -fno-builtin -mgeneral-regs-only \
	-nostdlib -fno-rtti -fno-exceptions -DPRINTF_DISABLE_SUPPORT_FLOAT

# -Wl,option tells g++ to pass 'option' to the linker with commas replaced by spaces
# doing this rather than calling the linker ourselves simplifies the compilation procedure
LDFLAGS:=-Wl,-nmagic -Wl,-Tlinker.ld

# Source files and include dirs
SOURCES :=  $(wildcard *.cc)	$(wildcard *.S)	$(wildcard	*/*.cc) 
# Create .o and .d files for every .cc and .S (hand-written assembly) file
OBJECTS := $(patsubst %.cc, %.o, $(patsubst %.S, %.o, $(SOURCES)))
DEPENDS := $(patsubst %.cc, %.d, $(patsubst %.S, %.d, $(SOURCES)))

# The first rule is the default, ie. "make", "make all" and "make kernel8.img" mean the same
all: kernel8.img

debug: CFLAGS += -DOUR_DEBUG
debug: kernel8.img

opt: CFLAGS += -O3
opt: kernel8.img

localize:
	$(eval XDIR:=~/cs452/arm-gnu-toolchain-12.2.rel1-x86_64-aarch64-none-elf)
	$(eval XBINDIR:=$(XDIR)/bin)
	$(eval CC:=$(XBINDIR)/$(TRIPLE)-g++)
	$(eval OBJCOPY:=$(XBINDIR)/$(TRIPLE)-objcopy)
	$(eval OBJDUMP:=$(XBINDIR)/$(TRIPLE)-objdump)

# Local: localizes the toolchain and then compiles
local: localize kernel8.img

lopt: CFLAGS += -O3
lopt: localize kernel8.img

simu: CFLAGS += -DSIMULATED_UART_1
simu: kernel8.img

timeout: CFLAGS += -DTIME_OUT
timeout: kernel8.img

clean:
	rm -f $(OBJECTS) $(DEPENDS) kernel8.elf kernel8.img buffer.o buffer.d

kernel8.img: kernel8.elf
	$(OBJCOPY) $< -O binary $@

kernel8.elf: $(OBJECTS) linker.ld
	$(CC) $(CFLAGS) $(filter-out %.ld, $^) -o $@ $(LDFLAGS)
	@$(OBJDUMP) -d kernel8.elf | fgrep -q q0 && printf "\n***** WARNING: SIMD INSTRUCTIONS DETECTED! *****\n\n" || true

%.o: %.cc Makefile
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

%.o: %.S Makefile
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

-include $(DEPENDS)
