# -*-coding=utf-8-*-

import os

'''
for i in range(101):
	p1 = 0.3+0.01*i
	os.system('python pathTracking.py %.2f -1.6' %(p1))
	print('%d ok\r\n' %(i))
'''

for i in range(101):
	p2 = -2.0 + 0.01*i
	os.system('python pathTracking.py 0.95 %.2f ' %(p2))
	print('%d ok\r\n' %(i))
