#include "lvgl.h"

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_PIC_BAT_CHARGING
#define LV_ATTRIBUTE_IMG_PIC_BAT_CHARGING
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_PIC_BAT_CHARGING uint8_t pic_bat_charging_map[] = {
  0x00, 0x00, 0x00, 0x00, 	/*Color of index 0*/
  0x02, 0x14, 0xf2, 0x28, 	/*Color of index 1*/
  0x02, 0x14, 0xf2, 0x71, 	/*Color of index 2*/
  0x03, 0x14, 0xf1, 0xf1, 	/*Color of index 3*/

  0x05, 0x54, 
  0x0f, 0xf8, 
  0x1f, 0xf0, 
  0x2f, 0xc0, 
  0x7f, 0x54, 
  0xbf, 0xf4, 
  0xff, 0xd0, 
  0x5f, 0x80, 
  0x1e, 0x00, 
  0x3c, 0x00, 
  0x74, 0x00, 
  0x90, 0x00, 
};

const lv_img_dsc_t pic_bat_charging = {
  .header.cf = LV_IMG_CF_INDEXED_2BIT,
  .header.always_zero = 0,
  .header.reserved = 0,
  .header.w = 7,
  .header.h = 12,
  .data_size = 40,
  .data = pic_bat_charging_map,
};
