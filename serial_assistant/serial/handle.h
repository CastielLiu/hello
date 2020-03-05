#ifndef HANDLE_H
#define HANDLE_H
#include<QThread>
#include<QStringListModel>
#include<QStringList>
#include"serial/serial.h"
#include<iostream>
#include<string>
#include<boost/thread.hpp>
#include<boost/bind.hpp>
#include<sstream>
#include<cstdio>

using namespace serial;

bool is_hex_string(std::string &str);

class Handle : public QThread
{
Q_OBJECT

public:
    typedef enum
    {
        SerialClosed,
        SerialOpened,
        SerialRunning

    }serial_status_t;

public:
    Handle();
    ~Handle();

    bool init();
    virtual void run();

    void newThread();

    void startRefreshSerialPort(){is_refresh_serial_port = true;}

    serial_status_t &serialStatus(){return serial_status;}

    int readSerial(uint8_t *buf, int len);
    int writeSerial(const uint8_t *buf,int len);

Q_SIGNALS:
    void loggingUpdated();
    void updateSerialPort(QStringList list);
    void show_log(QString qstr);
    void showMessage_in_statusBar(QString qstr,int time_out);

public Q_SLOTS:
   void openSerial(std::string port, int boudrate, float stopBit, char dataBit, char checkBit);
    void closeSerial();

private:
    void refreshSerialPort();

    void closeAllThread();

private:

     bool run_flag;

     Serial *serial;

     serial_status_t serial_status;

     boost::shared_ptr<boost::thread> new_thread;

     bool is_refresh_serial_port;

     boost::mutex mutex;
};

#endif // HANDLE_H
