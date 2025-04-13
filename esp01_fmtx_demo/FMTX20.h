/*****************************************************************************/
/*!
    @file       FMTX20.h
    @author     esp8266-www.elechouse.com
    @version    V2.0
    @date       2025-04-10
    @brief      FMTX20 demo header file

    @section    HISTORY
  
    V2.0        ESP01 Initial version.

    Copyright (c) 2025 esp8266 www.elechouse.com  All right reserved.
*/
/*****************************************************************************/

#ifndef __FMTX20_H
#define __FMTX20_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "Wire.h"


#ifndef __TYPE_REDEFINE
#define __TYPE_REDEFINE
typedef uint8_t u8;
typedef int8_t  s8;
typedef uint16_t u16;
typedef int16_t  s16;
typedef uint32_t u32;
typedef int32_t  s32;
#endif

#ifndef I2C_FREQ
  #define I2C_FREQ 100000L
#endif

#define FMTX_CMD_WRITE      (0x7C)
#define FMTX_CMD_READ       (0x7D)

typedef enum{
    PGA_0DB = 4,
    PGA_4DB = 5,
    PGA_8DB = 6,
    PGA_12DB = 7,
    PGA_N0DB = 0,
    PGA_N4DB = 1,
    PGA_N8DB = 2,
    PGA_N12DB = 3,
}fmtx_pga_type;
typedef enum{
    USA,
    JAPAN,
    EUROPE,
    AUSTRALIA,
    CHINA,
}country_type;
typedef enum{
    RFGAIN_LOW = 0,
    RFGAIN_MEDIUM = 1,
    RFGAIN_HIGH = 2,
    RFGAIN_MAX = 3,
}fmtx_rfgain_type;
typedef enum{
    DISABLE = 0,
    ENABLE_76KHZ = 1,
    ENABLE_120KHZ = 2,
}pilottone_type;

bool fmtx_write_reg(u8 reg, u8 dt);
u8 fmtx_read_reg(u8 reg);
void fmtx_read_all(u8 *buf);
void fmtx_set_alc(u8 sta);
void fmtx_set_pga(fmtx_pga_type pga);
void fmtx_set_sl(void);
void fmtx_set_rfgain(fmtx_rfgain_type rfgain);
bool fmtx_set_mute(bool mute);
bool fmtx_set_mono();
bool fmtx_set_stereo();
void fmtx_set_phcnst(country_type country);
bool fmtx_set_pilottone(u8 mode);
void fmtx_set_au_enhance(void);
void fmtx_set_xtal(void);
bool fmtx_set_bass(u8 bass);
u16 fmtx_get_channel(void);
bool fmtx_get_mute(void);
u8 fmtx_get_rfgain(void);
u8 fmtx_get_pga(void);
u8 fmtx_is_stereo(void);
bool fmtx_get_silence(void);
u8 fmtx_get_pilot(void);
u8 fmtx_get_alc(void);
u8 fmtx_get_country(void);
bool fmtx_get_powerok(void);

void fmtx_init(float freq, country_type country);
void fmtx_set_freq(float freq);
float fmtx_get_freq(void);

extern u8 fmtx_reg_val[18];
extern u8 fmtx_reg[18];



#endif
