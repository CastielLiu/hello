#include<opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc,char** argv)
{
	Mat srcImage,middleImage, reduceImage;
	
	srcImage = imread("1.png");
	
	imshow("srcImage",srcImage);
	
	Canny(srcImage,middleImage,50,200,3);
	
	resize(middleImage,reduceImage,Size(middleImage.cols/2,middleImage.rows/2),0,0,INTER_LINEAR);
	
	imshow("reduceImage",reduceImage);
	
	bool flag = imwrite("lunkuo.jpg",reduceImage);
	
	if(flag==1)
		cout << "ok" << endl;
		
	waitKey(0);
	return 0;
}
