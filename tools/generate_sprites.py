from PIL import Image, ImageDraw
import os

def create_directory_if_not_exists(path):
    if not os.path.exists(path):
        os.makedirs(path)

def generate_player_sprite():
    # Create base image with transparency
    size = (32, 32)
    image = Image.new('RGBA', size, (0, 0, 0, 0))
    draw = ImageDraw.Draw(image)
    
    # Draw basic humanoid shape
    # Head
    draw.ellipse([(12, 4), (20, 12)], fill=(200, 200, 200, 255))
    # Body
    draw.rectangle([(14, 12), (18, 22)], fill=(100, 100, 200, 255))
    # Arms
    draw.rectangle([(10, 14), (14, 20)], fill=(100, 100, 200, 255))
    draw.rectangle([(18, 14), (22, 20)], fill=(100, 100, 200, 255))
    # Legs
    draw.rectangle([(14, 22), (16, 28)], fill=(100, 100, 200, 255))
    draw.rectangle([(16, 22), (18, 28)], fill=(100, 100, 200, 255))
    
    return image

def generate_npc_sprite():
    # Create base image with transparency
    size = (32, 32)
    image = Image.new('RGBA', size, (0, 0, 0, 0))
    draw = ImageDraw.Draw(image)
    
    # Draw basic humanoid shape in different color
    # Head
    draw.ellipse([(12, 4), (20, 12)], fill=(200, 150, 150, 255))
    # Body
    draw.rectangle([(14, 12), (18, 22)], fill=(200, 100, 100, 255))
    # Arms
    draw.rectangle([(10, 14), (14, 20)], fill=(200, 100, 100, 255))
    draw.rectangle([(18, 14), (22, 20)], fill=(200, 100, 100, 255))
    # Legs
    draw.rectangle([(14, 22), (16, 28)], fill=(200, 100, 100, 255))
    draw.rectangle([(16, 22), (18, 28)], fill=(200, 100, 100, 255))
    
    return image

def generate_tileset():
    # Create base image with transparency
    tile_size = 32
    tiles_per_row = 5
    rows = 4
    size = (tile_size * tiles_per_row, tile_size * rows)
    image = Image.new('RGBA', size, (0, 0, 0, 0))
    draw = ImageDraw.Draw(image)
    
    # Empty tile (0, 0)
    # Leave transparent
    
    # Wall tile (1, 0)
    draw.rectangle([(tile_size, 0), (tile_size * 2 - 1, tile_size - 1)], 
                  fill=(100, 100, 100, 255))
    
    # Floor tile (2, 0)
    draw.rectangle([(tile_size * 2, 0), (tile_size * 3 - 1, tile_size - 1)], 
                  fill=(150, 150, 150, 255))
    draw.line([(tile_size * 2, 0), (tile_size * 3 - 1, tile_size - 1)], 
              fill=(130, 130, 130, 255))
    draw.line([(tile_size * 2, tile_size - 1), (tile_size * 3 - 1, 0)], 
              fill=(130, 130, 130, 255))
    
    # Door tile (3, 0)
    draw.rectangle([(tile_size * 3, 0), (tile_size * 4 - 1, tile_size - 1)], 
                  fill=(120, 80, 40, 255))
    draw.ellipse([(tile_size * 3 + 12, 14), (tile_size * 3 + 16, 18)], 
                 fill=(200, 200, 0, 255))
    
    # Obstacle tile (4, 0)
    draw.rectangle([(tile_size * 4, 0), (tile_size * 5 - 1, tile_size - 1)], 
                  fill=(160, 120, 80, 255))
    for i in range(4):
        draw.line([(tile_size * 4 + i * 8, 0), 
                   (tile_size * 4 + i * 8, tile_size - 1)], 
                  fill=(140, 100, 60, 255))
    
    return image

def main():
    # Create output directories
    create_directory_if_not_exists("../resources/sprites/player")
    create_directory_if_not_exists("../resources/sprites/npc")
    create_directory_if_not_exists("../resources/maps")
    
    # Generate and save player sprite
    player_sprite = generate_player_sprite()
    player_sprite.save("../resources/sprites/player/idle.png")
    
    # Generate and save NPC sprite
    npc_sprite = generate_npc_sprite()
    npc_sprite.save("../resources/sprites/npc/base.png")
    
    # Generate and save tileset
    tileset = generate_tileset()
    tileset.save("../resources/maps/tileset_main.png")

if __name__ == "__main__":
    main() 