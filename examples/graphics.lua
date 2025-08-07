open_window(800, 800, "Sample Window")
set_fps(60)

while window_running() do
	begin_drawing()
	clear_window(color.BLACK)
	draw_info()
	end_drawing()
end

close_window()
