/*
 * jwslog.c
 *
 *  Created on: Jul 10, 2014
 *      Author: zjp35
 */
#include "jwslog.h"
#define MAXLEVELNUM (3)
 
LOGSET logsetting;
LOG loging;
 
const static char LogLevelText[4][10]={"INF","DEBUG","ERROR","ERROR"};
 
static char * jwslog_getdate(char *date);
 
static unsigned char getcode(char *path)
{
    unsigned char code=255;
    if(strcmp("INF",path)==0)
        code=1;
    else if(strcmp("DEBUG",path)==0)
        code=2;
    else if(strcmp("ERROR",path)==0)
        code=4;
    else if(strcmp("NONE",path)==0)
        code=0;
    return code;
}
 
static unsigned char ReadConfig(char *path)
{
    char value[512]={0x0};
    char data[50]={0x0};
 
    FILE *fpath=fopen(path,"r");
    if(fpath==NULL)
        return -1;
    fscanf(fpath,"path=%s\n",value);
    jwslog_getdate(data);
    strcat(data,".log");
    strcat(value,"/");
    strcat(value,data);
    if(strcmp(value,logsetting.filepath)!=0)
        memcpy(logsetting.filepath,value,strlen(value));
    memset(value,0,sizeof(value));
 
    fscanf(fpath,"level=%s\n",value);
    logsetting.loglevel=getcode(value);
    fclose(fpath);
    return 0;
}

/*
 *日志设置信息
 * */
static LOGSET *getlogset()
{
    char path[512]={0x0};
    //getcwd(path,sizeof(path));
    strcat(path,"/root/log.conf");
    if(access(path,F_OK)==0){
        if(ReadConfig(path)!=0){
            logsetting.loglevel=INF;
            logsetting.maxfilelen=4096;
        }
    }else{
        logsetting.loglevel=INF;
        logsetting.maxfilelen=4096;
    }
    return &logsetting;
}

/**
 * Fetching system date
 * @param  date date pointer
 * @return      date pointer
 */
static char * jwslog_getdate(char *date)
{
    time_t timer=time(NULL);
    strftime(date,11,"%Y-%m-%d",localtime(&timer));
    return date;
}
 
/**
 * Fetching system time
 */
static void settime()
{
    time_t timer=time(NULL);
    strftime(loging.logtime,20,"%Y-%m-%d %H:%M:%S",localtime(&timer));
}
 

 /**
  * 不定参打印
  * @param fromat [description]
  * @param args   [description]
  */
static void PrintfLog(char * fromat,va_list args)
{
    vfprintf(loging.logfile, fromat, args);
}

/**
 * [initlog description]
 * @param  loglevel [description]
 * @return          [description]
 */
static int initlog(unsigned char loglevel)
{
    char strdate[30]={0x0};
    LOGSET *logsetting;
    /* Fetching log config info */
    if((logsetting=getlogset())==NULL){
        perror("Get Log Set Fail!");
        return -1;
    }
    if((loglevel&(logsetting->loglevel))!=loglevel)
        return -1;
 
    memset(&loging,0,sizeof(LOG));
    /* Fetching log time */
    settime();
    if(strlen(logsetting->filepath)==0){
        //char *path=getenv("HOME");
        //memcpy(logsetting->filepath,path,strlen(path));
 
        jwslog_getdate(strdate);
        strcat(strdate,".log");
        strcat(logsetting->filepath,"/var/log/");
        strcat(logsetting->filepath,strdate);
    }
    memcpy(loging.filepath,logsetting->filepath,MAXFILEPATH);
    /* Opening log file */
    if(loging.logfile==NULL)
        loging.logfile=fopen(loging.filepath,"a+");
    if(loging.logfile==NULL){
        perror("Open Log File Fail!");
        return -1;
    }
    /* Writing log file level and time */
    fprintf(loging.logfile,"[%s] [%s]:",LogLevelText[loglevel-1],loging.logtime);
    return 0;
}

/**
 * Write log
 */
int LogWrite(unsigned char loglevel,char *fromat,...)
{
    va_list args;
    /* initialation log */
    if(initlog(loglevel)!=0)
        return -1;
    /* print log message */
    va_start(args,fromat);
    PrintfLog(fromat,args);
    va_end(args);
    /* Flush log file */
    fflush(loging.logfile);
    /* Close log file */
    if(loging.logfile!=NULL)
        fclose(loging.logfile);
    loging.logfile=NULL;
    return 0;
}