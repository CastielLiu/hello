# -*-coding=utf-8-*-

import numpy as np
import math
import matplotlib.pyplot as plt

 
 

def main():
	path = []
	with open("full_path.txt","r") as f:
		while True:
			line = f.readline()
			if len(line) == 0:
				break
			x,y = line.split()
			x = float(x)
			y = float(y)
			path.append([x,y])
	
	direction = [0] * len(path)
	curvature = [0] * len(path)
	
	interval = 60
	for i in range(interval,len(path)):
		direction[i] = math.atan2(path[i][1] - path[i-interval][1], path[i][0] - path[i-interval][0])
	
	fig1 = plt.figure(1)
	plt.plot(range(len(direction)),direction,'b.')
	
	
	interval = 60
	for i in range(interval,len(path)):
		delta_y = path[i][1] - path[i-interval][1]
		delta_x = path[i][0] - path[i-interval][0]
	
		dis = math.sqrt(delta_y*delta_y + delta_x*delta_x)
		curvature[i] = (direction[i] - direction[i-interval])/dis
	
	interval = 30
	temp_curvature = curvature[:]
	for i in range(interval,len(path)-interval):
		curvature[i] = sum(temp_curvature[i-interval:i+interval])/interval/2
	
	fig2 = plt.figure(2)
	plt.plot(range(len(curvature)),curvature)
	
	with open("path_info.txt",'w') as f:
		for i in range(len(curvature)):
			f.write("%f\t%f\t%f\n" %(path[i][0],path[i][1],curvature[i]))

	fig3 = plt.figure(3)
	path = np.array(path)
	plt.plot(path[:,0],path[:,1],'r.')
	plt.axis('equal')
	plt.show()


main()

