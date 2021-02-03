#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <thread>
#include <chrono>
#include <netdb.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <QMainWindow>
#include <QTimerEvent>
#include <QTextCodec>
#include <QDebug>
#include <QtSql>
#include <QSqlQuery>
#include <watchdogthread.h>
#include <QApplication>

#define SYSFS_GPIO_EXPORT "/sys/class/gpio/export"
#define SYSFS_GPIO_RST_PIN_VAL_1 "20"
#define SYSFS_GPIO_RST_PIN_VAL_2 "21"
#define SYSFS_GPIO_RST_PIN_VAL_3 "24"
#define SYSFS_GPIO_RST_PIN_VAL_4 "25"
#define SYSFS_GPIO_RST_PIN_VAL_5 "28"
#define SYSFS_GPIO_RST_PIN_VAL_6 "29"
#define SYSFS_GPIO_RST_PIN_VAL_7 "30"
#define SYSFS_GPIO_RST_PIN_VAL_8 "31"
#define SYSFS_GPIO_RST_DIR_1 "/sys/class/gpio/gpio20/direction"
#define SYSFS_GPIO_RST_DIR_2 "/sys/class/gpio/gpio21/direction"
#define SYSFS_GPIO_RST_DIR_3 "/sys/class/gpio/gpio24/direction"
#define SYSFS_GPIO_RST_DIR_4 "/sys/class/gpio/gpio25/direction"
#define SYSFS_GPIO_RST_DIR_5 "/sys/class/gpio/gpio28/direction"
#define SYSFS_GPIO_RST_DIR_6 "/sys/class/gpio/gpio29/direction"
#define SYSFS_GPIO_RST_DIR_7 "/sys/class/gpio/gpio30/direction"
#define SYSFS_GPIO_RST_DIR_8 "/sys/class/gpio/gpio31/direction"
#define SYSFS_GPIO_RST_DIR_VAL_OUT "out"
#define SYSFS_GPIO_RST_DIR_VAL_IN "in"
#define SYSFS_GPIO_RST_VAL_1 "/sys/class/gpio/gpio20/value"
#define SYSFS_GPIO_RST_VAL_2 "/sys/class/gpio/gpio21/value"
#define SYSFS_GPIO_RST_VAL_3 "/sys/class/gpio/gpio24/value"
#define SYSFS_GPIO_RST_VAL_4 "/sys/class/gpio/gpio25/value"
#define SYSFS_GPIO_RST_VAL_5 "/sys/class/gpio/gpio28/value"
#define SYSFS_GPIO_RST_VAL_6 "/sys/class/gpio/gpio29/value"
#define SYSFS_GPIO_RST_VAL_7 "/sys/class/gpio/gpio30/value"
#define SYSFS_GPIO_RST_VAL_8 "/sys/class/gpio/gpio31/value"
#define SYSFS_GPIO_RST_VAL_H "1"
#define SYSFS_GPIO_RST_VAL_L "0"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr,QApplication *a = nullptr);
    ~MainWindow();
    virtual void timerEvent( QTimerEvent *event);

private slots:
    void on_pushButton_clicked();

private:
    QApplication *app;
    Ui::MainWindow *ui;
    QSqlDatabase db;
    int m_nTimerID0;
};

#endif // MAINWINDOW_H
