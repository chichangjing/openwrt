/*******************************************************************************
* cmdFileTransferClient.c
*
* DESCRIPTION:
*       File transfer client program
*
* DEPENDENCIES:
*       Linux, FreeBSD, win32, etc
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*
* COMMENTS:
*       Win32 build:
*       VC:
*           cl cmdFileTransferClient.c /wd4996 /DWIN32 ws2_32.lib
*        
*       Error constants taken from uClinux under BSD 2.0 license: 
*       uClinux-samsung/user/python/Modules/socketmodule.c 
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

/***** Defines  ********************************************************/

#define NET_FLUSH_TIMEOUT       1
#define NET_WAITSTRING_TIMEOUT  60
#define CMD_FS_PORT             "12346"

/***** Global macros ***************************************************/

/***** Private Types ***************************************************/

/***** Private Data ****************************************************/

static int verbose = 0;
static int quiet = 0;
static int sock = -1;
static char buf[BUFSIZ];
static char *target_filename = NULL;

/***** Private Functions ***********************************************/

#ifdef WIN32
const char* sockerrorstr(void)
{
    int k;
    int err;
    static struct  {
        int err;
        const char *errorstr;
    } sock_errors [] = {
        { 0,                  "No error" },
        { WSAEINTR,           "Interrupted system call" },
        { WSAEBADF,           "Bad file number" },
        { WSAEACCES,          "Permission denied" },
        { WSAEFAULT,          "Bad address" },
        { WSAEINVAL,          "Invalid argument" },
        { WSAEMFILE,          "Too many open sockets" },
        { WSAEWOULDBLOCK,     "Operation would block" },
        { WSAEINPROGRESS,     "Operation now in progress" },
        { WSAEALREADY,        "Operation already in progress" },
        { WSAENOTSOCK,        "Socket operation on non-socket" },
        { WSAEDESTADDRREQ,    "Destination address required" },
        { WSAEMSGSIZE,        "Message too long" },
        { WSAEPROTOTYPE,      "Protocol wrong type for socket" },
        { WSAENOPROTOOPT,     "Bad protocol option" },
        { WSAEPROTONOSUPPORT, "Protocol not supported" },
        { WSAESOCKTNOSUPPORT, "Socket type not supported" },
        { WSAEOPNOTSUPP,      "Operation not supported on socket" },
        { WSAEPFNOSUPPORT,    "Protocol family not supported" },
        { WSAEAFNOSUPPORT,    "Address family not supported" },
        { WSAEADDRINUSE,      "Address already in use" },
        { WSAEADDRNOTAVAIL,   "Can't assign requested address" },
        { WSAENETDOWN,        "Network is down" },
        { WSAENETUNREACH,     "Network is unreachable" },
        { WSAENETRESET,       "Net connection reset" },
        { WSAECONNABORTED,    "Software caused connection abort" },
        { WSAECONNRESET,      "Connection reset by peer" },
        { WSAENOBUFS,         "No buffer space available" },
        { WSAEISCONN,         "Socket is already connected" },
        { WSAENOTCONN,        "Socket is not connected" },
        { WSAESHUTDOWN,       "Can't send after socket shutdown" },
        { WSAETOOMANYREFS,    "Too many references, can't splice" },
        { WSAETIMEDOUT,       "Connection timed out" },
        { WSAECONNREFUSED,    "Connection refused" },
        { WSAELOOP,           "Too many levels of symbolic links" },
        { WSAENAMETOOLONG,    "File name too long" },
        { WSAEHOSTDOWN,       "Host is down" },
        { WSAEHOSTUNREACH,    "No route to host" },
        { WSAENOTEMPTY,       "Directory not empty" },
        { WSAEPROCLIM,        "Too many processes" },
        { WSAEUSERS,          "Too many users" },
        { WSAEDQUOT,          "Disc quota exceeded" },
        { WSAESTALE,          "Stale NFS file handle" },
        { WSAEREMOTE,         "Too many levels of remote in path" },
        { WSASYSNOTREADY,     "Network system is unavailable" },
        { WSAVERNOTSUPPORTED, "Winsock version out of range" },
        { WSANOTINITIALISED,  "WSAStartup not yet called" },
        { WSAEDISCON,         "Graceful shutdown in progress" },
        { WSAHOST_NOT_FOUND,  "Host not found" },
        { WSANO_DATA,         "No host data of that type was found" },
        { 0, NULL }
    };
    err = WSAGetLastError();
    for (k = 0; sock_errors[k].errorstr; k++)
    {
        if (err == sock_errors[k].err)
            return sock_errors[k].errorstr;
    }
    return "Unknown error";
}

char *optarg = NULL;
int optind = 1;
int opterr = 1;
int optopt = '?';
char *optptr = NULL;
int getopt(int argc,char **argv,char *optstring)
{
    const char *p;
    if (optptr == NULL || *optptr == 0)
    {
        if (optind >= argc)
            return EOF;
        optptr = argv[optind];
        if (optptr[0] == 0 || optptr[0] != '-')
        {
            /* empty arg or start with no '-' */
            return EOF;
        }
        if (optptr[1] == '-' && optptr[2] == 0)
        {
            /* -- */
            optind++;
            return EOF;
        }
        optptr++;
    }
    optopt = *optptr++;
    if ((p = strchr(optstring, optopt)) == NULL)
        return '?';
    if (p[1] != ':')
    {
        if (*optptr == 0)
            optind++;
        return optopt;
    }
    if (*optptr)
    {
        optarg = optptr;
        optptr = NULL;
        optind++;
        return optopt;
    }
    optind++;
    if (optind >= argc)
        return '?';
    optarg = argv[optind];
    optptr = NULL;
    optind++;
    return optopt;
}
#endif

#ifndef WIN32
/*******************************************************************************
* connectTcp
*
* DESCRIPTION:
*       Connect to remote host
*
* INPUTS:
*       host
*       port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       socket or -1 if error
*
* COMMENTS:
*
*******************************************************************************/
int connectTcp(const char *host, const char *port)
{
    struct addrinfo hints;
    struct addrinfo *ai;
    int     er;
    int     sock;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;/* AF_INET */
    hints.ai_socktype = SOCK_STREAM;
    er = getaddrinfo(host, port, &hints, &ai);
    if (er)
    {
        if (!quiet)
            fprintf(stderr, "can't getaddrinfo(): %s\n", gai_strerror(er));
        return -1;
    }
    sock = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
    if (sock < 0) {
        if (!quiet)
            fprintf(stderr, "can't socket(): %s\n", strerror(errno));
        freeaddrinfo(ai);
        return -1;
    }
    if (connect(sock,  ai->ai_addr,ai->ai_addrlen) < 0) {
        if (!quiet)
            fprintf(stderr, "can't connect(): %s\n", strerror(errno));
        freeaddrinfo(ai);
        close(sock);
        return -1;
    }
    freeaddrinfo(ai);

    return sock;
}
#else /* WIN32 */
/*******************************************************************************
* connectTcp
*
* DESCRIPTION:
*       Connect to remote host
*
* INPUTS:
*       host
*       port
*
* OUTPUTS:
*       None
*
* RETURNS:
*       socket or -1 if error
*
* COMMENTS:
*
*******************************************************************************/
int connectTcp(const char *host, const char *port)
{
    struct sockaddr_in addr;
    unsigned long address;
    int     sock;

    address = inet_addr(host);
    if (address == INADDR_NONE)
    {
        if (!quiet)
            fprintf(stderr, "bad ip address\n");
        return -1;
    }
    addr.sin_addr.s_addr =address;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(port));

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        if (!quiet)
            fprintf(stderr, "can't socket(): %s\n", sockerrorstr());
        return -1;
    }
    if (connect(sock, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
        if (!quiet)
            fprintf(stderr, "can't connect(): %s\n", sockerrorstr());
        closesocket(sock);
        return -1;
    }

    return sock;
}

#endif

/*******************************************************************************
* sockWaitString
*
* DESCRIPTION:
*       Wait for a string from connection
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

    if (timeout < 0)
        timeout = NET_WAITSTRING_TIMEOUT;
    
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
        if (k == 0)
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
* send_cmd
*
* DESCRIPTION:
*       Send command to board
*
* INPUTS:
*       format      string format, see printf
*       ...
*
* OUTPUTS:
*       None
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
static int send_cmd(const char *format, ...)
{
    va_list args;
    int     ret;
    char buf[2048];

    va_start(args, format);
    ret = vsprintf(buf, format, args);
    va_end(args);

    if (verbose)
        printf("sending command \"%.*s\"\n", ret, buf);

    buf[ret++] = '\n';
    send(sock, buf, ret, 0);
    return 0;
}

/*******************************************************************************
* wait_ack
*
* DESCRIPTION:
*       Wait command acknowledge
*
* INPUTS:
*       msg1    - error message to report in case of timeout
*       msg2    - error message to report in case of bad ack
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0 if responce is good
*       1 otherwise
*
* COMMENTS:
*
*******************************************************************************/
static int wait_ack(const char *msg1, const char *msg2)
{
    if (sockWaitString(sock, -1, buf, sizeof(buf)) < 3)
    {
        if (!quiet)
            fprintf(stderr, "%s\n", msg1 ? msg1 : "failed to wait command ack");
        return 1;
    }
    if (verbose)
        printf("%s\n",buf);

    if (buf[0] != '2')
    {
        if (!quiet && !verbose)
            printf("%s\n",msg2 ? msg2 : buf);
        return 1;
    }
    return 0;
}


/*******************************************************************************
* do_ls
*
* DESCRIPTION:
*       execute ls command and print result
*
* INPUTS:
*       argc
*       argv
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0 successfull
*       1 otherwise
*
* COMMENTS:
*
*******************************************************************************/
static int do_ls(int argc, char **argv)
{
    char tbuf[5];

    if (optind < argc)
    {
        send_cmd("ls %s", argv[optind]);
    }
    else
    {
        send_cmd("ls");
    }

    if (wait_ack(NULL, NULL) != 0)
        return 1;

    /* responce should be 2xx-.... */
    if (buf[3] != '-')
    {
        if (!verbose)
            printf("%s\n",buf);
        return 0;
    }
    /* save responce code */
    strncpy(tbuf, buf, 3);
    tbuf[3] = ' ';
    tbuf[4] = 0;
    /* print till line startting with the same responce code
     * following by space */
    while(1)
    {
        if (sockWaitString(sock, -1, buf, sizeof(buf)) < 0)
            break;
        if (verbose)
            printf("%s\n",buf);
        if (buf[0] != ' ')
        {
            if (strncmp(buf, tbuf, 4) == 0)
            {
                /* this is the last line of reply */
                if (!verbose)
                    printf("%s\n",buf+4);
                return 0;
            }
            else
            {
                /* line starts with wrong code, return error */
                if (!verbose)
                    printf("%s\n",buf);
                return 1;
            }
        }
        else
        {
            /* delete leading space */
            if (!verbose)
                printf("%s\n",buf+1);
        }
    }
    return 1;
}

/*******************************************************************************
* do_upload
*
* DESCRIPTION:
*       upload one file to board
*
* INPUTS:
*       cmd
*       argc
*       argv
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0 successfull
*       1 otherwise
*
* COMMENTS:
*       optind points to current file name
*
*******************************************************************************/
static int do_upload(const char *cmd, int argc, char **argv)
{
    FILE    *f;
    int     filesize;
    char    target_name[1024];
    int     k;

    if (!quiet)
    {
        printf("Uploading file %s", argv[optind]);
    }

    /*TODO: check that file is regular one */

    /* open file */
    f = fopen(argv[optind], "rb");
    if (f == NULL)
    {
        if (!quiet)
            printf(": failed to open: %s\n", strerror(errno));
        return 1;
    }
    /* get file size */
    fseek(f, 0, SEEK_END);
    filesize = ftell(f);
    rewind(f);

    if (!quiet)
    {
        printf("...");
    }

    strcpy(target_name, target_filename ? target_filename : argv[optind]);
    for (k = 0; target_name[k]; k++)
    {
        if (target_name[k] == '\\')
            target_name[k] = '/';
    }

    send_cmd("%s %d %s", cmd, filesize, target_name);

    if (wait_ack(NULL, NULL) != 0)
        return 1;

    /* board is ready to receive file, send it */
    while (!feof(f) && filesize > 0)
    {
        char    filebuf[BUFSIZ];
        int k = sizeof(filebuf);
        if (k > filesize)
            k = filesize;

        k = fread(filebuf, 1, k, f);
        if (k > 0)
        {
            send(sock, filebuf, k, 0);
            if (verbose)
                write(1, filebuf, k);
            filesize -= k;
        }
    }
    fclose(f);

    if (verbose)
        printf("waiting final ack...\n");

    if (wait_ack(NULL, NULL) != 0)
        return 1;
    
    if (!quiet)
        printf("OK\n");
    return 0;
}

/*******************************************************************************
* do_get
*
* DESCRIPTION:
*       get one file from board
*
* INPUTS:
*       argc
*       argv
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0 successfull
*       1 otherwise
*
* COMMENTS:
*       optind points to current file name
*
*******************************************************************************/
static int do_get(int argc, char **argv)
{
    FILE    *f;
    int     filesize;

    if (!quiet)
    {
        printf("Downloading file %s", argv[optind]);
        if (target_filename)
            printf(" to %s", target_filename);
    }

    send_cmd("get %s", argv[optind]);
    if (wait_ack(NULL, NULL) != 0)
        return 1;

    /* get file size from buf */
    sscanf(buf+4, "%d", &filesize);

    f = fopen(target_filename ? target_filename : argv[optind], "wb");
    if (f == NULL)
    {
        if (!quiet)
            printf(": failed to create local file: %s\n", strerror(errno));
    }
    else
    {
        if (!quiet)
        {
            printf("...");
        }
    }

    /* board is ready to receive file, send it */
    while (filesize > 0)
    {
        char    filebuf[BUFSIZ];
        int k = sizeof(filebuf);
        if (k > filesize)
            k = filesize;

        k = recv(sock, filebuf, k, 0);
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

    if (verbose)
        printf("waiting final ack...\n");

    if (wait_ack(NULL, NULL) != 0)
        return 1;
    
    if (!quiet)
        printf("OK\n");
    return 0;
}

/*******************************************************************************
* do_delete
*
* DESCRIPTION:
*       delete one file on the board
*
* INPUTS:
*       argc
*       argv
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0 successfull
*       1 otherwise
*
* COMMENTS:
*       optind points to current file name
*
*******************************************************************************/
static int do_delete(int argc, char **argv)
{
    if (!quiet)
    {
        printf("Deleting file %s...", argv[optind]);
    }

    send_cmd("del %s", argv[optind]);

    if (wait_ack(NULL, NULL) != 0)
        return 1;

    if (!quiet)
        printf("OK\n");
    return 0;
}


/***** Public Functions ************************************************/

/*******************************************************************************
* Usage
*
* DESCRIPTION:
*       Print command line description and exit
*
* INPUTS:
*       argv0   - argv[0]
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
void Usage(char *argv0)
{
    printf("Usage: %s [-v] [-q] [-l] [-u] [-z] [-d] [-o _name_] _board_ip_ [filename...]\n", argv0);
    printf("\t-v        verbose\n");
    printf("\t-q        quiet operations, check exit status\n");
    printf("\t-l        list files\n");
    printf("\t-p        push files (default mode)\n");
    printf("\t-d        delete files on board\n");
    printf("\t-g        get files from board\n");
    printf("\t-o _name_ target file name\n");
    printf("\t-z        upload file with compression\n");
    exit(0);
}

/*******************************************************************************
* main
*
* DESCRIPTION:
*       main
*
* INPUTS:
*
* OUTPUTS:
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
int main(int argc, char **argv)
{
    enum {
        MODE_UPLOAD,
        MODE_LS,
        MODE_DELETE,
        MODE_GET
    }                   mode = MODE_UPLOAD;
    int                 k;
    int                 ret = 0;
    int                 upload_with_compression = 0;

    setbuf(stdout, NULL);

    /* parse command line */
    while ((k = getopt(argc, argv, "vqludgo:z")) != EOF)
    {
        switch(k)
        {
            case 'v':
                verbose = 1;
                quiet = 0;
                break;
            case 'q':
                quiet = 1;
                verbose = 0;
                break;
            case 'l':
                mode = MODE_LS;
                break;
            case 'u':
                mode = MODE_UPLOAD;
                break;
            case 'd':
                mode = MODE_DELETE;
                break;
            case 'g':
                mode = MODE_GET;
                break;
            case 'o':
                target_filename = optarg;
                break;
            case 'z':
                upload_with_compression = 1;
                break;
            default:
                fprintf(stderr, "wrong option: -%c\n", k);
                Usage(argv[0]);
        }
    }

    if (optind >= argc)
    {
        Usage(argv[0]);
    }

#ifdef  WIN32
    {
        WSADATA ptr;
        if (WSAStartup (0x0202, &ptr) != 0)
        {
            if (!quiet)
                fprintf(stderr, "WSAStartup() failed\n");
            return 1;
        }
    }

#endif
    /* connect to the board */
    if (verbose)
        printf("Connecting to %s:%s...", argv[optind], CMD_FS_PORT);
    sock = connectTcp(argv[optind], CMD_FS_PORT);
    if (sock < 0)
    {
        return 1;
    }
    if (verbose)
        printf("connected\n");

    optind++;

    /* wait greeting message */
    if (wait_ack(
            "failed to wait greeting message",
            "service is not ready"
        ) != 0)
    {
#ifndef WIN32
        close(sock);
#else
        closesocket(sock);
#endif
        return 1;
    }


    /* now list, upload or delete */
    if (mode == MODE_LS)
    {
        ret = do_ls(argc, argv);
    }

    if (mode == MODE_UPLOAD)
    {
        for (;optind < argc; optind++)
        {
            if (do_upload(upload_with_compression?"zpush":"push", argc, argv))
                ret = 1;
            if (target_filename) /* only one file when -o _name_ */
                break;
        }
    }

    if (mode == MODE_DELETE)
    {
        for (;optind < argc; optind++)
        {
            if (do_delete(argc, argv))
                ret = 1;
        }
    }

    if (mode == MODE_GET)
    {
        for (;optind < argc; optind++)
        {
            if (do_get(argc, argv))
                ret = 1;
            if (target_filename) /* only one file when -o _name_ */
                break;
        }
    }

#ifndef WIN32
    close(sock);
#else
    closesocket(sock);
#endif
    return ret;
}
