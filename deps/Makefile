CCCOLOR="\033[34m"
LINKCOLOR="\033[34;1m"
SRCCOLOR="\033[33m"
BINCOLOR="\033[1;32m"
MAKECOLOR="\033[32;1m"
ENDCOLOR="\033[0m"


default:
	@echo "use make in the top dir"

libusb:
	@printf '%b %b\n' $(MAKECOLOR)MAKE$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR)
	cd libusb &&  ./configure  --host=arm-linux --prefix=/opt/libusb --disable-udev --disable-shared --enable-static
	cd libusb && $(MAKE)
	cd libusb && cp libusb/.libs/libusb-1.0.a libusb.a

.PHONY: libusb


inih:
	@printf '%b %b\n' $(MAKECOLOR)MAKE$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR)
	cd inih && $(MAKE)

.PHONY: inih

sqlite:
	@printf '%b %b\n' $(MAKECOLOR)MAKE$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR)
	cd sqlite &&  ./configure  --host=arm-linux --prefix=/opt/sqlite --enable-static
	cd sqlite && $(MAKE)
	cd sqlite && cp .libs/libsqlite3.a libsqlite3.a

.PHONY: sqlite