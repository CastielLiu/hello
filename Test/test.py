#!/usr/bin/python  
import roslib
import sys
import rospy
import cv2
import math
import os
import numpy as np
import datetime


class Test():
	def __init__(self):
		self.a,self.b = self.fun()
		
	def fun(self):
		return 1,2

test = Test()
print test.a,test.b

