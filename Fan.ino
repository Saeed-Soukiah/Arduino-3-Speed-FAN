/*
  Author: Saeed Soukiah
  Created: 2025-11-16
  Description:
    Simple Arduino fan controller using an L298N driver and ezButton.
    A momentary pushbutton cycles the fan through four PWM speed states:
      OFF (0), LOW (85), MEDIUM (170), HIGH (255).
    IN2 is held LOW to fix motor direction; IN1 receives PWM to control speed.
    Uses INPUT_PULLUP for the button and ezButton for debouncing and press counting.

  Hardware:
    - Arduino (Uno/Nano/Any with PWM on pin 6)
    - L298N motor driver (ENA on pin 5, IN1 on pin 6, IN2 on pin 7)
    - Momentary pushbutton to GND on pin 2 (uses internal pull-up)
    - External motor power supply appropriate for the fan

  Pins:
    buttonPin = 2  // INPUT_PULLUP
    enPin     = 5  // ENA (not PWM controlled here)
    in1Pin    = 6  // IN1 (PWM)
    in2Pin    = 7  // IN2 (held LOW for forward only)

  Dependencies:
    - ezButton library (install from Library Manager)
*/
#include <ezButton.h>                    // ezButton library: handles debouncing and simple button state/counting

const uint8_t buttonPin = 2;             // Physical pin where the pushbutton is connected (uses INPUT_PULLUP)
const uint8_t enPin = 5;                 // ENA pin on L298N (not PWM'd here because ENA is tied high by jumper)
const uint8_t in1Pin = 6;                // IN1 pin on L298N — this pin will receive PWM to vary motor/fan speed
const uint8_t in2Pin = 7;                // IN2 pin on L298N — held LOW to force a single direction (forward only)

ezButton button(buttonPin);              // Create ezButton instance bound to buttonPin

// Discrete PWM levels used to step through OFF, LOW, MEDIUM, HIGH
const uint8_t PWM_LEVELS[] = {0, 85, 170, 255};
// Number of distinct states derived from the array length
const uint8_t NUM_STATES = sizeof(PWM_LEVELS) / sizeof(PWM_LEVELS[0]);

void setup() {
  // Configure the button pin as input with internal pull-up resistor enabled.
  // Using INPUT_PULLUP means the button should connect the pin to GND when pressed.
  pinMode(buttonPin, INPUT_PULLUP);

  // Configure ezButton behavior:
  // - setDebounceTime to ignore contact bounce shorter than 50 ms
  // - setCountMode(COUNT_FALLING) to increment the internal counter when the pin transitions from HIGH to LOW (button press)
  button.setDebounceTime(50);
  button.setCountMode(COUNT_FALLING);

  // Configure the motor-control pins as outputs.
  pinMode(enPin, OUTPUT);    // ENA on L298N (here it's an output in case user decides to control it later)
  pinMode(in1Pin, OUTPUT);   // IN1 will get PWM to control speed
  pinMode(in2Pin, OUTPUT);   // IN2 will be held LOW to fix motor direction

  // Fix direction: tie IN2 LOW so IN1 PWM controls forward speed only.
  // This prevents reversing the motor while the code only intends speed steps.
  digitalWrite(in2Pin, LOW);

  // Ensure the motor starts stopped by writing zero PWM to IN1.
  analogWrite(in1Pin, 0);

  // Start serial for human-readable status messages and debugging.
  Serial.begin(115200);
  Serial.println("L298N Fan controller (ENA jumper present)");
}

void loop() {
  // Must call button.loop() regularly for ezButton to sample the pin and update its internal state.
  button.loop();

  // Read how many presses have been counted so far (wrap-around handled below).
  int count = button.getCount();

  // If count exceeds the number of defined states, wrap it back to 0.
  // This enables cyclic stepping: OFF -> LOW -> MEDIUM -> HIGH -> OFF ...
  if (count >= NUM_STATES) {
    button.resetCount();   // reset ezButton internal counter to 0
    count = 0;             // keep local variable consistent with reset state
  }

  // Track last applied state to avoid re-applying same PWM and re-printing unchanged status every loop.
  static int lastState = -1;
  if (count != lastState) {
    // Map the current count to an explicit PWM duty cycle from the PWM_LEVELS array.
    uint8_t pwm = PWM_LEVELS[count];

    // Apply PWM to IN1; since IN2 is LOW, this controls forward speed only.
    analogWrite(in1Pin, pwm);

    // Print a concise status message over Serial so a human can monitor changes.
    // Messages correspond to PWM_LEVELS entries by index.
    switch (count) {
      case 0: Serial.println("Fan: OFF"); break;      // pwm == 0
      case 1: Serial.println("Fan: LOW"); break;      // pwm == 85 (~33% duty)
      case 2: Serial.println("Fan: MEDIUM"); break;   // pwm == 170 (~67% duty)
      case 3: Serial.println("Fan: HIGH"); break;     // pwm == 255 (100% duty)
    }

    // Update lastState so we only act when the state actually changes.
    lastState = count;
  }

  // Small delay to reduce loop CPU usage and limit Serial flood rate.
  // ezButton sampling is asynchronous within loop(); the debounce timer is managed by ezButton internals.
  delay(10);
}