#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <linux/input.h>
#include <fcntl.h>
#include <stdint.h>
#include <cstring>



typedef struct input_event Event;

void simulateKeyBoard(int fd ,uint16_t code)
{
	Event event;
	
	gettimeofday(&event.time, 0);
	event.type = EV_KEY;
	event.code = code;
	event.value = 1;
	write(fd,&event,sizeof(event));
	
	event.type = EV_SYN; //report to system
	event.value = 0;
	event.code = SYN_REPORT;
	write(fd,&event,sizeof(event));
	
	gettimeofday(&event.time,0);
	event.type = EV_KEY;
	event.code = code ;
	event.value = 0;
	write(fd,&event,sizeof(event));
	
	event.type = EV_SYN;
	event.value = 0;
	event.code = SYN_REPORT;
	write(fd,&event,sizeof(event));
}

void simulateMouse(int fd,uint16_t code,int x,int y)
{
	Event event;
	gettimeofday(&event.time,0);
	
	//x轴坐标的相对位移
	event.type = 3;
	event.code = 1;//rel_x
	event.value = x;
	write(fd,&event,sizeof(event));
	
	//y轴坐标的相对位移
	event.type =3;
	event.code = 0;//rel_y
	event.value = y;
	write(fd,&event,sizeof(event));

   //同步	
	event.type = EV_SYN;
	event.value = 0;
	event.code = SYN_REPORT;
	write(fd,&event,sizeof(event));
	
	//鼠标按下
	gettimeofday(&event.time,0);
	event.type = 1;//press key
	event.code = code;
	event.value = 1;//press
	write(fd,&event,sizeof(event));
	
	 //同步
	event.type = EV_SYN;
	event.value = 0;
	event.code = SYN_REPORT; 
	write(fd,&event,sizeof(event));
	
	//鼠标弹起
	gettimeofday(&event.time,0);
	event.type = 1;//press key
	event.code = code;
	event.value = 0;
	write(fd,&event,sizeof(event));
	
	 //同步
	event.type = EV_SYN;
	event.value = 0;
	event.code = SYN_REPORT;
	write(fd,&event,sizeof(event));
}


 
int main(int argc,char** argv)
{
    int keyBoard_fd,mouse_fd;

    struct input_event keyBoardEvent, mouseEvent;
    keyBoard_fd=open("/dev/input/event1",O_RDWR|O_NONBLOCK);//keyBoard 只读&非阻塞MODE
    mouse_fd=open("/dev/input/event2",O_RDWR|O_NONBLOCK);//mouse 只读&非阻塞MODE
    if(keyBoard_fd<=0 || mouse_fd<=0)
    {   
        printf("open input event file error\n");
        return -1; 
    }   
    
    while(1)
    {  
    	read(keyBoard_fd,&keyBoardEvent,sizeof(keyBoardEvent)); 
    	read(mouse_fd,&mouseEvent,sizeof(mouseEvent));
    	if(mouseEvent.type ==1)
    	{
			if( mouseEvent.code ==273 && mouseEvent.value ==0) //鼠标右键弹起
			{
				//usleep(10000);
				std::cout <<"标注完成"<<std::endl;
				simulateMouse(mouse_fd,272,15967,46053);//选择标注完成
				//std::cout <<"标注完成"<<std::endl;
			}
			else if( mouseEvent.code ==274 && mouseEvent.value ==0) //滚轮按下
			{
				usleep(10000);
				simulateMouse(mouse_fd,272,47967,42725);//选择有cancel
				usleep(10000);
				simulateMouse(mouse_fd,272,15967,48869);//选择有问题
				std::cout <<"有问题"<<std::endl;
			}
		}
		
    	if(keyBoardEvent.type==1 )
    	{
  #ifdef MODE1
    		if(keyBoardEvent.code == 30 && keyBoardEvent.value == 0)// A up
    		{
    			simulateMouse(mouse_fd,272,35103,31308);//1
    			usleep(30000);
    			simulateMouse(mouse_fd,272,36703,31308);//man
    			usleep(30000);
    			simulateMouse(mouse_fd,272,33503,34124);//选择  无遮挡
    			usleep(300000);
    			simulateMouse(mouse_fd,272,48095,37759);//选择ok
    			//simulateMouse(mouse_fd,272,48095,37759);//选择ok
    			
    		}
			else if(keyBoardEvent.code ==31 &&keyBoardEvent.value ==0)//s up
			{
				simulateMouse(mouse_fd,272,35103,34124);//2
    			usleep(30000);
    			simulateMouse(mouse_fd,272,36703,31308);//man
    			usleep(30000);
    			simulateMouse(mouse_fd,272,33503,34124);//选择  无遮挡
    			usleep(300000);
    			simulateMouse(mouse_fd,272,48095,37759);//选择ok
			
			}
			else if(keyBoardEvent.code ==32 &&keyBoardEvent.value ==0 ) //z
			{
				simulateMouse(mouse_fd,272,35103,36940);//3
    			usleep(30000);
    			simulateMouse(mouse_fd,272,36703,31308);//man
    			usleep(30000);
    			simulateMouse(mouse_fd,272,33503,34124);//选择  无遮挡
    			usleep(300000);
    			simulateMouse(mouse_fd,272,48095,37759);//选择ok
			}
			else if(keyBoardEvent.code ==33 &&keyBoardEvent.value ==0 )//x
			{
				simulateMouse(mouse_fd,272,35103,39602);//4
    			usleep(30000);
    			simulateMouse(mouse_fd,272,36703,31308);//man
    			usleep(30000);
    			simulateMouse(mouse_fd,272,33503,34124);//选择  无遮挡
    			usleep(300000);
    			simulateMouse(mouse_fd,272,48095,37759);//选择ok
			}
		#else
			if(keyBoardEvent.code ==44 &&keyBoardEvent.value ==0 )//z
			{
				
    			simulateMouse(mouse_fd,272,36703,31308);//man
    			usleep(30000);
    			simulateMouse(mouse_fd,272,33503,34124);//选择  无遮挡
    			usleep(300000);
    			simulateMouse(mouse_fd,272,48095,37759);//选择ok
			}
			if(keyBoardEvent.code ==45 &&keyBoardEvent.value ==0 )//z
			{
				
    			simulateMouse(mouse_fd,272,36703,31308);//man
    			usleep(30000);
    			simulateMouse(mouse_fd,272,33503,31308);//选择  遮挡
    			usleep(300000);
    			simulateMouse(mouse_fd,272,48095,37759);//选择ok
			}
		#endif
			
			else if(keyBoardEvent.code ==57 &&keyBoardEvent.value ==0 )//space
			{
				std::cout <<"标注完成"<<std::endl;
				simulateMouse(mouse_fd,272,15967,46053);//选择标注完成
			}
		
		}
		
      	usleep(30000);
      	memset(&keyBoardEvent,0,sizeof(keyBoardEvent));
      	memset(&mouseEvent,0,sizeof(mouseEvent));
      	//std::cout << "waiting" <<std::endl;
       
    }   
    close(keyBoard_fd);
    close(mouse_fd );
    return 0;
}

