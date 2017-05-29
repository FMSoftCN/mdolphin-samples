/*
 ** $Id$
 **
 ** fifo.h: 
 **
 ** Copyright (C) 2003 ~ 2010 Beijing Feynman Software Technology Co., Ltd. 
 ** 
 ** All rights reserved by Feynman Software.
 **   
 ** Current maintainer: lvlei 
 **  
 ** Create date: 2010-08-16 
 */

#ifndef fifo_H
#define fifo_H

#include <stdio.h>

#define TRIANGLEPATHNAME "/tmp/triangle"

bool existFIFO();
int createFIFO();
void deleteFIFO();

int sendMessageToFIFO(const char *);
int praseMessage(const char *);
int sendSetIntervalMessage(int interval);
int sendClearMessage();

#endif
