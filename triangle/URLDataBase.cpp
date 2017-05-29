/*
 ** $Id$
 **
 ** URLDataBase.cpp: 
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

#include <iostream>
#include <fstream>
#include "URLDataBase.h"

#define DBNAME    "triangle.db"
#define DBTABLE   "triangle"

#define SQL_MAXLEN 1024
#define URL_MAXLEN 256

URLItem URLDataBase::URLItemSelected((char *)malloc(URL_MAXLEN), 0);

bool URLDataBase::init(const char* etcFile)
{
    m_vectorListFinished = false;
    m_vectorCurPos = 0;
    char buff[51];

    if (GetValueFromEtcFile (etcFile, "databaseinput", "filename", buff, 50) ==  ETC_OK) {
        m_fileName = buff;
    }

    ifstream infile(m_fileName.c_str());
    if (infile) {
        string url;
        while(getline(infile, url).good()) {
            char *curl = static_cast<char *>(malloc(url.size() + 1));
            bzero(curl, url.size() + 1);
            strcpy(curl, url.c_str());
            m_URLItems.push_back(URLItem(false, curl));
        }

        infile.close(); 
    }
    else
        m_vectorListFinished = true;

	if( sqlite3_open(DBNAME, &m_pdb ) != SQLITE_OK ) {
        return false;
    }

    return createTableIfNeed(DBTABLE);

}

bool URLDataBase::createTableIfNeed(const char* tableName)
{

    char sql[SQL_MAXLEN];
	char **result;
	char *perrmsg;
	int nrow, ncol;
	// Are there tableName ? if not, create it.
	memset(sql, 0, SQL_MAXLEN);
	snprintf(sql, SQL_MAXLEN, "select * from %s limit 1", tableName);

	if( sqlite3_get_table(m_pdb, sql, &result, &nrow, &ncol, &perrmsg) != SQLITE_OK) {
		//create the table
		memset(sql, 0, SQL_MAXLEN);
		snprintf(sql, SQL_MAXLEN, "create table %s(url,flag)", tableName);

		//exec the create table sql
		if( sqlite3_exec(m_pdb, sql, 0, 0, &perrmsg) != SQLITE_OK ) {
			fprintf(stderr, "create table error!");
            if (m_pdb)
                sqlite3_close(m_pdb);

			return false;
		}
	}

	sqlite3_free_table(result);
	return true;
}

void URLDataBase::finish()
{
	if(m_pdb)
		sqlite3_close(m_pdb);
	m_pdb = NULL;
}

bool URLDataBase::isEmpty()
{

}

size_t URLDataBase::size()
{
    if (!m_vectorListFinished) 
        return 0;

    if(!m_pdb)
        return 0;

    char sql[SQL_MAXLEN];
	char **result;
	char *perrmsg;
	int nrow, ncol;
	memset(sql, 0, SQL_MAXLEN);
	snprintf(sql, SQL_MAXLEN , "select count(*) from %s where flag=\"0\" ", DBTABLE);

    int ret = sqlite3_get_table(m_pdb, sql, &result, &nrow, &ncol, &perrmsg);
	if(ret != SQLITE_OK){
		fprintf(stderr, "slecet count(*)  table error!, the sql is... %s", perrmsg);
		return 0;
	} 

    long size;
    if(nrow) {
        size = atol(result[ncol]);
    }

	sqlite3_free_table(result);
    return size;
}

const URLItem* URLDataBase::findAt(int index)
{
    if (!m_vectorListFinished) {
         const URLItem* item = &(m_URLItems[m_vectorCurPos]);
         m_vectorCurPos++;
         if (m_vectorCurPos >= m_URLItems.size())
            m_vectorListFinished = true;
         return item;
    }

    if(!m_pdb)
        return false;

    char sql[SQL_MAXLEN];
    if (!URLDataBase::URLItemSelected.url)
        URLDataBase::URLItemSelected.url = (char *)malloc(URL_MAXLEN);

    char *url = URLDataBase::URLItemSelected.url;
	memset(sql, 0, SQL_MAXLEN);
	char **result;
	char *perrmsg;
	int nrow, ncol;

	snprintf(sql, SQL_MAXLEN , "select url from %s where flag=\"0\" limit %d,1", DBTABLE, index);
    int ret = sqlite3_get_table(m_pdb, sql, &result, &nrow, &ncol, &perrmsg);
	if(ret != SQLITE_OK){
		fprintf(stderr, "slecet flag  table error!, the sql is... %s", perrmsg);
		return NULL;
	} 

    if(nrow) {
		strncpy(url,result[1], URL_MAXLEN);
	} else {
		url[0] = '\0';
		sqlite3_free_table(result);
		return NULL;
	}

	sqlite3_free_table(result);

	// update databse, set visited flag = 1
	memset(sql, 0, SQL_MAXLEN);
	snprintf(sql, SQL_MAXLEN,"update %s set flag=\"1\" where url=\"%s\"", DBTABLE, url);
	if (sqlite3_exec(m_pdb, sql, 0, 0, &perrmsg) != SQLITE_OK) {
		fprintf(stderr, "select table error!, the sql is...%s", *perrmsg);
		return NULL;
	}

    return &(URLDataBase::URLItemSelected);
}

bool URLDataBase::append(const URLItem* item)
{
    if(!m_pdb)
        return false;

	int ret, nrow, ncol;
	char *perrmsg;
	char **result;
    char sql[SQL_MAXLEN];
    char *url = item->url;
	// select url from database, if not, insert it into database
	memset(sql, 0, SQL_MAXLEN);
	snprintf(sql, SQL_MAXLEN, "select flag from %s where url=\"%s\" limit 1", DBTABLE, url);
	ret = sqlite3_get_table(m_pdb, sql, &result, &nrow, &ncol, &perrmsg);
	if (ret != SQLITE_OK) {
		//here we not close the db..., 
		//maybe some sqls are wrong, not the db error. 
		//and we write the sql to log file
		fprintf(stderr, "slecet flag  table error!, the sql is...\n");
        return false;
    }

	sqlite3_free_table(result);
	if (ret == SQLITE_OK && !nrow) {
		memset(sql, 0, SQL_MAXLEN);
		snprintf(sql, SQL_MAXLEN, "insert into %s values(\"%s\",\"0\")", DBTABLE, url);
		if (sqlite3_exec(m_pdb, sql, 0, 0, &perrmsg) != SQLITE_OK) {
			//here we not close the db..., 
			//becase some sql is wrong, not the db error. 
			//and we write the sql to log file
			fprintf(stderr, "insert create table error!, the sql is...\n");
		    return false;
        }
	}
    return true;
}
