import machine
from machine import Pin
import vfs
import os

cs_sd = Pin(5, Pin.OUT)      # CS for SD card
cs_touch = Pin(4, Pin.OUT)   # CS for Touch controller

def start_SD():
    try:
        print("Initializing SD card...")
        cs_sd.value(0)  # Select SD card
        cs_touch.value(1)  # Deselect Touch controller

        # Initialize the SD card
        sd = machine.SDCard(slot=2, sck=Pin(18), mosi=Pin(23), miso=Pin(19), cs=Pin(5))
        
        # Mount the SD card
        vfs.mount(sd, '/sd')
        print("SD card mounted successfully.")
        print("Contents of SD card:", os.listdir('/sd'))

    except Exception as e:
        print(f"Failed to initialize SD card: {e}")