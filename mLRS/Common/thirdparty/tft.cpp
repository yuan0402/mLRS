#include "tft.hpp"

// Global instance
TFT tft;

TFT::TFT() = default;
TFT::~TFT()
{
    delete _gfx;
    delete _bus;
}

bool TFT::begin(TFTModel model, uint8_t rotation, bool invert,
                int8_t sck, int8_t mosi, int8_t miso,
                int8_t cs, int8_t dc, int8_t rst, int8_t bl)
{
    _model = model;
    _blPin = bl;

    // Backlight
    if (_blPin >= 0)
    {
        pinMode(_blPin, OUTPUT);
        digitalWrite(_blPin, LOW);
    }

    // Create Bus + GFX by model
    switch (_model)
    {
    case TFTModel::ST7789_240x240:
        _width = 240;
        _height = 240;
        _bus = new Arduino_ESP32SPI(dc, cs, sck, mosi, miso, HSPI);
        _gfx = new Arduino_ST7789(_bus, rst, rotation, true /* IPS */);
        break;

    case TFTModel::ST7789_240x240_TFT:
        _width = 240;
        _height = 240;
        _bus = new Arduino_ESP32SPI(dc, cs, sck, mosi, miso, HSPI);
        _gfx = new Arduino_ST7789(_bus, rst, rotation, false /* non-IPS */);
        break;

    case TFTModel::ST7789_320x240:
        _width = 320;
        _height = 240;
        _bus = new Arduino_ESP32SPI(dc, cs, sck, mosi, miso, HSPI);
        _gfx = new Arduino_ST7789(_bus, rst, rotation, false);
        break;

    case TFTModel::ILI9341:
        _width = 320;
        _height = 240;
        _bus = new Arduino_ESP32SPI(dc, cs, sck, mosi, miso, HSPI);
        _gfx = new Arduino_ILI9341(_bus, rst, rotation);
        break;

    case TFTModel::ST7735_128x128:
        _width = 128;
        _height = 128;
        _bus = new Arduino_ESP32SPI(dc, cs, sck, mosi, miso, HSPI);
        _gfx = new Arduino_ST7735(_bus, rst, rotation);
        break;

    case TFTModel::ST7735_128x160:
        _width = 128;
        _height = 160;
        _bus = new Arduino_ESP32SPI(dc, cs, sck, mosi, miso, HSPI);
        _gfx = new Arduino_ST7735(_bus, rst, rotation);
        break;

    case TFTModel::ST7735_160x80:
    {
        // ELRS reference: panel native 80x160, rotation=1 maps to 160x80
        // ips=true, col_offset1/2=26, row_offset1/2=1
        _width = 80;
        _height = 160;
        _bus = new Arduino_ESP32SPI(dc, cs, sck, mosi, miso, HSPI);
        uint8_t rot = (rotation == 0) ? 1 : rotation;
        _gfx = new Arduino_ST7735(_bus, rst, rot, true, // ips
                                  80, 160,              // native panel w,h
                                  26, 1,                // col_offset1, row_offset1
                                  26, 1);               // col_offset2, row_offset2
        break;
    }

    case TFTModel::GC9A01:
        _width = 240;
        _height = 240;
        _bus = new Arduino_ESP32SPI(dc, cs, sck, mosi, miso, HSPI);
        _gfx = new Arduino_GC9A01(_bus, rst, rotation);
        break;

    default:
        return false;
    }

    // Init
    if (_gfx)
    {
        _gfx->begin();
        // Read back actual logical resolution after rotation
        _width = _gfx->width();
        _height = _gfx->height();
        if (invert)
            _gfx->invertDisplay(true);
        _initialized = true;
        return true;
    }
    return false;
}

void TFT::setRotation(uint8_t r)
{
    if (_gfx) {
        _gfx->setRotation(r);
        _width  = _gfx->width();
        _height = _gfx->height();
    }
}

// Backlight
void TFT::setBacklight(uint8_t brightness)
{
    if (_blPin >= 0)
        analogWrite(_blPin, brightness);
}

void TFT::backlightOn()
{
    if (_blPin >= 0)
        digitalWrite(_blPin, HIGH);
}

void TFT::backlightOff()
{
    if (_blPin >= 0)
        digitalWrite(_blPin, LOW);
}

// Drawing
void TFT::fillScreen(uint16_t color)
{
    if (_gfx)
        _gfx->fillScreen(color);
}
void TFT::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    if (_gfx)
        _gfx->drawPixel(x, y, color);
}

void TFT::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
    if (_gfx)
        _gfx->drawLine(x0, y0, x1, y1, color);
}

void TFT::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    if (_gfx)
        _gfx->drawRect(x, y, w, h, color);
}

void TFT::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
    if (_gfx)
        _gfx->fillRect(x, y, w, h, color);
}

void TFT::drawCircle(int16_t x, int16_t y, int16_t r, uint16_t color)
{
    if (_gfx)
        _gfx->drawCircle(x, y, r, color);
}

void TFT::fillCircle(int16_t x, int16_t y, int16_t r, uint16_t color)
{
    if (_gfx)
        _gfx->fillCircle(x, y, r, color);
}

void TFT::drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h,
                        int16_t r, uint16_t color)
{
    if (_gfx)
        _gfx->drawRoundRect(x, y, w, h, r, color);
}

void TFT::fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h,
                        int16_t r, uint16_t color)
{
    if (_gfx)
        _gfx->fillRoundRect(x, y, w, h, r, color);
}

void TFT::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                       int16_t x2, int16_t y2, uint16_t color)
{
    if (_gfx)
        _gfx->drawTriangle(x0, y0, x1, y1, x2, y2, color);
}

void TFT::fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                       int16_t x2, int16_t y2, uint16_t color)
{
    if (_gfx)
        _gfx->fillTriangle(x0, y0, x1, y1, x2, y2, color);
}

// Text
void TFT::setCursor(int16_t x, int16_t y)
{
    if (_gfx)
        _gfx->setCursor(x, y);
}
void TFT::setTextColor(uint16_t color)
{
    if (_gfx)
        _gfx->setTextColor(color);
}
void TFT::setTextColor(uint16_t fg, uint16_t bg)
{
    if (_gfx)
        _gfx->setTextColor(fg, bg);
}
void TFT::setTextSize(uint8_t size)
{
    if (_gfx)
        _gfx->setTextSize(size);
}
void TFT::setTextWrap(bool wrap)
{
    if (_gfx)
        _gfx->setTextWrap(wrap);
}

size_t TFT::print(const char *str) { return _gfx ? _gfx->print(str) : 0; }
size_t TFT::print(const String &str) { return _gfx ? _gfx->print(str) : 0; }
size_t TFT::println(const char *str) { return _gfx ? _gfx->println(str) : 0; }
size_t TFT::println(const String &str) { return _gfx ? _gfx->println(str) : 0; }

void TFT::_drawStringInternal(const char *str, int16_t x, int16_t y,
                              uint16_t color, bool centerX)
{
    if (!_gfx)
        return;
    _gfx->setTextColor(color);
    if (centerX)
    {
        int16_t x1, y1;
        uint16_t w, h;
        _gfx->getTextBounds(str, 0, 0, &x1, &y1, &w, &h);
        x = (_width - w) / 2;
    }
    _gfx->setCursor(x, y);
    _gfx->print(str);
}

void TFT::drawString(const char *str, int16_t x, int16_t y, uint16_t color)
{
    _drawStringInternal(str, x, y, color, false);
}

void TFT::drawCenterString(const char *str, int16_t y, uint16_t color)
{
    _drawStringInternal(str, 0, y, color, true);
}

// Utility
void TFT::drawTitle(const char *title, uint16_t color)
{
    drawCenterString(title, 0, color);
    drawLine(0, 20, _width - 1, 20, color);
}

void TFT::drawProgressBar(int16_t x, int16_t y, int16_t w, int16_t h,
                          uint8_t percent,
                          uint16_t barColor, uint16_t bgColor)
{
    if (!_gfx)
        return;
    if (percent > 100)
        percent = 100;
    _gfx->fillRect(x, y, w, h, bgColor);
    int16_t barW = (w * percent) / 100;
    if (barW > 0)
        _gfx->fillRect(x, y, barW, h, barColor);
    _gfx->drawRect(x, y, w, h, WHITE);
}

void TFT::showInfo(const char *line1, const char *line2, const char *line3)
{
    if (!_gfx)
        return;
    fillScreen(BLACK);
    drawCenterString(line1, 10, WHITE);
    if (line2)
        drawCenterString(line2, 40, WHITE);
    if (line3)
        drawCenterString(line3, 70, WHITE);
}

// Bitmaps
void TFT::drawBitmap(int16_t x, int16_t y, const uint8_t *data,
                     int16_t w, int16_t h, uint16_t color)
{
    if (_gfx)
        _gfx->drawBitmap(x, y, data, w, h, color);
}

void TFT::drawBitmap16(int16_t x, int16_t y, const uint16_t *data,
                       int16_t w, int16_t h)
{
    if (_gfx)
        _gfx->draw16bitRGBBitmap(x, y, data, w, h);
}

void TFT::drawBitmap24(int16_t x, int16_t y, const uint8_t *data,
                       int16_t w, int16_t h)
{
    if (_gfx)
        _gfx->draw24bitRGBBitmap(x, y, data, w, h);
}

void TFT::drawPageNotify(const char *s, const GFXfont *font,
                         int16_t y, uint16_t color, uint8_t shortLen)
{
    if (!_gfx || !s)
        return;

    fillScreen(BLACK);

    if (font)
        _gfx->setFont(font);

    uint8_t len = strlen(s);
    if (len < shortLen)
    {
        // Short text: center horizontally
        drawCenterString(s, y, color);
    }
    else
    {
        // Long text: left aligned
        setCursor(0, y);
        _gfx->print(s);
    }

    if (font)
        _gfx->setFont(); // restore default font
}

// Color conversion
uint16_t TFT::color565(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}