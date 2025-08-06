RAYLIB  := raylib-5.5_linux_amd64
LUA     := lua-5.4.8

CC      ?= tcc
CFLAGS  := -std=c99 -v -g -I./vendor/$(RAYLIB)/include -I./vendor/$(LUA)/src
LDFLAGS := -L./vendor/$(RAYLIB)/lib -lraylib -L./vendor/$(LUA)/src -llua -lm
PROG    := bidi

$(PROG): lua *.c
	$(CC) $(CFLAGS) *.c -o $(PROG) $(LDFLAGS)

lua:
	cd vendor/$(LUA) && make
