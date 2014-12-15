SRC = ini.c
OBJ = $(SRC:.c=.o)
OUT = libinih.a
CFLAGS = -O2
CC = arm-linux-gcc
AR = arm-linux-ar

default: $(OUT)

.c.o:
	$(CC) $(INCLUDES) $(CCFLAGS) $(EXTRACCFLAGS) -c $< -o $@

$(OUT): $(OBJ)
	$(AR) rcs $(OUT) $(OBJ) $(EXTRAARFLAGS)

clean:
	rm -f $(OBJ) $(OUT)