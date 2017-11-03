# esp32\_lamp

esp32\_lamp lets you toggle Philips Hue lights with a pushbutton and an ESP32
microcontroller.

It works on a simple model: you specicfy a room and a scene. When you press the
button, if any lights in that room are on, they all get turned off; if none of
them are on, they get set to the specified scene.

## Getting Started

You'll need to create a file called `secret.h` with a few `#define` statements.
Define all of these as string literals:

```c
#define SSID "Your Wi-Fi SSID"
#define PWD "Your Wi-Fi password"
#define BRIDGE "IP address or hostname of your Hue bridge"
#define API_USERNAME "Username for Hue API"
#define ROOM "ID of the room to control"
#define SCENE "ID of the scene to set lights to"
```

esp32\_lamp has these dependencies:

* https://github.com/espressif/arduino-esp32
* https://github.com/arduino-libraries/ArduinoHttpClient
* https://github.com/bblanchon/ArduinoJson

In `esp32_lamp.ino`, adjust `INPUT_PIN` and `LED_PIN` for your ESP32 board.
`INPUT_PIN` must support `INPUT_PULLUP` mode.

Attach a normally-closed pushbutton between INPUT\_PIN and ground on your ESP32
board. Plug in the board and program it. When the LED stops blinking, it's
connected. Push the button to toggle the state of the lights in the specified
room.
