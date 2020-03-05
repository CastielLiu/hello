# -*-coding=utf-8-*-

import numpy as np
import math
import matplotlib.pyplot as plt
import time
import commands

k = 0.1  # 前视距离系数
Lfc = 0.8  # 前视距离
Kp = 1.0  # 速度P控制器系数
dt = 0.01  # 时间间隔，单位：s
L = 1.5  # 车辆轴距，单位：m


front_wheel_angle=0.0


class VehicleState:

    def __init__(self, x=0.0, y=0.0, yaw=0.0, v=0.0):
        self.x = x
        self.y = y
        self.yaw = yaw
        self.v = v

# a acceleration 
# delta  front wheel corner
def update(state, a, delta):

    state.x = state.x + state.v * math.cos(state.yaw) * dt
    state.y = state.y + state.v * math.sin(state.yaw) * dt
    state.yaw = state.yaw + state.v / L * math.tan(delta) * dt
    state.v = state.v + a * dt

    return state
def PControl(target, current):
    a = Kp * (target - current)

    return a


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

    if state.v < 0:  # back
        alpha = math.pi - alpha

    #Lf = k * state.v + Lfc
    global Lfc
    Lf = Lfc
    delta = math.atan2(2.0 * L * math.sin(alpha) / Lf, 1.0)

    return delta, ind,Lf

    
def calc_target_index(state, cx, cy):
    # 搜索最临近的路点
    dx = [state.x - icx for icx in cx]
    dy = [state.y - icy for icy in cy]
    d = [abs(math.sqrt(idx ** 2 + idy ** 2)) for (idx, idy) in zip(dx, dy)]
    ind = d.index(min(d)) # find the index of minimum value in array d
    L = 0.0

# calculate the foresight distance  velocity is ralated
    Lf = k * state.v + Lfc

# calulate the tracking point 
#(ensure the distance between current point and target point greater than the foresight distance)
    while Lf>L and (ind)<len(cx):
    	dx=state.x-cx[ind]
    	dy=state.y-cy[ind]
    	L=math.sqrt(dx ** 2 + dy ** 2)
    	ind+=1
    
    #while Lf > L and (ind + 1) < len(cx):
     #   dx = cx[ind + 1] - cx[ind]
      #  dy = cy[ind + 1] - cy[ind]
       # L += math.sqrt(dx ** 2 + dy ** 2)
        #ind += 1

    return ind-1
    
    
def main():
	#  设置目标路点
	cx = np.arange(0, 50, 1)
	cy = [math.sin(ix / 5.0) * ix / 2.0 for ix in cx]

	target_speed = 40.0 / 3.6  # [m/s]

	T = 100.0  # 最大模拟时间

	# 设置车辆的初始状态 a class
	state = VehicleState(x=-0.0, y=-3.0, yaw=0.0, v=0.0)

	lastIndex = len(cx) - 1
	time = 0.0
	x = [state.x]
	y = [state.y]
	yaw = [state.yaw]
	v = [state.v]
	t = [0.0]
	target_ind = calc_target_index(state, cx, cy)

	plt.plot(cx, cy, ".r", label="ideal path")
	plt.axis("equal")
	plt.grid(True)
	plt.xlabel('m')
	plt.ylabel('m')
    
	first_time_flag = 1
	plt.pause(5)
    
	while T >= time and lastIndex > target_ind:

		ai = PControl(target_speed, state.v) #calculate the acceleration
		di, target_ind,Lf = pure_pursuit_control(state, cx, cy, target_ind)#front wheel angle , index ,front distance
		state = update(state, ai, di)

		time = time + dt

		x.append(state.x)
		y.append(state.y)
		yaw.append(state.yaw)
		v.append(state.v)
		t.append(time)

        #plt.cla()
		plt.plot(cx[target_ind], cy[target_ind], "go", label="target")
		plt.plot(state.x, state.y, ".b", label="trajectory")
		
        
      #  plt.legend(loc = 2)
		plt.title("  foresight distance " + str(Lf)[:4]+"m")
		plt.pause(0.001)
        
		if(first_time_flag==1):
			first_time_flag =0
			plt.legend(loc=2)
	plt.pause(5)
	plt.savefig("a.pdf")
	plt.figure(2)
	global Lfc
	plt.title("foresight distance " + str(Lfc)[:4]+"m")
	plt.axis("equal")
	plt.grid(True)
	plt.xlabel('m')
	plt.ylabel('m')
	plt.plot(cx,cy,'r-',label="ideal path")
	plt.plot(x,y,'b-',label="trajectory")
	plt.legend(loc=2)
	plt.pause(1)
	plt.savefig("b.pdf")
	

if __name__ == '__main__':
		main()
