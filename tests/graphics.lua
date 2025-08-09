math.randomseed(os.time())

test_button_square = { x = 400, y = 400 }
test_button_color = color.RED
test_button_speed = 200

test_images_asset1 = load_image("tests/icons/icon_1.png")

open_window(800, 800, "Sample Window")
set_fps(60)

function test_api()
	draw_rect(100, 100, 300, 200, color.YELLOW)
	draw_text("Label text", 10, 10, 20, color.VIOLET)
	draw_line(400, 10, 500, 100, color.RED)
	draw_circle(500, 500, 100, color.BLUE)
	draw_ellipse(200, 500, 100, 50, color.BLUE)
	draw_triangle(20, 20, 100, 20, 50, 100, color.BLUE)
	draw_text(string.format("fps: %d", get_fps()), 10, 30, 20, color.VIOLET)
	draw_text(string.format("dt: %.3f", get_dt()), 10, 50, 20, color.VIOLET)
end

function get_random_color()
    local keys = {}
    for k in pairs(color) do
        table.insert(keys, k)  -- Collect all keys
    end
    local randomKey = keys[math.random(1, #keys)]  -- Select a random key
    return color[randomKey]  -- Return the corresponding color
end

function test_buttons()
	-- Testing button presses.
	if button_down(button.PAD_UP) then
		draw_text("Pad Up", 10, 10, 20, color.VIOLET)
	end

	if button_down(button.PAD_DOWN) then
		draw_text("Pad Down", 10, 40, 20, color.VIOLET)
	end

	if button_down(button.PAD_LEFT) then
		draw_text("Pad Left", 10, 70, 20, color.VIOLET)
	end

	if button_down(button.PAD_RIGHT) then
		draw_text("Pad Right", 10, 100, 20, color.VIOLET)
	end
	
	if button_down(button.A) then
		draw_text("A", 150, 10, 20, color.VIOLET)
	end

	if button_down(button.B) then
		draw_text("B", 150, 40, 20, color.VIOLET)
	end

	if button_down(button.X) then
		draw_text("X", 150, 70, 20, color.VIOLET)
	end

	if button_down(button.Y) then
		draw_text("Y", 150, 100, 20, color.VIOLET)
	end

	-- Moving square around.
	if button_down(button.PAD_UP) then
		test_button_square.y = test_button_square.y - (test_button_speed * get_dt())
	end

	if button_down(button.PAD_DOWN) then
		test_button_square.y = test_button_square.y + (test_button_speed * get_dt())
	end

	if button_down(button.PAD_LEFT) then
		test_button_square.x = test_button_square.x - (test_button_speed * get_dt())
	end

	if button_down(button.PAD_RIGHT) then
		test_button_square.x = test_button_square.x + (test_button_speed * get_dt())
	end

	if button_pressed(button.A) then
		test_button_color = get_random_color()
	end

	draw_rect(test_button_square.x, test_button_square.y, 50, 50, test_button_color)
end

while window_running() do
	begin_drawing()
	clear_window(color.BLACK)

	-- test_api()
	test_buttons()

	draw_info()
	end_drawing()
end

close_window()
