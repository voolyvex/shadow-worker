from PIL import Image, ImageDraw

# Create a 64x64 image for the icon
img = Image.new('RGBA', (64, 64), (0, 0, 0, 0))
draw = ImageDraw.Draw(img)

# Draw a simple icon (blue square with inner details)
draw.rectangle([4, 4, 59, 59], fill=(60, 120, 255, 255), outline=(40, 100, 235, 255), width=2)
draw.rectangle([16, 16, 47, 47], fill=(80, 140, 255, 255))
draw.ellipse([24, 24, 39, 39], fill=(100, 160, 255, 255))

# Save as PNG
img.save('icon.png') 