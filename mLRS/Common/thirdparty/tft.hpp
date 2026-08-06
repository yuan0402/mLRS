#pragma once

#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// Default TFT pin definitions (override in HAL if needed)
#ifndef TFT_SCLK
#define TFT_SCLK 2      // SPI clock
#endif
#ifndef TFT_MOSI
#define TFT_MOSI 12      // SPI data out
#endif
#ifndef TFT_MISO
#define TFT_MISO -1      // SPI data in (unused = -1)
#endif
#ifndef TFT_CS
#define TFT_CS -1        // chip select (unused = -1, DC auto-control)
#endif
#ifndef TFT_DC
#define TFT_DC 14        // data/command select (required)
#endif
#ifndef TFT_RST
#define TFT_RST 33       // reset pin (-1 = unused)
#endif
#ifndef TFT_BL
#define TFT_BL 32        // backlight control (-1 = unused)
#endif

// Display model selection
enum class TFTModel : uint8_t
{
    ST7789_240x240,      // IPS
    ST7789_240x240_TFT,  // non-IPS
    ST7789_320x240,
    ILI9341,
    ST7735_128x128,
    ST7735_128x160,
    ST7735_160x80,
    GC9A01,              // round 240x240
};

#define TFT_DARKGRAY 0x8410  // dark gray, useful for progress bar background

// Wrapper class for SPI TFT displays using Arduino_GFX (Bus + GFX architecture).
// Supports ST7789, ILI9341, ST7735, GC9A01.
//
// Usage:
//   TFT tft;
//   tft.begin(TFTModel::ST7735_160x80, 0, true);
//   tft.fillScreen(BLACK);
//   tft.setTextSize(2);
//   tft.drawCenterString("Hello!", tft.height()/2, WHITE);
class TFT
{
public:
    TFT();
    ~TFT();

    // Initialize display.
    // model: display model enum
    // rotation: 0..3 (for ST7735_160x80, 0 auto-maps to 1 = landscape)
    // invert: swap black/white if colors are reversed
    // remaining params: pin numbers (defaults from macros above)
    bool begin(TFTModel model, uint8_t rotation = 0, bool invert = false,
               int8_t sck = TFT_SCLK, int8_t mosi = TFT_MOSI,
               int8_t miso = TFT_MISO, int8_t cs = TFT_CS,
               int8_t dc = TFT_DC, int8_t rst = TFT_RST,
               int8_t bl = TFT_BL);

    Arduino_GFX *gfz() { return _gfx; }  // raw underlying driver access

    int16_t width() const { return _width; }
    int16_t height() const { return _height; }
    void setRotation(uint8_t r);

    // Backlight: 0 = off, 255 = max
    void setBacklight(uint8_t brightness);
    void backlightOn();
    void backlightOff();

    // Drawing (all use RGB565 colors)
    void fillScreen(uint16_t color);
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void drawCircle(int16_t x, int16_t y, int16_t r, uint16_t color);
    void fillCircle(int16_t x, int16_t y, int16_t r, uint16_t color);
    void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h,
                        int16_t r, uint16_t color);
    void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h,
                        int16_t r, uint16_t color);
    void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                      int16_t x2, int16_t y2, uint16_t color);
    void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                      int16_t x2, int16_t y2, uint16_t color);

    // Text (font size affected by setTextSize)
    void setCursor(int16_t x, int16_t y);
    void setTextColor(uint16_t color);
    void setTextColor(uint16_t fg, uint16_t bg);
    void setTextSize(uint8_t size);   // scale 1~7 (base 5x7)
    void setTextWrap(bool wrap);
    size_t print(const char *str);
    size_t print(const String &str);
    size_t println(const char *str = "");
    size_t println(const String &str);
    void drawString(const char *str, int16_t x, int16_t y,
                    uint16_t color = WHITE);
    void drawCenterString(const char *str, int16_t y,
                          uint16_t color = WHITE);

    // Utility
    void drawTitle(const char *title, uint16_t color = WHITE);
    void drawProgressBar(int16_t x, int16_t y, int16_t w, int16_t h,
                         uint8_t percent,
                         uint16_t barColor = GREEN,
                         uint16_t bgColor = TFT_DARKGRAY);
    void drawPageNotify(const char *s,
                        const GFXfont *font = nullptr,
                        int16_t y = 27,
                        uint16_t color = WHITE,
                        uint8_t shortLen = 9);
    void showInfo(const char *line1, const char *line2 = nullptr,
                  const char *line3 = nullptr);

    // Bitmaps
    void drawBitmap(int16_t x, int16_t y, const uint8_t *data,
                    int16_t w, int16_t h, uint16_t color);
    void drawBitmap16(int16_t x, int16_t y, const uint16_t *data,
                      int16_t w, int16_t h);
    void drawBitmap24(int16_t x, int16_t y, const uint8_t *data,
                      int16_t w, int16_t h);

    // Color conversion: 8-bit RGB to RGB565
    static uint16_t color565(uint8_t r, uint8_t g, uint8_t b);

private:
    Arduino_DataBus *_bus = nullptr;
    Arduino_GFX *_gfx = nullptr;
    int16_t _width = 0;
    int16_t _height = 0;
    int8_t _blPin = -1;
    TFTModel _model;
    bool _initialized = false;

    void _drawStringInternal(const char *str, int16_t x, int16_t y,
                             uint16_t color, bool centerX);
};