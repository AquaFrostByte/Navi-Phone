# This file is executed on every boot (including wake-boot from deepsleep)
from machine import Pin, SPI, ADC, idle
import os
from time import sleep
from ili9341 import Display, color565
from xglcd_font import XglcdFont
import TOUCH_INPUT as TI
import main
import boot
import mail

# Function to set up SPI for TFT display
display_spi = SPI(1, baudrate=60000000, sck=Pin(14), mosi=Pin(13))

# Set up display
display = Display(display_spi, dc=Pin(2), cs=Pin(15), rst=Pin(15),
                  width=320, height=240, rotation=270)

#color definitions
white_color = color565(255, 255, 255)  # white color
black_color = color565(0, 0, 0)        # black color

def start():
    #draws image to the screen has to be raw image!
    #				   {dir}		{x} {y} {w}  {h}   {w} => width {h} => {hight}   
    display.draw_image('sd/Full_menu.raw', 0, 0, 320, 240)
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
    print(x, y)
    
    if x > 50 and x < 150 and y > 30 and y < 160:
        main.main()
        
    if y > 190 and x > 80 and x < 140:
        print("mail")
        mail.mail_menu()
    if x > 195 and y >200:
        print("Boot Menu")
        boot.boot_menu()
    
def main_menu():
    try:
        backlight_setup()
        clear()
        start()
        while True:
            touched()
            sleep(1)
    except Exception as e:
        print('Error occured: ', e)
    except KeyboardInterrupt:
        print('Program Interrupted by the user')

