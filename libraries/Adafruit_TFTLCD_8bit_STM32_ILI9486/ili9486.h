#ifndef _ILI9486_H_
#define _ILI9486_H_


// Register names
#define ILI9486_SOFTRESET          0x01
#define ILI9486_SLEEPIN            0x10
#define ILI9486_SLEEPOUT           0x11
#define ILI9486_NORMALDISP         0x13
#define ILI9486_INVERTOFF          0x20
#define ILI9486_INVERTON           0x21
//#define ILI9486_GAMMASET           0x26
#define ILI9486_DISPLAYOFF         0x28
#define ILI9486_DISPLAYON          0x29
#define ILI9486_COLADDRSET         0x2A
#define ILI9486_PAGEADDRSET        0x2B
#define ILI9486_MEMORYWRITE        0x2C
#define ILI9486_PIXELFORMAT        0x3A
#define ILI9486_FRAMECONTROL       0xB1
#define ILI9486_DISPLAYFUNC        0xB6
#define ILI9486_ENTRYMODE          0xB7
#define ILI9486_POWERCONTROL1      0xC0
#define ILI9486_POWERCONTROL2      0xC1
#define ILI9486_VCOMCONTROL1      0xC5
//#define ILI9486_VCOMCONTROL2      0xC7
#define ILI9486_MEMCONTROL      0x36
#define ILI9486_MADCTL			0x36

#define ILI9486_MADCTL_MY  0x80
#define ILI9486_MADCTL_MX  0x40
#define ILI9486_MADCTL_MV  0x20
#define ILI9486_MADCTL_ML  0x10
#define ILI9486_MADCTL_RGB 0x00
#define ILI9486_MADCTL_BGR 0x08
#define ILI9486_MADCTL_MH  0x04


/*****************************************************************************/
extern void ili9486_begin(void);
extern void ili9486_setAddrWindow(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
extern void ili9486_fillScreen(uint16_t color);
extern void ili9486_drawPixel(int16_t x, int16_t y, uint16_t color);
extern void ili9486_setRotation(uint8_t x);
extern uint16_t ili9486_readPixel(int16_t x, int16_t y);


#endif
