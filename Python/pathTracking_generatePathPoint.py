# -*-coding=utf-8-*-

import numpy as np
import math
import matplotlib.pyplot as plt
import time
import commands

dt = 0.01  #step length

class VehicleState:
	def __init__(self, x=0.0, y=0.0, yaw=0.0,roadWheelAngle=0.0, \
				steeringSpeed=0.0, speed=0.0,wheel_base=2.0, \
				maxRoadWheelAngle=30.0/180.0*math.pi):
		self.x = x
		self.y = y
		self.yaw = yaw
		self.roadWheelAngle = roadWheelAngle
		self.steeringSpeed = steeringSpeed
		self.speed = speed
		self.last_roadWheelAngle = roadWheelAngle
		self.wheel_base = wheel_base
		self.maxRoadWheelAngle = maxRoadWheelAngle

	def update(self,steeringDirection=0):
		self.x = self.x+self.speed*math.cos(self.yaw)*dt
		self.y = self.y+self.speed * math.sin(self.yaw) * dt
		self.yaw = self.yaw+self.speed/self.wheel_base * math.tan(self.roadWheelAngle)*dt
		self.roadWheelAngle = self.roadWheelAngle+self.steeringSpeed* steeringDirection*dt
		if(self.roadWheelAngle > self.maxRoadWheelAngle):
			self.roadWheelAngle = self.maxRoadWheelAngle
		elif(self.roadWheelAngle < -self.maxRoadWheelAngle):
			self.roadWheelAngle = -self.maxRoadWheelAngle
		
def main():
	
	vehicleState = VehicleState(x=0.0,y=0.0,yaw=0.0,roadWheelAngle=0.0, \
								steeringSpeed=1.0,speed=10.0)
	
	plt.figure('test')
	
	plt.axis('equal')
	#plt.xlim(0,10)
	#plt.ylim(0,10)
	
	steeringDir = 0
	count = 0
	xList = []
	yList = []
	radiusList=[]
	roadWheelAngleList =[]
	
	while True:
		count = count+1
		if(count==50):
			steeringDir = 1
		elif(count==80):
			steeringDir = -1
		elif(count==120):
			steeringDir = 0
		elif(count==300):
			break
			
		vehicleState.update(steeringDir)
		
		xList.append(vehicleState.x)
		yList.append(vehicleState.y)
		if(vehicleState.roadWheelAngle==0):
			radius = float('inf')
		else:
			radius = vehicleState.wheel_base/math.tan(vehicleState.roadWheelAngle)
		radiusList.append(radius)
		roadWheelAngleList.append(vehicleState.roadWheelAngle)
		
		plt.plot(vehicleState.x,vehicleState.y,'r.')
		#plt.pause(0.001)
		print(vehicleState.roadWheelAngle*180/math.pi)
	plt.show()	
	with open('path.txt','w') as f:
		for i in range(len(xList)):
			f.write('%.2f\t%.2f\t%.2f\t%.2f\n' %(xList[i],yList[i],roadWheelAngleList[i],radiusList[i]))
			
if __name__ == '__main__':
	main()
