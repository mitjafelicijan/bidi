OS := $(shell uname)
ifeq ($(OS), Linux)
SYSTEM = linux_amd64
else ifeq ($(OS), Darwin)
SYSTEM = macos
else ifeq ($(OS), WindowsNT)
SYSTEM = windows
else
SYSTEM = unknown
endif

CC           ?= tcc
# RAYLIB       := raylib-5.5_linux_amd64
RAYLIB_VER   := raylib-5.5_$(SYSTEM)
LUA          := lua-5.4.8
CFLAGS       := -std=c99 -v -g -I./vendor/$(RAYLIB_VER)/include -I./vendor/$(LUA)/src
LDFLAGS      := -L./vendor/$(RAYLIB_VER)/lib -lraylib -L./vendor/$(LUA)/src -llua -lm
STDLIB_FILES := $(wildcard stdlib/*.lua)
FONT_FILES   := $(wildcard fonts/*.ttf)
PROG         := bidi
PROG_C       := main.c

# Check if macOS and then append proper CFLAGS.
ifeq ($(SYSTEM), macos)
CFLAGS += -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
endif

all: info lua hexdump $(FONT_FILES:.ttf=.h) $(STDLIB_FILES:.lua=.h) $(PROG)

info: # Print out information about the build
	$(info SYSTEM     : $(SYSTEM))
	$(info RAYLIB_VER : $(RAYLIB_VER))
	$(info CFLAGS     : $(CFLAGS))
	$(info LDFLAGS    : $(LDFLAGS))

%.h: %.lua
	./hexdump $< $(@:stdlib/%.h=%) > $@

%.h: %.ttf
	./hexdump $< $(@:fonts/%.h=%) > $@

$(PROG): $(PROG_C)
	$(CC) $(CFLAGS) $(PROG_C) -o $(PROG) $(LDFLAGS)

hexdump: hexdump.c
	$(CC) -std=c99 -o hexdump hexdump.c

lua:
	cd vendor/$(LUA) && make

clean:
	-rm $(PROG) hexdump fonts/*.h stdlib/.*h
	cd vendor/$(LUA) && make clean
