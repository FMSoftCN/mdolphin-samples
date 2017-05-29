#ifndef MDOLPHIN_URLDATABASE_H
#define MDOLPHIN_URLDATABASE_H

void saveURL(const char* url);
BOOL getURL(char* url);

//char* getURL(void);

void saveLogmsg(char *log);
void destroy_db();

#endif
