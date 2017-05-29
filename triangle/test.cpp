/*
 ** $Id$
 **
 ** test.cpp: 
 **
 ** Copyright (C) 2003 ~ 2010 Beijing Feynman Software Technology Co., Ltd. 
 ** 
 ** All rights reserved by Feynman Software.
 **   
 ** Current maintainer: lvlei 
 **  
 ** Create date: 2010-08-16 
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>
#include <errno.h>

#define TRIANGLEPATHNAME "/tmp/triangle"

int main(int argc, char** argv) 
{
       FILE *fd = fopen(TRIANGLEPATHNAME, "w+");
       if (fd <= 0) {
           exit(0);
       }
        unsigned long long count = 0;
    while(1) {
        fprintf(fd, "Hello World %lld\n", count);
        count++;
        usleep(10000);
    }

}
