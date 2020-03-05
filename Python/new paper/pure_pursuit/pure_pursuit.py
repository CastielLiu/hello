# -*-coding=utf-8-*-

import numpy as np
import math
import matplotlib.pyplot as plt
import time
import commands

Lfc = 8  # 前视距离
dt = 0.03  # 时间间隔，单位：s
Speed = 13.0



class VehicleState:
	def __init__(self, x=0.0, y=0.0, yaw=0.0,roadWheelAngle=0.0, \
				 speed=0.0, steeringSpeed=3.0, wheel_base=2.15, \
				maxRoadWheelAngle=25.0):
		self.x = x
		self.y = y
		self.yaw = yaw
		self.roadWheelAngle = roadWheelAngle
		self.steeringSpeed = steeringSpeed
		self.speed = speed
		self.last_roadWheelAngle = roadWheelAngle
		self.wheel_base = wheel_base
		self.maxRoadWheelAngle = maxRoadWheelAngle
	def setSteeringSpeed(self,s):
		self.steeringSpeed = s
	
	def update(self,steeringDirection):
		self.x = self.x+self.speed*math.cos(self.yaw)*dt
		self.y = self.y+self.speed * math.sin(self.yaw) * dt
		if(self.roadWheelAngle != 0):
			R = self.wheel_base/math.tan(self.roadWheelAngle*math.pi/180.0)
			self.yaw = self.yaw + self.speed / R * dt
		self.roadWheelAngle = self.roadWheelAngle+self.steeringSpeed* steeringDirection*dt
		if(self.roadWheelAngle > self.maxRoadWheelAngle):
			self.roadWheelAngle = self.maxRoadWheelAngle
		elif(self.roadWheelAngle < -self.maxRoadWheelAngle):
			self.roadWheelAngle = -self.maxRoadWheelAngle

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
	
def coordinateTrans(x,y,x0=0.0,y0=0.0,theta0=0.0):
	X = (x-x0)*math.cos(theta0) + (y-y0)*math.sin(theta0)
	Y = (x-x0)*(-math.sin(theta0))+(y-y0)*math.cos(theta0)
	return X,Y

def pure_pursuit_control(state, cx, cy, pind):

    ind = calc_target_index(state, cx, cy)

    if pind >= ind:
        ind = pind

    if ind < len(cx):
        tx = cx[ind]
        ty = cy[ind]
    else:
        tx = cx[-1]
        ty = cy[-1]
        ind = len(cx) - 1

    alpha = math.atan2(ty - state.y, tx - state.x) - state.yaw

    #Lf = k * state.v + Lfc
    global Lfc
    Lf = Lfc
    delta = math.atan2(2.0 * state.wheel_base * math.sin(alpha) / Lf, 1.0) * 180.0/math.pi

    return delta, ind

    
def calc_target_index(state, cx, cy):
    # 搜索最临近的路点
    dx = [state.x - icx for icx in cx]
    dy = [state.y - icy for icy in cy]
    d = [abs(math.sqrt(idx ** 2 + idy ** 2)) for (idx, idy) in zip(dx, dy)]
    ind = d.index(min(d)) # find the index of minimum value in array d
    L = 0.0


# calulate the tracking point 
#(ensure the distance between current point and target point greater than the foresight distance)
    while Lfc > L and (ind)<len(cx):
    	dx=state.x-cx[ind]
    	dy=state.y-cy[ind]
    	L=math.sqrt(dx ** 2 + dy ** 2)
    	ind+=1

    return ind-1
    
    
def main():
	cx = []
	cy = []
	
	with open('output.txt','r') as f:
		while True:
			line = f.readline()
			if not line:
				break
			x,y = line.split()
			cx.append(float(x))
			cy.append(float(y))
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
