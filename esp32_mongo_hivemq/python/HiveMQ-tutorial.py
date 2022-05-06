#source: https://www.hivemq.com/blog/mqtt-client-library-paho-python/

import paho.mqtt.client as paho
import time

def on_publish(client, userdata, mid):
    print("mid: "+str(mid))
 
client = paho.Client()
client.on_publish = on_publish
client.username_pw_set("fireser", "abcd1234")
client.connect("broker.mqttdashboard.com", 1883)
client.loop_start()
temperature = 0

while True:
    temperature += 1
    (rc, mid) = client.publish("LED/Status", str(temperature), qos=0)
    time.sleep(1)