/*
 ** $Id$
 **
 ** TriangleLogging.h: 
 **
 ** Copyright (C) 2003 ~ 2010 Beijing Feynman Software Technology Co., Ltd. 
 ** 
 ** All rights reserved by Feynman Software.
 **   
 ** Current maintainer: lvlei 
 **  
 ** Create date: 2010-08-10 
 */
#ifndef TriangleLogging_h
#define TriangleLogging_h

#include <stdarg.h>
#include <iostream>
using std::string;

class TriangleLogging {
public: 
    TriangleLogging() {}
    virtual ~TriangleLogging() {}

    virtual bool init(const char* etcFile) = 0;
    virtual void plainDebug(const char* format, ...)=0;
    virtual void debug(const char* format, ...)= 0;
    virtual void error(const char* format, ...)= 0;
    virtual void coredump(const char* str)=0;
    virtual void bmp()=0;
    virtual void meminfo()=0;
    virtual bool finish()=0;
    //FIXME: flush???
    string timedate(); 
    string formatString(const char* format, va_list *args); 
    string sysMemInfo();
    int parseLine(char *line); 

protected:
    string debugOutput(const char* format, va_list *args);
    string errorOutput(const char* format, va_list *args);
};

class LoggingShell : public TriangleLogging {
public:
    LoggingShell();
    ~LoggingShell() {}
    bool init(const char* etcFile);
    void plainDebug(const char* format, ...);
    void debug(const char* format, ...);
    void error(const char* format, ...);
    void coredump(const char* str);
    void bmp();
    void meminfo();
    bool finish();
private:
     char *m_fileName;
};

class LoggingFile : public TriangleLogging {
public:
    LoggingFile();
    ~LoggingFile();
    bool init(const char* etcFile);
    void plainDebug(const char* format, ...);
    void debug(const char* format, ...);
    void error(const char* format, ...);
    void coredump(const char* str);
    void bmp();
    void meminfo();
    bool finish();
private:
    int createNewFile();
    int appendFile(const char* str, int length);
    size_t fileSize();
private:
    int m_fileSize;
    char *m_fileDir;
    char *m_fileName;
};

class LoggingSocket : public TriangleLogging {
public:
    LoggingSocket();
    ~LoggingSocket() {}
    bool init(const char* etcFile);
    void plainDebug(const char* format, ...);
    void debug(const char* format, ...);
    void error(const char* format, ...);
    void coredump(const char* str);
    void bmp();
    void meminfo();
    bool finish();
private:
    char *m_ip;
};

class TriangleLoggingFactory {
public:
    static TriangleLogging* createLoggingInstance(const char* etcFile);
};
#endif 
