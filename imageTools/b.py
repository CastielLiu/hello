#coding:utf-8
import sys
import cv2
import numpy as np

img_raw = cv2.imread('2.png')

gray=cv2.cvtColor(img_raw,cv2.COLOR_BGR2GRAY)

median = cv2.medianBlur(gray,5)
cv2.imshow("median",median)

cv2.imshow("gray",gray)

ret, binary = cv2.threshold(median, 150, 255, cv2.THRESH_BINARY)
kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (1, 1))
binary = cv2.dilate(binary, kernel)

cv2.imshow("binary",binary)
cv2.namedWindow('img_raw',0)
#cv2.imshow("img_raw",img_raw)
#cv2.imshow("erosion",erosion)


cv2.namedWindow('img_raw',0)
cv2.namedWindow('canny',0)

canny = cv2.Canny(img_raw, 200, 500)
cv2.imshow("canny",canny)
cv2.imshow("img_raw",img_raw)
cv2.waitKey(0)
