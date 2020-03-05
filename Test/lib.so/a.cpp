#include"lib.h"
#include<iostream>
#include<unistd.h>
int main()
{
	while(1)
	{
		std::cout << g_num <<std::endl;
		usleep(1000000);
	}
	return 0;

}
