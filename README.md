---

# ESP32 Navi

*A recreation of the Pocket Navi from [Serial Experiments Lain](https://archive.org/details/serial-experiments-lain-english)*

---

## ✨ Overview

This is Version 2 of my Pocket Navi project. The original was built with MicroPython, but due to its limitations, I’ve rebuilt everything from scratch in C/C++ — which took quite a bit of time!

The base device is a CYD ESP32 board, which comes with a display and SD card slot. I highly recommend using the same model, available here:
👉 [Buy on AliExpress](https://www.aliexpress.com/item/1005004502250619.html) *(Note: get the one **without** USB-C!)*

I’ve also included the custom libraries I used — place them in the `libraries` folder of your Arduino setup. I recommend using these versions because I had issues with the default ones.

Hope you enjoy this! <3

---

## 🛠 Features

### Carried Over from v1

* Image display
* Logout screen
* Displays a few email messages from the anime

### New in v2

* Display turns off in logout mode to save power
* Sound playback support (WAV format)
* Much faster performance thanks to C++
* Now uses `.jpg` images instead of raw RGB565
* Integrated "Messenger" system (very basic)

---

## ⚙️ How to Use / Customize the Navi

I’ll be honest — I’m kind of burned out on this project, so it's a little rough around the edges. But if you're comfortable with Git and Arduino, you should be able to work it out!

---

### 🖼️ Images

The images that cycle when you press the image button are saved on the SD card as `.jpg` files.

You’ll need to manually add your filenames to this array in the code:

```cpp
String Images[] = {
  "/lain-bg.jpg",
  "/BG-Moon.jpg",
  "/EE.jpg",
  "/Lain.jpg"
};
```

---

### 🔊 Sound

This part’s a bit trickier. Sounds must be in `.wav` format and properly encoded. I recommend using [Audacity](https://www.audacityteam.org/) to prepare your audio files.

<img width="551" height="507" alt="image" src="https://github.com/user-attachments/assets/221698ce-00d9-458a-b263-d0caeda791ce" />

Each sound button corresponds to a function like this:

```cpp
void Sound1() {
  PrePlay("/musicbox.wav");
}
```

Just replace `"musicbox.wav"` with your file's name. Files are loaded from the SD card root (`/`).

**Note:** The button label won't change, as it's baked into the background image. You can change that by updating the corresponding image (e.g., `Sounds.jpg` in `fileNames[]`).

---

### 💬 Messenger Setup

The messenger feature is just a fun concept right now.

It works, but it’s **not secure** — messages are sent in **plain HTTP**, and anyone could potentially read them by guessing usernames. I'm working on updating the server and may release a desktop client too.

If you still want to try it (it’s pretty cool), here's how:

1. Make sure the `"WIFI"` flag is enabled (default: ON).
2. Set your Wi-Fi credentials and server address:

```cpp
const char* SERVER_URL = "http://YOUR.SERVER.IP/messages"; 

#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASS "YOUR_WIFI_PASSWORD"

IPAddress local_IP(192, 168, X, X);
IPAddress gateway(192, 168, X, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(8, 8, 8, 8);
```

3. On your ESP, just open Messenger and click Send or Receive.

> 💡 To type, just tap anywhere outside the on-screen keyboard!

---

## 🧱 STL / 3D Files

I’ve removed the STL files for now — they weren’t very good. I’ll re-upload improved versions once I’m happy with them.

---

## 🖥️ Server & Client

### 🌐 Server

The Messenger server is a simple Flask HTTP server.

Before running `app.py`, initialize the SQLite database:

```bash
python init_db.py
```

Then run:

```bash
python app.py
```

That’s it!

---

### 🧪 Client

Coming soon! I’ll release a basic CLI tool for testing and then build a full desktop app when I can. :3

---

## ❤️ Final Words

This project took a lot of time and energy, and I really hope you enjoy it. Whether you're using it as decoration, a conversation piece, or just for fun — have a great time with it.

Let’s all love Lain. <3

---

Let me know if you'd like a Markdown version you can copy-paste directly into your GitHub README.md file, or if you want help generating screenshots, diagrams, or 3D renders.
