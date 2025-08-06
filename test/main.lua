for i = 0, 3 do
	print(string.format("Hi %d times", i))
end

local file = io.open("test/test.txt", "r")
local content = file:read("*a")
print(content)
file:close()

open_window(600, 600, "My Game")
set_fps(60)

while not window_should_close() do
	begin_drawing()
	clear_window()

	draw_fps()
	end_drawing()
end

close_window()
