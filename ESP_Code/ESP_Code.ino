#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int joyA1_X = A0;
const int joyA1_SW = 35;
enum Difficulty { EASY = 0, MEDIUM = 1, HARD = 2 };
Difficulty selectedDifficulty = EASY;
const char* DIFFICULTY_NAMES[3] = {"EASY", "MEDIUM", "HARD"};
bool difficultySelected = false;

void setup() {
  pinMode(joyA1_SW, INPUT_PULLUP);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  pinMode(joyA1_X, INPUT);
}

void loop() {
  if (!difficultySelected) {
    difficultySelectionMenu();
    delay(50);
  }
}

void difficultySelectionMenu() {
  static unsigned long lastUpdate = 0;
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(15, 0);
  display.println("Select");
  display.setTextSize(3);
  display.setCursor(14, 28);
  display.print(DIFFICULTY_NAMES[selectedDifficulty]);
  display.display();

  int joyX1 = analogRead(joyA1_X);

  if (millis() - lastUpdate > 200) {
    if ((joyX1 > 700) && selectedDifficulty < HARD) {
      selectedDifficulty = (Difficulty)(selectedDifficulty + 1);
      lastUpdate = millis();
    }
    if ((joyX1 < 300) && selectedDifficulty > EASY) {
      selectedDifficulty = (Difficulty)(selectedDifficulty - 1);
      lastUpdate = millis();
    }
    if (digitalRead(joyA1_SW) == LOW) {
      difficultySelected = true;
      display.clearDisplay(); display.setTextSize(2);
      display.setCursor(14,24);
      display.println(DIFFICULTY_NAMES[selectedDifficulty]);
      display.display();
      delay(1000);
    }
  }
}