// Source: https://khuenguyencreator.com/lap-trinh-esp32-mqtt-bat-tat-den-voi-hivemq-broker/

#include <WiFi.h>
#include <PubSubClient.h>
#define LED1 13

/* 
Step 1: Hardware Setup
LED -> ESP32:
- VCC -> pin 13
- GND -> GND
Step 2: Setup wifi ssid and password (your wifi name and password)
Step 3: Go to http://www.hivemq.com/demos/websocket-client/, setup Username and Password, then fill at MQTT_USER and MQTT_PASSWORD
Step 4: Upload the code to ESP32 using Arduino IDE (remember set port and board). Hold boot button when reaching at "..." part.
Step 5: Back to Websocket site, click Connect, then on Publish, replace topic with "MQTT_ESP32/LED1". Do same on Subcribe area.
Step 6: On Publish area, type "ON" and click Publish button, your LED should be flash and the Subcribe should says message "ON"
Step 7: Try with "OFF" to turn off led. Done
*/

String ledStatus1 = "OFF";

// Wifi config
const char* ssid = "";
const char* password = "";

// MQTT config and user config: https://www.hivemq.com/public-mqtt-broker/
#define MQTT_SERVER "broker.mqttdashboard.com"
#define MQTT_PORT 1883
#define MQTT_USER "nhom1"
#define MQTT_PASSWORD "nhom1"
 
#define MQTT_LED1_TOPIC "LED/Status"
 
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
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      delay(2000);
    }
  }
}
 
void callback(char* topic, byte *payload, unsigned int length) {
  char status[20];
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
    if(String(status) == "OFF")
    {
      ledStatus1 = "OFF";
      digitalWrite(LED1, LOW);
      Serial.println("LED1 OFF");
    }
    else if(String(status) == "ON")
    {
      ledStatus1 = "ON";
      digitalWrite(LED1, HIGH);
      Serial.println("LED1 ON");
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
  //pinMode(BUT1, INPUT_PULLUP);
  pinMode(LED1, OUTPUT);
}
 
  
void loop() {
  client.loop();
  if (!client.connected()) {
    connect_to_broker();
  }
}

