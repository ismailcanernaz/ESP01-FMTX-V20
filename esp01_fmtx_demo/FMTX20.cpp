/*****************************************************************************/
/*!
    @file       FMTX20.cpp
    @author     esp8266-www.elechouse.com
    @version    V2.0
    @date       2025-04-10
    @brief      FMTX20 demo header file

    @section    HISTORY
  
    V2.0        ESP01 Initial version.

    Copyright (c) 2025 esp8266 www.elechouse.com  All right reserved.
*/
/*****************************************************************************/

#include "FMTX20.h"
#include <Wire.h>

u8 fmtx_reg[18]={
    0x00, 0x01, 0x02, 0x04, 0x0B,
    0x0C, 0x0E, 0x0F, 0x10, 0x12,
    0x13, 0x14, 0x15, 0x16, 0x17,
    0x1E, 0x26, 0x27,
};

u8 fmtx_reg_val[18]={

};

bool fmtx_write_reg(u8 reg, u8 dt)
{
    Wire.beginTransmission(FMTX_CMD_WRITE >> 1);
    Wire.write(reg);
    Wire.write(dt);
    return (Wire.endTransmission() == 0);
}

u8 fmtx_read_reg(u8 reg)
{
    u8 dt;
    Wire.beginTransmission(FMTX_CMD_WRITE >> 1);
    Wire.write(reg);
    Wire.endTransmission(false);

    Wire.requestFrom(FMTX_CMD_READ >> 1, (u8)1);
    if (Wire.available()) {
        dt = Wire.read();
    }

    return dt;
}

/**
    Register    0x00, 0x01, 0x02, 0x04, 0x0B, 0x0C, 0x0E, 0x0F, 0x10, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x1E, 0x26, 0x27,
    Read        0x5C, 0xC3, 0x40, 0x04, 0x00, 0x00, 0xC2, 0xFB, 0xA8, 0x80, 0x80, 0x00, 0xE0, 0x00, 0x00, 0x00, 0xA0, 0x00,
    Default     0x5C, 0xC3, 0x40, 0x04, 0x00, 0x00, 0x02, 0x14, 0xA8, 0x80, 0x80, 0x00, 0xE0, 0x00, 0x00, 0x00, 0xA0, 0x00,
    others      0x7A, 0x03, 0x01, 0x04, 0x00, 0x00, 0xC2, 0xFB, 0xA8, 0x80, 0x80, 0x00, 0xE0, 0x00, 0x04, 0x40, 0xA0, 0x00,
				0x5C, 0xC3, 0x40, 0x04, 0x00, 0x00, 0xC2, 0x00, 0xA8, 0x80, 0x80, 0x00, 0xE0, 0x00, 0x00, 0x40, 0xA0, 0x00,
*/
void fmtx_read_all(u8 *buf)
{
    u8 i;
    for(i=0; i<18; i++){
        buf[i] = fmtx_read_reg(fmtx_reg[i]);
    }
}

void fmtx_set_freq(float freq)
{
    u16 f;
    u8 reg0, reg1_8, reg9_11;

    reg0 = fmtx_read_reg(0x02);
    reg9_11 = fmtx_read_reg(0x01);

    freq *= 20;
    f = (u16)freq;
    f &= 0x0FFF;

    if(f&0x01){
        reg0 |= 0x80;
    }else{
        reg0 &= ~0x80;
    }

    reg1_8 = (u8)(f>>1);
    reg9_11 = (reg9_11&0xF8) | (u8)(f>>9);

    fmtx_write_reg(0x02, reg0);
    fmtx_write_reg(0x01, reg9_11);
    fmtx_write_reg(0x00, reg1_8);

}

void fmtx_set_pga(fmtx_pga_type pga)
{
    u8 reg;
    u8 pga_val;
    reg = fmtx_read_reg(0x01);
    pga_val = (u8)pga;
    pga_val &= ~0xC7;
    reg = (reg&0xC7) | pga_val;
    fmtx_write_reg(0x01, reg);
}

void fmtx_set_rfgain(fmtx_rfgain_type rfgain)
{
    u8 reg3, reg0_1, reg2;

    reg0_1 = fmtx_read_reg(0x01);
    reg2 = fmtx_read_reg(0x13);
    reg3 = fmtx_read_reg(0x02);

    u8 gain = static_cast<u8>(rfgain) & 0x0F;
    reg0_1 = (reg0_1&0x3F) | (gain<<6);
    if(gain & 0x04){
        reg2 |= 0x80;
    }else{
        reg2 &= ~0x80;
    }

    if(gain & 0x08){
        reg3 |= 0x40;
    }else{
        reg3 &= ~0x40;
    }

    fmtx_write_reg(0x01, reg0_1);
    fmtx_write_reg(0x13, reg2);
    fmtx_write_reg(0x02, reg3);
}

bool fmtx_set_mute(bool mute)
{
    u8 i = fmtx_read_reg(0x02);
    if ((mute == true)  && (i & 0x08) == 0x08) return true;
    if ((mute == false) && (i & 0x08) == 0x00) return true;
    i = i ^ 0x08;
    return fmtx_write_reg(0x02, i);
}

bool fmtx_set_mono()
{
    u8 i = fmtx_read_reg(0x04);
    if (i & (1 << 6))
    {
      i &= ~(1 << 6);
      return fmtx_write_reg(0x04, i);
    }
    return true;
}

bool fmtx_set_stereo()
{
    u8 i = fmtx_read_reg(0x04);
    if ((i & (1 << 6)) == 0)
    {
      i |= (1 << 6);
      return fmtx_write_reg(0x04, i);
    }
    return true;
}

void fmtx_set_alc(u8 sta)
{
    u8 reg;
    reg = fmtx_read_reg(0x04);
    if(!sta){
        reg &= ~0x80;
    }else{
        reg |= 0x80;
    }
    fmtx_write_reg(0x04, reg);
}

void fmtx_set_sl(void)
{
    u8 reg;
    reg = fmtx_read_reg(0x12);
    fmtx_write_reg(0x12, (reg&0x81) | 0x7E);

    reg = fmtx_read_reg(0x14);
    fmtx_write_reg(0x14, (reg&0x02) | 0xDD);

    reg = fmtx_read_reg(0x16);
    fmtx_write_reg(0x16, (reg&0xF8) | 0x07);

    reg = fmtx_read_reg(0x0B);
    fmtx_write_reg(0x1B, reg | 0x04);

    reg = fmtx_read_reg(0x12);
    fmtx_write_reg(0x12, reg&0x7F);
}

void fmtx_set_phcnst(country_type country)
{
    u8 reg;
    reg = fmtx_read_reg(0x02);
    switch(country){
        case USA:
        case JAPAN:
            reg &= ~0x01;
            break;
        case EUROPE:
        case AUSTRALIA:
        case CHINA:
            reg |= 0x01;
            break;
        default:
            break;
    }
    fmtx_write_reg(0x02, reg);
}

bool fmtx_set_pilottone(u8 mode)
{
    if (mode > 1) return false;
    u8 i = fmtx_read_reg(0x02);
    if ((mode == 1) && (i & 0x04) == 0x04) return true;
    if ((mode == 0) && (i & 0x04) == 0x00) return true;
    i = i ^ 0x04;
    return fmtx_write_reg(0x02, i);
}

void fmtx_set_au_enhance(void)
{
    u8 reg;
    reg = fmtx_read_reg(0x17);

    fmtx_write_reg(0x17, reg |= 0x20);
}

void fmtx_set_xtal(void)
{
    u8 reg;
    reg = fmtx_read_reg(0x1E);
    fmtx_write_reg(0x1E, reg | 0x40);
}

bool fmtx_set_bass(u8 bass)
{
    if (bass > 3) return false;
    u8 i = fmtx_read_reg(0x04);
    i &= ~0x03;
    i |= bass;
    return fmtx_write_reg(0x04, i);
}

u16 fmtx_get_channel(void)
{
    u16 reg = fmtx_read_reg(0x01) & 0x07;
    reg <<= 8;
    reg |= fmtx_read_reg(0x00);
    reg <<= 1;
    reg |= (fmtx_read_reg(0x02) >> 7);
    return reg;
}

float fmtx_get_freq(void)
{
    return fmtx_get_channel() * 0.05;
}

bool fmtx_get_mute(void)
{
    return (fmtx_read_reg(0x02) & 0x08) > 0;
}

u8 fmtx_get_rfgain(void)
{
    u8 dt = fmtx_read_reg(0x01) >> 6;
    dt |= ((fmtx_read_reg(0x13) & 0x80) >> 5);
    dt |= ((fmtx_read_reg(0x02) & 0x40) >> 3);
    return dt;
}

u8 fmtx_get_pga(void)
{
    return (fmtx_read_reg(0x01) >> 3) & 0x07;
}

u8 fmtx_is_stereo(void)
{
    u8 reg = fmtx_read_reg(0x04);
    return (reg & (1 << 6));
}

bool fmtx_get_silence(void)
{
    u8 reg = fmtx_read_reg(0x0F);
    return (reg & (1 << 2)) > 0;
}

u8 fmtx_get_pilot(void)
{
    return (fmtx_read_reg(0x02) & 0x04) > 0;
}

u8 fmtx_get_alc(void)
{
    u8 reg = fmtx_read_reg(0x08);
    return (reg >> 4) & 0x01;
}

u8 fmtx_get_country(void)
{
    return (fmtx_read_reg(0x02) & 0x01) > 0;
}

bool fmtx_get_powerok(void)
{
    u8 reg = fmtx_read_reg(0x0F);
    return (reg & (1 << 4)) > 0;
}

void fmtx_init(float freq, country_type country)
{
	fmtx_set_freq(freq);
  fmtx_set_mute(false);
	fmtx_set_rfgain(RFGAIN_MAX);
	fmtx_set_pga(PGA_0DB);
	fmtx_set_phcnst(country);
	fmtx_set_pilottone(1);
	fmtx_set_stereo();
  fmtx_set_bass(0);
	fmtx_set_xtal();
}
