# -*-coding=utf-8-*-

import numpy as np
import math
import matplotlib.pyplot as plt

A = 20
B = 40
C = 60
D = 70
E = 80
def fun(x):
	y = 0
	if(x<A):
		y = 0
	elif(x<B):
		y = math.pow((A+B)/2-x,1.0/3)+5
	return y

#def main():
#	X = np.linspace(0,30,400)
#	Y = [fun(x) for x in X]
#	print(X)
#	print(Y)


#	plt.plot(X,Y)
#	try:
#		plt.show()
#	except KeyboardInterrupt:
#		pass

def main():
	path = []
	with open("segment_path.txt","r") as f:
		while True:
			line = f.readline()
			if len(line) == 0:
				break
			x,y = line.split()
			x = float(x)
			y = float(y)
			if(x > 100):
				x = x -20
			x = x - 40
			path.append([x,y])
	#line
	for i in range(208):
		x = 90 + 0.1*i
		y = 0
		path.append([x,y])
		
	#cycle
	R = 5
	for i in range(1,2*R*10):
		y = i*0.1
		x = math.sqrt(R*R-(y-R)*(y-R)) + 110
		path.append([x,y])
	
	#line
	for i in range(500):
		x = 110 - 0.1*i + 0.8
		y = 2*R
		path.append([x,y])
	
	with open("full_path.txt",'w') as f:
		for data in path:
			f.write('%f\t%f\n' %(data[0],data[1]))
	
	path = np.array(path)
	plt.plot(path[:,0],path[:,1],'r-*')
	plt.axis('equal')
	plt.show( )


main()

