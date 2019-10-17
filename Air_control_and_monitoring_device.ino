#ifndef __CC3200R1M1RGC__
#include <SPI.h>
#endif
#include <WiFi.h>
#include "DHT.h"
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
#include <WiFiClient.h>
#include <Temboo.h>
#include "final.h"
char ssid[] = ".......";              //name of wifi connected
char password[] = "........";     //password of wifi
const String GMAIL_USER_NAME = ".......@gmail.com";        //mail sending address
const String GMAIL_APP_PASSWORD = "........";           //mail password
const String TO_EMAIL_ADDRESS = ".........";    //mail recieving address
boolean attempted = false; 
WiFiClient client;
char server[] = "api.thingspeak.com";
int8_t data1 ,data2 ,data3;
unsigned long lastConnectionTime = 0;
const unsigned long postingInterval = 10L * 1000L;
int sensorValue;
void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(18,OUTPUT);
  pinMode(24,OUTPUT);
  pinMode(27,OUTPUT);
  pinMode(28,OUTPUT);
  Serial.print("Attempting to connect to Network named: ");
  Serial.println(ssid); 
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
    }
  Serial.println("\nYou're connected to the network");
  Serial.println("Waiting for an ip address");
  while (WiFi.localIP() == INADDR_NONE) {
    Serial.print(".");
    delay(30);
  }
 Serial.println("\nIP Address obtained");
 printWifiStatus();
}
void loop() {
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }
  if (millis() - lastConnectionTime > postingInterval) {
    httpRequest();
  }
}
void httpRequest() {
  client.stop();
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print(" %\t");
    sensorValue = analogRead(6);
    Serial.print("AirQua=");
    Serial.print(sensorValue, DEC);
    Serial.println(" PPM");
    delay(100);
 if(sensorValue>50){
      if (!attempted) {
        digitalWrite(29,HIGH);
        Serial.println("Running SendAnEmail...");  
        TembooChoreo SendEmailChoreo(client);
        SendEmailChoreo.begin();    
        SendEmailChoreo.setAccountName(TEMBOO_ACCOUNT);
        SendEmailChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
        SendEmailChoreo.setAppKey(TEMBOO_APP_KEY);
        SendEmailChoreo.setChoreo("/Library/Google/Gmail/SendEmail"); 
        SendEmailChoreo.addInput("Username", GMAIL_USER_NAME);   
        SendEmailChoreo.addInput("Password", GMAIL_APP_PASSWORD);   
        SendEmailChoreo.addInput("ToAddress", TO_EMAIL_ADDRESS);
        SendEmailChoreo.addInput("Subject", "ALERT");  
        SendEmailChoreo.addInput("MessageBody", "Air quality index is above normal conditions.\nYou can take the following precautions:\n1.Use mask over the mouth.\n2.Switch on the air purifier.\n3.Open the windows.\n4.Stop smoking.");
        digitalWrite(24,HIGH);      //switching on air purifiers
        digitalWrite(27,HIGH);      //opening the windows
        unsigned int returnCode = SendEmailChoreo.run();
        if (returnCode == 0) {
          Serial.println("Success! Email sent!");
        } 
        else {
          while (SendEmailChoreo.available()) {
            char c = SendEmailChoreo.read();
            Serial.print(c);
          }
        } 
        SendEmailChoreo.close();    
        attempted = true;
      }
    }
   if(h>45){
    digitalWrite(10,HIGH);
    Serial.println("Running SendAnEmail...");  
    TembooChoreo SendEmailChoreo(client);
    SendEmailChoreo.begin();    
    SendEmailChoreo.setAccountName(TEMBOO_ACCOUNT);
    SendEmailChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    SendEmailChoreo.setAppKey(TEMBOO_APP_KEY);
    SendEmailChoreo.setChoreo("/Library/Google/Gmail/SendEmail"); 
    SendEmailChoreo.addInput("Username", GMAIL_USER_NAME);   
    SendEmailChoreo.addInput("Password", GMAIL_APP_PASSWORD);   
    SendEmailChoreo.addInput("ToAddress", TO_EMAIL_ADDRESS);
    SendEmailChoreo.addInput("Subject", "ALERT");  
    SendEmailChoreo.addInput("MessageBody", "Humidity is above normal conditions.\nYou can take the following precautions:\n1.Switch on A.C.\n2.Switch on the Exhaust fan.\n3.Open the windows.");
    digitalWrite(28,HIGH);      //switching on a.c.
    digitalWrite(18,HIGH);      //switching on exhaust
    unsigned int returnCode = SendEmailChoreo.run();
    if (returnCode == 0) {
        Serial.println("Success! Email sent!");
    } 
    else {
     while (SendEmailChoreo.available()) {
        char c = SendEmailChoreo.read();
        Serial.print(c);
      }
    } 
    SendEmailChoreo.close();    
    }
char array[200];
    sprintf(array, "GET /update?api_key=BJF1ON9XATJ2ZBFR &field1=%d""&field2=%f""&field3=%f",sensorValue,t,h);
    client.println(array);
    client.println("Host: https://www.api.thingspeak.com");
    client.println("User-Agent: Energia/1.1");
    client.println("Connection: close");
    client.println();
    lastConnectionTime = millis();
  }
  else {
    Serial.println("connection failed");
  }
}
void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
