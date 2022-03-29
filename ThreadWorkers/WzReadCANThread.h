
#ifndef WzReadCANThread_H
#define WzReadCANThread_H

#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <QString>
#include <QObject>
#include "can/WzCan.h"

class WzReadCANThread:public QObject
{
    Q_OBJECT
public:
    explicit WzReadCANThread(QObject *parent=0);
    ~WzReadCANThread();
    int readFlag;
    char buf[512];
signals:
    void sig_finish();
    void sig_recv_data(char *buf, int len);

public slots:
    void slot_dowork(WzCan *pObj);

};


#endif // WzReadSerialThread_H
