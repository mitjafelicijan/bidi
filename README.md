**Bidi** is a tiny framework/fantasy console for developing small video games for
learning and recreation. It takes heavy inspiration from
[LÖVE](https://www.love2d.org/), [Pico8](https://www.lexaloffle.com/pico-8.php)
and [Atari 2600](https://en.wikipedia.org/wiki/Atari_2600).

You develop your games in Lua and you are only allowed to have one Lua file
where all your code lives. You can then load external assets like images and
sounds.

> [!IMPORTANT]
> The Raylib library in the `vendor` folder contains prebuilt artifacts. All
> libraries are also available in the `archive` folder. You can manually
> download and replace them from official repositories if desired; they have
> not been tampered with.

## Documentation

### Included functions

| Function            | Description | Arguments | Returns |
|---------------------|-------------|-----------|---------|
| open_window         |             |           |         |
| close_window        |             |           |         |
| window_running      |             |           | bool    |
| start_drawing       |             |           |         |
| stop_drawing        |             |           |         |
| clear_window        |             |           |         |
| start_camera        |             |           |         |
| stop_camera         |             |           |         |
| move_camera         |             |           |         |
| set_fps             |             |           |         |
| get_fps             |             |           | number  |
| get_dt              |             |           | number  |
| get_width           |             |           | number  |
| get_height          |             |           | number  |
| draw_info           |             |           |         |
| draw_rect           |             |           |         |
| draw_text           |             |           |         |
| draw_pixel          |             |           |         |
| draw_line           |             |           |         |
| draw_circle         |             |           |         |
| draw_ellipse        |             |           |         |
| draw_triangle       |             |           |         |
| load_image          |             |           |         |
| load_audio          |             |           |         |
| button_down         |             |           | bool    |
| button_pressed      |             |           | bool    |

### Controller mappings

| Button           | Keyboard | Xbox | Playstation | Nintendo |
|------------------|----------|------|-------------|----------|
| button.PAD_UP    | W        | N/A  |             |          |
| button.PAD_DOWN  | S        | N/A  |             |          |
| button.PAD_LEFT  | A        | N/A  |             |          |
| button.PAD_RIGHT | D        | N/A  |             |          |
| button.A         | L        | N/A  |             |          |
| button.B         | P        | N/A  |             |          |
| button.X         | K        | N/A  |             |          |
| button.Y         | O        | N/A  |             |          |

### Default colors

| Color Name          | Red (r) | Green (g) | Blue (b) | Alpha (a) |
|---------------------|---------|-----------|----------|-----------|
| `color.LIGHTGRAY`   | 200     | 200       | 200      | 255       |
| `color.GRAY`        | 130     | 130       | 130      | 255       |
| `color.DARKGRAY`    | 80      | 80        | 80       | 255       |
| `color.YELLOW`      | 253     | 249       | 0        | 255       |
| `color.GOLD`        | 255     | 203       | 0        | 255       |
| `color.ORANGE`      | 255     | 161       | 0        | 255       |
| `color.PINK`        | 255     | 109       | 194      | 255       |
| `color.RED`         | 230     | 41        | 55       | 255       |
| `color.MAROON`      | 190     | 33        | 55       | 255       |
| `color.GREEN`       | 0       | 228       | 48       | 255       |
| `color.LIME`        | 0       | 158       | 47       | 255       |
| `color.DARKGREEN`   | 0       | 117       | 44       | 255       |
| `color.SKYBLUE`     | 102     | 191       | 255      | 255       |
| `color.BLUE`        | 0       | 121       | 241      | 255       |
| `color.DARKBLUE`    | 0       | 82        | 172      | 255       |
| `color.PURPLE`      | 200     | 122       | 255      | 255       |
| `color.VIOLET`      | 135     | 60        | 190      | 255       |
| `color.DARKPURPLE`  | 112     | 31        | 126      | 255       |
| `color.BEIGE`       | 211     | 176       | 131      | 255       |
| `color.BROWN`       | 127     | 106       | 79       | 255       |
| `color.DARKBROWN`   | 76      | 63        | 47       | 255       |
| `color.WHITE`       | 255     | 255       | 255      | 255       |
| `color.BLACK`       | 0       | 0         | 0        | 255       |
| `color.BLANK`       | 0       | 0         | 0        | 0         |
| `color.MAGENTA`     | 255     | 0         | 255      | 255       |

## Libraries & Assets

- https://github.com/rxi/microtar
- https://github.com/lua/lua
- https://github.com/raysan5/raylib
- https://github.com/rxi/json.lua
- https://dejavu-fonts.github.io/

## Cool tools

- https://hardwaretester.com/gamepad

## Development references

- https://www.love2d.org/wiki/Main_Page
- https://www.lexaloffle.com/dl/docs/pico-8_manual.html
- https://home.cs.colorado.edu/~main/bgi/doc/
