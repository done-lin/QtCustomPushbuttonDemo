#ifndef _WZCAN_H
#define _WZCAN_H

#include <sys/socket.h>
#include <net/if.h>
#include "linux/can.h"
#include <assert.h>

/*
    作者：欧？？
    日期：2017-12-14
    类名：WZSerialPort
    用途：CAN读写
    示例：
        参考 main.cpp
*/

class WzCan
{
public:
    WzCan();
    ~WzCan();

    // 打开CAN,成功返回true，失败返回false
    bool open(const char* portname, int baudrate, char synchronizeflag=1);

    //关闭串口，参数待定
    void close();

    //发送数据或写数据，成功返回发送数据长度，失败返回0
    int send(int canID, const void *buf,int len);

    //接受数据或读数据，成功返回读取实际数据的长度，失败返回0
    int receive(void *buf, int *actualLen);

    bool isOpen();

protected:
    struct sockaddr_can addr;
    int pHandle[4];
private:
    char synchronizeflag;
};

#endif

