# ESP32 Navi

*A recreation of the Pocket Navi from [Serial Experiments Lain](https://archive.org/details/serial-experiments-lain-english)*

## overview

This is the second version of my Pocket Navi project. I built the original with MicroPython, but its limitations pushed me to rebuild everything from scratch in C/C++. It took a while.

The device is built on a CYD ESP32 board, which conveniently includes a display and an SD card slot. I strongly recommend using the same model to avoid potential headaches. You can get it on AliExpress:

[Buy on AliExpress](https://www.aliexpress.com/item/1005004502250619.html)
*(Important: Get the version **without** the USB-C port.)*

The custom libraries I used are not included in this repository, but you can download them [here](https://archive.org/details/tft-e-spi/Librarys/XPT2046_Touchscreen/doc/ILI9431Test.jpg). Just drop them into the `libraries` folder of your Arduino installation. I suggest using these specific versions, as I ran into problems with the standard ones.

Hope you enjoy it.

## features

### carried over from v1

*   Image display
*   Logout screen
*   Displays a few email messages from the anime

### new in v2

*   Display now turns off in logout mode to save power
*   Sound playback support (WAV format)
*   Much faster performance, thanks to the C++ rewrite
*   Uses standard `.jpg` images instead of raw RGB565 files
*   A basic, integrated "Messenger" system

## how to use / customize the navi

I'll be honest, I'm a little burned out on this project, so some parts are rough around the edges. If you're comfortable with Git and Arduino, you should be able to figure it out.

### images

The images that cycle on the home screen are loaded from the SD card. They must be `.jpg` files.

You'll need to add your filenames to this array in the code:

```cpp
String Images[] = {
  "/lain-bg.jpg",
  "/BG-Moon.jpg",
  "/EE.jpg",
  "/Lain.jpg"
};
```

### sound

This part is a bit trickier. Sounds must be in `.wav` format and encoded correctly. I used [Audacity](https://www.audacityteam.org/) to prepare the audio files.

<img width="551" height="507" alt="Audacity export settings for WAV" src="https://github.com/user-attachments/assets/221698ce-00d9-458a-b263-d0caeda791ce" />

Each sound button calls a function in the code. To change a sound, find its corresponding function:

```cpp
void Sound1() {
  PrePlay("/musicbox.wav");
}
```

Just replace `"musicbox.wav"` with the name of your file. All sound files should be in the root directory of the SD card.

**Note:** The button labels on the screen are part of the background image (`Sounds.jpg`), so they won't update automatically. You'll have to edit the image itself if you want the text to match your new sound.

---

### messenger setup

The messenger is more of a proof-of-concept right now.

A word of warning: **it is not secure**. Messages are sent over plain HTTP without any encryption. Anyone on the network could potentially read them. I'm working on an update for the server and might release a desktop client later.

If you still want to try it out, here's how:

1.  Make sure the `WIFI` flag is enabled in the code (it is by default).
2.  Set your Wi-Fi credentials and server address:

```cpp
const char* SERVER_URL = "http://YOUR.SERVER.IP/messages"; 

#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASS "YOUR_WIFI_PASSWORD"

IPAddress local_IP(192, 168, X, X);
IPAddress gateway(192, 168, X, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(8, 8, 8, 8);
```

3.  On the ESP32, open the Messenger and use the Send or Receive buttons.

> **Note:** To type, tap anywhere on the screen outside of the on-screen keyboard area.

## stl / 3d files

I've removed the STL files for now. Frankly, they weren't very good. I'll re-upload them once I've designed a version I'm happy with.

## server & client

### server

The server for the messenger is a simple Flask application.

Before running `app.py`, you need to initialize the SQLite database:
```bash
python init_db.py
```

Then, start the server:
```bash
python app.py
```
That's it.

### client

Coming soon. I plan to release a simple CLI tool for testing first, and then work on a proper desktop app when I find the time. :3

## final words

This project took a lot of my time and energy. I really hope you have fun with it, whether you use it as a decoration, a conversation piece, or something else entirely.

Let’s all love Lain. <3
