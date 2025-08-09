asset1 = load_image("examples/icons/armor_3.png")

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

function test_buttons()
	if button_pressed(button.PAD_UP) then
		draw_text("Pad Up", 10, 10, 20, color.VIOLET)
	end

	if button_pressed(button.PAD_DOWN) then
		draw_text("Pad Down", 10, 40, 20, color.VIOLET)
	end

	if button_pressed(button.PAD_LEFT) then
		draw_text("Pad Left", 10, 70, 20, color.VIOLET)
	end

	if button_pressed(button.PAD_RIGHT) then
		draw_text("Pad Right", 10, 100, 20, color.VIOLET)
	end
	
	if button_pressed(button.A) then
		draw_text("A", 150, 10, 20, color.VIOLET)
	end

	if button_pressed(button.B) then
		draw_text("B", 150, 40, 20, color.VIOLET)
	end

	if button_pressed(button.X) then
		draw_text("X", 150, 70, 20, color.VIOLET)
	end

	if button_pressed(button.Y) then
		draw_text("Y", 150, 100, 20, color.VIOLET)
	end
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
