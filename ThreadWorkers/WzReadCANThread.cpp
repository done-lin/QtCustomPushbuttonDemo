#include "QObject"
#include "can/WzCan.h"
#include "WzReadCANThread.h"


WzReadCANThread::WzReadCANThread(QObject *parent):QObject(parent)
{
    qDebug()<<"worker()";
}

WzReadCANThread::~WzReadCANThread()
{
    readFlag = 0;
    qDebug()<<"~worker()";
}

//read serial thread
void WzReadCANThread::slot_dowork(WzCan *pObj)
{
    int len;
    QString str;
    qDebug()<< "CAN do work,thread id = " << QThread::currentThreadId();
    while(readFlag){
        pObj->receive(buf, &len);
        if(len > 0){
            qDebug()<< "do work,thread buf:" << buf;
            emit sig_recv_data(buf, len);
        }
        //qDebug()<< "CAN do work,thread id = " << QThread::currentThreadId();
        //QThread::msleep(1000);
    }
    emit sig_finish();
}
