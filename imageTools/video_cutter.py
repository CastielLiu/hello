import sys
import cv2
import imageio

def handle(video,start_time,end_time):
	cap = cv2.VideoCapture(video)
	totalFrameNumber = cap.get(cv2.CAP_PROP_FRAME_COUNT)
	frame_rate = cap.get(cv2.CAP_PROP_FPS)
	frame_cnt = cap.get(cv2.CAP_PROP_FRAME_COUNT)
	total_time = frame_cnt/frame_rate
	if(end_time > total_time):
		end_time = total_time
#	print('total_time: %f' %total_time)
#	size = (int(cap.get(cv2.CAP_PROP_FRAME_WIDTH)), int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT)))
#	print(size[0],size[1])
	
	imageBuf = []
	cap.set(cv2.CAP_PROP_POS_MSEC,start_time*1000)
	while True:
		time = cap.get(cv2.CAP_PROP_POS_MSEC)
		if(time >= end_time*1000):
			break
		result, img = cap.read()
		if(not result):
			break
		imageBuf.append(cv2.cvtColor(img, cv2.COLOR_BGR2RGB))
	prefix , _ = video.split('.')
	outName = prefix+'.gif'
	print("processing ...")
	gif=imageio.mimsave(outName,imageBuf,'GIF',duration=1.0/frame_rate)
	print("process ok...")

def main(argv):
	if(len(argv)!=4):
		print("please input video, start time, end time")
		exit()
	handle(argv[1],float(argv[2]),float(argv[3]))
	
if __name__ == "__main__":
	main(sys.argv)
