/*
 ** $Id$
 **
 ** fifo.cpp: 
 **
 ** Copyright (C) 2003 ~ 2010 Beijing Feynman Software Technology Co., Ltd. 
 ** 
 ** All rights reserved by Feynman Software.
 **   
 ** Current maintainer: lvlei 
 **  
 ** Create date: 2010-08-16 
 */

#include "fifo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

bool existFIFO()
{
    struct stat statBuf;
    stat(TRIANGLEPATHNAME, &statBuf);
    if (S_ISFIFO(statBuf.st_mode))
        return true;
    return false;
}

int createFIFO()
{
    deleteFIFO();
    return  mkfifo(TRIANGLEPATHNAME, 0700);
}

void deleteFIFO() 
{
   unlink(TRIANGLEPATHNAME);
}

int sendSetIntervalMessage(int interval)
{
    char buf[6]={0};

    snprintf(buf, 5, "%d",interval);
    int len = strlen(buf);
    char headBuffer[]= "M=0L=0\t\t\t\t";
    headBuffer[2]='s';
    headBuffer[5]=len + '0';
    char *sendStr = (char *)malloc(strlen(headBuffer) + strlen(buf) + 1);
    strcpy(sendStr, headBuffer);
    strncat(sendStr, buf, 5);
    sendMessageToFIFO(sendStr);
    free(sendStr);
    return 0;

}

int sendClearMessage()
{
    char headBuffer[]= "M=0L=0\t\t\t\t";
    headBuffer[2]='c';
    sendMessageToFIFO(headBuffer);
    return 0;
}

int sendMessageToFIFO(const char *str) 
{
    int fd = open(TRIANGLEPATHNAME, O_NONBLOCK |  O_WRONLY);
    if (fd <= 0) {
        perror("open file failed\n");
        return -1;
    }

    write(fd, str, strlen(str));
    close(fd);
    return 0;
}


