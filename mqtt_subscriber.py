import paho.mqtt.client as mqtt

MQTT_SERVER = "172.20.10.6"
MQTT_PATH = "test_channel"
def on_connect(client, userdata, flags, rc):
	print("Connected with result code "+str(rc))

	client.subscribe(MQTT_PATH)

def on_message(client, userdata, msg):
	print(msg.topic+" "+str(msg.payload))
	print_something()

def print_something():
	print("Something")


client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect(MQTT_SERVER, 1883, 60)

client.loop_forever()
