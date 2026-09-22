//*******************************************************
// Copyright (c) MLRS project
// GPL3 License
// https://www.gnu.org/licenses/gpl-3.0.de.html
//*******************************************************
// HAL: DAKEFPV 900MHz 2W TX, LR1121, ESP32
//*******************************************************

//-- Device Features
#define DEVICE_HAS_JRPIN5
#define DEVICE_HAS_IN
#define DEVICE_HAS_NO_DEBUG
#define DEVICE_HAS_SINGLE_LED_RGB
#define DEVICE_HAS_COM_ON_SERIAL
#define DEVICE_HAS_FAN_ONOFF
#define DEVICE_HAS_ESP_WIFI_BRIDGE_CONFIGURE
#define DEVICE_HAS_ESP_WIFI_BRIDGE_ESP32C3
#define DEVICE_HAS_SPI_DISPLAY
#define DEVICE_HAS_FIVEWAY

//-- UARTs
#define UARTB_USE_SERIAL
#define UARTB_BAUD             TX_SERIAL_BAUDRATE
#define UARTB_USE_TX_IO        IO_P1
#define UARTB_USE_RX_IO        IO_P3
#define UARTB_TXBUFSIZE        TX_SERIAL_TXBUFSIZE
#define UARTB_RXBUFSIZE        TX_SERIAL_RXBUFSIZE

#define UARTC_USE_SERIAL
#define UARTC_BAUD             115200
#define UARTC_USE_TX_IO        IO_P1
#define UARTC_USE_RX_IO        IO_P3
#define UARTC_TXBUFSIZE        0
#define UARTC_RXBUFSIZE        TX_COM_RXBUFSIZE

#define UART_USE_SERIAL1
#define UART_BAUD              400000
#define UART_USE_TX_IO         IO_P13
#define UART_USE_RX_IO         IO_P13
#define UART_TXBUFSIZE         0
#define UART_RXBUFSIZE         0

#define UARTD_USE_SERIAL2
#define UARTD_BAUD             TX_SERIAL_BAUDRATE
#define UARTD_USE_TX_IO        IO_P17
#define UARTD_USE_RX_IO        IO_P16
#define UARTD_TXBUFSIZE        TX_SERIAL_TXBUFSIZE
#define UARTD_RXBUFSIZE        TX_SERIAL_RXBUFSIZE

#define UARTE_USE_SERIAL1
#define UARTE_BAUD             100000
#define UARTE_USE_TX_IO        -1
#define UARTE_USE_RX_IO        IO_P13
#define UARTE_RXBUFSIZE        0

//-- SX: LR11xx & SPI
#define SPI_CS_IO              IO_P5
#define SPI_MISO               IO_P19
#define SPI_MOSI               IO_P23
#define SPI_SCK                IO_P18
#define SPI_FREQUENCY          16000000L
#define SX_RESET               IO_P22
#define SX_BUSY                IO_P38
#define SX_DIO1                IO_P39
#define SX_USE_REGULATOR_MODE_DCDC

IRQHANDLER(void SX_DIO_EXTI_IRQHandler(void);)

//-- TFT SPI Display
#define TFT_SCLK               IO_P2
#define TFT_MOSI               IO_P12
#define TFT_MISO               -1
#define TFT_CS                 -1
#define TFT_DC                 IO_P14
#define TFT_RST                IO_P33
#define TFT_BL                 IO_P32  // LOW=on, PNP transistor

//-- Fiveway Key
#define FIVEWAY_ADC_IO         IO_P35
#define KEY_DOWN_THRESH        3090
#define KEY_RIGHT_THRESH       0
#define KEY_UP_THRESH          1860
#define KEY_CENTER_THRESH      1190
#define KEY_LEFT_THRESH        2512

#if defined DEVICE_HAS_FIVEWAY || defined DEVICE_HAS_SPI_DISPLAY || defined DEVICE_HAS_SPI_DISPLAY_ROT270

void fiveway_init(void) {}

IRAM_ATTR uint16_t fiveway_adc_read(void)
{
    return analogRead(FIVEWAY_ADC_IO);
}

IRAM_ATTR uint8_t fiveway_read(void)
{
    int16_t adc = analogRead(FIVEWAY_ADC_IO);
    if (adc < ((KEY_RIGHT_THRESH + KEY_CENTER_THRESH) / 2))  return (1 << KEY_RIGHT);
    if (adc < ((KEY_CENTER_THRESH + KEY_UP_THRESH) / 2))     return (1 << KEY_CENTER);
    if (adc < ((KEY_UP_THRESH + KEY_LEFT_THRESH) / 2))       return (1 << KEY_UP);
    if (adc < ((KEY_LEFT_THRESH + KEY_DOWN_THRESH) / 2))     return (1 << KEY_LEFT);
    if (adc < ((KEY_DOWN_THRESH + 4095) / 2))                return (1 << KEY_DOWN);
    return 0;
}
#endif

//-- SX GPIO
void sx_init_gpio(void)
{
    gpio_init(SX_DIO1, IO_MODE_INPUT_ANALOG);
    gpio_init(SX_BUSY, IO_MODE_INPUT_ANALOG);
    gpio_init(SX_RESET, IO_MODE_OUTPUT_PP_LOW);
}

IRAM_ATTR bool sx_busy_read(void) { return gpio_read_activehigh(SX_BUSY) ? true : false; }
IRAM_ATTR void sx_amp_transmit(void) {}
IRAM_ATTR void sx_amp_receive(void) {}

void sx_dio_enable_exti_isr(void) { attachInterrupt(SX_DIO1, SX_DIO_EXTI_IRQHandler, RISING); }
void sx_dio_init_exti_isroff(void) { detachInterrupt(SX_DIO1); }
void sx_dio_exti_isr_clearflag(void) {}

IRAM_ATTR void spib_select(void) {}
IRAM_ATTR void spib_deselect(void) {}

//-- In Port
void in_init_gpio(void) {}
void in_set_normal(void)   { gpio_matrix_in((gpio_num_t)UARTE_USE_RX_IO, U1RXD_IN_IDX, false); }
void in_set_inverted(void) { gpio_matrix_in((gpio_num_t)UARTE_USE_RX_IO, U1RXD_IN_IDX, true); }

//-- Button
#define BUTTON                 IO_P0
#define BUTTON2                BUTTON

void button_init(void)
{
    gpio_init(BUTTON, IO_MODE_INPUT_PU);
    gpio_init(BUTTON2, IO_MODE_INPUT_PU);
}

IRAM_ATTR bool button_pressed(void)  { return gpio_read_activelow(BUTTON) ? true : false; }
IRAM_ATTR bool button2_pressed(void) { return gpio_read_activelow(BUTTON2) ? true : false; }

//-- LED
#define LED_RGB                IO_P21
#define LED_RGB_PIXEL_NUM      1
#include "esp-hal-led-rgb.h"

#ifdef DEVICE_HAS_COM_ON_SERIAL

bool ser_or_com_init(void) // return true if is_serial
{
    gpio_init(BUTTON, IO_MODE_INPUT_PU);
    gpio_init(BUTTON2, IO_MODE_INPUT_PU);
    uint8_t cnt = 0;
    for (uint8_t i = 0; i < 16; i++) {
        if (gpio_read_activelow(BUTTON) || gpio_read_activelow(BUTTON2)) cnt++;
    }
    return !(cnt > 8);
}
#endif

//-- Fan
#define FAN_IO                 IO_P4

void fan_init(void) { gpio_init(FAN_IO, IO_MODE_OUTPUT_PP_LOW); }

IRAM_ATTR void fan_set_power(int8_t power_dbm)
{
    if (power_dbm >= POWER_23_DBM) { gpio_high(FAN_IO); }
    else                           { gpio_low(FAN_IO); }
}

//-- ESP WiFi Bridge
#ifdef DEVICE_HAS_ESP_WIFI_BRIDGE_ESP32C3

#define ESP_RESET              IO_P25
#define ESP_GPIO0              IO_P15
#define ESP_BOOT0              IO_P0

uint8_t esp_boot0(void) { return gpio_read_activelow(ESP_BOOT0); }

void esp_init(void)
{
    gpio_init(ESP_RESET, IO_MODE_OUTPUT_PP_LOW);
    gpio_init(ESP_GPIO0, IO_MODE_OUTPUT_PP_LOW);
}

IRAM_ATTR void esp_reset_high(void)  { gpio_high(ESP_RESET); }
IRAM_ATTR void esp_reset_low(void)   { gpio_low(ESP_RESET); }
IRAM_ATTR void esp_gpio0_high(void)  { gpio_low(ESP_GPIO0); }
IRAM_ATTR void esp_gpio0_low(void)   { gpio_high(ESP_GPIO0); }

#endif

//-- Power (external PA with DAC control)
#include "../../setup_types.h"
#define SX_USE_LP_PA
#define SX_PA_DAC_IO           IO_P26

void lr11xx_rfpower_calc(const int8_t power_dbm, int8_t *sx_power, int8_t *actual_power_dbm, const uint8_t frequency_band)
{
    if (frequency_band != SX_FHSS_FREQUENCY_BAND_2P4_GHZ) {
        uint8_t dac = 245;

        if (power_dbm >= POWER_33_DBM)      { dac = 245; *sx_power = 7;   *actual_power_dbm = 33; }
        else if (power_dbm >= POWER_30_DBM) { dac = 245; *sx_power = 3;   *actual_power_dbm = 30; }
        else if (power_dbm >= POWER_27_DBM) { dac = 240; *sx_power = -1;  *actual_power_dbm = 27; }
        else if (power_dbm >= POWER_24_DBM) { dac = 230; *sx_power = -4;  *actual_power_dbm = 24; }
        else if (power_dbm >= POWER_20_DBM) { dac = 230; *sx_power = -6;  *actual_power_dbm = 20; }
        else if (power_dbm >= POWER_17_DBM) { dac = 230; *sx_power = -8;  *actual_power_dbm = 17; }
        else if (power_dbm >= POWER_14_DBM) { dac = 200; *sx_power = -11; *actual_power_dbm = 14; }
        else                                { dac = 200; *sx_power = -13; *actual_power_dbm = 10; }

        dacWrite(SX_PA_DAC_IO, dac);
    }
}

#define RFPOWER_DEFAULT 3

const rfpower_t rfpower_list[] = {
    {.dbm = POWER_10_DBM, .mW = 10},
    {.dbm = POWER_14_DBM, .mW = 25},
    {.dbm = POWER_17_DBM, .mW = 50},
    {.dbm = POWER_20_DBM, .mW = 100},
    {.dbm = POWER_24_DBM, .mW = 250},
    {.dbm = POWER_27_DBM, .mW = 500},
    {.dbm = POWER_30_DBM, .mW = 1000},
    {.dbm = POWER_33_DBM, .mW = 2000},
};