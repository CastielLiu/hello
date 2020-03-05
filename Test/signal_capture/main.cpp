#include <iostream>
#include <csignal>
#include <unistd.h>
#include<cstdlib>
 
using namespace std;

class Test
{
public:
	Test(){}
	~Test()
	{
		cout << "over" <<endl;
	}
};

 
void signalHandler( int signum )
{
    cout << "Interrupt signal (" << signum << ") received.\n";
 
    // 清理并关闭
    // 终止程序  
 	cout << "this is heli de !!" <<endl;
 
	exit(signum);  
 
}
 
int main ()
{
    // 注册信号 SIGINT 和信号处理程序
    signal(SIGINT, signalHandler);  
    Test test;
 
    while(1){
       cout << "Going to sleep...." << endl;
       sleep(1);
    }
 
    return 0;
}
