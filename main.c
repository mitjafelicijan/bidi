#include <stdio.h>

#include "raylib.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "stdlib/color.h"
#include "stdlib/json.h"

#define IN_FILE "test/main.lua"
#define DEBUG_LEVEL LOG_DEBUG

static int lua_getfield_int(lua_State *L, int index, const char *key) {
	lua_getfield(L, index, key);
	int val = (int)luaL_checknumber(L, -1);
	lua_pop(L, 1);
	return val;
}

static int lua_getfield_int_opt(lua_State *L, int index, const char *key, int def) {
	lua_getfield(L, index, key);
	int val = lua_isnil(L, -1) ? def : (int)luaL_checknumber(L, -1);
	lua_pop(L, 1);
	return val;
}

static int l_open_window(lua_State *L) {
	int width = luaL_checknumber(L, 1);
	int height = luaL_checknumber(L, 2);
	const char *title = luaL_checkstring(L, 3);
	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	InitWindow(width, height, title);
	TraceLog(LOG_DEBUG, "l_open_window");
	return 0;
}

// TODO: This function name is still a bit sus. Revisit the name later.
static int l_window_running(lua_State *L) {
	lua_pushboolean(L, !WindowShouldClose());
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
	luaL_checktype(L, 1, LUA_TTABLE);
	Color color = {
		.r = (unsigned char)lua_getfield_int(L, 1, "r"),
		.g = (unsigned char)lua_getfield_int(L, 1, "g"),
		.b = (unsigned char)lua_getfield_int(L, 1, "b"),
		.a = (unsigned char)lua_getfield_int_opt(L, 1, "a", 255)
	};
	ClearBackground(color);
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

	// Registring Raylib mappings.
	lua_register(L, "open_window", l_open_window);
	lua_register(L, "close_window", l_close_window);
	lua_register(L, "window_running", l_window_running);
	lua_register(L, "begin_drawing", l_begin_drawing);
	lua_register(L, "end_drawing", l_end_drawing);
	lua_register(L, "set_fps", l_set_fps);
	lua_register(L, "draw_fps", l_draw_fps);
	lua_register(L, "clear_window", l_clear_window);

	// Embedding color module.
	if (luaL_loadbuffer(L, color, color_len, "color") || lua_pcall(L, 0, 1, 0)) {
		fprintf(stderr, "Error loading color.lua: %s\n", lua_tostring(L, -1));
		lua_close(L);
		return 1;
	}
	lua_setglobal(L, "color");

	// Embedding JSON module.
	if (luaL_loadbuffer(L, json, json_len, "json") || lua_pcall(L, 0, 1, 0)) {
		fprintf(stderr, "Error loading json.lua: %s\n", lua_tostring(L, -1));
		lua_close(L);
		return 1;
	}
	lua_setglobal(L, "json");

	// Interpreting and running input file Lua script.
	if (luaL_loadfile(L, IN_FILE) || lua_pcall(L, 0, 0, 0)) {
		fprintf(stderr, "Error: %s\n", lua_tostring(L, -1));
		return 1;
	}

	lua_close(L);
	return 0;
}
