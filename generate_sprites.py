from PIL import Image, ImageDraw
import os

def ensure_dir(directory):
    if not os.path.exists(directory):
        os.makedirs(directory)

def create_player_idle():
    # Create a 32x32 sprite with transparency
    img = Image.new('RGBA', (32, 32), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    
    # Body (dark gray)
    draw.rectangle([12, 8, 20, 24], fill=(64, 64, 64, 255))
    # Head (light gray)
    draw.ellipse([10, 2, 22, 14], fill=(192, 192, 192, 255))
    # Eyes (white with black pupils)
    draw.rectangle([13, 6, 15, 8], fill=(255, 255, 255, 255))
    draw.rectangle([17, 6, 19, 8], fill=(255, 255, 255, 255))
    draw.point([14, 7], fill=(0, 0, 0, 255))
    draw.point([18, 7], fill=(0, 0, 0, 255))
    
    ensure_dir('resources/sprites/player')
    img.save('resources/sprites/player/idle.png')

def create_player_walk():
    # Create a 128x32 sprite sheet (4 frames)
    img = Image.new('RGBA', (128, 32), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    
    # Create 4 walking frames
    for frame in range(4):
        x_offset = frame * 32
        # Body
        draw.rectangle([x_offset + 12, 8, x_offset + 20, 24], fill=(64, 64, 64, 255))
        # Head
        draw.ellipse([x_offset + 10, 2, x_offset + 22, 14], fill=(192, 192, 192, 255))
        # Eyes
        draw.rectangle([x_offset + 13, 6, x_offset + 15, 8], fill=(255, 255, 255, 255))
        draw.rectangle([x_offset + 17, 6, x_offset + 19, 8], fill=(255, 255, 255, 255))
        draw.point([x_offset + 14, 7], fill=(0, 0, 0, 255))
        draw.point([x_offset + 18, 7], fill=(0, 0, 0, 255))
        # Legs (alternating positions)
        if frame % 2 == 0:
            draw.line([x_offset + 14, 24, x_offset + 12, 30], fill=(64, 64, 64, 255), width=2)
            draw.line([x_offset + 18, 24, x_offset + 20, 30], fill=(64, 64, 64, 255), width=2)
        else:
            draw.line([x_offset + 14, 24, x_offset + 16, 30], fill=(64, 64, 64, 255), width=2)
            draw.line([x_offset + 18, 24, x_offset + 16, 30], fill=(64, 64, 64, 255), width=2)
    
    ensure_dir('resources/sprites/player')
    img.save('resources/sprites/player/walk.png')

def create_tileset():
    # Create a 128x128 tileset
    img = Image.new('RGBA', (128, 128), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    
    # Floor tile (light gray with grid pattern)
    for y in range(4):
        for x in range(4):
            draw.rectangle([x*32, y*32, (x+1)*32-1, (y+1)*32-1], fill=(200, 200, 200, 255))
            draw.rectangle([x*32+1, y*32+1, (x+1)*32-2, (y+1)*32-2], fill=(180, 180, 180, 255))
            draw.line([x*32, y*32, (x+1)*32, y*32], fill=(160, 160, 160, 255))
            draw.line([x*32, y*32, x*32, (y+1)*32], fill=(160, 160, 160, 255))
    
    # Wall tiles (darker with texture)
    for i in range(2):
        draw.rectangle([i*32, 96, (i+1)*32-1, 127], fill=(100, 100, 100, 255))
        for j in range(3):
            draw.line([i*32+j*10, 96, i*32+j*10, 127], fill=(90, 90, 90, 255))
    
    # Door tile
    draw.rectangle([64, 96, 95, 127], fill=(139, 69, 19, 255))
    draw.ellipse([88, 108, 92, 112], fill=(200, 200, 0, 255))  # Door handle
    
    # Obstacle tile
    draw.rectangle([96, 96, 127, 127], fill=(120, 120, 120, 255))
    draw.polygon([96, 96, 127, 96, 111, 111], fill=(140, 140, 140, 255))
    
    ensure_dir('resources/maps')
    img.save('resources/maps/tileset_main.png')

def create_npc_base():
    # Create a 32x32 sprite
    img = Image.new('RGBA', (32, 32), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    
    # Body (different color from player)
    draw.rectangle([12, 8, 20, 24], fill=(128, 0, 0, 255))
    # Head
    draw.ellipse([10, 2, 22, 14], fill=(255, 200, 200, 255))
    # Eyes
    draw.rectangle([13, 6, 15, 8], fill=(255, 255, 255, 255))
    draw.rectangle([17, 6, 19, 8], fill=(255, 255, 255, 255))
    draw.point([14, 7], fill=(0, 0, 0, 255))
    draw.point([18, 7], fill=(0, 0, 0, 255))
    
    ensure_dir('resources/sprites/npc')
    img.save('resources/sprites/npc/base.png')

if __name__ == '__main__':
    create_player_idle()
    create_player_walk()
    create_tileset()
    create_npc_base()
    print("All sprites generated successfully!") 