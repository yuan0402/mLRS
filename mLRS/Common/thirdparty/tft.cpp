#include "tft.hpp"

// ============================================================
//  global instance
// ============================================================
TFT tft;

// ============================================================
//  构造 / 析构
// ============================================================
TFT::TFT() = default;
TFT::~TFT()
{
    delete _gfx;
    delete _bus;
}

// ============================================================
//  初始化
// ============================================================
bool TFT::begin(TFTModel model, uint8_t rotation, bool invert,
                int8_t sck, int8_t mosi, int8_t miso,
                int8_t cs, int8_t dc, int8_t rst, int8_t bl)
{
    _model = model;
    _blPin = bl;

    // ---------- 背光 ----------
    if (_blPin >= 0)
    {
        pinMode(_blPin, OUTPUT);
        digitalWrite(_blPin, LOW);
    }

    // ---------- 根据型号创建 Bus + GFX ----------
    switch (_model)
    {
    // ===== ST7789 =====
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
        _gfx = new Arduino_ST7789(_bus, rst, rotation, false /* 非 IPS */);
        break;

    case TFTModel::ST7789_320x240:
        _width = 320;
        _height = 240;
        _bus = new Arduino_ESP32SPI(dc, cs, sck, mosi, miso, HSPI);
        _gfx = new Arduino_ST7789(_bus, rst, rotation, false);
        break;

    // ===== ILI9341 =====
    case TFTModel::ILI9341:
        _width = 320;
        _height = 240;
        _bus = new Arduino_ESP32SPI(dc, cs, sck, mosi, miso, HSPI);
        _gfx = new Arduino_ILI9341(_bus, rst, rotation);
        break;

    // ===== ST7735 =====
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
        // ELRS 参考实现：面板原生 80x160，rotation=1 旋转后映射为 160x80
        // ips=true, col_offset1/2=26, row_offset1/2=1
        _width = 80;
        _height = 160;
        _bus = new Arduino_ESP32SPI(dc, cs, sck, mosi, miso, HSPI);
        // rotation 默认传 1（竖屏→横屏），与 ELRS 一致
        uint8_t rot = (rotation == 0) ? 1 : rotation;
        _gfx = new Arduino_ST7735(_bus, rst, rot, true, // ips=true
                                  80, 160,              // 面板原生宽高
                                  26, 1,                // col_offset1, row_offset1
                                  26, 1);               // col_offset2, row_offset2
        break;
    }

    // ===== GC9A01 =====
    case TFTModel::GC9A01:
        _width = 240;
        _height = 240;
        _bus = new Arduino_ESP32SPI(dc, cs, sck, mosi, miso, HSPI);
        _gfx = new Arduino_GC9A01(_bus, rst, rotation);
        break;

    default:
        return false;
    }

    // ---------- 初始化 ----------
    if (_gfx)
    {
        _gfx->begin();
        // 从驱动读取旋转后的实际逻辑分辨率
        _width = _gfx->width();
        _height = _gfx->height();
        if (invert)
            _gfx->invertDisplay(true);
        _initialized = true;
        return true;
    }
    return false;
}

// ============================================================
//  旋转
// ============================================================
void TFT::setRotation(uint8_t r)
{
    if (_gfx) {
        _gfx->setRotation(r);
        _width  = _gfx->width();
        _height = _gfx->height();
    }
}

// ============================================================
//  背光
// ============================================================
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

// ============================================================
//  绘图
// ============================================================
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

// ============================================================
//  文字
// ============================================================
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

// ============================================================
//  实用功能
// ============================================================
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

// ============================================================
//  位图
// ============================================================

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

    // 1. 清屏
    fillScreen(BLACK);

    // 2. 设置自定义字体（如果提供）
    if (font)
    {
        _gfx->setFont(font);
    }

    // 3. 根据文本长度选择居中或左对齐
    uint8_t len = strlen(s);
    if (len < shortLen)
    {
        // 短文本：水平居中
        drawCenterString(s, y, color);
    }
    else
    {
        // 长文本：左对齐
        setCursor(0, y);
        _gfx->print(s);
    }

    // 4. 恢复默认字体
    if (font)
    {
        _gfx->setFont(); // 无参 = 恢复默认字体
    }
}

// ============================================================
//  颜色工具
// ============================================================
uint16_t TFT::color565(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}