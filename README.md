# L298N Fan Stepper Controller

A simple Arduino sketch that cycles a DC fan through four speed levels (OFF, LOW, MEDIUM, HIGH) using a pushbutton and PWM control via the L298N motor driver. Button presses are debounced and counted using the [ezButton](https://github.com/ArduinoGetStarted/ezButton) library.

## 🔧 Features

- Pushbutton-controlled speed stepping (cycles through 0%, 33%, 67%, 100% duty)
- Uses ezButton for reliable debouncing and press counting
- Serial output for real-time status monitoring
- Fixed motor direction (IN2 LOW) to simplify control logic
- Compact and beginner-friendly code structure

## 🛠️ Hardware Setup

| Component        | Pin Connection | Notes                                  |
|------------------|----------------|----------------------------------------|
| Pushbutton       | D2             | Connect to GND (uses INPUT_PULLUP)     |
| L298N ENA        | D5             | Jumpered HIGH or optionally controlled |
| L298N IN1        | D6             | Receives PWM signal                    |
| L298N IN2        | D7             | Held LOW for forward-only direction    |
| Fan/Motor        | L298N Output   | Powered via external supply            |

## 📦 Dependencies

- [ezButton library](https://github.com/ArduinoGetStarted/ezButton)  
  Install via Arduino Library Manager.

## 🚀 Getting Started

1. Wire up the components as described above.
2. Upload the sketch to your Arduino board.
3. Open Serial Monitor at 115200 baud.
4. Press the button to cycle through fan speeds.
