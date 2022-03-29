#include "WzCan.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>    
#include <string.h>
#include <unistd.h>    
#include <sys/types.h>  
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>      
#include <errno.h>
#include "linux/can.h"
#include <pthread.h>


WzCan::WzCan()
{
    pHandle[0] = -1;
}

WzCan::~WzCan()
{
    pHandle[0] = -1;
}

bool WzCan::open(const char* portname, int baudrate, char synchronizeflag)
{
    struct ifreq ifr;
    int ret;

    //init fdx
    for(ret = 0; ret<4; ret++){
        pHandle[ret] = -1;
    }


    /* Opening device */
    pHandle[0] = socket(PF_CAN,SOCK_RAW,CAN_RAW);

    if(pHandle[0]==-1)
    {
        printf("Can open failed.");
        return   0;
    }


    strcpy((char *)(ifr.ifr_name), portname);
    ioctl(pHandle[0],SIOCGIFINDEX,&ifr);


    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    bind(pHandle[0],(struct sockaddr*)&addr,sizeof(addr));

    return pHandle[0];
}

void WzCan::close()
{
    if(pHandle[0] > 0) {
        ::close(pHandle[0]);
        }
    pHandle[0]=-1;

}

int WzCan::send(int canID, const void *buf,int len)
{
    int nbytes;
    struct can_frame frame;
    memset(&frame,0,sizeof(struct can_frame));

    printf("canid: %d, count: %d", canID, len);

    if(canID < 2048) {
        frame.can_id = canID;
    }
    else if((canID >= 2048) && (canID <= 536870911)) {
        frame.can_id = 0x80000000U | canID;
    }else{
        printf("CanID Error: %u\n", canID);
        return -1;
    }

    frame.can_dlc = len;

    memcpy((char *)frame.data, buf, frame.can_dlc);

    //printf("can_id: %d, can_dlc: %d, data: %s", frame.can_id, frame.can_dlc, frame.data);

    nbytes = sendto(pHandle[0], &frame, sizeof(struct can_frame), 0, (struct sockaddr*)&addr, sizeof(addr));

    //printf("nbytes: %d, canfd: %d", nbytes, canfd);

    return nbytes;
}

int WzCan::receive(void *buf, int *actualLen)
{
    unsigned long nbytes;
    socklen_t len;

    struct can_frame frame = {0};
    int k=0;

    char temp[16];

    fd_set rfds;
    int retval;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000;

    bzero(temp,16);
    if(pHandle[0]<=-1){
        printf("Can not Open.\n");
        frame.can_id=0;
        frame.can_dlc=0;
    }else{
        FD_ZERO(&rfds);
        FD_SET(pHandle[0], &rfds);
        retval = select(pHandle[0]+1 , &rfds, NULL, NULL, &tv);
        if (retval == -1){
            printf("CAN slect fd error");
        }else if (retval == 0){
            printf("Can Read timeout");
        }else{
            nbytes = recvfrom(pHandle[0], &frame, sizeof(struct can_frame), 0, (struct sockaddr *)&addr,&len);
            for(k = 0;k < frame.can_dlc;k++){
                temp[k] = frame.data[k];
            }
            frame.can_id = frame.can_id & CAN_EFF_MASK;//支持扩展帧,标准帧11bit,扩展29bit
            //LOGD("Can Read slect success.");
        }
        *actualLen = frame.can_dlc;
        memcpy(buf, temp, frame.can_dlc);
    }
    return frame.can_id;

}

bool WzCan::isOpen()
{
    if(pHandle[0] != -1){
        std::cout << " can is open : " << pHandle[0] << std::endl;
        return true;
    }else{
        std::cout << " can is closed : " << pHandle[0] << std::endl;
        return false;
    }
}


