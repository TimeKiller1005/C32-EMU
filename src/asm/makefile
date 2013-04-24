C=gcc
CXX=g++
RM=rm -f
DEBUG=-g
CFLAGS=-Wall -c $(DEBUG)
LDFLAGS=$(DEBUG)
LDLIBS=
COMMON=../common
INCLUDES=-I$(COMMON)

SRCS=asm.C

COMMON_SRCS=$(COMMON)/emu_defs.h \
			$(COMMON)/file_utils.c
COMMON_OBJS=$(subst .c,.o,$(COMMON_SRCS))

		
OBJS=$(subst .C,.o,$(SRCS))
TAR=asm

all: dbg source project

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

source: $(SRCS)
	@echo "Compiling"
	$(CXX) $(DEBUG) -Wall -c $(SRCS) $(INCLUDES) $(LDLIBS)

project: $(OBJS)
	@echo ""
	@echo "Binding $(TAR)"
	$(CXX) $(LDFLAGS) -o $(TAR) $(OBJS) $(COMMON_OBJS) $(LDLIBS)
	@echo ""
	@echo "Done."

clean:
	$(RM) $(OBJS)
