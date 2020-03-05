#coding:utf-8
import sys
#sys.path.remove('/opt/ros/kinetic/lib/python2.7/dist-packages')
import cv2

img_raw = cv2.imread('1.png')

size = (img_raw.shape[1],img_raw.shape[0])
print size

fourcc = cv2.VideoWriter_fourcc(*'XVID')

out = cv2.VideoWriter('output.avi', fourcc, 20.0, size)

out.write(img_raw)

ROI = img_raw[84:184,232:467].copy()
ROI2 = img_raw[151:416,750:769]
img_raw[84:184,232:467] = 255
img = img_raw.copy()

cv2.namedWindow('result',1)

for col in range(126,761,4):
	img[89:424,col:761] = 255
	img[84:184,232:467] = ROI
	img[151:416,750:769] = ROI2
	cv2.imshow("result",img)
	out.write(img)
	img = img_raw.copy()
	cv2.waitKey(1)

	cv2.waitKey(1)
cv2.waitKey(0)
out.release()
cv2.destroyAllWindows()


