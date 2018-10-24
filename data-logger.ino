//==========================================================
/*   https://github.com/grmis-fr

  SD card datalogger
  Arduino UNO

  SD card attached to SPI bus:
  MOSI - pin 11
  MISO - pin 12
  CLK - pin 13
  CS - pin 4

  I2C SSD1306 Oled DISPLAY: 
  SCL = A5  ,  SDA = A4

  RTC module:
  SCL = A5  ,  SDA = A4

*/
//==========================================================

#include <SdFat.h>
#include <Wire.h>// Library for I2C  bus communication (with the RTC module)
#include "DS3231.h" //Real-time clock (RTC) RTC module library
#include <SPI.h> // Library for SPI  bus communication (with the SD card reader)

//ASCII-only (uses less memory than graphical versions) OLED display library
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

// Define proper RST_PIN if required.
#define RST_PIN -1

SSD1306AsciiWire oled;
SdFat SD;
//==========================================================

#define chipSelect 4
#define signal_PIN 2 //Data pin connected to the input signal
#define filename "data.txt"
#define date_filename "setdate.txt"

// Minimum time between two successive change of the input (in ms). Usefull to "de-bounce" the input.
// You may reduce the delay belo if you expect faster changes on the input pin.
#define minimum_time 200 

RTClib rtc;
DS3231 Clock;
uint8_t  state, last_state;
unsigned long last_change_time = 0;
//==========================================================
String time2string() {
  DateTime now = rtc.now();
  String d = String(now.unixtime())// number of seconds elapsed since Jan. 1st 1970.
             + ", " + now.year() + ", " + now.month() + ", " + now.day() + ", " + now.hour() + ", " + now.minute() + ", " + now.second();
  return d;
}
//==========================================================
void Println(const String s) {
  Serial.println(s);
  oled.println(s);
}
void Print(const String s) {
  Serial.print(s);
  oled.print(s);
}
void Println(const int s) {
  Serial.println(s);
  oled.println(s);
}
void Print(const int s) {
  Serial.print(s);
  oled.print(s);
}
//--------------------------------------------------------
void PrintDate() {
  DateTime now = rtc.now();
  Print(now.day()); Print(" ");
  switch (now.month()) {
    case 1: Print(F("Jan.")); break;
    case 2: Print(F("Fev.")); break;
    case 3: Print(F("Mar.")); break;
    case 4: Print(F("Avr.")); break;
    case 5: Print(F("Mai.")); break;
    case 6: Print(F("Juin")); break;
    case 7: Print(F("Juil.")); break;
    case 8: Print(F("Aout")); break;
    case 9: Print(F("Sept.")); break;
    case 10: Print(F("Oct.")); break;
    case 11: Print(F("Nov.")); break;
    case 12: Print(F("Dec.")); break;
  }
  Print(" ");
  Print(now.hour()); Print("h"); if (now.minute() < 10) Print("0"); Println(now.minute());
}
//--------------------------------------------------------
void Print2file(const String s) {
  if (SD.exists(filename)) {
    File myFile = SD.open(filename, FILE_WRITE);
    if (myFile) {
      myFile.println(s); myFile.close();
    } else {
      Println(F("file error."));
    }
  } else {
    Println(F("file error."));
  }
}
//==========================================================
void switch_detected() {
  String m  = (state == 1) ? ("ON,  ") : ("OFF, ");
  Print(m); PrintDate();
  m = m + time2string() + ", " + String(Clock.getTemperature(), 1);
  Print2file(m);
}
//==========================================================

void GetDateFromFile(byte& Year, byte& Month, byte& Day, byte& Hour, byte& Minute, byte& Second,
                     File& f) {
  // The date in the file should be in
  // the order YYMMDDHHMMSS, with an 'x' at the end.
  boolean GotString = false;
  char InChar;
  byte Temp1, Temp2;
  char InString[20];

  byte j = 0;
  while (!GotString) {
    if (f.available()) {
      InChar = f.read();
      InString[j] = InChar;
      j += 1;
      if (InChar == 'x') GotString = true;
    }
  }
  uint8_t c = 0;
  //  Year first
  Temp1 = (byte)InString[c++] - 48;
  Temp2 = (byte)InString[c++] - 48;
  Year = Temp1 * 10 + Temp2;
  // Month
  Temp1 = (byte)InString[c++] - 48;
  Temp2 = (byte)InString[c++] - 48;
  Month = Temp1 * 10 + Temp2;
  // Date
  Temp1 = (byte)InString[c++] - 48;
  Temp2 = (byte)InString[c++] - 48;
  Day = Temp1 * 10 + Temp2;
  // Hour
  Temp1 = (byte)InString[c++] - 48;
  Temp2 = (byte)InString[c++] - 48;
  Hour = Temp1 * 10 + Temp2;
  // Minute
  Temp1 = (byte)InString[c++] - 48;
  Temp2 = (byte)InString[c++] - 48;
  Minute = Temp1 * 10 + Temp2;
  // Second
  Temp1 = (byte)InString[c++] - 48;
  Temp2 = (byte)InString[c++] - 48;
  Second = Temp1 * 10 + Temp2;
}
//==========================================================
void setup() {
  pinMode(signal_PIN, INPUT_PULLUP);
  Serial.begin(9600);

  //--------------------------------------------------------
  Wire.begin(); // Initialize the I2C
  Wire.setClock(400000L); // I2C speed
#if RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else // RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
#endif // RST_PIN >= 0

  oled.setFont(System5x7);
  oled.clear();
#if INCLUDE_SCROLLING == 0
#error INCLUDE_SCROLLING must be non-zero.  Edit SSD1306Ascii.h
#endif //  INCLUDE_SCROLLING
  // Set auto scrolling at end of window.
  oled.setScrollMode(SCROLL_MODE_AUTO);
  Println(F("================="));
  Println(F("== DATA LOGGER =="));
  Println(F("================="));
  PrintDate(); Print(F("Temp. ")); Print(String(Clock.getTemperature(), 1)); Println(" C");

  //--------------------------------------------------------
  Println(F("SD card init"));
  // Wait until the card is present and can be initialized:
  while (!SD.begin(chipSelect)) {
    Print(F("."));
    delay(200);
  }
  Println("Card Ok");
  //--------------------------------------------------------
  if (SD.exists(date_filename)) {
    Print(date_filename); Println(F(" found"));
    byte Year, Month, Date, Hour, Minute, Second;
    File myFile = SD.open(date_filename);
    GetDateFromFile(Year, Month, Date, Hour, Minute, Second, myFile);
    if (Year < 18 || Year > 99 || Month > 12 || Date > 31 || Hour > 24 || Minute > 59 || Second > 59) {
      Println(F("Date/time error"));
    } else {
      Clock.setClockMode(false);  // set to 24h
      Clock.setYear(Year); Clock.setMonth(Month);
      Clock.setDate(Date); //Clock.setDoW(DoW);
      Clock.setHour(Hour); Clock.setMinute(Minute);
      Clock.setSecond(Second);
      //myFile.close();
      Println(F("Date set to:"));
      PrintDate();
      Println(rtc.now().year());
      String new_filename = "NO"; new_filename += date_filename;
      if (myFile.rename(SD.vwd(), new_filename.c_str()))
        Println(F("file renamed."));
      else
        Println(F("rename error"));
    }
  }
  //--------------------------------------------------------
  String m;
  if (SD.exists(filename)) {
    Print(F("File "));
    Println(filename); Println(F("exists"));
    File myFile = SD.open(filename, FILE_WRITE);
    m = "RESTART, "; m = m + time2string();
    Print2file(m);
    myFile.close();
  } else {
    Print(filename); Println(F(" doesn't exist."));
    Println(F("->creating it."));
    File myFile = SD.open(filename, FILE_WRITE);
    m = "CREATED,"; m = m + time2string();
    Print2file(m);
    myFile.close();
  }
  //--------------------------------------------------------
  state = digitalRead(signal_PIN);
}
//==========================================================
unsigned long last_print = 0;
void loop() {
  uint8_t input = digitalRead(signal_PIN);
  if (input != last_state) {
    // memorize the time of this input change
    last_change_time = millis();
  }
  if (input != state)
    if ((millis() - last_change_time) > minimum_time) {
      // the new state has been here for >minimum_time, so it should be considered as the new state
      state = input;
      switch_detected();
    }
  last_state = input;
}
