#coding:utf-8
import sys
#sys.path.remove('/opt/ros/kinetic/lib/python2.7/dist-packages')
import cv2
 
#cap = cv2.VideoCapture("/home/wendao/Desktop/mda-ja7kdq59gj4w1pd3.mp4")

#size = (1052-177,657-133)
#fourcc = cv2.VideoWriter_fourcc(*'XVID')
#out = cv2.VideoWriter('output.avi', fourcc, 20.0, size)
#print(size)
#start = False
#key = 0
#while(1):
#    ret, frame = cap.read()
#    frame = frame[133:657,177:1052]
#    if ret==True:
#    	if(start==False and ord('s') == key):
#    		start = True
#    	if start:
#        	out.write(frame)
#        cv2.imshow('frame',frame)
#        key = cv2.waitKey(0)
#        if ord('q') == key:
#			cap.release()
#			out.release()
#			cv2.destroyAllWindows()
#			break

def main():
	cap = cv2.VideoCapture("/home/wendao/Desktop/tractor.mp4")

	totalFrameNumber = cap.get(cv2.CAP_PROP_FRAME_COUNT)
	rate = cap.get(cv2.CAP_PROP_FPS);

	print('frames: %f \t frame rate: %f' %(totalFrameNumber,rate))

	size = (int(cap.get(cv2.CAP_PROP_FRAME_WIDTH)), int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT)))
	#fourcc = cv2.VideoWriter_fourcc(*'WMV1')

	fourcc = cv2.VideoWriter_fourcc(*'XVID')
	out = cv2.VideoWriter('1.avi', fourcc, rate, size)
	print(size)
	start = False
	key = 0
	while(1):
		key = cv2.waitKey(0)
		if(start==False and ord('s') == key):
			start = True
		elif ord('q') == key:
			cap.release()
			out.release()
			cv2.destroyAllWindows()
			break
		
		ret, frame = cap.read()
	
		current_frame = cap.get(cv2.CAP_PROP_POS_FRAMES)
		print ('current_frame: %d' %current_frame)
	
		cap.set(cv2.CAP_PROP_POS_FRAMES,current_frame+2)
	
		if ret==True:
			if start:
				out.write(frame)
			cv2.imshow('frame',frame)
			print('current_time: %f' %(cap.get(cv2.CAP_PROP_POS_MSEC)/1000))
		else:
			break
	

if __name__ == '__main__':
	try:
		main()
	except KeyboardInterrupt:
		exit()













