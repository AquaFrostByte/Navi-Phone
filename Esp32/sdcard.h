#include "pins_arduino.h"
/*
 * pin 1 - not used          |  Micro SD card     |
 * pin 2 - CS (SS)           |                   /
 * pin 3 - DI (MOSI)         |                  |__
 * pin 4 - VDD (3.3V)        |                    |
 * pin 5 - SCK (SCLK)        | 8 7 6 5 4 3 2 1   /
 * pin 6 - VSS (GND)         | ▄ ▄ ▄ ▄ ▄ ▄ ▄ ▄  /
 * pin 7 - DO (MISO)         | ▀ ▀ █ ▀ █ ▀ ▀ ▀ |
 * pin 8 - not used          |_________________|
 *                             ║ ║ ║ ║ ║ ║ ║ ║
 *                     ╔═══════╝ ║ ║ ║ ║ ║ ║ ╚═════════╗
 *                     ║         ║ ║ ║ ║ ║ ╚══════╗    ║
 *                     ║   ╔═════╝ ║ ║ ║ ╚═════╗  ║    ║
 * Connections for     ║   ║   ╔═══╩═║═║═══╗   ║  ║    ║
 * full-sized          ║   ║   ║   ╔═╝ ║   ║   ║  ║    ║
 * SD card             ║   ║   ║   ║   ║   ║   ║  ║    ║
 * Pin name         |  -  DO  VSS SCK VDD VSS DI CS    -  |
 * SD pin number    |  8   7   6   5   4   3   2   1   9 /
 *                  |                                  █/
 *                  |__▍___▊___█___█___█___█___█___█___/
 *
 * Note:  The SPI pins can be manually configured by using `SPI.begin(sck, miso, mosi, cs).`
 *        Alternatively, you can change the CS pin and use the other default settings by using `SD.begin(cs)`.
 *
 * +--------------+---------+-------+----------+----------+----------+----------+----------+
 * | SPI Pin Name | ESP8266 | ESP32 | ESP32‑S2 | ESP32‑S3 | ESP32‑C3 | ESP32‑C6 | ESP32‑H2 |
 * +==============+=========+=======+==========+==========+==========+==========+==========+
 * | CS (SS)      | GPIO15  | GPIO5 | GPIO34   | GPIO10   | GPIO7    | GPIO18   | GPIO0    |
 * +--------------+---------+-------+----------+----------+----------+----------+----------+
 * | DI (MOSI)    | GPIO13  | GPIO23| GPIO35   | GPIO11   | GPIO6    | GPIO19   | GPIO25   |
 * +--------------+---------+-------+----------+----------+----------+----------+----------+
 * | DO (MISO)    | GPIO12  | GPIO19| GPIO37   | GPIO13   | GPIO5    | GPIO20   | GPIO11   |
 * +--------------+---------+-------+----------+----------+----------+----------+----------+
 * | SCK (SCLK)   | GPIO14  | GPIO18| GPIO36   | GPIO12   | GPIO4    | GPIO21   | GPIO10   |
 * +--------------+---------+-------+----------+----------+----------+----------+----------+
 *
 * For more info see file README.md in this library or on URL:
 * https://github.com/espressif/arduino-esp32/tree/master/libraries/SD
 */

#include "FS.h"
#include "SD.h"
#include "SPI.h"

/* Uncomment and set up if you want to use custom pins for the SPI communication
#define REASSIGN_PINS

// https://github.com/espressif/arduino-esp32/blob/master/variants/jczn_2432s028r/pins_arduino.h#L70C9-L70C28
int sck   = CYD_SD_SCK;   // 18
int miso  = CYD_SD_MISO;  // 19
int mosi  = CYD_SD_MOSI;  // 23
int cs    = CYD_SD_SS;    //  5
//*/

void listDir(fs::FS &fs, const char *dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.path(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }

    file = root.openNextFile();
  }
}

void createDir(fs::FS &fs, const char *path) {
  Serial.printf("Creating Dir: %s\n", path);
  if (fs.mkdir(path)) {
    Serial.println("Dir created");
  } else {
    Serial.println("mkdir failed");
  }
}

void removeDir(fs::FS &fs, const char *path) {
  Serial.printf("Removing Dir: %s\n", path);
  if (fs.rmdir(path)) {
    Serial.println("Dir removed");
  } else {
    Serial.println("rmdir failed");
  }
}

void readFile(fs::FS &fs, const char *path) {
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}

void writeFile(fs::FS &fs, const char *path, const char *message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char *path, const char *message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void renameFile(fs::FS &fs, const char *path1, const char *path2) {
  Serial.printf("Renaming file %s to %s\n", path1, path2);
  if (fs.rename(path1, path2)) {
    Serial.println("File renamed");
  } else {
    Serial.println("Rename failed");
  }
}

void deleteFile(fs::FS &fs, const char *path) {
  Serial.printf("Deleting file: %s\n", path);
  if (fs.remove(path)) {
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }
}

void testFileIO(fs::FS &fs, const char *path) {
  File file = fs.open(path);
  static uint8_t buf[512];
  size_t len = 0;
  uint32_t start = millis();
  uint32_t end = start;
  if (file) {
    len = file.size();
    size_t flen = len;
    start = millis();
    while (len) {
      size_t toRead = len;
      if (toRead > 512) {
        toRead = 512;
      }
      file.read(buf, toRead);
      len -= toRead;
    }
    end = millis() - start;
    Serial.printf("%u bytes read for %lu ms\n", flen, end);
    file.close();
  } else {
    Serial.println("Failed to open file for reading");
  }

  file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }

  size_t i;
  start = millis();
  for (i = 0; i < 2048; i++) {
    file.write(buf, 512);
  }
  end = millis() - start;
  Serial.printf("%u bytes written for %lu ms\n", 2048 * 512, end);
  file.close();
}

/*--------------------------------------------------------------------------------
 * Setup SD card (Mounting fails if card is not inserted)
 * Note the global variable 'SPI' is assigned to 'VSPI' same as 'CYD_SD_SPI_BUS'.
 * https://github.com/espressif/arduino-esp32/blob/master/libraries/SPI/src/SPI.cpp#L333-L337
 *--------------------------------------------------------------------------------*/
void sdcard_setup() {

#if   false

#ifdef REASSIGN_PINS
  SPI.begin(sck, miso, mosi, cs);

  if (!SD.begin(cs)) {
#else
  // this also works since CYD_SD_* are assigned to the default spi pins.
  if (!SD.begin()) {
#endif
    Serial.println("Card Mount Failed");
    return;
  }

#elif 0

  // this also works since the SD card is connected to the default SPI bus, VSPI
  // and when display and touch panel are connected to HSPI
  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    return;
  }

#elif 1

#ifndef CYD_SD_SPI_BUS
#define CYD_SD_SCK  SCK
#define CYD_SD_MISO MISO
#define CYD_SD_MOSI MOSI
#define CYD_SD_SS   SS
#endif

  // this also works since the SD card is connected to the SPI bus, HSPI
  // and when display and touch panel are connected to VSPI
  static SPIClass spi = SPIClass(HSPI /*CYD_SD_SPI_BUS*/); // VSPI
  spi.begin(CYD_SD_SCK, CYD_SD_MISO, CYD_SD_MOSI, CYD_SD_SS);

  if (!SD.begin(CYD_SD_SS, spi, 50000000)) {
    Serial.println("Card Mount Failed");
    return;
  }

#else

// this does not work because SD has different pins
#define SD_CONFIG SS, tft.getSPIinstance(), 20000000

  if (!SD.begin(SD_CONFIG)) {
    Serial.println("Card Mount Failed");
    return;
  }

#endif

  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

#if   false
  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }
#endif
}

/*--------------------------------------------------------------------------------
 * Test basic functions
 *--------------------------------------------------------------------------------*/
void sdcard_test(void) {
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  listDir(SD, "/", 0);
  createDir(SD, "/mydir");
  listDir(SD, "/", 0);
  removeDir(SD, "/mydir");
  listDir(SD, "/", 2);
  writeFile(SD, "/hello.txt", "Hello ");
  appendFile(SD, "/hello.txt", "World!\n");
  readFile(SD, "/hello.txt");
  deleteFile(SD, "/foo.txt");
  renameFile(SD, "/hello.txt", "/foo.txt");
  readFile(SD, "/foo.txt");
  testFileIO(SD, "/test.txt");
  Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
}

/*--------------------------------------------------------------------------------
 * Convert between RGB565 and RGB888
 *--------------------------------------------------------------------------------*/
#define RGB_SWAP(t, a, b) {t tmp = a; a = b; b = tmp;}
#define RGB565(r, g, b)   ((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3))

inline void color565toRGB(uint16_t color, uint8_t &r, uint8_t &g, uint8_t &b) __attribute__((always_inline));
inline void color565toRGB(uint16_t color, uint8_t &r, uint8_t &g, uint8_t &b) {
  r = (color>>8)&0x00F8;
  g = (color>>3)&0x00FC;
  b = (color<<3)&0x00F8;
}

/*--------------------------------------------------------------------------------
 * Correct color shift issue by TFT_eSPI on ESP32 2432S028R
 *--------------------------------------------------------------------------------*/
#if defined (_TFT_eSPIH_) && (DISPLAY_CYD_2USB == true)
#define RGB_CORRECTION    (1)
#else
#define RGB_CORRECTION    (0)
#endif



/*--------------------------------------------------------------------------------
 * LCD screen capture to save image to SD card
 *--------------------------------------------------------------------------------*/
/*
// Graphics library
#if   defined (_TFT_eSPIH_)
#define GFX_TYPE  TFT_eSPI
#elif defined (LOVYANGFX_HPP_)
#define GFX_TYPE  LGFX
#else
//#error Support only for TFT_eSPI and LovyanGFX
//Serial.println("Support only for TFT_eSPI and LovyanGFX");
#endif

bool SaveBMP24(fs::FS &fs, const char *path, GFX_TYPE &tft) {
  uint32_t start = millis();

  uint32_t w = tft.width();
  uint32_t h = tft.height();

#if   defined (LOVYANGFX_HPP_)

  lgfx::rgb888_t rgb[w];

#elif defined (_TFT_eSPIH_)

  uint8_t rgb[w * 3];

#endif // LovyanGFX or TFT_eSPI

  File file = fs.open(path, FILE_WRITE);

  if (!file) {
    Serial.printf("SD: open %s failed.\n", path);
    return false;
  } else {
    Serial.printf("saving %s\n", path);
  }

  unsigned char bmFlHdr[14] = {
    'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0
  };

  // set color depth to 24 as we're storing 8 bits for r-g-b
  unsigned char bmInHdr[40] = {
    40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0,
  };

  unsigned long fileSize = 3ul * h * w + 54;

  bmFlHdr[ 2] = (unsigned char)(fileSize);
  bmFlHdr[ 3] = (unsigned char)(fileSize >> 8);
  bmFlHdr[ 4] = (unsigned char)(fileSize >> 16);
  bmFlHdr[ 5] = (unsigned char)(fileSize >> 24);

  bmInHdr[ 4] = (unsigned char)(w);
  bmInHdr[ 5] = (unsigned char)(w >> 8);
  bmInHdr[ 6] = (unsigned char)(w >> 16);
  bmInHdr[ 7] = (unsigned char)(w >> 24);
  bmInHdr[ 8] = (unsigned char)(h);
  bmInHdr[ 9] = (unsigned char)(h >> 8);
  bmInHdr[10] = (unsigned char)(h >> 16);
  bmInHdr[11] = (unsigned char)(h >> 24);

  file.write(bmFlHdr, sizeof(bmFlHdr));
  file.write(bmInHdr, sizeof(bmInHdr));

  for (int y = h - 1; y >= 0; --y) {
    if (y % 10 == 0) {
      Serial.printf(".");
      yield(); // Prevent the watchdog from firing in core 0
    }

#if defined (LOVYANGFX_HPP_)

    tft.readRect(0, y, w, 1, rgb);

  #if RGB_CORRECTION
    for (int i = 0; i < w; ++i) {
      rgb[i].r <<= RGB_CORRECTION;
      rgb[i].g <<= RGB_CORRECTION;
      rgb[i].b <<= RGB_CORRECTION;
    }
  #endif

#else // TFT_eSPI

    tft.readRectRGB(0, y, w, 1, (uint8_t*)rgb);

    for (int i = 0; i < sizeof(rgb); i += 3) {
  #if defined (TFT_RGB_ORDER) && (TFT_RGB_ORDER == TFT_BGR)
      RGB_SWAP(uint8_t, rgb[i+1], rgb[i+2]);
  #else
      RGB_SWAP(uint8_t, rgb[i+0], rgb[i+2]);
  #endif

  #if RGB_CORRECTION
      rgb[i  ] <<= RGB_CORRECTION;
      rgb[i+1] <<= RGB_CORRECTION;
      rgb[i+2] <<= RGB_CORRECTION;
  #endif
    }

#endif // LovyanGFX or TFT_eSPI

    int len = file.write((uint8_t*)rgb, sizeof(rgb));

    if (file.getWriteError() != 0) {
      Serial.printf("SD write error: len: %d\n", len); // getWriteError() returns 1
      file.close();
      return false;
    };
  }

  file.close();
  start = millis() - start;
  Serial.printf("done (%d msec).\n", start);
//listDir(SD, "/", 0);
  return true;
}
*/