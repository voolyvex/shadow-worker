from PIL import Image, ImageDraw

# Create a 32x32 image for the NPC
img = Image.new('RGBA', (32, 32), (0, 0, 0, 0))
draw = ImageDraw.Draw(img)

# Draw NPC body (green diamond)
points = [(16, 4), (28, 16), (16, 28), (4, 16)]
draw.polygon(points, fill=(60, 255, 120, 255))

# Draw NPC details
inner_points = [(16, 8), (24, 16), (16, 24), (8, 16)]
draw.polygon(inner_points, fill=(80, 255, 140, 255))
draw.ellipse([12, 12, 19, 19], fill=(100, 255, 160, 255))

# Save the image
img.save('assets/npc.png') 