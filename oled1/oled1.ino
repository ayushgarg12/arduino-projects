#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>  // http web access library
#include <ArduinoJson.h>        // JSON decoding library
// Libraries for SSD1306 OLED display
#include <Wire.h>              // include wire library (for I2C devices such as the SSD1306 display)
#include <Adafruit_GFX.h>      // include Adafruit graphics library
#include <Adafruit_SSD1306.h>  // include Adafruit SSD1306 OLED display driver
 
#define OLED_RESET   5     // define SSD1306 OLED reset at ESP8266 GPIO5 (NodeMCU D1)
Adafruit_SSD1306 display(OLED_RESET);
 
// set Wi-Fi SSID and password
const char *ssid     = "vivo 1807";
const char *password = "ayush1234";
 
// set location and API key
String Location = "ghaziabad,IN";
String API_Key  = "2b0bdf51e49efcd6e065cf4f799677a1";

void setup(void)
{
  Serial.begin(9600);
  delay(1000);
 
  Wire.begin(4, 0);           // set I2C pins [SDA = GPIO4 (D2), SCL = GPIO0 (D3)], default clock is 100kHz
 
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D);  // initialize with the I2C addr 0x3D (for the 128x64)
  // init done
 
  Wire.setClock(400000L);   // set I2C clock to 400kHz
 
  display.clearDisplay();  // clear the display buffer
  display.setTextColor(WHITE, BLACK);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("  Internet Weather");
  display.print("  Station - GHAZIABAD");
  display.display();
 
  WiFi.begin(ssid, password);
 
  Serial.print("Connecting.");
  display.setCursor(0, 24);
  display.println("Connecting...");
  display.display();
  while ( WiFi.status() != WL_CONNECTED )
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("connected");
  display.print("connected");
  display.display();
  delay(1000);
 
}
 
void loop()
{
  if (WiFi.status() == WL_CONNECTED)  //Check WiFi connection status
  {
    HTTPClient http;  //Declare an object of class HTTPClient
 
    // specify request destination
    http.begin("http://api.openweathermap.org/data/2.5/weather?q=" + Location + "&APPID=" + API_Key);  // !!
 
    int httpCode = http.GET();  // send the request
 
    if (httpCode > 0)  // check the returning code
    {
      String payload = http.getString();   //Get the request response payload
     
      DynamicJsonDocument doc(1024);
    
      // Parse JSON object 
     char json[]= "{\"sensor\";\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}"; 
       
      DeserializationError error= deserializeJson(doc, json);
      if (error) {
        Serial.print("deserializeJson() failed with code");
        Serial.println(error.c_str());
        return;
      }
 
      float temp = (float)(doc["main"]["temp"]) - 273.15;        // get temperature in °C
      int   humidity = doc["main"]["humidity"];                  // get humidity in %
      float pressure = (float)(doc["main"]["pressure"]) / 1000;  // get pressure in bar
      float wind_speed = doc["wind"]["speed"];                   // get wind speed in m/s
      int  wind_degree = doc["wind"]["deg"];                     // get wind degree in °
 
      // print data
      Serial.printf("Temperature = %.2f°C\r\n", temp);
      Serial.printf("Humidity    = %d %%\r\n", humidity);
      Serial.printf("Pressure    = %.3f bar\r\n", pressure);
      Serial.printf("Wind speed  = %.1f m/s\r\n", wind_speed);
      Serial.printf("Wind degree = %d°\r\n\r\n", wind_degree);
 
      display.setCursor(0, 24);
      display.printf("Temperature: %5.2f C\r\n", temp);
      display.printf("Humidity   : %d %%\r\n", humidity);
      display.printf("Pressure   : %.3fbar\r\n", pressure);
      display.printf("Wind speed : %.1f m/s\r\n", wind_speed);
      display.printf("Wind degree: %d", wind_degree);
      display.drawRect(109, 24, 3, 3, WHITE);     // put degree symbol ( ° )
      display.drawRect(97, 56, 3, 3, WHITE);
      display.display();
 
    }
 
    http.end();   //Close connection
 
  }
 
  delay(60000);   // wait 1 minute
 
}

 
