/*
 ** $Id$
 **
 ** URLStorage.cpp: 
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

#include <string.h>
#include <stdlib.h>
#include "URLStorage.h"
#include "URLDataBase.h"


URLAccessPolicy* URLStorage::createAccessPolicyInstance(const char* type)
{
    URLAccessType accessType = SeqAccess;    
    if (!strcmp(type, "seq"))
        accessType = SeqAccess;
    else if (!strcmp(type, "cyc"))
        accessType = CycAccess;
    else if (!strcmp(type, "random"))
        accessType = RandomAccess;

    return createAccessPolicyInstance(accessType);
}

URLAccessPolicy* URLStorage::createAccessPolicyInstance(URLAccessType accessType)
{
    if (accessType == SeqAccess)
        return new SeqURLAccessPolicy();
    else if(accessType == CycAccess)
        return  new CycURLAccessPolicy();
    else if (accessType == RandomAccess)
        return  new RandomURLAccessPolicy();
    else 
        return NULL;
}

URLDepository* URLStorage::createDepositoryInstance(const char *depoName)
{
    if(!depoName)
        return NULL;

    if (!strcmp(depoName, "fileinput"))
        return new URLMemDepository();

    if (!strcmp(depoName, "databaseinput"))
        return new URLDataBase;
}

URLStorage::URLStorage()
    :m_depository(0)
    ,m_policy(0)
    ,m_curPos(-1)
{

}

URLStorage::URLStorage(char *depoName, URLAccessType accessType)
{
    m_curPos = -1; 
    m_depository = URLStorage::createDepositoryInstance(depoName);

    m_policy =  URLStorage::createAccessPolicyInstance(accessType);
}

URLStorage::URLStorage(URLDepository* depository, URLAccessType accessType)
{
    m_curPos = -1; 
    m_depository = depository;
    m_policy =  URLStorage::createAccessPolicyInstance(accessType);
}


bool URLStorage::init(const char* etcFile) {

    char buff[51];

    if (GetValueFromEtcFile (etcFile, "global", "input", buff, 50) ==  ETC_OK) {
        m_depository = URLStorage::createDepositoryInstance(buff);
        m_depository->init(etcFile);
    }

    if (GetValueFromEtcFile (etcFile, buff, "type", buff, 50) ==  ETC_OK) {
        m_policy =  URLStorage::createAccessPolicyInstance(buff);
    }

    if (m_depository && m_policy)
        return true;
    else 
        return false;
}

