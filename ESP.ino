//Master
#include <ESP8266WiFi.h> // Enables the ESP8266 to connect to the local network (via WiFi)
#include <PubSubClient.h> // Allows us to connect to, and publish to the MQTT broker
#include <Wire.h>

// WiFi
const char* ssid = "Eric";
const char* wifi_password = "testtest123";

// MQTT
// Make sure to update this for your own MQTT Broker!
const char* mqtt_server = "172.20.10.7";
const char* mqtt_topic = "test_channel2";
const char* mqtt_username = "Eric";
const char* mqtt_password = "Li";
// The client id identifies the ESP8266 device. Think of it a bit like a hostname (Or just a name, like Greg).
const char* clientID = "Client ID";

// Initialise the WiFi and MQTT Client objects
WiFiClient wifiClient;
PubSubClient client(mqtt_server, 1883, wifiClient); // 1883 is the listener port for the Broker

//Callback function to process MQTT dat from the RasPi
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received in topic: ");
  Serial.print(topic);
  Serial.print("   length is:");
  Serial.println(length);

  //Trandfer data byte by byte to Uno via I2C write
  Wire.beginTransmission(4);
  Serial.print("Data Received From Broker:");
  //Max of 3 bytes sent to Uno
  if (length > 3) {
    length = 3;
  }
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    Wire.write((char)payload[i]);
  }
  Wire.endTransmission();

  Serial.println();
  Serial.println("-----------------------");
  Serial.println();

}

//Set up WiFi client, subscribe to test_channel1 for shape information
void setup()
{
  Wire.begin(); // join i2c bus (address optional for master)
//  Wire.onReceive(receiveEvent);
  Serial.begin(115200); 
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Connect to the WiFi
  WiFi.begin(ssid, wifi_password);

  // Wait until the connection has been confirmed before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  // Debugging - Output the IP Address of the ESP8266
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Connect to MQTT Broker
  // client.connect returns a boolean value to let us know if the connection was successful.
  // If the connection is failing, make sure you are using the correct MQTT Username and Password (Setup Earlier in the Instructable)
  if (client.connect(clientID, mqtt_username, mqtt_password)) {
    
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }
  client.subscribe("test_channel1");

}

int X0;

//Loop to refresh the client, and periodically request to see if the arm is available
//If arm is available, Uno Slave will send a byte "A" to ESP, which will publish an armReady message to RasPi
void loop()
{
  client.loop();

  delay(10000);
  Wire.requestFrom(4,1); // Request the transmitted two bytes from the two registers
  
  if(Wire.available()<=1) {  // 
    X0 = Wire.read(); // Reads the data from the register 
  }
  Serial.print("X0 = ");
  Serial.println(X0);
  if (X0 == 65) {
    client.publish(mqtt_topic, "armReady");
  }
  else {
    Serial.println("notReady");
  }
}


