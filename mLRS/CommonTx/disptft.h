//*******************************************************
// Copyright (c) MLRS project
// GPL3
// https://www.gnu.org/licenses/gpl-3.0.de.html
// OlliW @ www.olliw.eu
//*******************************************************
// Display Interface — TFT version
//
// TFT screen menu system for mLRS transmitter.
// Ported from disp.h (OLED/gdisp), using TFT class for rendering.
//********************************************************
#ifndef DISPTFT_H
#define DISPTFT_H
#pragma once

#include "../Common/hal/hal.h"

#ifndef USE_TFTDISPLAY

class tTxDispTFT
{
public:
    void Init(void) {}
    void Tick_ms(void) {}
    void DrawNotify(const char *const s) {}
    void DrawBoot(void) {}
};

#else

#include <stdlib.h>
#include <ctype.h>
#include "../Common/thirdparty/mlrs-logo.h"
#include "../Common/tasks.h"
#include "../Common/thirdparty/tft.hpp"
// #include "../Common/thirdparty/log.hpp"

extern bool connected(void);
extern bool connected_and_rx_setup_available(void);
extern tStats stats;
extern tSetupMetaData SetupMetaData;
extern tSetup Setup;
extern tGlobalConfig Config;
extern tTxInfo info;
extern tTasks tasks;
extern TFT tft;

#define DISP_START_PAGE_TMO_MS SYSTICK_DELAY_MS(1500)
#define KEYS_DEBOUNCE_TMO_MS SYSTICK_DELAY_MS(40)

#ifndef DISP_CONTENT_Y_BASE
#define DISP_CONTENT_Y_BASE 24
#endif

#ifndef TFT_BG_COLOR
#define TFT_BG_COLOR BLACK
#endif
#ifndef TFT_FG_COLOR
#define TFT_FG_COLOR WHITE
#endif
#ifndef TFT_HL_BG
#define TFT_HL_BG WHITE
#endif
#ifndef TFT_HL_FG
#define TFT_HL_FG BLACK
#endif

typedef enum
{
    PAGE_STARTUP = 0,
    PAGE_NOTIFY_BIND,
    PAGE_NOTIFY_STORE,
    PAGE_MAIN,
    PAGE_COMMON,
    PAGE_TX,
    PAGE_RX,
    PAGE_ACTIONS,
    PAGE_NAV_MIN = PAGE_MAIN,
    PAGE_NAV_MAX = PAGE_ACTIONS,
} PAGE_ENUM;

typedef enum
{
    SUBPAGE_DEFAULT = 0,
    SUBPAGE_MAIN_SUB0 = SUBPAGE_DEFAULT,
    SUBPAGE_MAIN_SUB1,
    SUBPAGE_MAIN_SUB2,
    SUBPAGE_MAIN_SUB3,
#if defined USE_ESP_WIFI_BRIDGE_CONFIGURE || defined USE_HC04_MODULE
    SUBPAGE_MAIN_SUB4,
#endif
    SUBPAGE_MAIN_NUM,
} SUBPAGE_ENUM;

typedef enum
{
    DISP_ACTION_STORE = 0,
    DISP_ACTION_BIND,
    DISP_ACTION_BOOT,
    DISP_ACTION_FLASH_ESP,
} DISP_ACTION_ENUM;

const uint8_t disp_actions[] = {
    DISP_ACTION_STORE,
    DISP_ACTION_BIND,
#if !(defined ESP8266 || defined ESP32)
    DISP_ACTION_BOOT,
#endif
#ifdef USE_ESP_WIFI_BRIDGE
    DISP_ACTION_FLASH_ESP,
#endif
};
#define DISP_ACTION_NUM sizeof(disp_actions)

class tTxDispTFT
{
public:
    void Init(void);
    void Tick_ms(void);
    void UpdateMain(void);
    void SetBind(void);
    void Draw(void);
    void DrawNotify(const char *const s);
    void DrawBoot(void);

    typedef struct
    {
        uint8_t list[SETUP_PARAMETER_NUM];
        uint8_t num;
        uint8_t allowed_num[SETUP_PARAMETER_NUM];
        void clear(void) { num = 0; }
        void add(uint8_t i)
        {
            list[num] = i;
            allowed_num[num] = param_get_allowed_opt_num(i);
            num++;
        }
    } tParamList;

    bool key_has_been_pressed(uint8_t key_idx);
    void draw_page_startup(void);
    void draw_page_notify(const char *const s);
    void draw_page_main(void);
    void draw_page_common(void);
    void draw_page_tx(void);
    void draw_page_rx(void);
    void draw_page_actions(void);
    void draw_page_main_sub0(void);
    void draw_page_main_sub1(void);
    void draw_page_main_sub2(void);
    void draw_page_main_sub3(void);
    void draw_page_main_sub4(void);
    void draw_header(const char *const s);
    void draw_options(tParamList *const list);

    void _setcurX(int16_t x);
    void _setcurY(int16_t y);
    void _setcurXY(int16_t x, int16_t y);
    void _movecurX(int16_t dx);
    void _puts(const char *s);
    void _putc(char c);
    void _puts_XCentered(const char *s);
    void _setinverted(void);
    void _unsetinverted(void);

    bool initialized;
    bool connected_last;
    bool setupmetadata_rx_available_last;
    uint16_t keys_tick;
    uint8_t keys_state, keys_ct0, keys_ct1, keys_has_been_pressed;
    uint16_t keys_pressed_tmo;
    uint8_t keys_pending;
    uint8_t page;
    uint16_t page_startup_tmo;
    bool page_modified, page_update;
    uint8_t subpage, subpage_max;
    uint8_t idx_first, idx_max, idx_focused;
    bool idx_focused_in_edit;
    uint8_t idx_focused_pos;
    uint8_t edit_setting_task_pending;
    uint16_t bind_tmo;
    tParamList common_list, tx_list, rx_list;
    int16_t _cur_x, _cur_y;
    uint8_t _fontSize;

    void load_rx_list(void);
    tParamList *current_list(void);
    void page_init(void);
    bool edit_setting(void);
    void run_action(void);
};

//==================== cursor tracking ====================
void tTxDispTFT::_setcurX(int16_t x)
{
    _cur_x = x;
    tft.setCursor(_cur_x, _cur_y);
}
void tTxDispTFT::_setcurY(int16_t y)
{
    _cur_y = y;
    tft.setCursor(_cur_x, _cur_y);
}
void tTxDispTFT::_setcurXY(int16_t x, int16_t y)
{
    _cur_x = x;
    _cur_y = y;
    tft.setCursor(_cur_x, _cur_y);
}
void tTxDispTFT::_movecurX(int16_t dx)
{
    _cur_x += dx;
    tft.setCursor(_cur_x, _cur_y);
}

void tTxDispTFT::_puts(const char *s)
{
    tft.print(s);
    _cur_x += strlen(s) * 6 * _fontSize;
}
void tTxDispTFT::_putc(char c)
{
    char b[2] = {c, 0};
    tft.print(b);
    _cur_x += 6 * _fontSize;
}
void tTxDispTFT::_puts_XCentered(const char *s)
{
    uint16_t w = strlen(s) * 6 * _fontSize;
    int16_t x = (tft.width() - w) / 2;
    _setcurX(x);
    tft.print(s);
    _cur_x = x + w;
}
void tTxDispTFT::_setinverted(void) { tft.setTextColor(TFT_HL_FG, TFT_HL_BG); }
void tTxDispTFT::_unsetinverted(void) { tft.setTextColor(TFT_FG_COLOR); }

//==================== Init ====================
void tTxDispTFT::Init(void)
{
    initialized = tft.begin(TFTModel::ST7735_160x80);
    if (initialized)
    {
        tft.setRotation(3);
        tft.fillScreen(TFT_BG_COLOR);
        tft.setTextColor(TFT_FG_COLOR);
        tft.setTextSize(1);
        _fontSize = 1;

    }
    connected_last = false;
    setupmetadata_rx_available_last = false;
    keys_tick = 0;
    keys_state = 0;
    keys_ct0 = 0;
    keys_ct1 = 0;
    keys_has_been_pressed = 0;
    keys_pressed_tmo = 0;
    keys_state = fiveway_read();
    page = PAGE_STARTUP;
    page_startup_tmo = DISP_START_PAGE_TMO_MS;
    page_modified = true;
    page_update = false;
    subpage = SUBPAGE_DEFAULT;
    subpage_max = 0;
    _cur_x = 0;
    _cur_y = 0;
    common_list.clear();
    tx_list.clear();
    rx_list.clear();
    for (uint8_t i = 0; i < SETUP_PARAMETER_NUM; i++)
    {
        if (setup_param_is_tx(i))
            tx_list.add(i);
        else if (setup_param_is_rx(i))
        {
        }
        else
        {
            if (param_get_allowed_mask(i))
                common_list.add(i);
        }
    }
    idx_first = 0;
    idx_focused = 0;
    idx_max = 0;
    idx_focused_in_edit = false;
    idx_focused_pos = 0;
    edit_setting_task_pending = MAIN_TASK_NONE;
}

void tTxDispTFT::load_rx_list(void)
{
    rx_list.clear();
    for (uint8_t i = 0; i < SETUP_PARAMETER_NUM; i++)
    {
        if (setup_param_is_rx(i))
            rx_list.add(i);
    }
}
tTxDispTFT::tParamList *tTxDispTFT::current_list(void)
{
    switch (page)
    {
    case PAGE_COMMON:
        return &common_list;
    case PAGE_TX:
        return &tx_list;
    case PAGE_RX:
        return &rx_list;
    }
    return nullptr;
}

//==================== Tick_ms ====================
void tTxDispTFT::Tick_ms(void)
{
    uint16_t keys, i, keys_new;
    if (!initialized)
        return;
    DECc(keys_tick, KEYS_DEBOUNCE_TMO_MS / 4);
    if (!keys_tick)
    {
        keys = fiveway_read();
        keys_has_been_pressed = 0;
        i = keys_state ^ keys;
        keys_ct0 = ~(keys_ct0 & i);
        keys_ct1 = keys_ct0 ^ (keys_ct1 & i);
        i &= keys_ct0 & keys_ct1;
        keys_state ^= i;
        keys_new = keys_state & i;
        keys_has_been_pressed |= keys_new;
    }
    if (!keys_state)
        keys_pressed_tmo = 0;
    uint8_t fm = (1 << KEY_DOWN) | (1 << KEY_UP);
    if (idx_focused_in_edit)
        fm = (1 << KEY_LEFT) | (1 << KEY_RIGHT);
    if (keys_has_been_pressed & fm)
        keys_pressed_tmo = SYSTICK_DELAY_MS(750);
    if (keys_pressed_tmo)
    {
        keys_pressed_tmo--;
        if (!keys_pressed_tmo && (keys_state & fm))
        {
            keys_pressed_tmo = SYSTICK_DELAY_MS(175);
            keys_has_been_pressed = keys_state & fm;
        }
    }
    if (page == PAGE_STARTUP && page_startup_tmo)
    {
        page_startup_tmo--;
        if (!page_startup_tmo)
        {
            page = PAGE_MAIN;
            subpage = SUBPAGE_DEFAULT;
            subpage_max = SUBPAGE_MAIN_NUM - 1;
            page_modified = true;
        }
        return;
    }
    if (page == PAGE_NOTIFY_BIND) {
        DECc(bind_tmo, SYSTICK_DELAY_MS(1000));
        if (!bind_tmo || key_has_been_pressed(KEY_RIGHT)) {
            bind.StopBind();
            page = PAGE_MAIN;
            subpage = SUBPAGE_DEFAULT;
            subpage_max = SUBPAGE_MAIN_NUM - 1;
            page_modified = true;
        }
        if (!bind.IsInBind()) {
            page = PAGE_MAIN;
            subpage = SUBPAGE_DEFAULT;
            subpage_max = SUBPAGE_MAIN_NUM - 1;
            page_modified = true;
        }
        return;
    }
    if (page == PAGE_NOTIFY_STORE)
        return;
    if (connected_last != connected())
        page_modified = true;
    connected_last = connected();
    if (setupmetadata_rx_available_last != SetupMetaData.rx_available) {
        page_modified = true;
        if (SetupMetaData.rx_available) load_rx_list();
    }
    setupmetadata_rx_available_last = SetupMetaData.rx_available;
    if (!connected() && page == PAGE_RX)
    {
        idx_first = 0;
        idx_focused = 0;
        idx_focused_in_edit = false;
        idx_focused_pos = 0;
        keys_has_been_pressed &= ((1 << KEY_RIGHT) | (1 << KEY_LEFT));
    }
    if (!idx_focused_in_edit)
    {
        if (key_has_been_pressed(KEY_RIGHT))
        {
            if (page >= PAGE_NAV_MIN && page <= PAGE_NAV_MAX)
            {
                page++;
                if (page > PAGE_NAV_MAX) page = PAGE_NAV_MIN;
                page_init();
                page_modified = true;
            }
        }
        else if (key_has_been_pressed(KEY_LEFT))
        {
            if (page >= PAGE_NAV_MIN && page <= PAGE_NAV_MAX)
            {
                if (page == PAGE_NAV_MIN) page = PAGE_NAV_MAX;
                else page--;
                page_init();
                page_modified = true;
            }
        }
        else if (key_has_been_pressed(KEY_DOWN))
        {
            idx_focused++;
            if (idx_focused > idx_max) idx_focused = 0;
            if (idx_focused - idx_first > (5 - 1))
                idx_first = idx_focused - (5 - 1);
            if (idx_focused < idx_first)
                idx_first = 0;
            page_modified = true;
            page_modified = true;
            subpage++;
            if (subpage > subpage_max) subpage = 0;
            page_modified = true;
        }
        else if (key_has_been_pressed(KEY_UP))
        {
            if (idx_focused == 0) {
                idx_focused = idx_max;
                if (idx_focused - idx_first > (5 - 1))
                    idx_first = idx_focused - (5 - 1);
                if (idx_focused < idx_first)
                    idx_first = 0;
            } else {
                idx_focused--;
                if (idx_focused < idx_first)
                    idx_first = idx_focused;
            }
            page_modified = true;
            if (subpage == 0) subpage = subpage_max;
            else subpage--;
            page_modified = true;
        }
        else if (key_has_been_pressed(KEY_CENTER))
        {
            if (page >= PAGE_COMMON && page <= PAGE_RX)
            {
                if (current_list()->allowed_num[idx_focused] > 1)
                {
                    idx_focused_in_edit = true;
                    idx_focused_pos = 0;
                    page_modified = true;
                }
            }
            else if (page == PAGE_ACTIONS)
                run_action();
        }
    }
    else
    {
        if (edit_setting())
        {
            idx_focused_in_edit = false;
            page_modified = true;
            if (edit_setting_task_pending != MAIN_TASK_NONE)
                tasks.SetDisplayTask(edit_setting_task_pending);
            edit_setting_task_pending = MAIN_TASK_NONE;
        }
    }
}

void tTxDispTFT::page_init(void)
{
    idx_first = 0;
    idx_focused = 0;
    idx_focused_pos = 0;
    idx_max = 0;
    switch (page)
    {
    case PAGE_COMMON:
        idx_max = common_list.num - 1;
        break;
    case PAGE_TX:
        idx_max = tx_list.num - 1;
        break;
    case PAGE_RX:
        idx_max = rx_list.num - 1;
        break;
    case PAGE_ACTIONS:
        idx_max = DISP_ACTION_NUM - 1;
        break;
    }
    subpage = SUBPAGE_DEFAULT;
    subpage_max = 0;
    if (page == PAGE_MAIN)
        subpage_max = SUBPAGE_MAIN_NUM - 1;
}

void tTxDispTFT::run_action(void)
{
    if (idx_focused >= DISP_ACTION_NUM)
        while (1)
        {
        }
    switch (disp_actions[idx_focused])
    {
    case DISP_ACTION_STORE:
        page = PAGE_NOTIFY_STORE;
        page_modified = true;
        tasks.SetDisplayTask(TX_TASK_PARAM_STORE);
        break;
    case DISP_ACTION_BIND:
        tasks.SetDisplayTask(MAIN_TASK_BIND_START);
        break;
    case DISP_ACTION_BOOT:
        tasks.SetDisplayTask(MAIN_TASK_SYSTEM_BOOT);
        break;
    case DISP_ACTION_FLASH_ESP:
        tasks.SetDisplayTask(TX_TASK_FLASH_ESP);
        break;
    }
}

void tTxDispTFT::UpdateMain(void)
{
    if (page == PAGE_MAIN)
        page_update = true;
}
void tTxDispTFT::SetBind(void)
{
    if (page == PAGE_NOTIFY_BIND)
        return;
    page = PAGE_NOTIFY_BIND;
    bind_tmo = SYSTICK_DELAY_MS(10000); // 10秒超时
    page_modified = true;
}
void tTxDispTFT::DrawNotify(const char *const s)
{
    if (!initialized)
        return;
    page_modified = true;
    draw_page_notify(s);
    page_modified = false;
}
void tTxDispTFT::DrawBoot(void)
{
    DrawNotify("BOOT");
    delay_ms(250);
}

void tTxDispTFT::Draw(void)
{
    if (!initialized)
        return;
    if (page_modified || page_update)
    {
        switch (page)
        {
        case PAGE_STARTUP:
            draw_page_startup();
            break;
        case PAGE_MAIN:
            draw_page_main();
            break;
        case PAGE_COMMON:
            draw_page_common();
            break;
        case PAGE_TX:
            draw_page_tx();
            break;
        case PAGE_RX:
            draw_page_rx();
            break;
        case PAGE_ACTIONS:
            draw_page_actions();
            break;
        case PAGE_NOTIFY_BIND:
            draw_page_notify("BINDING");
            break;
        case PAGE_NOTIFY_STORE:
            draw_page_notify("STORE");
            break;
        }
        page_modified = false;
        page_update = false;
    }
}



bool tTxDispTFT::key_has_been_pressed(uint8_t key_idx)
{
    bool p = (keys_has_been_pressed & (1 << key_idx));
    keys_has_been_pressed &= ~(1 << key_idx);
    return p;
}

void _diversity_str(char *s, uint8_t div)
{
    switch (div)
    {
    case DIVERSITY_DEFAULT:
        strcpy(s, "en.");
        return;
    case DIVERSITY_ANTENNA1:
        strcpy(s, "ant1");
        return;
    case DIVERSITY_ANTENNA2:
        strcpy(s, "ant2");
        return;
    case DIVERSITY_R_ENABLED_T_ANTENNA1:
        strcpy(s, "re,t1");
        return;
    case DIVERSITY_R_ENABLED_T_ANTENNA2:
        strcpy(s, "re,t2");
        return;
    }
    strcpy(s, "?");
}

void _draw_dot2(uint8_t x, uint8_t y)
{
    tft.drawPixel(x, y - 1, TFT_FG_COLOR);
    tft.drawLine(x - 1, y, x + 1, y, TFT_FG_COLOR);
    tft.drawPixel(x, y + 1, TFT_FG_COLOR);
}

//==================== draw_header ====================
void tTxDispTFT::draw_header(const char *const s)
{
    tft.fillScreen(TFT_BG_COLOR);
    _setcurXY(8, 8);
    _putc('0' + Config.ConfigId);
    tft.drawLine(17, 1, 17, 17, TFT_FG_COLOR);
    _setcurXY(21, 8);
    _puts(s);
    tft.drawLine(0, 18, tft.width() - 1, 18, TFT_FG_COLOR);
}

//==================== draw_options ====================
void tTxDispTFT::draw_options(tParamList *const list)
{
    char s[32];
    for (uint8_t idx = 0; idx < list->num; idx++)
    {
        if (idx < idx_first)
            continue;
        if (idx - idx_first >= 5)
            break;
        uint8_t param_idx = list->list[idx];
        int16_t row_y = (idx - idx_first) * 10 + DISP_CONTENT_Y_BASE;
        if (idx == idx_focused)
            _setinverted();
        if (setup_param_is_tx(param_idx) || setup_param_is_rx(param_idx))
            strcpy(s, SetupParameter[param_idx].name + 3);
        else
            strcpy(s, SetupParameter[param_idx].name);
        s[13] = 0;
        _setcurXY(8, row_y);
        _puts(s);
        _unsetinverted();
        int16_t val_x = tft.width() - 1 - 7 * 6;
        strcpy(s, "ups ?");
        if (SetupParameter[param_idx].type == SETUP_PARAM_TYPE_STR6)
        {
            param_get_val_formattedstr(s, param_idx, PARAM_FORMAT_DISPLAY);
            _setcurX(val_x);
            _puts(s);
            if ((idx == idx_focused) && idx_focused_in_edit) {
                _setcurX(val_x + idx_focused_pos * 6);
                _setinverted();
                _putc(s[idx_focused_pos]);
                _unsetinverted();
            }
        }
        else
        {
            if (idx == idx_focused && idx_focused_in_edit)
                _setinverted();
            if (list->allowed_num[idx] == 0)
                strcpy(s, "-");
            else
            {
                param_get_val_formattedstr(s, param_idx, PARAM_FORMAT_DISPLAY);
                if (!strncmp(s, "antenna", 7))
                {
                    s[3] = s[7];
                    s[4] = 0;
                }
            }
            s[7] = 0;
            _setcurX(val_x);
            _puts(s);
        }
        _unsetinverted();
    }
}

//==================== pages ====================
void tTxDispTFT::draw_page_startup(void)
{
    if (!page_modified)
        return;
    tft.fillScreen(TFT_BG_COLOR);
    tft.setTextSize(4);
    _fontSize = 4;
    _setcurY(10);
    _puts_XCentered("mLRS");
    tft.setTextSize(1);
    _fontSize = 1;
    _setcurY(52);
    _puts_XCentered(DEVICE_NAME);
    _setcurY(63);
    _puts_XCentered(VERSIONONLYSTR);
}

void tTxDispTFT::draw_page_notify(const char *const s)
{
    if (!page_modified)
        return;
    tft.fillScreen(TFT_BG_COLOR);
    uint8_t len = strlen(s);
    if (len < 9)
    {
        tft.setTextSize(3);
        _fontSize = 3;
        _setcurY(27);
        _puts_XCentered(s);
    }
    else
    {
        tft.setTextSize(2);
        _fontSize = 2;
        _setcurXY(27, 30);
        _puts(s);
    }
    tft.setTextSize(1);
    _fontSize = 1;
}

void tTxDispTFT::draw_page_main_sub0(void)
{
    char s[32];
    int8_t power;
    draw_header("Main");
    param_get_val_formattedstr(s, PARAM_INDEX_MODE, PARAM_FORMAT_DISPLAY);
    if (strlen(s) > 5)
        _setcurXY(51, 8);
    else
        _setcurXY(58, 8);
    _puts(s);
    _setcurX(99);
    power = sx.RfPower_dbm();
    if (power >= -9)
    {
        stoBCDstr(power, s);
        _puts(s);
    }
    else
        _puts("-\x7F");
    _setcurX(114);
    if (connected_and_rx_setup_available())
    {
        power = SetupMetaData.rx_actual_power_dbm;
        if (power >= -9)
        {
            stoBCDstr(power, s);
            _puts(s);
        }
        else
            _puts("-\x7F");
    }
    _setcurX(connected_and_rx_setup_available() ? 133 : 123);
    _puts("dB");
    _setcurXY(8, 0 * 10 + DISP_CONTENT_Y_BASE);
    _puts("Rssi");
    _setcurXY(123, 1 * 10 + DISP_CONTENT_Y_BASE + 5);
    _puts("dB");
    _setcurXY(8, 3 * 10 + DISP_CONTENT_Y_BASE - 4);
    _puts("LQ");
    _setcurXY(123 + 6, 4 * 10 + DISP_CONTENT_Y_BASE + 1);
    _puts("%");
    tft.setTextSize(2);
    _fontSize = 2;
    _setcurXY(13, 1 * 10 + DISP_CONTENT_Y_BASE + 5);
    s8toBCDstr(stats.GetLastRssi(), s);
    _puts(s);
    _setcurX(68);
    s8toBCDstr(stats.received_rssi, s);
    if (connected())
        _puts(s);
    _setcurXY(13 + 11, 4 * 10 + DISP_CONTENT_Y_BASE + 1);
    stoBCDstr(stats.GetLQ_serial(), s);
    _puts(s);
    _setcurX(68 + 11);
    if (connected())
    {
        stoBCDstr(stats.GetReceivedLQ_rc(), s);
        _puts(s);
    }
    tft.setTextSize(1);
    _fontSize = 1;
}

void tTxDispTFT::draw_page_main_sub1(void)
{
    char s[32];
    draw_header("Main/2");
    _setcurXY(0, 0 * 10 + DISP_CONTENT_Y_BASE);
    _puts("Mode");
    _setcurX(40);
    param_get_val_formattedstr(s, PARAM_INDEX_MODE);
    _puts(s);
    _setcurX(80 + 5);
    stoBCDstr(sx.ReceiverSensitivity_dbm(), s);
    _puts(s);
    _puts(" dB");
    _setcurXY(0, 1 * 10 + DISP_CONTENT_Y_BASE);
    _puts("Power");
    _setcurX(40);
    stoBCDstr(sx.RfPower_dbm(), s);
    _puts(s);
    _setcurX(80);
    stoBCDstr(SetupMetaData.rx_actual_power_dbm, s);
    if (connected_and_rx_setup_available())
        _puts(s);
    _setcurX(115);
    _puts("dB");
    _setcurXY(0, 2 * 10 + DISP_CONTENT_Y_BASE);
    _puts("Div.");
    _setcurX(40);
    _diversity_str(s, Config.Diversity);
    _puts(s);
    _setcurX(80);
    uint8_t rd = (SetupMetaData.rx_available) ? SetupMetaData.rx_actual_diversity : DIVERSITY_NUM;
    _diversity_str(s, rd);
    if (connected_and_rx_setup_available())
        _puts(s);
}

void tTxDispTFT::draw_page_main_sub2(void)
{
    char s[32];
    if (page_modified)
    {
        draw_header("Main/3");
        _setcurXY(5, 0 * 10 + DISP_CONTENT_Y_BASE);
        _puts("Tx");
        _setcurX(110);
        _puts("Rx");
        _setcurXY(92, 2 * 10 + DISP_CONTENT_Y_BASE);
        _putc('>');
        tft.drawLine(32, 2 * 10 + DISP_CONTENT_Y_BASE + 4, 94, 2 * 10 + DISP_CONTENT_Y_BASE + 4, TFT_FG_COLOR);
        _setcurXY(28, 3 * 10 + DISP_CONTENT_Y_BASE);
        _putc('<');
        tft.drawLine(32, 3 * 10 + DISP_CONTENT_Y_BASE + 4, 94, 3 * 10 + DISP_CONTENT_Y_BASE + 4, TFT_FG_COLOR);
        if (Config.Diversity == DIVERSITY_DEFAULT && !Config.IsDualBand)
            _draw_dot2(1, 2 * 10 + DISP_CONTENT_Y_BASE - 3);
        else if (Config.IsDualBand)
        {
            _setcurXY(4, 2 * 10 + DISP_CONTENT_Y_BASE);
            _puts("a12");
        }
        if (SetupMetaData.rx_available && (SetupMetaData.rx_actual_diversity == DIVERSITY_DEFAULT || SetupMetaData.rx_actual_diversity == DIVERSITY_R_ENABLED_T_ANTENNA1 || SetupMetaData.rx_actual_diversity == DIVERSITY_R_ENABLED_T_ANTENNA2))
            _draw_dot2(125, 2 * 10 + DISP_CONTENT_Y_BASE - 3);
        if (Config.Diversity == DIVERSITY_DEFAULT || Config.Diversity == DIVERSITY_R_ENABLED_T_ANTENNA1 || Config.Diversity == DIVERSITY_R_ENABLED_T_ANTENNA2)
            _draw_dot2(1, 3 * 10 + DISP_CONTENT_Y_BASE - 3);
        if (SetupMetaData.rx_available && SetupMetaData.rx_actual_diversity == DIVERSITY_DEFAULT && !Config.IsDualBand)
            _draw_dot2(125, 3 * 10 + DISP_CONTENT_Y_BASE - 3);
        else if (Config.IsDualBand)
        {
            _setcurXY(105, 3 * 10 + DISP_CONTENT_Y_BASE);
            _puts("a12");
        }
        _setcurXY(70, 1 * 10 + DISP_CONTENT_Y_BASE);
        _puts("Bps");
        _setcurXY(70, 4 * 10 + DISP_CONTENT_Y_BASE);
        _puts("Bps");
    }
    tft.setTextColor(TFT_FG_COLOR, TFT_BG_COLOR);
    uint8_t tra = stats.last_antenna, tta = stats.last_transmit_antenna, rra = stats.received_antenna, rta = stats.received_transmit_antenna;
    _setcurXY(10, 2 * 10 + DISP_CONTENT_Y_BASE);
    if (!Config.IsDualBand)
        _puts((tta == ANTENNA_2) ? "a2" : "a1");
    _setcurX(105);
    _puts((rra == ANTENNA_2) ? "a2" : "a1");
    _setcurXY(10, 3 * 10 + DISP_CONTENT_Y_BASE);
    _puts((tra == ANTENNA_2) ? "a2" : "a1");
    _setcurX(105);
    if (!Config.IsDualBand)
        _puts((rta == ANTENNA_2) ? "a2" : "a1");
    uint16_t bt = stats.bytes_transmitted.GetBytesPerSec(), br = stats.bytes_received.GetBytesPerSec();
    _setcurXY(40, 1 * 10 + DISP_CONTENT_Y_BASE);
    utoBCDstr(bt, s);
    for (uint8_t i = 0; i < 4; i++)
    {
        if (s[i] == 0)
        {
            s[i] = ' ';
            s[i + 1] = 0;
        }
    }
    _puts(s);
    _setcurXY(40, 4 * 10 + DISP_CONTENT_Y_BASE);
    utoBCDstr(br, s);
    for (uint8_t i = 0; i < 4; i++)
    {
        if (s[i] == 0)
        {
            s[i] = ' ';
            s[i + 1] = 0;
        }
    }
    _puts(s);
    tft.setTextColor(TFT_FG_COLOR);
}

void tTxDispTFT::draw_page_main_sub3(void)
{
    char s[32];
    draw_header("Main/4");
    _setcurXY(0, 0 * 10 + DISP_CONTENT_Y_BASE);
    _puts(DEVICE_NAME);
    _setcurXY(0, 1 * 10 + DISP_CONTENT_Y_BASE);
    _puts(VERSIONONLYSTR);
#if !(defined USE_ESP_WIFI_BRIDGE_CONFIGURE || defined USE_HC04_MODULE)
    if (info.WirelessDeviceName_disp(s))
    {
        _setcurXY(60, 1 * 10 + DISP_CONTENT_Y_BASE);
        _puts(s);
    }
#endif
    if (connected_and_rx_setup_available())
    {
        _setcurXY(0, 3 * 10 + DISP_CONTENT_Y_BASE);
        _puts(SetupMetaData.rx_device_name);
        _setcurXY(0, 4 * 10 + DISP_CONTENT_Y_BASE);
        version_to_str(s, SetupMetaData.rx_firmware_version);
        _puts(s);
    }
}

void tTxDispTFT::draw_page_main_sub4(void)
{
#if defined USE_ESP_WIFI_BRIDGE_CONFIGURE || defined USE_HC04_MODULE
    char s[48];
    uint8_t yofs = 0;
    draw_header("Main/5");
    _setcurXY(0, 0 * 10 + DISP_CONTENT_Y_BASE);
    if (!info.WirelessDeviceName_cli(s))
    {
        _puts("wireless bridge");
        _setcurXY(0, 1 * 10 + DISP_CONTENT_Y_BASE);
        _puts("not available/enabled");
        return;
    }
    _puts(s);
    yofs = (strlen(s) > 21) ? 1 : 0;
    _setcurXY(0, (1 + yofs) * 10 + DISP_CONTENT_Y_BASE);
#endif
}

void tTxDispTFT::draw_page_main(void)
{
    switch (subpage)
    {
    case SUBPAGE_MAIN_SUB1:
        if (!page_modified)
            return;
        draw_page_main_sub1();
        return;
    case SUBPAGE_MAIN_SUB2:
        draw_page_main_sub2();
        return;
    case SUBPAGE_MAIN_SUB3:
        if (!page_modified)
            return;
        draw_page_main_sub3();
        return;
#ifdef USE_ESP_WIFI_BRIDGE_CONFIGURE
    case SUBPAGE_MAIN_SUB4:
        if (!page_modified)
            return;
        draw_page_main_sub4();
        return;
#endif
    default:
        draw_page_main_sub0();
    }
}

void tTxDispTFT::draw_page_common(void)
{
    if (!page_modified)
        return;
    draw_header("Common");
    draw_options(&common_list);
    char es[8];
    if (except_str_from_bindphrase(es, Setup.Common[Config.ConfigId].BindPhrase, Config.FrequencyBand))
    {
        _setcurXY(0, 4 * 10 + DISP_CONTENT_Y_BASE);
        _puts("except ");
        _puts(es);
    }
}

void tTxDispTFT::draw_page_tx(void)
{
    if (!page_modified)
        return;
    draw_header("Tx");
    draw_options(&tx_list);
}

void tTxDispTFT::draw_page_rx(void)
{
    if (!page_modified)
        return;
    draw_header("Rx");
    if (!connected())
    {
        _setcurY(DISP_CONTENT_Y_BASE + 10);
        _puts_XCentered("not connected!");
        return;
    }
    draw_options(&rx_list);
}

void tTxDispTFT::draw_page_actions(void)
{
    if (!page_modified)
        return;
    draw_header("Actions");
    tft.setTextSize(2);
    _fontSize = 2;
    uint8_t idx = 0;
    _setcurXY(5, idx * 16 + DISP_CONTENT_Y_BASE + 5);
    if (idx == idx_focused)
        _setinverted();
    _puts("STORE");
    _unsetinverted();
    idx++;
    _setcurXY(5, idx * 16 + DISP_CONTENT_Y_BASE + 5);
    if (idx == idx_focused)
        _setinverted();
    _puts("BIND");
    _unsetinverted();
    idx++;
    if ((idx < DISP_ACTION_NUM) && (disp_actions[idx] == DISP_ACTION_FLASH_ESP))
    {
        _setcurXY(5, idx * 16 + DISP_CONTENT_Y_BASE + 5);
        if (idx == idx_focused)
            _setinverted();
        _puts("FLASH ");
        _movecurX(-2);
        _puts("ESP");
        _unsetinverted();
        idx++;
    }
    tft.setTextSize(1);
    _fontSize = 1;
    if ((idx < DISP_ACTION_NUM) && (disp_actions[idx] == DISP_ACTION_BOOT))
    {
        _setcurXY(75, 0 * 11 + DISP_CONTENT_Y_BASE + 5);
        if (idx == idx_focused)
            _setinverted();
        _puts("BOOT");
        _unsetinverted();
    }
}

bool tTxDispTFT::edit_setting(void)
{
    if (!keys_has_been_pressed)
        return false;
    tParamList *list = current_list();
    uint8_t param_idx = list->list[idx_focused];
    if (key_has_been_pressed(KEY_CENTER))
    {
        if (SetupParameter[param_idx].type == SETUP_PARAM_TYPE_STR6)
        {
            idx_focused_pos++;
            page_modified = true;
            return (idx_focused_pos >= 6);
        }
        return true;
    }
    tParamValue vv;
    bool rxc = false;
    if (key_has_been_pressed(KEY_RIGHT))
    {
        if (SetupParameter[param_idx].type == SETUP_PARAM_TYPE_INT8)
        {
            int8_t v = *(int8_t *)(SetupParameterPtr(param_idx));
            if (v < SetupParameter[param_idx].max.INT8_value)
            {
                vv.i8 = v + 1;
                rxc = setup_set_param(param_idx, vv);
                page_modified = true;
            }
        }
        else if (SetupParameter[param_idx].type == SETUP_PARAM_TYPE_LIST)
        {
            uint8_t v = *(uint8_t *)(SetupParameterPtr(param_idx)), vmax = param_get_opt_num(param_idx);
            while (v < vmax)
            {
                v++;
                if (param_get_allowed_mask(param_idx) & (1 << v))
                    break;
            }
            if (v < vmax)
            {
                vv.u8 = v;
                rxc = setup_set_param(param_idx, vv);
                page_modified = true;
            }
        }
        else if (SetupParameter[param_idx].type == SETUP_PARAM_TYPE_STR6)
        {
            char c = ((char *)SetupParameterPtr(param_idx))[idx_focused_pos];
            const char *vp = strchr(bindphrase_chars, c);
            if (!vp)
                while (1)
                {
                }
            vp++;
            if (vp >= bindphrase_chars + BINDPHRASE_CHARS_LEN)
                vp = bindphrase_chars;
            ((char *)SetupParameterPtr(param_idx))[idx_focused_pos] = *vp;
            rxc = true;
            page_modified = true;
        }
    }
    else if (key_has_been_pressed(KEY_LEFT))
    {
        if (SetupParameter[param_idx].type == SETUP_PARAM_TYPE_INT8)
        {
            int8_t v = *(int8_t *)(SetupParameterPtr(param_idx));
            if (v > SetupParameter[param_idx].min.INT8_value)
            {
                vv.i8 = v - 1;
                rxc = setup_set_param(param_idx, vv);
                page_modified = true;
            }
        }
        else if (SetupParameter[param_idx].type == SETUP_PARAM_TYPE_LIST)
        {
            uint8_t v = *(uint8_t *)(SetupParameterPtr(param_idx));
            while (1)
            {
                if (v == 0)
                {
                    v = UINT8_MAX;
                    break;
                }
                v--;
                if (param_get_allowed_mask(param_idx) & (1 << v))
                    break;
            }
            if (v != UINT8_MAX)
            {
                vv.u8 = v;
                rxc = setup_set_param(param_idx, vv);
                page_modified = true;
            }
        }
        else if (SetupParameter[param_idx].type == SETUP_PARAM_TYPE_STR6)
        {
            char c = ((char *)SetupParameterPtr(param_idx))[idx_focused_pos];
            const char *vp = strchr(bindphrase_chars, c);
            if (!vp)
                while (1)
                {
                }
            vp--;
            if (vp < bindphrase_chars)
                vp = bindphrase_chars + BINDPHRASE_CHARS_LEN - 1;
            ((char *)SetupParameterPtr(param_idx))[idx_focused_pos] = *vp;
            rxc = true;
            page_modified = true;
        }
    }
    if (rxc)
        edit_setting_task_pending = TX_TASK_RX_PARAM_SET;
    return false;
}

#endif // USE_TFTDISPLAY
#endif // DISPTFT_H