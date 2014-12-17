DEPENDENCY_TARGETS=libusb inih sqlite


CC = arm-linux-gcc
FINAL_CFLAGS = -std=c99 -D_GNU_SOURCE -I../deps/libusb/libusb -I../deps/inih/
FINAL_LDFLAGS=
FINAL_LDLIBS = -lpthread -lrt

OBJS = printer.o musb.o log.o config.o
APP = printer

CCCOLOR="\033[34m"
LINKCOLOR="\033[34;1m"
SRCCOLOR="\033[33m"
BINCOLOR="\033[1;32m"
MAKECOLOR="\033[32;1m"
ENDCOLOR="\033[0m"

#if we do not want quit making, then run make V=1
ifndef V
QUIET_CC = @printf '    %b %b\n' $(CCCOLOR)CC$(ENDCOLOR) $(SRCCOLOR)$@$(ENDCOLOR) 1>&2;
QUIET_LINK = @printf '    %b %b\n' $(LINKCOLOR)LINK$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR) 1>&2;
QUIET_INSTALL = @printf '    %b %b\n' $(LINKCOLOR)INSTALL$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR) 1>&2;
endif

$(APP): $(OBJS)
	$(QUIET_LINK) $(FINAL_LDFLAGS) $(CC) -o $@ $^ ../deps/libusb/libusb.a ../deps/inih/libinih.a $(FINAL_LDLIBS)

.make_libs:
	@touch $@
	cd ../deps && $(MAKE) $(DEPENDENCY_TARGETS)

%.o:%.c .make_libs
	$(QUIET_CC) $(CC) -c $< $(FINAL_CFLAGS)

# use make dep to generate this
-include makefile.dep

dep:
	$(QUIET_CC) $(CC) -MM *.c > makefile.dep
.PHONY: dep

clean:
	rm -f *.o $(APP) makefile.dep

.PHONY: clean

