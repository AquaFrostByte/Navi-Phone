# This file is executed on every boot (including wake-boot from deepsleep)
from machine import Pin, SPI, ADC, idle
import os
from time import sleep
from ili9341 import Display, color565
from xglcd_font import XglcdFont
import SD_CARD
import TOUCH_INPUT as TI
import Menu
import uos


def is_sd_mounted(mount_point="/sd"):
    try:
        # Attempt to list the contents of the mount point
        uos.listdir(mount_point)
        return True  # If successful, the SD card is mounted
    except OSError:
        return False  # If an error occurs, the SD card is not mounted

# Function to set up SPI for TFT display
display_spi = SPI(1, baudrate=60000000, sck=Pin(14), mosi=Pin(13))

# Set up display
display = Display(display_spi, dc=Pin(2), cs=Pin(15), rst=Pin(15),
                  width=320, height=240, rotation=270)

#color definitions
white_color = color565(255, 255, 255)  # white color
black_color = color565(0, 0, 0)        # black color

def start():
    #draws image to the screen has to be raw imamge!
    #				   {dir}		{x} {y} {w}  {h}   {w} => width {h} => {hight}   
    display.draw_image('sd/Start.raw', 0, 0, 320, 240)
    
    #print(img_val) 
def clear():
    # Clear display
    display.clear(black_color)
    
def backlight_setup():
    # Turn on display backlight
    backlight = Pin(21, Pin.OUT)
    backlight.on()
    
def touched():
    global button_show
    
    x, y = TI.touched()

    Menu.main_menu()
    
def boot_menu():
    try:
        if is_sd_mounted():
            print("mounted already")
        else:
            print("mounting")
            SD_CARD.start_SD()
        backlight_setup()
        clear()
        start()
        while True:
            touched()
    except Exception as e:
        print('Error occured: ', e)
    except KeyboardInterrupt:
        print('Program Interrupted by the user')
    
boot_menu()