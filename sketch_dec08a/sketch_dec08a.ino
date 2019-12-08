//Slave
#include <Wire.h>

void setup()
{
  Wire.begin(4);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  Wire.onRequest(requestEvent);
  Serial.begin(115200);           // start serial for output
}

void loop()
{
  delay(100);
}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  while(1 < Wire.available()) // loop through all but the last
  {
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
  }
  char x = Wire.read();    // receive byte as an integer
  Serial.println(x);         // print the integer
//  Wire.beginTransmission(5); // transmit to device #4
//  Wire.write("x is ");        // sends five bytes
////  Wire.write(x);              // sends one byte  
//  Serial.println("Done");
//  Wire.endTransmission();
}

byte x = 0;

void requestEvent ()
{
  if (x == 0) {
    Wire.write ("A", 1);
    x = 1;
  }
  else {
    Wire.write ("B", 1);
    x = 0;
  }
} // end of requestEvent
