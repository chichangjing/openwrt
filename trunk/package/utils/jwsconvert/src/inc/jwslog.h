/*
 * jwslog.h
 *
 *  Created on: Jul 10, 2014
 *      Author: zjp35
 */
 
#ifndef __LOGC_H_
#define __LOGC_H_

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "time.h"
#include "stdarg.h"
#include "unistd.h"
 
#define MAXLEN (2048)
#define MAXFILEPATH (512)
#define MAXFILENAME (50)
typedef enum{
    ERROR_1=-1,
    ERROR_2=-2,
    ERROR_3=-3
}ERROR0;
 
typedef enum{
    NONE=0,
    INF=1,
    DEBUG=2,
    ERROR=4,
    ALL=255
}LOGLEVEL;
 
typedef struct log{
    char logtime[20];
    char filepath[MAXFILEPATH];
    FILE *logfile;
}LOG;
 
typedef struct logseting{
    char filepath[MAXFILEPATH];
    unsigned int maxfilelen;
    unsigned char loglevel;
}LOGSET;

#define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1):__FILE__)

int LogWrite(unsigned char loglevel,char *fromat,...);

#endif /* __LOGC_H_ */