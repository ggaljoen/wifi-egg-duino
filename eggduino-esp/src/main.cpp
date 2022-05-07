#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>

#include "web.h"
#include "printer.h"

#ifdef BOARD_M5STACKCORE
    #include <M5Stack.h>
    #include "Free_Fonts.h"
#else
    #include <Wire.h>
    #include <SPI.h>
    #include "FS.h"
    #include "SD.h"

    SPIClass *sd_spi = NULL;
    
    #ifdef DISABLE_BROWNOUT_DETECTOR
        #include "soc/soc.h"
        #include "soc/rtc_cntl_reg.h"
    #endif
#endif

Printer printer;
Web web(SD, printer);
DNSServer dnsServer;

void startAp()
{
    Serial.print("Eggbot Setup AP: ");
    char ssid[14];
    snprintf(ssid, sizeof(ssid), "EggBot-%04X", (uint16_t)(ESP.getEfuseMac() >> 32));
    Serial.println(WiFi.softAP(ssid) ? "Ready" : "Failed!");
}

void OnWiFiEvent(WiFiEvent_t event)
{
    switch (event)
    {
        case SYSTEM_EVENT_WIFI_READY:
            Serial.println("Eggbot WiFi Initialized");
            break;
        case SYSTEM_EVENT_STA_CONNECTED:
            Serial.print("Eggbot Connected to WiFi Network: ");
            Serial.println(WiFi.SSID());
            #ifndef BOARD_M5STACKCORE
                digitalWrite(LED_BUILTIN, HIGH);
            #endif
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            Serial.println("Eggbot Disconnected from WiFi Network");
            #ifndef BOARD_M5STACKCORE
                digitalWrite(LED_BUILTIN, LOW);
            #endif
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            Serial.print("Eggbot IP: ");
            Serial.println(WiFi.localIP());
            break;
        case SYSTEM_EVENT_AP_START:
            Serial.println("Eggbot setup AP started");
            break;
        case SYSTEM_EVENT_AP_STACONNECTED:
            Serial.println("Client connected to Eggbot's setup AP");
            break;
        case SYSTEM_EVENT_AP_STADISCONNECTED:
            Serial.println("Client disconnected from Eggbot's setup AP");
            break;
        default:
            break;
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.println();
    #ifndef BOARD_M5STACKCORE
        pinMode(LED_BUILTIN, OUTPUT);
        digitalWrite(LED_BUILTIN, LOW);
    #endif

    #ifdef DISABLE_BROWNOUT_DETECTOR
        WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
    #endif

    WiFi.onEvent(OnWiFiEvent);
    WiFi.mode(WIFI_MODE_APSTA);
    WiFi.setHostname("EggBot");
    if (WiFi.begin() != WL_CONNECTED)
    {
        startAp();
    }
    dnsServer.start(53, "*", IPAddress(192, 168, 4, 1));

    #ifdef BOARD_M5STACKCORE
        M5.begin();
        M5.Lcd.setFreeFont(FSS12);
        M5.Lcd.println();
    #else
        sd_spi = new SPIClass(VSPI);
        sd_spi->begin(VSPI_SCLK, VSPI_MISO, VSPI_MOSI, VSPI_SS);
        pinMode(VSPI_SS, OUTPUT);

        if (!SD.begin(VSPI_SS, *sd_spi, 4000000))
        {
            Serial.println("Card Mount Failed");
        }

        uint8_t cardType = SD.cardType();

        if (cardType == CARD_NONE)
        {
            Serial.println("No SD card attached");
        }
    #endif

    printer.begin();
    web.begin();
}

void loop()
{
    dnsServer.processNextRequest();

    #ifdef WIFI_RETRY
      uint32_t nextCheck = 10000;
      if (millis() > nextCheck)
      {
        nextCheck = millis() + 10000;

        auto status = WiFi.status();
        if (status == WL_CONNECTED)
        {
          WiFi.enableAP(false);
        }
        else if (status == WL_NO_SSID_AVAIL)
        {
          startAp();
          WiFi.enableSTA(false);
        }
        else if (status == WL_DISCONNECTED)
        {
          startAp();
        }
      }
    #endif

    #ifdef BOARD_M5STACKCORE
        M5.update();
        if (M5.BtnA.wasPressed() || M5.BtnB.wasPressed() || M5.BtnC.wasPressed())
        {
            printer.continuePrint();
        }
    #endif
}
