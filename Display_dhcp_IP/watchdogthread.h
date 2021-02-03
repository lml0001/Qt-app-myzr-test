#ifndef WATCHDOGTHREAD_H
#define WATCHDOGTHREAD_H

#include <QObject>

#include <QThread>
#include <unistd.h>
#include <iostream>
#include <QApplication>
class watchdogthread: public QThread
{
       Q_OBJECT
public:
    explicit watchdogthread(QObject *parent = nullptr,QApplication *a = nullptr);
    //实现run接口
    void run();
    QApplication *app;
    int cnt;

};

#endif // WATCHDOGTHREAD_H
