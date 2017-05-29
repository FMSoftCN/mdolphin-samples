#include <stdio.h>
#include <string.h>
#include "sqlite3.h"
#include "mdolphin_app.h"
#include "mdolphin_urldatabase.h"

#define DBNAME    "spider.db"
#define DBTABLE   "myurl"
#define LOGFILE   "spider.log"

#define SQL_MAXLEN 512
#define URL_MAXLEN 256

sqlite3 * pdb = NULL; 
FILE *logfp = NULL;

//global share sql variable
char sql[SQL_MAXLEN] = {0};

static BOOL init_log();
static BOOL init_db();
static void parse_url(char* newurl, const char* url);

static BOOL init_urldb()
{
	static	BOOL isInited = FALSE;
	if(isInited)
		return TRUE;

	if (!init_log())
		return FALSE;
	if (!init_db())
		return FALSE;

	isInited = TRUE;
	return TRUE;
}


void term_urldb()
{
	if(pdb)
		sqlite3_close(pdb);
	pdb = NULL;

	if(logfp)
		fclose(logfp);
	logfp = NULL;
}

void destroy_db()
{
	term_urldb();
}

static BOOL init_log()
{
    logfp  = fopen(LOGFILE,"a+");
    if(logfp == NULL){
        fprintf(stderr, "open log file error!\n");

		//!!!!!! we will exit !!!!!!//
		exit(0);
        return FALSE;
    }
	return TRUE;
}

static BOOL init_db()
{
	if(pdb)
		return TRUE;

	char **result;
	char *perrmsg;
	int nrow, ncol;

	//open db
	if( sqlite3_open(DBNAME, &pdb ) != SQLITE_OK ) {
		saveLogmsg("open database fail");
		exit(0);
		return FALSE ;
	}


	// Are there DBTABLE ? if not, create it.
	memset(sql, 0, SQL_MAXLEN);
	sprintf(sql,"select * from %s limit 1", DBTABLE);

	if( sqlite3_get_table(pdb, sql, &result, &nrow, &ncol, &perrmsg) != SQLITE_OK)
	{
		//create the table
		memset(sql, 0, SQL_MAXLEN);
		sprintf(sql, "create table %s(url,flag)", DBTABLE);

		//exec the create table sql
		if( sqlite3_exec(pdb, sql, 0, 0, &perrmsg) != SQLITE_OK )
		{
			saveLogmsg("create table error!");
			term_urldb();

			//!!!!! can not create the table ,we will exit the app!!!!!//
			exit(0);
			return FALSE;
		}
	}

	sqlite3_free_table(result);
	return TRUE;
}

static void parse_url(char* newurl, const char* url)
{
	int new_len = 0;
	// get the root url
	const char *p = strchr(url,':');  //url like "http://www.baidu.com/hi/ny.html
	if(p)
		p = strchr(p+3, '/');   //get the first / except http://
	else                      //url like www.baidu.com/hi/my.html
		p = strchr(url,'/');

	if(p == NULL)               //url like www.baidu.com
		new_len = strlen(url);
	else
		new_len = p - url;

	if(new_len >= URL_MAXLEN)
		new_len	= URL_MAXLEN - 1;

	memcpy(newurl, url, new_len);
	newurl[new_len] = '\0';
}

void saveURL(const char* url)
{
	char myurl[URL_MAXLEN] = {0};
	int ret, nrow, ncol;
	char *perrmsg;
	char **result;

	if (!init_urldb())
		return ;

	parse_url(myurl, url);

	// select url from database, if not, insert it into database
	memset(sql, 0, SQL_MAXLEN);
	sprintf(sql,"select flag from %s where url=\"%s\" limit 1", DBTABLE, myurl);
	ret = sqlite3_get_table(pdb, sql, &result, &nrow, &ncol, &perrmsg);
	if (ret != SQLITE_OK) {
		//here we not close the db..., 
		//maybe some sqls are wrong, not the db error. 
		//and we write the sql to log file
		saveLogmsg("slecet flag  table error!, the sql is...");
        saveLogmsg(sql);
        return;
    }

	sqlite3_free_table(result);
	if (ret == SQLITE_OK && !nrow) {
		memset(sql, 0, SQL_MAXLEN);
		sprintf(sql,"insert into %s values(\"%s\",\"0\")", DBTABLE, myurl);
		if (sqlite3_exec(pdb, sql, 0, 0, &perrmsg) != SQLITE_OK) {
			//here we not close the db..., 
			//becase some sql is wrong, not the db error. 
			//and we write the sql to log file
			saveLogmsg("insert create table error!, the sql is...");
            saveLogmsg(sql);
		    return;
        }
	}
    return;
}

//return FALSE no url or will return TRUE
BOOL getURL(char* url)
{
	int ret, nrow, ncol;
	char* perrmsg;
	char **result;

	if (!init_urldb())
		return FALSE;

	// select url from databse 
	memset(sql, 0, SQL_MAXLEN);
	sprintf(sql,"select url from %s where flag=\"0\" limit 1", DBTABLE);
    ret = sqlite3_get_table(pdb, sql, &result, &nrow, &ncol, &perrmsg);
	if(ret != SQLITE_OK){
		saveLogmsg("slecet flag  table error!, the sql is...");
		saveLogmsg(sql);
		return FALSE;
	} 

    if(nrow) {
		strcpy(url,result[1]);
	} else {
		url[0] = '\0';
		sqlite3_free_table(result);
		return FALSE;
	}

	sqlite3_free_table(result);

	// update databse, set visited flag = 1
	memset(sql, 0, SQL_MAXLEN);
	sprintf(sql,"update %s set flag=\"1\" where url=\"%s\"", DBTABLE, url);
	if (sqlite3_exec(pdb, sql, 0, 0, &perrmsg) != SQLITE_OK) {
		saveLogmsg("select table error!, the sql is...");
		saveLogmsg(sql);
		return FALSE;
	}

	return TRUE;
}

// save error log to ./m_log file
void saveLogmsg(char *log) //(char* fmt,...)
{ 
	if (!init_urldb())
		return ;
	fwrite(log, 1, strlen(log), logfp);
	fwrite("\n",1, 1, logfp);
	fflush(logfp);
}
