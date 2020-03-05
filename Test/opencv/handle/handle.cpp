#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc,char** argv)
{
	Mat srcImage,middleImage, reduceImage;
	
	srcImage = imread("../1.png");
	
	imshow("srcImage",srcImage);
	
	reduceImage = srcImage;
	//resize(srcImage,reduceImage,Size(srcImage.cols/10,srcImage.rows/10),0,0,INTER_LINEAR);
	
	Canny(reduceImage,middleImage,50,200,3);
	
	
	imshow("middleImage",middleImage);
	
	//cout << middleImage <<endl;
	bool flag = imwrite("lunkuo.jpg",middleImage);
	
	if(flag==1)
		cout << "ok" << endl;
	
	int size =7;
	Mat element = getStructuringElement(MORPH_RECT,Size(2*size+1,2*size+1),Point(size,size));
	
	Mat dstImage;
	
	dilate(middleImage,dstImage,element);
	
	imwrite("dstImage.jpg",dstImage);
		
	waitKey(0);
	return 0;
}
