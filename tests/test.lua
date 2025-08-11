math.randomseed(os.time())

open_window(800, 800, "Sample Window")
set_fps(60)

test_button_square = { x = 400, y = 400 }
test_button_color = color.RED
test_button_speed = 200
test_camera_position = { x = 0, y = 0 }
test_camera_speed = 200
test_images_asset1 = load_image("tests/icons/icon_1.png")
test_images_asset2 = load_image("tests/icons/icon_2.png")

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
		table.insert(keys, k)
	end
	local randomKey = keys[math.random(1, #keys)]
	return color[randomKey]
end

function test_json()
	local file = io.open("tests/test.json", "r")
	local content = file:read("*a")
	file:close()

	local data = json.decode(content)

	print("name: " .. data.name)
	for _, n in pairs(data.numbers) do
		print(" - number: " .. n)
	end
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

	if button_down(button.SELECT) then
		draw_text("Select", 10, 130, 20, color.VIOLET)
	end

	if button_down(button.START) then
		draw_text("Start", 10, 160, 20, color.VIOLET)
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

function test_camera()
	if button_down(button.PAD_UP) then
		test_camera_position.y = test_camera_position.y - (test_camera_speed * get_dt())
	end

	if button_down(button.PAD_DOWN) then
		test_camera_position.y = test_camera_position.y + (test_camera_speed * get_dt())
	end

	if button_down(button.PAD_LEFT) then
		test_camera_position.x = test_camera_position.x - (test_camera_speed * get_dt())
	end

	if button_down(button.PAD_RIGHT) then
		test_camera_position.x = test_camera_position.x + (test_camera_speed * get_dt())
	end

	-- Using camera
	move_camera(test_camera_position.x, test_camera_position.y)

	draw_rect(100, 100, 300, 200, color.BLUE)

	start_camera()
	draw_rect(0, 0, 300, 200, color.YELLOW)
	stop_camera()

	draw_text("This text doesn't move!", 10, 10, 20, color.VIOLET)
end

function test_images()
	draw_text(string.format("uid: %s", test_images_asset1), 50, 50, 20, color.VIOLET)
	draw_text(string.format("uid: %s", test_images_asset2), 50, 80, 20, color.VIOLET)

	draw_image(test_images_asset1, 150, 150)
	draw_image(test_images_asset2, 200, 200)
end

while window_running() do
	start_drawing()
	clear_window(color.BLACK)

	-- test_json()
	-- test_api()
	-- test_buttons()
	-- test_camera()
	-- test_images()

	draw_info()
	stop_drawing()
end

close_window()
