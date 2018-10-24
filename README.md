# Arduino-Data-logger-with-RTC-SD-card-OLED-display
This code is for an Arduino pro-mini connected to:
  - A real time clock (RTC) module (I2C interface).
  - A SD card reader (with SPI interface).
  - an SSD1306 OLED display (I2C interface)
The systems monitors the changes of state of an digital pin of the Arduino, and writes the the and dates of each change to the SD card. It is also possible to set the time and date of the RTC using a special file on the SD card. Finally, the OLED display shows the time of each change of the pin.

Pins:
SD card attached to SPI bus as follows:
  MOSI - pin 11
  MISO - pin 12
  CLK - pin 13
  CS - pin 4

  I2C SSD1306 Oled DISPLAY
  SCL = A5  ,  SDA = A4

  I2C RTC module
  SCL = A5  ,  SDA = A4
