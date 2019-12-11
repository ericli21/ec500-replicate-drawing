#include <AFMotor.h>
#include <Wire.h>


// BASE RANGE: 50 (LEFT) to 463(RIGHT), 315 MID
// SHOULDER RANGE: 245 (up) to 945 (DOWN), 325 MID
// ELBOW RANGE: 145 (UP) to 610 (DOWN), 415 MID
// WRIST RANGE: 370 (UP) to 490 (DOWN) , 512 MID

// drawing pos
// elbow 143
// wrist 490
// base 280
// shoulder 1023

// Sketch variables, motor declarations
AF_DCMotor baseMotor(1); //A0
AF_DCMotor shoulderMotor(2); //A1
AF_DCMotor elbowMotor(3); //A2
AF_DCMotor wristMotor(4); //A3

// Point Offsets
int diffArray [10] [4] = {
 {14, 92, -44, 81}, // START PG
 {1, 103, -103, -1}, // PG -Y
 {-15, -195, 147, -80}, // -Y START
 {-15, 70, -125, 0}, // -Y SQ
 {-20, -116, 219, -30}, // SQ +X
 {20, -149, 53, -50}, // +X START
 {15, 195, -147, 80}, // START -Y
 {-20, -13, 25, -6}, // SQ HG 
 {0, -103, 194, -24}, // HG +X
 {50, 0, 0, 0}  //offest
};

// Signals for the Raspberry Pi
char inShapes[15];
int shapes[3];
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
  delay(1500);
  int ret3 = topLeft();
  delay(1500);
  int ret2 =  drawStartY(diffArray[6]);

/*
  if(ret == 1)
  {

    // Ready to draw
    armReady = true;

    while(drawingReceived == false)
    {
      delay(250);
    }

    // Drawing
    armReady = false;
    
    for(int i = 0; i < (sizeof(shapes) / sizeof(shapes[0])); i++) 
    {
      switch (shapes[i]) {
        case 3: //Triangle
          {
            Serial.println("Triangle");
            //drawLineXYPos(false, int delayTime)
            //drawLineXYNeg(true, int delayTime)
            //drawLineX(true, int delayTime)
          }
          break;
        case 4: // Square
          {
            Serial.println("Square");
            //drawLineX(true, int delayTime)
            //drawLineY(true, int delayTime)
            //drawLineX(false, int delayTime)
            //drawLineY(false, int delayTime)
          }
          break;
        case 5: // Pentagon
          {
            Serial.println("Pentagon");
            //drawLineXYPos(false, int delayTime)
            //drawLineXYNeg(true, int delayTime)
            //drawLineXYPos(true, int delayTime)
            //drawLineX(false, int delayTime)
            //drawLineXYNeg(false, int delayTime)
          }
          break;
        case 6: // Hexagon
          {
            Serial.println("Hexagon");
            //drawLineX(false, int delayTime)
            //drawLineXYNeg(true, int delayTime)
            //drawLineXYPos(true, int delayTime)
            //drawLineX(false, int delayTime)
            //drawLineXYNeg(false, int delayTime)
            //drawLineXYPos(false, int delayTime)
          }
          break;
        case 8: // Circle
          {
            Serial.println("Circle");
            //drawCircle(true, int delayTime)
          }
          break;
        default:
          Serial.println("No more shapes!");
          break;
      }
    }
  }
*/
  // Release motors
  wristMotor.run(RELEASE);
  baseMotor.run(RELEASE);
  elbowMotor.run(RELEASE);
  shoulderMotor.run(RELEASE);
  
  delay(50000);
  
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

    if(!inRangeShoulder)
    {
      // Shoulder motor reset, reset value range is 320 - 325
      while(sensorValueA1 < 400 || sensorValueA1 > 405)
      {
        if(sensorValueA1 < 400)
        {
          shoulderMotor.run(FORWARD);
          sensorValueA1 = analogRead(A1);
          Serial.print("Shoulder (A1) value BACKWARD: ");
          Serial.println(sensorValueA1);
          delay(50);
        }
        else if(sensorValueA1 > 405)
        {
          shoulderMotor.run(BACKWARD);
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
  bool inRangeBase = false;
  bool inRangeShoulder = false;
  bool inRangeElbow = false;
  bool inRangeWrist = false;

  while(!(inRangeBase && inRangeShoulder && inRangeElbow && inRangeWrist))
  {
    Serial.println("Resetting drawing position");
    
    // Base motor reset, reset value range is 930 - 935
    if(!inRangeBase)
    {
      while(sensorValueA0 < 278 || sensorValueA0 > 283)
      {
        if(sensorValueA0 < 278)
        {
          baseMotor.run(BACKWARD);
          sensorValueA0 = analogRead(A0);
          Serial.print("Base (A0) value: ");
          Serial.println(sensorValueA0);
          delay(10);
        }
        else if(sensorValueA0 > 283)
        {
          baseMotor.run(FORWARD);
          sensorValueA0 = analogRead(A0);
          Serial.print("Base (A2) value: ");
          Serial.println(sensorValueA0);
          delay(10);
        }
      }
      // Stop base motor
      Serial.println("Base motor drawing position reached");
      baseMotor.run(RELEASE);
      inRangeBase = true;
    }

    if(!inRangeWrist)
    {
      // Wrist motor reset, reset value range is 465 - 470
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
      Serial.println("Wrist motor drawing position reached");
      wristMotor.run(RELEASE);
      inRangeWrist = true;
    }

    if(!inRangeElbow)
    {
      // Elbow motor reset, reset value range is 75 - 80
      while(sensorValueA2 < 150 || sensorValueA2 > 155)
      {
        if(sensorValueA2 < 150)
        {
          elbowMotor.run(FORWARD);
          sensorValueA2 = analogRead(A2);
          Serial.print("Elbow (A1) value: ");
          Serial.println(sensorValueA2);
          delay(10);
        }
        else if(sensorValueA2 > 155)
        {
          elbowMotor.run(BACKWARD);
          sensorValueA2 = analogRead(A2);
          Serial.print("Elbow (A2) value: ");
          Serial.println(sensorValueA2);
          delay(10);
        }
      }
    
      // Stop elbow motor
      Serial.println("Elbow motor drawing position reached");
      elbowMotor.run(RELEASE);
      inRangeElbow = true;
    }

    if(!inRangeShoulder)
    {
      // Shoulder motor reset, reset value range is 515 - 520
      while(sensorValueA1 < 923 || sensorValueA1 > 928)
      {
        if(sensorValueA1 < 923)
        {
          shoulderMotor.run(FORWARD);
          sensorValueA1 = analogRead(A1);
          Serial.print("Shoulder (A1) value: ");
          Serial.println(sensorValueA1);
          delay(10);
        }
        else if(sensorValueA1 > 928)
        {
          shoulderMotor.run(BACKWARD);
          sensorValueA1 = analogRead(A1);
          Serial.print("Shoulder (A1) value: ");
          Serial.println(sensorValueA1);
          delay(10);
        }
      }
    
      // Stop shoulder motor
      shoulderMotor.run(RELEASE);
      Serial.println("Shoulder motor drawing position reached");
      inRangeShoulder = true;
    }
    
  }
  
  Serial.println("Drawing position reached");
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
  int index = 0;
  
  while(0 < Wire.available()) // loop through all but the last
  {
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character

    inShapes[index] = c;
    index++;
    inShapes[index] = '\0'; // Keep the string NULL terminated
  }

  // Parse through shapes data
  int shapesNum = atoi(inShapes);
  index = 0;
  
  while(shapesNum > 0 && index < 3)
  {
    shapes[index] = shapesNum % 10;
    shapesNum = shapesNum / 10;
    index++;
  }

  drawingReceived = true;

}

void requestEvent()
{
  if (armReady == true) {
    Wire.write ("A", 1);
  }
  else {
    // Do nothing
  }
}













// Draw from given position to another given point
int drawStartY(int diffArray[4]){

  // Potentiometer values
  int sensorValueA0 = analogRead(A0);
  int sensorValueA1 = analogRead(A1);
  int sensorValueA2 = analogRead(A2);
  int sensorValueA3 = analogRead(A3);

  // Flags
  bool inRangeBase = true;
  bool inRangeShoulder = false;
  bool inRangeElbow = false;
  bool inRangeWrist = false;

  // Desired readings
  int basePoint = sensorValueA0 - diffArray[0];
  int shoulderPoint = sensorValueA1 - diffArray[1];
  int elbowPoint = sensorValueA2 - diffArray[2] + 45;
  int wristPoint = sensorValueA3 - diffArray[3];

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

  Serial.print("Base value: ");
  Serial.println(basePoint);
  Serial.print("Shoulder value: ");
  Serial.println(shoulderPoint);
  Serial.print("Elbow value: ");
  Serial.println(elbowPoint);
  Serial.print("Wrist value: ");
  Serial.println(wristPoint);

  delay(5000);
  
  while(!(inRangeShoulder && inRangeElbow && inRangeWrist))
  {

        if(sensorValueA1 < (shoulderPoint - 3) && !inRangeShoulder)
        {
          shoulderMotor.run(FORWARD);
          sensorValueA1 = analogRead(A1);
          Serial.print("Shoulder (A1) value BACKWARD: ");
          Serial.println(sensorValueA1);
        }
        else if(sensorValueA1 > (shoulderPoint + 3) && !inRangeShoulder)
        {
          shoulderMotor.run(BACKWARD);
          sensorValueA1 = analogRead(A1);
          Serial.print("Shoulder (A1) value FORWARD: ");
          Serial.println(sensorValueA1);
        }
        else
        {
          // Stop shoulder motor
          shoulderMotor.run(RELEASE);
          inRangeShoulder = true;
        }

        if(sensorValueA2 < (elbowPoint - 3) && !inRangeElbow)
        {
          elbowMotor.run(FORWARD);
          sensorValueA2 = analogRead(A2);
          Serial.print("Elbow (A2) value: ");
          Serial.println(sensorValueA2);
        }
        else if(sensorValueA2 > (elbowPoint + 3) && !inRangeElbow)
        {
          elbowMotor.run(BACKWARD);
          sensorValueA2 = analogRead(A2);
          Serial.print("Elbow (A2) value: ");
          Serial.println(sensorValueA2);
        }
        else
        {
          // Stop elbow motor
          elbowMotor.run(RELEASE);
          inRangeElbow = true;
        }
        
        if(sensorValueA3 < (wristPoint - 3) && !inRangeWrist)
        {
          wristMotor.run(FORWARD);
          sensorValueA3 = analogRead(A3);
          Serial.print("Wrist (A3) value: ");
          Serial.println(sensorValueA3);
        }
        else if(sensorValueA3 > (wristPoint + 3) && !inRangeWrist)
        {
          wristMotor.run(BACKWARD);
          sensorValueA3 = analogRead(A3);
          Serial.print("Wrist (A3) value: ");
          Serial.println(sensorValueA3);
        }
        else
        {
          // Stop wrist motor
          Serial.println("Wrist motor starting position reached");
          wristMotor.run(RELEASE);
          inRangeWrist = true;
        }

        delay(50);
    
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
  
  Serial.println("Segment drawn");
  return 1;
}
