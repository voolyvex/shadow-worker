from PIL import Image, ImageDraw, ImageFont
import os

def create_directory_if_not_exists(path):
    if not os.path.exists(path):
        os.makedirs(path)

def generate_bitmap_font():
    # Font settings
    char_width = 16
    char_height = 16
    grid_size = 16  # 16x16 grid for 256 characters
    image_size = (char_width * grid_size, char_height * grid_size)
    
    # Create base image with transparency
    image = Image.new('RGBA', image_size, (0, 0, 0, 0))
    draw = ImageDraw.Draw(image)
    
    # Generate basic shapes for each character
    for i in range(256):
        x = (i % grid_size) * char_width
        y = (i // grid_size) * char_height
        
        # Draw character box
        draw.rectangle([(x, y), (x + char_width - 1, y + char_height - 1)],
                      outline=(255, 255, 255, 255))
        
        # Draw character (simple shapes for now)
        if i >= 32 and i <= 126:  # Printable ASCII
            # Draw a simple representation
            draw.rectangle([(x + 4, y + 4), (x + char_width - 5, y + char_height - 5)],
                         fill=(255, 255, 255, 255))
            
            # Add character identifier
            draw.text((x + 6, y + 4), chr(i), fill=(0, 0, 0, 255))
    
    return image

def main():
    # Create output directory
    create_directory_if_not_exists("../resources/fonts")
    
    # Generate and save bitmap font
    font = generate_bitmap_font()
    font.save("../resources/fonts/main.png")
    
    # Create a simple font descriptor file
    with open("../resources/fonts/main.fnt", "w") as f:
        f.write("info face=\"Shadow Worker Font\" size=16 bold=0 italic=0 charset=\"\" unicode=0 stretchH=100 smooth=0 aa=1 padding=0,0,0,0 spacing=1,1\n")
        f.write("common lineHeight=16 base=13 scaleW=256 scaleH=256 pages=1 packed=0\n")
        f.write("page id=0 file=\"main.png\"\n")
        f.write("chars count=95\n")
        
        # Write character info for printable ASCII
        for i in range(32, 127):
            x = (i % 16) * 16
            y = (i // 16) * 16
            f.write(f"char id={i} x={x} y={y} width=16 height=16 xoffset=0 yoffset=0 xadvance=16 page=0 chnl=0\n")

if __name__ == "__main__":
    main() 