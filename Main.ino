#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define STEP_PIN 3
#define DIR_PIN 6
#define ENABLE_PIN 8

#define SPEED_PIN 11
#define LEFT_PIN 10
#define RIGHT_PIN 9
#define HOLD_PIN 1 // New pin for holding button

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int speed = 5; // Initial speed
bool movingLeft = false;
bool movingRight = false;
bool Holding = false; // Variable for holding button

void setup() {
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);
  pinMode(SPEED_PIN, INPUT_PULLUP); // Change to INPUT_PULLUP
  pinMode(LEFT_PIN, INPUT_PULLUP);
  pinMode(RIGHT_PIN, INPUT_PULLUP);
  pinMode(HOLD_PIN, INPUT_PULLUP); // New pin mode

  digitalWrite(ENABLE_PIN, HIGH); // Keep motor disabled initially

  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.display();
  delay(1000);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Speed: ");
  display.print(speed);
  displayStandbyStatus(); // Update standby status
  updateHoldingStatus(); // Update holding status
  display.display();
}

void loop() {
  handleSpeedButton();
  handleDirectionButtons();
  handleHoldingButton(); // New function call
}

void handleSpeedButton() {
  int buttonState = digitalRead(SPEED_PIN);
  if (buttonState == HIGH) { // Button pressed
    speed++;
    if (speed > 201) { // Check if speed exceeds 201
      speed = 0; // Reset speed to 0
    }
    updateDisplay();
    delay(50); // Debounce delay
  }
}

void handleDirectionButtons() {
  if (digitalRead(LEFT_PIN) == HIGH) { // Left button pressed
    if (Holding) {
      movingLeft = true;
      movingRight = false;
      digitalWrite(ENABLE_PIN, LOW); // Enable the motor
      digitalWrite(DIR_PIN, HIGH); // Set direction to move left
      stepMotor();
    } else {
      displayActiveStatus();
      moveLeft();
    }
  } else if (digitalRead(RIGHT_PIN) == HIGH) { // Right button pressed
    if (Holding) {
      movingRight = true;
      movingLeft = false;
      digitalWrite(ENABLE_PIN, LOW); // Enable the motor
      digitalWrite(DIR_PIN, LOW); // Set direction to move right
      stepMotor();
    } else {
      displayActiveStatus();
      moveRight();
    }
  } else { // No button pressed
    stopSlider();
    updateDisplay(); // Update display when no button is pressed
  }
}

void handleHoldingButton() {
  if (digitalRead(HOLD_PIN) == HIGH) { // Holding button pressed
    Holding = !Holding; // Toggle Holding variable
    delay(200); // Debounce delay
    updateDisplay(); // Update display to reflect holding status change
  }
}

void moveLeft() {
  movingLeft = true;
  digitalWrite(ENABLE_PIN, LOW); // Enable the motor
  digitalWrite(DIR_PIN, HIGH); // Set direction to move left
  stepMotor();
}

void moveRight() {
  movingRight = true;
  digitalWrite(ENABLE_PIN, LOW); // Enable the motor
  digitalWrite(DIR_PIN, LOW); // Set direction to move right
  stepMotor();
}

void stepMotor() {
  unsigned long stepDelay = 1000 / (speed * 4); // Calculate delay for steps
  unsigned long previousMillis = millis();
  bool activeDisplayed = false; // Flag to ensure "Active" is displayed only once

  while ((movingLeft && digitalRead(LEFT_PIN) == HIGH) || (movingRight && digitalRead(RIGHT_PIN) == HIGH) || (Holding && (movingLeft || movingRight))) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= stepDelay) {
      digitalWrite(STEP_PIN, HIGH);
      delayMicroseconds(500); // Adjust delay as needed
      digitalWrite(STEP_PIN, LOW);
      delayMicroseconds(500); // Adjust delay as needed

      previousMillis = currentMillis;

      if (Holding && !activeDisplayed) {
        displayActiveStatus(); // Display "Active" only once when Holding is true
        activeDisplayed = true; // Set flag to true to avoid repeated display
      }
    }
  }
}

void stopSlider() {
  digitalWrite(ENABLE_PIN, HIGH); // Disable the motor
}

void updateDisplay() {
  display.clearDisplay(); // Clear the display before updating
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print("Speed: ");
  display.print(speed);
  displayStandbyStatus(); // Update standby status
  updateHoldingStatus(); // Update holding status
  display.display();
}

void displayActiveStatus() {
  display.clearDisplay(); // Clear the display before updating
  display.setTextSize(1); // Reset text size to small
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  display.print("Active");
  display.display();
}

void displayStandbyStatus() {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  display.print("Standby");
}

void updateHoldingStatus() {
  if (Holding) {
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(SCREEN_WIDTH - 6, SCREEN_HEIGHT - 8);
    display.print("x");
  }
}
