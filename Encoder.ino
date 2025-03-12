#include <Adafruit_TinyUSB.h>

// Define key codes manually
#define KEY_W 0x1A
#define KEY_S 0x16
#define KEY_A 0x04
#define KEY_D 0x07
#define KEY_UP_ARROW 0x52
#define KEY_DOWN_ARROW 0x51
#define KEY_LEFT_ARROW 0x50
#define KEY_RIGHT_ARROW 0x4F

// USB HID device instances for gamepad and keyboard
Adafruit_USBD_HID usb_hid;
Adafruit_USBD_HID usb_keyboard;

// Right joystick pins
const int R_Up = 8, R_Down = 9, R_Left = 10, R_Right = 11;
// Left joystick pins
const int L_Up = 12, L_Down = 13, L_Left = 14, L_Right = 15;

// Struct for HID gamepad report (modified for right joystick)
typedef struct
{
    int8_t x;
    int8_t y;
    uint16_t buttons;
} GamepadReport_t;

GamepadReport_t gamepadR = {0, 0, 0};

// Custom HID descriptor for a gamepad
uint8_t const gamepadReportDescriptor[] = {
    TUD_HID_REPORT_DESC_GAMEPAD()};

// Custom HID descriptor for a keyboard
uint8_t const keyboardReportDescriptor[] = {
    TUD_HID_REPORT_DESC_KEYBOARD()};

void setup()
{
    Serial.begin(115200);

    // Initialize USB HID as a gamepad
    usb_hid.setReportDescriptor(gamepadReportDescriptor, sizeof(gamepadReportDescriptor));
    usb_hid.begin();

    // Initialize USB HID as a keyboard
    usb_keyboard.setReportDescriptor(keyboardReportDescriptor, sizeof(keyboardReportDescriptor));
    usb_keyboard.begin();

    delay(2000);

    TinyUSBDevice.setProductDescriptor("Custom Game Controller");

    // Setup input buttons with pull-ups for the right and left joysticks
    int pins[] = {R_Up, R_Down, R_Left, R_Right, L_Up, L_Down, L_Left, L_Right};

    for (int i = 0; i < sizeof(pins) / sizeof(pins[0]); i++)
    {
        pinMode(pins[i], INPUT_PULLUP);
    }

    Serial.println("USB HID Keyboard Ready!");
}

void loop()
{
    uint8_t keycode[6] = {0}; // Array to hold keycodes
    bool keysPressed = false;

    // Read left joystick inputs (Arrow keys)
    int xAxisL = (digitalRead(L_Left) == LOW) ? -1 : (digitalRead(L_Right) == LOW) ? 1 : 0;
    int yAxisL = (digitalRead(L_Up) == LOW) ? -1 : (digitalRead(L_Down) == LOW) ? 1 : 0;

    // Map left joystick movements to arrow keys
    if (yAxisL == -1)
    {
        keycode[0] = KEY_UP_ARROW; // Up arrow key
        keysPressed = true;
    }
    else if (yAxisL == 1)
    {
        keycode[0] = KEY_DOWN_ARROW; // Down arrow key
        keysPressed = true;
    }

    if (xAxisL == -1)
    {
        keycode[1] = KEY_LEFT_ARROW; // Left arrow key
        keysPressed = true;
    }
    else if (xAxisL == 1)
    {
        keycode[1] = KEY_RIGHT_ARROW; // Right arrow key
        keysPressed = true;
    }

    // Read right joystick inputs (WASD)
    int xAxisR = (digitalRead(R_Left) == LOW) ? -1 : (digitalRead(R_Right) == LOW) ? 1 : 0;
    int yAxisR = (digitalRead(R_Up) == LOW) ? -1 : (digitalRead(R_Down) == LOW) ? 1 : 0;

    // Map right joystick movements to WASD keys
    if (yAxisR == -1)
    {
        keycode[2] = KEY_W; // W key for up
        keysPressed = true;
    }
    else if (yAxisR == 1)
    {
        keycode[2] = KEY_S; // S key for down
        keysPressed = true;
    }

    if (xAxisR == -1)
    {
        keycode[3] = KEY_A; // A key for left
        keysPressed = true;
    }
    else if (xAxisR == 1)
    {
        keycode[3] = KEY_D; // D key for right
        keysPressed = true;
    }

    usb_keyboard.keyboardReport(0, 0, keycode);

    // Release keys if both joysticks are in neutral position and keys were pressed
    if (xAxisL == 0 && yAxisL == 0 && xAxisR == 0 && yAxisR == 0 && keysPressed)
    {
        usb_keyboard.keyboardRelease(0);
    }

    delay(10);
}