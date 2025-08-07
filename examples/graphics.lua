open_window(800, 800, "Sample Window")
set_fps(60)

while window_running() do
	begin_drawing()
	clear_window(color.BLACK)

	draw_rect(100, 100, 300, 200, color.YELLOW)
	draw_text("YOOOOO", 10, 10, 20, color.VIOLET)

	draw_info()
	end_drawing()
end

close_window()
