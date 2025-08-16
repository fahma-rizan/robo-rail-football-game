#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>

// ====== ROBOT / MOTORS / STEPPERS / JOYSTICKS ======
const int joyA1_X = A0;
const int joyA2_X = A1;
const int joyB1_Y = A3;
const int joyB2_Y = A2;
const int joyA1_SW = 35;
const int joyA2_SW = 37;

const int motorA1 = 31, motorA2 = 29, enableA = 33;
const int motorB1 = 27, motorB2 = 25, enableB = 23;
const int DIR1 = 51, STEP1 = 49, DIR2 = 47, STEP2 = 45;
const int center = 512, threshold = 100, motorSpeed = 200;

unsigned long prevStepTime1 = 0, prevStepTime2 = 0, stepInterval1 = 1000, stepInterval2 = 1000;
bool stepState1 = false, stepState2 = false;
unsigned long lastPrint = 0;
const unsigned long printInterval = 500;

// ====== PAUSE BUTTON ======
const int pauseButtonPin = 44;
bool paused = false;
bool pauseBtnPrevState = HIGH; // For edge detection

// ====== UNDO BUTTON ======
const int undoButtonPin = 46;
bool undoBtnPrevState = HIGH;
int prevPlayer1Score = 0;
int prevPlayer2Score = 0;

// ====== LED STRIP CONTROL ======
const int ledStripPin = 48;
bool ledStripNormallyOn = true;  // LED strip should normally be ON

// ========== ULTRASONIC & SERVO ==========
#define TRIG_A 8
#define ECHO_A 9
#define TRIG_B 6
#define ECHO_B 7
#define TRIG_C 4
#define ECHO_C 5
#define TRIG_D 2
#define ECHO_D 3
#define SERVO_LEFT_PIN 11
#define SERVO_RIGHT_PIN 12
Servo servoLeft, servoRight;
unsigned long detectionStartTime = 0;
bool objectDetected = false;
const int joyA2_SW_EXTRA = 39, joyB2_SW_EXTRA = 41, SERVO1_PIN = 38, SERVO2_PIN = 40;
Servo servo1, servo2; const int IR1_PIN = 32, IR2_PIN = 30;

// ========== DUAL OLED SCOREBOARD & SCORING ==========
const int ldrScorePin1 = 22, ldrScorePin2 = 24, scoreBuzzerPin = 42;
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 scoreDisplay1(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // 0x3C
Adafruit_SSD1306 scoreDisplay2(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // 0x3D

int player1Score = 0, player2Score = 0;
bool player1BeamWasBroken = false, player2BeamWasBroken = false;
bool scoreCooldownActive = false;
unsigned long scoreCooldownStart = 0;
const unsigned long scoreCooldownPeriod = 5000;

// ========== DIFFICULTY SELECTION ==========
enum Difficulty { EASY = 0, MEDIUM = 1, HARD = 2 };
Difficulty selectedDifficulty = EASY;
const char* DIFFICULTY_NAMES[3] = {"EASY", "MEDIUM", "HARD"};
int WIN_SCORE_OPTIONS[3] = {5, 6, 7};
int WIN_SCORE = 5;
bool difficultySelected = false;
bool gameEnded = false;
bool firstScoresShown = false;

// ========== SPLASH SCREEN FLAG ==========
bool waitingForPlayButton = true;

// ====== WiFi ESP MODULE COMMUNICATION ======
// Using Serial3 for ESP32 communication (pins 14, 15 on Arduino Mega)

// ========== LED STRIP FUNCTIONS ==========
void ledStripScoreBlink() {
  // Blink two times for scoring
  for (int i = 0; i < 2; i++) {
    digitalWrite(ledStripPin, LOW);   // Turn OFF
    delay(150);                       // OFF for 150ms
    digitalWrite(ledStripPin, HIGH);  // Turn ON
    if (i < 1) delay(150);           // ON for 150ms (except after last blink)
  }
  // LED strip remains ON after blinking
}

void ledStripWinnerBlink() {
  // Blink 5 times in 5 seconds for winner
  for (int i = 0; i < 5; i++) {
    digitalWrite(ledStripPin, LOW);   // Turn OFF
    delay(500);                       // OFF for 0.5 seconds
    digitalWrite(ledStripPin, HIGH);  // Turn ON
    if (i < 4) delay(500);           // ON for 0.5 seconds (except last blink)
  }
  // LED strip remains ON after blinking
}

// ========== ESP32 SCORE TRANSFER ==========
void sendScoresToESP32() {
  // Send scores in format: "P1:score1,P2:score2"
  String scoreData = "P1:" + String(player1Score) + ",P2:" + String(player2Score);
  Serial3.println(scoreData);
  Serial.print("Sent to ESP32: ");
  Serial.println(scoreData);
}

// ========== SETUP ==========
void setup() {
  pinMode(motorA1, OUTPUT); pinMode(motorA2, OUTPUT); pinMode(enableA, OUTPUT);
  pinMode(motorB1, OUTPUT); pinMode(motorB2, OUTPUT); pinMode(enableB, OUTPUT);
  pinMode(DIR1, OUTPUT);   pinMode(STEP1, OUTPUT); pinMode(DIR2, OUTPUT); pinMode(STEP2, OUTPUT);

  pinMode(joyA1_SW, INPUT_PULLUP); pinMode(joyA2_SW, INPUT_PULLUP);
  stopMotor(motorA1, motorA2, enableA); stopMotor(motorB1, motorB2, enableB);

  pinMode(pauseButtonPin, INPUT_PULLUP); // PAUSE button
  pinMode(undoButtonPin, INPUT_PULLUP);  // UNDO button
  
  // LED Strip setup
  pinMode(ledStripPin, OUTPUT);
  digitalWrite(ledStripPin, HIGH);  // Turn ON LED strip initially

  Serial.begin(9600);
  Serial3.begin(9600);    // Serial3 to ESP32 for score transfer

  pinMode(TRIG_A, OUTPUT); pinMode(ECHO_A, INPUT);
  pinMode(TRIG_B, OUTPUT); pinMode(ECHO_B, INPUT);
  pinMode(TRIG_C, OUTPUT); pinMode(ECHO_C, INPUT);
  pinMode(TRIG_D, OUTPUT); pinMode(ECHO_D, INPUT);

  servoLeft.attach(SERVO_LEFT_PIN); servoRight.attach(SERVO_RIGHT_PIN);
  servoLeft.write(0); servoRight.write(0);
  pinMode(joyA2_SW_EXTRA, INPUT_PULLUP); pinMode(joyB2_SW_EXTRA, INPUT_PULLUP);
  servo1.attach(SERVO1_PIN); servo2.attach(SERVO2_PIN); servo1.write(0); servo2.write(0);
  pinMode(IR1_PIN, INPUT); pinMode(IR2_PIN, INPUT);
  pinMode(ldrScorePin1, INPUT); pinMode(ldrScorePin2, INPUT); pinMode(scoreBuzzerPin, OUTPUT);

  Wire.begin();

  if(!scoreDisplay1.begin(SSD1306_SWITCHCAPVCC, 0x3C)) for(;;);
  if(!scoreDisplay2.begin(SSD1306_SWITCHCAPVCC, 0x3D)) for(;;);
  
  // Send initial scores to ESP32
  sendScoresToESP32();
}

// ========== MAIN LOOP ==========
void loop() {
  // ------ SPLASH: Stay until player presses SW ------
  while(waitingForPlayButton) {
    splashScoreDisplays();
    if (digitalRead(joyA1_SW) == LOW || digitalRead(joyA2_SW) == LOW) {
      delay(200); // debounce
      waitingForPlayButton = false;
      difficultySelected = false;
      firstScoresShown = false;
    }
    delay(50);
    return;
  }

  // ------ PAUSE FUNCTIONALITY ------
  static bool prevPaused = false;
  bool pauseBtnNow = digitalRead(pauseButtonPin);
  if (pauseBtnPrevState == HIGH && pauseBtnNow == LOW) { // just pressed (active low)
    paused = !paused;
    delay(150); // debounce
  }
  pauseBtnPrevState = pauseBtnNow;

  // Show "RESUMED" briefly on unpause, then immediately redraw
  if (prevPaused && !paused) {
    showResumeScreen();
    delay(800);
    // Immediately refresh the display to game state/menu state after RESUMED
    if (!difficultySelected) {
      difficultySelectionMenu();
    } else if (!firstScoresShown) {
      showScoresOnBoth();
      firstScoresShown = true;
    } else if (!gameEnded) {
      showScoresOnBoth();
    }
    delay(50);
  }
  prevPaused = paused;

  if (paused) {
    showPauseScreen();
    return;
  }

  // ------ DIFFICULTY SELECTION --------
  if (!difficultySelected) {
    difficultySelectionMenu();
    firstScoresShown = false;
    return;
  }
  if (!firstScoresShown) {
    showScoresOnBoth();
    firstScoresShown = true;
  }

  // ------ MAIN GAME --------
  if (!gameEnded) {
    int xA1 = analogRead(joyA1_X), xA2 = analogRead(joyA2_X);
    int yB1 = analogRead(joyB1_Y), yB2 = analogRead(joyB2_Y);
    bool swA1 = digitalRead(joyA1_SW) == LOW, swA2 = digitalRead(joyA2_SW) == LOW;
    controlStepper(yB1, DIR1, STEP1, prevStepTime1, stepInterval1, stepState1);
    controlStepper(yB2, DIR2, STEP2, prevStepTime2, stepInterval2, stepState2);
    controlMotor(xA1, motorA1, motorA2, enableA);
    controlMotor(xA2, motorB1, motorB2, enableB);

    if (millis() - lastPrint >= printInterval) {
      Serial.print("SW: A1="); Serial.print(swA1 ? "ON" : "OFF");
      Serial.print(" A2="); Serial.println(swA2 ? "ON" : "OFF");
      lastPrint = millis();
    }

    long distA = getDistance(TRIG_A, ECHO_A);
    long distB = getDistance(TRIG_B, ECHO_B);
    long distC = getDistance(TRIG_C, ECHO_C);
    long distD = getDistance(TRIG_D, ECHO_D);

    bool isClose = (distA < 30 && distA > 0) || (distB < 30 && distB > 0) ||
                   (distC < 30 && distC > 0) || (distD < 30 && distD > 0);

    // Ultrasonic triggers for servoLeft/servoRight (unchanged)
    if (isClose) {
      if (!objectDetected) {
        objectDetected = true;
        detectionStartTime = millis();
      } else if (millis() - detectionStartTime > 3000) {
        servoLeft.write(90); servoRight.write(90);
        delay(500); servoLeft.write(0); servoRight.write(0);
        objectDetected = false;
      }
    } else {
      objectDetected = false;
    }

    // --- BUTTON-BASED SERVO CONTROL (always allowed) ---
    bool btnA1_extra = (digitalRead(joyA1_SW) == LOW);
    bool btnA2_extra = (digitalRead(joyA2_SW_EXTRA) == LOW);
    if (btnA1_extra) { servo1.write(90); delay(200); servo1.write(0); }
    if (btnA2_extra) { servo2.write(90); delay(200); servo2.write(0); }

    // --- IR SENSOR SERVO CONTROL: ONLY IN EASY MODE ---
    if (selectedDifficulty == EASY) {
      if (digitalRead(IR1_PIN) == LOW) { servo1.write(90); delay(200); servo1.write(0); delay(200);}
      if (digitalRead(IR2_PIN) == LOW) { servo2.write(90); delay(200); servo2.write(0); delay(200);}
    }

    // ------ UNDO BUTTON LOGIC ------
    bool undoBtnNow = digitalRead(undoButtonPin);
    if (undoBtnPrevState == HIGH && undoBtnNow == LOW) { // just pressed
      if (player1Score != prevPlayer1Score || player2Score != prevPlayer2Score) {
        player1Score = prevPlayer1Score;
        player2Score = prevPlayer2Score;
        showScoresOnBoth();
        sendScoresToESP32();  // Send updated scores to ESP32
      }
      delay(200); // debounce
    }
    undoBtnPrevState = undoBtnNow;
  }

  // ------ SCORING, WIN/RESTART ------
  unsigned long now = millis();
  if (!gameEnded) {
    if (scoreCooldownActive && (now - scoreCooldownStart >= scoreCooldownPeriod)) {
      scoreCooldownActive = false;
    }
    if (!scoreCooldownActive) {
      int beam1 = digitalRead(ldrScorePin1), beam2 = digitalRead(ldrScorePin2);
      if (beam1 == HIGH && !player1BeamWasBroken) {
        // Save previous score for undo
        prevPlayer1Score = player1Score;
        prevPlayer2Score = player2Score;
        player1Score++; 
        player1BeamWasBroken = true; 
        showScoresOnBoth(); 
        playScoreSound(); 
        setScoreCooldown(now);
        sendScoresToESP32();  // Send updated scores to ESP32
        ledStripScoreBlink(); // Blink LED strip two times for scoring
      } else if (beam1 == LOW) { 
        player1BeamWasBroken = false; 
      }
      if (beam2 == HIGH && !player2BeamWasBroken) {
        // Save previous score for undo
        prevPlayer1Score = player1Score;
        prevPlayer2Score = player2Score;
        player2Score++; 
        player2BeamWasBroken = true; 
        showScoresOnBoth(); 
        playScoreSound(); 
        setScoreCooldown(now);
        sendScoresToESP32();  // Send updated scores to ESP32
        ledStripScoreBlink(); // Blink LED strip two times for scoring
      } else if (beam2 == LOW) { 
        player2BeamWasBroken = false; 
      }
    }
    if ((player1Score >= WIN_SCORE) || (player2Score >= WIN_SCORE)) {
      showWinnerOnBoth();
      ledStripWinnerBlink(); // Blink LED strip 5 times for winner
      gameEnded = true; 
      delay(5000); // Reduced delay since LED blinking takes 5 seconds  
      player1Score = 0; 
      player2Score = 0; 
      player1BeamWasBroken = false; 
      player2BeamWasBroken = false;
      scoreCooldownActive = false;
      sendScoresToESP32();  // Send reset scores to ESP32
      showRestartSplash(); 
      delay(1500);
      waitingForPlayButton = true;
      difficultySelected = false;
      firstScoresShown = false;
      gameEnded = false;
    }
  }
}

// ========== SPLASH SCREEN ==========
void splashScoreDisplays() {
  scoreDisplay1.clearDisplay();
  scoreDisplay1.setTextSize(2);
  scoreDisplay1.setTextColor(SSD1306_WHITE);
  scoreDisplay1.setCursor(10, 8);
  scoreDisplay1.println("ROBORAIL");
  scoreDisplay1.setCursor(10, 30);
  scoreDisplay1.println("FOOTBALL");
  scoreDisplay1.setTextSize(1);
  scoreDisplay1.setCursor(34, 54);
  scoreDisplay1.println("> PLAY <");
  scoreDisplay1.display();

  scoreDisplay2.clearDisplay();
  scoreDisplay2.setTextSize(2);
  scoreDisplay2.setTextColor(SSD1306_WHITE);
  scoreDisplay2.setCursor(10, 8);
  scoreDisplay2.println("ROBORAIL");
  scoreDisplay2.setCursor(10, 30);
  scoreDisplay2.println("FOOTBALL");
  scoreDisplay2.setTextSize(1);
  scoreDisplay2.setCursor(34, 54);
  scoreDisplay2.println("> PLAY <");
  scoreDisplay2.display();
}

// ========== PAUSE/RESUME SCREEN ==========
void showPauseScreen() {
  scoreDisplay1.clearDisplay();
  scoreDisplay1.setTextSize(3);
  scoreDisplay1.setCursor(15, 20);
  scoreDisplay1.println("PAUSED");
  scoreDisplay1.display();

  scoreDisplay2.clearDisplay();
  scoreDisplay2.setTextSize(3);
  scoreDisplay2.setCursor(15, 20);
  scoreDisplay2.println("PAUSED");
  scoreDisplay2.display();
}

void showResumeScreen() {
  scoreDisplay1.clearDisplay();
  scoreDisplay1.setTextSize(2);
  scoreDisplay1.setCursor(14, 24);
  scoreDisplay1.println("RESUMED");
  scoreDisplay1.display();

  scoreDisplay2.clearDisplay();
  scoreDisplay2.setTextSize(2);
  scoreDisplay2.setCursor(14, 24);
  scoreDisplay2.println("RESUMED");
  scoreDisplay2.display();
}

// ========== DIFFICULTY SELECTION MENU ==========
void difficultySelectionMenu() {
  static unsigned long lastUpdate = 0;
  static bool firstEntry = true;
  if (firstEntry) {
    selectedDifficulty = EASY;
    firstEntry = false;
  }
  scoreDisplay1.clearDisplay();
  scoreDisplay1.setTextSize(2);
  scoreDisplay1.setCursor(15, 0);
  scoreDisplay1.println("Select");
  scoreDisplay1.setTextSize(3);
  scoreDisplay1.setCursor(14, 28);
  scoreDisplay1.print(DIFFICULTY_NAMES[selectedDifficulty]);
  scoreDisplay1.display();

  scoreDisplay2.clearDisplay();
  scoreDisplay2.setTextSize(3);
  scoreDisplay2.setCursor(14, 16);
  scoreDisplay2.print(DIFFICULTY_NAMES[selectedDifficulty]);
  scoreDisplay2.display();

  int joyX1 = analogRead(joyA1_X);
  int joyX2 = analogRead(joyA2_X);

  if (millis() - lastUpdate > 200) {
    if ((joyX1 > 700 || joyX2 > 700) && selectedDifficulty < HARD) {
      selectedDifficulty = static_cast<Difficulty>(selectedDifficulty + 1);
      lastUpdate = millis();
    }
    if ((joyX1 < 300 || joyX2 < 300) && selectedDifficulty > EASY) {
      selectedDifficulty = static_cast<Difficulty>(selectedDifficulty - 1);
      lastUpdate = millis();
    }
    if (digitalRead(joyA1_SW) == LOW || digitalRead(joyA2_SW) == LOW) {
      difficultySelected = true;
      WIN_SCORE = WIN_SCORE_OPTIONS[selectedDifficulty];
      scoreDisplay1.clearDisplay(); scoreDisplay1.setTextSize(2);
      scoreDisplay1.setCursor(14,24);
      scoreDisplay1.println(DIFFICULTY_NAMES[selectedDifficulty]);
      scoreDisplay1.display();

      scoreDisplay2.clearDisplay(); scoreDisplay2.setTextSize(2);
      scoreDisplay2.setCursor(14,24);
      scoreDisplay2.println(DIFFICULTY_NAMES[selectedDifficulty]);
      scoreDisplay2.display();

      delay(1000);
      firstEntry = true;
    }
  }
}

// ========== SCORING, WIN, & HELPERS ==========
void showScoresOnBoth() {
  scoreDisplay1.clearDisplay(); scoreDisplay1.setTextSize(2); scoreDisplay1.setCursor(0, 0);
  scoreDisplay1.print("P1: "); scoreDisplay1.println(player1Score); scoreDisplay1.setCursor(0, 32);
  scoreDisplay1.print("P2: "); scoreDisplay1.println(player2Score); scoreDisplay1.setCursor(0, 56);
  scoreDisplay1.print("LVL:"); scoreDisplay1.print(DIFFICULTY_NAMES[selectedDifficulty]);
  scoreDisplay1.display();
  scoreDisplay2.clearDisplay(); scoreDisplay2.setTextSize(2); scoreDisplay2.setCursor(0, 0);
  scoreDisplay2.print("P1: "); scoreDisplay2.println(player1Score); scoreDisplay2.setCursor(0, 32);
  scoreDisplay2.print("P2: "); scoreDisplay2.println(player2Score); scoreDisplay2.setCursor(0, 56);
  scoreDisplay2.print("LVL:"); scoreDisplay2.print(DIFFICULTY_NAMES[selectedDifficulty]);
  scoreDisplay2.display();
}

void showWinnerOnBoth() {
  scoreDisplay1.clearDisplay();
  scoreDisplay1.setTextSize(2);
  scoreDisplay1.setCursor(0, 0);
  if (player1Score > player2Score) {
    scoreDisplay1.println("WINNER:"); scoreDisplay1.setCursor(0,30); scoreDisplay1.println("PLAYER 1");
  } else if (player2Score > player1Score) {
    scoreDisplay1.println("WINNER:"); scoreDisplay1.setCursor(0,30); scoreDisplay1.println("PLAYER 2");
  } else { scoreDisplay1.println("DRAW"); }
  scoreDisplay1.display();
  scoreDisplay2.clearDisplay(); scoreDisplay2.setTextSize(2); scoreDisplay2.setCursor(0, 0);
  if (player1Score > player2Score) {
    scoreDisplay2.println("WINNER:"); scoreDisplay2.setCursor(0,30); scoreDisplay2.println("PLAYER 1");
  } else if (player2Score > player1Score) {
    scoreDisplay2.println("WINNER:"); scoreDisplay2.setCursor(0,30); scoreDisplay2.println("PLAYER 2");
  } else { scoreDisplay2.println("DRAW"); }
  scoreDisplay2.display();
}

void showRestartSplash() {
  scoreDisplay1.clearDisplay();
  scoreDisplay1.setTextSize(2);
  scoreDisplay1.setCursor(0, 0);
  scoreDisplay1.println(" New Game ");
  scoreDisplay1.setCursor(0,28);
  scoreDisplay1.println(" Get Ready!");
  scoreDisplay1.display();

  scoreDisplay2.clearDisplay();
  scoreDisplay2.setTextSize(2);
  scoreDisplay2.setCursor(0, 0);
  scoreDisplay2.println(" New Game ");
  scoreDisplay2.setCursor(0,28);
  scoreDisplay2.println(" Get Ready!");
  scoreDisplay2.display();
}

void controlMotor(int val, int pin1, int pin2, int pwmPin) {
  int diff = val - center;
  if (abs(diff) > threshold) {
    if (diff > 0) {
      digitalWrite(pin1, HIGH); digitalWrite(pin2, LOW); analogWrite(pwmPin, motorSpeed);
    } else {
      digitalWrite(pin1, LOW); digitalWrite(pin2, HIGH); analogWrite(pwmPin, motorSpeed);
    }
  } else {
    stopMotor(pin1, pin2, pwmPin);
  }
}

void controlStepper(int yVal, int dirPin, int stepPin, unsigned long &prevTime, unsigned long &interval, bool &stepState) {
  int delta = yVal - 512;
  if (abs(delta) > 100) {
    digitalWrite(dirPin, delta > 0 ? LOW : HIGH);
    interval = map(abs(delta), 100, 512, 1500, 200);
    if (micros() - prevTime >= interval) {
      prevTime = micros(); stepState = !stepState;
      digitalWrite(stepPin, stepState);
    }
  } else { digitalWrite(stepPin, LOW); }
}

void stopMotor(int pin1, int pin2, int pwmPin) {
  digitalWrite(pin1, LOW); digitalWrite(pin2, LOW); analogWrite(pwmPin, 0);
}

long getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW); delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); delayMicroseconds(10); digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000);
  return duration * 0.034 / 2;
}

void playScoreSound() { 
  tone(scoreBuzzerPin, 1000, 300); 
}

void setScoreCooldown(unsigned long now) {
  scoreCooldownActive = true; 
  scoreCooldownStart = now;
  player1BeamWasBroken = true; 
  player2BeamWasBroken = true;
}