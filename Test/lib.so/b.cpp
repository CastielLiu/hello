#include"lib.h"
#include<unistd.h>
#include<iostream>
int main()
{
	while(1)
	{
		usleep(100000);
		g_num ++;
	 	std::cout << g_num<<std::endl;
	}
	return 0;

}
