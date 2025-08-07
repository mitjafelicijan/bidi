local file = io.open("examples/test.json", "r")
local content = file:read("*a")
file:close()

local data = json.decode(content)

print("name: " .. data.name)
for _, n in pairs(data.numbers) do
	print(" - number: " .. n)
end
