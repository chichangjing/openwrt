/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cmdFileTransferServer.c
*
* DESCRIPTION:
*       File transfer server program for the host
*
* DEPENDENCIES:
*       Linux, FreeBSD, win32, etc
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
* COMMENTS:
*       Win32 build:
*       VC:
*           cl cmdFileTransferServer.c /wd4996 /DWIN32 ws2_32.lib
*******************************************************************************/

/***** Include files ***************************************************/

#ifndef WIN32
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/select.h>
#else
#include <winsock2.h>
#endif
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>

#define DEFAULT_PORT 12346

static char buf[BUFSIZ];
static char *target_filename = NULL;


 
int socketTcpCreateListen (int* serverSocket,unsigned short portNum);
static int do_downloadFromBoard(int acceptSocket);
static int do_uploadToBoard(int acceptSocket);
int getCmd(int acceptSocket);
int parseCommandLine(int argc, char **argv,int * portNum);
int sockWaitString(int sock, int timeout, char *buf, int bufsiz);



#ifndef WIN32
/*******************************************************************************
* socketTcpCreateListen
*
* DESCRIPTION:
*       Create TCP socket, bind it and start listening for incoming connections
*
* INPUTS:
*       serverSocket      - the var to save the socket on
*                           NULL means listen on all interfaces
*       port              - tcp port for incoming connections
*
* OUTPUTS:
*       serverSocket     - Store socket descriptior
*
* RETURNS:
*       0 - Error
*       1 - Socket is now open.
*
* COMMENTS:
*       None
*
*******************************************************************************/
int socketTcpCreateListen (int* serverSocket,unsigned short portNum)
{
  struct sockaddr_in serviceSock;

  *serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // create socket for  listening

  if (*serverSocket < 0)
  {
      printf("Error creating socket");
      return 0;
  }

  memset(&serviceSock, 0, sizeof(serviceSock));	/* zero channel */
  serviceSock.sin_family = AF_INET;
  serviceSock.sin_addr.s_addr = htonl(INADDR_ANY);
  serviceSock.sin_port = htons(portNum);

  if (bind(*serverSocket, (struct sockaddr *) &serviceSock, sizeof(serviceSock))<0)
  {
        printf("Binding of port %d failed.\n",portNum);
        return 0;
  }

  if (listen(*serverSocket, 1)<0)
  {
      printf("listen(): Error listening on socket\n");
      return 0;
  }

  return 1;
}
#else /* WIN32 */


/*********** Socket listener implementation **********************************/

/*******************************************************************************
* socketTcpCreateListen
*
* DESCRIPTION:
*       Create TCP socket, bind it and start listening for incoming connections
*
* INPUTS:
*       serverSocket      - the var to save the socket on
*                           NULL means listen on all interfaces
*       port              - tcp port for incoming connections
*
* OUTPUTS:
*       serverSocket     - Store socket descriptior
*
* RETURNS:
*       0 - Error
*       1 - Socket is now open.
*
* COMMENTS:
*       None
*
*******************************************************************************/
int socketTcpCreateListen
(
    int *serverSocket,
    unsigned short portNum
)
{
    struct sockaddr_in serviceSock;
    WSADATA ptr;

    if (WSAStartup (0x0202, &ptr) != 0)
    {
        printf("WSAStartup() failed\n");
        return 0;
    }
    
    *serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (*serverSocket == INVALID_SOCKET)
    {
        printf("Error creating socket");
        return 0;
    }

    serviceSock.sin_family = AF_INET;
    serviceSock.sin_addr.s_addr = inet_addr("");
    serviceSock.sin_port = htons(portNum);

    if (bind( *serverSocket, (SOCKADDR*) &serviceSock, sizeof(serviceSock)) == SOCKET_ERROR)
    {
        printf("Binding of port %d failed.\n",portNum);
        return 0;
    }

    if (listen( *serverSocket, 1) == SOCKET_ERROR)
    {
        printf("listen(): Error listening on socket\n");
        return 0;
    }
  return 1;
}
 
#endif

int main(int argc, char **argv)
{
	int acceptSocket,serverSocket;
    int portNum=DEFAULT_PORT;
    char * path;

    if (parseCommandLine(argc,argv,&portNum)!=0)
        return 1;

    if (socketTcpCreateListen(&serverSocket,(unsigned short)portNum)==1)
    {
        while (1)  // CTRL+C stops the program
        {
             printf("\nWaiting for client to connect on port %d...\n",portNum);
             acceptSocket = accept(serverSocket, NULL, NULL);
             printf("Client connected!\n");
             getCmd(acceptSocket);

             #ifndef WIN32
                close(acceptSocket);
            #else
                shutdown(acceptSocket,SD_BOTH); //finish sending messages
                closesocket(acceptSocket);
            #endif
         }
    }
     printf("Exiting server...\n");
 
    #ifndef WIN32
        close(serverSocket);
    #else
        shutdown(acceptSocket,SD_BOTH); //finish sending messages
        WSACleanup();
        closesocket(serverSocket);
    #endif
     return 0; 
}

/*******************************************************************************
* parseCommandLine
*
* DESCRIPTION:
*       receives the arguements of the program and translates it to the correct variables
*
* INPUTS:
*       argc      - the number of arguements
*                   
*       argv      - the arguements
*
* OUTPUTS:
*       none
*
* RETURNS:
*       0 - Command line parsed correctly
*       1 - Error.
*
* COMMENTS:
*       None
*
*******************************************************************************/
int parseCommandLine(int argc, char **argv,int *portNum)
{
    if (argc>=4) 
    {
        printf("Wrong usage.\nusage: cmdFileTransferServer.exe [port number] and\\or [path]\n");
        return 1;
    }
    else if (argc==2) 
    {
        if ((chdir(argv[1])==0) || (0<(*portNum=atoi(argv[1]))))  {return 0;}
        else if  (chdir(argv[1])!=0)
        {
            printf("The directory does not exist.\n");
            return 1;
        }
        else
        {
            printf("The port you entered is not legal.\n");
            return 1;
        }
    }
    else if (argc==3)
    {
        if (((chdir(argv[1])==0)&& (0<(*portNum=atoi(argv[2])))) || ((chdir(argv[2])==0)&& (0<(*portNum=atoi(argv[1])))))
        {
            return 0;
        }
        else
        {
            if ((chdir(argv[1])!=0) && (chdir(argv[2])!=0) )
            {
                printf("The directory does not exist.\n");
            }
            else
            {
                printf("The port you entered is not legal.\n");
            }
            return 1;
        }
    }

    if (!((*portNum<65536)&& (*portNum>0))) 
    {
        printf("Wrong usage, please enter a legal port number.\n");
        return 1;
    }

    return 0;
}

/*******************************************************************************
* getCmd
*
* DESCRIPTION:
*       receives the first command from the client and calls the upload/download functions accordingly
*
* INPUTS:
*       acceptSocket  - the client-server socket
*                   
* OUTPUTS:
*       none
*
* RETURNS:
*       0 - Action perfeormed correctly.
*       1 - Error.
*
* COMMENTS:
*       None
*
*******************************************************************************/
int getCmd(int acceptSocket)
{
    int ret;
    ret=sockWaitString(acceptSocket,1000,buf,sizeof(buf));

    if (ret==0)
    {
        printf("Connection timed out");
        return 1;
    }
    else if(ret==-1)
    {
        printf("Connection error");
        return 1;
    }

    if (strncmp(buf, "push", 4) == 0)
    {
        return do_downloadFromBoard(acceptSocket);
    }
    else if (strncmp(buf, "get", 3) == 0) 
    {
        return do_uploadToBoard(acceptSocket);
    }
    return 1;
}




/*******************************************************************************
* sockWaitString
*
* DESCRIPTION:
*       Wait for a string from connection (end with a \n)
*
* INPUTS:
*       sock
*       timeout
*       bufsiz
*
* OUTPUTS:
*       buf
*
* RETURNS:
*       string length
*       0 if timeout
*       -1 if error
*
* COMMENTS:
*
*******************************************************************************/
int sockWaitString(int sock, int timeout, char *buf, int bufsiz)
{
    int p;
    
    p = 0;
    while (1)
    {
        fd_set  rfds;
        int k;
        struct timeval tv;

        FD_ZERO(&rfds);
        FD_SET(sock, &rfds);

        tv.tv_sec = timeout;
        tv.tv_usec = 0;

        k = select(sock + 1, &rfds, (fd_set*)NULL, (fd_set*)NULL, &tv);
#ifndef WIN32
        if (k < 0 && errno == EINTR)
            continue;
#else /* WIN32 */
        if (k < 0 && WSAGetLastError() == WSAEINTR)
            continue;
#endif
        if (k = 0)
        {
            break;
        }
        if (k < 0)
        {
            break;
        }
        k = recv(sock, buf+p, 1, 0);
        if (k == 0)
        {
            /* eof */
            buf[p] = 0;
            return p ? p : -1;
        }
        if (k < 0)
        {
            return p ? p : -1;
        }
        if (buf[p] == '\n')
        {
            break;
        }
        p++;
        if (p + 1 == bufsiz)
        {
            break;
        }
    }
    buf[p] = 0;
    if (p && buf[p-1] == '\r')
        buf[--p] = 0;
    return p;
}



/*******************************************************************************
* do_uploadToBoard
*
* DESCRIPTION:
*       upload one file to board
*
* INPUTS:
*       acceptSocket -      the server-client socket
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0 successfull
*       1 error
*
* COMMENTS:
*       none
*
*******************************************************************************/

static int do_uploadToBoard(int acceptSocket)
{
    FILE    *f;
    int     filesize;
    char    filename[1024];
    int     k;

    /* get file name and size from buf */
    sscanf(buf+4, "%s", filename);
    printf("Client requested file :\"%s\"\n",filename);

    /* open file */
    f = fopen(filename, "rb");
    if (f == NULL)
    {    
        sprintf(buf,"file not found\n");
        send(acceptSocket,buf, strlen(buf), 0);
        printf("Failed to open: %s\n", strerror(errno));
        return 1;
    }
    /* get file size */
    fseek(f, 0, SEEK_END);
    filesize = ftell(f);
    rewind(f);

    //acking the board and sending file size
    sprintf(buf,"get file acked %d\n",filesize);
    send(acceptSocket,buf, strlen(buf), 0);

    for (k = 0; filename[k]; k++)
    {
        if (filename[k] == '\\')
            filename[k] = '/';
    }

    /* send file to board */
    while (!feof(f) && filesize > 0)
    {
        char    filebuf[BUFSIZ];
        int k = sizeof(filebuf);
        if (k > filesize)
            k = filesize;

        k = fread(filebuf, 1, k, f);
        if (k > 0)
        {
            send(acceptSocket, filebuf, k, 0);
            filesize -= k;
        }
    }
    fclose(f);

    if ((sockWaitString(acceptSocket,10000,buf,sizeof(buf))>0) && (strncmp(buf, "get file complete", 17) == 0) )
    {
        printf("File \"%s\" uploaded successfuly\n",filename);
        return 0;
    }
    else
    {
        printf("Connection error (no completeion ack)\n");
        return 1;
    }
}

/*******************************************************************************
* do_downloadFromBoard
*
* DESCRIPTION:
*       download one file to board
*
* INPUTS:
*       acceptSocket -      the server-client socket
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0 successfull
*       1 error
*
* COMMENTS:
*       none
*
*******************************************************************************/

static int do_downloadFromBoard(int acceptSocket)
{
    FILE    *f;
    int     filesize;
    char    filename[1024];
    int i=0;

    /* get file name and size from buf */
    sscanf(buf+5, "%s %d", filename,&filesize);   //receiving "push (filename) (size)"

    f = fopen(filename, "wb");
    if (f == NULL)
    {
        printf("Failed to create local file: %s\n", strerror(errno));
        return 1;
    }

    sprintf(buf,"push file acked\n");
    send(acceptSocket,buf, strlen(buf), 0);
 
    /* board is ready to receive file, send it */
    while (filesize > 0)
    {
        char    filebuf[BUFSIZ];
        int k = sizeof(filebuf);
        if (k > filesize)
            k = filesize;

        k = recv(acceptSocket, filebuf, k, 0);

        if (k < 0)
            break;
        if (k > 0)
        {
            if (f != NULL)
            {
                fwrite(filebuf, 1, k, f);
            }
            filesize -= k;
        }
    }
    if (f != NULL)
    {
        fclose(f);
    }

    //send ack
    sprintf(buf,"push file complete\n");
    send(acceptSocket,buf, strlen(buf), 0);
    printf("File :\"%s\" received successfuly\n",filename);


    return 0;
}
