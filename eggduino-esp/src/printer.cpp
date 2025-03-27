#include "esp32-hal-ledc.h"
#include "printer.h"

static const char* TAG = "Printer";
#define TASK_PRIORITY 2

void printTaskHandler(void *arg)
{
    ESP_LOGV(TAG, "<- was called");
    ((Printer *)arg)->printTask();
}

Printer::Printer()
    : mRotation(AccelStepper::DRIVER, PIN_ROT_STEP, PIN_ROT_DIR),
      mPen(AccelStepper::DRIVER, PIN_PEN_STEP, PIN_PEN_DIR)
{
    mRotation.setEnablePin(PIN_ROT_RES);
    mPen.setEnablePin(PIN_PEN_RES);
    mRotation.setPinsInverted(parameters.reverseRotation, false, true);
    mPen.setPinsInverted(parameters.reversePen, false, true);

    mRotation.setAcceleration(.1);
    mPen.setAcceleration(.1);

    multiStepper.addStepper(mRotation);
    multiStepper.addStepper(mPen);
}

void Printer::begin()
{
    ESP_LOGV(TAG, "<- was called");
    disableMotors();
    ledcSetup(SERVO_CHA, 50, 16);

    preferences.begin("motion");
    getParameters(parameters);
    applyParameters();
    penCurrValue = penUpValue;
    ledcAttachPin(PIN_SERVO, SERVO_CHA);
    ledcWrite(SERVO_CHA, 0);
//    ledcWrite(SERVO_CHA, penUpValue);

 //   penUp();
    disableMotors();
}

void Printer::stop()
{
    ESP_LOGV(TAG, "<- was called");
    TaskHandle_t handle = printTaskHandle;
    if (!handle)
    {
        return;
    }

    waiting = false;
    printedLines = 0;
    waitingFor = String();
    printTaskHandle = NULL;
    onStatus();

    if (xTaskGetCurrentTaskHandle() != handle)
    {
        // if we stop from outside the print task,
        // stop first, close file 2nd
        vTaskDelete(handle);
        handle = NULL;
    }

    if (printing)
    {
        printing->close();
        delete printing;
        printing = NULL;
    }

    penUp();
    disableMotors();

    if (handle)
    {
        vTaskDelete(handle);
    }
}

void Printer::print(fs::File file)
{
    ESP_LOGV(TAG, "<- was called");
    stop();

    printing = new File(file);
    mPen.setCurrentPosition(0);
    mRotation.setCurrentPosition(0);
    xTaskCreatePinnedToCore(printTaskHandler, "Print", 8000, this, TASK_PRIORITY, &printTaskHandle, 0);
}

void Printer::printTask()
{
    ESP_LOGV(TAG, "<- was called");
    uint32_t lastProgress = 0;
    while (printing->available())
    {
        char buffer[31];
        auto read = printing->readBytesUntil('\n', buffer, 30);
        printedLines++;
        if (!read)
        {
            continue;
        }
        buffer[read] = 0;

        if (strcmp(buffer, "P0") == 0)
        {
            penUp();
        }
        else if (strcmp(buffer, "P1") == 0)
        {
            penDown();
        }
        else if (strcmp(buffer, "M1") == 0)
        {
            enableMotors();
        }
        else if (strcmp(buffer, "M0") == 0)
        {
            disableMotors();
        }
        else if (buffer[0] == 'Z')
        {
            // TODO: report progress percent on LCD
            //  progress = atoi(&buffer[2]);
        }
        else if (buffer[0] == 'S')
        {
            long lastPenPosition = mPen.currentPosition();
            moveTo(mRotation.currentPosition(), 0);
            waitingFor = String(&buffer[2]);
            pause();
            waitingFor = String();
            moveTo(mRotation.currentPosition(), lastPenPosition);
        }
        else if (strcmp(buffer, "H") == 0)
        {
            moveTo(0, 0);
        }
        else if (buffer[0] == 'T')
        {
            long x = roundf((float)atof(&buffer[2]) / 360.0f * parameters.stepsPerRotation);
//            long y = roundf((float)atof(strchr(&buffer[2], ' ')) / 360.0f * parameters.stepsPerRotation);
            long y = roundf((float)atof(strchr(&buffer[2], ' ')) / 180.0f * parameters.stepsPerRotation ); // y => pen only 1/2 steps
            moveTo(x, y);
        }

        uint32_t now = millis();
        if (now - lastProgress > 1000)
        {
            lastProgress = now;
            if (onProgress)
            {
                onProgress();
            }
        }

        vTaskDelay(1);
    }

    stop();
}

void Printer::moveTo(long x, long y)
{
    ESP_LOGD(TAG, "Move to X:%d Y:%d", x, y);
    disableCore0WDT();
    long pos[2] = {x, y};
    multiStepper.moveTo(pos);
    multiStepper.runSpeedToPosition();
    enableCore0WDT();
}

void Printer::pause()
{
    ESP_LOGV(TAG, "<- was called");
    if (printTaskHandle && !waiting)
    {
        waiting = true;
        onStatus();
        vTaskSuspend(printTaskHandle);
    }
}

void Printer::continuePrint()
{
    ESP_LOGV(TAG, "<- was called");
    if (printTaskHandle && waiting)
    {
        vTaskResume(printTaskHandle);
        waiting = false;
        onStatus();
    }
}

void Printer::penUp()
{
    ESP_LOGV(TAG, "<- was called");
    _isPenUp = true;
    #ifdef SLOWER_SERVO
        for (uint16_t i = penCurrValue; i >= penUpValue; i--)
        {
            ledcWrite(SERVO_CHA, i);
            delayMicroseconds(250);
        }
        penCurrValue = penUpValue;
    #else
        ledcWrite(SERVO_CHA, penUpValue);
    #endif
    delay(parameters.penMoveDelay);
    mRotation.setMaxSpeed(parameters.travelSpeed);
    mPen.setMaxSpeed(parameters.travelSpeed);
}

void Printer::penDown()
{
    ESP_LOGV(TAG, "<- was called");
    _isPenUp = false;
    #ifdef SLOWER_SERVO
        for (uint16_t i = penCurrValue; i <= penDownValue; i++)
        {
            ledcWrite(SERVO_CHA, i);
            delayMicroseconds(250);
        }
        penCurrValue = penDownValue;
    #else
        ledcWrite(SERVO_CHA, penDownValue);
    #endif
    delay(parameters.penMoveDelay);
    mRotation.setMaxSpeed(parameters.drawingSpeed);
    mPen.setMaxSpeed(parameters.drawingSpeed);
}

void Printer::getParameters(MotionParameters &params)
{
    ESP_LOGV(TAG, "<- was called");
    auto size = sizeof(MotionParameters);
    if (preferences.getBytes("eggbot", &params, size) != size)
    {
        ESP_LOGD(TAG, "Setting first run config defaults");
        params.penDownPercent = 70;
        params.penUpPercent = 40;
        params.drawingSpeed = 500;
        params.travelSpeed = 2000;
        params.penMoveDelay = 150;
        params.stepsPerRotation = 6400;
        params.reversePen = false;
        params.reverseRotation = false;
        setParameters(params);
    }
}

void Printer::setParameters(const MotionParameters &params)
{
    ESP_LOGV(TAG, "<- was called");
    parameters = params;
    preferences.putBytes("eggbot", &parameters, sizeof(MotionParameters));
    applyParameters();
}

void Printer::applyParameters()
{
    ESP_LOGV(TAG, "<- was called");
    penUpValue = SERVO_MIN + (SERVO_MAX - SERVO_MIN) * parameters.penUpPercent / 100;
    penDownValue = SERVO_MIN + (SERVO_MAX - SERVO_MIN) * parameters.penDownPercent / 100;

    if (_isPenUp)
    {
        penUp();
        mRotation.setMaxSpeed(parameters.travelSpeed);
        mPen.setMaxSpeed(parameters.travelSpeed);
    }
    else
    {
        penDown();
        mRotation.setMaxSpeed(parameters.drawingSpeed);
        mPen.setMaxSpeed(parameters.drawingSpeed);
    }

    mRotation.setPinsInverted(parameters.reverseRotation, false, true);
    mPen.setPinsInverted(parameters.reversePen, false, true);
}

void Printer::enableMotors()
{
    ESP_LOGV(TAG, "<- was called");
    mRotation.enableOutputs();
    mPen.enableOutputs();
    delay(2000);
}

void Printer::disableMotors()
{
    ESP_LOGV(TAG, "<- was called");
    mRotation.setPinsInverted(parameters.reverseRotation, false, true);
    mPen.setPinsInverted(parameters.reversePen, false, true);

    mRotation.disableOutputs();
    mPen.disableOutputs();
}
