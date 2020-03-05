# -*-coding=utf-8-*-

import numpy as np
import math
import matplotlib.pyplot as plt
import time
import commands

Lfc = 8  # 前视距离
Speed = 13.0


class VehicleModel:
	def __init__(self)
		self.x = 0
		self.y = 0
		self.yaw = 0
		self.roadWheelAngle = 0.0
		self.steeringSpeed = 3.0 #deg/s
		self.speed = 0.0
		self.accel = 2.0 #m/s2
		self.wheelBase = 2.15 #m
		self.maxRoadWheelAngle = 25.0 #deg
		self.maxSideAceel = 1.8 #m/s2
		
		self.dt = 0.01 #s
		
	def setStepLength(self, size):
		self.dt = size
		
	def setSteeringSpeed(self,s):
		self.steeringSpeed = s
	
	def setMaxSideAceel(self,val):
		self.maxSideAceel = val
	
	def setInitialState(self,x,y,yaw):
		self.x = x
		self.y = y
		self.yaw = yaw
	
	def setMaxRoadWheelAngle(self, val):
		self.maxRoadWheelAngle = val
	
	def setWheelBase(self, val):
		self.wheelBase = val
		
	def setSteeringSpeed(self, val):
		self.steeringSpeed = val
		
	def setAccelerate(self, val):
		self.accel = val
	
	def update(self,steeringDirection=0.0, accelDirection=0.0):
		self.x = self.x+self.speed*math.cos(self.yaw)*self.dt
		self.y = self.y+self.speed * math.sin(self.yaw) * self.dt
		if(self.roadWheelAngle != 0):
			R = self.wheelBase/math.tan(self.roadWheelAngle*math.pi/180.0)
			self.yaw = self.yaw + self.speed / R * self.dt
		self.roadWheelAngle = self.roadWheelAngle+self.steeringSpeed* steeringDirection * self.dt
		if(self.roadWheelAngle > self.maxRoadWheelAngle):
			self.roadWheelAngle = self.maxRoadWheelAngle
		elif(self.roadWheelAngle < -self.maxRoadWheelAngle):
			self.roadWheelAngle = -self.maxRoadWheelAngle
		
		self.speed = self.speed + self.accelDirection * self.dt


class PathTracking():
	def __init__(self):
		self.vehicleModel = VehicleModel()
		self.maxSpeed = None
		self.pathPoints = []
		self.nearestPointIndex = 0
		self.targetIndex = 0
		self.foresightDistance = 5.0
		self.pathPointsSize = 0
		self.latErrList = []
		self.yawErrList = []
		self.trajectoryList = []
		
	def setVehicleModel(self,model):
		self.vehicleModel = model
		
	def setMaxSpeed(self, speed):
		self.maxSpeed = speed
	
	def loadPathPoints(self,file_name):
		with open(file_name, 'r') as f:
		while True:
			line = f.readline()
			if not line:
				break
			data = line.split() #x,y,curveture
			self.pathPoints.append((data[0],data[1],data[2]))
			self.pathPointsSize = len(self.pathPoints)
		
	
	def initTargetIndex(self):
		minDis2 = float('inf')
		
		for index in range(len(self.pathPoints)):
			dx = self.pathPoints[index][0] - self.vehicleModel.x
			dy = self.pathPoints[index][1] - self.vehicleModel.y
			dis2 = dx*dx + dy*dy
			if(dis2 < minDis2):
				minDis2 = dis2
				self.nearestPointIndex = index
		l2 = self.foresightDistance * self.foresightDistance
		targetIndex = 0
		for index in range(self.nearestPointIndex, self.pathPointsSize):
			dx = self.pathPoints[index][0] - self.vehicleModel.x
			dy = self.pathPoints[index][1] - self.vehicleModel.y
			dis2 = dx*dx + dy*dy
			if(dis2 >= l2):
				self.targetIndex = index
				break
	
	def updateTargetIndex(self):
		l2 = self.foresightDistance * self.foresightDistance
		for index in range(self.targetIndex, self.pathPointsSize):
			dx = self.pathPoints[index][0] - self.vehicleModel.x
			dy = self.pathPoints[index][1] - self.vehicleModel.y
			dis2 = dx*dx + dy*dy
			if(dis2 >= l2):
				self.targetIndex = index
				break
	
	def calculateTrackingError(self):
		minDis2 = float('inf')
		for index in range(self.nearestPointIndex, self.pathPointsSize):
			dx = self.pathPoints[index][0] - self.vehicleModel.x
			dy = self.pathPoints[index][1] - self.vehicleModel.y
			dis2 = dx*dx + dy*dy
			if(dis2 < minDis2):
				minDis2 = dis2
				self.nearestPointIndex = index
				
		#把离当前位置最近的路径点转换到车体坐标系 #利用转换后的坐标判断横向偏差的±
		x,y = self.coordinateTrans(self.pathPoints[self.nearestPointIndex][0], self.pathPoints[self.nearestPointIndex][1], \
									self.vehicleModel.x, self.vehicleModel.y, self.vehicleModel.yaw)
		latErr = y

		if(self.nearestPointIndex==0):#第一个路径点
			pathPointYaw =  math.atan2(self.pathPoints[self.nearestPointIndex+1][1] - self.pathPoints[self.nearestPointIndex][1], \
								self.pathPoints[self.nearestPointIndex+1][0] - self.pathPoints[self.nearestPointIndex][0])#与x轴夹角
		else:
			pathPointYaw =  math.atan2(self.pathPoints[self.nearestPointIndex][1] - self.pathPoints[self.nearestPointIndex-1][1], \
								self.pathPoints[self.nearestPointIndex][0] - self.pathPoints[self.nearestPointIndex-1][0])#与x轴夹角
		yawErr = yaw - pathPointYaw
		latErrList.append(latErr)
		yawErrList.append(yawErr)
		
		return latErr, yawErr

	#x,y is the position brfore transform
	#a,b is the translation and theta is rotation
	#s,t is the position in new coordinate system
	def coordinateTrans(self, x,y, a,b, theta):
		s = (x-a)*math.cos(theta) + (y-b)*math.sin(theta)
		t = -(x-a)*math.sin(theta) + (y-b)*math.cos(theta)
		return s,t
	
	def getTolerateMaxSpeed(self):
		maxCurvture = 0
		for index in range(self.nearestPointIndex, self.targetIndex):
			curveture = abs(self.pathPoints[index][2])
			if(curveture > maxCurvture):
				maxCurvture = curveture
		maxSpeed = math.sqrt()
			
		
	def run(self):
		self.initTargetIndex()
		while self.targetIndex < self.pathPointsSize:
			
			x = self.vehicleModel.x
			y = self.vehicleModel.y
			yaw = self.vehicleModel.yaw
			speed = self.vehicleModel.speed
			
			k1 = 1.8
			k2 = 0.3
			Lc = 5.0
			latErr, yawErr = self.calculateTrackingError()
			self.foresightDistance = k1*speed + k2*abs(latErr) + Lc
			self.updateTargetIndex()
			
			target_x = self.pathPoints[self.targetIndex][0]
			target_y = self.pathPoints[self.targetIndex][1]
			
			theta = math.atan2(target_y - y, target_x - x) - yaw  #fake yaw error
			delta = math.atan(2*self.vehicleModel.wheelBase * math.sin(theta) / self.foresightDistance) * 180.0/math.pi
			
			self.vehicleModel.update(delta, accelDirection)

def main():
	vehicleModel = VehicleModel()
	
	vehicleModel.setInitialState(x=0.0,y=0.0,yaw=0.0)
	vehicleModel.setAccelerate(2.0)
	vehicleModel.setSteeringSpeed(3.0)
	vehicleModel.setWheelBase(2.15)
	vehicleModel.setMaxRoadWheelAngle(25.0)
	vehicleModel.setStepLength(0.03)
	vehicleModel.setMaxSideAceel(1.8)
	tracker = PathTracking()
	tracker.setVehicleModel(vehicleModel)
	tracker.loadPathPoints("path.txt")
	tracker.setMaxSpeed(30.0/3.6)
	tracker.run()
	
	

def main():
	
	plt.figure(0)
	plt.ylim(-5,15)
	plt.plot(cx,cy,'r-')
	# 设置车辆的初始状态 a class
	vehicleState = VehicleState(x=0.0,y=0.0,yaw=0.0,roadWheelAngle=0.0, \
								speed=Speed/3.6,steeringSpeed=5)

	total_len = len(cx) - 1

	x = []
	y = []
	yaw = []

	target_ind = calc_target_index(vehicleState, cx, cy)
	
	latErrList=[]
	yawErrList=[]
	
	cnt = 0

	while total_len > target_ind:

		latErr,yawErr,index = lat_yawErr(vehicleState.x,vehicleState.y,vehicleState.yaw,cx,cy)
		latErrList.append(latErr)
		yawErrList.append(yawErr)
		
		delta, target_ind = pure_pursuit_control(vehicleState, cx, cy, target_ind)#front wheel angle , index ,front distance
		
		steering_direction = 0
		delta_err = delta - vehicleState.roadWheelAngle
		if(delta_err > 0):
			steering_direction = 1
		elif(delta_err < 0):
			steering_direction = -1
		
		if(abs(delta_err) < 2.0):
			vehicleState.setSteeringSpeed(2.0)
		elif(abs(delta_err) < 5.0):
			vehicleState.setSteeringSpeed(5.0)
		else:
			vehicleState.setSteeringSpeed(10.0)
		
		vehicleState.update(steering_direction)

		x.append(vehicleState.x)
		y.append(vehicleState.y)
		yaw.append(vehicleState.yaw)
#		if(vehicleState.x > 120):
#			break;
#		print(vehicleState.x,vehicleState.y,vehicleState.yaw*180.0/math.pi,delta,vehicleState.roadWheelAngle,steering_direction)

		if(cnt%10==0):
			plt.plot(cx[target_ind], cy[target_ind], "go", label="target")
			plt.plot(vehicleState.x, vehicleState.y, ".b", label="trajectory")
		cnt = cnt + 1
		print("%d/%d" %(target_ind,total_len))
		
	out_file_name = 'purepursuit_' + str(Lfc) + '_' + str(Speed) + '.txt'
	with open(out_file_name,'w') as f:
		for i in range(len(x)):
			f.write('%f\t%f\t%f\t%f\n' %(x[i],y[i],latErrList[i],yawErrList[i]))
	
	fig = plt.figure('pure_pursuit')
	axis1 = fig.add_subplot(311)
	axis1.plot(cx, cy, "-r", label="ideal path")
	axis1.plot(x,y,'b')
	
	ax2 = fig.add_subplot(312)
	ax2.plot(x,latErrList,'r-',label='latErr',linewidth=0.5)
	ax2.grid()
	
	ax2.plot(x,[0]*len(x),'k-')
	ax2.legend(loc=4)
	latErrList = [math.fabs(x_) for x_ in latErrList]
	LatMainErr = reduce(lambda x_,y_:x_+y_ ,latErrList)*1.0/len(latErrList)
	ax2.text(5,0,'LatMainErr:%.2fcm'%(LatMainErr*100))
	
	ax3 =fig.add_subplot(313)
	ax3.plot(x,[x_*180.0/math.pi for x_ in yawErrList],'r-')
	ax3.plot(x,[0]*len(x),'k-')
	
	
	
	
	
	plt.savefig('pure_pursuit.pdf')
	plt.show()
	

if __name__ == '__main__':
		main()
