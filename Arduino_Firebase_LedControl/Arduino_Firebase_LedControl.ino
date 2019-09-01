#include <FirebaseArduino.h>
#include <ESP8266WiFi.h>
#include <DHT.h>

#define LED_PIN D7


// Config DHT
#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);


// Config Firebase
#define FIREBASE_HOST "ledesp8266-ad24a.firebaseio.com"
#define FIREBASE_AUTH "VAjJu2YYL2tE3Q21ogdTDA1DoUPTZSA0j5Tsfs9v"
#define WIFI_SSID "iPadSun"
#define WIFI_PASSWORD "23march2548"

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(A0,INPUT);
  
  Serial.begin(115200);
  
  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.stream("/Node1/LED");  
}

void loop() {
  if (Firebase.failed()) {
    Serial.println("streaming error");
    Serial.println(Firebase.error());
  }
  
  if (Firebase.available()) {
     FirebaseObject event = Firebase.readEvent();
     String eventType = event.getString("type");
     eventType.toLowerCase();

     if (eventType == "") return ;
     Serial.print("event: ");
     Serial.println(eventType);
     if (eventType == "put") {
       String path = event.getString("path");
       int data = event.getInt("data");
       Serial.println("[" + path + "] " + String(data));
       if (path == "/") {
         digitalWrite(LED_PIN, (data == 0 ? LOW : HIGH));
       }
     }
  }   
  delay(10);

  int vr = analogRead(A0);

  Firebase.pushInt("VRanalogPush", vr);
  Firebase.setInt("VRanalogSet", vr);

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  int number = Firebase.getInt("Number");

  Serial.print("number :");
  Serial.println(number);

  if (isnan(h) || isnan(t)) {
   // Serial.println("Failed to read from DHT sensor!");
    delay(500);
    return;
  }

  // append a new value to /temperature
  Firebase.pushFloat("temperature", 25.0);
  if (Firebase.failed()) {
      Serial.print("pushing /temperature failed:");
      Serial.println(Firebase.error());  
      return;
  }

  // append a new value to /temperature
   Firebase.pushFloat("humidity", 20.0);
  if (Firebase.failed()) {
      Serial.print("pushing /humidity failed:");
      Serial.println(Firebase.error());  
      return;
  }
  
  delay(5000);
}
