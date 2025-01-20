@tool
extends EditorScript

func _run():
	# Generate player sprite
	var player_image = Image.create(32, 32, false, Image.FORMAT_RGBA8)
	player_image.fill(Color(0.2, 0.6, 1.0, 1.0))  # Blue player
	# Add a simple shape
	for x in range(8, 24):
		for y in range(8, 24):
			player_image.set_pixel(x, y, Color(0.3, 0.7, 1.0, 1.0))
	player_image.save_png("res://assets/player/player.png")
	
	# Generate tileset
	var tileset_image = Image.create(64, 32, false, Image.FORMAT_RGBA8)
	# Floor tile (gray)
	for x in range(32):
		for y in range(32):
			var color = Color(0.8, 0.8, 0.8, 1.0)
			if (x + y) % 4 == 0:
				color = Color(0.7, 0.7, 0.7, 1.0)
			tileset_image.set_pixel(x, y, color)
	# Wall tile (brown)
	for x in range(32, 64):
		for y in range(32):
			var color = Color(0.6, 0.4, 0.2, 1.0)
			if (x + y) % 4 == 0:
				color = Color(0.5, 0.3, 0.1, 1.0)
			tileset_image.set_pixel(x, y, color)
	tileset_image.save_png("res://assets/tiles/tileset.png")
	
	print("Assets generated successfully!") 
