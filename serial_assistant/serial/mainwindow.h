#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include<handle.h>
#include<QTimer>
#include<sstream>
#include<QTextBlock>
#include<QTextCursor>
#include<sstream>
#include<QMessageBox>
#include<QDebug>
#include<QColor>
#include<QPlainTextEdit>



#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void newThread();
    void init();

private:
    void setSomeWidgedStatus(bool status);
    void setPlainTextEditClor(QPlainTextEdit *edit,QPalette::ColorRole cr,const QColor &color);
protected:
    bool eventFilter(QObject *target, QEvent *event);//事件过滤器



private:
    Ui::MainWindow *ui;
    Handle * user_handle;
    QTimer * update_widget_timer;

    boost::shared_ptr<boost::thread> new_thread;
    bool new_thread_run_flag;

    const int MaxLineCount;
    const int MaxCharacterCount;

    QStringList send_msg_list;
    size_t current_send_msg_index;


public Q_SLOTS:
    void updateLoggingView();
    void updateSerialPort(QStringList list);
    void updateWidged();
    void show_log_slot(QString str);
    void showMessage_in_statusBar(QString qstr, int time_out);
    void actionAbout_slot();
    void actionVersion_slot();

private slots:
    void on_pushButton_openSerial_clicked(bool checked);

    void on_comboBox_serialPort_activated(const QString &arg1);

    void on_plainTextEdit_log_textChanged();


    void on_pushButton_clearReceive_clicked();

    void on_pushButton_send_clicked();

    void on_pushButton_clearSend_clicked();

    void on_pushButton_saveWindow_clicked();

    void on_checkBox_2_clicked(bool checked);

    void on_pushButton_last_send_msg_clicked();

    void on_pushButton_next_send_msg_clicked();

Q_SIGNALS:
    void openSerial(std::string port,int boudrate,float stopBit,char dataBit,char checkBit);
    void closeSerial();
    void show_log_signal(QString str);
};

#endif // MAINWINDOW_H
