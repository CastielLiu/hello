#!/usr/bin/python  

import math
import numpy as np

def insertSort(array):
	length = len(array)
	for i in range(length)[1:]:
		anchor = array[i]
		j = i-1
		while True:
			if(j < 0):
				break;
			if(anchor < array[j]):
				array[j+1] = array[j]
			else:
				break;
			j = j-1
		array[j+1] = anchor

a = range(10)

a.reverse()

print(a)

#insertSort(a)

a.sort()

print(a)
