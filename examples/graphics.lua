open_window(800, 800, "Sample Window")
set_fps(60)

while window_running() do
	begin_drawing()
	clear_window(color.BLACK)

	draw_rect(100, 100, 300, 200, color.YELLOW)
	draw_text("YOOOOO", 10, 10, 20, color.VIOLET)

	draw_text(string.format("fps: %d", get_fps()), 10, 30, 20, color.VIOLET)
	draw_text(string.format("dt: %.3f", get_dt()), 10, 50, 20, color.VIOLET)

	draw_line(400, 10, 500, 100, color.RED)
	draw_circle(500, 500, 100, color.BLUE)
	draw_ellipse(200, 500, 100, 50, color.BLUE)

	draw_triangle(20, 20, 100, 20, 50, 100, color.BLUE)

	draw_info()
	end_drawing()
end

close_window()
