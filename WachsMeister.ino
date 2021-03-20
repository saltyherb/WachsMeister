#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>
#include <analogWrite.h>

#define BLYNK_PRINT Serial

#define DHTTYPE DHT11
#define DHTPIN 4
#define PWMPIN = 27;

DHT dht(DHTPIN, DHTTYPE);

float temp;
float hum;
float light;
bool lightstate = false;
int value1;
int value2;
int value3;
int pwmload_vent = 5;
char LDRpin = A6;
String eventName = "pushtosheet";

//Set Blynk Virtual LED
WidgetLED led1(V2);

//Millis variables for timing
long SerialPreviousMillis = 0;
long SerialInterval = 5000;                 //Updatetime interval for Serial Monitor
long SheetPreviousMillis = 0;
long SheetInterval = 900000;                //Updatetime interval for Spreadsheet
unsigned long currentSheetMillis;
unsigned long currentSerialMillis;


const char * ssid = "Vodafone-0F87";
const char * pass = "pX3ixfaRvFg3KTdA";
const char auth[] = "0eORGPayoFHReYMb2TPx-a-M4Qq3g3o0";

String server = "http://maker.ifttt.com";
String IFTTT_Key = "nNmnqsxYPIjSxq-fWPiaa_zWe3ufmvtmEywIw5wkqmd";
String IFTTTUrl = "https://maker.ifttt.com/trigger/PotHum/with/key/nNmnqsxYPIjSxq-fWPiaa_zWe3ufmvtmEywIw5wkqmd";



void setup() {
  //Setting up sensors and interfaces
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  Blynk.begin(auth, ssid, pass);
  dht.begin();



  //Connecting to Wifi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Nice, Connected !!!");


}

BLYNK_WRITE(V3) 
{ int pwmload_vent = param.asInt();
Serial.println(pwmload_vent);
}


void pushtosheet(void)
{
  String url = server + "/trigger/" + eventName + "/with/key/" + IFTTT_Key + "?value1=" + String((int)value1) + "&value2=" + String((int)value2) + "&value3=" + String((int)value3);
  Serial.println(url);
  HTTPClient http;
  Serial.print("[HTTP] begin...\n");
  http.begin(url); //HTTP

  Serial.print("[HTTP] GET...\n");
  int httpCode = http.GET();
  // httpCode will be negative on error
  if (httpCode > 0) {

    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();

}


void lightcheck(void)
{ 

if (currentSheetMillis - SheetPreviousMillis > SheetInterval) {

    pushtosheet();
    SheetPreviousMillis = currentSheetMillis;
  }


  if (light >= 3500) {
    led1.on();
    lightstate = true;
  }
  else {
    led1.off();
    lightstate = false;
  }
}

void readLDR(void)
{
  int light = analogRead(LDRpin);
}

void readDHT(void)
{
  float hum = dht.readHumidity();
  float temp = dht.readTemperature();

}

void writevalues(void)
{
  value1 = temp;
  value2 = hum;
  value3 = light;
}

void writeserial(void)
{
    Serial.print((float)hum);
    Serial.println("%RLF");
    Serial.print((float)temp);
    Serial.println("°C");
    Serial.print((int)light);
    Serial.println("Lightmeasurement");
    Serial.print("Light 1 or 0? : ");
    Serial.println((bool)lightstate);
}

void loop() {

  unsigned long currentSheetMillis = millis();
  unsigned long currentSerialMillis = millis();
  
  Blynk.run();
  
  lightcheck();
  readDHT();


  float hum = dht.readHumidity();
  float temp = dht.readTemperature();

  value1 = temp;
  value2 = hum;
  value3 = light;

  if (currentSheetMillis - SheetPreviousMillis > SheetInterval) {


    writevalues();
    pushtosheet();
    SheetPreviousMillis = currentSheetMillis;
  }

   if (currentSerialMillis - SerialPreviousMillis > SerialInterval) {
    writeserial();
   }


}
