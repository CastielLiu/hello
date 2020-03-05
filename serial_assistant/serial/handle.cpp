#include"handle.h"
#include<thread>
#include<fcntl.h>
#include<unistd.h>

Handle::Handle():
    serial_status(this->SerialClosed),
    is_refresh_serial_port(true)
{
    run_flag = true;
}
Handle::~Handle()
{
    closeAllThread();
}

void Handle::closeAllThread()
{
     run_flag = false;
     this->terminate(); //close the QThread
}

bool Handle::init()
{
     new_thread = boost::shared_ptr<boost::thread >
                 (new boost::thread(boost::bind(&Handle::newThread, this)));

     this->start(); //start the QThread
    return true;
}

void Handle::newThread()
{
    while(run_flag)
    {
        if(is_refresh_serial_port)
        {
            refreshSerialPort();
            boost::mutex::scoped_lock look(mutex);
            is_refresh_serial_port = false;
        }

       usleep(100000);
       //std::cout << "boost thread is runing.. ID:" << std::this_thread::get_id() << std::endl;
    }
}

void Handle::refreshSerialPort()
{
    FILE *fpr = NULL ;
    const int Max_len = 105*13;

    char *buf = new char[Max_len];

    QStringList list;

    fpr = popen("ls /dev/ttyS* /dev/ttyU* /dev/U*","r");

    fread(buf,1,Max_len,fpr);

    list.clear();

    std::stringstream ss(buf);

    std::string port;

    while(ss >> port)
    {
        if(port.find("dev") != port.npos)
            list << port.substr(5,10).c_str();
    }
    list.sort();

    pclose(fpr);

    delete [] buf;

    Q_EMIT this->updateSerialPort(list);
}


void Handle::run()
{


    while(run_flag)
    {
       //std::cout << "QThread is runing.. ID:" << std::this_thread::get_id() << std::endl;
       usleep(100000);

    }
}

void Handle::openSerial(std::string port,int baudrate,float stopBit,char dataBit,char checkBit)
{
    port = "/dev/"+port;
/*
    if(open(port.c_str(), O_RDWR | O_NONBLOCK)==-1)
    {
        std::string log_msg = "open serial "+ port +" failed ";
        Q_EMIT this->showMessage_in_statusBar(QString::fromStdString(log_msg),3000);
        return ;
    }
*/

    stopbits_t stopbit;
    if(stopBit==1)  stopbit = stopbits_one;
    else if(stopBit ==2) stopbit = stopbits_two;
    else stopbit = stopbits_one_point_five;

    this->serial = new Serial(port,baudrate,serial::Timeout(10)); /*,bytesize_t(dataBit),
                              parity_t(checkBit),stopbit,flowcontrol_none);*/

    if(serial->isOpen())
    {
        serial_status = this->SerialOpened;

        std::string log_msg = "open serial "+ port +" ok ";

        Q_EMIT this->showMessage_in_statusBar(QString::fromStdString(log_msg),3000);
    }
    else
    {
        delete serial;
        serial = NULL;
        std::string log_msg = "open serial "+ port +" failed ";
        Q_EMIT this->showMessage_in_statusBar(QString::fromStdString(log_msg),3000);
    }
}

void Handle::closeSerial()
{
    serial_status = this->SerialClosed;
    usleep(10000); //wait other thread

    serial->close();
    delete serial;
    serial = NULL;

    //std::cout << "serial closed"<<std::endl;
    Q_EMIT showMessage_in_statusBar(QString::fromStdString("serial has closed..."),2000);

}

int Handle::readSerial(uint8_t *buf, int len)
{
    if(serial_status != SerialClosed)
    {
        return serial->read(buf, len);
    }
    return -1;
}

int Handle::writeSerial(const uint8_t *buf, int len)
{
    if(serial_status != SerialClosed)
    {
        return  serial->write(buf,len);
    }
    return -1;
}

bool is_hex_string(std::string &str)
{
    for(size_t i=0;i<str.length();i++)
    {
        if(!((str[i]<='9' && str[i] >='0') ||
           (str[i]>='A' && str[i] <= 'F')||
           (str[i]>='a' && str[i] <= 'f') ||
           (str[i]==' ')))
            return false;
    }
    return true;
}
