/*
 ** $Id$
 **
 ** triangledaemon.cpp: 
 **
 ** Copyright (C) 2003 ~ 2010 Beijing Feynman Software Technology Co., Ltd. 
 ** 
 ** All rights reserved by Feynman Software.
 **   
 ** Current maintainer: lvlei 
 **  
 ** Create date: 2010-08-13 
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
#include <signal.h>

#define TRIANGLEPATHNAME "/tmp/triangle"

int main(int argc, char** argv) 
{
   unlink(TRIANGLEPATHNAME);
   mkfifo(TRIANGLEPATHNAME, 0700);
   int pid = -1;
   char **childArgv;
   if (argc < 0)
       argc = 0;

   childArgv = (char **)malloc(argc+1);
    childArgv [0] = "triangle";
    for (int i= 1; i< argc + 1; i++) {
        childArgv [i] = argv[i];
    }
   if ((pid = fork()) == 0) {
       if(execv("./triangle", childArgv))
           perror("Error: execute triangle");
   }
   else 
   {
       int fd = open(TRIANGLEPATHNAME, O_NONBLOCK |  O_RDWR);
       char buff[100];
       if (fd <= 0) {
           perror("open file failed\n");
           exit(0);
       }

#if 0
       //Product process
       if (fork() == 0) {
           unsigned long long count = 0;
           while(1) {
               sprintf(buff, "Hello World %lld", count);
               fprintf(stderr, "%s\n", buff);
               count++;
               write(fd, buff, strlen(buff));
               usleep(50000);
           }
       }
#endif
       //
       unsigned int intervalMaxTimeInMin = 5;
       struct timeval timeout;
       timeout.tv_sec = 60 * 5; //default 5 min
       timeout.tv_usec =0;
       int max_fd = fd +1;
       fd_set rset;
       FD_ZERO(&rset);

       int ret;
       while(1) {
           FD_SET(fd, &rset);
           timeout.tv_sec = (intervalMaxTimeInMin * 60) ;
           timeout.tv_usec = 0;
            ret = select(max_fd, &rset, NULL, NULL, &timeout);
            if (ret < 0) {
                 if (errno == EINTR)
                     continue;
                 else 
                     fprintf(stderr, "Error: select failed\n");
            }
            else if (ret == 0) { //time out 
                printf("time out\n");
#if 1
                if(kill(pid, SIGTERM) == 0) {
                    sleep(5); // sleep 5 seconds
                    if ((pid = fork()) == 0) {
                        if(execv("./triangle", childArgv))
                            perror("Error: execute triangle");
                    }
                }
#endif
                continue;
            }
            else  {
                if (FD_ISSET(fd, &rset)) {
                    FD_CLR(fd, &rset);
                    size_t readCount = read(fd, buff, 10);
                    if(readCount != 10){
                        printf("error msg\n");
                        continue;
                    }
                    char *pos = strchr(buff, '\t');
                    char tmp[5] = {0};
                    strncpy(tmp, buff+5, pos - buff - 4);
                    int msgLen = atoi(tmp);

                    switch(buff[2]) {
                        case    's': {
                            char *msg = (char *)malloc(msgLen +1);   
                            bzero(msg, msgLen+1);
                            ret = read(fd, msg, msgLen); 
                            if (ret < 0) {
                                fprintf(stderr, "Daemon: Get intervalMaxTime value from triangle failed\n");
                                break;
                            }
                            intervalMaxTimeInMin = atoi(msg);
                           }
                            break;
                        case    'c':
                            break;
                        default:
                            break;
                    }
                }
            }
       }
   }
   
}
