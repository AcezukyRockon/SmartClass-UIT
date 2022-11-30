#include <WiFi.h>
#include <PubSubClient.h>
#define BUT1 4
#define BUT2 0
#define LED1 14
#define LED2 12
String ledStatus1 = "1stLEDisONN";
String ledStatus2 = "2ndLEDisONN";
 
const char* ssid = "";
const char* password = "";
 
#define MQTT_SERVER "192.168.196.86"
#define MQTT_PORT 1883
#define MQTT_USER ""
#define MQTT_PASSWORD ""
 
#define MQTT_LED1_TOPIC "ESP32/LED1"
#define MQTT_LED2_TOPIC "ESP32/LED2"
 
unsigned long previousMillis = 0; 
const long interval = 5000;
 
WiFiClient wifiClient;
PubSubClient client(wifiClient);
 
 
void setup_wifi() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
 
void connect_to_broker() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe(MQTT_LED1_TOPIC);
      client.subscribe(MQTT_LED2_TOPIC);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      delay(2000);
    }
  }
}
 
void callback(char* topic, byte *payload, unsigned int length) {
  char status[100];
  Serial.println("-------new message from broker-----");
  Serial.print("topic: ");
  Serial.println(topic);
  Serial.print("message: ");
  Serial.write(payload, length);
  Serial.println();
  for(int i = 0; i<length; i++)
  {
    status[i] = payload[i];
  }
  Serial.println(status);
  if(String(topic) == MQTT_LED1_TOPIC)
  {
    if(String(status) == "1stLEDisONN")
    {
      ledStatus1 = "1stLEDisONN";
      digitalWrite(LED1, HIGH);
      Serial.println("1stLEDisONN");
    }
    else if(String(status) == "1stLEDisOFF")
    {
      ledStatus1 = "1stLEDisOFF";
      digitalWrite(LED1, LOW);
      Serial.println("1stLEDisOFF");
    }
  }
 
  if(String(topic) == MQTT_LED2_TOPIC)
  {
    if(String(status) == "2ndLEDisONN")
    {
      ledStatus2 = "2ndLEDisONN";
      digitalWrite(LED2, HIGH);
      Serial.println("2ndLEDisONN");
    }
    else if(String(status) == "2ndLEDisOFF")
    {
      ledStatus2 = "2ndLEDisOFF";
      digitalWrite(LED2, LOW);
      Serial.println("2ndLEDisOFF");
    }
  }
   
}
 
void setup() {
  Serial.begin(9600);
  Serial.setTimeout(500);
  setup_wifi();
  client.setServer(MQTT_SERVER, MQTT_PORT );
  client.setCallback(callback);
  connect_to_broker();
  Serial.println("Start transfer");
  pinMode(BUT1, INPUT_PULLUP);
  pinMode(BUT2, INPUT_PULLUP);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
}
 
  
void loop() {
  client.loop();
  if (!client.connected()) {
    connect_to_broker();
  }
 
  if(digitalRead(BUT1) == 0) 
  {
    while (digitalRead(BUT1) == 0)
    {
      /* cho nut dc nha */
    }
    if(ledStatus1 == "1stLEDisOFF")
    {
      client.publish(MQTT_LED1_TOPIC, "1stLEDisONN");
      ledStatus1 = "1stLEDisONN";
    }
    else if(ledStatus1 == "1stLEDisONN")
    {
      client.publish(MQTT_LED1_TOPIC, "1stLEDisOFF");
      ledStatus1 = "1stLEDisOFF";
    }
  }
  if(digitalRead(BUT2) == 0) 
  {
    while (digitalRead(BUT2) == 0)
    {
      /* cho nut dc nha */
    }
    if(ledStatus2 == "2ndLEDisONN")
    {
      client.publish(MQTT_LED2_TOPIC, "2ndLEDisOFF");
      ledStatus2 = "2ndLEDisOFF";
    }
    else if(ledStatus2 == "2ndLEDisOFF")
    {
      client.publish(MQTT_LED2_TOPIC, "2ndLEDisONN");
       ledStatus2 = "2ndLEDisONN";
    }
  }
}
