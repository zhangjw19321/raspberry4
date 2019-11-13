import os
os.chdir("/home/pi/Desktop/SenseStormFaceRecognition")
import sys
sys.path.append("/home/pi/Desktop/SenseStormFaceRecognition")
from FaceAPI_dlib import *
import sys
import cv2
from time import sleep
from legoCore import LEGO_Serial
LS = LEGO_Serial()
def open_door(speed = -40, r_time = 0.5):
    LS.C_motor_timed(speed,r_time)
    sleep(r_time)

def close_door(speed = 40,r_time = 0.3):
    LS.C_motor_timed(speed,r_time)
    sleep(r_time)

# part two: use ultra to control
def get_distance():
    dis = float(LS.getDistance())
    distance = ("%.2f"%dis)
    res = float(distance)
    return dis

faceAPI = FaceAPI()

def get_image():
    cap = cv2.VideoCapture(0)
    ret, frame = cap.read()
    cap.release()
    if ret:
        return frame
    else:
        print("Read Camera Error, Please check the camera")
        return None

def detect(image):
    if (image is not None):
        flag = faceAPI.detect(image)
        return flag
    else:
        print("No Image")

def feature_extract(image):
    rects = faceAPI.detectRects(image)
    feature = faceAPI.extractFeature(image,rects[0])
    return feature

def compare(feature1,feature2):
    flag = faceAPI.compare(feature1,feature2)
    return flag

def face_recognition(feature):
    cap = cv2.VideoCapture(0)
    i = 1
    while True:
        i = i + 1
        ret, frame = cap.read()
        if i % 1 == 0:
            flag = detect(frame)
            if flag:
                rects = faceAPI.detectRects(frame)
                input_feature = faceAPI.extractFeature(frame,rects[0])
                flag1 = faceAPI.compare(feature,input_feature)
                if flag1:
                    cap.release()
                    return True
                else:
                    cap.release()
                    return False 
            else:
                print("No face detected")