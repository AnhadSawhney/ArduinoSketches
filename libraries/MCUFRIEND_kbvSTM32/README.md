# STM32_TFT_8bit
STM32 8bit TFT Library

I ported from here.   
https://github.com/prenticedavid/MCUFRIEND_kbv   

----

# Software requirement    

Adafruit GFX Library   
https://github.com/adafruit/Adafruit-GFX-Library   

----

# Wirering for 8bit Parallel TFT   

Using GPIOA as Data Port   
`#define TFT_DATA       GPIOA`   

|TFT||STM32F103|
|:-:|:-:|:-:|
|LCD_RST|--|PB7|
|LCD_CS|--|PB6|
|LCD_RS|--|PB5|
|LCD_WR|--|PB4|
|LCD_RD|--|PB3|
|LCD_D0|--|PA0|
|LCD_D1|--|PA1|
|LCD_D2|--|PA2|
|LCD_D3|--|PA3|
|LCD_D4|--|PA4|
|LCD_D5|--|PA5|
|LCD_D6|--|PA6|
|LCD_D7|--|PA7|
|5V|--|5V(*)|
|3.3V|--|3.3V(*)|
|GND|--|GND|


Using GPIOC as Data Port   
`#define TFT_DATA       GPIOC`   

|TFT||STM32F103|
|:-:|:-:|:-:|
|LCD_RST|--|PB7|
|LCD_CS|--|PB6|
|LCD_RS|--|PB5|
|LCD_WR|--|PB4|
|LCD_RD|--|PB3|
|LCD_D0|--|PC0|
|LCD_D1|--|PC1|
|LCD_D2|--|PC2|
|LCD_D3|--|PC3|
|LCD_D4|--|PC4|
|LCD_D5|--|PC5|
|LCD_D6|--|PC6|
|LCD_D7|--|PC7|
|5V|--|5V(*)|
|3.3V|--|3.3V(*)|
|GND|--|GND|

\*When a regulator(It's often AMS1117) is mounted on the back, it's operated 5V.   
\*When a regulator is NOT mounted on the back, it's operated 3.3V.   

Pin define is "STM32_TFT_8bit.h"   

----

# Tested TFT    
ILI9325 2.4inch 240x320   
ILI9341 2.4inch 240x320   
ILI9342 2.4inch 240x320   
SPFD5408 2.4inch 240x320   
R61505 2.4inch 240x320   
ST7783 2.4inch 240x320   
LGDP4532 2.4inch 240x320   
ILI9481 3.5inch 320x480   
R61509V 3.6inch 240x400   
RM68140 3.95inch 320x480   

----

# Setting your TFT's resolution    

If your TFT's resolution is 320x480,   
you have to set your TFT's resolution using tft.setResoution.   

Exsample:   
```
ID = tft.readID();
tft.setResolution(320, 480); // Set your resolution
Serial.print("Device ID: 0x"); Serial.println(ID, HEX);
tft.begin(ID);
uint32_t width = tft.width();
Serial.print("Width: "); Serial.println(width); // You will see 320
uint32_t height = tft.height();
Serial.print("Height: "); Serial.println(height); // You will see 480
```

If your TFT's resolution is 240x400,   
you have to set your TFT's resolution and TFT's offset.   

Exsample:   
```
ID = tft.readID();
tft.setResolution(240, 400); // Set your resolution
tft.setOffset(32); // Set your offset
Serial.print("Device ID: 0x"); Serial.println(ID, HEX);
tft.begin(ID);
uint32_t width = tft.width();
Serial.print("Width: "); Serial.println(width); // You will see 240
uint32_t height = tft.height();
Serial.print("Height: "); Serial.println(height); // You will see 400
```

---

# R61505 2.4 inch TFT
![r61505](https://user-images.githubusercontent.com/6020549/34552852-70d1e6bc-f167-11e7-8df9-41587ebccc79.JPG)

---

# SPFD5408 2.4 inch TFT
![spfd5408](https://user-images.githubusercontent.com/6020549/34552862-7ae8d62e-f167-11e7-84dd-73b07476cd50.JPG)

---

# ILI9325 2.4 inch TFT
![ili9325](https://user-images.githubusercontent.com/6020549/34552868-848b0fd0-f167-11e7-92f1-9c07e741a33e.JPG)

---

# ILI9341 2.4 inch TFT
![ili9341](https://user-images.githubusercontent.com/6020549/34552875-8eb6c832-f167-11e7-9c8c-7e3e602887d3.JPG)

---

# ILI9342 2.4 inch TFT
![ili9342](https://user-images.githubusercontent.com/6020549/34552887-9aa02fb2-f167-11e7-8ca6-6517de127a4a.JPG)

