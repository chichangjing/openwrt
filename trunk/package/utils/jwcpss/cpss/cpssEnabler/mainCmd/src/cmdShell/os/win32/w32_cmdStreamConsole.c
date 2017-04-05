/*******************************************************************************
*              (c), Copyright 2007, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* w32_cmdStreamConsole.c
*
* DESCRIPTION:
*       This file contains mainCmd stream implementation for Win32 console
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*
*******************************************************************************/

#include <cmdShell/os/cmdStreamImpl.h>
#include <gtOs/gtGenTypes.h>
#include <windows.h>

typedef struct {
        char    inputBuffer[10];
        int     state;
        int     nArgs;
        int     args[10];
} VT100_STC;

typedef struct {
    IOStreamSTC common;
    HANDLE hInput;
    HANDLE hOutput;
    /* one of FILE_TYPE_CHAR or FILE_TYPE_PIPE */
    DWORD type;
    /* terminal emulation support */
    VT100_STC vt100;
} consoleStreamSTC, *consoleStreamPTR;

/*******************************************************************************
* a simple VT100 emulation
*
* Supported escape sequences:
*
*   
* ANSI compatible mode
*
*   ESC [ count A   cursor up
*   ESC [ count B   cursor down
*   ESC [ count C   cursor right
*   ESC [ count D   cursor left
*   ESC [ line ; column H  cursor position
*   ESC [ line ; column f  cursor position
*
*   ESC [ what J    erace in display
*           0   - erase to end of screen inclusive
*           1   - erase from start to current position, inclusive
*           2   - erase the whole screen
*   ESC [ what K    erace in line
*           0   - erase to end of line, inclusive
*           1   - erase from start to current position, inclusive
*           2   - erase all of the line
*
*   ESC [ Ps ; ... ; Ps m    Select graphic rendition
*           0   - reset attributes
*           1   - bold on
*           4   - underscore
*           5   - blink
*           6   - negative
*        Foreground Colours
*           30  Black
*           31  Red
*           32  Green
*           33  Yellow
*           34  Blue
*           35  Magenta
*           36  Cyan
*           37  White
*
*        Background Colours
*           40  Black
*           41  Red
*           42  Green
*           43  Yellow
*           44  Blue
*           45  Magenta
*           46  Cyan
*           47  White
*
*   ESC [ count @   shift right
*   ESC [ count P   shift left
*
* VT52 compatibility
*   ESC J   erace to end of screen inclusive
*   ESC K   erace to end of line, inclusive
*
*
* Unsupported:
*   ESC D   scroll down
*   ESC M   scroll up
*   
*******************************************************************************/

#define VT100 stream->vt100

static struct {
    WORD        wVirtualKeyCode;
    const char *sequence;
} VT100_keys[] = {
    { VK_ESCAPE,    "\033" },
    { VK_UP,        "\033[A" },
    { VK_DOWN,      "\033[B" },
    { VK_RIGHT,     "\033[C" },
    { VK_LEFT,      "\033[D" },
    { 0,            NULL }
};

static int VT100_FG[8] = {
    0, /* black */
    FOREGROUND_RED, /* red */
    FOREGROUND_GREEN, /* green */
    FOREGROUND_RED | FOREGROUND_GREEN, /* yellow */
    FOREGROUND_BLUE, /* blue */
    FOREGROUND_BLUE | FOREGROUND_RED, /* magenta */
    FOREGROUND_BLUE | FOREGROUND_GREEN, /* cyan */
    FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED /* white */
};

static int VT100_BG[8] = {
    0, /* black */
    BACKGROUND_RED, /* red */
    BACKGROUND_GREEN, /* green */
    BACKGROUND_RED | BACKGROUND_GREEN, /* yellow */
    BACKGROUND_BLUE, /* blue */
    BACKGROUND_BLUE | BACKGROUND_RED, /* magenta */
    BACKGROUND_BLUE | BACKGROUND_GREEN, /* cyan */
    BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED /* white */
};

static void consoleStreamVT100Init(consoleStreamPTR stream)
{
    VT100.inputBuffer[0] = 0;
    VT100.state = 0;
}

/* terminal emulation */
static int consoleStreamVT100Output(consoleStreamPTR stream, const char *buff, int bufferLength)
{
    enum {
        OP_NONE,
        OP_TYPE,
        OP_CURSOR_LEFT,
        OP_CURSOR_RIGHT,
        OP_CURSOR_UP,
        OP_CURSOR_DOWN,
        OP_CURSOR_POSITION,
        OP_ERASE_LINE,
        OP_ERASE_SCREEN,
        OP_SET_MODE,
        OP_SHIFT_RIGHT,
        OP_SHIFT_LEFT
    } operation;
    int b = -1;
    int p;
    int count;
    DWORD bytes;

    for (p = 0; p < bufferLength; p++)
    {
        operation = OP_NONE;
        /* a simple state machine */
        switch (VT100.state)
        {
            case 0:
                VT100.nArgs = 0;
                VT100.args[0] = -1;
                switch(buff[p])
                {
                    case '\033':
                        VT100.state = 1; break; /* ESC */
                    /* Backspace, tab, bell, carriage return, and linefeed
                     * characters are processed by console,
                     * see SetConsoleMode() for reference
                     */
                    default: 
                        operation = OP_TYPE;
                }
                break;
            case 1: /* ESC */
                switch(buff[p])
                {
                    case '[':
                        VT100.state = 2;    /* ESC [ */
                        break;
                    case 'J': operation = OP_ERASE_SCREEN; break;
                    case 'K': operation = OP_ERASE_LINE; break;
                    default:
                        WriteFile(stream->hOutput, "\033", 1, &bytes, 0);
                        VT100.state = 0;
                        operation = OP_TYPE;
                }
                break;
            case 2: /* ESC [ */
                if (buff[p] >= '0' && buff[p] <= '9')
                {
                    if (VT100.nArgs == 0)
                    {
                        VT100.nArgs = 1;
                        VT100.args[VT100.nArgs-1] = 0;
                    }
                    if (VT100.args[VT100.nArgs-1] == -1)
                        VT100.args[VT100.nArgs-1] = 0;
                    VT100.args[VT100.nArgs-1] *= 10;
                    VT100.args[VT100.nArgs-1] += buff[p] - '0';
                    break;
                }
                if (buff[p] == ';')
                {
                    if (VT100.nArgs == 0)
                    {
                        VT100.args[VT100.nArgs] = -1;
                        VT100.nArgs = 1;
                    }
                    if (VT100.nArgs < 10)
                    {
                        VT100.args[VT100.nArgs] = -1;
                        VT100.nArgs++;
                    }
                    else
                    {
                        VT100.state = 0; /* overflow, fallback to state 0 */
                    }
                    break;
                }
                switch (buff[p])
                {
                    case 'A': operation = OP_CURSOR_UP; break;
                    case 'B': operation = OP_CURSOR_DOWN; break;
                    case 'C': operation = OP_CURSOR_RIGHT; break;
                    case 'D': operation = OP_CURSOR_LEFT; break;
                    case 'H': operation = OP_CURSOR_POSITION; break;
                    case 'f': operation = OP_CURSOR_POSITION; break;
                    case 'J': operation = OP_ERASE_SCREEN; break;
                    case 'K': operation = OP_ERASE_LINE; break;
                    case 'm': operation = OP_SET_MODE; break;
                    case '@': operation = OP_SHIFT_RIGHT; break;
                    case 'P': operation = OP_SHIFT_LEFT; break;
                    default:
                        VT100.state = 0;
                }
                break;
        }
        /* execute operation */
        if (operation == OP_TYPE)
        {
            if (b == -1)
                b = p;
        }
        else
        {
            CONSOLE_SCREEN_BUFFER_INFO inf;
            COORD c;
            SMALL_RECT sr;
            CHAR_INFO fill;

            if (b >= 0 && b < p)
            {
                WriteFile(stream->hOutput, buff+b, p - b, &bytes, 0);
            }
            b = -1;

            GetConsoleScreenBufferInfo(stream->hOutput, &inf);
            c = inf.dwCursorPosition;
            sr = inf.srWindow;
            fill.Char.UnicodeChar = 0x20;
            fill.Attributes = inf.wAttributes;

#define nvl(idx,value) \
            while (VT100.nArgs < idx+1) \
                VT100.args[VT100.nArgs++] = -1; \
            if (VT100.args[idx] == -1) \
                VT100.args[idx] = value;
#define arg(idx)    VT100.args[idx]
            switch (operation)
            {
                case OP_NONE:
                case OP_TYPE:
                    break;
                case OP_SHIFT_RIGHT:
                    nvl(0,1);
                    sr.Top = sr.Bottom = c.Y;
                    sr.Left = c.X;
                    c.X += arg(0);
                    ScrollConsoleScreenBuffer(stream->hOutput,
                                    &sr, NULL, c, &fill);
                    break;
                case OP_SHIFT_LEFT:
                    nvl(0,1);
                    sr.Top = sr.Bottom = c.Y;
                    sr.Left = c.X + arg(0);
                    ScrollConsoleScreenBuffer(stream->hOutput,
                                    &sr, NULL, c, &fill);
                    break;
                case OP_CURSOR_LEFT:
                    nvl(0,1);
                    while (arg(0) && c.X)
                    {
                        arg(0)--;
                        c.X--;
                    }
                    SetConsoleCursorPosition(stream->hOutput, c);
                    break;
                case OP_CURSOR_RIGHT:
                    nvl(0,1);
                    while (arg(0) && c.X < inf.dwSize.X)
                    {
                        arg(0)--;
                        c.X++;
                    }
                    SetConsoleCursorPosition(stream->hOutput, c);
                    break;
                case OP_CURSOR_UP:
                    nvl(0,1);
                    while (arg(0) && c.Y)
                    {
                        arg(0)--;
                        c.Y--;
                    }
                    SetConsoleCursorPosition(stream->hOutput, c);
                    break;
                case OP_CURSOR_DOWN:
                    nvl(0,1);
                    while (arg(0) && c.Y < inf.dwSize.Y)
                    {
                        arg(0)--;
                        c.Y++;
                    }
                    SetConsoleCursorPosition(stream->hOutput, c);
                    break;
                case OP_CURSOR_POSITION:
                    nvl(0,1);
                    nvl(1,1);
		    if (arg(0) == 0) arg(0) = 1;
		    if (arg(1) == 0) arg(1) = 1;
                    c.Y = ((arg(0) - 1) > inf.dwSize.Y) ? inf.dwSize.Y : (arg(0) - 1);
                    c.X = ((arg(1) - 1) > inf.dwSize.X) ? inf.dwSize.X : (arg(1) - 1);
                    SetConsoleCursorPosition(stream->hOutput, c);
                    break;
                case OP_ERASE_LINE:
                    nvl(0,0);
                    count = 0;
                    if (arg(0) == 0) { /* erase to end of line, inclusive */
                        count = inf.dwSize.X - c.X;
                    }
                    else if (arg(0) == 1) {/* erase from start to cursor, inclusive */
                        count = c.X + 1;
                        c.X = 0;
                    }
                    else if (arg(0) == 2) {/* erase the whole line */
                        count = inf.dwSize.X;
                        c.X = 0;
                    }
                    if (count)
                    {
                        DWORD   numOut;
                        FillConsoleOutputCharacter(stream->hOutput,
                                0x20, count, c, &numOut);
                        FillConsoleOutputAttribute(stream->hOutput,
                                fill.Attributes, count, c, &numOut);
                    }
                    break;
                case OP_ERASE_SCREEN:
                    nvl(0,0);
                    count = 0;
                    if (arg(0) == 0) { /* erase to end of screen, inclusive */
                        count = inf.dwSize.X - c.X;
                        count += inf.dwSize.X * (inf.dwSize.Y - c.Y - 1);
                    }
                    else if (arg(0) == 1) {/* erase from start to cursor, inclusive */
                        count = c.X + 1;
                        count += inf.dwSize.X * c.Y;
                        c.X = 0;
                        c.Y = 0;
                    }
                    else if (arg(0) == 2) {/* erase the whole screen */
                        count = inf.dwSize.X * inf.dwSize.Y;
                        c.X = 0;
                        c.Y = 0;
                    }
                    if (count)
                    {
                        DWORD   numOut;
                        FillConsoleOutputCharacter(stream->hOutput,
                                0x20, count, c, &numOut);
                        FillConsoleOutputAttribute(stream->hOutput,
                                fill.Attributes, count, c, &numOut);
                    }
                    break;
                case OP_SET_MODE:
#define attr inf.wAttributes
                    if (VT100.nArgs == 0)
                    {
                        attr = VT100_FG[7] | VT100_BG[0];
                    }
                    for (count = 0; count < VT100.nArgs; count++)
                    {
                        nvl(count, 0);
                        switch(arg(count))
                        {
                            case 0: /* reset attributes */
                                attr = VT100_FG[7] | VT100_BG[0];
                                break;
                            case 1: /* bold on */
                                attr |= FOREGROUND_INTENSITY;
                                break;
                            case 4: /* underscore */
                                attr |= COMMON_LVB_UNDERSCORE;
                            case 5: /* blink */
                                break;
                            case 6: /* negative */
                                attr |= COMMON_LVB_REVERSE_VIDEO;
                                break;
                            case 30: case 31: case 32: case 33:
                            case 34: case 35: case 36: case 37:
                                attr &= ~(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
                                attr |= VT100_FG[arg(count)-30];
                                break;
                            case 40: case 41: case 42: case 43:
                            case 44: case 45: case 46: case 47:
                                attr &= ~(BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED);
                                attr |= VT100_BG[arg(count)-40];
                                break;
                        }
                    }
                    SetConsoleTextAttribute(stream->hOutput, attr);
                    break;
#undef  attr
            }
#undef  nvl
#undef  arg
            if (operation != OP_NONE)
            {
                VT100.state = 0;
            }
        }
    }
    if (VT100.state == 0 && b >= 0 && b < p)
    {
        WriteFile(stream->hOutput, buff+b, p - b, &bytes, 0);
    }
    return bufferLength;
}



static int consoleStreamDestroy(IOStreamPTR streamP)
{
    consoleStreamPTR stream = (consoleStreamPTR)streamP;

    if (!stream)
        return -1;

    CloseHandle(stream->hInput);
    CloseHandle(stream->hOutput);

    cmdOsFree(stream);

    return 0;
}

static int consoleStreamRead(IOStreamPTR streamP, void* bufferPtr, int bufferLength)
{
    consoleStreamPTR stream = (consoleStreamPTR)streamP;
    DWORD status, bytes;

    if (!stream)
        return -1;

    /* perform a read request */
    status = ReadFile(stream->hInput, bufferPtr, bufferLength, &bytes, 0);
    if (status == 0)
    {
        switch (GetLastError())
        {
            case ERROR_COUNTER_TIMEOUT:
            case ERROR_TIMEOUT:
            case WAIT_TIMEOUT:
                return 0;
            default:
                return -1;
        }
    }

    return bytes;
}

static int consoleStreamReadChar(IOStreamPTR streamP, void* charPtr, int timeOut)
{
    consoleStreamPTR stream = (consoleStreamPTR)streamP;
    DWORD status, bytes;

    if (!stream)
        return -1;

    if (stream->type == FILE_TYPE_CHAR)
    {
        /* console */
        while (1) {
            INPUT_RECORD irec;

            if (VT100.inputBuffer[0])
            {
                *((char*)charPtr) = VT100.inputBuffer[0];
                strcpy(VT100.inputBuffer, VT100.inputBuffer+1);
                return 1;
            }
            status = WaitForSingleObject(stream->hInput,timeOut);
            if (status == WAIT_TIMEOUT)
                return 0;
            if (status != WAIT_OBJECT_0)
                return -1;

            if (!PeekConsoleInputW(stream->hInput, &irec, 1, &bytes))
                break;
            if (!bytes)
                continue;
            if (irec.EventType == KEY_EVENT && irec.Event.KeyEvent.bKeyDown)
            {
                int k;
                if (irec.Event.KeyEvent.uChar.UnicodeChar != 0
                    && irec.Event.KeyEvent.wVirtualKeyCode != VK_ESCAPE)
                    break;
                /* handle control keys */
                for (k = 0; VT100_keys[k].sequence; k++)
                {
                    if (irec.Event.KeyEvent.wVirtualKeyCode == VT100_keys[k].wVirtualKeyCode)
                        break;
                }
                if (VT100_keys[k].sequence)
                {
                    strcpy(VT100.inputBuffer, VT100_keys[k].sequence);
                }
#if 0
                else
                {
                    fprintf(stderr, "rc=%d vk=%x vs=%x uc=%x ks=%x\n",
                        irec.Event.KeyEvent.wRepeatCount,
                        irec.Event.KeyEvent.wVirtualKeyCode,
                        irec.Event.KeyEvent.wVirtualScanCode,
                        irec.Event.KeyEvent.uChar.UnicodeChar,
                        irec.Event.KeyEvent.dwControlKeyState);
                }
#endif
            }
            ReadConsoleInputW(stream->hInput, &irec, 1, &bytes);
        }
    }
    if (stream->type == FILE_TYPE_PIPE)
    {
        status = WaitForSingleObject(stream->hInput,timeOut);
        if (status == WAIT_TIMEOUT)
            return 0;
        if (status != WAIT_OBJECT_0)
            return -1;
    }

    /* perform a read request */
    status = ReadFile(stream->hInput, charPtr, 1, &bytes, 0);
    if (status == 0)
    {
        switch (GetLastError())
        {
            case ERROR_COUNTER_TIMEOUT:
            case ERROR_TIMEOUT:
            case WAIT_TIMEOUT:
                return 0;
            default:
                return -1;
        }
    }

    return bytes;
}

static int consoleStreamReadLine(IOStreamPTR streamP, char* bufferPtr, int bufferLength)
{
    consoleStreamPTR stream = (consoleStreamPTR)streamP;
    DWORD status, bytes;

    if (!stream)
        return -1;

    /* set input mode */

    status = ReadFile(stream->hInput, bufferPtr, bufferLength, &bytes, 0);
    if (status == 0)
    {
        switch (GetLastError())
        {
            case ERROR_COUNTER_TIMEOUT:
            case ERROR_TIMEOUT:
            case WAIT_TIMEOUT:
                return 0;
            default:
                return -1;
        }
    }
    if (bytes > 0 && bufferPtr[bytes-1] == '\n')
        bufferPtr[--bytes] = 0;
    if (bytes > 0 && bufferPtr[bytes-1] == '\r')
        bufferPtr[--bytes] = 0;

    return bytes;
}

static int consoleStreamWrite(IOStreamPTR streamP, const void* bufferPtr, int bufferLength)
{
    consoleStreamPTR stream = (consoleStreamPTR)streamP;
    DWORD status, bytes = 0;

    if (!stream)
        return -1;

    if (!bufferLength)
        return 0;
    if (stream->type == FILE_TYPE_PIPE)
    {
        status = WriteFile(stream->hOutput, bufferPtr, bufferLength, &bytes, 0);
        if (status == 0)
            return -1;
    }
    if (stream->type == FILE_TYPE_CHAR)
    {
        /* terminal emulation */
        bytes = consoleStreamVT100Output(stream, (const char*)bufferPtr, bufferLength);
    }

    return (int)bytes;
}

static int consoleStreamConnected(IOStreamPTR streamP)
{
    consoleStreamPTR stream = (consoleStreamPTR)streamP;

    if (!stream)
        return -1;

    return 1;
}

static int consoleGrabStd(IOStreamPTR streamP)
{
    consoleStreamPTR stream = (consoleStreamPTR)streamP;

    if (!stream)
        return -1;

    SetStdHandle(STD_OUTPUT_HANDLE, stream->hOutput);
    SetStdHandle(STD_ERROR_HANDLE, stream->hOutput);

    return 1;
}

static int consoleSetTtyMode(IOStreamPTR streamP, int mode)
{
    consoleStreamPTR stream = (consoleStreamPTR)streamP;

    if (stream->type == FILE_TYPE_CHAR)
    {
        /* console */
        if (mode == 0)
        {
            /* raw mode */
            SetConsoleMode(stream->hInput, 0);
            SetConsoleMode(stream->hOutput,
                ENABLE_PROCESSED_OUTPUT |
                ENABLE_WRAP_AT_EOL_OUTPUT
            );
        }
        else
        {
            /* line edit mode */
            SetConsoleMode(stream->hInput,
                ENABLE_ECHO_INPUT |
                ENABLE_LINE_INPUT |
                ENABLE_PROCESSED_INPUT
            );
            SetConsoleMode(stream->hOutput,
                ENABLE_PROCESSED_OUTPUT |
                ENABLE_WRAP_AT_EOL_OUTPUT
            );
        }
    }
    if (stream->type == FILE_TYPE_PIPE)
    {
#if 0
        /* this doesn't work */
        DWORD m;
        if (mode == 0)
        {
            /* raw mode */
            m = PIPE_READMODE_BYTE | PIPE_NOWAIT;
        }
        else
        {
            /* line edit mode */
            m = PIPE_READMODE_BYTE | PIPE_WAIT;
        }
        SetNamedPipeHandleState(stream->hInput, &m, NULL, NULL);
#endif
    }
    return 0;
}

/*******************************************************************************
* cmdStreamCreateWin32Console
*
* DESCRIPTION:
*       Create win32 console stream
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       stream - console/pipe stream
*       NULL if error
*
* COMMENTS:
*       None
*
*******************************************************************************/
IOStreamPTR cmdStreamCreateWin32Console(void)
{
    consoleStreamPTR stream;
    HANDLE hInput;
    HANDLE hOutput;

    hInput = GetStdHandle(STD_INPUT_HANDLE);
    if (!hInput)
        return NULL;
    hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    if (!hOutput)
    {
        CloseHandle(hInput);
        return NULL;
    }

    /* store handle and return */
    stream = (consoleStreamPTR)cmdOsMalloc(sizeof(*stream));
    if (!stream)
    {
        CloseHandle(hInput);
        CloseHandle(hOutput);
        return NULL;
    }
    memset(stream, 0, sizeof(*stream));
    stream->common.destroy = consoleStreamDestroy;
    stream->common.read = consoleStreamRead;
    stream->common.readChar = consoleStreamReadChar;
    stream->common.readLine = consoleStreamReadLine;
    stream->common.write = consoleStreamWrite;
    stream->common.writeBuf = IOStreamGenericWriteBuf;
    stream->common.writeLine = IOStreamGenericWriteLine;
    stream->common.connected = consoleStreamConnected;
    stream->common.grabStd = consoleGrabStd;
    stream->common.setTtyMode = consoleSetTtyMode;
    stream->common.isConsole = GT_FALSE;
    stream->common.flags = 0;
    stream->hInput = hInput;
    stream->hOutput = hOutput;
    stream->type = GetFileType(hInput);
    if (stream->type == FILE_TYPE_CHAR)
    {
        consoleStreamVT100Init(stream);
        /* SetConsoleTitle("TitleString"); */
    }
    /* force lineedit mode */
    consoleSetTtyMode((IOStreamPTR)stream, 1);

    return (IOStreamPTR)stream;
}
