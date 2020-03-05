# -*-coding=utf-8-*-

import numpy as np
import math
import matplotlib.pyplot as plt
import time
import commands
import datetime
import sys

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

def coordinateTrans(x,y,x0=0.0,y0=0.0,theta0=0.0):
	X = (x-x0)*math.cos(theta0) + (y-y0)*math.sin(theta0)
	Y = (x-x0)*(-math.sin(theta0))+(y-y0)*math.cos(theta0)
	return X,Y
	
	
	
def lateralErr(x,y,yaw,path_x,path_y):
	disList = [math.sqrt(math.pow(path_x[i]-x,2)+math.pow(path_y[i]-y,2)) for i in range(len(path_x))]
	
	disMin = min(disList)
	indexOfDisMin = disList.index(disMin)
	
	#把离当前位置最近的路径点转换到车体坐标系
	#利用转换后的坐标判断横向偏差的±
	X,Y = coordinateTrans(path_x[indexOfDisMin],path_y[indexOfDisMin],x,y,yaw)
	#print(X,Y)
	if(Y>0):
		return -disMin #Y大于0表明当前位置在路径右侧，右偏
	elif(Y<0):
		return disMin
	else:
		return 0

def lat_yawErr(x,y,yaw,path_x,path_y):
	disList = [math.sqrt(math.pow(path_x[i]-x,2)+math.pow(path_y[i]-y,2)) for i in range(len(path_x))]
	
	disMin = min(disList)
	indexOfDisMin = disList.index(disMin)
	
	#把离当前位置最近的路径点转换到车体坐标系
	#利用转换后的坐标判断横向偏差的±
	X,Y = coordinateTrans(path_x[indexOfDisMin],path_y[indexOfDisMin],x,y,yaw)

	if(Y<0):
		disMin = -disMin #Y<0表明当前位置在路径左侧，左偏
	elif(Y==0):
		disMin = 0
		
	if(indexOfDisMin==0):#第一个路径点
		theta =  math.atan2((path_y[indexOfDisMin+1] - path_y[indexOfDisMin]), \
				(path_x[indexOfDisMin+1] - path_x[indexOfDisMin]))#与x轴夹角
	elif(indexOfDisMin==len(path_x)-1):#最后一个路径点
		theta = math.atan2((path_y[indexOfDisMin] - path_y[indexOfDisMin-1]), \
				(path_x[indexOfDisMin] - path_x[indexOfDisMin-1]))#与x轴夹角
	else:
		theta = math.atan2((path_y[indexOfDisMin+1] - path_y[indexOfDisMin-1]), \
				(path_x[indexOfDisMin+1] - path_x[indexOfDisMin-1]))#与x轴夹角
	yawErr = yaw - theta
	return disMin,yawErr,indexOfDisMin

def calCurvature(xList,yList,index):
	if(index==0 or index==len(xList)-1):
		print('index error in calCurvature function')
		exit()
	else:
		y_i =(yList[index+1]-yList[index])/(xList[index+1]-xList[index])
		y__i = (yList[index+1]+yList[index-1]-2*yList[index])/((xList[index+1]-xList[index])*(xList[index+1]-xList[index]))
		curvature = math.fabs(y__i)/math.pow(1+y_i*y_i,1.5)
		return curvature

def calTangentAngle(xList,yList,index):
	if(index==0 or index==len(xList)-1):
		print('index error in calTangentAngle function')
		exit()
	else:
		dy = yList[index+1] - yList[index-1]
		dx = xList[index+1] - xList[index-1]
		return math.atan2(dy,dx)

def p2pDistance(x0,y0,x1,y1):
	dx = x1-x0
	dy = y1-y0
	return math.sqrt(dx*dx+dy*dy)

def calCurvature2(xList,yList,index):
	if(index <5 or index >len(xList)-6):
		print('index error in calCurvature2 function')
		exit()
	else:
		arcLength = p2pDistance(xList[index-5],yList[index-5],xList[index],yList[index]) + \
					p2pDistance(xList[index],yList[index],xList[index+5],yList[index+5])
		rotateAngle = calTangentAngle(xList,yList,index+5) - calTangentAngle(xList,yList,index-5)
		print(rotateAngle*180/math.pi)
		return rotateAngle/arcLength

global count 
count = 0
		
def drawTangentLine(plt,x,y,theta):
	if(theta==0):
		return
	global count
	count = count+1
	if(count%5==0):
		y0 = y-x*math.tan(theta)
		xArray = [0,x]
		yArray = [y0,y]
		plt.plot(xArray,yArray,'-*')
		plt.text(-3-count%10,y0,str(count))
		
def main():

	path_x = []
	path_y = []
	t_roadWheelAngleList =[]
	
	with open('path.txt','r') as f:
		while True:
			line = f.readline()
			if not line:
				break
			x,y,roadWheelAngle,_ = line.split()
			path_x.append(float(x))
			path_y.append(float(y))
			t_roadWheelAngleList.append(float(roadWheelAngle))
	
	
	pointNum = len(path_x)
	
	steeringDir=0
	
	for i_ in range(101):
		p1 = 0.95
		p2 = -2.0 + i_*0.01
		latErrList=[]
		yawErrList=[]
		trackingEffect_x=[]
		trackingEffect_y=[]
		vehicleState = VehicleState(x=0.0,y=1.0,yaw=0.0,roadWheelAngle=0.0, \
								steeringSpeed=5.0,speed=10.0)
								
		test_name = 'Loc:('+str(vehicleState.x)+','+str(vehicleState.y)+')'+ \
				' p1:'+str(p1)+' p2:'+str(p2)+ \
				' steerSpeed:'+ str(vehicleState.steeringSpeed)+ \
				' speed:'+str(vehicleState.speed)
								
		while True:
			#print(lateralErr(vehicleState.x,vehicleState.y,vehicleState.yaw,path_x,path_y))
			#print(vehicleState.x,vehicleState.y)
			latErr,yawErr,index = lat_yawErr(vehicleState.x,vehicleState.y,vehicleState.yaw,path_x,path_y)
			latErrList.append(latErr)
			yawErrList.append(yawErr)
			#print(latErr,yawErr*180.0/math.pi)
			t_roadWheelAngle = t_roadWheelAngleList[index+1] + p1*latErr + p2*yawErr
			if(t_roadWheelAngle - vehicleState.roadWheelAngle>0.0):
				steeringDir = 1
			elif(t_roadWheelAngle - vehicleState.roadWheelAngle<0.0):
				steeringDir = -1
			else:
				steeringDir = 0
		
			if(math.fabs(t_roadWheelAngle-vehicleState.roadWheelAngle)<5.0/180*math.pi):
				vehicleState.steerSpeed = 1.0
			else:
				vehicleState.steerSpeed = 5.0
			
			vehicleState.update(steeringDir)
			trackingEffect_x.append(vehicleState.x)
			trackingEffect_y.append(vehicleState.y)
		
			#ax1.plot(vehicleState.x,vehicleState.y,'b.')
			#plt.pause(0.001)
		
			if(pointNum == index+2 ):
				break
		fig = plt.figure('new algorithm')
		ax1 =fig.add_subplot(311)
		ax1.plot(path_x,path_y,'r-',label='path')
		ax1.legend(loc=1)
		ax1.set_title(test_name)
	
		ax1.plot(trackingEffect_x,trackingEffect_y,'b-')
		ax2 = fig.add_subplot(312)
		ax2.plot(trackingEffect_x,latErrList,'r-',label='latErr')
		ax2.plot(trackingEffect_x,[0]*len(trackingEffect_x),'k-')
		ax2.legend(loc=4)
	
		ax3 =fig.add_subplot(313)
		ax3.plot(trackingEffect_x,[x*180.0/math.pi for x in yawErrList],'r-')
		ax3.plot(trackingEffect_x,[0]*len(trackingEffect_x),'k-')

		plt.savefig('figure/'+test_name+'.pdf')
	
		
	
		latErrList = [math.fabs(x) for x in latErrList]
		LatMainErr = reduce(lambda x,y:x+y ,latErrList)*1.0/len(latErrList)
		ax2.text(5,-1,'LatMainErr:%.2fcm'%(LatMainErr*100))
		
		yawErrList = [math.fabs(x) for x in yawErrList]
		YawMainErr = reduce(lambda x,y:x+y ,yawErrList)*1.0/len(yawErrList)
		ax3.text(5,-1,'YawMainErr:%.2fdeg'%(YawMainErr*180.0/math.pi))
		plt.close()
		with open('debug.txt','a') as f:
			f.write('%.2f\t%.2f\t%.2f\t%.2f\t%.2f\r\n' %(p1,p2,vehicleState.speed,LatMainErr,YawMainErr*180.0/math.pi))
		print('%d/100 has finish\r\n' %(i_))
		#plt.show()

			
if __name__ == '__main__':
	main()
