#ifndef WzReadSerialThread_H
#define WzReadSerialThread_H

#include <QCoreApplication>
#include <QDebug>
#include <QThread>
#include <QString>
#include <QObject>
#include <Serial/WzSerialPort.h>

class WzReadSerialThread:public QObject
{
    Q_OBJECT
public:
    explicit WzReadSerialThread(QObject *parent=0);
    ~WzReadSerialThread();
    int readFlag;
    char buf[512];
signals:
    void sig_finish();
    void sig_recv_data(char *buf);

public slots:
    void slot_dowork(WzSerialPort *pObj);

};


#endif // WzReadSerialThread_H
