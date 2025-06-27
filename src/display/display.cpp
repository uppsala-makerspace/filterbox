#include "display.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

// Pins for TFT — adjust to your setup

#define TFT_CS 10
#define TFT_RST 8
#define TFT_DC 9

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

void initDisplay()
{
  tft.initR(INITR_GREENTAB);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
}

void showSplashScreen()
{
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(10, 30);
  tft.print("Welcome!");
  delay(2000); // Show for 2 seconds
}

void drawGraph(uint16_t *history, uint16_t color)
{
  int graphX = 0, graphY = 80, graphW = 128, graphH = 48;
  tft.drawRect(graphX, graphY, graphW, graphH, ST77XX_WHITE);

  // Find max value for scaling
  uint16_t maxVal = 1;
  uint16_t count = historyFilled ? HISTORY_SIZE : historyIndex;
  for (uint16_t i = 0; i < count; i++)
  {
    if (history[i] > maxVal)
      maxVal = history[i];
  }

  // Draw lines
  for (uint16_t i = 1; i < count; i++)
  {
    int x0 = graphX + (graphW * (i - 1)) / (HISTORY_SIZE - 1);
    int y0 = graphY + graphH - (history[(i - 1 + historyIndex) % HISTORY_SIZE] * graphH) / maxVal;
    int x1 = graphX + (graphW * i) / (HISTORY_SIZE - 1);
    int y1 = graphY + graphH - (history[(i + historyIndex) % HISTORY_SIZE] * graphH) / maxVal;
    tft.drawLine(x0, y0, x1, y1, color);
  }
}

void updateDisplay(const AirData &data)
{
  tft.fillScreen(ST77XX_BLACK);

  tft.setCursor(0, 10);
  tft.print("pm03c: ");
  tft.println(data.pm03Count);

  tft.setCursor(0, 30);
  tft.print("PM2.5: ");
  tft.println(data.pm25);

  tft.setCursor(0, 50);
  tft.print("cPm2.5: ");
  tft.println(data.calibratedPm25);

  // Draw PM2.5 graph (for example, in green)
  drawGraph(pm25History, ST77XX_GREEN);
}