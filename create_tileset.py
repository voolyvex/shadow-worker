from PIL import Image, ImageDraw

# Create a 64x32 image for two 32x32 tiles
img = Image.new('RGBA', (64, 32), (0, 0, 0, 0))
draw = ImageDraw.Draw(img)

# Draw first tile (wall)
draw.rectangle([0, 0, 31, 31], fill=(100, 100, 100, 255), outline=(80, 80, 80, 255))
draw.rectangle([4, 4, 27, 27], fill=(120, 120, 120, 255))

# Draw second tile (floor)
draw.rectangle([32, 0, 63, 31], fill=(150, 150, 150, 255), outline=(130, 130, 130, 255))
draw.line([32, 16, 63, 16], fill=(140, 140, 140, 255))
draw.line([48, 0, 48, 31], fill=(140, 140, 140, 255))

# Save the image
img.save('assets/tiles/tileset.png')