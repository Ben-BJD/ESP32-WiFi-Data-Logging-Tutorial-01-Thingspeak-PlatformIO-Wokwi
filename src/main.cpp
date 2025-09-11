#include <Arduino.h>

/*
  Configuration 
*/

// Replace with your network credentials
#define WIFI_SSID "<WIFI_SSID_NAME>"		    //replace with your WiFi network name
#define WIFI_PASSWORD "<WIFI_PASSWORD>"	    //replace with your WiFi password

//Thingspeak Settings
#define THINGSPEAK_CH_ID 00000			                            // replace 0000000 with your channel number
#define THINGSPEAK_WRITE_APIKEY "<THINGSPEAK_WRITE_API_KEY>"    // replace XYZ with your channel write API Key

#include <WiFi.h>
#include "ThingSpeak.h" // always include thingspeak header file after other header files and custom macros

/*
  Wifi 
*/

char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASSWORD;

//interval to wait before next wifi reconnection attempt (if connection is lost)
unsigned long wifiReconnectIntervalMs = 30000;
//millis based time reference - used for wifi reconnection
unsigned long lastTimeRecorded = 0;

// Create a WiFiClient object to manage the thingspeak connection
WiFiClient  client;

/*
  Thingspeak 
*/

unsigned long myChannelNumber = THINGSPEAK_CH_ID;
const char * myWriteAPIKey = THINGSPEAK_WRITE_APIKEY;

// Variable to hold a random number to be sent to the channel
int number1 = random(0, 1000);

// Function to connect and initialize WiFi
void initWiFi() 
{
  // We start by connecting to a WiFi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  //some cheaper modules may require to set a lower transmit power
  //default is 20.5dBm
  //WiFi.setTxPower(WIFI_POWER_8_5dBm); 

  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print('.');
    delay(300);
  }
  Serial.println(WiFi.localIP());
}

void setup() 
{
  Serial.begin(9600);
  initWiFi();
  
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() 
{

  //Wifi reconnection handling
  unsigned long currentMillis = millis();
  // if WiFi is down, try reconnecting every 30 seconds
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - lastTimeRecorded >=wifiReconnectIntervalMs)) 
  {
    Serial.print(millis());
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    lastTimeRecorded = currentMillis;
  }

  //set field value (example with random value between 0 and 1000)
  ThingSpeak.setField(1, number1);

  //more fields could be added here if needed (up to 8) ...
  //ThingSpeak.setField(2, number);
  //ThingSpeak.setField(3, string);
  //ThingSpeak.setField(n, ...);
  
  //update thingspeak channel with field values
  int result = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(result == 200)//200 is the HTTP response code for a successful update
  {
    Serial.println("Channel update successful.");
  }
  else
  {
    Serial.print("Problem updating channel. HTTP error code: " );
    Serial.println(result);
  }

  // generate new random values for next update
  number1 = random(0, 1000);
  
  delay(20000); // Wait 20 seconds to update the channel again
}