#include <stdio.h>
#include <stdarg.h>
#include <getopt.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "raylib.h"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "stdlib/json.h"
#include "stdlib/color.h"
#include "stdlib/button.h"
#include "stdlib/helpers.h"

#include "fonts/dejavusans_mono_bold.h"

#define VERSION "x.x"
#define DEBUG_LEVEL LOG_DEBUG
#define FONT_IMPORT_SIZE 30
#define UID_LENGTH 64
#define GAMEPAD_INDEX 0
#define XBOX_ALIAS_1 "xbox"
#define XBOX_ALIAS_2 "x-box"
#define PS_ALIAS "playstation"

typedef struct ExternalImage {
	char uid[UID_LENGTH + 1];
	Texture2D texture;
	struct ExternalImage *next;
} ExternalImage;

typedef struct {
	ExternalImage *head;
	ExternalImage *tail;
	int count;
} ImageList;

typedef struct ExternalSound {
	char uid[UID_LENGTH + 1];
	Sound sound;
	struct ExternalSound *next;
} ExternalSound;

typedef struct {
	ExternalSound *head;
	ExternalSound *tail;
	int count;
} SoundList;

typedef struct {
	Font font;
	int font_size;
	Camera2D camera;
	ImageList images;
	SoundList sounds;
} Context;

// Setting up global context.
Context ctx = {0};

static void generate_uid(char *str, size_t length) {
	static const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	size_t charset_size = sizeof(charset) - 1; // exclude null terminator

	for (size_t i = 0; i < length; i++) {
		int key = rand() % charset_size;
		str[i] = charset[key];
	}
	str[length] = '\0';
}

void init_image_list(ImageList *list) {
	list->head = NULL;
	list->tail = NULL;
	list->count = 0;
}

void init_sound_list(SoundList *list) {
	list->head = NULL;
	list->tail = NULL;
	list->count = 0;
}

void free_image_list(ImageList *list) {
	ExternalImage *curr = list->head;
	while (curr) {
		ExternalImage *next = curr->next;
		UnloadTexture(curr->texture);
		free(curr);
		curr = next;
	}
	list->head = list->tail = NULL;
	list->count = 0;
}

void free_sound_list(SoundList *list) {
	ExternalSound *curr = list->head;
	while (curr) {
		ExternalSound *next = curr->next;
		UnloadSound(curr->sound);
		free(curr);
		curr = next;
	}
	list->head = list->tail = NULL;
	list->count = 0;
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
		TraceLog(LOG_FATAL, "Error loading %s: %s\n", module_name, lua_tostring(L, -1));
		return 0;
	}
	lua_setglobal(L, module_name);
	return 1;
}

static int l_open_window(lua_State *L) {
	int width = luaL_checknumber(L, 1);
	int height = luaL_checknumber(L, 2);
	const char *title = luaL_checkstring(L, 3);
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	InitWindow(width, height, title);

	ctx.font_size = FONT_IMPORT_SIZE;
	ctx.font = LoadFontFromMemory(".ttf", dejavusans_mono_bold, dejavusans_mono_bold_len, ctx.font_size, NULL, 0);
	SetTextureFilter(ctx.font.texture, TEXTURE_FILTER_TRILINEAR);

	if (!IsFontValid(ctx.font)) {
		TraceLog(LOG_DEBUG, "Font not valid.");
	}

	ctx.camera.target = (Vector2){ 0.0f, 0.0f };
	ctx.camera.offset = (Vector2){ GetScreenHeight()/2.0f, GetScreenHeight()/2.0f };
	ctx.camera.rotation = 0.0f;
	ctx.camera.zoom = 1.0f;

	InitAudioDevice();

	init_image_list(&ctx.images);
	init_sound_list(&ctx.sounds);

	return 0;
}

static int l_close_window(lua_State *L) {
	free_image_list(&ctx.images);
	free_sound_list(&ctx.sounds);
	UnloadFont(ctx.font);
	CloseAudioDevice();
	CloseWindow();
	return 0;
}

// XXX: This function name is still a bit sus. Revisit the name later.
static int l_window_running(lua_State *L) {
	lua_pushboolean(L, !WindowShouldClose());
	return 1;
}

static int l_set_fps(lua_State *L) {
	int fps = luaL_checknumber(L, 1);
	SetTargetFPS(fps);
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

static int l_get_time(lua_State *L) {
	lua_pushnumber(L, GetTime());
	return 1;
}

static int l_get_width(lua_State *L) {
	lua_pushnumber(L, GetScreenWidth());
	return 1;
}

static int l_get_height(lua_State *L) {
	lua_pushnumber(L, GetScreenHeight());
	return 1;
}

static int l_start_drawing(lua_State *L) {
	BeginDrawing();
	return 0;
}

static int l_stop_drawing(lua_State *L) {
	EndDrawing();
	return 0;
}

static int l_start_camera(lua_State *L) {
	BeginMode2D(ctx.camera);
	return 0;
}

static int l_stop_camera(lua_State *L) {
	EndMode2D();
	return 0;
}

static int l_move_camera(lua_State *L) {
	int x = luaL_checknumber(L, 1);
	int y = luaL_checknumber(L, 2);

	ctx.camera.target.x = x;
	ctx.camera.target.y = y;
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

	DrawRectangle(10, height - 75, 150, 65, DARKBLUE);
	DrawTextEx(ctx.font, TextFormat("fps: %d", fps), (Vector2){ 15, height - 70 }, 16, 0, RAYWHITE);
	DrawTextEx(ctx.font, TextFormat("run: %.4f", runtime), (Vector2){ 15, height - 50 }, 16, 0, RAYWHITE);
	DrawTextEx(ctx.font, TextFormat("dt: %.4f", delta), (Vector2){ 15, height - 30 }, 16, 0, RAYWHITE);

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
	luaL_checktype(L, 1, LUA_TTABLE);
	int keyboard = (int)lua_getfield_int(L, 1, "keyboard");
	int xbox = (int)lua_getfield_int(L, 1, "xbox");
	int playstation = (int)lua_getfield_int(L, 1, "playstation");
	bool is_active = false;

	// Check keyboard first.
	if (IsKeyDown(keyboard)) {
		is_active = true;
	} else {
		// FIXME: This does not work currently due to a bug in GLFW that Raylib
		// is using. SDL could be used to compile Raylib but that doesn't
		// statically link against SDL so it's not ideal.

		// Check for controllers otherwise.
		if (IsGamepadAvailable(GAMEPAD_INDEX)) {
			// Xbox controller.
			if (TextFindIndex(TextToLower(GetGamepadName(GAMEPAD_INDEX)), XBOX_ALIAS_1) > -1 || TextFindIndex(TextToLower(GetGamepadName(GAMEPAD_INDEX)), XBOX_ALIAS_2) > -1) {
				if (IsGamepadButtonDown(GAMEPAD_INDEX, xbox)) {
					is_active = true;
				}
			}

			// Playstation controller.
			if (TextFindIndex(TextToLower(GetGamepadName(GAMEPAD_INDEX)), PS_ALIAS) > -1) {
				if (IsGamepadButtonDown(GAMEPAD_INDEX, playstation)) {
					is_active = true;
				}
			}
		}
	}

	lua_pushboolean(L, is_active);
	return 1;
}

static int l_button_pressed(lua_State *L) {
	luaL_checktype(L, 1, LUA_TTABLE);
	int keyboard = (int)lua_getfield_int(L, 1, "keyboard");
	int xbox = (int)lua_getfield_int(L, 1, "xbox");
	int playstation = (int)lua_getfield_int(L, 1, "playstation");
	bool is_active = false;

	// Check keyboard first.
	if (IsKeyPressed(keyboard)) {
		is_active = true;
	} else {
		// FIXME: This does not work currently due to a bug in GLFW that Raylib
		// is using. SDL could be used to compile Raylib but that doesn't
		// statically link against SDL so it's not ideal.

		// Check for controllers otherwise.
		if (IsGamepadAvailable(GAMEPAD_INDEX)) {
			// Xbox controller.
			if (TextFindIndex(TextToLower(GetGamepadName(GAMEPAD_INDEX)), XBOX_ALIAS_1) > -1 || TextFindIndex(TextToLower(GetGamepadName(GAMEPAD_INDEX)), XBOX_ALIAS_2) > -1) {
				if (IsGamepadButtonPressed(GAMEPAD_INDEX, xbox)) {
					is_active = true;
				}
			}

			// Playstation controller.
			if (TextFindIndex(TextToLower(GetGamepadName(GAMEPAD_INDEX)), PS_ALIAS) > -1) {
				if (IsGamepadButtonPressed(GAMEPAD_INDEX, playstation)) {
					is_active = true;
				}
			}
		}
	}

	lua_pushboolean(L, is_active);
	return 1;
}

static int l_load_image(lua_State *L) {
	const char *filepath = luaL_checkstring(L, 1);

	ExternalImage *img = malloc(sizeof(ExternalImage));
	if (!img) {
		TraceLog(LOG_FATAL, "Out of memory!");
		return 0;
	}

	Image image = LoadImage(filepath);
	if (!image.data) {
		TraceLog(LOG_FATAL, "Failed to load image: %s\n", filepath);
		free(img);
		return 0;
	}

	img->texture = LoadTextureFromImage(image);
	UnloadImage(image);

	generate_uid(img->uid, UID_LENGTH);
	img->next = NULL;

	if (ctx.images.tail) {
		ctx.images.tail->next = img;
		ctx.images.tail = img;
	} else {
		ctx.images.head = img;
		ctx.images.tail = img;
	}
	ctx.images.count++;

	TraceLog(LOG_DEBUG, "[add_img] %s (%d)", img->uid, strlen(img->uid));

	lua_pushstring(L, img->uid);
	return 1;
}

static int l_draw_image(lua_State *L) {
	const char *uid = luaL_checkstring(L, 1);
	int x = luaL_checknumber(L, 2);
	int y = luaL_checknumber(L, 3);

	ExternalImage *current = ctx.images.head;
	while (current) {
		if (strncmp(current->uid, uid, UID_LENGTH) == 0) {
			DrawTexture(current->texture, x, y, WHITE);
			break;
		}
		current = current->next;
	}

	return 0;
}

static int l_load_sound(lua_State *L) {
	const char *filepath = luaL_checkstring(L, 1);

	ExternalSound *snd = malloc(sizeof(ExternalSound));
	if (!snd) {
		TraceLog(LOG_FATAL, "Out of memory!");
		return 0;
	}

	snd->sound = LoadSound(filepath);
	generate_uid(snd->uid, UID_LENGTH);
	snd->next = NULL;

	if (ctx.sounds.tail) {
		ctx.sounds.tail->next = snd;
		ctx.sounds.tail = snd;
	} else {
		ctx.sounds.head = snd;
		ctx.sounds.tail = snd;
	}
	ctx.sounds.count++;

	TraceLog(LOG_DEBUG, "[add_snd] %s (%d)", snd->uid, strlen(snd->uid));

	lua_pushstring(L, snd->uid);
	return 1;
}

static int l_play_sound(lua_State *L) {
	const char *uid = luaL_checkstring(L, 1);

	ExternalSound *current = ctx.sounds.head;
	while (current) {
		if (strncmp(current->uid, uid, UID_LENGTH) == 0) {
			PlaySound(current->sound);
			break;
		}
		current = current->next;
	}

	return 0;
}

static int l_stop_sound(lua_State *L) {
	const char *uid = luaL_checkstring(L, 1);

	ExternalSound *current = ctx.sounds.head;
	while (current) {
		if (strncmp(current->uid, uid, UID_LENGTH) == 0) {
			StopSound(current->sound);
			break;
		}
		current = current->next;
	}

	return 0;
}

static void help(const char *argv0) {
	printf("Usage: %s [options]\n"
			"\nAvailable options:\n"
			"  -f,--file=file.lua      run input file\n"
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
		{ "file", 1, NULL, 'f' },
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
		if (!load_embedded_module(L, helpers, helpers_len, "helpers")) return 1;

		// Registring Raylib mappings.
		lua_register(L, "open_window", l_open_window);
		lua_register(L, "close_window", l_close_window);
		lua_register(L, "window_running", l_window_running);

		lua_register(L, "start_drawing", l_start_drawing);
		lua_register(L, "stop_drawing", l_stop_drawing);
		lua_register(L, "clear_window", l_clear_window);

		lua_register(L, "start_camera", l_start_camera);
		lua_register(L, "stop_camera", l_stop_camera);
		lua_register(L, "move_camera", l_move_camera);

		lua_register(L, "set_fps", l_set_fps);
		lua_register(L, "get_fps", l_get_fps);
		lua_register(L, "get_dt", l_get_dt);
		lua_register(L, "get_time", l_get_time);
		lua_register(L, "get_width", l_get_width);
		lua_register(L, "get_height", l_get_height);

		lua_register(L, "button_down", l_button_down);
		lua_register(L, "button_pressed", l_button_pressed);

		lua_register(L, "draw_info", l_draw_info);
		lua_register(L, "draw_rect", l_draw_rect);
		lua_register(L, "draw_text", l_draw_text);
		lua_register(L, "draw_pixel", l_draw_pixel);
		lua_register(L, "draw_line", l_draw_line);
		lua_register(L, "draw_circle", l_draw_circle);
		lua_register(L, "draw_ellipse", l_draw_ellipse);
		lua_register(L, "draw_triangle", l_draw_triangle);

		lua_register(L, "load_image", l_load_image);
		lua_register(L, "draw_image", l_draw_image);

		lua_register(L, "load_sound", l_load_sound);
		lua_register(L, "play_sound", l_play_sound);
		lua_register(L, "stop_sound", l_play_sound);

		// Interpreting and running input file Lua script.
		if (luaL_loadfile(L, run_file) || lua_pcall(L, 0, 0, 0)) {
			TraceLog(LOG_FATAL, "Error: %s\n", lua_tostring(L, -1));
			return 1;
		}

		lua_close(L);
	}

	return 0;
}
