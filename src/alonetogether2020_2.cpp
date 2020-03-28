#include "Particle.h"

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#if defined(PARTICLE)
   #define TFT_DC   D5
   #define TFT_CS   D4
   #define STMPE_CS D3
   #define SD_CS    D2
#endif

// Anything else!
#if defined (__AVR_ATmega32U4__) || defined(ARDUINO_SAMD_FEATHER_M0) || defined (__AVR_ATmega328P__) || \
    defined(ARDUINO_SAMD_ZERO) || defined(__SAMD51__) || defined(__SAM3X8E__) || defined(ARDUINO_NRF52840_FEATHER)
   #define STMPE_CS 6
   #define TFT_CS   9
   #define TFT_DC   10
   #define SD_CS    5
#endif


Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

bool saw_message = false;
bool showing_times = false;
int lastMillis = 0;
const int MSG_LIMIT = 14;
time_t messageTimes[14];


// setup() runs once, when the device is first turned on.
void setup();
void loop();
void handleMessage(const char* event, const char* data);
void addMessageTime(time_t t);
void displayTimes();
void flashNotificiation();

//==================================================================

void handleMessage(const char* event, const char* data)
{
    addMessageTime(Time.now());
    saw_message = true;
}


void addMessageTime(time_t t)
{
    for(int i = 0; i < MSG_LIMIT; i++)
    {
        // find empty slot
        if(messageTimes[i] == 0)
        {
            messageTimes[i] = t;
            return;
        }
    }

    // no empty slots so shift all left and drop new value at end
    for(int i = 0; i < MSG_LIMIT-1; i++)
    {
        messageTimes[i] = messageTimes[i+1];
    }
    messageTimes[MSG_LIMIT-1] = t;

}


void displayTimes()
{
    showing_times = true;
    tft.fillScreen(ILI9341_BLACK);
    tft.setCursor(0,0);
    tft.setTextColor(ILI9341_YELLOW);
    tft.setTextSize(2);

    // print the header
    tft.println(" Alone Together Log\n");
    tft.println("--------------------\n");

    // walk through the array of times and display them UNTIL you hit a ZERO
    for(int i = 0; i < MSG_LIMIT; i++)
    {
        time_t time = messageTimes[i];
        if(time)
        {
            int month   = Time.month(time);
            int day     = Time.day(time);
            int hour    = Time.hour(time);
            int minute  = Time.minute(time);
            int second  = Time.second(time);
            tft.printf("   %02d/%02d %02d:%02d:%02d\n", month, day, hour, minute, second);
        }
    }
}


void flashNotificiation()
{
    showing_times = false;
    tft.fillScreen(ILI9341_BLACK);
    tft.setCursor(0,0);

    static const char* msg = "YOU ARE\n  NOT ALONE!!";

    int16_t startX, startY;
    startX = 60;
    startY = 150;

    for(int i = 0; i < 10; i++)
    {
        tft.setCursor(startX, startY);
        tft.setTextSize(3);
        tft.setTextColor(ILI9341_GREEN);
        tft.println(msg);

        int16_t x,y;
        uint16_t w, h;

        tft.getTextBounds(msg, startX, startY, &x, &y, &w, &h);
        delay(500);
        tft.fillRect(x, y, w, h, ILI9341_BLACK);
    }
}


void setup() 
{
    // Put initialization like pinMode and begin functions here.
    tft.begin();
    tft.fillScreen(ILI9341_BLACK);
    tft.setCursor(0,0);
    tft.scrollTo(0);
    tft.setTextSize(2);

    for(int i = 0; i < MSG_LIMIT; i++)
    {
        messageTimes[i] = 0;
    }

    Particle.connect();
    Time.zone(-4);
    Particle.syncTime();
    Particle.subscribe("alonetogether2020", handleMessage, ALL_DEVICES);
}



void loop() 
{
    if(saw_message)
    {
        saw_message = false;
        flashNotificiation();
        displayTimes();
    }

    int second = Time.second(Time.now());
    if(second >= 0 && second <= 10)
    {
        if(!showing_times)
            displayTimes();
    }

}

/*  saved 
    tft.setTextColor(ILI9341_BLUE); tft.setTextSize(3);
    tft.println("Hi Noah and John!\n");

    tft.setTextColor(ILI9341_GREEN); tft.setTextSize(2);
    tft.println("Welcome to the light show festival!!\n");

    tft.setTextColor(ILI9341_YELLOW); tft.setTextSize(2);
    tft.println("We have lots of different light shows you could watch.  1 hour later.  Thank you for watching, come again tomorrow");
*/
