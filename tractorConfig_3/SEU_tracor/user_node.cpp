#include "user_node.h"

USER_NODE::USER_NODE() :
    CAN_2_SERIAL()
{
    showStdCanMsg =false;
    cfgDataClearOK = false;
    steerVerifyOK = false;
    angleSensorVerifyOK = false;
    recordTargetPointOK = false;
    targetPointSeq = 0;
    vehicleDrivingMode = MANUAL_MODE ;
    yesOrNo_recordData = false;
    vehicleStatus={0.0,0.0,0.0,0.0,0.0};
}

USER_NODE::~USER_NODE()
{
    this->closeThread();//接收can消息的线程
    this->exit();
    this->wait();
}

void USER_NODE::run()
{
    threadRunFlag = true;
    while(threadRunFlag)//用于外部终止线程
    {
        uint8_t pkgCmd = receiveCanMsgFromDev();
        //printf("%02x\r\n",pkgCmd);
        switch (pkgCmd)
        {
        case 0:
            usleep(5000);
            break;
        case 0xB1://std can msg
            this->stdCanMsgProcess();

            if(this->isShowStdCanMsg())
            {
                /* 大量can消息导致listview卡死*/
                std::stringstream msg;
                msg.clear();
                msg << "0x"<< std::hex <<std::setw(2) << std::setfill('0')<< int(getStdCanMsg().ID) <<"\t";
                for(int i=0;i<getStdCanMsg().len;i++)
                    msg << std::hex <<std::setw(2) << std::setfill('0') << int(getStdCanMsg().data[i]) << "    ";
                std::cout << int(getStdCanMsg().len) <<std::endl;
                emit viewDebugMsg_signal(QString(msg.str().c_str()));
                this->printStdCanMsg(); //print msg in terminal
            }

            break;
      /*  case 0x92: //baudrate set responds
            if(this->isBaudrateConfigOK())
            {
                //printf("baudrate set complete!\r\n");
                emit statusBarShowMsg_signal(QString("baudrate set complete!"),5000);
                this->resetBaudrateConfigStatus();
            }
            break; */
        default:
            break;
        }
        //usleep(500000);
        //tcflush(serial.getPortNum(),TCIFLUSH);

    }
}

//gps数据通过多条can消息发送，只有所需信息接收完整之后才进行存储
void USER_NODE::stdCanMsgProcess()
{
    static uint8_t GPSdataStatus = 0;
    switch (getStdCanMsg().ID)
    {
    case 0x4C0:{
        uint8_t latBuf[4] = {getStdCanMsg().data[3],getStdCanMsg().data[2],getStdCanMsg().data[1],getStdCanMsg().data[0]};
        uint8_t lonBuf[4] = {getStdCanMsg().data[7],getStdCanMsg().data[6],getStdCanMsg().data[5],getStdCanMsg().data[4]};

        vehicleStatus.latitude = *(uint32_t *)latBuf * 1.0 / 10000000;
        vehicleStatus.lontitude= *(uint32_t *)lonBuf *1.0/10000000;
        GPSdataStatus |= 1<<0;
        break;}
    case 0x4C2:
        vehicleStatus.speed = (getStdCanMsg().data[0]*256 + getStdCanMsg().data[1]) *1.0/100;
        vehicleStatus.yaw = (getStdCanMsg().data[2]*256 + getStdCanMsg().data[3]) *1.0/100;
        GPSdataStatus |= 1<<1;
        break;
    case 0x4C3:
        vehicleStatus.roadWheelAngle= (getStdCanMsg().data[1]*256+getStdCanMsg().data[2]) *1.0/100;
        if(getStdCanMsg().data[0]==1)
            vehicleStatus.roadWheelAngle = -vehicleStatus.roadWheelAngle;
        GPSdataStatus |=1<<2;
        break;
    case 0x3C0:
        if(getStdCanMsg().data[0]==0)
            cfgDataClearOK = true;
        else if(getStdCanMsg().data[0]==1)
            steerVerifyOK = true;
        else if(getStdCanMsg().data[0]==2)
            angleSensorVerifyOK =true;
        else if(getStdCanMsg().data[0]==3)
        {
            recordTargetPointOK =true;
            targetPointSeq = getStdCanMsg().data[1];
        }
        else if(getStdCanMsg().data[0]==4)
            vehicleDrivingMode = DrivingStatus_t(getStdCanMsg().data[1]);
    default:
        break;
    }

    if(GPSdataStatus == 0x07)
    {
        this->recordDataToFile();
        GPSdataStatus = 0;
    }
   // qDebug() << GPSdataStatus;
}

void USER_NODE::recordDataToFile()
{
    if(this->isRecordData())
    {
        outFile_trackData.flags(ios_base::fixed);
        outFile_trackData.precision(7);

        outFile_trackData << vehicleStatus.lontitude << "\t" <<vehicleStatus.latitude <<"\t";
        outFile_trackData.precision(2);
        outFile_trackData  << vehicleStatus.yaw <<"\t" <<vehicleStatus.speed <<"\t" \
                           << vehicleStatus.roadWheelAngle <<std::endl;
    }
}

