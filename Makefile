CC      ?= tcc
RAYLIB  := raylib-5.5_linux_amd64
LUA     := lua-5.4.8
CFLAGS  := -std=c99 -v -g -I./vendor/$(RAYLIB)/include -I./vendor/$(LUA)/src
LDFLAGS := -L./vendor/$(RAYLIB)/lib -lraylib -L./vendor/$(LUA)/src -llua -lm
PROG    := bidi
PROG_C  := main.c

$(PROG): lua hexdump $(PROG_C)
	$(CC) $(CFLAGS) $(PROG_C) -o $(PROG) $(LDFLAGS)

hexdump: hexdump.c
	$(CC) -std=c99 -o hexdump hexdump.c

lua:
	cd vendor/$(LUA) && make
