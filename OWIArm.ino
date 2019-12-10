#include <AFMotor.h>
#include <Wire.h>


// BASE RANGE: 50 (LEFT) to 463(RIGHT), 315 MID
// SHOULDER RANGE: 245 (up) to 945 (DOWN), 325 MID
// ELBOW RANGE: 145 (UP) to 610 (DOWN), 415 MID
// WRIST RANGE: 370 (UP) to (DOWN) , 512 MID

// Sketch variables, motor declarations
AF_DCMotor baseMotor(1); //A0
AF_DCMotor shoulderMotor(2); //A1
AF_DCMotor elbowMotor(3); //A2
AF_DCMotor wristMotor(4); //A3

// Signals for the Raspberry Pi
bool armReady = false;
bool drawingReceived = false;

void setup() {

  // Set up serial monitor for debugging purposes
  Serial.begin(9600);

  // Set the speed of all motors
  baseMotor.setSpeed(150);
  shoulderMotor.setSpeed(150);
  elbowMotor.setSpeed(150);
  wristMotor.setSpeed(150);

  // Set up LEDs
  pinMode(7, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(4, OUTPUT);

  // MQTT stuff
  Wire.begin(4);                // join i2c bus with address #4
  Wire.onReceive(receiveEvent); // register event
  Wire.onRequest(requestEvent);

}

void loop() {

  // Release motors
  wristMotor.run(RELEASE);
  baseMotor.run(RELEASE);
  elbowMotor.run(RELEASE);
  shoulderMotor.run(RELEASE);
  
  // Potentiometer values
  int sensorValueA0 = analogRead(A0);
  int sensorValueA1 = analogRead(A1);
  int sensorValueA2 = analogRead(A2);
  int sensorValueA3 = analogRead(A3);
  /*
  // Test LEDs
  digitalWrite(7, HIGH);
  delay(1000);
  digitalWrite(6, HIGH);
  delay(1000);
  digitalWrite(5, HIGH);
  delay(1000);
  digitalWrite(4, HIGH);
  delay(1000);

  digitalWrite(7, LOW);
  delay(1000);
  digitalWrite(6, LOW);
  delay(1000);
  digitalWrite(5, LOW);
  delay(1000);
  digitalWrite(4, LOW);
  delay(1000);
*/
  // Wait for signal from Raspberry Pi
  while(drawingReceived == false)
  {
    // Waiting for drawing here
  }

  // Print sensor value
  Serial.print("Base (A0) value: ");
  Serial.println(sensorValueA0);
  Serial.print("Shoulder (A1) value: ");
  Serial.println(sensorValueA1);
  Serial.print("Elbow (A2) value: ");
  Serial.println(sensorValueA2);
  Serial.print("Wrist (A3) value: ");
  Serial.println(sensorValueA3);
  Serial.println("");


  // Bring arm back to starting position
  int ret = resetPos();

  int shapes[] = {2, 3, 4};

  if(ret == 1)
  {
    for(int i = 0; i < (sizeof(shapes) / sizeof(shapes[0])); i++) 
    {
      switch (shapes[i]) {
        case 1: //Triangle
          {
            //drawLineXYPos(false, int delayTime)
            //drawLineXYNeg(true, int delayTime)
            //drawLineX(true, int delayTime)
          }
          break;
        case 2: // Square
          {
            //drawLineX(true, int delayTime)
            //drawLineY(true, int delayTime)
            //drawLineX(false, int delayTime)
            //drawLineY(false, int delayTime)
          }
          break;
        case 3: // Pentagon
          {
            //drawLineXYPos(false, int delayTime)
            //drawLineXYNeg(true, int delayTime)
            //drawLineXYPos(true, int delayTime)
            //drawLineX(false, int delayTime)
            //drawLineXYNeg(false, int delayTime)
          }
          break;
        case 4: // Hexagon
          {
            //drawLineX(false, int delayTime)
            //drawLineXYNeg(true, int delayTime)
            //drawLineXYPos(true, int delayTime)
            //drawLineX(false, int delayTime)
            //drawLineXYNeg(false, int delayTime)
            //drawLineXYPos(false, int delayTime)
          }
          break;
        case 5: // Circle
          {
            //drawCircle(true, int delayTime)
          }
          break;
        default:
          Serial.println("Error in shape input!");
          break;
      }
    }
  }

  // Broascast "Arm Ready" signal, reset for next drawing
  armReady = true;
  drawingReceived = false;

  delay(1000);
  
}

// Reach starting position
int resetPos(){

  // Potentiometer values
  int sensorValueA0 = analogRead(A0);
  int sensorValueA1 = analogRead(A1);
  int sensorValueA2 = analogRead(A2);
  int sensorValueA3 = analogRead(A3);

  // Flags
  bool inRangeBase = false;
  bool inRangeShoulder = false;
  bool inRangeElbow = false;
  bool inRangeWrist = false;

  while(!(inRangeBase && inRangeShoulder && inRangeElbow && inRangeWrist))
  {
    Serial.println("Resetting arm position");
    
    // Base motor reset, reset value range is 320 - 325
    if(!inRangeBase)
    {
      while(sensorValueA0 < 315 || sensorValueA0 > 320)
      {
        if(sensorValueA0 < 315)
        {
          baseMotor.run(BACKWARD);
          sensorValueA0 = analogRead(A0);
          Serial.print("Base (A0) value: ");
          Serial.println(sensorValueA0);
          delay(10);
        }
        else if(sensorValueA0 > 320)
        {
          baseMotor.run(FORWARD);
          sensorValueA0 = analogRead(A0);
          Serial.print("Base (A0) value: ");
          Serial.println(sensorValueA0);
          delay(10);
        }
      }
      // Stop base motor
      Serial.println("Base motor starting position reached");
      baseMotor.run(RELEASE);
      inRangeBase = true;
    }

    if(!inRangeWrist)
    {
      // Wrist motor reset, reset value range is 510 - 515
      while(sensorValueA3 < 510 || sensorValueA3 > 515)
      {
        if(sensorValueA3 < 510)
        {
          wristMotor.run(FORWARD);
          sensorValueA3 = analogRead(A3);
          Serial.print("Wrist (A3) value: ");
          Serial.println(sensorValueA3);
          delay(10);
        }
        else if(sensorValueA3 > 515)
        {
          wristMotor.run(BACKWARD);
          sensorValueA3 = analogRead(A3);
          Serial.print("Wrist (A3) value: ");
          Serial.println(sensorValueA3);
          delay(10);
        }
      }
      // Stop wrist motor
      Serial.println("Wrist motor starting position reached");
      wristMotor.run(RELEASE);
      inRangeWrist = true;
    }

    if(!inRangeElbow)
    {
      // Elbow motor reset, reset value range is 510 - 515
      while(sensorValueA2 < 415 || sensorValueA2 > 420)
      {
        if(sensorValueA2 < 415)
        {
          elbowMotor.run(FORWARD);
          sensorValueA2 = analogRead(A2);
          Serial.print("Elbow (A2) value: ");
          Serial.println(sensorValueA2);
          delay(10);
        }
        else if(sensorValueA2 > 420)
        {
          elbowMotor.run(BACKWARD);
          sensorValueA2 = analogRead(A2);
          Serial.print("Elbow (A2) value: ");
          Serial.println(sensorValueA2);
          delay(10);
        }
      }
    
      // Stop elbow motor
      elbowMotor.run(RELEASE);
      inRangeElbow = true;
    }

    if(!inRangeShoulder)
    {
      // Shoulder motor reset, reset value range is 320 - 325
      while(sensorValueA1 < 320 || sensorValueA1 > 325)
      {
        if(sensorValueA1 < 320)
        {
          shoulderMotor.run(BACKWARD);
          sensorValueA1 = analogRead(A1);
          Serial.print("Shoulder (A1) value BACKWARD: ");
          Serial.println(sensorValueA1);
          delay(50);
        }
        else if(sensorValueA1 > 325)
        {
          shoulderMotor.run(FORWARD);
          sensorValueA1 = analogRead(A1);
          Serial.print("Shoulder (A1) value FORWARD: ");
          Serial.println(sensorValueA1);
          delay(50);
        }
      }
    
      // Stop shoulder motor
      shoulderMotor.run(RELEASE);
      inRangeShoulder = true;
    }
    
  }
  
  Serial.println("Starting position reached");
  return 1;
}


// Reach starting position for drawing
int topLeft(){

  // Potentiometer values
  int sensorValueA0 = analogRead(A0);
  int sensorValueA1 = analogRead(A1);
  int sensorValueA2 = analogRead(A2);
  int sensorValueA3 = analogRead(A3);

  // Flags
  bool inRangeBase = true;
  bool inRangeShoulder = true;
  bool inRangeElbow = false;
  bool inRangeWrist = true;

  while(!(inRangeBase && inRangeShoulder && inRangeElbow && inRangeWrist))
  {
    Serial.println("Resetting arm position");
    
    // Base motor reset, reset value range is 930 - 935
    if(!inRangeBase)
    {
      while(sensorValueA0 < 930 || sensorValueA0 > 935)
      {
        if(sensorValueA0 < 930)
        {
          baseMotor.run(BACKWARD);
          sensorValueA0 = analogRead(A0);
          Serial.print("Base (A0) value: ");
          Serial.println(sensorValueA0);
          delay(10);
        }
        else if(sensorValueA0 > 935)
        {
          baseMotor.run(FORWARD);
          sensorValueA0 = analogRead(A0);
          Serial.print("Base (A2) value: ");
          Serial.println(sensorValueA0);
          delay(10);
        }
      }
      // Stop base motor
      Serial.println("Base motor starting position reached");
      baseMotor.run(RELEASE);
      inRangeBase = true;
    }

    if(!inRangeWrist)
    {
      // Wrist motor reset, reset value range is 465 - 470
      while(sensorValueA3 < 465 || sensorValueA3 > 470)
      {
        if(sensorValueA3 < 465)
        {
          wristMotor.run(FORWARD);
          sensorValueA3 = analogRead(A3);
          Serial.print("Wrist (A3) value: ");
          Serial.println(sensorValueA3);
          delay(10);
        }
        else if(sensorValueA3 > 470)
        {
          wristMotor.run(BACKWARD);
          sensorValueA3 = analogRead(A3);
          Serial.print("Wrist (A3) value: ");
          Serial.println(sensorValueA3);
          delay(10);
        }
      }
      // Stop wrist motor
      Serial.println("Wrist motor starting position reached");
      wristMotor.run(RELEASE);
      inRangeWrist = true;
    }

    if(!inRangeElbow)
    {
      // Elbow motor reset, reset value range is 75 - 80
      while(sensorValueA1 < 145 || sensorValueA1 > 150)
      {
        if(sensorValueA1 < 145)
        {
          elbowMotor.run(FORWARD);
          sensorValueA1 = analogRead(A1);
          Serial.print("Elbow (A1) value: ");
          Serial.println(sensorValueA1);
          delay(10);
        }
        else if(sensorValueA1 > 150)
        {
          elbowMotor.run(BACKWARD);
          sensorValueA1 = analogRead(A1);
          Serial.print("Elbow (A1) value: ");
          Serial.println(sensorValueA1);
          delay(10);
        }
      }
    
      // Stop elbow motor
      elbowMotor.run(RELEASE);
      inRangeElbow = true;
    }

    if(!inRangeShoulder)
    {
      // Shoulder motor reset, reset value range is 515 - 520
      while(sensorValueA0 < 515 || sensorValueA0 > 520)
      {
        if(sensorValueA0 < 515)
        {
          shoulderMotor.run(FORWARD);
          sensorValueA0 = analogRead(A0);
          Serial.print("Shoulder (A0) value: ");
          Serial.println(sensorValueA0);
          delay(10);
        }
        else if(sensorValueA0 > 520)
        {
          shoulderMotor.run(BACKWARD);
          sensorValueA0 = analogRead(A0);
          Serial.print("Shoulder (A0) value: ");
          Serial.println(sensorValueA0);
          delay(10);
        }
      }
    
      // Stop shoulder motor
      shoulderMotor.run(RELEASE);
      inRangeShoulder = true;
    }
    
  }
  
  Serial.println("Starting position reached");
  return 1;
}

// Draw verticle line
// Need direction and delay (pseudo length) as input
void drawLineY(bool direction, int delayTime){

  // Shoulder, Elbow, and Wrist motors only
  if(direction) // +Y
  {
    shoulderMotor.run(BACKWARD);
    elbowMotor.run(BACKWARD);
    wristMotor.run(FORWARD);  
  }
  else // -Y
  {
    shoulderMotor.run(FORWARD);
    elbowMotor.run(FORWARD);
    wristMotor.run(BACKWARD);
  }
  
  delay(delayTime);

  // Release all motors
  shoulderMotor.run(RELEASE);
  elbowMotor.run(RELEASE);
  wristMotor.run(RELEASE);
  
}

// Draw horizontal line
// Need direction and delay (pseudo length) as input
void drawLineX(bool direction, int delayTime){

  // Calibrate motor speed to offset base motor arc
  baseMotor.setSpeed(100);
  shoulderMotor.setSpeed(100);
  elbowMotor.setSpeed(100);
  wristMotor.setSpeed(100);
  
  // All motors used
  if(direction) // +X (LEFT)
  {
    baseMotor.run(FORWARD);
    
    shoulderMotor.run(FORWARD);
    elbowMotor.run(FORWARD);
    wristMotor.run(BACKWARD);
  }
  else // -X (RIGHT)
  {
    baseMotor.run(BACKWARD);
    shoulderMotor.run(BACKWARD);
    elbowMotor.run(BACKWARD);
    wristMotor.run(FORWARD);
  }
  
  delay(delayTime);

  baseMotor.run(RELEASE);
  shoulderMotor.run(RELEASE);
  elbowMotor.run(RELEASE);
  wristMotor.run(RELEASE);
  
}

// Draw negative slope diagonal line
// Need direction and delay (pseudo length) as input
// True is bottom to top
// False is top to bottom
void drawLineXYNeg(bool direction, int delayTime){

  // Calibrate motor speed to offset base motor arc
  baseMotor.setSpeed(100);
  shoulderMotor.setSpeed(100);
  elbowMotor.setSpeed(100);
  wristMotor.setSpeed(100);
  
  // All motors used
  if(direction) // Top to bottom
  {
    baseMotor.run(FORWARD);
    
    shoulderMotor.run(FORWARD);
    elbowMotor.run(FORWARD);
    wristMotor.run(BACKWARD);
  }
  else // Bottom to top
  {
    baseMotor.run(BACKWARD);
    shoulderMotor.run(BACKWARD);
    elbowMotor.run(BACKWARD);
    wristMotor.run(FORWARD);
  }
  
  delay(delayTime);

  baseMotor.run(RELEASE);
  shoulderMotor.run(RELEASE);
  elbowMotor.run(RELEASE);
  wristMotor.run(RELEASE);
  
}

// Draw positive slope diagonal line
// Need direction and delay (pseudo length) as input
void drawLineXYPos(bool direction, int delayTime){

  // Calibrate motor speed to offset base motor arc
  baseMotor.setSpeed(100);
  shoulderMotor.setSpeed(100);
  elbowMotor.setSpeed(100);
  wristMotor.setSpeed(100);
  
  // All motors used
  if(direction) // Top to bottom
  {
    baseMotor.run(FORWARD);
    shoulderMotor.run(FORWARD);
    elbowMotor.run(FORWARD);
    wristMotor.run(BACKWARD);
  }
  else // Bottom to Top
  {
    baseMotor.run(BACKWARD);
    shoulderMotor.run(BACKWARD);
    elbowMotor.run(BACKWARD);
    wristMotor.run(FORWARD);
  }
  
  delay(delayTime);

  baseMotor.run(RELEASE);
  shoulderMotor.run(RELEASE);
  elbowMotor.run(RELEASE);
  wristMotor.run(RELEASE);
  
}

// Draw a circle
void drawCircle(){

  // Calibrate motor speed to offset base motor arc
  baseMotor.setSpeed(100);
  shoulderMotor.setSpeed(100);
  elbowMotor.setSpeed(100);
  wristMotor.setSpeed(100);

  baseMotor.run(RELEASE);
  shoulderMotor.run(RELEASE);
  elbowMotor.run(RELEASE);
  wristMotor.run(RELEASE);
  
}


// Function that executes whenever data is received from master
// This function is registered as an event, see setup()
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
}
