#include <stdio.h>
#include <stdarg.h>
#include <getopt.h>
#include <stdlib.h>
#include <time.h>

#include "raylib.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "stdlib/json.h"
#include "stdlib/color.h"
#include "stdlib/button.h"
#include "stdlib/tilemap.h"

#include "fonts/dejavusans_mono_bold.h"

#define VERSION "x.x"
#define DEBUG_LEVEL LOG_DEBUG
#define FONT_IMPORT_SIZE 30
#define UID_LENGTH 36

typedef struct {
	char uid[UID_LENGTH + 1];
	const char* filepath;
} ExternalImage;

typedef struct {
} ExternalAudio;

typedef struct {
	Font font;
	int font_size;
	Camera2D camera;
	ExternalImage *images;
	ExternalAudio *audio;
} Context;

// Setting up global context.
Context ctx = {0};

static void generate_uid(char *uid) {
	const char *chars = "0123456789abcdef";
	for (size_t i = 0; i < UID_LENGTH; i++) {
		uid[i] = chars[rand() % 16];
	}
	uid[UID_LENGTH + 1] = '\0';
}

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

static int l_get_dt(lua_State *L) {
	lua_pushnumber(L, GetFrameTime());
	return 1;
}

static int l_get_fps(lua_State *L) {
	lua_pushnumber(L, GetFPS());
	return 1;
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

static int l_draw_rect(lua_State *L) {
	int x = luaL_checknumber(L, 1);
	int y = luaL_checknumber(L, 2);
	int width = luaL_checknumber(L, 3);
	int height = luaL_checknumber(L, 4);
	luaL_checktype(L, 5, LUA_TTABLE);
	Color color = {
		.r = (unsigned char)lua_getfield_int(L, 5, "r"),
		.g = (unsigned char)lua_getfield_int(L, 5, "g"),
		.b = (unsigned char)lua_getfield_int(L, 5, "b"),
		.a = (unsigned char)lua_getfield_int_opt(L, 5, "a", 255)
	};
	DrawRectangle(x, y, width, height, color);
	return 0;
}

static int l_draw_text(lua_State *L) {
	const char *text = luaL_checkstring(L, 1);
	int x = luaL_checknumber(L, 2);
	int y = luaL_checknumber(L, 3);
	int size = luaL_checknumber(L, 4);
	luaL_checktype(L, 5, LUA_TTABLE);
	Color color = {
		.r = (unsigned char)lua_getfield_int(L, 5, "r"),
		.g = (unsigned char)lua_getfield_int(L, 5, "g"),
		.b = (unsigned char)lua_getfield_int(L, 5, "b"),
		.a = (unsigned char)lua_getfield_int_opt(L, 5, "a", 255)
	};
	DrawTextEx(ctx.font, text, (Vector2){ x, y }, size, 0, color);
	return 0;
}

static int l_draw_pixel(lua_State *L) {
	int x = luaL_checknumber(L, 1);
	int y = luaL_checknumber(L, 2);
	luaL_checktype(L, 3, LUA_TTABLE);
	Color color = {
		.r = (unsigned char)lua_getfield_int(L, 3, "r"),
		.g = (unsigned char)lua_getfield_int(L, 3, "g"),
		.b = (unsigned char)lua_getfield_int(L, 3, "b"),
		.a = (unsigned char)lua_getfield_int_opt(L, 3, "a", 255)
	};
	DrawPixel(x, y, color);
}

static int l_draw_line(lua_State *L) {
	int x1 = luaL_checknumber(L, 1);
	int y1 = luaL_checknumber(L, 2);
	int x2 = luaL_checknumber(L, 3);
	int y2 = luaL_checknumber(L, 4);
	luaL_checktype(L, 5, LUA_TTABLE);
	Color color = {
		.r = (unsigned char)lua_getfield_int(L, 5, "r"),
		.g = (unsigned char)lua_getfield_int(L, 5, "g"),
		.b = (unsigned char)lua_getfield_int(L, 5, "b"),
		.a = (unsigned char)lua_getfield_int_opt(L, 5, "a", 255)
	};
	DrawLine(x1, y1, x2, y2, color);
	return 0;
}

static int l_draw_circle(lua_State *L) {
	int center_x = luaL_checknumber(L, 1);
	int center_y = luaL_checknumber(L, 2);
	int radius = luaL_checknumber(L, 3);
	luaL_checktype(L, 4, LUA_TTABLE);
	Color color = {
		.r = (unsigned char)lua_getfield_int(L, 4, "r"),
		.g = (unsigned char)lua_getfield_int(L, 4, "g"),
		.b = (unsigned char)lua_getfield_int(L, 4, "b"),
		.a = (unsigned char)lua_getfield_int_opt(L, 4, "a", 255)
	};
	DrawCircle(center_x, center_y, radius, color);
	return 0;
}

static int l_draw_ellipse(lua_State *L) {
	int center_x = luaL_checknumber(L, 1);
	int center_y = luaL_checknumber(L, 2);
	int radius_h = luaL_checknumber(L, 3);
	int radius_v = luaL_checknumber(L, 4);
	luaL_checktype(L, 5, LUA_TTABLE);
	Color color = {
		.r = (unsigned char)lua_getfield_int(L, 5, "r"),
		.g = (unsigned char)lua_getfield_int(L, 5, "g"),
		.b = (unsigned char)lua_getfield_int(L, 5, "b"),
		.a = (unsigned char)lua_getfield_int_opt(L, 5, "a", 255)
	};
	DrawEllipse(center_x, center_y, radius_h, radius_v, color);
	return 0;
}

static int l_draw_triangle(lua_State *L) {
	int x1 = luaL_checknumber(L, 1);
	int y1 = luaL_checknumber(L, 2);
	int x2 = luaL_checknumber(L, 3);
	int y2 = luaL_checknumber(L, 4);
	int x3 = luaL_checknumber(L, 5);
	int y3 = luaL_checknumber(L, 6);
	luaL_checktype(L, 7, LUA_TTABLE);
	Color color = {
		.r = (unsigned char)lua_getfield_int(L, 7, "r"),
		.g = (unsigned char)lua_getfield_int(L, 7, "g"),
		.b = (unsigned char)lua_getfield_int(L, 7, "b"),
		.a = (unsigned char)lua_getfield_int_opt(L, 7, "a", 255)
	};

	// NOTE: Raylib orders vertices in counter-clockwise order. We order them
	// in clockwise order instead to make this a bit easier to use.
	Vector2 p1 = { x1, y1 };
	Vector2 p2 = { x3, y3 };
	Vector2 p3 = { x2, y2 };

	DrawTriangle(p1, p2, p3, color);
	return 0;
}

static int l_button_down(lua_State *L) {
	int button = luaL_checknumber(L, 1);
	lua_pushboolean(L, IsKeyDown(button));
	return 1;
}

static int l_button_pressed(lua_State *L) {
	int button = luaL_checknumber(L, 1);
	lua_pushboolean(L, IsKeyPressed(button));
	return 1;
}

static int l_load_image(lua_State *L) {
	return 0;
}

static int l_load_audio(lua_State *L) {
	return 0;
}

static int l_move_camera(lua_State *L) {
	return 0;
}

static void help(const char *argv0) {
	printf("Usage: %s [options]\n"
			"\nAvailable options:\n"
			"  -r,--run=file.lua       run input file\n"
			"  -b,--bundle             bundles this folder\n"
			"  -d,--debug              prints debug information\n"
			"  -h,--help               this help\n"
			"  -v,--version            show version\n",
			argv0);
}

static void version(const char *argv0) {
	printf("%s version %s\n", argv0, VERSION);
}

int main(int argc, char *argv[]) {
	srand(time(NULL));

	TraceLogLevel debug_level = LOG_WARNING;
	const char *run_file = NULL;

	const char short_options[] = "f:dbhv";
	const struct option long_options[] = {
		{ "file", 1, NULL, 'r' },
		{ "debug", 0, NULL, 'd' },
		{ "bundle", 0, NULL, 'b' },
		{ "help", 0, NULL, 'h' },
		{ "version", 0, NULL, 'v' },
		{ 0 },
	};

	int opt;
	while ((opt = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
		switch (opt) {
			case 'f':
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

		// Loading embeded modules into Lua state.
		if (!load_embedded_module(L, json, json_len, "json")) return 1;
		if (!load_embedded_module(L, color, color_len, "color")) return 1;
		if (!load_embedded_module(L, button, button_len, "button")) return 1;
		if (!load_embedded_module(L, tilemap, tilemap_len, "tilemap")) return 1;

		// Registring Raylib mappings.
		lua_register(L, "open_window", l_open_window);
		lua_register(L, "close_window", l_close_window);
		lua_register(L, "window_running", l_window_running);
		lua_register(L, "begin_drawing", l_begin_drawing);
		lua_register(L, "end_drawing", l_end_drawing);
		lua_register(L, "set_fps", l_set_fps);
		lua_register(L, "get_fps", l_get_fps);
		lua_register(L, "get_dt", l_get_dt);
		lua_register(L, "clear_window", l_clear_window);
		lua_register(L, "draw_info", l_draw_info);
		lua_register(L, "draw_rect", l_draw_rect);
		lua_register(L, "draw_text", l_draw_text);
		lua_register(L, "draw_pixel", l_draw_pixel);
		lua_register(L, "draw_line", l_draw_line);
		lua_register(L, "draw_circle", l_draw_circle);
		lua_register(L, "draw_ellipse", l_draw_ellipse);
		lua_register(L, "draw_triangle", l_draw_triangle);
		lua_register(L, "load_image", l_load_image);
		lua_register(L, "load_audio", l_load_audio);
		lua_register(L, "move_camera", l_move_camera);
		lua_register(L, "button_down", l_button_down);
		lua_register(L, "button_pressed", l_button_pressed);

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
