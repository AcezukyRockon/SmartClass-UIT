# Get the database using the method we defined in pymongo_test_insert file 
from mongodb_init import get_database
import paho.mqtt.client as paho
import time
import json

# Create a new collection
dbname = get_database()
collection_name = dbname["LED"]
item_details = collection_name.find()
#category_index = collection_name.create_index("category")
#item_details = collection_name.find({"category" : "food"})

# from pandas import DataFrame
# counter = 0
# items_df = DataFrame(item_details)
# print(item_details)
# print(items_df)	

# hiveMQ setup
def on_publish(client, userdata, mid):
    print(item["Status"])
 
client = paho.Client()
client.on_publish = on_publish
client.username_pw_set("nhom1", "nhom1")
client.connect("broker.mqttdashboard.com", 1883)
client.loop_start()

for item in item_details:
    # see the magic
    # print(item["Status"]) (at on_publish)
    (rc, mid) = client.publish("LED/Status", item["Status"], qos=0)
    time.sleep(1)
