import paho.mqtt.publish as publish
MQTT_SERVER = "172.20.10.6"
MQTT_PATH = "test_channel2"

publish.single(MQTT_PATH, "Hello channel 2", hostname=MQTT_SERVER)

