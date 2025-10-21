#include <LiquidCrystal.h>

// LCD pins - adjust these to match your wiring
// Format: LiquidCrystal(RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Button pins
const int hourButton = 8;
const int minuteButton = 9;
const int resetButton = 10;
const int modeButton = 7;

// Buzzer pin
const int buzzerPin = 6;

// Time variables
int hours = 12;
int minutes = 0;
int seconds = 0;

// Mode: 0 = display time, 1 = set hours, 2 = set minutes
int mode = 0;

// Button state variables
bool hourButtonPressed = false;
bool minuteButtonPressed = false;
bool resetButtonPressed = false;
bool modeButtonPressed = false;

// Timing variables
unsigned long previousMillis = 0;
unsigned long blinkMillis = 0;
const long interval = 1000; // 1 second
bool blinkState = true;

void setup() {
  // Initialize Serial for debugging
  Serial.begin(9600);
  Serial.println("Starting Clock...");
  
  // Setup button pins with pullup resistors
  pinMode(hourButton, INPUT_PULLUP);
  pinMode(minuteButton, INPUT_PULLUP);
  pinMode(resetButton, INPUT_PULLUP);
  pinMode(modeButton, INPUT_PULLUP);
  
  // Setup buzzer pin
  pinMode(buzzerPin, OUTPUT);
  
  // Initialize LCD with clear steps
  delay(100);
  lcd.begin(16, 2);
  delay(100);
  
  // Test LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LCD Test");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Real Time Clock");
  lcd.setCursor(0, 1);
  lcd.print("Ready!");
  delay(1500);
  lcd.clear();
  
  Serial.println("LCD Initialized");
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Read button states (buttons connected to GND, so LOW = pressed)
  bool hourBtn = !digitalRead(hourButton);
  bool minBtn = !digitalRead(minuteButton);
  bool resetBtn = !digitalRead(resetButton);
  bool modeBtn = !digitalRead(modeButton);
  
  // Mode button - toggle between display and set modes
  if (modeBtn && !modeButtonPressed) {
    modeButtonPressed = true;
    delay(50); // Debounce
    mode = (mode + 1) % 3;
    lcd.clear();
    tone(buzzerPin, 1000, 100);
    Serial.print("Mode changed to: ");
    Serial.println(mode);
  }
  if (!modeBtn) modeButtonPressed = false;
  
  // Hour button
  if (hourBtn && !hourButtonPressed) {
    hourButtonPressed = true;
    delay(50); // Debounce
    hours = (hours + 1) % 24;
    tone(buzzerPin, 800, 50);
    Serial.print("Hours: ");
    Serial.println(hours);
  }
  if (!hourBtn) hourButtonPressed = false;
  
  // Minute button
  if (minBtn && !minuteButtonPressed) {
    minuteButtonPressed = true;
    delay(50); // Debounce
    minutes = (minutes + 1) % 60;
    tone(buzzerPin, 800, 50);
    Serial.print("Minutes: ");
    Serial.println(minutes);
  }
  if (!minBtn) minuteButtonPressed = false;
  
  // Reset button
  if (resetBtn && !resetButtonPressed) {
    resetButtonPressed = true;
    delay(50); // Debounce
    hours = 0;
    minutes = 0;
    seconds = 0;
    mode = 0;
    lcd.clear();
    tone(buzzerPin, 1500, 200);
    Serial.println("Clock Reset");
  }
  if (!resetBtn) resetButtonPressed = false;
  
  // Update seconds (only in display mode)
  if (mode == 0 && currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    seconds++;
    if (seconds >= 60) {
      seconds = 0;
      minutes++;
      if (minutes >= 60) {
        minutes = 0;
        hours++;
        if (hours >= 24) {
          hours = 0;
        }
        // Beep on hour change
        tone(buzzerPin, 1200, 500);
      }
    }
  }
  
  // Display based on mode
  if (mode == 0) {
    displayTime();
  } else if (mode == 1) {
    displaySetHours();
  } else if (mode == 2) {
    displaySetMinutes();
  }
  
  delay(100); // Reduce loop speed
}

void displayTime() {
  lcd.setCursor(0, 0);
  lcd.print("   TIME NOW    ");
  
  lcd.setCursor(4, 1);
  if (hours < 10) lcd.print("0");
  lcd.print(hours);
  lcd.print(":");
  if (minutes < 10) lcd.print("0");
  lcd.print(minutes);
  lcd.print(":");
  if (seconds < 10) lcd.print("0");
  lcd.print(seconds);
  lcd.print("   ");
}

void displaySetHours() {
  lcd.setCursor(0, 0);
  lcd.print("  SET HOURS    ");
  
  // Blink hours
  if (millis() - blinkMillis > 500) {
    blinkMillis = millis();
    blinkState = !blinkState;
  }
  
  lcd.setCursor(4, 1);
  if (blinkState) {
    if (hours < 10) lcd.print("0");
    lcd.print(hours);
  } else {
    lcd.print("  ");
  }
  lcd.print(":");
  if (minutes < 10) lcd.print("0");
  lcd.print(minutes);
  lcd.print(":");
  if (seconds < 10) lcd.print("0");
  lcd.print(seconds);
  lcd.print("   ");
}

void displaySetMinutes() {
  lcd.setCursor(0, 0);
  lcd.print(" SET MINUTES   ");
  
  // Blink minutes
  if (millis() - blinkMillis > 500) {
    blinkMillis = millis();
    blinkState = !blinkState;
  }
  
  lcd.setCursor(4, 1);
  if (hours < 10) lcd.print("0");
  lcd.print(hours);
  lcd.print(":");
  if (blinkState) {
    if (minutes < 10) lcd.print("0");
    lcd.print(minutes);
  } else {
    lcd.print("  ");
  }
  lcd.print(":");
  if (seconds < 10) lcd.print("0");
  lcd.print(seconds);
  lcd.print("   ");
}