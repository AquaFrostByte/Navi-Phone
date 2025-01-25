from machine import Pin, SPI, ADC, idle, SoftSPI
import os
from xpt2046 import Touch
from time import sleep
from ili9341 import Display, color565
from xglcd_font import XglcdFont

# Set up SPI for the touch controller
spi2 = SoftSPI(baudrate=1000000, polarity=0, phase=0,
               sck=Pin(25), mosi=Pin(32), miso=Pin(39))

# A global variable to store touch coordinates
touch_coords = None

# Define a function for the interrupt handler
def handle_touch(x, y):
    global touch_coords
    touch_coords = (x, y)

# Function to initialize touch and return coordinates
def touched():
    global touch_coords

    # Initialize the touch screen with the handler
    touch = Touch(spi2, cs=Pin(4), int_pin=Pin(36),
                  int_handler=handle_touch)

    # Wait for a touch and return coordinates
    while True:
        if touch_coords is not None:
            coords = touch_coords
            touch_coords = None  # Reset after handling
            return coords
        idle()  # Save CPU cycles while waiting

