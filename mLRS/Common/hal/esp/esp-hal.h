//*******************************************************
// Copyright (c) MLRS project
// GPL3
// https://www.gnu.org/licenses/gpl-3.0.de.html
// OlliW @ www.olliw.eu
//*******************************************************
// rx hal splicer for ESP targets
//*******************************************************

//-------------------------------------------------------
// ESP Boards
//-------------------------------------------------------

//-- ELRS 868/915 MHz Generic Devices

#ifdef RX_ELRS_GENERIC_900_ESP8285
#include "rx-hal-generic-900-esp8285.h"
#endif

#ifdef RX_ELRS_GENERIC_900_PA_ESP8285
#include "rx-hal-generic-900-pa-esp8285.h"
#endif

#ifdef RX_ELRS_GENERIC_900_TD_PA_ESP32
#include "rx-hal-generic-900-td-pa-esp32.h"
#endif

#ifdef RX_ELRS_GENERIC_LR1121_TD_ESP32
#include "rx-hal-generic-lr1121-td-esp32.h"
#endif

#ifdef RX_ELRS_GENERIC_C3_LR1121_ESP32C3
#include "rx-hal-generic-c3-lr1121-esp32c3.h"
#endif

//-- Generic 868/915 MHz ELRS devices with overlays or other variations

#ifdef RX_ELRS_RADIOMASTER_BR3_900_ESP8285
#include "rx-hal-radiomaster-br3-900-esp8285.h"
#endif

#ifdef RX_ELRS_IFLIGHT_ELRS_TD_900_ESP32
#include "rx-hal-generic-900-td-pa-esp32.h"
#endif


//-- ELRS 2.4 GHz Generic Devices

#ifdef RX_ELRS_GENERIC_2400_ESP8285
#include "rx-hal-generic-2400-esp8285.h"
#endif

#ifdef RX_ELRS_GENERIC_2400_PA_ESP8285
#include "rx-hal-generic-2400-pa-esp8285.h"
#endif

#ifdef RX_ELRS_GENERIC_2400_D_PA_ESP8285
#include "rx-hal-generic-2400-d-pa-esp8285.h"
#endif

#ifdef RX_ELRS_GENERIC_2400_TD_PA_ESP32
#include "rx-hal-generic-2400-td-pa-esp32.h"
#endif

//-- Generic 2.4 GHz ELRS boards with overlays or other variations

#ifdef RX_ELRS_IFLIGHT_ELRS_TD_2400_ESP32
#include "rx-hal-iflight-elrs-td-2400-esp32.h"
#endif

#ifdef RX_ELRS_IFLIGHT_ELRS_2400_ESP8285
#include "rx-hal-iflight-elrs-2400-esp8285.h"
#endif

// DAKEFPV 900MHz RX
#ifdef RX_ELRS_DAKEFPV_900_ESP32C3
#include "rx-hal-generic-c3-lr1121-esp32c3.h"
#endif

//DAKEFPV 2400MHz RX
#ifdef RX_ELRS_DAKEFPV_2400_ESP32C3
#include "rx-hal-generic-c3-lr1121-esp32c3.h"
#endif

// DAKEFPV双频接收器
#ifdef RX_ELRS_DAKEFPV_LR1121_TD_ESP32C3
#include "rx-hal-dakefpv-lr1121-td-esp32c3.h"
#endif 

// DAKEFPV_BR3系列
#ifdef RX_ELRS_DAKEFPV_LR1121_915PRO_ESP32C3
#include "rx-hal-dakefpv-900pro-lr1121-esp32c3.h"
#endif

// DAKEFPV 2.4G高功率接收器配置
#ifdef RX_ELRS_DAKE2G4_NANOPRO_LR1121_ESP32C3
#include "rx-hal-dkfpv-c3-lr1121-esp32c3.h"
#endif

// DAKEFPV高功率接收器配置 superdpro系列
#ifdef RX_ELRS_DAKEFPV_SUPERDPRO_2400_ESP32C3
#include "rx-hal-dakefpv-lr1121-td-esp32c3.h"
#endif

// DAKEFPV高功率接收器配置 900MHz系列
#ifdef RX_ELRS_DAKEFPV_SUPERDPRO_900_ESP32C3
#include "rx-hal-dakefpv-lr1121-td-esp32c3.h"
#endif

//-- ELRS Selected Devices

#ifdef RX_ELRS_BAYCK_NANO_PRO_900_ESP8285
#include "rx-hal-generic-900-pa-esp8285.h"
#endif

#ifdef RX_ELRS_SPEEDYBEE_NANO_2400_ESP8285
#include "rx-hal-generic-2400-pa-esp8285.h"
#endif

#ifdef RX_ELRS_RADIOMASTER_RP4TD_2400_ESP32
#include "rx-hal-radiomaster-rp4td-2400-esp32.h"
#endif

#ifdef RX_ELRS_BETAFPV_SUPERD_2400_ESP32
#include "rx-hal-generic-2400-td-pa-esp32.h"
#endif

#ifdef RX_ELRS_RADIOMASTER_XR4_ESP32
#include "rx-hal-radiomaster-xr4-esp32.h"
#endif

#ifdef RX_ELRS_RADIOMASTER_XR1_ESP32C3
#include "rx-hal-radiomaster-xr1-esp32c3.h"
#endif

#ifdef TX_ELRS_RADIOMASTER_RP4TD_2400_ESP32
#include "tx-hal-radiomaster-rp4td-2400-esp32.h"
#endif

#ifdef TX_ELRS_RADIOMASTER_INTERNAL_2400_ESP32
#include "tx-hal-radiomaster-int-2400-esp32.h"
#endif

#ifdef TX_ELRS_RADIOMASTER_INTERNAL_BOXER_2400_ESP32
#include "tx-hal-radiomaster-int-boxer-2400-esp32.h"
#endif

#ifdef TX_ELRS_JUMPER_INTERNAL_2400_ESP32
#include "tx-hal-jumper-int-2400-esp32.h"
#endif

#ifdef TX_ELRS_JUMPER_INTERNAL_900_ESP32
#include "tx-hal-jumper-int-900-esp32.h"
#endif

#ifdef TX_ELRS_RADIOMASTER_INTERNAL_TX15_ESP32
#include "tx-hal-radiomaster-int-tx15-esp32.h"
#endif

#ifdef TX_ELRS_RADIOMASTER_INTERNAL_GX12_ESP32
#include "tx-hal-radiomaster-int-gx12-esp32.h"
#endif

#ifdef TX_ELRS_RADIOMASTER_INTERNAL_TX16SMK3_ESP32
#include "tx-hal-radiomaster-int-tx16smk3-esp32.h"
#endif

#ifdef TX_ELRS_RADIOMASTER_INTERNAL_AX12_ESP32
#include "tx-hal-radiomaster-int-ax12-esp32.h"
#endif

#ifdef TX_ELRS_BETAFPV_MICRO_1W_2400_ESP32
#include "tx-hal-betafpv-micro-1w-2400-esp32.h"
#endif

#ifdef TX_ELRS_RADIOMASTER_BANDIT_MICRO_900_ESP32
#include "tx-hal-radiomaster-bandit-series-900-esp32.h"
#endif

#ifdef TX_ELRS_RADIOMASTER_BANDIT_900_ESP32
#include "tx-hal-radiomaster-bandit-series-900-esp32.h"
#endif

#ifdef TX_ELRS_RADIOMASTER_RANGER_2400_ESP32
#include "tx-hal-radiomaster-ranger-2400-esp32.h"
#endif

#ifdef TX_ELRS_RADIOMASTER_NOMAD_ESP32
#include "tx-hal-radiomaster-nomad-esp32.h"
#endif

#ifdef TX_ELRS_FLYSKY_INTERNAL_PA01_2400_ESP32S3
#include "tx-hal-flysky-int-pa01-2400-esp32s3.h"
#endif

//DAKEFPV 2G4/900MHz 1W TX
#ifdef  TX_ELRS_DAKEFPV_2G4_ESP32
#include "tx-hal-dakefpv-2g4-esp32.h"
#endif

//DAKEFPV 900MHz 2W TX
#ifdef  TX_ELRS_DAKEFPV_900_ESP32
#include "tx-hal-dakefpv-9002w-esp32.h"
#endif