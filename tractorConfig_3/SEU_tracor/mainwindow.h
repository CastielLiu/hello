#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "user_node.h"
#include <QStringListModel>
#include <QStackedWidget>
#include <unistd.h>

#include <QMainWindow>
#include <QMessageBox>
#include <QTimer>

#include <string>
#include <cstring>
#include <sstream>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void viewMsg(QString qstr);
    void viewDebugMsg(QString qstr);
    void switchPage();

private slots:
    void on_pushButton_clearConfig_clicked();

    void aboutMsg();

    void on_pushButton_verifySteering_clicked();

    void on_pushButton_configAngleSensor_clicked();

    void viewMsg_slot(QString qstr) {viewMsg(qstr);}
    void viewDebugMsg_slot(QString qstr) {viewDebugMsg(qstr);}
    void statusBarShowMsg_slot(QString qstr, int timeOut);

    void on_pushButton_openConfigPort_clicked(bool checked);

    void getDevPort_slot();

    void showCurrentAngle();

    void updateVehicleMsg_slot();

    void on_pushButton_systemConfig_clicked();

    void on_toolButton_fromSystemSettingToHome_clicked();

    void on_toolButton_fromDriverlessToHome_clicked();

    void on_pushButton_driverless_clicked();

    void on_toolButton_fromDebugToHome_clicked();

    void on_pushButton_systemDebug_clicked();

    void on_pushButton_recordTarget_clicked();


    void on_pushButton_startDriverless_clicked(bool checked);

    void on_pushButton_canbusMonitor_clicked(bool checked);

    void on_pushButton_recordDataTofile_clicked(bool checked);

    void on_pushButton_steeringDebug_clicked();

    void on_checkBox_steerDebug_clicked();

    void on_pushButton_steeringReEnable_clicked();

    void on_pushButton_steeringReEnable_2_clicked();

    void on_pushButton_pauseDriverless_clicked();

    void on_pushButton_startDriverless_clicked();

    void on_pushButton_stopDriverless_clicked();


    void on_pushButton_disableSteer_clicked();

private:
    Ui::MainWindow *ui;
    QStringListModel logModel_systemSettingPage;
    QStringListModel logModel_debugPage;

    USER_NODE *userNode;

    QTimer *timer_getDevPort;
    QTimer *timer_getCurrentAngle;
    QTimer *timer_updateVehicleMsg;


};

#endif // MAINWINDOW_H
