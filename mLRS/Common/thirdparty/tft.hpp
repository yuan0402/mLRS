#pragma once

#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// ============================================================
//  TFT 引脚定义（默认值，若 HAL 文件已定义则优先使用）
// ============================================================
#ifndef TFT_SCLK
#define TFT_SCLK 2  // SPI 时钟
#endif
#ifndef TFT_MOSI
#define TFT_MOSI 12 // SPI 数据输出 (Master Out Slave In)
#endif
#ifndef TFT_MISO
#define TFT_MISO -1 // SPI 数据输入（不接 = -1）
#endif
#ifndef TFT_CS
#define TFT_CS -1   // 片选（不接 = -1，由 DC 脚自动控制）
#endif
#ifndef TFT_DC
#define TFT_DC 14   // Data/Command 选择（必须接）
#endif
#ifndef TFT_RST
#define TFT_RST 33  // Reset 复位（-1 表示不接）
#endif
#ifndef TFT_BL
#define TFT_BL 32   // 背光控制（-1 表示不接）
#endif

/**
 * @brief 显示屏型号枚举
 *
 * 用法：tft.begin(TFTModel::ST7735_160x80);
 */
enum class TFTModel : uint8_t
{
    ST7789_240x240,     // ST7789 方形屏 240x240 (IPS 面板)
    ST7789_240x240_TFT, // ST7789 方形屏 240x240 (非 IPS 面板)
    ST7789_320x240,     // ST7789 宽屏 320x240
    ILI9341,            // ILI9341 320x240
    ST7735_128x128,     // ST7735 128x128 小方屏
    ST7735_128x160,     // ST7735 128x160 竖屏
    ST7735_160x80,      // ST7735 160x80 横屏
    GC9A01,             // GC9A01 240x240 圆形屏
};

// 补充颜色（Arduino_GFX 已提供 BLACK/WHITE/RED/GREEN/BLUE/CYAN/MAGENTA/YELLOW/ORANGE 等宏）
#define TFT_DARKGRAY 0x8410 // 深灰色，常用于进度条背景

/**
 * @brief TFT 显示屏封装类
 *
 * 基于 moononournation/Arduino_GFX 库，采用 Bus + GFX 分层架构。
 * 支持 ST7789、ILI9341、ST7735、GC9A01 等常见 SPI 屏幕。
 *
 * 基本用法:
 * @code
 *   TFT tft;                                         // 1. 创建对象
 *   void setup() {
 *       tft.begin(TFTModel::ST7735_160x80, 0, true); // 2. 初始化（型号,旋转,invert）
 *       tft.fillScreen(BLACK);                       // 3. 清屏
 *       tft.setTextSize(2);                          // 4. 设置字号
 *       tft.drawCenterString("Hello!", tft.height()/2, WHITE); // 5. 居中显示
 *   }
 * @endcode
 */
class TFT
{
public:
    // ================================================================
    //  构造 / 析构
    // ================================================================

    TFT();  // 构造函数，创建 TFT 对象
    ~TFT(); // 析构函数，释放资源

    // ================================================================
    //  初始化
    // ================================================================

    /**
     * @brief  初始化显示屏
     * @param  model     屏幕型号：如 TFTModel::ST7735_160x80
     * @param  rotation  旋转角度：0=不转, 1=90°, 2=180°, 3=270°
     *                   对于 ST7735_160x80，rotation=0 自动设为 1（横屏）
     * @param  invert    颜色反转：true=反转, false=不反转
     *                   如果黑白色反了，调这个参数
     * @param  sck/mosi/miso/cs/dc/rst/bl 引脚号，默认用头文件顶部的宏
     * @return true 成功, false 失败
     *
     * 用法：
     *   tft.begin(TFTModel::ST7735_160x80);               // 默认参数
     *   tft.begin(TFTModel::ST7735_160x80, 0, true);      // 完整调用
     *   tft.begin(TFTModel::ST7735_160x80, 0, true, 6,7,2,4,5,10,8);  // 自定义引脚
     */
    bool begin(TFTModel model, uint8_t rotation = 0, bool invert = false,
               int8_t sck = TFT_SCLK, int8_t mosi = TFT_MOSI,
               int8_t miso = TFT_MISO, int8_t cs = TFT_CS,
               int8_t dc = TFT_DC, int8_t rst = TFT_RST,
               int8_t bl = TFT_BL);

    /// 获取底层 Arduino_GFX 指针，用于调用原始 API（高级用户）
    Arduino_GFX *gfz() { return _gfx; }

    // ================================================================
    //  基本信息
    // ================================================================

    int16_t width() const { return _width; }   // 返回屏幕逻辑宽度（像素）
    int16_t height() const { return _height; } // 返回屏幕逻辑高度（像素）

    /**
     * @brief  设置屏幕旋转
     * @param  r  0=正常, 1=90°, 2=180°, 3=270°
     *
     * 用法：tft.setRotation(2);  // 翻转180度
     */
    void setRotation(uint8_t r);

    // ================================================================
    //  背光控制
    // ================================================================

    /// 设置背光亮度：0=全暗, 255=最亮
    /// 用法：tft.setBacklight(128);  // 半亮
    void setBacklight(uint8_t brightness);

    /// 打开背光（最亮）
    /// 用法：tft.backlightOn();
    void backlightOn();

    /// 关闭背光（全暗，屏幕黑但内容还在）
    /// 用法：tft.backlightOff();
    void backlightOff();

    // ================================================================
    //  绘图（全部用 RGB565 颜色，直接用 BLACK/WHITE/RED/... 宏）
    // ================================================================

    /**
     * @brief  全屏填充单一颜色
     * 用法：tft.fillScreen(BLACK);   // 全屏变黑（清屏）
     *       tft.fillScreen(RED);     // 全屏变红
     */
    void fillScreen(uint16_t color);

    /**
     * @brief  画一个像素点
     * 用法：tft.drawPixel(80, 40, RED);  // 在坐标(80,40)画一个红点
     */
    void drawPixel(int16_t x, int16_t y, uint16_t color);

    /**
     * @brief  画线段
     * 用法：tft.drawLine(0, 0, 159, 79, WHITE);  // 画对角线
     */
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);

    /**
     * @brief  画空心矩形
     * 用法：tft.drawRect(10, 10, 100, 50, GREEN);  // 左上(10,10), 宽100高50, 绿色框
     */
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

    /**
     * @brief  画实心矩形
     * 用法：tft.fillRect(20, 30, 60, 40, BLUE);  // 蓝色实心方块
     */
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

    /**
     * @brief  画空心圆
     * 用法：tft.drawCircle(80, 40, 20, YELLOW);  // 圆心(80,40), 半径20, 黄色圆环
     */
    void drawCircle(int16_t x, int16_t y, int16_t r, uint16_t color);

    /**
     * @brief  画实心圆
     * 用法：tft.fillCircle(80, 40, 15, CYAN);  // 实心青色圆
     */
    void fillCircle(int16_t x, int16_t y, int16_t r, uint16_t color);

    /**
     * @brief  画空心圆角矩形
     * 用法：tft.drawRoundRect(5, 5, 150, 70, 10, WHITE);
     *       // 左上(5,5), 宽150高70, 圆角半径10, 白框
     */
    void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h,
                       int16_t r, uint16_t color);

    /**
     * @brief  画实心圆角矩形
     * 用法：tft.fillRoundRect(5, 5, 150, 70, 10, MAGENTA);
     */
    void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h,
                       int16_t r, uint16_t color);

    /**
     * @brief  画空心三角形
     * 用法：tft.drawTriangle(0,79, 80,0, 159,79, GREEN);
     *       // 三个顶点分别是 左下、顶部中间、右下
     */
    void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                      int16_t x2, int16_t y2, uint16_t color);

    /**
     * @brief  画实心三角形
     * 用法：tft.fillTriangle(80,0, 0,79, 159,79, RED);
     */
    void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                      int16_t x2, int16_t y2, uint16_t color);

    // ================================================================
    //  文字（字体会受 setTextSize 影响）
    // ================================================================

    /**
     * @brief  设置光标位置（print/println 从这里开始写）
     * 用法：tft.setCursor(10, 20);  // 光标移到 (10,20)
     */
    void setCursor(int16_t x, int16_t y);

    /**
     * @brief  设置文字前景色
     * 用法：tft.setTextColor(YELLOW);  // 后续文字用黄色
     */
    void setTextColor(uint16_t color);

    /**
     * @brief  设置文字前景色 + 背景色
     * 用法：tft.setTextColor(WHITE, BLUE);  // 白字蓝底
     */
    void setTextColor(uint16_t fg, uint16_t bg);

    /**
     * @brief  设置文字缩放倍数（1~7，默认 1）
     * 用法：tft.setTextSize(2);  // 放大到 2 倍
     *
     * 字号对照（默认 5×7 基础）：
     *   1 = 5×7     2 = 10×14    3 = 15×21
     *   你的屏幕高 80 像素，字号 2 时每行约 14 像素，可显示 ~5 行
     */
    void setTextSize(uint8_t size);

    /**
     * @brief  设置是否自动换行
     * 用法：tft.setTextWrap(true);  // 文字超宽自动换到下一行
     *       tft.setTextWrap(false); // 超宽截断（默认）
     */
    void setTextWrap(bool wrap);

    /**
     * @brief  在光标位置打印字符串（需要先用 setCursor 定位）
     * 用法：
     *   tft.setCursor(10, 5);
     *   tft.setTextColor(WHITE);
     *   tft.print("Hello");   // 在 (10,5) 开始写 "Hello"
     */
    size_t print(const char *str);
    size_t print(const String &str);

    /**
     * @brief  在光标位置打印并换行
     * 用法：
     *   tft.setCursor(10, 5);
     *   tft.println("Line1");  // 写 "Line1" 后换行，光标移到下一行开头
     *   tft.println("Line2");  // 在下一行接着写
     */
    size_t println(const char *str = "");
    size_t println(const String &str);

    /**
     * @brief  在指定坐标画字符串（不需要单独 setCursor）
     * 用法：tft.drawString("Hello", 10, 30, RED);
     *       // 坐标(10,30) 处用红色写 "Hello"
     */
    void drawString(const char *str, int16_t x, int16_t y,
                    uint16_t color = WHITE);

    /**
     * @brief  水平居中画字符串（只需提供 Y 坐标）
     * 用法：tft.drawCenterString("Title", 20, GREEN);
     *       // 在 y=20 处居中显示绿色 "Title"
     */
    void drawCenterString(const char *str, int16_t y,
                          uint16_t color = WHITE);

    // ================================================================
    //  实用功能
    // ================================================================

    /**
     * @brief  画带下划线的居中标题
     * 用法：tft.drawTitle("System Ready", CYAN);
     *       // 顶部居中显示青色标题 + 下面一条横线
     */
    void drawTitle(const char *title, uint16_t color = WHITE);

    /**
     * @brief  画进度条
     * @param  percent  百分比 0~100
     * @param  barColor 进度条颜色（默认 GREEN）
     * @param  bgColor  背景颜色（默认 TFT_DARKGRAY）
     *
     * 用法：tft.drawProgressBar(10, 60, 140, 15, 75);
     *       // (10,60) 起点, 宽140高15, 75% 进度
     */
    void drawProgressBar(int16_t x, int16_t y, int16_t w, int16_t h,
                         uint8_t percent,
                         uint16_t barColor = GREEN,
                         uint16_t bgColor = TFT_DARKGRAY);
    /**
     * @brief  清屏并显示通知文本（短文本居中，长文本左对齐）
     * @param  s         要显示的字符串
     * @param  font      自定义字体指针（nullptr = 使用默认字体）
     *                   如 &FreeMono12pt7b
     * @param  y         文本 Y 坐标（默认 27）
     * @param  color     文字颜色（默认 WHITE）
     * @param  shortLen  短文本阈值，len < shortLen 时居中（默认 9）
     *
     * 用法：
     *   tft.drawPageNotify("OK");                      // 短文本居中
     *   tft.drawPageNotify("Long Message", &FreeMono12pt7b);  // 自定义字体
     */
    void drawPageNotify(const char *s,
                        const GFXfont *font = nullptr,
                        int16_t y = 27,
                        uint16_t color = WHITE,
                        uint8_t shortLen = 9);

    /**
     * @brief  清屏并居中显示 1~3 行信息
     * 用法：tft.showInfo("Line1", "Line2", "Line3");
     *       tft.showInfo("Hello");  // 只显示一行
     */
    void showInfo(const char *line1, const char *line2 = nullptr,
                  const char *line3 = nullptr);

    // ================================================================
    //  位图（图片显示）
    // ================================================================

    /**
     * @brief  显示单色位图（1-bit）
     * 用法：tft.drawBitmap(10, 10, myBitmap, 16, 16, RED);
     *       // 在 (10,10) 显示 16x16 红色单色图标
     */
    void drawBitmap(int16_t x, int16_t y, const uint8_t *data,
                    int16_t w, int16_t h, uint16_t color);

    /**
     * @brief  显示 RGB565 彩色位图（推荐，体积小）
     * 制作：
     *   1. 打开 https://javl.github.io/image2cpp/
     *   2. Code output format 选 "Arduino code, 16-bit RGB565"
     *   3. 复制生成的 const uint16_t[] 数组
     * 用法：tft.drawBitmap16(48, 8, logo, 64, 64);
     *       // (48,8) 显示 64x64 彩色图片
     */
    void drawBitmap16(int16_t x, int16_t y, const uint16_t *data,
                      int16_t w, int16_t h);

    /**
     * @brief  显示 RGB888 真彩色位图（体积大）
     * 用法：tft.drawBitmap24(0, 0, rgb888data, 80, 40);
     */
    void drawBitmap24(int16_t x, int16_t y, const uint8_t *data,
                      int16_t w, int16_t h);

    // ================================================================
    //  颜色工具
    // ================================================================

    /**
     * @brief  将 8-bit RGB 转为 16-bit RGB565
     * 用法：uint16_t myColor = tft.color565(255, 128, 0);  // 橙色
     */
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