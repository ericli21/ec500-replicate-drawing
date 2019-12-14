'''
This script monitors the input of the Raspberry Pi camera module.
It detects the shapes present in the image that it captured.
It communicates using MQTT to send out the information
about the shapes that were detected.
'''

#Imports
import numpy as np
import cv2
from time import sleep
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
import RPi.GPIO as GPIO

#MQTT Server variables
MQTT_SERVER = "172.20.10.2"#"172.20.10.7"
MQTT_PATH1 = "test_channel1" #Raspi sending message to Arduino
MQTT_PATH2 = "test_channel2" #Arduino sending message to Raspi

#GPIO variables
CAPTURE_FGRD_BUTTON = 25 #pin 22
CAPTURE_BGRD_BUTTON = 18 #pin 12

#Initialize other variables used for image capture
fgrd_capture_flag = False

cap = cv2.VideoCapture(0)
ret, background = cap.read()

contours = []

'''
Initialize GPIOs
'''
def init():
    #GPIOs
    GPIO.setmode(GPIO.BCM)
    GPIO.setwarnings(False)
    GPIO.setup(CAPTURE_FGRD_BUTTON, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
    GPIO.setup(CAPTURE_BGRD_BUTTON, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)


'''
Called when we are connected to the MQTT client.
'''
def onConnect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    client.subscribe(MQTT_PATH2)
    
'''
Test function.
'''
def printSomething():
    print("Something")

'''
ISR for foreground capture
'''
def fgrdBtnEventHandler(pin):
    print("In foreground event handler")
    global fgrd_capture_flag
    fgrd_capture_flag = True
    
'''
ISR for background capture
'''
def bgrdBtnEventHandler(pin):
    print("In background event handler")
    global background
    global cap
    ret, background = cap.read()
    background = cv2.cvtColor(background, cv2.COLOR_RGB2GRAY)

'''
When the Raspberry Pi receives a message from the
Arduino (on testchannel2), this function will be called.
'''
def onMessage(client, userdata, msg):
    print("In onMessage")
    if len(contours) != 0: #should evaluate to true if contours is not empty
        message = ''
        for cnt in contours:
            approx = cv2.approxPolyDP(cnt, 0.04*cv2.arcLength(cnt, True), True)
            cv2.drawContours(thresh, [approx], 0, (0, 255, 0), 5)
            if (len(approx) == 3):
                print("Triangle")
            elif (len(approx) == 4):
                print("Square")
            elif (len(approx) == 5):
                print("Pentagon")
            elif (len(approx) == 6):
                print("Hexagon")
            elif (len(approx) >= 7):
                print("Circle")
            if len(approx) > 2 and len(approx) < 10:
                message = message + str(len(approx))
        publish.single(MQTT_PATH1, message, hostname=MQTT_SERVER)
        client.disconnect()
        client.loop_stop()

    else:
        print("done with shapes")
        client.disconnect()
        client.loop_stop()
        # Shouldn't get here

'''
main function.
Captures background subtracted image, determines
the contours present in the image and wait until
we receive a message on testchannel2 to determine
which shapes are present in the image and send that
information to the Arduino.
'''
def main():
    #Initialize GPIOs
    init()
    
    global background
    global thresh
    global contours
    global cap
    global fgrd_capture_flag
    
    #Capture inital background image
    print("Set up background image")
    sleep(10)
    print("Capturing background image")
    ret, background = cap.read()
    background = cv2.cvtColor(background, cv2.COLOR_RGB2GRAY)

    try:
        while True:
            #Enable interrupt for background capture and foreground capture buttons
            GPIO.add_event_detect(CAPTURE_FGRD_BUTTON, GPIO.RISING, fgrdBtnEventHandler, 500)
            GPIO.add_event_detect(CAPTURE_BGRD_BUTTON, GPIO.RISING, bgrdBtnEventHandler, 500)
            while True:
                ret, foreground = cap.read()
                foreground = cv2.cvtColor(foreground, cv2.COLOR_BGR2GRAY)
            
                #get background subtracted image
                background_sub = cv2.absdiff(background, foreground)

                #convert background subtracted image to binary image
                ret, thresh = cv2.threshold(background_sub, 40, 255, cv2.THRESH_BINARY)
            
                cv2.imshow('binary, background subtracted frame', thresh)
            
                cv2.waitKey(25)
                #If the foreground capture button was pressed, exit out of while loop
                if (fgrd_capture_flag):
                    break
            
            #Disable button interrupts during sending process
            GPIO.remove_event_detect(CAPTURE_FGRD_BUTTON)
            GPIO.remove_event_detect(CAPTURE_BGRD_BUTTON)
            
            #Find the contours present in the image
            contours, hierarchy = cv2.findContours(thresh, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
            
            #Enable MQTT communication
            client = mqtt.Client()
            client.on_connect = onConnect
            client.on_message = onMessage

            client.connect(MQTT_SERVER, 1883, 60)
            client.loop_forever()
            
            #Reset foreground capture flag
            fgrd_capture_flag = False

    except KeyboardInterrupt:
        #Close all windows
        print("Exiting")
        cap.release()
        cap.destroyAllWindows()

    
if __name__=="__main__":
    main()
    