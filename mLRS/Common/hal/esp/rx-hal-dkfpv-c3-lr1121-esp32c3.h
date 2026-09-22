//*******************************************************
// 版权所有 (c) MLRS 项目
// GPL3 许可证
// https://www.gnu.org/licenses/gpl-3.0.de.html
//*******************************************************
// 硬件抽象层(HAL)配置文件
//********************************************************

//-------------------------------------------------------
// ESP32, ELRS 通用 C3 LR1121 接收器配置
// 这是一个基于ESP32-C3芯片和LR1121射频芯片的接收器硬件配置
//-------------------------------------------------------

// 设备特性配置
#define DEVICE_HAS_SINGLE_LED_RGB  // 设备有一个RGB LED
#define DEVICE_HAS_NO_DEBUG         // 设备不支持调试功能
//#define DEVICE_HAS_SERIAL_OR_DEBUG  // 可选：设备支持串口或调试功能


//-- UART串口配置
// UARTB = 串口 (用于与飞控等设备通信)
// UART = 输出端口，用于SBus或其他输出协议
// UARTF = 调试端口 (用于输出调试信息)

// UARTB配置
#define UARTB_USE_SERIAL           // 使用串口功能
#define UARTB_BAUD                RX_SERIAL_BAUDRATE  // 波特率使用预定义的接收器串口波特率
#define UARTB_USE_TX_IO           IO_P21  // 串口发送引脚为P21
#define UARTB_USE_RX_IO           IO_P20  // 串口接收引脚为P20
#define UARTB_TXBUFSIZE           RX_SERIAL_TXBUFSIZE  // 发送缓冲区大小
#define UARTB_RXBUFSIZE           RX_SERIAL_RXBUFSIZE  // 接收缓冲区大小

// UARTF配置 (调试串口)
#define UARTF_USE_SERIAL           // 使用串口功能
#define UARTF_BAUD                115200  // 调试串口波特率固定为115200


//-- SX1: LR11xx 射频芯片与SPI接口配置

// SPI引脚定义
#define SPI_CS_IO                 IO_P7  // SPI片选引脚
#define SPI_MISO                  IO_P5  // SPI主入从出引脚
#define SPI_MOSI                  IO_P4  // SPI主出从入引脚
#define SPI_SCK                   IO_P6  // SPI时钟引脚
#define SPI_FREQUENCY             16000000L  // SPI频率16MHz (根据数据手册最大值)

// LR11xx芯片控制引脚
#define SX_BUSY                   IO_P3  // 芯片忙状态引脚
#define SX_DIO1                   IO_P1  // 数字IO引脚1 (用于中断)
#define SX_RESET                  IO_P2  // 芯片复位引脚

// 使用DCDC稳压模式
#define SX_USE_REGULATOR_MODE_DCDC

// 声明DIO中断处理函数
IRQHANDLER(void SX_DIO_EXTI_IRQHandler(void);)

/**
 * @brief 初始化LR11xx芯片的GPIO引脚
 */
void sx_init_gpio(void)
{
    gpio_init(SX_DIO1, IO_MODE_INPUT_ANALOG);  // 初始化DIO1为模拟输入模式
    gpio_init(SX_BUSY, IO_MODE_INPUT_ANALOG);  // 初始化BUSY为模拟输入模式
    gpio_init(SX_RESET, IO_MODE_OUTPUT_PP_LOW); // 初始化RESET为推挽输出，初始低电平
}

/**
 * @brief 读取LR11xx芯片的忙状态
 * @return true: 芯片忙, false: 芯片空闲
 */
IRAM_ATTR bool sx_busy_read(void)
{
    return (gpio_read_activehigh(SX_BUSY)) ? true : false;  // 高电平表示忙
}

/**
 * @brief 设置为发送模式时的放大器控制 (此硬件无外部放大器，函数为空)
 */
IRAM_ATTR void sx_amp_transmit(void) {}

/**
 * @brief 设置为接收模式时的放大器控制 (此硬件无外部放大器，函数为空)
 */
IRAM_ATTR void sx_amp_receive(void) {}

/**
 * @brief 初始化DIO中断但保持关闭状态
 */
void sx_dio_init_exti_isroff(void)
{
    detachInterrupt(SX_DIO1);  // 分离DIO1的中断
}

/**
 * @brief 启用DIO中断
 */
void sx_dio_enable_exti_isr(void)
{
    attachInterrupt(SX_DIO1, SX_DIO_EXTI_IRQHandler, RISING);  // 附加上升沿触发中断
}

/**
 * @brief 清除DIO中断标志 (此硬件不需要，函数为空)
 */
IRAM_ATTR void sx_dio_exti_isr_clearflag(void) {}


//-- 按钮配置

#define BUTTON                    IO_P9  // 按钮引脚定义

/**
 * @brief 初始化按钮GPIO
 */
void button_init(void)
{
    gpio_init(BUTTON, IO_MODE_INPUT_PU);  // 初始化为上拉输入模式
}

/**
 * @brief 检测按钮是否被按下
 * @return true: 按钮被按下, false: 按钮未按下
 */
IRAM_ATTR bool button_pressed(void)
{
    return gpio_read_activelow(BUTTON) ? true : false;  // 低电平表示按下
}


//-- LED配置

#define LED_RGB                   IO_P8  // RGB LED引脚
#define LED_RGB_PIXEL_NUM         1      // LED像素数量
#include "esp-hal-led-rgb.h"  // 包含RGB LED驱动头文件


//-- 射频功率配置

#include "../../setup_types.h" // 需要包含此文件以获取射频功率计算所需的频段条件

/**
 * @brief 计算LR11xx芯片的射频功率设置
 * @param power_dbm 期望的输出功率(dBm)
 * @param sx_power 输出参数，LR11xx芯片的功率寄存器值
 * @param actual_power_dbm 输出参数，实际输出功率(dBm)
 * @param frequency_band 频段 (2.4GHz或其他)
 */
#ifdef USE_HPOWER_MODE
#define SX_USE_LP_PA   // 使用低功率放大器 (针对 900MHz 频段)
void lr11xx_rfpower_calc(const int8_t power_dbm, int8_t* sx_power, int8_t* actual_power_dbm, const uint8_t frequency_band)
{
    if (frequency_band == SX_FHSS_FREQUENCY_BAND_2P4_GHZ) {
        // --- 2.4GHz 频段功率配置 ---
        // 根据 target power 设置芯片寄存器值
        if (power_dbm >= POWER_30_DBM) { // 30 dBm (1000mW)
            *sx_power = 2;
            *actual_power_dbm = 30;
        } else if (power_dbm >= POWER_27_DBM) { // 27 dBm (500mW)
            *sx_power = 0;
            *actual_power_dbm = 27;
        } else if (power_dbm >= POWER_24_DBM) { // 24 dBm (250mW)
            *sx_power = -4;
            *actual_power_dbm = 24;
        } else if (power_dbm >= POWER_20_DBM) { // 20 dBm (100mW)
            *sx_power = -6;
            *actual_power_dbm = 20;
        } else if (power_dbm >= POWER_17_DBM) { // 17 dBm (50mW)
            *sx_power = -8;
            *actual_power_dbm = 17;
        } else if (power_dbm >= POWER_14_DBM) { // 14 dBm (25mW)
            *sx_power = -14;
            *actual_power_dbm = 14;
        } else {
            *sx_power = -18;
            *actual_power_dbm = 10; // 实测约 11 dBm
        }
    } else {
        // --- 900MHz 频段功率配置 ---
        if (power_dbm >= POWER_30_DBM) { // 30 dBm

            *sx_power = 3;
            *actual_power_dbm = 30;
        } else if (power_dbm >= POWER_27_DBM) { // 27 dBm
            *sx_power = -1;
            *actual_power_dbm = 27;
        } else if (power_dbm >= POWER_24_DBM) { // 24 dBm
            *sx_power = -7;
            *actual_power_dbm = 24;
        } else if (power_dbm >= POWER_20_DBM) { // 20 dBm
            *sx_power = -11;
            *actual_power_dbm = 20;
        } else if (power_dbm >= POWER_17_DBM) { // 17 dBm
            *sx_power = -14;
            *actual_power_dbm = 17;
        } else if (power_dbm >= POWER_14_DBM) { // 14 dBm
            *sx_power = -16;
            *actual_power_dbm = 14;
        } else {
            *sx_power = -17;
            *actual_power_dbm = 10;
        }
    }
}

// 默认功率索引 (对应下面的列表第0个)
#define RFPOWER_DEFAULT 0 

// 支持的功率档位列表
const rfpower_t rfpower_list[] = {
    { .dbm = POWER_10_DBM, .mW = 10 },
    { .dbm = POWER_14_DBM, .mW = 25 },
    { .dbm = POWER_17_DBM, .mW = 50 },
    { .dbm = POWER_20_DBM, .mW = 100 },
    { .dbm = POWER_24_DBM, .mW = 250 },
    { .dbm = POWER_27_DBM, .mW = 500 },
    { .dbm = POWER_30_DBM, .mW = 1000 },
};
#else 
void lr11xx_rfpower_calc(const int8_t power_dbm, int8_t* sx_power, int8_t* actual_power_dbm, const uint8_t frequency_band)
{
    if (frequency_band == SX_FHSS_FREQUENCY_BAND_2P4_GHZ) {  // 2.4GHz频段
        if (power_dbm >= POWER_20_DBM) { // 期望功率 >= 20dBm
            *sx_power = 2;  // 芯片功率寄存器值
            *actual_power_dbm = 20;  // 实际输出功率20dBm (实测约19dBm，继续增加功率提升不大，PA最大输入为+5dBm)
        } else if (power_dbm >= POWER_14_DBM) { // 期望功率 >= 14dBm
            *sx_power = -6;
            *actual_power_dbm = 14;
        } else if (power_dbm >= POWER_10_DBM) { // 期望功率 >= 10dBm
            *sx_power = -11;
            *actual_power_dbm = 10;
        } else {  // 低功率模式
            *sx_power = -18;
            *actual_power_dbm = 3;
        }
    } else {  // 其他频段 (如868MHz/915MHz等)
        if (power_dbm >= POWER_20_DBM) { // 期望功率 >= 20dBm
            *sx_power = 22;
            *actual_power_dbm = 20;
        } else if (power_dbm >= POWER_14_DBM) { // 期望功率 >= 14dBm
            *sx_power = 16;
            *actual_power_dbm = 14;
        } else if (power_dbm >= POWER_10_DBM) { // 期望功率 >= 10dBm
            *sx_power = 12;
            *actual_power_dbm = 10;
        } else {  // 低功率模式
            *sx_power = 5;
            *actual_power_dbm = 3;
        }

    }
}

// 默认射频功率 (索引值，指向rfpower_list数组)
#define RFPOWER_DEFAULT           1

// 射频功率列表 (支持4档功率)
const rfpower_t rfpower_list[] = {
    { .dbm = POWER_3_DBM, .mW = 2 },   // 3dBm (约2mW)
    { .dbm = POWER_10_DBM, .mW = 10 }, // 10dBm (约10mW)
    { .dbm = POWER_14_DBM, .mW = 25 }, // 14dBm (约25mW)
    { .dbm = POWER_20_DBM, .mW = 100 },// 20dBm (约100mW)
};




#endif // !1

