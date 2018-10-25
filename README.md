# Arduino-based data logger with real-time clock, SD card and OLED display

This a simple  Arduino-based data logger. The systems monitors any change of state of a digital pin (here #2) of the Arduino, and writes the time and date of each change to the SD card (file "data.txt"). As a bonus, the temperature (from the internal RTC temperature sensor) is also written to the file. When booting, if the file "data.txt" does not exist, it is created. Otherwise the data are appended to it.

It is also possible to set the time and date of the RTC using a special file on the SD card ("setdate.txt" - see the code for the format). Finally, at each change of the input pin, the time is printed on the OLED display. 

# Physical connections

- Digital pin connected to the input signal: pin 2 (can be changed in the code of course)

- SD card attached to SPI bus as follows:
  - MOSI - pin 11
  - MISO - pin 12
  - CLK - pin 13
  - CS - pin 4

- I2C SSD1306 Oled DISPLAY:
  - SCL = A5
  - SDA = A4

- I2C RTC DS3231 module
  - SCL = A5
  - SDA = A4
