/*
 * Copyright (C) 2020 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define MTK_LOG_ENABLE 1

#include <sys/types.h>
#include <stdbool.h>
#include <string.h>

#include <cutils/properties.h>
#include <cutils/log.h>

#include "fmr.h"
#include "fmlib_cust.h"

#undef LOGV
#define LOGV(...) ALOGV(__VA_ARGS__)
#undef LOGD
#define LOGD(...) ALOGD(__VA_ARGS__)
#undef LOGI
#define LOGI(...) ALOGI(__VA_ARGS__)
#undef LOGW
#define LOGW(...) ALOGW(__VA_ARGS__)
#undef LOGE
#define LOGE(...) ALOGE(__VA_ARGS__)
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "FMLIB_CUST"

#define MT6620_FM_FAKE_CHANNEL \
{ \
    {10400, -40, -1}, \
    {9100, -40, -1},  \
    {9600, -40, -1},  \
    {9220, -80, -1},  \
    {0, 0, 0} \
}

#define MT6628_FM_FAKE_CHANNEL \
{ \
    {0, 0, 0} \
}

#define MT6627_FM_FAKE_CHANNEL \
{ \
    {9600, -107, -1},  \
    {10400, -107, -1}, \
    {10750, -224, -1}, \
    {0, 0, 0} \
}
#define MT6580_FM_FAKE_CHANNEL \
{ \
    {9600, -107, -1},  \
    {9570, -258, -1},  \
    {9580, -258, -1},  \
    {9590, -258, -1},  \
    {10400, -107, -1}, \
    {0, 0, 0} \
}

#define MT6630_FM_FAKE_CHANNEL \
{ \
    {9600,-107,-1},  \
    {10400,-107,-1}, \
    {0, 0, 0} \
}

#define MT6631_FM_FAKE_CHANNEL \
{ \
    {9210, -205, -1},  \
    {9600, -152, -1},  \
    {10400, -152, -1}, \
    {10750, -205, -1}, \
    {0, 0, 0} \
}

#define MT6632_FM_FAKE_CHANNEL \
{ \
    {9600, -107, -1},  \
    {10400, -107, -1}, \
    {0, 0, 0} \
}

static struct fm_fake_channel mt6620_fake_ch[] = MT6620_FM_FAKE_CHANNEL;
static struct fm_fake_channel mt6627_fake_ch[] = MT6627_FM_FAKE_CHANNEL;
static struct fm_fake_channel mt6628_fake_ch[] = MT6628_FM_FAKE_CHANNEL;
static struct fm_fake_channel mt6580_fake_ch[] = MT6580_FM_FAKE_CHANNEL;
static struct fm_fake_channel mt6630_fake_ch[] = MT6630_FM_FAKE_CHANNEL;
static struct fm_fake_channel mt6631_fake_ch[] = MT6631_FM_FAKE_CHANNEL;
static struct fm_fake_channel mt6632_fake_ch[] = MT6632_FM_FAKE_CHANNEL;

static struct fm_fake_channel_t fake_ch_info = {0, 0};

void CUST_update_cfg(struct CUST_cfg_ds *cfg, int chipid)
{
    struct fm_fake_channel *fake_ch = NULL;

    LOGI("update connsys chipid=0x%x\n", chipid);

    switch (chipid)
    {
    case FM_CHIP_MT6580:
        cfg->chip = FM_CHIP_MT6580;
        fake_ch = mt6580_fake_ch;
        break;
    case FM_CHIP_MT6620:
        cfg->chip = FM_CHIP_MT6620;
        fake_ch = mt6620_fake_ch;
        break;
    case FM_CHIP_MT6627:
        cfg->chip = FM_CHIP_MT6627;
        fake_ch = mt6627_fake_ch;
        break;
    case FM_CHIP_MT6628:
        cfg->chip = FM_CHIP_MT6628;
        fake_ch = mt6628_fake_ch;
        break;
    case FM_CHIP_MT6630:
        cfg->chip = FM_CHIP_MT6630;
        fake_ch = mt6630_fake_ch;
        break;
    case FM_CHIP_MT6631:
        cfg->chip = FM_CHIP_MT6631;
        fake_ch = mt6631_fake_ch;
        break;
    case FM_CHIP_MT6632:
        cfg->chip = FM_CHIP_MT6632;
        fake_ch = mt6632_fake_ch;
        break;
    case FM_CHIP_MT6635:
        cfg->chip = FM_CHIP_MT6635;
        fake_ch = mt6631_fake_ch;
        break;
    default:
        LOGE("not support chipid=0x%x\n", chipid);
        break;
    }

    cfg->band = FM_RAIDO_BAND;  // 1, UE; 2, JAPAN; 3, JAPANW

    cfg->low_band = FM_FREQ_MIN * 10;
    cfg->high_band = FM_FREQ_MAX * 10;

    if (property_get_int32("persist.vendor.connsys.fm_50khz_support", 0) == 1) {
        cfg->seek_space = 5;    // FM radio seek space, 5:50KHZ; 1:100KHZ; 2:200KHZ
    } else {
        cfg->seek_space = 1;
    }

    cfg->max_scan_num = FM_MAX_CHL_SIZE;
    cfg->seek_lev = FM_SEEKTH_LEVEL_DEFAULT;
    cfg->scan_sort = FM_SCAN_SORT_SELECT;

    if (property_get_int32("persist.vendor.connsys.fm_short_antenna_support", 0) == 1) {
        cfg->short_ana_sup = fm_false;
    } else {
        cfg->short_ana_sup = fm_true;
    }

    cfg->rssi_th_l2 = FM_CHIP_DESE_RSSI_TH;
    cfg->rssi_th_l2 = (cfg->rssi_th_l2 > -72) ? -72 : cfg->rssi_th_l2;
    cfg->rssi_th_l2 = (cfg->rssi_th_l2 < -102) ? -102 : cfg->rssi_th_l2;

    if (fake_ch) {
        fake_ch_info.chan = fake_ch;
        fake_ch_info.size = 0;
        while (fake_ch[fake_ch_info.size].freq > 0) {
            fake_ch_info.size++;
        }
    }

    cfg->fake_chan = &fake_ch_info;
}

int CUST_get_cfg(struct CUST_cfg_ds *cfg)
{
    char val[PROPERTY_VALUE_MAX] = {0};
    int chipid = FM_CHIP_UNSUPPORTED;

    if (property_get("persist.vendor.connsys.fm_chipid", val, NULL)) {
        if (strcmp(val, "soc") == 0) {
            chipid = FM_CHIP_MT6580;
        } else if (strcmp(val, "mt6620") == 0) {
            chipid = FM_CHIP_MT6620;
        } else if (strcmp(val, "mt6627") == 0) {
            chipid = FM_CHIP_MT6627;
        } else if (strcmp(val, "mt6628") == 0) {
            chipid = FM_CHIP_MT6628;
        } else if (strcmp(val, "mt6630") == 0) {
            chipid = FM_CHIP_MT6630;
        } else if (strcmp(val, "mt6631") == 0) {
            chipid = FM_CHIP_MT6631;
        } else if (strcmp(val, "mt6632") == 0) {
            chipid = FM_CHIP_MT6632;
        } else if (strcmp(val, "mt6635") == 0) {
            chipid = FM_CHIP_MT6635;
        }
    }

    if (chipid == FM_CHIP_UNSUPPORTED) {
        if (property_get("vendor.connsys.adie.chipid", val, NULL)) {
            if (strcmp(val, "0x6631") == 0) {
                chipid = FM_CHIP_MT6631;
            } else if (strcmp(val, "0x6635") == 0) {
                chipid = FM_CHIP_MT6635;
            } else {
                LOGE("not support chipid=%s\n", val);
            }
        } else {
            LOGE("get vendor.connsys.adie.chipid fail\n");
        }
    }

    CUST_update_cfg(cfg, chipid);

    return 0;
}
