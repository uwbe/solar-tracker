#include <Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_INA219.h>

//0x31
#define OLED_RESET -1
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
Adafruit_SSD1306 display(OLED_RESET);
bool invert = true;

Adafruit_INA219 ina219;

Servo horizontal; // horizontal servo
int servoh = 90;
int servohLimitHigh = 180;
int servohLimitLow = 0;

Servo vertical; // vertical servo
int servov = 45;
int servovLimitHigh = 95;
int servovLimitLow = 0;

// LDR pin connections
// name = analogpin;
int ldrtop = A0; //LDR top
int ldrright = A3; //LDR rigt
int ldrbottom = A1; //LDR bottom
int ldrleft = A2; //LDR left
int tolerance = 10;
int top = 0;
int bottom = 0;
int left = 0;
int right = 0;

int xpos = 89;
int dir = -1;

bool isSleepMode = false;
unsigned long startMillis;  
long currentAvg = 0;

//INA219 variables
float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;
  float power_mW = 0;

static const unsigned char PROGMEM sun [] = {
0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x43, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80,
0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x01, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00,
0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00,
0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x40, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x80, 0x20, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x80, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x00, 0x40, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x20, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
0x02, 0x00, 0xC0, 0x00, 0x00, 0x00, 0xC0, 0x10, 0x10, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00,
0x04, 0x00, 0x30, 0x00, 0x00, 0x03, 0x00, 0x08, 0x08, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00,
0x04, 0x00, 0x0E, 0x00, 0x00, 0x1C, 0x00, 0x04, 0x04, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00,
0x08, 0x00, 0x01, 0xF0, 0x01, 0xE0, 0x01, 0x02, 0x02, 0x00, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00,
0x10, 0x08, 0x00, 0x0F, 0xFE, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00,
0x20, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00,
0x40, 0x10, 0x40, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x40, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00,
0x40, 0x20, 0x80, 0x00, 0x00, 0x00, 0x40, 0x40, 0x00, 0x20, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00,
0x80, 0x40, 0x80, 0x40, 0x00, 0x80, 0x40, 0x20, 0x00, 0x10, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00,
0x00, 0x01, 0x00, 0x80, 0x80, 0x40, 0x20, 0x20, 0x00, 0x08, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00,
0x00, 0x01, 0x00, 0x80, 0x80, 0x40, 0x20, 0x10, 0x00, 0x04, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00,
0x00, 0x02, 0x01, 0x00, 0x80, 0x40, 0x10, 0x10, 0x00, 0x02, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00,
0x00, 0x04, 0x01, 0x00, 0x80, 0x20, 0x10, 0x08, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
0x00, 0x04, 0x02, 0x00, 0x80, 0x20, 0x08, 0x04, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x08, 0x00, 0x00, 0x80, 0x20, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x10, 0x00, 0x00, 0x80, 0x10, 0x08, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const unsigned char PROGMEM lupe_bmp[] = {
0x07, 0xc0, 0x00, 
0x18, 0x30, 0x00, 
0x20, 0x08, 0x00, 
0x40, 0x04, 0x00, 
0x40, 0x04, 0x00, 
0x80, 0x02, 0x00, 
0x80, 0x02, 0x00, 
0x80, 0x02, 0x00, 
0x80, 0x02, 0x00, 
0x80, 0x02, 0x00, 
0x40, 0x04, 0x00, 
0x40, 0x04, 0x00, 
0x20, 0x0c, 0x00, 
0x18, 0x3e, 0x00, 
0x07, 0xc7, 0x00, 
0x00, 0x03, 0x80, 
0x00, 0x01, 0xc0, 
0x00, 0x00, 0xe0, 
0x00, 0x00, 0x70, 
0x00, 0x00, 0x30
};

static const unsigned char PROGMEM dark [] = {
0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x40, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x0F, 0x78, 0x00, 0x00, 0x03, 0xF1, 0xFC, 0x00,
0x00, 0x40, 0x00, 0x00, 0x50, 0x00, 0x00, 0x01, 0xF0, 0x84, 0x00, 0x3F, 0xFC, 0x0E, 0x03, 0x00,
0x00, 0xA0, 0x00, 0x00, 0xDC, 0x00, 0x00, 0x7E, 0x10, 0x06, 0xFF, 0x40, 0x30, 0x08, 0x00, 0x80,
0x00, 0xA0, 0x00, 0x07, 0x03, 0x00, 0x00, 0x80, 0x00, 0x07, 0x00, 0xC0, 0x00, 0x00, 0x00, 0xF0,
0x07, 0x1C, 0x00, 0x01, 0xDC, 0x00, 0x01, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08,
0x18, 0x03, 0x00, 0x00, 0x50, 0x00, 0x01, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04,
0x07, 0x1C, 0x00, 0x00, 0x20, 0x00, 0x07, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04,
0x00, 0xA0, 0x00, 0x00, 0x20, 0x00, 0x08, 0x00, 0x01, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C,
0x00, 0xA0, 0x20, 0x00, 0x00, 0x00, 0x08, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1C,
0x00, 0x40, 0x20, 0x00, 0x00, 0x00, 0x08, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
0x00, 0x40, 0x50, 0x00, 0x00, 0x00, 0x06, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
0x00, 0x00, 0x50, 0x00, 0x00, 0x40, 0x04, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, 0x02,
0x00, 0x03, 0x8E, 0x00, 0x00, 0x40, 0x04, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x02, 0x0C,
0x00, 0x0C, 0x01, 0x80, 0x00, 0xA0, 0x06, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xF0,
0x00, 0x03, 0x8E, 0x00, 0x00, 0xA0, 0x01, 0xC0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
0x00, 0x00, 0x50, 0x00, 0x07, 0x1C, 0x00, 0xC1, 0x01, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
0x00, 0x00, 0x50, 0x00, 0x18, 0x03, 0x00, 0x7F, 0x82, 0x7F, 0x00, 0x00, 0x00, 0x08, 0x0C, 0x00,
0x00, 0x00, 0x20, 0x00, 0x07, 0x1C, 0x00, 0x00, 0x7C, 0x06, 0x00, 0x00, 0x00, 0x1F, 0xF0, 0x00,
0x00, 0x00, 0x20, 0x00, 0x00, 0xA0, 0x00, 0x00, 0x00, 0x01, 0xE1, 0xF8, 0x00, 0x30, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x07, 0x03, 0xC0, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


void setup() {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  splashScreen();
  ina219.begin();
  pinMode (ldrtop, INPUT);                                  // Config the mocrocontroller pins as an input pins.
  pinMode (ldrright, INPUT);
  pinMode (ldrbottom, INPUT);
  pinMode (ldrleft, INPUT);
  horizontal.attach(2);
  vertical.attach(3);
  vertical.write(servov);
  delay(1000);
  horizontal.write(servoh);
  delay(500);
  startMillis = millis();
}

void loop() {
  top = analogRead(ldrtop);
  right = analogRead(ldrright);
  left = analogRead(ldrleft);
  bottom = analogRead(ldrbottom);
  readINA219();
  
  if (isToDark() && millis() - startMillis  > 10000) {
    sleepMode();
    logSerial();
    return;
  } else {
    if (isSleepMode) {
      wakeUp();
    }
  }
  logSerial();
  if (inRange()){
    info();
  } else {
    //info();
    searchScreen();
  }
  verticalMove();
  horizontalMove();
  delay(500);
}

void horizontalMove() {
  if (abs(left - right) < tolerance) return;
  if (left < right)
  {
    servoh = --servoh;
    if (servoh <= servohLimitLow)
    {
      servoh = servohLimitLow;
      Serial.println("max east!");
    }
  }
  else if (left > right)
  {
    servoh = ++servoh;
    if (servoh > servohLimitHigh)
    {
      Serial.println("max west!");
      servoh = servohLimitHigh;
    }
  }
  else if (left = right)
  {
    delay(800);
  }
  horizontal.write(servoh);
}

void verticalMove() {
  if (abs(top - bottom) < tolerance) return;
  if (top > bottom) {
    servov = ++servov;
    if (servov > servovLimitHigh) {
      Serial.print("max height!: ");
      Serial.println(servov);
      servov = servovLimitHigh;
    }
  }
  else if (top < bottom) {
    servov = --servov;
    if (servov < servovLimitLow) {
      servov = servovLimitLow;
      Serial.println("min height!");
    }
  }
  vertical.write(servov);
}

bool inRange() {
  return abs(top - bottom) < tolerance * 3 && abs(left - right) < tolerance * 3;
}

bool isToDark() {
  currentAvg = (top + bottom + left + right) / 4;
  return currentAvg < 200; 
}

void sleepMode() {
  if (!isSleepMode) {
    servov = 85;
    vertical.write(servov);
  }
  isSleepMode = true;
  darkScreen();
}

void wakeUp() {
  isSleepMode = false;
  servov = 45;
  vertical.write(servov);
}

void readINA219() {
  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);
}

void logSerial() {
  Serial.print("top/bottom: ");
  Serial.print(top);
  Serial.print("/");
  Serial.print(bottom);
  Serial.print("/");
  Serial.println(abs(top - bottom));
  Serial.print("right/left: ");
  Serial.print(right);
  Serial.print("/");
  Serial.print(left);
  Serial.print("/");
  Serial.println(abs(left - right));
  Serial.println("");
  Serial.print("Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
  Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage); Serial.println(" mV");
  Serial.print("Load Voltage:  "); Serial.print(loadvoltage); Serial.println(" V");
  Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
  Serial.print("Power:         "); Serial.print(power_mW); Serial.println(" mW");
  Serial.println();
}

void info() {
  display.clearDisplay();
  display.setCursor(0,0);
  //display.println("     Zachi Zolar");
  display.print("ob/unt/d: ");
  display.print(top);
  display.print("/");
  display.print(bottom);
  display.print("/");
  display.println(abs(top - bottom));
  display.print("re/li/d:  ");
  display.print(right);
  display.print("/");
  display.print(left);
  display.print("/");
  display.println(abs(left - right));
  display.print("pos h/v:  ");
  display.print(servoh);
  display.print(" - ");
  display.println(servov);
  display.print("mW/mA: ");
  display.print(power_mW);
  display.print("/");
  display.print(current_mA);
  display.display();
}

void splashScreen() {
  display.clearDisplay();
  display.drawBitmap(0, 0,  sun, 128, 32, WHITE);
  display.setCursor(20,1);
  display.print("Zachi");
  display.setCursor(91,7);
  display.print("Zolar");
  display.display();
}

void searchScreen() {
  if (xpos > 100 || xpos < 5) {
    dir = dir * -1;
  }
  xpos = xpos + dir * 2;
  display.clearDisplay();
  display.drawBitmap(0, 0,  sun, 128, 32, WHITE);
  display.drawBitmap(xpos, 12,  lupe_bmp, 20, 20, WHITE);
  display.setCursor(20,1);
  display.print("Suche");
  display.display();
}

void darkScreen() {
  if (xpos > 90 || xpos < 5) {
    dir = dir * -1;
  }
  xpos = xpos + dir * 2;
  display.clearDisplay();
  display.drawBitmap(0, 0,  dark, 128, 32, WHITE);
  display.setCursor(xpos,24);
  display.print("Snooze");
  display.setCursor(88, 7);
  display.print(currentAvg);
  display.display();
}
