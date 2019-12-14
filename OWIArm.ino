#include <AFMotor.h>
#include <Wire.h>

// Sketch variables, motor declarations
AF_DCMotor baseMotor(1);      //A0
AF_DCMotor shoulderMotor(2);  //A1
AF_DCMotor elbowMotor(3);     //A2
AF_DCMotor wristMotor(4);     //A3

// Point Offsets
int diffArray [10] [4] = {
 {14, 92, -44, 81},         // START PG     0
 {1, 103, -103, -1},        // PG -Y        1
 {-15, -195, 147, -80},     // -Y START     2
 {-15, 70, -125, 0},        // -Y SQ        3
 {-20, -116, 219, -30},     // SQ +X        4
 {20, -149, 53, -50},       // +X START     5
 {15, 195, -147, 80},       // START -Y     6
 {-20, -13, 25, -6},        // SQ HG        7
 {0, -103, 194, -24},       // HG +X        8
 {50, 0, 0, 0}              //offest        9
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
  Wire.begin(4);
  Wire.onReceive(receiveEvent);
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
  
  if(ret == 1)
  {

    // Ready to draw
    armReady = true;

    // Wait for information from the Raspberry Pi
    while(drawingReceived == false)
    {
      delay(250);
    }

    // Drawing
    armReady = false;

    // For each shape received from the Pi
    for(int i = 0; i < (sizeof(shapes) / sizeof(shapes[0])); i++) 
    {
      switch (shapes[i]) {
        case 3: //Triangle
          {
            Serial.println("Triangle");
            //int ret3 = topLeft();
            //draw(diffArray[0]);
            //draw(diffArray[1]);
            //draw(diffArray[2]);
          }
          break;
        case 4: // Square
          {
            Serial.println("Square");
            //int ret3 = topLeft();
            //draw(diffArray[6]);
            //draw(diffArray[3]);
            //draw(diffArray[4]);
            //draw(diffArray[5]);

            // The following code is what currently works to draw a rectangle
            int ret3 = topLeft();
            delay(1500);

            // Draw first vertical line
            int ret2 =  draw(diffArray[6]);

            // Draw horizontal line
            baseMotor.run(BACKWARD);
            delay(100);

            // Draw horizontal line
            ret = resetPos();
            wristMotor.run(BACKWARD);
            delay(50);
            ret3 = topLeft();
            baseMotor.run(BACKWARD);
            delay(1000);
            baseMotor.run(RELEASE);

            // Draw vertical line
            ret2 =  draw(diffArray[6]);
          }
          break;
        case 5: // Pentagon
          {
            Serial.println("Pentagon");
            //int ret3 = topLeft();
            //draw(diffArray[0]);
            //draw(diffArray[1]);
            //draw(diffArray[3]);
            //draw(diffArray[4]);
            //draw(diffArray[5]);
          }
          break;
        case 6: // Hexagon
          {
            Serial.println("Hexagon");
            //int ret3 = topLeft();
            //draw(diffArray[0]);
            //draw(diffArray[1]);
            //draw(diffArray[3]);
            //draw(diffArray[7]);
            //draw(diffArray[8]);
            //draw(diffArray[5]);
          }
          break;
        case 7: // Circle
          {
            Serial.println("Circle");
            //int ret3 = topLeft();
            //drawCircle();
          }
          break;
        case 8: // Circle
          {
            Serial.println("Circle");
            //int ret3 = topLeft();
            //drawCircle();
          }
          break;
        case 9: // Circle
          {
            Serial.println("Circle");
            //int ret3 = topLeft();
            //drawCircle();
          }
          break;
        default:
          Serial.println("No more shapes!");
          break;
      }

      // Reset position for next shape
      ret = resetPos();
      
    }
  }

  // Release motors
  wristMotor.run(RELEASE);
  baseMotor.run(RELEASE);
  elbowMotor.run(RELEASE);
  shoulderMotor.run(RELEASE);

  drawingReceived = false;
  
  delay(2000);
  
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
    
    // Base motor reset, reset value range is 315 - 320
    if(!inRangeBase)
    {
      while(sensorValueA0 < 315 || sensorValueA0 > 320)
      {
        if(sensorValueA0 < 315)
        {
          baseMotor.run(BACKWARD);
          sensorValueA0 = analogRead(A0);
          delay(10);
        }
        else if(sensorValueA0 > 320)
        {
          baseMotor.run(FORWARD);
          sensorValueA0 = analogRead(A0);
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
      // Shoulder motor reset, reset value range is 400 - 405
      while(sensorValueA1 < 400 || sensorValueA1 > 405)
      {
        if(sensorValueA1 < 400)
        {
          shoulderMotor.run(FORWARD);
          sensorValueA1 = analogRead(A1);
          delay(50);
        }
        else if(sensorValueA1 > 405)
        {
          shoulderMotor.run(BACKWARD);
          sensorValueA1 = analogRead(A1);
          delay(50);
        }
      }
    
      // Stop shoulder motor
      shoulderMotor.run(RELEASE);
      inRangeShoulder = true;
    }

    if(!inRangeElbow)
    {
      // Elbow motor reset, reset value range is 415 - 420
      while(sensorValueA2 < 415 || sensorValueA2 > 420)
      {
        if(sensorValueA2 < 415)
        {
          elbowMotor.run(FORWARD);
          sensorValueA2 = analogRead(A2);
          delay(10);
        }
        else if(sensorValueA2 > 420)
        {
          elbowMotor.run(BACKWARD);
          sensorValueA2 = analogRead(A2);
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
          delay(10);
        }
        else if(sensorValueA3 > 515)
        {
          wristMotor.run(BACKWARD);
          sensorValueA3 = analogRead(A3);
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
// Same as resetPos but with different values
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
    
    // Base motor reset, reset value range is 278 - 283
    if(!inRangeBase)
    {
      while(sensorValueA0 < 278 || sensorValueA0 > 283)
      {
        if(sensorValueA0 < 278)
        {
          baseMotor.run(BACKWARD);
          sensorValueA0 = analogRead(A0);
          delay(10);
        }
        else if(sensorValueA0 > 283)
        {
          baseMotor.run(FORWARD);
          sensorValueA0 = analogRead(A0);
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
      // Wrist motor reset, reset value range is 510 - 515
      while(sensorValueA3 < 510 || sensorValueA3 > 515)
      {
        if(sensorValueA3 < 510)
        {
          wristMotor.run(FORWARD);
          sensorValueA3 = analogRead(A3);
          delay(10);
        }
        else if(sensorValueA3 > 515)
        {
          wristMotor.run(BACKWARD);
          sensorValueA3 = analogRead(A3);
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
      // Elbow motor reset, reset value range is 150 - 155
      while(sensorValueA2 < 150 || sensorValueA2 > 155)
      {
        if(sensorValueA2 < 150)
        {
          elbowMotor.run(FORWARD);
          sensorValueA2 = analogRead(A2);
          delay(10);
        }
        else if(sensorValueA2 > 155)
        {
          elbowMotor.run(BACKWARD);
          sensorValueA2 = analogRead(A2);
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
      // Shoulder motor reset, reset value range is 923 - 928
      while(sensorValueA1 < 923 || sensorValueA1 > 928)
      {
        if(sensorValueA1 < 923)
        {
          shoulderMotor.run(FORWARD);
          sensorValueA1 = analogRead(A1);
          delay(10);
        }
        else if(sensorValueA1 > 928)
        {
          shoulderMotor.run(BACKWARD);
          sensorValueA1 = analogRead(A1);
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


// Function that executes whenever data is received from master
// This function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  int index = 0;
  int shapesNum = 0;
  shapes[0] = 0;
  shapes[1] = 0;
  shapes[2] = 0;
  
  while(0 < Wire.available() && index < 15)
  {
    // Receive byte as a character
    char c = Wire.read(); 

    inShapes[index] = c;
    index++;
    // Keep the string NULL terminated
    inShapes[index] = '\0'; 
  }

  // Parse through shapes data
  shapesNum = atoi(inShapes);
  index = 0;
  
  while(shapesNum > 0 && index < 3)
  {
    shapes[index] = shapesNum % 10;
    shapesNum = shapesNum / 10;
    index++;
  }

  // Set drawing flag to true
  drawingReceived = true;

}

// Send armReady signal to Raspberry Pi
void requestEvent()
{
  if (armReady == true) {
    Wire.write ("A", 1);
  }
  else {
    // Do nothing
  }
}


// Draw from current position to another given point
// Second point location is defined by entries in diffArray
int draw(int diffArray[4]){

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
  int shoulderPoint = sensorValueA1 - diffArray[1] + 10;
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
  
  while(!(inRangeShoulder && inRangeElbow && inRangeWrist))
  {

        if(sensorValueA1 < (shoulderPoint - 3) && !inRangeShoulder)
        {
          shoulderMotor.run(FORWARD);
          sensorValueA1 = analogRead(A1);
        }
        else if(sensorValueA1 > (shoulderPoint + 3) && !inRangeShoulder)
        {
          shoulderMotor.run(BACKWARD);
          sensorValueA1 = analogRead(A1);
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
        }
        else if(sensorValueA2 > (elbowPoint + 3) && !inRangeElbow)
        {
          elbowMotor.run(BACKWARD);
          sensorValueA2 = analogRead(A2);
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
        }
        else if(sensorValueA3 > (wristPoint + 3) && !inRangeWrist)
        {
          wristMotor.run(BACKWARD);
          sensorValueA3 = analogRead(A3);
        }
        else
        {
          // Stop wrist motor
          Serial.println("Wrist motor starting position reached");
          wristMotor.run(RELEASE);
          inRangeWrist = true;
        }

        // 50ms delay for steadier potentiometer readings
        delay(50);
    
  }

  // Print sensor values
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
