import serial
import struct
import time
import sys 
import os

import pi_to_motor
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "..")))
import voice_nav.speak as vc

ser = serial.Serial('/dev/ttyUSB1', 115200, timeout=1)
num_sensor = 6

# Distance Thresholds
stop_distance = 30
reverse_threshhold = 20
avoidance_distance = 10
command_set = {"direction": None,"distance": 0, "time": 0,"angle": 0,"speed": 0}

def move_forward():
    command_set = {"direction": "forward","distance": 0, "time": 0,"angle": 0,"speed": 2}
    pi_to_motor.send_to_esp(command_set)
    #voice_nav.speak_text("Moving Forward.")

def stop_motor():
    #voice_nav.speak_text("Collision detected... Stopping immediately.")
    command_set = {"direction": "forward","distance": 0, "time": 0,"angle": 0,"speed": 0}
    pi_to_motor.send_to_esp(command_set)

def turn_right():    
    command_set = {"direction": "right","distance": 0, "time": 0,"angle": 45,"speed": 2}
    pi_to_motor.send_to_esp(command_set)
    #voice_nav.speak_text("Turning Left.")

def turn_left():    
    command_set = {"direction": "left","distance": 0, "time": 0,"angle": 45,"speed": 2}
    pi_to_motor.send_to_esp(command_set)
    #voice_nav.speak_text("Turning Left.")

def correct_path(direction):
    if direction == "right":
        turn_left()
    else:
        turn_right()


while True:
    if ser.in_waiting >= num_sensor * 4:
        data = ser.read(num_sensor * 4)
        distance = list(struct.unpack('I' * num_sensor, data))
        print("Distances: ", distance)

        front_left = distance[0]
        front_center = distance[1]
        front_right = distance[2]
        side_left = distance[3]
        side_right = distance[4]
        rear = distance[5]
        front_bottom = distance[6]

        if front_bottom < 20:
            stop_motor()
            vc.speak_text("Path ends here. Stopping SWheels. Waiting for further instruction.")

        if front_center < stop_distance:
            stop_motor()
            vc.speak_text("Obstacle detected. Taking measures. Please stay calm...!")

            if front_right > avoidance_distance:
                turn_right()
                # time.sleep(1)
                #move_forward()
                if side_left > stop_distance:
                    #time.sleep(1)
                    correct_path("right")
            if front_left > avoidance_distance:
                turn_left()
                # time.sleep(1)
                # move_forward()
                if side_right > stop_distance:
                    #time.sleep(1)
                    correct_path("left")

