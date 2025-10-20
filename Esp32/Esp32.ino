#include <TJpg_Decoder.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include "sdcard.h"
#include "driver/i2s.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define EXAMPLE_STD_DIN     I2S_GPIO_UNUSED
#define EXAMPLE_BUFF_SIZE   2048

#define CCCC(c1, c2, c3, c4) ((c4 << 24) | (c3 << 16) | (c2 << 8) | c1)

#define XPT2046_IRQ   36
#define XPT2046_MOSI  32
#define XPT2046_MISO  39
#define XPT2046_CLK   25
#define XPT2046_CS    33

//WIFI FLAG 
// Set to true if you have wifi and its set!! Also set the right ip!! 
// If you dont have or want wifi THEN SET THIS FLAG TO FALSE OR THE ESP WONT BOOT OR IT TAKES LONGER => you wont be able to use the Messager ofc

bool WIFI = true;


SPIClass touchscreenSPI(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);
TFT_eSPI tft = TFT_eSPI();

int vol_shift = 8;

File soundFile;

typedef enum headerState_e {
    HEADER_RIFF, HEADER_FMT, HEADER_DATA, DATA
} headerState_t;

typedef struct wavRiff_s {
    uint32_t chunkID;
    uint32_t chunkSize;
    uint32_t format;
} wavRiff_t;

typedef struct wavProperties_s {
    uint32_t chunkID;
    uint32_t chunkSize;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
} wavProperties_t;

headerState_t state = HEADER_RIFF;
wavProperties_t wavProps;

const char* SERVER_URL = "http://SERVERIP-HERE/messages"; 

#define WIFI_SSID "YOUR-WIFI-HERE"
#define WIFI_PASS "YOUR-PASSWORD-HERE"

IPAddress local_IP(XXX, XXX, XXX, XXX);
IPAddress gateway(XXX, XXX, XXX, XXX);
IPAddress subnet(XXX, XXX, XXX, XXX);
IPAddress dns(8, 8, 8, 8);

String message = "";  // Global input buffer
String To = "";
String From = "";
String Body = "";

const uint8_t DISPLAY_ORIENTATION_PORTRAIT = 1;

bool emailExitFlag = false;
bool soundExitFlag = false;
bool MessageExitFlag = false;

struct Point {
  int x;
  int y;
};

String fileNames[] = {
  "/V4.jpg",
  "/LogOut.jpg",
  "/Sounds.jpg"
};

String Images[] = {
  "/lain-bg.jpg",
  "/BG-Moon.jpg",
  "/EE.jpg",
  "/Lain.jpg"
};

String Email[] = {
  "/Email.jpg",
  "/Alice_MAil1.jpg",
};

String Messager[] = {
  "/To.jpg",
  "/From.jpg",
  "/Message.jpg",
  "/Receve.jpg",
  "/MessageMenu.jpg"
};

int ImageIndex = 0;

const int rawXMin = 200;
const int rawXMax = 3700;
const int rawYMin = 240;
const int rawYMax = 3800;

struct Button {
  String label;
  int x1;
  int y1;
  int x2;
  int y2;
};

// Buttons based on image coordinates (0,0 top-left)
Button buttons[] = {
  { "Images",  48,  30, 200, 83 }, // Your specified coords
  { "Sound",  65, 178, 164, 214 },
  { "Email",  192, 100, 317, 156 },
  { "Log Out", 252, 206, 320, 240 },
  { "Sound", 56, 160, 160, 210},
  { "Messager", 16, 109, 173 , 150}
};

Button MessagerB[] = {
  { "Send", 35, 24, 308, 105},
  { "Receive", 0, 137, 275, 209}
};

Button Mail[] = {
  { "Alice",  22, 145, 154, 172},
  { "Exit",  230, 0, 320, 55}
};

Button Sound[] = {
  { "Exit",  230, 0, 320, 40},
  { "Sound1",  93, 133, 202, 160},
  { "Sound2",  120, 187, 222, 215},
  { "Sound3",  234, 130, 320, 157},
  { "Sound4",  250, 165, 320, 195}
};

Button keyboard[] = {
  { "1", 2, 68, 30, 121 },
  { "2", 34, 68, 62, 121 },
  { "3", 66, 68, 94, 121 },
  { "4", 98, 68, 126, 121 },
  { "5", 130, 68, 158, 121 },
  { "6", 162, 68, 190, 121 },
  { "7", 194, 68, 222, 121 },
  { "8", 226, 68, 254, 121 },
  { "9", 258, 68, 286, 121 },
  { "0", 290, 68, 318, 121 },
  { "q", 2, 125, 30, 160 },
  { "w", 34, 125, 62, 160 },
  { "e", 66, 125, 94, 160 },
  { "r", 98, 125, 126, 160 },
  { "t", 130, 125, 158, 160 },
  { "y", 162, 125, 190, 160 },
  { "u", 194, 125, 222, 160 },
  { "i", 226, 125, 254, 160 },
  { "o", 258, 125, 286, 160 },
  { "p", 290, 125, 318, 160 },
  { "a", 2, 164, 30, 199 },
  { "s", 34, 164, 62, 199 },
  { "d", 66, 164, 94, 199 },
  { "f", 98, 164, 126, 199 },
  { "g", 130, 164, 158, 199 },
  { "h", 162, 164, 190, 199 },
  { "j", 194, 164, 222, 199 },
  { "k", 226, 164, 254, 199 },
  { "l", 258, 164, 286, 199 },
  { "point", 290, 164, 318, 199 },
  { "dpoint", 2, 203, 38, 238 },
  { "z", 42, 203, 70, 238 },
  { "x", 74, 203, 102, 238 },
  { "c", 106, 203, 134, 238 },
  { "v", 138, 203, 166, 238 },
  { "b", 170, 203, 198, 238 },
  { "n", 202, 203, 230, 238 },
  { "m", 234, 203, 262, 238 },
  { "space", 266, 203, 318, 238 },
  { "Exit", 0, 0, 320, 80}
};

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
  if (y >= tft.height()) return 0;
  tft.pushImage(x, y, w, h, bitmap);
  return 1;
}

void i2s_write_sample_nb(uint8_t sample) {
    uint16_t sample16 = (sample << vol_shift);
    size_t bytes_written;

    i2s_write(I2S_NUM_0, &sample16, 2, &bytes_written, portMAX_DELAY);
    i2s_write(I2S_NUM_0, &sample16, 2, &bytes_written, portMAX_DELAY);
}

void setup() {
  Serial.begin(115200);
  if(WIFI) {
    Serial.print("WIFI TURNED ON!!!");
    setupWiFi();
  } else {
    Serial.print("WIFI TURNED OFF!!!");
  }

  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touchscreen.begin(touchscreenSPI);
  touchscreen.setRotation(DISPLAY_ORIENTATION_PORTRAIT);

  tft.begin();
  tft.setRotation(DISPLAY_ORIENTATION_PORTRAIT);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true);

  sdcard_setup();
  i2sInit();

  TJpgDec.setJpgScale(1);
  TJpgDec.setCallback(tft_output);

  // Draw buttons as rectangles
  for (int i = 0; i < 4; i++) {
    tft.drawRect(buttons[i].x1, buttons[i].y1,
                 buttons[i].x2 - buttons[i].x1,
                 buttons[i].y2 - buttons[i].y1,
                 TFT_WHITE);
    tft.drawCentreString(buttons[i].label,
                         (buttons[i].x1 + buttons[i].x2) / 2,
                         (buttons[i].y1 + buttons[i].y2) / 2 - 8,
                         2);
  }

  TJpgDec.drawSdJpg(0, 0, fileNames[0]);
}

void setupWiFi() {

  Serial.printf("Connecting to %s...\n", WIFI_SSID);

  WiFi.disconnect(true);  // Clear previous config
  delay(1000);
  WiFi.mode(WIFI_STA);

  if (!WiFi.config(local_IP, gateway, subnet, dns)) {
    Serial.println("STA Failed to configure static IP");
  }

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 10) {
    Serial.print(".");
    delay(1000);
    retries++;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\n[ERROR] Wi-Fi connection failed.");
  } else {
    Serial.println("\n[OK] Wi-Fi connected.");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  }
}

Point touch() {
    TS_Point p = touchscreen.getPoint();
    int x = map(p.x, rawXMin, rawXMax, 0, 320);
    int y = map(p.y, rawYMin, rawYMax, 0, 240);
    x = constrain(x, 0, 320);
    y = constrain(y, 0, 240);
    Serial.printf("Touch mapped to image coordinates: X=%d, Y=%d\n", x, y);
    return {x, y};
}

void httprq() {
  Serial.print("Send HTTP");
  HTTPClient http;
  http.begin(SERVER_URL);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  String sender = From;
  String recipient = To;
  String message = Body;
  String postData = "sender=" + sender + "&recipient=" + recipient + "&message=" + message;

  int httpResponseCode = http.POST(postData);
  if (httpResponseCode > 0) {
    String response = http.getString();
    Serial.println("Response: " + response);
  } else {
    Serial.println("Error on sending POST: " + String(httpResponseCode));
  }
  http.end();
}

void displayImage(const String& imageName) {
  Serial.printf("Displaying image: %s\n", imageName.c_str());
  TJpgDec.drawSdJpg(0, 0, imageName);
}

/* Play WAV from SD card file || Sound cant be stop mid play TBD*/
void PlaySound(File &wavFile) {
    state = HEADER_RIFF;
    wavRiff_t wavRiff;
    uint32_t chunkId, chunkSize;
    uint8_t data;
    uint32_t dataBytesLeft = 0;   // declare here

    while (wavFile.available()) {
        switch (state) {
            case HEADER_RIFF:
                if (wavFile.read((uint8_t*)&wavRiff, sizeof(wavRiff_t)) != sizeof(wavRiff_t)) {
                    return; // error or EOF
                }
                if (wavRiff.chunkID == CCCC('R','I','F','F') && wavRiff.format == CCCC('W','A','V','E')) {
                    state = HEADER_FMT;
                } else {
                    return; // not a valid wav file
                }
                break;

            case HEADER_FMT:
                if (wavFile.read((uint8_t*)&wavProps, sizeof(wavProperties_t)) != sizeof(wavProperties_t)) {
                    return; // error or EOF
                }
                state = HEADER_DATA;
                Serial.printf("Channels: %d, Bits per sample: %d\n", wavProps.numChannels, wavProps.bitsPerSample);
                // Optionally set sample rate here if your i2s supports dynamic rate change
                break;

            case HEADER_DATA:
              while (true) {
                  if (wavFile.read((uint8_t*)&chunkId, 4) != 4) return;
                  if (wavFile.read((uint8_t*)&chunkSize, 4) != 4) return;

                  if (chunkId == CCCC('d','a','t','a')) {
                      state = DATA;
                      dataBytesLeft = chunkSize;
                      break;  // found data chunk
                  } else {
                      // skip this unknown chunk
                      // WAV chunks must align to even boundaries
                      uint32_t skipAmount = chunkSize + (chunkSize & 1);
                      wavFile.seek(wavFile.position() + skipAmount);

                  }
              }
              break;

            case DATA:
                while (dataBytesLeft > 0 && wavFile.available()) {
                    int readByte = wavFile.read();
                    if (readByte < 0) {  // real EOF or error
                        Serial.printf("File ended early, %u bytes left\n", dataBytesLeft);
                        return;
                    }
                    i2s_write_sample_nb((uint8_t)readByte);
                    dataBytesLeft--;
                }
            
                Serial.println("Playback done, chunk exhausted.");
                return;

        }
    }
}

void i2sInit(void) {
    i2s_config_t i2s_config = {
         .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
         .sample_rate = 11000,
         .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
         .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
         .communication_format = (i2s_comm_format_t)I2S_COMM_FORMAT_I2S_MSB,
         .intr_alloc_flags = 0,
         .dma_buf_count = 8,
         .dma_buf_len = 64,
         .use_apll = 1
    };
    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, NULL);
    i2s_set_sample_rates(I2S_NUM_0, 11000);
}

void PrePlay(const char* filename) {
  // Start I2S
  i2s_start(I2S_NUM_0);
  // Open the file
  File soundFile = SD.open(filename);
  if (soundFile) {
      PlaySound(soundFile);  // Your existing function to play sound
      soundFile.close();
  } else {
      Serial.println("File not found!");
      delay(100);
  }
  // Stop I2S
  i2s_stop(I2S_NUM_0);
}

void drawMessageArea(const String& msg) {
  // Clear only the message display rectangle to white background
  tft.fillRect(2, 24, 314, 52, TFT_WHITE);  // Width=316-2=314, Height=76-24=52
  
  // Set text color black on white background
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setTextSize(2);
  
  // Set cursor at starting x=2, y=24 to begin text inside the cleared rectangle
  tft.setCursor(2, 24);
  
  // Print the message string inside the defined box
  tft.print(msg);
}


void checkButtonPress(Point p, Button* btnArray, int btnCount) {
  bool isKeyboardKey = false;  // Flag to track if key is from keyboard

  for (int i = 0; i < btnCount; i++) {
    if (p.x >= btnArray[i].x1 && p.x <= btnArray[i].x2 &&
        p.y >= btnArray[i].y1 && p.y <= btnArray[i].y2) {
      Serial.printf("Button '%s' pressed at X=%d, Y=%d\n",
                    btnArray[i].label.c_str(), p.x, p.y);

      String label = btnArray[i].label;

      // Check if it is one of your keyboard keys by label set
      // (Adjust as needed if you want a more explicit keyboard array check)
      if (label.length() == 1 || label == "space" || label == "point" || label == "dpoint") {
        isKeyboardKey = true;
        if (label == "space") message += " ";
        else if (label == "point") message += ".";
        else if (label == "dpoint") message += ":";
        else message += label;
      } else {
        // System or special buttons handled here
        if (btnArray[i].label == "Log Out") handleLogout();
        if (btnArray[i].label == "Messager") handleMessager();
        if (btnArray[i].label == "Send") send_messages();
        if (btnArray[i].label == "Receive") receive_messages();
        if (btnArray[i].label == "Sound") handleSound();
        if (btnArray[i].label == "Images") handleImage();
        if (btnArray[i].label == "Email") handleEmail();
        if (btnArray[i].label == "Alice") EmailAlice();
        if (btnArray[i].label == "Exit") EmailExit();
        if (btnArray[i].label == "Exit") SoundExit();
        if (btnArray[i].label == "Exit") MessageExit();
        if (btnArray[i].label == "Sound1") Sound1();
        if (btnArray[i].label == "Sound2") Sound2();
        if (btnArray[i].label == "Sound3") Sound3();
        if (btnArray[i].label == "Sound4") Sound4();
      }

      // Only redraw message if it is a keyboard key
      if (isKeyboardKey) {
        drawMessageArea(message);
      }
      return;
    }
  }
}

void handleLogout() {
  int t = 0;  
  TJpgDec.drawSdJpg(0, 0, fileNames[1]); 
  while (true) {

    if (touchscreen.tirqTouched() && touchscreen.touched()) {

      tft.writecommand(0x11);  
      delay(120);
      tft.writecommand(0x29);  
      digitalWrite(TFT_BL, HIGH);
      TJpgDec.drawSdJpg(0, 0, fileNames[0]); 
      break;
    }

    delay(100); 

    if (t >= 150) { 
      tft.writecommand(0x28);
      tft.writecommand(0x10);  
      delay(120);
      digitalWrite(TFT_BL, LOW);
    } else {
      t++;
    }
  }
}

void handleImage(){
  TJpgDec.drawSdJpg(0, 0, Images[ImageIndex]);
  if(ImageIndex == 3){
    ImageIndex = 0;
  } else {
    ImageIndex++;
  }
  while(true){
    if (touchscreen.tirqTouched() && touchscreen.touched()) {
      TJpgDec.drawSdJpg(0, 0, fileNames[0]);
    break;
    }
  }
}

void handleSound(){
  soundExitFlag = false;
  TJpgDec.drawSdJpg(0, 0, fileNames[2]);
  while (!soundExitFlag) {
    if (touchscreen.tirqTouched() && touchscreen.touched()) {
      Point p = touch();
      checkButtonPress(p, Sound, sizeof(Sound) / sizeof(Sound[0]));
    }
  }
  TJpgDec.drawSdJpg(0, 0, fileNames[0]);
}

void Sound1(){
  PrePlay("/musicbox.wav");
}

void Sound2(){
  PrePlay("/nightfall.wav");
}

void Sound3(){
  PrePlay("/duvet.wav");
}

void Sound4(){
  PrePlay("/void.wav");
}

void SoundExit(){
  soundExitFlag = true;
}

void handleEmail(){
  emailExitFlag = false;
  TJpgDec.drawSdJpg(0, 0, Email[0]);
  while (!emailExitFlag) {
    if (touchscreen.tirqTouched() && touchscreen.touched()) {
      Point p = touch();
      checkButtonPress(p, Mail, sizeof(Mail) / sizeof(Mail[0]));
    }
  }
  TJpgDec.drawSdJpg(0, 0, fileNames[0]);
}

void EmailAlice(){
  TJpgDec.drawSdJpg(0, 0, Email[1]);
  while(true) {
    if (touchscreen.tirqTouched() && touchscreen.touched()) {
      TJpgDec.drawSdJpg(0, 0, Email[0]);
    break;
    }
  }
}

void EmailExit(){
  emailExitFlag = true;
}

void MessageExit(){
  MessageExitFlag = true;
}

void MessageTo(){
  MessageExitFlag = false;
  message = "";
    TJpgDec.drawSdJpg(0, 0, Messager[0]);
    delay(200);
    while (!MessageExitFlag) {
      if (touchscreen.tirqTouched() && touchscreen.touched()) {
        Point p = touch();
        //checkButtonPress(p, Mail, sizeof(Mail) / sizeof(Mail[0]));
        checkButtonPress(p, keyboard, sizeof(keyboard)/sizeof(Button));
        delay(150);
      }
    }
    To = message;
}

void MessageFrom(){
  MessageExitFlag = false;
  message = "";
  TJpgDec.drawSdJpg(0, 0, Messager[1]);
  delay(200);
  while (!MessageExitFlag) {
    if (touchscreen.tirqTouched() && touchscreen.touched()) {
      Point p = touch();
      //checkButtonPress(p, Mail, sizeof(Mail) / sizeof(Mail[0]));
      checkButtonPress(p, keyboard, sizeof(keyboard)/sizeof(Button));
      delay(150);
    }
  }
  From = message;
}

void MessageBody(){
  MessageExitFlag = false;
  message = "";
    TJpgDec.drawSdJpg(0, 0, Messager[2]);
    delay(200);
    while (!MessageExitFlag) {
      if (touchscreen.tirqTouched() && touchscreen.touched()) {
        Point p = touch();
        //checkButtonPress(p, Mail, sizeof(Mail) / sizeof(Mail[0]));
        checkButtonPress(p, keyboard, sizeof(keyboard)/sizeof(Button));
        delay(150);
      }
    }
    Body = message;
}

void handleMessager(){
  TJpgDec.drawSdJpg(0, 0, Messager[4]);
  while(true){
    if (touchscreen.tirqTouched() && touchscreen.touched()) {
      Point p = touch();
      //checkButtonPress(p, Mail, sizeof(Mail) / sizeof(Mail[0]));
      checkButtonPress(p, MessagerB, sizeof(keyboard)/sizeof(Button));
      break;
    }
  }
}

void send_messages(){
  MessageTo();
  MessageFrom();
  MessageBody();
  httprq();

  TJpgDec.drawSdJpg(0, 0, fileNames[0]);
}

String getUsernameFromKeyboard() {
    MessageExitFlag = false;      // reuse your exit flag
    message = "";
    TJpgDec.drawSdJpg(0, 0, Messager[1]); // Show "From" screen or custom
    delay(200);
    while (!MessageExitFlag) {
        if (touchscreen.tirqTouched() && touchscreen.touched()) {
            Point p = touch();
            checkButtonPress(p, keyboard, sizeof(keyboard)/sizeof(Button));
            delay(150);
        }
    }
    // Convert to lowercase (simple ASCII only!)
    message.toLowerCase();
    return message;
}

void receive_messages() {
    String username = getUsernameFromKeyboard();
    username.trim();                          // Cleans up whitespace
    // Draw message box
    TJpgDec.drawSdJpg(0, 0, Messager[3]);
    tft.fillRect(6, 26, 313-6, 234-26, TFT_WHITE);        // Fill box with white

    tft.setTextSize(1);                                    // Smallest font
    tft.setTextColor(TFT_BLACK, TFT_WHITE);                // Black text, white background

    int x0 = 8, y0 = 30, line_spacing = 12;                // Slight padding for neatness

    HTTPClient http;
    http.begin(SERVER_URL);
    int httpResponseCode = http.GET();
    if (httpResponseCode == 200) {
        String payload = http.getString();
        DynamicJsonDocument doc(8192);
        DeserializationError err = deserializeJson(doc, payload);

        if (err) {
            tft.setCursor(x0, y0);
            tft.setTextColor(TFT_RED, TFT_WHITE);
            tft.print("JSON parse err");
            Serial.println("JSON error:"); Serial.println(err.c_str());
            http.end();
            return;
        }

        JsonArray messages = doc.as<JsonArray>();
        bool found = false;
        int line = 0;

        for (JsonObject msg : messages) {
            String sender = msg["sender"].as<String>();
            String recipient = msg["recipient"].as<String>();
            String body = msg["message"].as<String>();
            String timestamp = msg["timestamp"].as<String>();

            sender.trim();      // Clean all fields
            recipient.trim();
            body.trim();

            if (sender.equalsIgnoreCase(username) || recipient.equalsIgnoreCase(username)) {
                found = true;
                String lineStr = timestamp + " " + sender + " -> " + recipient + ": " + body;
                tft.setCursor(x0, y0 + line * line_spacing);
                tft.print(lineStr);
                Serial.println(lineStr);
                line++;
                if ((y0 + line * line_spacing) > 234-12) break; // Avoid overflow
            }
        }

        if (!found) {
            tft.setCursor(x0, y0);
            tft.setTextColor(TFT_RED, TFT_WHITE);
            String out = "No messages for " + username;
            tft.print(out);
            Serial.println(out);
        }
    } else {
        tft.setTextColor(TFT_RED, TFT_WHITE);
        tft.setCursor(x0, y0);
        tft.print("Failed to get messages");
        Serial.print("Status: "); Serial.println(httpResponseCode);
    }
    http.end();

    while(true){
        if (touchscreen.tirqTouched() && touchscreen.touched()) {
          break;
      }
    }
    TJpgDec.drawSdJpg(0, 0, fileNames[0]);
}

void loop() {
  if (touchscreen.tirqTouched() && touchscreen.touched()) {
    Point p = touch();
    checkButtonPress(p, buttons, sizeof(buttons) / sizeof(buttons[0]));
  }
}

