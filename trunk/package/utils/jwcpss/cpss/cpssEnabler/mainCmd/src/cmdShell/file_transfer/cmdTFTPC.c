/*******************************************************************************
* cmdTFTPC.c
*
* DESCRIPTION:
*       TFTP client library implementation
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*******************************************************************************/

/***** Include files ***************************************************/

#include <cmdShell/common/cmdCommon.h>
#include <cmdShell/FS/cmdFS.h>


/* for simulation target use port 8069 to avoid root privileges */
#define TFTP_SVC_PORT   69

#define TFTP_RX_TIMEOUT      2000
#define TFTP_MAX_RETRANSMIT  3

/*
 * Trivial File Transfer Protocol (IEN-133)
 */
#define TFTP_SEGSIZE     512         /* data segment size */

/*
 * Packet types.
 */
#define TFTP_RRQ     01              /* read request */
#define TFTP_WRQ     02              /* write request */
#define TFTP_DATA    03              /* data packet */
#define TFTP_ACK     04              /* acknowledgement */
#define TFTP_ERROR   05              /* error code */

/*
 * Error codes.
 */
#define TFTP_EUNDEF      0       /* not defined */
#define TFTP_ENOTFOUND   1       /* file not found */
#define TFTP_EACCESS     2       /* access violation */
#define TFTP_ENOSPACE    3       /* disk full or allocation exceeded */
#define TFTP_EBADOP      4       /* illegal TFTP operation */
#define TFTP_EBADID      5       /* unknown transfer ID */
#define TFTP_EEXISTS     6       /* file already exists */
#define TFTP_ENOUSER     7       /* no such user */


/*******************************************************************************
* unpack_16bit
*
* DESCRIPTION:
*       Unpack 16bit value from network byte order to host
*
* INPUTS:
*       buf
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_U16
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_U16 unpack_16bit(
    IN const char  *buf
)
{
    return (((buf[0] << 8) & 0xff00) | (buf[1] & 0x00ff));
}

/*******************************************************************************
* pack_16bit
*
* DESCRIPTION:
*       Pack 16bit value to network byte order
*
* INPUTS:
*       value
*
* OUTPUTS:
*       buf
*
* RETURNS:
*       GT_U16
*
* COMMENTS:
*       None
*
*******************************************************************************/
static void pack_16bit(
    OUT char    *obuf,
    IN  GT_U16  value
)
{
    obuf[0] = (value >> 8) & 0x00ff;
    obuf[1] = value & 0x00ff;
}

/*******************************************************************************
* put_asciiz
*
* DESCRIPTION:
*       put asciiz parameter
*
* INPUTS:
*       abuf
*       bufLen
*       offset
*       param
*
* OUTPUTS:
*       abuf
*       offset
*       
*
* RETURNS:
*       
*
* COMMENTS:
*       None
*
*******************************************************************************/
static void put_asciiz(
    INOUT   char       *abuf,
    IN      int         bufLen,
    INOUT   int        *offset,
    IN      const char *param
)
{
    for (;param && *param && *offset < bufLen-1; param++, (*offset)++)
        abuf[*offset] = *param;
    abuf[(*offset)++] = 0;
}

/*******************************************************************************
* select_on_socket
*
* DESCRIPTION:
*       Wait data from socket
*
* INPUTS:
*       sock
*       timeOut         0xffffffff for infinite
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK, GT_FAIL, GT_TIMEOUT
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS select_on_socket(
    IN  GT_SOCKET_FD   sock,
    IN  GT_U32         timeOut
)
{
    GT_VOID *rdSet;
    int k;
    GT_STATUS rc = GT_FAIL;

    rdSet = cmdOsSelectCreateSet();
    if (rdSet == NULL)
        return rc;

    cmdOsSelectZeroSet(rdSet);
    cmdOsSelectAddFdToSet(rdSet, sock);

    k = cmdOsSelect(sock+1, rdSet, NULL, NULL, timeOut);

    if (k == 0)
        rc = GT_TIMEOUT;
    if (k > 0)
    {
        if (cmdOsSelectIsFdSet(rdSet, sock) == GT_TRUE)
            rc = GT_OK;
    }

    cmdOsSelectEraseSet(rdSet);
    return rc;
}

#define TFTP_GET    0
#define TFTP_PUT    1
/*******************************************************************************
* do_tftp
*
* DESCRIPTION:
*       Sendin/receive file using tftp protocol
*
* INPUTS:
*       cmd             - one of TFTP_GET or TFTP_PUT
*       server
*       port
*       remoterfile
*       localfd
*
* OUTPUTS:
*       errorBuf        at least 128 bytes
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_FOUND - local file not found
*
* COMMENTS:
*       None
*
*******************************************************************************/
static GT_STATUS tftp(
        int         cmd,
        const char  *server,
        GT_U16      port,
        const char  *remotefile,
        int         localfd,
        char        *errorBuf
)
{
    int len = 0;
    int finished = 0;

    GT_SOCKET_FD    sock;
    GT_VOID        *saAddr;
    GT_SIZE_T       saAddrLen;
    GT_VOID        *peerAddr;
    GT_SIZE_T       peerAddrLen;
    char            buf[TFTP_SEGSIZE+4];
    char            rbuf[TFTP_SEGSIZE+4];
    int             cp;
    GT_U16          opcode = 0;
    GT_U16          block_nr = 1;
    GT_U16          tmp;
    int             timeout = TFTP_MAX_RETRANSMIT;
    int             rc = GT_OK;

    /* see if the filename fits into buf */
    if (cmdOsStrlen(remotefile) > sizeof(buf) - 10)
    {
        cmdOsStrCpy(errorBuf, "too long remote name");
        return GT_FAIL;
    }
#define const_cast_str(a) ((char*)((GT_UINTPTR)(a)))
    if (cmdOsSocketCreateAddr(const_cast_str(server), port, &saAddr, &saAddrLen) != GT_OK)
    {
        cmdOsStrCpy(errorBuf, "Failed to resolve server address");
        return GT_FAIL;
    }
    sock = cmdOsSocketUdpCreate(-1);
    if (sock < 0)
    {
        cmdOsStrCpy(errorBuf, "Failed to create UDP socket");
        cmdOsSocketDestroyAddr(saAddr);
        return GT_FAIL;
    }
    if (cmdOsSocketCreateAddr(NULL, 0, &peerAddr, &peerAddrLen) != GT_OK)
    {
        cmdOsStrCpy(errorBuf, "Internal failure");
        cmdOsSocketUdpDestroy(sock);
        cmdOsSocketDestroyAddr(saAddr);
        return GT_FAIL;
    }
    if (cmdOsSocketBind(sock, peerAddr, peerAddrLen) != GT_OK)
    {
        cmdOsStrCpy(errorBuf, "socket bind failure");
        cmdOsSocketDestroyAddr(peerAddr);
        cmdOsSocketUdpDestroy(sock);
        cmdOsSocketDestroyAddr(saAddr);
        return GT_FAIL;
    }

    opcode = (cmd == TFTP_GET) ? TFTP_RRQ : TFTP_WRQ;
    /* build request packet */
    pack_16bit(buf, opcode);
    cp = 2;
    /* fill filename and mode in packet */
    put_asciiz(buf, sizeof(buf), &cp, remotefile);
    put_asciiz(buf, sizeof(buf), &cp, "octet");

    while (1)
    {
        /* send packet */
        timeout = TFTP_MAX_RETRANSMIT;  /* re-initialize */
        do
        {
            GT_SIZE_T         plen;
            cmdOsSocketSendTo(sock, buf, cp, saAddr, saAddrLen);
            if (finished && (opcode == TFTP_ACK)) {
                rc = GT_OK;
                break;
            }

            /* receive packet */
            rc = select_on_socket(sock, TFTP_RX_TIMEOUT);
            if (rc == GT_TIMEOUT)
            {
                timeout--;
                if (timeout)
                    continue;
                cmdOsStrCpy(errorBuf, "timeout waiting ack");
                break;
            }
            if (rc != GT_OK)
                break;

            plen = peerAddrLen;
            len = cmdOsSocketRecvFrom(sock,
                    rbuf, sizeof(rbuf),
                    GT_TRUE,
                    peerAddr, &plen);
            if (opcode == TFTP_RRQ || opcode == TFTP_WRQ)
            {
                /* first reply packet */
                /* sa.sin_port = from.sin_port; */
                cmdOsMemCpy(saAddr, peerAddr, saAddrLen);
                break;
            }
            if (cmdOsMemCmp(saAddr, peerAddr, saAddrLen) == 0)
            {
                break;
            }
            /* fall-through for bad packets! */
            /* discard the packet - treat as timeout */
            timeout = TFTP_MAX_RETRANSMIT;

        } while (timeout && (len >= 0));

        if (finished || rc != GT_OK)
        {
            break;
        }

        /* process received packet */


        opcode = unpack_16bit(rbuf);
        tmp = unpack_16bit(rbuf+2);

        if (opcode == TFTP_ERROR)
        {
            cmdOsSprintf(errorBuf, "error %d: %s", tmp, rbuf + 4);
            rc = GT_FAIL;
            break;
        }

        if (cmd == TFTP_GET && opcode == TFTP_DATA)
        {
            opcode = TFTP_ACK;
            pack_16bit(buf, TFTP_ACK);
            cp = 4;
            /* build ack */
            if (tmp == block_nr)
            {
                if (len - 4 != TFTP_SEGSIZE)
                {
                    /* last data packet */
                    finished++;
                }
                len = cmdFS.write(localfd, &rbuf[4], len - 4);
                if (len < 0)
                {
                    cmdOsSprintf(errorBuf, "cmdFS.write(): %s", cmdFS.lastError());
                    rc = GT_FAIL;
                    break;
                }

                pack_16bit(buf + 2, block_nr);
                block_nr++;
                continue;
            }
            /* in case the last ack disappeared into the ether */
            if ( tmp == (block_nr - 1) )
            {
                pack_16bit(buf + 2, block_nr-1);
                continue;
            }
            else if (tmp + 1 == block_nr)
            {
                /* Server lost our TFTP_ACK.  Resend it */
                block_nr = tmp;
                pack_16bit(buf + 2, block_nr);
                continue;
            }
        }

        if (cmd == TFTP_PUT && opcode == TFTP_ACK)
        {
            if (tmp == (GT_U16)(block_nr - 1))
            {
                if (finished)
                {
                    /* last data packet acknowledged */
                    break;
                }

                opcode = TFTP_DATA;
                pack_16bit(buf, opcode);
                pack_16bit(buf + 2, block_nr);
                cp = 4;
                block_nr++;

                len = cmdFS.read(localfd, buf + cp, TFTP_SEGSIZE);

                if (len < 0) {
                    cmdOsStrCpy(errorBuf, "cmdFS.read() error");
                    rc = GT_FAIL;
                    break;
                }

                if (len != TFTP_SEGSIZE) {
                    finished++;
                }
                cp += len;

                continue;
            }
            if (tmp == (GT_U16)(block_nr - 2))
            {
                /* retransmit last packet */
                continue;
            }
            break;
        }
    }

    cmdOsSocketDestroyAddr(peerAddr);
    cmdOsSocketUdpDestroy(sock);
    cmdOsSocketDestroyAddr(saAddr);

    return finished ? GT_OK : rc;
}


/*******************************************************************************
* cmdFS_tftp_put
*
* DESCRIPTION:
*       Send file using tftp protocol
*
* INPUTS:
*       server
*       port
*       remoterfile
*       localfile
*
* OUTPUTS:
*       errorBuf        at least 128 bytes
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_FOUND - local file not found
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdFS_tftp_put(
    IN  const char  *server,
    IN  GT_U16      port,
    IN  const char  *localfile,
    IN  const char  *remotefile,
    OUT char        *errorBuf
)
{
    int fd;
    GT_STATUS rc;

    fd = cmdFS.open(localfile, GEN_FS_O_RDONLY);
    if (fd < 0)
    {
        cmdOsSprintf(errorBuf, "can't open local file: %s", cmdFS.lastError());
        return GT_NOT_FOUND;
    }
    rc = tftp(TFTP_PUT, server, port, remotefile, fd, errorBuf);
    cmdFS.close(fd);
    return rc;
}
/*******************************************************************************
* cmdFS_tftp_get
*
* DESCRIPTION:
*       Receive file using tftp protocol
*
* INPUTS:
*       server      - TFTP server IP
*       port        - TFTP server port
*       remoterfile - remote file name
*       localfile   - local file name
*       compress    - add GEN_FS_O_COMPRESS for local file
*
* OUTPUTS:
*       errorBuf        at least 128 bytes
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_FOUND - local file not found
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdFS_tftp_get(
    IN  const char  *server,
    IN  GT_U16      port,
    IN  const char  *localfile,
    IN  const char  *remotefile,
    IN  GT_BOOL     compress,
    OUT char        *errorBuf
)
{
    int         fd;
    GT_STATUS   rc;
    int         flags = GEN_FS_O_WRONLY | GEN_FS_O_CREAT | GEN_FS_O_TRUNC;

    if (compress == GT_TRUE)
    {
        flags |= GEN_FS_O_COMPRESS;
    }

    fd = cmdFS.open(localfile, flags);
    if (fd < 0)
    {
        cmdOsSprintf(errorBuf, "can't open/create local file: %s", cmdFS.lastError());
        return GT_NOT_FOUND;
    }
    rc = tftp(TFTP_GET, server, port, remotefile, fd, errorBuf);
    cmdFS.close(fd);
    return rc;
}
