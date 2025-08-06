#include <stdio.h>

#include "raylib.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#define IN_FILE "test/main.lua"
#define DEBUG_LEVEL LOG_DEBUG

static int l_open_window(lua_State *L) {
	int width = luaL_checknumber(L, 1);
	int height = luaL_checknumber(L, 2);
	const char *title = luaL_checkstring(L, 3);
	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	InitWindow(width, height, title);
	TraceLog(LOG_DEBUG, "l_open_window");
	return 0;
}

static int l_window_should_close(lua_State *L) {
	lua_pushboolean(L, WindowShouldClose());
	return 1;
}

static int l_set_fps(lua_State *L) {
	int fps = luaL_checknumber(L, 1);
	SetTargetFPS(fps);
	TraceLog(LOG_DEBUG, "l_set_fps");
	return 0;
}

static int l_close_window(lua_State *L) {
	CloseWindow();
	TraceLog(LOG_DEBUG, "l_close_window");
	return 0;
}

static int l_begin_drawing(lua_State *L) {
	BeginDrawing();
	return 0;
}

static int l_end_drawing(lua_State *L) {
	EndDrawing();
	return 0;
}

static int l_clear_window(lua_State *L) {
	ClearBackground(BLACK);
	return 0;
}

static int l_draw_fps(lua_State *L) {
	DrawFPS(GetScreenWidth() - 100, 20); 
	return 0;
}

int main(void) {
	SetTraceLogLevel(DEBUG_LEVEL);

	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	lua_register(L, "open_window", l_open_window);
	lua_register(L, "close_window", l_close_window);
	lua_register(L, "window_should_close", l_window_should_close);
	lua_register(L, "begin_drawing", l_begin_drawing);
	lua_register(L, "end_drawing", l_end_drawing);
	lua_register(L, "set_fps", l_set_fps);
	lua_register(L, "draw_fps", l_draw_fps);
	lua_register(L, "clear_window", l_clear_window);

	// TODO: This should probably use loadbuffer instead.
	// https://www.lua.org/manual/5.4/manual.html#luaL_loadbuffer
	if (luaL_loadfile(L, IN_FILE) || lua_pcall(L, 0, 0, 0)) {
		fprintf(stderr, "Error: %s\n", lua_tostring(L, -1));
		return 1;
	}

	lua_close(L);
	return 0;
}
