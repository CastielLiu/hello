#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    user_handle(new Handle),
    update_widget_timer(new QTimer(this)),
    MaxLineCount(500),
    MaxCharacterCount(100000),
    current_send_msg_index(0)
{
    ui->setupUi(this);
    this->setWindowTitle("Serial Assistant");
    this->setWindowIcon(QIcon(":/images/seu.png"));

    //this->setMaximumSize(627,547);
    //this->setMinimumSize(762,543);
    this->setBaseSize(796,650);

    ui->comboBox_baundrate->setCurrentIndex(4);
    ui->plainTextEdit_log->setReadOnly(true);
    ui->plainTextEdit_input->installEventFilter(this);

    user_handle->init();

    QObject::connect(user_handle, SIGNAL(loggingUpdated()), this, SLOT(updateLoggingView()));
    QObject::connect(this,SIGNAL(openSerial(std::string,int,float,char,char)),
                     user_handle,SLOT(openSerial(std::string,int,float,char,char)));
    QObject::connect(this,SIGNAL(closeSerial()),user_handle,SLOT(closeSerial()));
    QObject::connect(user_handle,SIGNAL(updateSerialPort(QStringList)),
                     this,SLOT(updateSerialPort(QStringList)));
    QObject::connect(update_widget_timer, SIGNAL(timeout()), this, SLOT(updateWidged()));
    QObject::connect(this,SIGNAL(show_log_signal(QString)),this,SLOT(show_log_slot(QString)));
    QObject::connect(user_handle,SIGNAL(showMessage_in_statusBar(QString,int)),
                     this,SLOT(showMessage_in_statusBar(QString,int)));
    QObject::connect(ui->actionAbout,SIGNAL(triggered(bool)),this,SLOT(actionAbout_slot()));
    QObject::connect(ui->actionVersion,SIGNAL(triggered(bool)),this,SLOT(actionVersion_slot()));


    update_widget_timer->start(200);

    new_thread = boost::shared_ptr<boost::thread >
                (new boost::thread(boost::bind(&MainWindow::newThread, this)));
    setSomeWidgedStatus(true);
}

void MainWindow::init()
{
    setPlainTextEditClor(ui->plainTextEdit_log,QPalette::Text,Qt::green);
    setPlainTextEditClor(ui->plainTextEdit_log,QPalette::Base,Qt::black);
}

void MainWindow::newThread()
{
    new_thread_run_flag = true;

    const int Len = 3000;
    uint8_t * buf = new uint8_t[Len+1];

    std::stringstream ss;

    while(new_thread_run_flag)
    {
       //std::cout << "QThread is runing.. ID:" << std::this_thread::get_id() << std::endl;

        usleep(300000);

        int len = user_handle->readSerial(buf,Len-1);

        if(len <= 0)
            continue;

        if(ui->checkBox_receiveHex->isChecked())
        {
            std::stringstream ss("");
            for(size_t i =0; i< len; i++)
            {
                //printf("%02x ",buf[i]);
                ss << std::hex << std::setw(2) <<std::setfill('0') << int(buf[i]);
                ss << "  ";
            }
            Q_EMIT this->show_log_signal(QString::fromStdString(ss.str()));
        }
        else
        {
            buf[len] = '\0';
            std::string str((char *)buf);
            Q_EMIT this->show_log_signal(QString::fromStdString(str));
        }
    }
    delete [] buf;
}

MainWindow::~MainWindow()
{
    new_thread_run_flag = false;
    delete ui;

    delete user_handle;
    update_widget_timer->stop();
    delete update_widget_timer;

}

void MainWindow::updateLoggingView()
{

}

void MainWindow::on_pushButton_openSerial_clicked(bool checked)
{
    if(checked == true)
    {
        std::string port = ui->comboBox_serialPort->currentText().toStdString();
        int baudrate = ui->comboBox_baundrate->currentText().toInt();
        float stopBit = ui->comboBox_checkBit->currentText().toFloat();
        char dataBit = ui->comboBox_dataBiit->currentText().toInt();
        char checkBit = ui->comboBox_stopBit->currentIndex();
        Q_EMIT this->openSerial(port,baudrate,stopBit,dataBit,checkBit);
        this->setSomeWidgedStatus(false);
    }
    else
    {
        Q_EMIT this->closeSerial();
        ui->pushButton_openSerial->setText("open serial");
        this->setSomeWidgedStatus(true);
    }
}

void MainWindow::updateSerialPort(QStringList list)
{
    ui->comboBox_serialPort->clear();
    ui->comboBox_serialPort->addItems(list);
    ui->comboBox_serialPort->addItem("Refresh");
}

void MainWindow::on_comboBox_serialPort_activated(const QString &arg1)
{
    if(arg1 == "Refresh")
    {
        user_handle->startRefreshSerialPort();
    }
}

void MainWindow::updateWidged()
{
    if(user_handle->serialStatus()==user_handle->SerialOpened && ui->pushButton_openSerial->isChecked())
    {
         user_handle->serialStatus() = user_handle->SerialRunning;
         ui->pushButton_openSerial->setText("close serial");
    }
}

void MainWindow::on_plainTextEdit_log_textChanged()
{
     //ui->plainTextEdit_log->moveCursor(QTextCursor::End);
}

void MainWindow::show_log_slot(QString qstr)
{
    // std::cout << "document line num:" << ui->plainTextEdit_log->document()->lineCount()<<std::endl;
    // std::cout << "document character num:" << ui->plainTextEdit_log->toPlainText().count() <<std::endl;

     ui->plainTextEdit_log->insertPlainText(qstr);

     if(ui->plainTextEdit_log->toPlainText().count() - MaxCharacterCount > 10000)
     {
         QTextCursor cursor = ui->plainTextEdit_log->textCursor();
         cursor.setPosition(10000);
         ui->plainTextEdit_log->setTextCursor(cursor);  //must, otherwise "\n" will insert in former cursor
         ui->plainTextEdit_log->insertPlainText("\n");
         cursor.setPosition(1);
         cursor.select(QTextCursor::BlockUnderCursor);
         cursor.removeSelectedText();
     }

     ui->plainTextEdit_log->moveCursor(QTextCursor::End);
}

void MainWindow::on_pushButton_clearReceive_clicked()
{
    ui->plainTextEdit_log->clear();
    ui->plainTextEdit_log->setPlainText("");
}

void MainWindow::showMessage_in_statusBar(QString qstr,int time_out)
{
    ui->statusBar->showMessage(qstr,time_out);
}

void MainWindow::setSomeWidgedStatus(bool status)
{
    ui->comboBox_baundrate->setEnabled(status);
    ui->comboBox_checkBit->setEnabled(status);
    ui->comboBox_dataBiit->setEnabled(status);
    ui->comboBox_serialPort->setEnabled(status);
    ui->comboBox_stopBit->setEnabled(status);
    ui->pushButton_send->setDisabled(status);
    ui->checkBox_delayedSend->setDisabled(status);

    QString styleSheet;
    if(status)
        styleSheet = "QPushButton{color:green;}";
    else
        styleSheet = "QPushButton{color:red;}";

    ui->pushButton_openSerial->setStyleSheet(styleSheet);
}


void MainWindow::on_pushButton_send_clicked()
{
    QString qstr =  ui->plainTextEdit_input->toPlainText();
    std::string str = qstr.toStdString();

    if(str.empty())
        return;

    if(ui->checkBox_sendHex->isChecked())
    {
        if(!is_hex_string(str))
        {
            this->showMessage_in_statusBar("illegal input !!!",3000);
            return ;
        }

        uint8_t *buf = new uint8_t[str.length()/2+2];

        std::stringstream ss_old(str);
        int int_num;

        int count = 0;
        while(ss_old >> std::hex >> int_num)
        {
            buf[count] = int_num;
            count ++;
        }
        if(ui->checkBox_sendNewLine->isChecked())
        {
            buf[count++] = '\r';
            buf[count++] = '\n';
        }
        if(user_handle->writeSerial(buf,count))
            this->showMessage_in_statusBar("send message ok",1000);
        else
            this->showMessage_in_statusBar("send message failed",1000);

       /*for(int i=0; i<count; i++)
        {
            printf("%02x  ",buf[i]);
        }
        printf("\r\n");*/

        delete [] buf;
    }
    else
    {
        if(ui->checkBox_sendNewLine->isChecked())
            str = str+"\r\n";
        if(user_handle->writeSerial((const uint8_t *)str.c_str(),str.length()))
        {
            this->showMessage_in_statusBar("send message ok",1000);
        }
        else
            this->showMessage_in_statusBar("send message failed",1000);

       // printf("%s\r\n",str.c_str());
    }

    send_msg_list.removeOne(qstr);
    if(send_msg_list.size() > 10)
        send_msg_list.removeFirst();
    send_msg_list.append(qstr);
    current_send_msg_index = send_msg_list.size()-1;

}

void MainWindow::on_pushButton_clearSend_clicked()
{
    ui->plainTextEdit_input->clear();

    if(send_msg_list.size() > 0)
        current_send_msg_index = send_msg_list.size() - 1;
}

void MainWindow::actionAbout_slot()
{
    std::string msgs = "";
    msgs = msgs +
           "Author      : wendao\n" +
           "Orgnization : southeast university\n" +
           "Email       : castiel_liu@outlook.com";

    QMessageBox::information(this,"About",QString::fromStdString(msgs));
}

void MainWindow::actionVersion_slot()
{
    QMessageBox::information(this,"Version",QString::fromStdString("version. 1.0 "));
}

void MainWindow::on_pushButton_saveWindow_clicked()
{

}

void MainWindow::on_checkBox_2_clicked(bool checked)
{
    if(checked == true)
    {
        setPlainTextEditClor(ui->plainTextEdit_log,QPalette::Text,Qt::black);
        setPlainTextEditClor(ui->plainTextEdit_log,QPalette::Base,Qt::white);
       // ui->plainTextEdit_log->setStyleSheet("background-color:rgb(0,0,0);");
    }
    else
    {
        setPlainTextEditClor(ui->plainTextEdit_log,QPalette::Text,Qt::green);
        setPlainTextEditClor(ui->plainTextEdit_log,QPalette::Base,Qt::black);
       // ui->plainTextEdit_log->setStyleSheet("background-color:rgb(255,255,255);");
    }
}

void MainWindow::setPlainTextEditClor(QPlainTextEdit *edit,QPalette::ColorRole cr,const QColor &color)
{
    QPalette p = edit->palette();
    p.setColor(QPalette::Active, cr, color);
    p.setColor(QPalette::Inactive, cr, color);
    edit->setPalette(p);
}




void MainWindow::on_pushButton_last_send_msg_clicked()
{
    if(current_send_msg_index > 0)
    {
        current_send_msg_index --;
        ui->plainTextEdit_input->clear();
        ui->plainTextEdit_input->appendPlainText(send_msg_list.at(current_send_msg_index));
    }

}

void MainWindow::on_pushButton_next_send_msg_clicked()
{
    if(current_send_msg_index+1 < send_msg_list.size())
    {
        current_send_msg_index ++;
        ui->plainTextEdit_input->clear();
        ui->plainTextEdit_input->appendPlainText(send_msg_list.at(current_send_msg_index));
    }
}

bool MainWindow::eventFilter(QObject *target, QEvent *event)
{
   QKeyEvent *qkey_event  = static_cast<QKeyEvent *>(event);

    if(target == ui->plainTextEdit_input)
    {
        if(event->type() == QEvent::KeyPress)
        {
            if((qkey_event->key() == Qt::Key_Return || qkey_event->key() == Qt::Key_Enter) &&
               (qkey_event->modifiers() & Qt::ControlModifier))
            {
                    Q_EMIT ui->pushButton_send->click();
            }
        }

    }

    return QWidget::eventFilter(target,event);
}
