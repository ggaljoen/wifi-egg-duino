#ifndef PRINTER_H
#define PRINTER_H

#include <AccelStepper.h>
#include <FS.h>
#include <functional>
#include <MultiStepper.h>
#include <Preferences.h>

typedef std::function<void()> PrinterHandler;

/**
 * --------------------------------------------------------------
 * Board Selection:
 * Uncomment ONLY ONE of the following boards to set the correct
 * pin definitions.
 * --------------------------------------------------------------
 */

//#define BOARD_M5STACKCORE   // M5Stack Core
#define BOARD_ESP32DEV      // Standard ESP32 Development module
//#define BOARD_R32CNCSHIELD    // Wemos D1 R32 Uno Clone with CNC shield

#ifdef BOARD_M5STACKCORE
    // Egg Rotation Stepper: ("X")
    #define PIN_ROT_STEP 17
    #define PIN_ROT_DIR 2
    #define PIN_ROT_RES 16
    // Pen Stepper:          ("Y")
    #define PIN_PEN_STEP 26
    #define PIN_PEN_DIR 5
    #define PIN_PEN_RES 22
    // Servo:
    #define PIN_SERVO 21
    #define SERVO_CHA 2
    #define SERVO_MIN (65536 / 20)
    #define SERVO_MAX (2 * SERVO_MIN)
#endif

#ifdef BOARD_ESP32DEV
    // Egg Rotation Stepper: ("X")
    #define PIN_ROT_STEP 25
    #define PIN_ROT_DIR 33
    #define PIN_ROT_RES 13
    // Pen Stepper:          ("Y")
    #define PIN_PEN_STEP 27
    #define PIN_PEN_DIR 26
    #define PIN_PEN_RES 13
    // Servo:
    #define PIN_SERVO 22
    #define SERVO_CHA 2
    #define SERVO_MIN (65536 / 20)
    #define SERVO_MAX (2 * SERVO_MIN)
    // SD Card SPI:
    #define VSPI_SCLK 18
    #define VSPI_MISO 19
    #define VSPI_MOSI 23
    #define VSPI_SS 5
#endif

#ifdef BOARD_R32CNCSHIELD
    // Egg Rotation Stepper: ("X")
    #define PIN_ROT_STEP 26
    #define PIN_ROT_DIR 16
    #define PIN_ROT_RES 12
    // Pen Stepper:          ("Y")
    #define PIN_PEN_STEP 25
    #define PIN_PEN_DIR 27
    #define PIN_PEN_RES 12
    // Servo:
    #define PIN_SERVO 17
    #define SERVO_CHA 2
    #define SERVO_MIN (65536 / 20)
    #define SERVO_MAX (2 * SERVO_MIN)
    // SD Card SPI:
    #define VSPI_SCLK 18
    #define VSPI_MISO 19
    #define VSPI_MOSI 23
    #define VSPI_SS 5
#endif

/**
 * --------------------------------------------------------------
 * Misc Options
 * --------------------------------------------------------------
 */

/** Enable if your controller is having power stability issues.
 *  More info: https://iotespresso.com/how-to-disable-brownout-detector-in-esp32-in-arduino/
 */
//#define DISABLE_BROWNOUT_DETECTOR

/** Enable to reduce pen hammering. May cause jitter on certain servos. */
#define SLOWER_SERVO

/** Enable for WiFi connection retries */
//#define WIFI_RETRY

// END CONFIG ---------------------------------------------------

struct MotionParameters
{
    uint8_t penUpPercent, penDownPercent;
    uint16_t drawingSpeed, travelSpeed;
    uint16_t penMoveDelay;
    uint16_t stepsPerRotation;
    bool reverseRotation, reversePen;
};

class Printer
{
public:
    Printer();
    void begin();

    void penUp();
    void penDown();
    void enableMotors();
    void disableMotors();
    void getParameters(MotionParameters &params);
    void setParameters(const MotionParameters &params);

    void print(File file);
    void stop();
    void pause();
    void continuePrint();

    bool isPaused() { return waiting; }
    String getWaitingFor() { return waitingFor; }
    bool isPrinting() { return printTaskHandle ? true : false; }
    const ulong getPrintedLines() { return printedLines; }
    const char *printingFileName() { return printing->name(); }

    void onProgressChanged(PrinterHandler handler) { onProgress = handler; }
    void onStatusChanged(PrinterHandler handler) { onStatus = handler; }

    void printTask();

private:
    void applyParameters();
    void moveTo(long x, long y);

    bool waiting;
    File *printing;

    ulong printedLines;
    PrinterHandler onProgress, onStatus;
    String waitingFor;
    AccelStepper mRotation, mPen;
    MultiStepper multiStepper;

    int32_t posX, posY;
    uint16_t penUpValue, penDownValue, penCurrValue;
    bool _isPenUp, _isMoving;
    Preferences preferences;
    MotionParameters parameters;
    TaskHandle_t printTaskHandle = NULL;
};

#endif
