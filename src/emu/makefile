C=gcc
CXX=g++
RM=rm -f
DEBUG=-g
CFLAGS=-Wall -c $(DEBUG)
LDFLAGS=$(DEBUG)
LDLIBS=-lSDL -lSDL_ttf -lSDL_image
COMMON=../common
INCLUDES=-I. -I$(COMMON)

SRCS=main.c \
        emu.c \
		emu_cpu.c \
		emu_ram.c \
		emu_utils.c \
		emu_disp.c \
		emu_disk.c \
		emu_sdl.c \
		emu_clk.c

COMMON_SRCS=$(COMMON)/file_utils.c
COMMON_OBJS=$(subst .c,.o,$(COMMON_SRCS))

		
OBJS=$(subst .c,.o,$(SRCS))
TAR=emu

all: dbg project

dbg:
	@echo "============================================="
	@echo "Flags"
	@echo "---------------------------------------------"
	@echo "$(LDFLAGS) $(CFLAGS)"
	@echo "============================================="
	@echo "Libs | Includes"
	@echo "---------------------------------------------"
	@echo "$(LDLIBS) | $(INCLUDES)"
	@echo "============================================="
	@echo ""
	@echo "Compiling"

%.o: %.c
	$(C) -c $(LDFLAGS) $(INCLUDES) $(LDLIBS) $< -o $@

project: $(OBJS)
	@echo ""
	@echo "Binding $(TAR)"
	$(C) $(LDFLAGS) -o $(TAR) $(OBJS) $(COMMON_OBJS) $(LDLIBS)
	@echo ""
	@echo "Done."


clean:
	$(RM) $(OBJS)
