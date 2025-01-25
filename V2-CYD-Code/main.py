#Define imports
from machine import Pin, SPI, ADC, idle
import os
from time import sleep
from ili9341 import Display, color565
from xglcd_font import XglcdFont
import TOUCH_INPUT as TI
import Menu


# Function to set up SPI for TFT display
display_spi = SPI(1, baudrate=60000000, sck=Pin(14), mosi=Pin(13))

# Set up display
display = Display(display_spi, dc=Pin(2), cs=Pin(15), rst=Pin(15),
                  width=320, height=240, rotation=270)

#val for diffrent stuff
img_val = 0 #image index
button_show = 1 #toggel for button (could be boolen to save storage!)
current_image_index = 0
#color definitions
white_color = color565(255, 255, 255)  # white color
black_color = color565(0, 0, 0)        # black color

def touched():
    global button_show
    
    x, y = TI.touched()

    if x < 20 and y > 200:
        button_show = 0
        print(button_show)
    
    print(x, y)
    if x > 175 and button_show == 1:
        Menu.main_menu()
    else:
        switch_img()
        draw_button_lall()
        
def draw_button_lall():
    global button_show
    if button_show == 1:
        display.draw_text8x8(200, 0, 'Hide Button', white_color, black_color, 0)
    
    if button_show == 1:
        display.draw_image('sd/main_menu_button.raw', 0, 175, 320, 65) # reset is needed to get back to 1!

def draw_img_lain():
    #draws image to the screen has to be raw imamge!
    #				   {dir}		{x} {y} {w}  {h}   {w} => width {h} => {hight}   
    display.draw_image('sd/lain.raw', 0, 0, 205, 240)
    
def switch_img():
    global current_image_index
    
    image_directory = "sd/"
    image_files = sorted([f for f in os.listdir(image_directory) if f.endswith("_full.raw")])
    
    # Check if there are any images
    if not image_files:
        print("No images found in the directory.")
        return

    # Get the next image file
    image_file = image_files[current_image_index]

    # Draw the image on the display
    display.draw_image(f"{image_directory}/{image_file}", 0, 0, 320, 240)

    # Update the index to the next image
    current_image_index = (current_image_index + 1) % len(image_files)  # Loop back to the first image

    #print(img_val) 
def clear():
    # Clear display
    display.clear(black_color)
    
def backlight_setup():
    # Turn on display backlight
    
    backlight = Pin(21, Pin.OUT)
    backlight.on()

def main():

    try:
        backlight_setup()
        clear()
        switch_img()
        draw_button_lall()
        while True:
            touched()
    except Exception as e:
        print('Error occured: ', e)
    except KeyboardInterrupt:
        print('Program Interrupted by the user')        
