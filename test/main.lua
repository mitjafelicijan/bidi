function test_json()
	local file = io.open("test/test.json", "r")
	local content = file:read("*a")
	file:close()

	local data = json.decode(content)

	print("name: " .. data.name)
	for _, n in pairs(data.numbers) do
		print(" - number: " .. n)
	end
end

function test_graphics()
	open_window(600, 600, "My Game")
	set_fps(60)

	while window_running() do
		begin_drawing()
		clear_window(color.RAYWHITE)
		draw_fps()
		end_drawing()
	end

	close_window()
end

test_json()
test_graphics()
