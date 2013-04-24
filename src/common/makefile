C=gcc
CXX=g++
RM=rm -f
DEBUG=-g
CFLAGS=-Wall -c $(DEBUG)
LDFLAGS=$(DEBUG)
LDLIBS=
INCLUDES=

SRCS=file_utils.c
		
OBJS=$(subst .c,.o,$(SRCS))
#TAR=emu

all: dbg source

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
	$(C) $(DEBUG) -Wall -c $(SRCS) $(INCLUDES) $(LDLIBS)

project: $(OBJS)
	@echo ""
	@echo "Binding $(TAR)"
	$(C) $(LDFLAGS) -o $(TAR) $(OBJS) $(LDLIBS)
	@echo ""
	@echo "Done."


clean:
	$(RM) $(OBJS)
