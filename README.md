**Bidi** is a lightweight framework and/or fantasy console designed for
creating tiny video games, perfect for learning and having fun. It takes heavy
inspiration from [LÖVE](https://www.love2d.org/),
[Pico8](https://www.lexaloffle.com/pico-8.php), [Atari
2600](https://en.wikipedia.org/wiki/Atari_2600) and [Windows
3.1](https://en.wikipedia.org/wiki/Windows_3.1).

You develop your games in Lua and you are only allowed to have one Lua file
where all your code lives. You can then load external assets like images and
sounds.

> [!IMPORTANT]
> The Raylib library in the `vendor` folder contains prebuilt artifacts. All
> libraries are also available in the `archive` folder. You can manually
> download and replace them from official repositories if desired; they have
> not been tampered with.

## Documentation

### Quick example

```lua
-- mygame.lua
-- bidi -f mygame.lua

open_window(800, 800, "My Game")
set_fps(60)

axe_image = load_image("images/axe.png")
slash_effect = load_sound("sounds/slash.wav")

while window_running() do
    start_drawing()
    clear_window(color.BLACK)

    draw_rect(100, 100, 300, 200, color.YELLOW)
    draw_text("Label text", 10, 10, 20, color.VIOLET)
    draw_line(400, 10, 500, 100, color.RED)
    draw_circle(500, 500, 100, color.BLUE)
    draw_ellipse(200, 500, 100, 50, color.BLUE)
    draw_triangle(20, 20, 100, 20, 50, 100, color.BLUE)
    draw_text(string.format("fps: %d", get_fps()), 10, 30, 20, color.VIOLET)
    draw_text(string.format("dt: %.3f", get_dt()), 10, 50, 20, color.VIOLET)

	if button_down(button.PAD_UP) then
		draw_text("Pad Up", 10, 10, 20, color.VIOLET)
	end

	if button_pressed(button.A) then
        play_sound(slash_effect)
	end

    draw_image(axe_image, 100, 200)

    draw_info()
    stop_drawing()
end

close_window()
```

### Included functions

| Function         | Arguments                                                                                   | Returns  |
| ---------------- | ------------------------------------------------------------------------------------------- | -------- |
| `open_window`    | `number width`, `number height`, `string title`                                             |          |
| `close_window`   |                                                                                             |          |
| `window_running` |                                                                                             | `bool`   |
| `start_drawing`  |                                                                                             |          |
| `stop_drawing`   |                                                                                             |          |
| `clear_window`   | `color color`                                                                               |          |
| `start_camera`   |                                                                                             |          |
| `stop_camera`    |                                                                                             |          |
| `move_camera`    | `number x`, `number y`                                                                      |          |
| `set_fps`        | `number fps`                                                                                |          |
| `get_fps`        |                                                                                             | `number` |
| `get_dt`         |                                                                                             | `number` |
| `get_width`      |                                                                                             | `number` |
| `get_height`     |                                                                                             | `number` |
| `draw_info`      |                                                                                             |          |
| `draw_rect`      | `number x`, `number y`, `number width`, `number height`, `color color`                      |          |
| `draw_text`      | `string text`, `number x`, `number y`, `number font_size`, `color color`                    |          |
| `draw_pixel`     | `number x`, `number y`, `color color`                                                       |          |
| `draw_line`      | `number x1`, `number y1`, `number x2`, `number y2`, `color color`                           |          |
| `draw_circle`    | `number center_x`, `number center_y`, `number radius`, `color color`                        |          |
| `draw_ellipse`   | `number center_x`, `number center_y`, `number radius_h`, `number radius_v`, `color color`   |          |
| `draw_triangle`  | `number x1`, `number y1`, `number x2`, `number y2`, `number x3`, `number y3`, `color color` |          |
| `load_image`     | `string filepath`                                                                           | `uid`    |
| `draw_image`     | `uid uid`, `number x`, `number y`                                                           |          |
| `button_down`    | `button button`                                                                             | `bool`   |
| `button_pressed` | `button button`                                                                             | `bool`   |
| `load_sound`     | `string filepath`                                                                           | `uid`    |
| `play_sound`     | `uid uid`                                                                                   |          |
| `stop_sound`     | `uid uid`                                                                                   |          |

### Controller mappings

| Button             | Keyboard      | Xbox          | Playstation   |
| ------------------ | ------------- | ------------- | ------------- |
| `button.PAD_UP`    | `Arrow Up`    | `D-pad Up`    | `D-pad Up`    |
| `button.PAD_DOWN`  | `Arrow Down`  | `D-pad Down`  | `D-pad Down`  |
| `button.PAD_LEFT`  | `Arrow Left`  | `D-pad Left`  | `D-pad Left`  |
| `button.PAD_RIGHT` | `Arrow Right` | `D-pad Right` | `D-pad Right` |
| `button.A`         | `A`           | `A`           | `Cross`       |
| `button.B`         | `S`           | `B`           | `Circle`      |
| `button.X`         | `Q`           | `X`           | `Square`      |
| `button.Y`         | `W`           | `Y`           | `Triangle`    |
| `button.SELECT`    | `D`           | `Back`        | `Share`       |
| `button.START`     | `E`           | `Start`       | `Options`     |
                                                        
### Default colors

| Color Name         | Red (r) | Green (g) | Blue (b) | Alpha (a) |
| ------------------ | ------- | --------- | -------- | --------- |
| `color.LIGHTGRAY`  | 200     | 200       | 200      | 255       |
| `color.GRAY`       | 130     | 130       | 130      | 255       |
| `color.DARKGRAY`   | 80      | 80        | 80       | 255       |
| `color.YELLOW`     | 253     | 249       | 0        | 255       |
| `color.GOLD`       | 255     | 203       | 0        | 255       |
| `color.ORANGE`     | 255     | 161       | 0        | 255       |
| `color.PINK`       | 255     | 109       | 194      | 255       |
| `color.RED`        | 230     | 41        | 55       | 255       |
| `color.MAROON`     | 190     | 33        | 55       | 255       |
| `color.GREEN`      | 0       | 228       | 48       | 255       |
| `color.LIME`       | 0       | 158       | 47       | 255       |
| `color.DARKGREEN`  | 0       | 117       | 44       | 255       |
| `color.SKYBLUE`    | 102     | 191       | 255      | 255       |
| `color.BLUE`       | 0       | 121       | 241      | 255       |
| `color.DARKBLUE`   | 0       | 82        | 172      | 255       |
| `color.PURPLE`     | 200     | 122       | 255      | 255       |
| `color.VIOLET`     | 135     | 60        | 190      | 255       |
| `color.DARKPURPLE` | 112     | 31        | 126      | 255       |
| `color.BEIGE`      | 211     | 176       | 131      | 255       |
| `color.BROWN`      | 127     | 106       | 79       | 255       |
| `color.DARKBROWN`  | 76      | 63        | 47       | 255       |
| `color.WHITE`      | 255     | 255       | 255      | 255       |
| `color.BLACK`      | 0       | 0         | 0        | 255       |
| `color.BLANK`      | 0       | 0         | 0        | 0         |
| `color.MAGENTA`    | 255     | 0         | 255      | 255       |

#### Using custom colors

Color is essentially a structure with RGBA data. So constructing new colors is
very easy.

Alpha channel can be omitted and will by default be 255.

```lua
-- my_game.lua

open_window(800, 800, "My Game")
set_fps(60)

my_color1 = { r = 200, g = 200, b = 200, a = 255 }
my_color2 = { r = 200, g = 200, b = 200 }

while window_running() do
    start_drawing()
    clear_window(color.BLACK)

    draw_rect(100, 100, 100, 100, my_color1)
    draw_rect(300, 100, 100, 100, my_color2)

    draw_info()
    stop_drawing()
end

close_window()
```

## Libraries & Assets

- https://github.com/rxi/microtar
- https://github.com/lua/lua
- https://github.com/raysan5/raylib
- https://github.com/rxi/json.lua
- https://dejavu-fonts.github.io/
- https://freetestdata.com/audio-files/

## Cool tools

- Controller testing:
    - https://hardwaretester.com/gamepad
    - https://www.raylib.com/examples/core/loader.html?name=core_input_gamepad
- Sound effect generators:
    - https://www.bfxr.net
    - https://sfxr.me

## Interesting games

- https://www.old-games.com/download/3873/aethra-s-chronicles
- https://www.old-games.com/download/10746/wintrek-1992
- https://www.old-games.com/download/3887/bard-s-tale-1
- https://www.old-games.com/download/3902/castle-of-the-winds
- https://www.old-games.com/download/3923/decker
- https://www.old-games.com/download/3920/darkspyre
- https://www.old-games.com/download/6089/exile-3-ruined-world

## Development references

- https://www.love2d.org/wiki/Main_Page
- https://www.lexaloffle.com/dl/docs/pico-8_manual.html
- https://home.cs.colorado.edu/~main/bgi/doc/
