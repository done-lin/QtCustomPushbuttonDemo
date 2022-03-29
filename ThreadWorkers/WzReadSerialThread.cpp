#include "QObject"
#include "WzReadSerialThread.h"
#include "Serial/WzSerialPort.h"


WzReadSerialThread::WzReadSerialThread(QObject *parent):QObject(parent)
{
    qDebug()<<"worker()";
}

WzReadSerialThread::~WzReadSerialThread()
{
    readFlag = 0;
    qDebug()<<"~worker()";
}

//read serial thread
void WzReadSerialThread::slot_dowork(WzSerialPort *pObj)
{
    int len, ret;
    len = 256;

    qDebug()<< "do work,thread id000 = " << QThread::currentThreadId();
    while(readFlag){
        ret = pObj->receive(buf, len);
        if(ret > 0){
            qDebug()<< "do work,thread buf:" << buf << ", ret:" << ret;
            emit sig_recv_data(buf);
        }
        //qDebug()<< "do work,thread id = " << QThread::currentThreadId() << ", readflag:" << readFlag << ", len:" << len;
        //QThread::msleep(1000);
    }
    emit sig_finish();
}
