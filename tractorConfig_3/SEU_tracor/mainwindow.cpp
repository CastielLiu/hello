#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>



//#pragma execution_character_set("utf-8")

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    timer_getDevPort(new QTimer(this)),
    timer_getCurrentAngle(new QTimer(this)),
    timer_updateVehicleMsg(new QTimer(this))
{
    setWindowIcon(QIcon(":/images/seu.png"));
    ui->setupUi(this);

    ui->listView_systemSettingPage->setModel(&logModel_systemSettingPage);
    ui->listView_debugPage->setModel(&logModel_debugPage);

    ui->lineEdit_maxAd->setText(QString("4096"));
    ui->lineEdit_maxAngle->setText(QString("150"));
    ui->lineEdit_testNumToRecordFile->setText(QString("test"));
    ui->pushButton_systemConfig->setDisabled(true);
    ui->pushButton_driverless->setDisabled(true);
    ui->pushButton_systemDebug->setDisabled(true);

    QObject::connect(ui->actionAbout,SIGNAL(triggered(bool)),this,SLOT(aboutMsg()));
    QObject::connect(timer_getDevPort,SIGNAL(timeout()),this,SLOT(getDevPort_slot()));
    QObject::connect(timer_getCurrentAngle,SIGNAL(timeout()),this,SLOT(showCurrentAngle()));
    QObject::connect(timer_updateVehicleMsg,SIGNAL(timeout()),this,SLOT(updateVehicleMsg_slot()));

    timer_getDevPort->start(1000);//1s


    this->setMaximumSize(531,305);
    this->setMinimumSize(531,305);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete userNode;

    timer_getDevPort->stop();
    timer_getCurrentAngle->stop();
    timer_updateVehicleMsg->stop();

    delete timer_getDevPort;
    delete timer_getCurrentAngle;
    delete timer_updateVehicleMsg;
}

void MainWindow::on_pushButton_openConfigPort_clicked(bool checked)
{
    if(checked == true)
    {
        userNode = new USER_NODE();

        QObject::connect(userNode,SIGNAL(viewMsg_signal(QString)),this,SLOT(viewMsg_slot(QString)));
        QObject::connect(userNode,SIGNAL(viewDebugMsg_signal(QString)),this,SLOT(viewDebugMsg_slot(QString)));
        QObject::connect(userNode,SIGNAL(statusBarShowMsg_signal(QString,int)),this,SLOT(statusBarShowMsg_slot(QString,int)));

        QString qstr_port = "/dev/" + ui->comboBox_port->currentText();

        if(userNode->openPort(qstr_port.toStdString().c_str()))
        {
            this->viewMsg(qstr_port + QString(" open ok..."));

            ui->statusBar->showMessage(qstr_port + QString(" open ok..."),5000);

            userNode->start();//start thread

            if(ui->comboBox_baudrate->currentIndex()==0)
                userNode->configBaudrate(500);
            else if(ui->comboBox_baudrate->currentIndex()==1)
                userNode->configBaudrate(250);
            int _timeout = 50;
            while(!userNode->isBaudrateConfigOK())
            {
                usleep(1000);
                _timeout --;
                if(_timeout==0)
                {
                    ui->pushButton_openConfigPort->setChecked(false);
                    ui->statusBar->showMessage(QString("baudrate set failed, please try again!"),5000);
                    return;
                }
            }
            ui->statusBar->showMessage(QString("baudrate set complete!"),5000);

            ui->comboBox_port->setDisabled(true);
            ui->comboBox_baudrate->setDisabled(true);
            ui->pushButton_systemConfig->setDisabled(false);
            ui->pushButton_driverless->setDisabled(false);
            ui->pushButton_systemDebug->setDisabled(false);

            ui->pushButton_openConfigPort->setText(QString::fromLocal8Bit("关闭端口"));
            timer_getCurrentAngle->start(300); //300ms
            timer_updateVehicleMsg->start(300);
        }
        else
        {
            this->viewMsg(qstr_port + QString(" open failed ..."));
            ui->pushButton_openConfigPort->setChecked(false);
        }
    }
    else
    {
       delete userNode;

       ui->comboBox_port->setDisabled(false);
       ui->comboBox_baudrate->setDisabled(false);
       ui->pushButton_systemConfig->setDisabled(true);
       ui->pushButton_driverless->setDisabled(true);
       ui->pushButton_systemDebug->setDisabled(true);

       ui->pushButton_openConfigPort->setText(QString::fromLocal8Bit("打开端口"));
       timer_getCurrentAngle->stop();
       timer_updateVehicleMsg->stop();
    }

}

void MainWindow::on_pushButton_clearConfig_clicked()
{
    STD_CAN_MSG canMsg;
    canMsg.ID = 0x3C0;
    canMsg.len = 1;
    canMsg.data[0] = 0;
    userNode->sendStdCanMsg(canMsg);
   // if(userNode->cfgDataClearOK)
    this->viewMsg(QString("clear configuration data completed."));

}

void MainWindow::viewMsg(QString qstr)
{
    logModel_systemSettingPage.insertRows(logModel_systemSettingPage.rowCount(),1);
    QVariant newRow(qstr);
    logModel_systemSettingPage.setData(logModel_systemSettingPage.index(logModel_systemSettingPage.rowCount()-1),newRow);
     ui->listView_systemSettingPage->scrollToBottom();
    // qDebug() << logModel_systemSettingPage.rowCount() << logModel_systemSettingPage.index(logModel_systemSettingPage.rowCount()-1);
}

void MainWindow::viewDebugMsg(QString qstr)
{
    logModel_debugPage.insertRows(logModel_debugPage.rowCount(),1);

    //QVariant newRow(qstr);
    //logModel_debugPage.setData(logModel_debugPage.index(logModel_debugPage.rowCount()-1),newRow);
    logModel_debugPage.setData(logModel_debugPage.index(logModel_debugPage.rowCount()-1),QVariant(qstr));
    //logModel_debugPage.removeRows()
    ui->listView_debugPage->scrollToBottom();

    qDebug() << logModel_debugPage.rowCount();
}

void MainWindow::on_pushButton_verifySteering_clicked()
{
    if (QMessageBox::Yes == QMessageBox::question( this,
                                                  tr("Verify steering"),
                                                  tr("Is the steering wheel in the middle?"),
                                                  QMessageBox::Yes | QMessageBox::Cancel,
                                                  QMessageBox::Cancel))
    {
        STD_CAN_MSG canMsg;
        canMsg.ID = 0x3C0;
        canMsg.len = 8;
        canMsg.data[0] = 1;
        userNode->sendStdCanMsg(canMsg);
        usleep(300000);
        userNode->sendStdCanMsg(canMsg);
        this->viewMsg(QString("steering verify completed."));
    }
}

void MainWindow::on_pushButton_configAngleSensor_clicked()
{
    QString maxAdValueStr = ui->lineEdit_maxAd->text();
    QString maxAngleStr = ui->lineEdit_maxAngle->text();
    if(maxAdValueStr.isEmpty())
        this->viewMsg(QString("please input the maxinum AD value !"));
    else if(maxAdValueStr.isEmpty())
        this->viewMsg(QString("please input the angle sensor's maxinum angle !"));
    else
    {
        int maxAdValue = maxAdValueStr.toInt();
        int maxAngle = maxAngleStr.toInt();
        STD_CAN_MSG canMsg={0,0,0};
        canMsg.ID = 0x3C0;
        canMsg.len = 8;
        canMsg.data[0] = 2;
        canMsg.data[2] = maxAdValue &0xff;
        canMsg.data[1] = (maxAdValue >>8)&0xff;
        canMsg.data[4] = maxAngle &0xff;
        canMsg.data[3]=(maxAngle >>8)&0xff;

        if(ui->comboBox_steeringSign->currentIndex()==0)
            canMsg.data[5] = 1;
        else
            canMsg.data[5] = 0;

        userNode->sendStdCanMsg(canMsg);
        usleep(300000);
        userNode->sendStdCanMsg(canMsg);
        this->viewMsg(QString("angle sensor configure completed."));
    }
}

void MainWindow::aboutMsg()
{
    QMessageBox *msgBox = new QMessageBox;
    msgBox->information(this,tr("About"),tr("edit by wendao."));
    msgBox->show();
    delete msgBox;
}

void MainWindow::getDevPort_slot()
{
    using namespace std;

    FILE *fpr = NULL ;

    int buf_len = 13*10;
    char buf[buf_len]={'\0'};
    fpr = popen("ls /dev/ttyUSB*","r");
    fread(buf,1,buf_len,fpr);

   QStringList oldList;
   oldList.clear();
   for(int i=0;i<ui->comboBox_port->count();i++)
       oldList << ui->comboBox_port->itemText(i);

   QStringList newList;
   newList.clear();

    for(int i=0;i<10;i++)
    {
        stringstream portName;
        portName << "ttyUSB" << i;

        if(strstr(buf,portName.str().c_str())!=NULL)
           newList <<portName.str().c_str();
    }
    if(oldList != newList)
    {
        ui->comboBox_port->clear();
        ui->comboBox_port->addItems(newList);
    }

    pclose(fpr);
}



void MainWindow::on_toolButton_fromSystemSettingToHome_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->checkBox_steerDebug->setChecked(false);
}

void MainWindow::on_toolButton_fromDriverlessToHome_clicked()
{
     ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_toolButton_fromDebugToHome_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    if(ui->pushButton_canbusMonitor->isChecked())
    {
        userNode->setShowStdCanMsg(false);
        ui->pushButton_canbusMonitor->setText(QString::fromLocal8Bit("开始监控"));
    }
}


void MainWindow::on_pushButton_systemConfig_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);//index=1 system Config
}

void MainWindow::on_pushButton_driverless_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);//index=2 driverless
}



void MainWindow::on_pushButton_systemDebug_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);//index=3 debug
}

void MainWindow::on_pushButton_recordTarget_clicked()
{
    STD_CAN_MSG recordTargetMSg={0x3C0,8};
    recordTargetMSg.data[0] = 3;
    userNode->sendStdCanMsg(recordTargetMSg);

    QString appd = ui->lineEdit_testNumToRecordFile->text();
    QString qfileName = "../data/targetPoint_"+appd+".txt";
   // qDebug()<<qfileName;
    userNode->outFile_trackData.open(qfileName.toStdString().c_str(),ios_base::app);

    userNode->outFile_targetPoint.flags(ios_base::fixed);
    userNode->outFile_targetPoint.precision(7);
    userNode->outFile_targetPoint << userNode->getVehicleStatus().lontitude <<"\t"\
                                  <<userNode->getVehicleStatus().latitude << std::endl;
    userNode->outFile_targetPoint.close();

}


void MainWindow::on_pushButton_startDriverless_clicked(bool checked)
{
    STD_CAN_MSG changeModeMsg={0x3C0,8};
    if(true == checked)
    {
        changeModeMsg.data[0]=4;
        ui->pushButton_startDriverless->setText(QString::fromLocal8Bit("暂停自动驾驶"));
    }
    else
    {
        changeModeMsg.data[0]=5;
        ui->pushButton_startDriverless->setText(QString::fromLocal8Bit("开始自动驾驶"));
    }
    userNode->sendStdCanMsg(changeModeMsg);
    usleep(50000);
    userNode->sendStdCanMsg(changeModeMsg);
}

void MainWindow::on_pushButton_canbusMonitor_clicked(bool checked)
{
    if(checked==true)
    {
         userNode->setShowStdCanMsg(true);
         ui->pushButton_canbusMonitor->setText(QString::fromLocal8Bit("停止监控"));
    }
    else
    {
        userNode->setShowStdCanMsg(false);
        ui->pushButton_canbusMonitor->setText(QString::fromLocal8Bit("开始监控"));
    }
}


void MainWindow::on_pushButton_recordDataTofile_clicked(bool checked)
{
    if(checked==true)
    {
        if(!userNode->outFile_trackData.is_open())
        {
            QString appd = ui->lineEdit_testNumToRecordFile->text();
            QString qfileName = "../data/trackData_"+appd+".txt";
            userNode->outFile_trackData.open(qfileName.toStdString().c_str(),ios_base::app);
            userNode->setRecordDataEnable(true);
        }
        ui->pushButton_recordDataTofile->setText(QString::fromLocal8Bit("停止记录数据"));
    }
    else
    {
        if(userNode->outFile_trackData.is_open())
            userNode->outFile_trackData.close();
        ui->pushButton_recordDataTofile->setText(QString::fromLocal8Bit("开始记录数据"));
    }
}

void MainWindow::showCurrentAngle()
{
    QString str = QString::number(userNode->getVehicleStatus().roadWheelAngle,'f',2);
    ui->lineEdit_currentAngle->setText(str);
}

void MainWindow::on_pushButton_steeringDebug_clicked()
{
    STD_CAN_MSG steeringDebugMsg={0x3C0,8};
    float setAngle = ui->lineEdit_setAngle->text().toFloat();
    if(setAngle>25.0) setAngle=25.0;
    else if(setAngle <-25.0) setAngle = -25.0;
    int16_t  int16_angle =  int16_t(setAngle *100);
    steeringDebugMsg.data[0] = 7;
    steeringDebugMsg.data[1] = (int16_angle >> 8) & 0xff;
    steeringDebugMsg.data[2] = int16_angle & 0xff;
    if(ui->checkBox_steerDebug->checkState()==2)
        steeringDebugMsg.data[3]=1;
    else
        steeringDebugMsg.data[3]=0;
   // qDebug()<<steeringDebugMsg.data[3];
    userNode->sendStdCanMsg(steeringDebugMsg);
   // qDebug() << setAngle ;
}

void MainWindow::on_checkBox_steerDebug_clicked()
{
    if(ui->checkBox_steerDebug->checkState()==false) //close debug
    {
        STD_CAN_MSG steeringDebugMsg={0x3C0,8};
        steeringDebugMsg.data[0] = 7;
        steeringDebugMsg.data[3] = 0;
        userNode->sendStdCanMsg(steeringDebugMsg);
        //qDebug()<<"bbb";
    }
    //else
    // qDebug()<< ui->checkBox_steerDebug->checkState();
}

void MainWindow::statusBarShowMsg_slot(QString qstr,int timeOut)
{
    ui->statusBar->showMessage(qstr,timeOut);
}

void MainWindow::on_pushButton_steeringReEnable_clicked()
{
    STD_CAN_MSG steeringReEnableMsg={0x3C0,8};
    steeringReEnableMsg.data[0] = 8;
    userNode->sendStdCanMsg(steeringReEnableMsg);
}

void MainWindow::on_pushButton_steeringReEnable_2_clicked()
{
    STD_CAN_MSG steeringReEnableMsg={0x3C0,8};
    steeringReEnableMsg.data[0] = 8;
    userNode->sendStdCanMsg(steeringReEnableMsg);
}

void MainWindow::updateVehicleMsg_slot()
{
    ui->lineEdit_lontitude->setText(QString::number(userNode->getVehicleStatus().lontitude,'f',7));
    ui->lineEdit_latitude->setText(QString::number(userNode->getVehicleStatus().latitude,'f',7));
    ui->lineEdit_vehicleYaw->setText(QString::number(userNode->getVehicleStatus().yaw,'f',2));
    ui->lineEdit_roadWheelngle->setText(QString::number(userNode->getVehicleStatus().roadWheelAngle,'f',2));

}

void MainWindow::on_pushButton_pauseDriverless_clicked()
{
    STD_CAN_MSG pauseDriverless={0x3C0,8};
    pauseDriverless.data[0] = REQ_PAUSE_DRIVERLESS;
    userNode->sendStdCanMsg(pauseDriverless);
}

void MainWindow::on_pushButton_startDriverless_clicked()
{
    STD_CAN_MSG startDriverless={0x3C0,8};
    startDriverless.data[0] = REQ_START_DRIVERLESS;
    userNode->sendStdCanMsg(startDriverless);
}

void MainWindow::on_pushButton_stopDriverless_clicked()
{
    if (QMessageBox::Yes == QMessageBox::question( this,
                                                  tr("question"),
                                                  tr("Are you sure ?"),
                                                  QMessageBox::Yes | QMessageBox::Cancel,
                                                  QMessageBox::Yes))
    {
        STD_CAN_MSG stopDriverless={0x3C0,8};
        stopDriverless.data[0] = REQ_STOP_DRIVERLESS;
        userNode->sendStdCanMsg(stopDriverless);
    }
}



void MainWindow::on_pushButton_disableSteer_clicked()
{
    STD_CAN_MSG steeringDisableMsg={0x3C0,8};
    steeringDisableMsg.data[0] = 9;
    userNode->sendStdCanMsg(steeringDisableMsg);
}
