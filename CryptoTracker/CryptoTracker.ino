#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_HX8357.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#define STMPE_CS 16
#define TFT_CS   D8
#define TFT_DC   D4
#define SD_CS    2
#define TFT_RST -1

#define STASSID "YOUR NETWORK"
#define STAPSK  "YOUR PASSWORD"

const char* ssid     = STASSID;
const char* password = STAPSK;

const char* host = "https://min-api.cryptocompare.com/data/pricemulti?fsyms=BTC,ETH,LTC,DGB,XRP,XVG,THETA,DOT&tsyms=USD&api_key={YOUR_API_KEY}";
const uint16_t port = 443;

HTTPClient http;

const long utcOffsetInSeconds = -18000;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);

const int updateInt = 120;
int cntDelay;

void setup() {
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(HX8357_BLACK);
  cntDelay = updateInt+1;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    //tft.print(".");
  }
  timeClient.begin();
}



void clearPixels(int xmin,int xmax,int ymin,int ymax){
  for (int y=ymin; y<=ymax; y++){
    for (int x=xmin; x<xmax; x++){
      tft.drawPixel(x, y, HX8357_BLACK);
    }
  }
}

void SetPriceColor(float oldPrice, float newPrice){
      if (oldPrice == 0){
        tft.setTextColor(HX8357_YELLOW);      
      }
      else if (oldPrice > newPrice){
        tft.setTextColor(HX8357_RED); 
      }
      else if (oldPrice < newPrice){
        tft.setTextColor(HX8357_GREEN); 
      }
      else{
        tft.setTextColor(HX8357_YELLOW);
      } 
}

float BTC_USD_;
float ETH_USD_;
float LTC_USD_;
float DGB_USD_;
float XRP_USD_;
float XVG_USD_;
float THETA_USD_;
float DOT_USD_;

void loop() {
  clearPixels(90,127,0,10);
  int diff = updateInt - cntDelay;
  
  if(diff < 0){
    clearPixels(0,127,0,12);
    tft.setCursor(0, 4);
    tft.setTextColor(HX8357_WHITE);  
    tft.setTextSize(1);
   
    tft.print("Updating...");
           
    WiFiClientSecure client;
    client.setInsecure(); //the magic line, use with caution
    client.connect(host, 443);
    http.begin(client, host); 

    if (http.GET() == HTTP_CODE_OK){
      clearPixels(0,127,0,12);
      String payload = http.getString();
      DynamicJsonDocument doc(384);
      deserializeJson(doc, payload);
      
      float BTC_USD = doc["BTC"]["USD"]; // 41391.03    
      float ETH_USD = doc["ETH"]["USD"]; // 1230.45     
      float LTC_USD = doc["LTC"]["USD"]; // 175.82      
      float DGB_USD = doc["DGB"]["USD"]; // 0.03022      
      float XRP_USD = doc["XRP"]["USD"]; // 0.3155      
      float XVG_USD = doc["XVG"]["USD"]; // 0.0149
      float DOT_USD = doc["DOT"]["USD"];      
      float THETA_USD = doc["THETA"]["USD"]; // 1.926
            
      tft.setCursor(0, 40);
      tft.setTextSize(4);                 
      
      clearPixels(120,400,40,72);
      SetPriceColor(BTC_USD_,BTC_USD);
      tft.print("BTC: $");
      tft.println(BTC_USD);
      
      clearPixels(120,400,72,100);
      SetPriceColor(ETH_USD_,ETH_USD);
      tft.print("ETH: $");
      tft.println(ETH_USD);
  
      clearPixels(120,400,100,132);
      SetPriceColor(LTC_USD_,LTC_USD);
      tft.print("LTC: $");
      tft.println(LTC_USD);
  
      clearPixels(120,400,132,164);
      SetPriceColor(DGB_USD_,DGB_USD);
      tft.print("DGB: $");
      tft.println(DGB_USD);
  
      clearPixels(120,400,164,196);
      SetPriceColor(XRP_USD_,XRP_USD);
      tft.print("XRP: $");
      tft.println(XRP_USD);
  
      clearPixels(120,400,196,228);
      SetPriceColor(XVG_USD_,XVG_USD);
      tft.print("XVG: $");
      tft.println(XVG_USD);
 
      clearPixels(120,400,228,260);
      SetPriceColor(DOT_USD_,DOT_USD);
      tft.print("DOT: $");
      tft.println(DOT_USD);    
      cntDelay = 0;

      clearPixels(160,400,260,292);
      SetPriceColor(THETA_USD_,THETA_USD);
      tft.print("THETA: $");
      tft.println(THETA_USD);    
      cntDelay = 0;

      BTC_USD_ = BTC_USD;
      ETH_USD_ = ETH_USD;
      LTC_USD_ = LTC_USD;
      DGB_USD_ = DGB_USD;
      XRP_USD_ = XRP_USD;
      XVG_USD_ = XVG_USD;
      THETA_USD_= THETA_USD;
      DOT_USD_= DOT_USD;
    }
    else {
      clearPixels(0,127,0,12);
      tft.print("Failed to pull data");
      delay(5000);
    }
  }

  tft.setCursor(0, 4);
  tft.setTextColor(HX8357_WHITE);  
  tft.setTextSize(1);
 
  tft.print("Next update in: ");
  tft.print(diff);   
   
  delay(1000);
  cntDelay++;
}
