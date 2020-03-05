#!/usr/bin/python  
import roslib
import sys
import rospy
import cv2
import math
import os
import numpy as np



image = np.zeros((480,640,1), np.uint8)

cv2.line(image,(0,0),(640,480),(255,255,255),5)

cv2.imshow('image',image)


cv2.waitKey()

