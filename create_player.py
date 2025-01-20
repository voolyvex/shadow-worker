from PIL import Image, ImageDraw

# Create a 32x32 image for the player
img = Image.new('RGBA', (32, 32), (0, 0, 0, 0))
draw = ImageDraw.Draw(img)

# Draw player body (blue circle)
draw.ellipse([4, 4, 27, 27], fill=(60, 120, 255, 255))

# Draw player details
draw.ellipse([8, 8, 23, 23], fill=(80, 140, 255, 255))
draw.ellipse([12, 12, 19, 19], fill=(100, 160, 255, 255))

# Save the image
img.save('assets/player.png') 