// see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
#ifndef __ICONS_H__
#define __ICONS_H__

#ifdef MOD_OLED
const uint8_t activeSymbol[] PROGMEM = {
    B00000000,
    B00000000,
    B00011000,
    B00100100,
    B01000010,
    B01000010,
    B00100100,
    B00011000
};

const uint8_t inactiveSymbol[] PROGMEM = {
    B00000000,
    B00000000,
    B00000000,
    B00000000,
    B00011000,
    B00011000,
    B00000000,
    B00000000
};


#define bat_width 16
#define bat_height 8
//static char bat_000_bits[] = { 0xFF, 0x3F, 0x01, 0x20, 0x01, 0xE0, 0x01, 0x80, 0x01, 0x80, 0x01, 0xE0, 0x01, 0x20, 0xFF, 0x3F };
//static char bat_010_bits[] = { 0xFF, 0x3F, 0x01, 0x20, 0x05, 0xE0, 0x05, 0x80, 0x05, 0x80, 0x05, 0xE0, 0x01, 0x20, 0xFF, 0x3F };
//static char bat_020_bits[] = { 0xFF, 0x3F, 0x01, 0x20, 0x0D, 0xE0, 0x0D, 0x80, 0x0D, 0x80, 0x0D, 0xE0, 0x01, 0x20, 0xFF, 0x3F };
//static char bat_030_bits[] = { 0xFF, 0x3F, 0x01, 0x20, 0x1D, 0xE0, 0x1D, 0x80, 0x1D, 0x80, 0x1D, 0xE0, 0x01, 0x20, 0xFF, 0x3F };
//static char bat_040_bits[] = { 0xFF, 0x3F, 0x01, 0x20, 0x3D, 0xE0, 0x3D, 0x80, 0x3D, 0x80, 0x3D, 0xE0, 0x01, 0x20, 0xFF, 0x3F };
//static char bat_050_bits[] = { 0xFF, 0x3F, 0x01, 0x20, 0x7D, 0xE0, 0x7D, 0x80, 0x7D, 0x80, 0x7D, 0xE0, 0x01, 0x20, 0xFF, 0x3F };
//static char bat_060_bits[] = { 0xFF, 0x3F, 0x01, 0x20, 0xFD, 0xE0, 0xFD, 0x80, 0xFD, 0x80, 0xFD, 0xE0, 0x01, 0x20, 0xFF, 0x3F };
//static char bat_070_bits[] = { 0xFF, 0x3F, 0x01, 0x20, 0xFD, 0xE1, 0xFD, 0x81, 0xFD, 0x81, 0xFD, 0xE1, 0x01, 0x20, 0xFF, 0x3F };
//static char bat_080_bits[] = { 0xFF, 0x3F, 0x01, 0x20, 0xFD, 0xE3, 0xFD, 0x83, 0xFD, 0x83, 0xFD, 0xE3, 0x01, 0x20, 0xFF, 0x3F };
//static char bat_090_bits[] = { 0xFF, 0x3F, 0x01, 0x20, 0xFD, 0xE7, 0xFD, 0x87, 0xFD, 0x87, 0xFD, 0xE7, 0x01, 0x20, 0xFF, 0x3F };
//static char bat_100_bits[] = { 0xFF, 0x3F, 0x01, 0x20, 0xFD, 0xEF, 0xFD, 0x8F, 0xFD, 0x8F, 0xFD, 0xEF, 0x01, 0x20, 0xFF, 0x3F };


#define WiFi_width 46
#define WiFi_height 32
const uint8_t WiFi_bits[] PROGMEM = {
  0x00, 0x00, 0xFC, 0x07, 0x00, 0x00, 0x00, 0x00, 0xFE, 0x3F, 0x00, 0x00,
  0x00, 0x80, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0xE0, 0xFF, 0xFF, 0x01, 0x00,
  0x00, 0xF0, 0xFF, 0xFF, 0x03, 0x00, 0x00, 0xF8, 0xFF, 0xFF, 0x07, 0x00,
  0xE0, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0xF8, 0xFF, 0xFF, 0xDF, 0xAD, 0x07,
  0xFC, 0xFF, 0xFF, 0x07, 0x00, 0x0E, 0xFE, 0xFF, 0xFF, 0x03, 0x00, 0x18,
  0xFE, 0xFF, 0xFB, 0x01, 0xC0, 0x18, 0x8F, 0x19, 0xF1, 0xF0, 0xCF, 0x31,
  0x8F, 0x10, 0xF3, 0xF8, 0xCF, 0x30, 0x9F, 0x10, 0xFB, 0x70, 0x09, 0x30,
  0x9F, 0x90, 0xF3, 0x38, 0xC0, 0x30, 0x1F, 0x80, 0xF1, 0xF0, 0xE2, 0x31,
  0x1F, 0x80, 0xF3, 0xF8, 0xC7, 0x21, 0x1F, 0x84, 0xF1, 0xF0, 0xCF, 0x30,
  0x3F, 0xC6, 0xF1, 0x38, 0xC0, 0x30, 0x3F, 0xC6, 0xF1, 0x70, 0xC0, 0x31,
  0x3F, 0xC6, 0xF3, 0x30, 0xC0, 0x30, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0x10,
  0xFE, 0xFF, 0x7F, 0x00, 0x00, 0x18, 0xFC, 0xFF, 0x3F, 0x00, 0x00, 0x0C,
  0xF8, 0xFF, 0x3F, 0x00, 0x00, 0x07, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0x03,
  0x80, 0xFA, 0xFF, 0xFF, 0x57, 0x00, 0x00, 0xF0, 0xFF, 0xFF, 0x03, 0x00,
  0x00, 0xE0, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0x80, 0xFF, 0xFF, 0x00, 0x00,
  0x00, 0x00, 0xFF, 0x1F, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x0F, 0x00, 0x00,
  };


#define WiFi1_width 60
#define WiFi1_height 36
const uint8_t WiFi1_bits[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xFF, 0x07, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xE0, 0xFF, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xFF,
  0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFF, 0xFF, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xFE, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF,
  0xFF, 0x03, 0x00, 0x00, 0x00, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00,
  0x00, 0xFF, 0xFF, 0xFF, 0x07, 0xC0, 0x83, 0x01, 0x80, 0xFF, 0xFF, 0xFF,
  0x01, 0x00, 0x07, 0x00, 0xC0, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x0C, 0x00,
  0xC0, 0xFF, 0xFF, 0x7C, 0x00, 0x60, 0x0C, 0x00, 0xC0, 0x31, 0x46, 0x7C,
  0xFC, 0x77, 0x08, 0x00, 0xE0, 0x23, 0xC6, 0x3C, 0xFC, 0x67, 0x18, 0x00,
  0xE0, 0x23, 0xE4, 0x3F, 0x1C, 0x00, 0x18, 0x00, 0xE0, 0x23, 0x60, 0x3C,
  0x1C, 0x70, 0x18, 0x00, 0xE0, 0x03, 0x60, 0x3C, 0x1C, 0x70, 0x18, 0x00,
  0xE0, 0x07, 0x60, 0x3C, 0xFC, 0x73, 0x18, 0x00, 0xE0, 0x87, 0x70, 0x3C,
  0xFC, 0x73, 0x18, 0x00, 0xE0, 0x87, 0x70, 0x3C, 0x1C, 0x70, 0x18, 0x00,
  0xE0, 0x87, 0x70, 0x3C, 0x1C, 0x70, 0x18, 0x00, 0xE0, 0x8F, 0x71, 0x3C,
  0x1C, 0x70, 0x18, 0x00, 0xC0, 0xFF, 0xFF, 0x3F, 0x00, 0x00, 0x08, 0x00,
  0xC0, 0xFF, 0xFF, 0x1F, 0x00, 0x00, 0x0C, 0x00, 0x80, 0xFF, 0xFF, 0x1F,
  0x00, 0x00, 0x06, 0x00, 0x80, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x07, 0x00,
  0x00, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0xF8, 0xFF, 0xFF,
  0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0xFF, 0x01, 0x00, 0x00,
  0x00, 0x00, 0xFC, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xFF,
  0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xFF, 0x1F, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x80, 0xFF, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  };

#define ch2i_width 77
#define ch2i_height 64
const uint8_t ch2i_bits[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0xF0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xFC, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xFE, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xFF, 0x3F,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xFF, 0xFF, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x0F, 0xFE, 0x03, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xFC, 0x07, 0xF8, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xFF, 0x01, 0xF0, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x7F,
  0x00, 0xC0, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x1F, 0x00, 0x00,
  0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x0F, 0x00, 0x00, 0xFE, 0x07,
  0x00, 0x00, 0x00, 0x00, 0xFE, 0x03, 0x00, 0x00, 0xF8, 0x0F, 0x00, 0x00,
  0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0xE0, 0x1F, 0x00, 0x00, 0x00, 0x00,
  0x7F, 0x00, 0x00, 0x00, 0x80, 0x3F, 0x00, 0x00, 0x00, 0x80, 0x1F, 0x00,
  0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x80, 0x0F, 0x00, 0x00, 0x00,
  0x00, 0x3E, 0x00, 0x00, 0x00, 0x80, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x3C,
  0x00, 0x00, 0x00, 0x80, 0x07, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00,
  0x00, 0x80, 0x07, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x01, 0x80,
  0x07, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x0E, 0x80, 0x07, 0x00,
  0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0xFE, 0x8B, 0x07, 0x00, 0x00, 0x00,
  0x00, 0x3C, 0x00, 0x00, 0xFC, 0xFF, 0x07, 0x00, 0x00, 0x00, 0x00, 0x3C,
  0x00, 0x00, 0xFC, 0xFF, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00,
  0xF8, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0xF8, 0xF3,
  0xFF, 0x00, 0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0xF0, 0x07, 0xFE, 0x03,
  0x00, 0x00, 0x00, 0x3C, 0x00, 0x00, 0xF0, 0x1F, 0xF8, 0x03, 0x00, 0x14,
  0x00, 0x3C, 0x00, 0x00, 0xE0, 0x3F, 0xE0, 0x0F, 0x80, 0xFF, 0x00, 0x3C,
  0x00, 0x00, 0xE0, 0x7F, 0xF0, 0x0F, 0xC0, 0xFF, 0x00, 0x3C, 0x00, 0x00,
  0xC0, 0x0F, 0xF0, 0x1F, 0xE0, 0xFF, 0x01, 0x3C, 0x00, 0x00, 0xC0, 0x3F,
  0xC0, 0x3F, 0xE0, 0xF1, 0x03, 0xFC, 0x0F, 0x00, 0x80, 0xFF, 0x83, 0x3F,
  0xE0, 0xC1, 0x01, 0xFC, 0x3F, 0x00, 0x80, 0xFF, 0x0F, 0x7F, 0xE0, 0xC0,
  0x03, 0x7C, 0x7F, 0x00, 0x00, 0xFF, 0x3F, 0x7E, 0xE0, 0xC1, 0x01, 0x3C,
  0xF0, 0x01, 0x00, 0xFE, 0xFF, 0xFC, 0xE0, 0xC0, 0x03, 0x3C, 0xE0, 0x01,
  0x00, 0xFC, 0xFF, 0xF9, 0xE0, 0xC1, 0x01, 0xFC, 0x81, 0x03, 0x00, 0xFC,
  0xFF, 0xF3, 0xE0, 0xC0, 0x03, 0xFE, 0x87, 0x07, 0x00, 0xF8, 0xFF, 0xE7,
  0xE0, 0xC1, 0x03, 0xBF, 0x0F, 0x0F, 0x00, 0xE0, 0xFF, 0xEF, 0xFF, 0xC0,
  0xFF, 0x3F, 0x1E, 0x0E, 0x00, 0xC0, 0xFF, 0xDF, 0xFF, 0x80, 0xFF, 0x1F,
  0x3C, 0x1C, 0x00, 0x80, 0xFF, 0xBF, 0x7F, 0x80, 0xFF, 0x7F, 0x78, 0x1C,
  0x00, 0x00, 0xFC, 0xBF, 0x3F, 0x00, 0xFE, 0xFF, 0x70, 0x1C, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xE3, 0x18, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x80, 0xE3, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x10, 0xE7, 0x18, 0x00, 0x00, 0x00, 0xE0, 0x01, 0x00, 0x00, 0x3C,
  0xC6, 0x18, 0x00, 0x00, 0x00, 0xE0, 0x00, 0x00, 0x00, 0x3C, 0xE6, 0x00,
  0x00, 0x00, 0x00, 0xE0, 0x01, 0x00, 0x05, 0x3C, 0xC6, 0x00, 0x00, 0x00,
  0x00, 0xE0, 0x00, 0xC0, 0x1F, 0x18, 0x06, 0x00, 0x00, 0x00, 0x00, 0xE0,
  0x01, 0xE0, 0x3F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xE0, 0x1C, 0xF0,
  0x7F, 0x3C, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xE3, 0x7F, 0xF0, 0x78, 0x3C,
  0x00, 0x00, 0x00, 0x00, 0xFF, 0xE3, 0xFF, 0x60, 0x78, 0x3C, 0x00, 0x00,
  0x00, 0x00, 0x9F, 0xE1, 0xFB, 0x00, 0x78, 0x3C, 0x00, 0x00, 0x00, 0x00,
  0x0F, 0xE0, 0xF1, 0x01, 0x3C, 0x3C, 0x00, 0x00, 0x00, 0x80, 0x07, 0xE0,
  0xE0, 0x00, 0x3E, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x07, 0xE0, 0xE1, 0x81,
  0x1F, 0x3C, 0x00, 0x00, 0x00, 0x80, 0x07, 0xE0, 0xE0, 0x80, 0x0F, 0x3C,
  0x00, 0x00, 0x00, 0x00, 0x0F, 0xE0, 0xE1, 0xE1, 0x07, 0x3C, 0x00, 0x00,
  0x00, 0x00, 0x0F, 0xE3, 0xE0, 0xE0, 0x03, 0x3C, 0x00, 0x00, 0x00, 0x00,
  0xFF, 0xE3, 0xE1, 0xF1, 0x7F, 0x3C, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xE3,
  0xE0, 0xF0, 0x7F, 0x3C, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xE1, 0xE1, 0xF1,
  0x7F, 0x3C, 0x00, 0x00
 };

#define remora_width 110
#define remora_height 64
const uint8_t remora_bits[] PROGMEM = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0x7f, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xff, 0xff, 0xff,
   0xff, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xff, 0xff,
   0xff, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0,
   0xff, 0x1f, 0x00, 0x00, 0xfe, 0xff, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0xfe, 0x7f, 0x00, 0x00, 0x00, 0x80, 0xff, 0x1f, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x80, 0xff, 0x07, 0x80, 0xff, 0x7f, 0x00, 0xf8, 0xff, 0x00,
   0x00, 0x00, 0x00, 0x00, 0xe0, 0x7f, 0x80, 0xff, 0xff, 0xff, 0x7f, 0x80,
   0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x0f, 0xfc, 0xff, 0xff, 0xf8,
   0xff, 0x0f, 0xfc, 0x0f, 0x00, 0x00, 0x00, 0x00, 0xff, 0xc3, 0xff, 0xff,
   0x7f, 0xf8, 0xff, 0xff, 0xf0, 0x3f, 0x00, 0x00, 0x00, 0x80, 0xff, 0xf8,
   0x7f, 0xff, 0x7f, 0xf0, 0xff, 0xff, 0xc7, 0xff, 0x00, 0x00, 0x00, 0xe0,
   0x1f, 0xfe, 0x3f, 0xfc, 0x3f, 0xf0, 0xff, 0xff, 0x1f, 0xfe, 0x01, 0x00,
   0x00, 0xf0, 0x8f, 0xff, 0x1f, 0xfc, 0x3f, 0xf0, 0xff, 0xf1, 0x7f, 0xfc,
   0x07, 0x00, 0x00, 0xf8, 0xe1, 0xff, 0x0f, 0xfc, 0x3f, 0xf0, 0xff, 0xe0,
   0xff, 0xe1, 0x0f, 0x00, 0x00, 0xfe, 0xf8, 0xff, 0x0f, 0xfc, 0x1f, 0xf0,
   0xff, 0xe0, 0xff, 0xc7, 0x1f, 0x00, 0x00, 0x3f, 0x3e, 0xfe, 0x0f, 0xfc,
   0x1f, 0xf0, 0x7f, 0xe0, 0xff, 0x1f, 0x3f, 0x00, 0x80, 0x1f, 0x0f, 0xfc,
   0x07, 0xfc, 0x0f, 0xf8, 0x7f, 0xe0, 0xff, 0x3f, 0x7e, 0x00, 0xc0, 0x8f,
   0x03, 0xfc, 0x07, 0xfc, 0x0f, 0xf8, 0x7f, 0xe0, 0xff, 0x7f, 0xfc, 0x00,
   0xc0, 0xc7, 0x01, 0xfc, 0x07, 0xfc, 0x07, 0xf8, 0x3f, 0xf0, 0xff, 0xff,
   0xf8, 0x01, 0xe0, 0xf3, 0x00, 0xfe, 0x03, 0xfe, 0x07, 0xf8, 0x3f, 0xf0,
   0xff, 0xff, 0xf3, 0x03, 0xf0, 0x39, 0x00, 0xfe, 0x03, 0xfe, 0x07, 0xfc,
   0x1f, 0xf0, 0xff, 0xff, 0xe7, 0x03, 0xf0, 0x1d, 0x00, 0xff, 0x01, 0xfe,
   0x03, 0xfc, 0x1f, 0xf0, 0xff, 0xc7, 0xef, 0x07, 0xf8, 0x1c, 0x00, 0xff,
   0x01, 0xfe, 0x03, 0xfc, 0x1f, 0xf8, 0xff, 0x03, 0xcf, 0x07, 0x7c, 0x0e,
   0x80, 0xff, 0x00, 0xfe, 0x01, 0xfc, 0x0f, 0xf8, 0xff, 0x03, 0x9e, 0x0f,
   0x7c, 0x06, 0x80, 0xff, 0x00, 0xff, 0x01, 0xfe, 0x0f, 0xf8, 0xff, 0x01,
   0x9c, 0x1f, 0x3c, 0x07, 0x00, 0x00, 0x00, 0xff, 0x01, 0xfe, 0x0f, 0xf8,
   0xff, 0x00, 0x38, 0x1f, 0x3e, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0xf8, 0xff, 0x00, 0x38, 0x1f, 0x1e, 0x03, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x1e, 0x9e, 0x03, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x3e, 0x9e, 0x01,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x3e,
   0x9e, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x60, 0x3e, 0x9e, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x60, 0x3e, 0x9e, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x60, 0x3e, 0x9e, 0x01, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x3e, 0x9e, 0x01, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x3e, 0x9e, 0x03,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x3e,
   0x1e, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x30, 0x1e, 0x3e, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8,
   0xff, 0x00, 0x38, 0x1f, 0x3c, 0x07, 0x00, 0x00, 0x00, 0xff, 0x01, 0xfe,
   0x0f, 0xf8, 0xff, 0x00, 0x38, 0x1f, 0x7c, 0x06, 0x80, 0xff, 0x00, 0xff,
   0x01, 0xfe, 0x0f, 0xf8, 0xff, 0x01, 0x9c, 0x1f, 0x7c, 0x0e, 0x80, 0xff,
   0x01, 0xfe, 0x01, 0xfc, 0x0f, 0xf8, 0xff, 0x03, 0x9e, 0x0f, 0xf8, 0x1c,
   0x00, 0xff, 0x01, 0xfe, 0x03, 0xfc, 0x1f, 0xf8, 0xff, 0x03, 0xdf, 0x07,
   0xf0, 0x1d, 0x00, 0xff, 0x01, 0xfe, 0x03, 0xfc, 0x1f, 0xf0, 0xff, 0xc7,
   0xef, 0x07, 0xf0, 0x79, 0x00, 0xfe, 0x03, 0xfe, 0x07, 0xfc, 0x1f, 0xf0,
   0xff, 0xff, 0xe7, 0x03, 0xe0, 0xf3, 0x00, 0xfe, 0x03, 0xfe, 0x07, 0xf8,
   0x3f, 0xf0, 0xff, 0xff, 0xf3, 0x03, 0xc0, 0xc7, 0x01, 0xfc, 0x07, 0xfc,
   0x07, 0xf8, 0x3f, 0xf0, 0xff, 0xff, 0xf8, 0x01, 0xc0, 0x8f, 0x03, 0xfc,
   0x07, 0xfc, 0x0f, 0xf8, 0x7f, 0xe0, 0xff, 0x7f, 0xfc, 0x00, 0x80, 0x1f,
   0x0f, 0xfc, 0x07, 0xfc, 0x0f, 0xf8, 0x7f, 0xe0, 0xff, 0x3f, 0x7e, 0x00,
   0x00, 0x3f, 0x3e, 0xfe, 0x0f, 0xfc, 0x1f, 0xf0, 0x7f, 0xe0, 0xff, 0x1f,
   0x3f, 0x00, 0x00, 0xfe, 0xf8, 0xff, 0x0f, 0xfc, 0x1f, 0xf0, 0xff, 0xe0,
   0xff, 0xc7, 0x1f, 0x00, 0x00, 0xf8, 0xe1, 0xff, 0x0f, 0xfc, 0x3f, 0xf0,
   0xff, 0xe0, 0xff, 0xe1, 0x0f, 0x00, 0x00, 0xf0, 0x8f, 0xff, 0x1f, 0xfc,
   0x3f, 0xf0, 0xff, 0xf1, 0x7f, 0xfc, 0x07, 0x00, 0x00, 0xe0, 0x1f, 0xfe,
   0x3f, 0xfc, 0x3f, 0xf0, 0xff, 0xff, 0x1f, 0xfe, 0x01, 0x00, 0x00, 0x80,
   0xff, 0xf8, 0x7f, 0xff, 0x7f, 0xf0, 0xff, 0xff, 0xc7, 0xff, 0x00, 0x00,
   0x00, 0x00, 0xff, 0xc3, 0xff, 0xff, 0x7f, 0xf8, 0xff, 0xff, 0xf0, 0x3f,
   0x00, 0x00, 0x00, 0x00, 0xfc, 0x0f, 0xfc, 0xff, 0xff, 0xf8, 0xff, 0x0f,
   0xfc, 0x0f, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x7f, 0x80, 0xff, 0xff, 0xff,
   0x7f, 0x80, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x80, 0xff, 0x07, 0x80,
   0xff, 0x7f, 0x00, 0xf8, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc,
   0x7f, 0x00, 0x00, 0x00, 0x80, 0xff, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0xf0, 0xff, 0x1f, 0x00, 0x00, 0xfe, 0xff, 0x03, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x80, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xff, 0xff, 0xff, 0xff, 0x07,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff,
   0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
   0xff, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
#endif // MOD_OLED
#endif // ICONS_H