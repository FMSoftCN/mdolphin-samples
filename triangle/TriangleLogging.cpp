/*
 ** $Id$
 **
 ** TriangleLogging.cpp: 
 **
 ** Copyright (C) 2003 ~ 2010 Beijing Feynman Software Technology Co., Ltd. 
 ** 
 ** All rights reserved by Feynman Software.
 **   
 ** Current maintainer: lvlei 
 **  
 ** Create date: 2010-08-10 
 */

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include "TriangleLogging.h"

using namespace std;

string TriangleLogging::timedate()  
{
    time_t curTime; 
    time(&curTime);

   struct tm *localTime;
   localTime = localtime(&curTime);

   stringstream stream;
   stream << localTime->tm_mon<<"/"
          << (localTime->tm_mday+1)<<"/" 
          << (localTime->tm_year+1900)<<"--" 
          << localTime->tm_hour<<":" 
          << localTime->tm_min<<":" 
          << localTime->tm_sec;

    string result;
    stream >> result;
    return result; 
}

string TriangleLogging::debugOutput(const char* format, va_list *args)
{
    string message;
    message = "DEBUG:";
    message.append("   ");
    message += timedate();
    message.append("   ");

    message.append(formatString(format, args));

    return message;
}

string TriangleLogging::errorOutput(const char* format, va_list *args)
{
    string message;
    message = "ERROR:";
    message.append("   ");
    message += timedate();
    message.append("   ");

    message.append(formatString(format, args));

    return message;
}

string TriangleLogging::formatString(const char* format, va_list *args)
{
    static char buffer[1024];
    vsprintf(buffer, format, *args);

    string str(buffer);
    return str;
}


int TriangleLogging::parseLine(char *line)
{
    int i = strlen(line);
    while (*line < '0' || *line > '9') line++;
    line[i-3] = '\0';
    i = atoi(line);
    return i;
}

string TriangleLogging::sysMemInfo() 
{
    FILE* file = fopen("/proc/self/status", "r");
    int physMem = -1;
    int virtualMem = -1;
    char line[128];

    while (fgets(line, 128, file) != NULL){
        if (strncmp(line, "VmRSS:", 6) == 0) {
            physMem = parseLine(line);
        }
        else if (strncmp(line, "VmSize:", 7) == 0) {
            virtualMem = parseLine(line);
        }

        if ((physMem != -1) && (virtualMem != -1))
            break;
    }

    fclose(file);

    stringstream stream;
    string result;

    //FIXME: how to use white-sapce ??
    //stream << "MEMINFO:" << "Virtual-MemSize:"<< virtualMem << "KB" <<"Phys-MemSize:"<< physMem  << "KB" << " ";
    stream << "MEMINFO:";
    stream >> result;
    result.append("   ");
    stream.clear();

    stream.str("");
    stream << "Virtual-MemSize:"<< virtualMem << "KB";
    result.append(stream.str());
    result.append("   ");
    stream.clear();

    stream.str("");
    stream << "Phys-MemSize:"<< physMem  << "KB";
    result.append(stream.str());
    result.append("   ");
    stream.clear();
    return result;
}

LoggingShell::LoggingShell() 
{

}

bool LoggingShell::init(const char* etcFile)
{
    return true;
}

void LoggingShell::plainDebug(const char* format, ...)
{
    string message;
    va_list args;
    va_start(args, format);
    message = formatString(format, &args);
    va_end(args);

    fprintf(stdout, "%s", message.c_str());
}

void LoggingShell::debug(const char* format, ...)
{
    string message;
    
    va_list args;
    va_start(args, format);
    message = debugOutput(format, &args);
    va_end(args);

    fprintf(stdout, "%s", message.c_str());
}

void LoggingShell::error(const char* format, ...)
{
    string message;
    
    va_list args;
    va_start(args, format);
    message = errorOutput(format, &args);
    va_end(args);

    fprintf(stderr, "%s", message.c_str());
}

void LoggingShell::coredump(const char* str)
{

}

void LoggingShell::bmp()
{

}

void LoggingShell::meminfo()
{
    string message;
    message = sysMemInfo();

    fprintf(stdout, "%s\n", message.c_str());
}

bool LoggingShell::finish()
{
    return true;
}


LoggingFile::LoggingFile()
    :m_fileSize(-1)
    ,m_fileDir(0)
    ,m_fileName(0)
{
    
}

LoggingFile::~LoggingFile() 
{
    free(m_fileDir);
    free(m_fileName);
}

bool LoggingFile::init(const char* etcFile)
{
    m_fileSize = 8;  //default 8M
    if (GetIntValueFromEtcFile (etcFile, "fileoutput", "filesize", &m_fileSize) !=  ETC_OK) {
        fprintf(stderr, "LoggingFile: Get fileSize failed !\n");
        m_fileSize = 8;  //default 8M
    }

    m_fileSize = ((unsigned int )m_fileSize) << 20; //convert to byte

    char buff[51];
    bzero(buff,51); 
    if (GetValueFromEtcFile (etcFile, "fileoutput", "outputdir", buff, 50) ==  ETC_OK) {
        if (strlen(buff)) {
            m_fileDir =(char *)malloc(strlen(buff) + 1);
            strcpy(m_fileDir, buff);
        }
    }
    else 
        m_fileDir = NULL;

    if (!createNewFile())
        return true;

    return false;
}

void LoggingFile::plainDebug(const char* format, ...)
{
    string message;
    va_list args;
    va_start(args, format);
    message = formatString(format, &args);
    va_end(args);

    appendFile(message.c_str(), message.size()); 
    if ((int)fileSize() > m_fileSize) {
        createNewFile();
    }
}

void LoggingFile::debug(const char* format, ...)
{
    string message;
    
    va_list args;
    va_start(args, format);
    message = debugOutput(format, &args);
    va_end(args);

    appendFile(message.c_str(), message.size()); 

    if ((int)fileSize() > m_fileSize) {
        createNewFile();
    }
}

void LoggingFile::error(const char* format, ...)
{
    string message;
    
    va_list args;
    va_start(args, format);
    message = errorOutput(format, &args);
    va_end(args);

    appendFile(message.c_str(), message.size()); 

    if ((int)fileSize() > m_fileSize) {
        createNewFile();
    }
}

void LoggingFile::coredump(const char* str)
{

}

void LoggingFile::bmp()
{

}

void LoggingFile::meminfo()
{
    string message;
    message = sysMemInfo();

    message.append("\n");
    appendFile(message.c_str(), message.size()); 

    if ((int)fileSize() > m_fileSize) {
        createNewFile();
    }
}

bool LoggingFile::finish()
{
    return true;
}

int LoggingFile::createNewFile()
{
    string fileName = timedate();
    for (int i = 0; i< fileName.size(); i++) {
        if (fileName[i] == '/' || fileName[i] == ':')
            fileName[i] = '-';
    }

    string filePath;
    if (m_fileDir) {
        filePath = m_fileDir; 
        filePath.append("/");
    }

    if (!filePath.empty()) {
        string createDirCMD = "mkdir -p ";
        createDirCMD += filePath;
        int ret = system(createDirCMD.c_str());
        if (ret == -1 || ret == 127) {
            perror(createDirCMD.c_str());
            return -1;
        }
    }

    filePath += fileName;
    ofstream outFile;
    outFile.open(filePath.c_str(), ios_base::out);
    if (!outFile ) {
        perror(filePath.c_str());
        return -1;
    }
    outFile.close();

    m_fileName = (char *)malloc(filePath.size() + 1);
    strcpy(m_fileName, filePath.c_str());
    return 0;
}

int LoggingFile::appendFile(const char* str, int length)
{
   ofstream outFile(m_fileName, ofstream::out | ofstream::app); 
   if (!outFile)
       return -1;

   outFile << str ;
   outFile.close();
   return 0;
}

size_t LoggingFile::fileSize()
{
    size_t size = 0;
    ifstream inFile;
    inFile.open(m_fileName);
    if(!inFile.good())
        return 0;
    inFile.seekg(0, ios_base::end);
    size = inFile.tellg();
    inFile.close();
    return size;
}

LoggingSocket::LoggingSocket() 
{

}

bool LoggingSocket::init(const char* etcFile)
{

}

void LoggingSocket::plainDebug(const char* format, ...)
{
}

void LoggingSocket::debug(const char* format, ...)
{

}

void LoggingSocket::error(const char* format, ...)
{

}

void LoggingSocket::coredump(const char* str)
{

}

void LoggingSocket::bmp()
{

}

void LoggingSocket::meminfo()
{

}

bool LoggingSocket::finish()
{

}

TriangleLogging* TriangleLoggingFactory::createLoggingInstance(const char* etcFile) 
{
    char outputValue[51];
    if (GetValueFromEtcFile (etcFile, "global", "output", outputValue, 50) !=  ETC_OK) {
        fprintf(stderr, "Get output value failed from %s\n", etcFile);
        return NULL;
    }

    if (!strcmp(outputValue, "shelloutput"))
        return new LoggingShell();
    else if (!strcmp(outputValue, "fileoutput"))
        return new LoggingFile();
    else if (!strcmp(outputValue, "networkoutput"))
        return new LoggingSocket();
    else return NULL;
}


