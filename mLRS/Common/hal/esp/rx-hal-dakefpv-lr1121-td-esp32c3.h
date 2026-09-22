/*******************************************************
 * Copyright (c) MLRS project
 * GPL3
 * https://www.gnu.org/licenses/gpl-3.0.de.html
 *******************************************************
 * 硬件抽象层(HAL)配置文件
 * 用于ESP32C3平台的ELRS双频接收器配置
 *******************************************************/

//-------------------------------------------------------
// ESP32, ELRS GENERIC LR1121 True Diversity Receiver
// ESP32C3 ELRS通用LR1121双频接收器
//-------------------------------------------------------

// 设备特性定义
#define DEVICE_HAS_SINGLE_LED_RGB       // 设备使用单个RGB LED
#define DEVICE_HAS_DIVERSITY_SINGLE_SPI // 使用单SPI接口的双天线分集功能(必须设置，否则无法工作)
#define DEVICE_HAS_NO_DEBUG             // 禁用调试功能
// #define DEVICE_HAS_SERIAL_OR_DEBUG    // 可选：启用串口或调试功能

//-- UARTS (串口配置)
// UARTB = 串口(用于通信)
// UART = 输出端口，用于SBus或其他输出
// UARTF = 调试端口

// 串口B配置(串口通信)
#define UARTB_USE_SERIAL                    // 启用串口B
#define UARTB_BAUD RX_SERIAL_BAUDRATE       // 波特率使用RX串口波特率定义
#define UARTB_TXBUFSIZE RX_SERIAL_TXBUFSIZE // 发送缓冲区大小
#define UARTB_RXBUFSIZE RX_SERIAL_RXBUFSIZE // 接收缓冲区大小

// 串口F配置(调试)
#define UARTF_USE_SERIAL  // 启用串口F
#define UARTF_BAUD 115200 // 调试波特率115200

//-- SX1: LR11xx & SPI (第一个射频模块配置)

// SPI引脚定义
#define SPI_CS_IO IO_P0         // SPI片选引脚
#define SPI_MISO IO_P5          // SPI主入从出引脚
#define SPI_MOSI IO_P4          // SPI主出从入引脚
#define SPI_SCK IO_P6           // SPI时钟引脚
#define SPI_FREQUENCY 16000000L // SPI频率16MHz(数据手册最大值)

// SX1模块引脚定义
#define SX_BUSY IO_P3  // 忙状态引脚
#define SX_DIO1 IO_P1  // 数字IO1引脚(用于中断)
#define SX_RESET IO_P2 // 复位引脚

// 电源模式配置
#define SX_USE_REGULATOR_MODE_DCDC // 使用DCDC稳压模式

// 中断处理函数声明
IRQHANDLER(void SX_DIO_EXTI_IRQHandler(void);)

/**
 * @brief 初始化SX1模块的GPIO引脚
 */
void sx_init_gpio(void)
{
  gpio_init(SX_DIO1, IO_MODE_INPUT_ANALOG);   // DIO1配置为模拟输入模式
  gpio_init(SX_BUSY, IO_MODE_INPUT_ANALOG);   // BUSY配置为模拟输入模式
  gpio_init(SX_RESET, IO_MODE_OUTPUT_PP_LOW); // RESET配置为推挽输出，初始低电平
}

/**
 * @brief 读取SX1模块的忙状态
 * @return true=忙状态, false=空闲状态
 */
IRAM_ATTR bool sx_busy_read(void)
{
  return (gpio_read_activehigh(SX_BUSY)) ? true : false;
}

/**
 * @brief SX1发射模式下的功放控制(本设备无外部功放，函数为空)
 */
IRAM_ATTR void sx_amp_transmit(void) {}

/**
 * @brief SX1接收模式下的功放控制(本设备无外部功放，函数为空)
 */
IRAM_ATTR void sx_amp_receive(void) {}

/**
 * @brief 初始化SX1的DIO中断并关闭中断
 */
void sx_dio_init_exti_isroff(void)
{
  detachInterrupt(SX_DIO1); // 分离DIO1中断
}

/**
 * @brief 启用SX1的DIO外部中断
 */
void sx_dio_enable_exti_isr(void)
{
  attachInterrupt(SX_DIO1, SX_DIO_EXTI_IRQHandler, RISING); // 在上升沿触发中断
}

/**
 * @brief 清除SX1的DIO中断标志(本设备无需清除，函数为空)
 */
IRAM_ATTR void sx_dio_exti_isr_clearflag(void) {}

//-- SX2: LR11xx & SPI (第二个射频模块配置)

// SX2模块引脚定义(共用SPI总线，使用不同的片选)
#define SX2_CS_IO IO_P7  // SPI片选引脚(第二个模块)
#define SX2_BUSY IO_P8   // 忙状态引脚
#define SX2_DIO1 IO_P18  // 数字IO1引脚(用于中断)
#define SX2_RESET IO_P10 // 复位引脚

// 电源模式配置
#define SX2_USE_REGULATOR_MODE_DCDC // 使用DCDC稳压模式

// 中断处理函数声明
IRQHANDLER(void SX2_DIO_EXTI_IRQHandler(void);)

/**
 * @brief 初始化SX2模块的GPIO引脚
 */
void sx2_init_gpio(void)
{
  gpio_init(SX2_CS_IO, IO_MODE_OUTPUT_PP_HIGH); // CS初始化为推挽输出高电平
  gpio_init(SX2_DIO1, IO_MODE_INPUT_ANALOG);    // DIO1配置为模拟输入模式
  gpio_init(SX2_BUSY, IO_MODE_INPUT_ANALOG);    // BUSY配置为模拟输入模式
  gpio_init(SX2_RESET,
            IO_MODE_OUTPUT_PP_LOW); // RESET配置为推挽输出，初始低电平
}

/**
 * @brief 选择SX2的SPI总线(拉低片选信号)
 */
IRAM_ATTR void spib_select(void)
{
  gpio_low(SX2_CS_IO); // 拉低CS信号，选择SX2
}

/**
 * @brief 取消选择SX2的SPI总线(拉高片选信号)
 */
IRAM_ATTR void spib_deselect(void)
{
  gpio_high(SX2_CS_IO); // 拉高CS信号，取消选择SX2
}

/**
 * @brief 读取SX2模块的忙状态
 * @return true=忙状态, false=空闲状态
 */
IRAM_ATTR bool sx2_busy_read(void)
{
  return (gpio_read_activehigh(SX2_BUSY)) ? true : false;
}

/**
 * @brief SX2发射模式下的功放控制(本设备无外部功放，函数为空)
 */
IRAM_ATTR void sx2_amp_transmit(void) {}

/**
 * @brief SX2接收模式下的功放控制(本设备无外部功放，函数为空)
 */
IRAM_ATTR void sx2_amp_receive(void) {}

/**
 * @brief 初始化SX2的DIO中断并关闭中断
 */
void sx2_dio_init_exti_isroff(void)
{
  detachInterrupt(SX2_DIO1); // 分离DIO1中断
}

/**
 * @brief 启用SX2的DIO外部中断
 */
void sx2_dio_enable_exti_isr(void)
{
  attachInterrupt(SX2_DIO1, SX2_DIO_EXTI_IRQHandler,
                  RISING); // 在上升沿触发中断
}

/**
 * @brief 清除SX2的DIO中断标志(本设备无需清除，函数为空)
 */
void sx2_dio_exti_isr_clearflag(void) {}

//-- Button (按键配置)

#define BUTTON IO_P9 // 按键引脚定义

/**
 * @brief 初始化按键GPIO
 */
void button_init(void)
{
  gpio_init(BUTTON, IO_MODE_INPUT_PU); // 配置为上拉输入模式
}

/**
 * @brief 检测按键是否按下
 * @return true=按下, false=未按下
 */
IRAM_ATTR bool button_pressed(void)
{
  return gpio_read_activelow(BUTTON) ? true : false; // 低电平有效
}

//-- LEDs (LED配置)

#define LED_RGB IO_P19          // RGB LED引脚
#define LED_RGB_PIXEL_NUM 1     // LED像素数量
#include "esp-hal-led-rgb.h" // 包含RGB LED驱动头文件

//-- POWER (射频功率配置)

#include "../../setup_types.h" // 包含设置类型定义，用于射频功率计算中的频段条件

/**
 * @brief 计算LR11xx射频芯片的功率设置
 * @param power_dbm 期望的输出功率(dBm)
 * @param sx_power 输出参数: SX芯片的功率设置值
 * @param actual_power_dbm 输出参数: 实际输出功率(dBm)
 * @param frequency_band 工作频段
 */

// 使用高功率模式
#ifdef USE_HPOWER_MODE
#define SX_USE_LP_PA // 使用低功率放大器 (针对 900MHz 频段)
void lr11xx_rfpower_calc(const int8_t power_dbm, int8_t *sx_power,
                         int8_t *actual_power_dbm,
                         const uint8_t frequency_band)
{
  if (frequency_band == SX_FHSS_FREQUENCY_BAND_2P4_GHZ)
  {
    // --- 2.4GHz 频段功率配置 ---
    // 根据 target power 设置芯片寄存器值
    if (power_dbm >= POWER_27_DBM)
    { // 27 dBm (500mW)
      *sx_power = 1;
      *actual_power_dbm = 27;
    }
    else if (power_dbm >= POWER_24_DBM)
    { // 24 dBm (250mW)
      *sx_power = -1;
      *actual_power_dbm = 24;
    }
    else if (power_dbm >= POWER_20_DBM)
    { // 20 dBm (100mW)
      *sx_power = -6;
      *actual_power_dbm = 20;
    }
    else if (power_dbm >= POWER_17_DBM)
    { // 17 dBm (50mW)
      *sx_power = -8;
      *actual_power_dbm = 17;
    }
    else if (power_dbm >= POWER_14_DBM)
    { // 14 dBm (25mW)
      *sx_power = -14;
      *actual_power_dbm = 14;
    }
    else
    {
      *sx_power = -18;
      *actual_power_dbm = 10; // 实测约 11 dBm
    }
  }
  else
  {
    // --- 900MHz 频段功率配置 ---
    if (power_dbm >= POWER_27_DBM)
    { // 27 dBm
      *sx_power = -1;
      *actual_power_dbm = 27;
    }
    else if (power_dbm >= POWER_24_DBM)
    { // 24 dBm
      *sx_power = -7;
      *actual_power_dbm = 24;
    }
    else if (power_dbm >= POWER_20_DBM)
    { // 20 dBm
      *sx_power = -11;
      *actual_power_dbm = 20;
    }
    else if (power_dbm >= POWER_17_DBM)
    { // 17 dBm
      *sx_power = -14;
      *actual_power_dbm = 17;
    }
    else if (power_dbm >= POWER_14_DBM)
    { // 14 dBm
      *sx_power = -16;
      *actual_power_dbm = 14;
    }
    else
    {
      *sx_power = -17;
      *actual_power_dbm = 10;
    }
  }
}

// 默认射频功率 (索引值，指向rfpower_list数组)
#define RFPOWER_DEFAULT 1

// 射频功率列表 (支持4档功率)
const rfpower_t rfpower_list[] = {
    {.dbm = POWER_10_DBM, .mW = 10},
    {.dbm = POWER_14_DBM, .mW = 25},
    {.dbm = POWER_17_DBM, .mW = 50},
    {.dbm = POWER_20_DBM, .mW = 100},
    {.dbm = POWER_24_DBM, .mW = 250},
    {.dbm = POWER_27_DBM, .mW = 500},
};
#else
void lr11xx_rfpower_calc(const int8_t power_dbm, int8_t *sx_power,
                         int8_t *actual_power_dbm,
                         const uint8_t frequency_band)
{
  // 2.4GHz频段功率配置
  if (frequency_band == SX_FHSS_FREQUENCY_BAND_2P4_GHZ)
  {
    if (power_dbm >= POWER_20_DBM)
    { // -> 20dBm
      *sx_power = 3;
      *actual_power_dbm =
          20; // 实测约19dBm，增加功率效果不明显，PA最大输入为+5dBm
    }
    else if (power_dbm >= POWER_14_DBM)
    { // -> 14dBm
      *sx_power = -6;
      *actual_power_dbm = 14;
    }
    else if (power_dbm >= POWER_10_DBM)
    { // -> 10dBm
      *sx_power = -11;
      *actual_power_dbm = 10;
    }
    else
    {
      *sx_power = -18;
      *actual_power_dbm = 3;
    }
  }
  // 其他频段(如868/915MHz)功率配置
  else
  {
    if (power_dbm >= POWER_20_DBM)
    { // -> 20dBm
      *sx_power = 22;
      *actual_power_dbm = 20;
    }
    else if (power_dbm >= POWER_14_DBM)
    { // -> 14dBm
      *sx_power = 16;
      *actual_power_dbm = 14;
    }
    else if (power_dbm >= POWER_10_DBM)
    { // -> 10dBm
      *sx_power = 12;
      *actual_power_dbm = 10;
    }
    else
    {
      *sx_power = 5;
      *actual_power_dbm = 3;
    }
  }
}

// 默认射频功率设置(索引到rfpower_list数组)
#define RFPOWER_DEFAULT 1

// 射频功率列表
const rfpower_t rfpower_list[] = {
    {.dbm = POWER_3_DBM, .mW = 2},    // 3dBm = 2mW
    {.dbm = POWER_10_DBM, .mW = 10},  // 10dBm = 10mW
    {.dbm = POWER_14_DBM, .mW = 25},  // 14dBm = 25mW
    {.dbm = POWER_20_DBM, .mW = 100}, // 20dBm = 100mW
};
#endif
