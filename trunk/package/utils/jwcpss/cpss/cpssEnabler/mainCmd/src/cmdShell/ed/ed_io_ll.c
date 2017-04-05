#include "ed.h"
#include <extUtils/IOStream/IOStream.h>

int iostream_readline(ed_data_t *ed, char *bufferPtr, int bufferLength)
{
    IOStreamPTR stream = (IOStreamPTR)ed->iostream; 
    int ret;
    ret = stream->readLine(stream, bufferPtr, bufferLength);
    if (ret < 0)
        return ret;
    /* add \n */
    bufferPtr[ret++] = '\n';
    return ret;
}

int iostream_write(ed_data_t *ed, const void *bufferPtr, int bufferLength)
{
    IOStreamPTR stream = (IOStreamPTR)ed->iostream; 
    return stream->write(stream, bufferPtr, bufferLength);
}

int iostream_writeLine(ed_data_t *ed, const char *s)
{
    IOStreamPTR stream = (IOStreamPTR)ed->iostream; 
    return stream->writeLine(stream, s);
}

int iostream_writeLineNL(ed_data_t *ed, const char *s)
{
    int k;
    k = iostream_writeLine(ed, s);
    k += iostream_write(ed, "\n", 1);
    return k;
}
