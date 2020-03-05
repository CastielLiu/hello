#!/usr/bin/python  
import roslib
import sys
import rospy
import cv2
import math
import os
import numpy as np



image = cv2.imread('lane.jpg')

#print(image.shape)

image = cv2.resize(image,(640,480))

image = image[276:480, : ]  #intresting area

img_gray = cv2.cvtColor(image,cv2.COLOR_RGB2GRAY)

blur = cv2.GaussianBlur(img_gray, (3, 3), 0)

cv2.imshow('blur',blur)

#canny = cv2.Canny(blur,100,200)
canny = cv2.Canny(blur,120,300)

kernel = np.ones((2,2),np.uint8)

dilation = cv2.dilate(canny,kernel,iterations = 2)

cv2.imshow('image',image)
cv2.imshow('gray',img_gray)
cv2.imshow('canny',canny)
cv2.imshow('dilation',dilation)

cv2.waitKey()

