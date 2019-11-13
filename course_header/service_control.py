import serial
import time
import threading
from time import sleep

def face_recognition_start():
    import os
    os.system("sh /home/pi/SenseStorm/service/face_start.sh &")

def face_recognition_stop():
    import os
    os.system("sh /home/pi/SenseStorm/service/face_stop.sh")

def hand_recognition_start():
    import os
    os.system("sh /home/pi/SenseStorm/service/hand_start.sh &")

def hand_recognition_stop():
    import os
    os.system("sh /home/pi/SenseStorm/service/hand_stop.sh &")

def body_recognition_start():
    import os
    os.system("sh /home/pi/SenseStorm/service/body_start.sh &")

def body_recognition_stop():
    import os
    os.system("sh /home/pi/SenseStorm/service/body_stop.sh &")


