#include <WiFi.h>
#include <PubSubClient.h>
#define BUT1 4
#define BUT2 0
#define LED1 14
#define LED2 12
#define LED3 13
int ledStatus1;
int ledStatus2;
int lastButton1State;
int lastButton2State;
int button1_state;
int button2_state;

const char* ssid = "";
const char* password = "";
 
#define MQTT_SERVER "192.168.2.86"
//#define MQTT_SERVER "broker.mqttdashboard.com"
#define MQTT_PORT 1883
#define MQTT_USER ""
#define MQTT_PASSWORD ""
 
#define MQTT_LED1_TOPIC "ESP32/LED1"
#define MQTT_LED2_TOPIC "ESP32/LED2"
#define MQTT_LED3_TOPIC "ESP32/LED3"
 
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
      client.subscribe(MQTT_LED3_TOPIC);
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
      ledStatus1 = HIGH;
      digitalWrite(LED1, HIGH);
    }
    else if(String(status) == "1stLEDisOFF")
    {
      ledStatus1 = LOW;
      digitalWrite(LED1, LOW);
    }
  }
 
  if(String(topic) == MQTT_LED2_TOPIC)
  {
    if(String(status) == "2ndLEDisONN")
    {
      ledStatus2 = HIGH;
      digitalWrite(LED2, HIGH);
    }
    else if(String(status) == "2ndLEDisOFF")
    {
      ledStatus2 = LOW;
      digitalWrite(LED2, LOW);
    }
  }
  if(String(topic) == MQTT_LED3_TOPIC)
  {
    if(String(status) == "LEDAttended")
    {
      digitalWrite(LED3, HIGH);
      //Serial.println("LEDAttended");
      delay(2000);
      digitalWrite(LED3, LOW);
      Serial.println("led3 turned off after 2s");
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
  pinMode(LED3, OUTPUT);
}
 
  
void loop() {
  client.loop();
  if (!client.connected()) {
    connect_to_broker();
  }
  lastButton1State = button1_state;
  lastButton2State = button2_state;
  button1_state = digitalRead(BUT1);
  button2_state = digitalRead(BUT2);
 
  if(lastButton1State == HIGH && button1_state == LOW) {
    Serial.println("The button 1 is pressed");
    ledStatus1 = !ledStatus1;
    digitalWrite(LED1, ledStatus1); 
    if(ledStatus1 == HIGH) client.publish(MQTT_LED1_TOPIC, "LED1 is ON via button");
    if(ledStatus1 == LOW) client.publish(MQTT_LED1_TOPIC, "LED1 is OFF via button");
  }
  
  if(lastButton2State == HIGH && button2_state == LOW) {
    Serial.println("The button 2 is pressed");
    ledStatus2 = !ledStatus2;
    digitalWrite(LED2, ledStatus2); 
    if(ledStatus2 == HIGH) client.publish(MQTT_LED2_TOPIC, "LED2 is ON via button");
    if(ledStatus2 == LOW) client.publish(MQTT_LED2_TOPIC, "LED2 is OFF via button");
  }
}
