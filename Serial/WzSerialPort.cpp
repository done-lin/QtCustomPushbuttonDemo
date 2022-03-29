#include "WzSerialPort.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>    
#include <string.h>
#include <unistd.h>    
#include <sys/types.h>  
#include <sys/stat.h>   
#include <fcntl.h>      
#include <termios.h>  
#include <errno.h>
#include <pthread.h>

WzSerialPort::WzSerialPort()
{
    pHandle[0] = -1;
}

WzSerialPort::~WzSerialPort()
{
    pHandle[0] = -1;
}

bool WzSerialPort::open(const char* portname, int baudrate, char parity, char databit, char stopbit, char synchronizeflag)
{
    // 打开串口
    // 以 读写、不阻塞 方式打开
    pHandle[0] = ::open(portname,O_RDWR|O_NOCTTY|O_NONBLOCK);
    
    // 打开失败，则打印失败信息，返回false
    if(pHandle[0] == -1)
    {
        std::cout << portname << " open failed , may be you need 'sudo' permission." << std::endl;
        return false;
    }else{
        std::cout << portname << " open ok , handle: " << pHandle[0] << std::endl;
    }

    // 设置串口参数
    // 创建串口参数对象
    struct termios options;
    // 先获得串口的当前参数
    if(tcgetattr(pHandle[0],&options) < 0)
    {
        std::cout << portname << " open failed , get serial port attributes failed." << std::endl;
        return false;
    }

    // 设置波特率
    switch(baudrate)
    {
        case 2400:
            cfsetispeed(&options,B2400);
            cfsetospeed(&options,B2400);
        break;
        case 4800:
            cfsetispeed(&options,B4800);
            cfsetospeed(&options,B4800);
            break;
        case 9600:
            cfsetispeed(&options,B9600);
            cfsetospeed(&options,B9600);
            break;   
        case 19200:
            cfsetispeed(&options,B19200);
            cfsetospeed(&options,B19200);
            break;
        case 38400:
            cfsetispeed(&options,B38400);
            cfsetospeed(&options,B38400);
            break;
        case 57600:
            cfsetispeed(&options,B57600);
            cfsetospeed(&options,B57600);
            break;
        case 115200:
            cfsetispeed(&options,B115200);
            cfsetospeed(&options,B115200);
            break;
        default:
            std::cout << portname << " open failed , unkown baudrate , only support 2400,4800,9600,19200,38400,57600,115200." << std::endl;
            return false;
    }

    // 设置校验位
    switch(parity)
    {
        // 无校验
        case 0:
            options.c_cflag &= ~PARENB;//PARENB：产生奇偶位，执行奇偶校验
            options.c_cflag &= ~INPCK;//INPCK：使奇偶校验起作用
            break;
        // 设置奇校验
        case 1:
            options.c_cflag |= PARENB;//PARENB：产生奇偶位，执行奇偶校验
            options.c_cflag |= PARODD;//PARODD：若设置则为奇校验,否则为偶校验
            options.c_cflag |= INPCK;//INPCK：使奇偶校验起作用
            options.c_cflag |= ISTRIP;//ISTRIP：若设置则有效输入数字被剥离7个字节，否则保留全部8位
            break;
        // 设置偶校验
        case 2:
            options.c_cflag |= PARENB;//PARENB：产生奇偶位，执行奇偶校验
            options.c_cflag &= ~PARODD;//PARODD：若设置则为奇校验,否则为偶校验
            options.c_cflag |= INPCK;//INPCK：使奇偶校验起作用
            options.c_cflag |= ISTRIP;//ISTRIP：若设置则有效输入数字被剥离7个字节，否则保留全部8位
            break;
        default:
            std::cout << portname << " open failed , unkown parity ." << std::endl;
            return false;
    }

    // 设置数据位
    switch(databit)
    {
        case 5:
            options.c_cflag &= ~CSIZE;//屏蔽其它标志位
            options.c_cflag |= CS5;
            break;
        case 6:
            options.c_cflag &= ~CSIZE;//屏蔽其它标志位
            options.c_cflag |= CS6;
            break;
        case 7:
            options.c_cflag &= ~CSIZE;//屏蔽其它标志位
            options.c_cflag |= CS7;
            break;
        case 8:
            options.c_cflag &= ~CSIZE;//屏蔽其它标志位
            options.c_cflag |= CS8;
            break;
        default:
            std::cout << portname << " open failed , unkown databit ." << std::endl;
            return false;
    }

    // 设置停止位
    switch(stopbit)
    {
        case 1:
            options.c_cflag &= ~CSTOPB;//CSTOPB：使用1位停止位
            break;
        case 2:
            options.c_cflag |= CSTOPB;//CSTOPB：使用2位停止位
            break;
        default:
            std::cout << portname << " open failed , unkown stopbit ." << std::endl;
            return false;
    }

    options.c_cflag |= CLOCAL;////忽略clocal
    options.c_cflag |= CREAD;////打开接受者

    options.c_cc[VINTR]=0;
    options.c_cc[VQUIT]=0;
    options.c_cc[VERASE]=0;
    options.c_cc[VKILL]=0;
    options.c_cc[VEOF]=0;
    options.c_cc[VSWTC]=0;
    options.c_cc[VSTART]=0;
    options.c_cc[VSTOP]=0;
    options.c_cc[VSUSP]=0;
    options.c_cc[VEOL]=0;
    options.c_cc[VREPRINT]=0;
    options.c_cc[VDISCARD]=0;
    options.c_cc[VWERASE]=0;
    options.c_cc[VLNEXT]=0;
    options.c_cc[VEOL2]=0;

    options.c_iflag |= IGNPAR;//忽略错误帧和奇偶错误
    //软件流控制屏蔽，CR-NL的映射，即串口能把回车和换行当成同一个字符，可以进行如下设置屏蔽之：
    //options.c_iflag &= ~(ICRNL|ISTRIP|IXON|IGNCR);//不要回车,关闭xon流控制，不要忽略回车信息，不要把回车信息翻译成特殊信息
    options.c_iflag &= ~(ICRNL|IXON);
    //回车，换行的屏蔽
    options.c_lflag &= ~(ICANON|ECHO|ECHOE|ISIG);//non ICANON。取消收到特殊字符的新号和处理，非标准模式
    ///////////////////////
    options.c_oflag &= ~(ONLCR|OCRNL);
    options.c_oflag &= ~OPOST;

///////////////////////////////////
    options.c_cc[VTIME] = 10;// wait for 1 ms
    options.c_cc[VMIN] = 0;//read 8 bytes in,串口读入个数，如果是0,表示一旦亦数据马上返回

    // 激活新配置
    if((tcsetattr(pHandle[0],TCSANOW,&options))!=0)
    {
        std::cout << portname << " open failed , can not complete set attributes ." << std::endl;
        return false;
    }
    tcflush(pHandle[0],TCIOFLUSH);

    return true;
}

void WzSerialPort::close()
{
    if(pHandle[0] != -1)
    {
        ::close(pHandle[0]);
        pHandle[0] = -1;
    }
}

int WzSerialPort::send(const void *buf,int len)
{
    int sendCount = 0;

    std::cout << " handle is: " << pHandle[0] << std::endl;
    if(pHandle[0] != -1)
    {
        // 将 buf 和 len 转换成api要求的格式
        const char *buffer = (char*)buf;
        size_t length = len;
        // 已写入的数据个数
        ssize_t tmp;

        while(length > 0)
        {
            if((tmp = write(pHandle[0], buffer, length)) <= 0)
            {
                if(tmp < 0&&errno == EINTR)
                {
                    tmp = 0;
                }
                else
                {
                    break;
                }
            }
            length -= tmp;
            buffer += tmp;
        }

        sendCount = len - length;
    }
   
    return sendCount;
}

int WzSerialPort::receive(void *buf,int maxlen)
{
    int receiveCount = ::read(pHandle[0], buf, maxlen);
    if(receiveCount < 0)
    {
        receiveCount = 0;
    }
    //std::cout << " rec handle: " << pHandle[0] << std::endl;
    return receiveCount;
}

bool WzSerialPort::isOpen()
{
    if(pHandle[0] != -1){
        std::cout << " open is true : " << pHandle[0] << std::endl;
        return true;
    }else{
        std::cout << " open is false : " << pHandle[0] << std::endl;
        return false;
    }
}


