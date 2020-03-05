#include"open_usart.h"
#include<iostream>
#include<stdint.h>
#include <string>

using namespace std;



typedef struct
{
	double lon;
	double lat;
	
	float yaw;
	
}gpsMsg_t;

gpsMsg_t A = {118.8103950,31.8881787};
gpsMsg_t B = {118.8097060,31.8881449};
gpsMsg_t C = {118.8097519,31.8883161};
gpsMsg_t D = {118.8103677,31.8883353};

gpsMsg_t targetPoints[4] = {
{118.8103950,31.8881787},
{118.8097060,31.8881449},
{118.8097519,31.8883161},
{118.8103677,31.8883353}
};

uint8_t buf[]="$GPRMC,094403.00,A,4004.73794422,N,11614.18999462,E,0.037,5.5,260815,6.5,W,A*35";

string str1 = "$GPRMC,094403.00,A,";
string str2_lat = "";
string str3 = ",N,";
string str4_lon ="";
string str5 = ",E,0.037,";
string str6_yaw = "";
string str7 = ",260815,6.5,W,A*35";

void handle(int fd, const gpsMsg_t &gpsMsg);

int main()
{
	int fd = open_usart("/dev/ttyUSB1");
	set_speed(fd,115200);
	set_Parity(fd,8,1,'N');
	if(fd == -1)
	{
		cout << "open port failed"<<endl;
		return 1;
	}
 
	for(int i=0;i<4;i++){cout << i <<endl;handle(fd,targetPoints[i]);usleep(1500000);}
 

double 	lon_increment, lat_increment;
bool newTargetFlag = true;
int targetIndex=0,lastTargerIndex=3;
int segmentSeq = 0;

const int sectionNum = 1000;

int sectionIndex = 0;

gpsMsg_t tempTargetPoint;

	while(1)
	{
		if(newTargetFlag)
		{
			newTargetFlag = false;
			if(targetIndex==0) lastTargerIndex=3;
			else lastTargerIndex = targetIndex-1;
			lon_increment = (targetPoints[targetIndex].lon - targetPoints[lastTargerIndex].lon)/sectionNum;
			lat_increment = (targetPoints[targetIndex].lat - targetPoints[lastTargerIndex].lat)/sectionNum;
			
			cout << lon_increment <<"\t" << lat_increment <<"\n";
		}
		tempTargetPoint.lon = targetPoints[lastTargerIndex].lon + lon_increment*sectionIndex;
		tempTargetPoint.lat = targetPoints[lastTargerIndex].lat + lat_increment*sectionIndex;
		sectionIndex++;
		
		
		printf("%.7f\t%.7f\t%d\n",tempTargetPoint.lon,tempTargetPoint.lat,targetIndex);
		
		handle(fd,tempTargetPoint);
		
		if(sectionIndex==sectionNum)
		{
			targetIndex++;
			sectionIndex = 0;
			newTargetFlag = true;
		}
		if(targetIndex == 4) targetIndex =0;
		usleep(10000);
		//cout << targetIndex <<endl;
	}
	
}




void handle(int fd, const gpsMsg_t &gpsMsg)
{
	char buf[100]="\0";
	double lat = (gpsMsg.lat - int(gpsMsg.lat))*60 + int(gpsMsg.lat)*100;
	double lon = (gpsMsg.lon - int(gpsMsg.lon))*60 + int(gpsMsg.lon)*100;
	double yaw = 0;
	float speed = 10/1.85;
	
	sprintf(buf,"$GPRMC,094403.00,A,%4.8f,N,%5.8f,E,%2.4f,%3.2f,260815,6.5,W,A*35",lat,lon,speed,yaw);
	
	//cout <<buf <<endl;
 
	write(fd,buf,sizeof(buf));
}
