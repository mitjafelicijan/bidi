#include <stdio.h>
#include <stdarg.h>
#include <getopt.h>
#include <stdlib.h>

#include "raylib.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "stdlib/json.h"
#include "stdlib/color.h"
#include "stdlib/tilemap.h"

#include "fonts/dejavusans_mono_bold.h"

#define VERSION "x.x"
#define DEBUG_LEVEL LOG_DEBUG
#define FONT_IMPORT_SIZE 30

typedef struct {
	Font font;
	int font_size;
} Context;

// Setting up global context.
Context ctx = {0};

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

static int load_embedded_module(lua_State *L, const char *module_code, size_t code_len, const char *module_name) {
	if (luaL_loadbuffer(L, module_code, code_len, module_name) || lua_pcall(L, 0, 1, 0)) {
		fprintf(stderr, "Error loading %s: %s\n", module_name, lua_tostring(L, -1));
		return 0;
	}
	lua_setglobal(L, module_name);
	return 1;
}

static int l_open_window(lua_State *L) {
	int width = luaL_checknumber(L, 1);
	int height = luaL_checknumber(L, 2);
	const char *title = luaL_checkstring(L, 3);
	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	InitWindow(width, height, title);
	TraceLog(LOG_DEBUG, "l_open_window");

	ctx.font_size = FONT_IMPORT_SIZE;
	ctx.font = LoadFontFromMemory(".ttf", dejavusans_mono_bold, dejavusans_mono_bold_len, ctx.font_size, NULL, 0);
	SetTextureFilter(ctx.font.texture, TEXTURE_FILTER_TRILINEAR);

	if (!IsFontValid(ctx.font)) {
		printf("font not valid\n");
	}

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

static int l_draw_fps_meter(lua_State *L) {
	DrawFPS(GetScreenWidth() - 100, 20); 
	return 0;
}

static int l_draw_info(lua_State *L) {
	float delta = GetFrameTime();
	int fps = GetFPS();
	double runtime = GetTime();
	int height = GetScreenHeight();

	DrawTextEx(ctx.font, TextFormat("fps: %d", fps), (Vector2){ 20, height - 80 }, 20, 0, RAYWHITE);
	DrawTextEx(ctx.font, TextFormat("run: %f", runtime), (Vector2){ 20, height - 60 }, 20, 0, RAYWHITE);
	DrawTextEx(ctx.font, TextFormat("dt: %f", delta), (Vector2){ 20, height - 40 }, 20, 0, RAYWHITE);

	return 0;
}

static void help(const char *argv0) {
	printf("Usage: %s [options]\n"
			"\nAvailable options:\n"
			"  -r,--run=file.lua            run input file\n"
			"  -b,--bundle                  bundles this folder\n"
			"  -d,--debug                   prints debug information\n"
			"  -h,--help                    this help\n"
			"  -v,--version                 show version\n",
			argv0);
}

static void version(const char *argv0) {
	printf("%s version %s\n", argv0, VERSION);
}

int main(int argc, char *argv[]) {
	TraceLogLevel debug_level = LOG_WARNING;
	const char *run_file = NULL;

	const char short_options[] = "r:dbhv";
	const struct option long_options[] = {
		{ "run", 1, NULL, 'r' },
		{ "debug", 0, NULL, 'd' },
		{ "bundle", 0, NULL, 'b' },
		{ "help", 0, NULL, 'h' },
		{ "version", 0, NULL, 'v' },
		{ 0 },
	};

	int opt;
	while ((opt = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
		switch (opt) {
			case 'r':
				run_file = optarg;
				break;
			case 'b':
				printf("TODO: Bundler\n");
				return 0;
			case 'd':
				debug_level = LOG_DEBUG;
				break;
			case 'h':
				help(argv[0]);
				return 0;
			case 'v':
				version(argv[0]);
				return 0;
			default:
				fprintf(stdout, "Missing options. Check help.\n");
				return 0;
		}
	}

	if (run_file) {
		SetTraceLogLevel(debug_level);

		lua_State *L = luaL_newstate();
		luaL_openlibs(L);

		// Registring Raylib mappings.
		lua_register(L, "open_window", l_open_window);
		lua_register(L, "close_window", l_close_window);
		lua_register(L, "window_running", l_window_running);
		lua_register(L, "begin_drawing", l_begin_drawing);
		lua_register(L, "end_drawing", l_end_drawing);
		lua_register(L, "set_fps", l_set_fps);
		lua_register(L, "draw_fps_meter", l_draw_fps_meter);
		lua_register(L, "draw_info", l_draw_info);
		lua_register(L, "clear_window", l_clear_window);

		// Loading embeded modules into Lua state.
		if (!load_embedded_module(L, json, json_len, "json")) return 1;
		if (!load_embedded_module(L, color, color_len, "color")) return 1;
		if (!load_embedded_module(L, tilemap, tilemap_len, "tilemap")) return 1;

		// Interpreting and running input file Lua script.
		if (luaL_loadfile(L, run_file) || lua_pcall(L, 0, 0, 0)) {
			fprintf(stderr, "Error: %s\n", lua_tostring(L, -1));
			return 1;
		}

		UnloadFont(ctx.font);
		lua_close(L);
	}

	return 0;
}
