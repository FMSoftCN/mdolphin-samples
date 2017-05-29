/*
 ** $Id$
 **
 ** URLDepository.cpp: 
 **
 ** Copyright (C) 2003 ~ 2010 Beijing Feynman Software Technology Co., Ltd. 
 ** 
 ** All rights reserved by Feynman Software.
 **   
 ** Current maintainer: lvlei 
 **  
 ** Create date: 2010-08-09 
 */

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include "URLDepository.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

URLMemDepository::URLMemDepository()
{
    m_isAllReadMemory = true;
}

bool URLMemDepository::init(const char* etcFile) 
{
    char buff[51];

    if (GetValueFromEtcFile (etcFile, "fileinput", "filename", buff, 50) ==  ETC_OK) {
        m_fileName = buff;
    }

    int optype = 1;
    if (GetIntValueFromEtcFile (etcFile, "fileinput", "optype", &optype) ==  ETC_OK) {
        m_isAllReadMemory = (optype != 0);
    }

    ifstream infile(m_fileName.c_str());
    if (!infile)
        return false;

    if (!m_isAllReadMemory) 
        m_FileAccessIndex.push_back(0);

    string url;
    while(getline(infile, url).good()) {
        if (url.size() <= 0)
            continue;
        if (m_isAllReadMemory) {
            char *curl = static_cast<char *>(malloc(url.size() + 1));
            strcpy(curl, url.c_str());
            m_URLItems.push_back(URLItem(false, curl));
        }
        else {
            int pos = infile.tellg() ;
            m_FileAccessIndex.push_back(pos);
        }
    }

    infile.close(); 
}

void URLMemDepository::finish()
{

}

bool URLMemDepository::isEmpty()
{
    return m_URLItems.empty();
}

size_t URLMemDepository::size()
{
    if (m_isAllReadMemory) 
        return m_URLItems.size();
    else 
        return m_FileAccessIndex.size();
}

const URLItem* URLMemDepository::findAt(int index)
{
    if (index <0  || index > size())
        return NULL;

    if (m_isAllReadMemory) {
        return &m_URLItems[index];
    }
    else {
        ifstream infile(m_fileName.c_str());
        if (!infile)
            return NULL;
        
        int pos = m_FileAccessIndex[index];
        infile.seekg(pos);
        string url;
        if (getline(infile, url).good()) {
            static URLItem _URLItemResult; //FIXME: static variable
            char* curl = NULL;
            if (_URLItemResult.url)
                free(_URLItemResult.url);

            curl = (char *)malloc(url.size()+1);
            bzero(curl, url.size()+1);
            strcpy(curl, url.c_str());
            _URLItemResult.url = curl;
            return &_URLItemResult;
        }
    }
    return NULL;
}

bool URLMemDepository::append(const URLItem* item)
{
    if (m_isAllReadMemory) {
        m_URLItems.push_back(URLItem(*item));
    }
}
