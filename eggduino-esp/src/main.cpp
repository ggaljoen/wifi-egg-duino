#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "FS.h"
#include "SD.h"

#include "web.h"
#include "printer.h"

#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

SPIClass *sd_spi = NULL;


Printer printer;
Web web(SD, printer);
DNSServer dnsServer;

void startAp()
{
  char ssid[14];
  snprintf(ssid, sizeof(ssid), "EggBot-%04X", (uint16_t)(ESP.getEfuseMac() >> 32));
  WiFi.softAP(ssid);
}

void initWiFi()
{

  WiFi.mode(WIFI_STA);
  if (WiFi.begin() != WL_CONNECTED)
  {
    startAp();
    dnsServer.start(53, "*", IPAddress(192, 168, 4, 1));
  }else {
    Serial.println(WiFi.localIP());
  }
  /*
  WiFi.mode(WIFI_STA);
  WiFi.begin(WL_SSID, WL_PASS);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(1000);
  }
  Serial.println();
  Serial.println(WiFi.localIP());
  */
}

void setup()
{
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable   detector

  Serial.begin(115200);


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
  /*
  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC)
  {
    Serial.println("MMC");
  }
  else if (cardType == CARD_SD)
  {
    Serial.println("SDSC");
  }
  else if (cardType == CARD_SDHC)
  {
    Serial.println("SDHC");
  }
  else
  {
    Serial.println("UNKNOWN");
  }
  
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
  */

  initWiFi();


  printer.begin();
  web.begin();
}

void loop()
{
  dnsServer.processNextRequest();

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

  //M5.update();
/*
  if (M5.BtnA.wasPressed() || M5.BtnB.wasPressed() || M5.BtnC.wasPressed())
  {
    printer.continuePrint();
  }
  */
}