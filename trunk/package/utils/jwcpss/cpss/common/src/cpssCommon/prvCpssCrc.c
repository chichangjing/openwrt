/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssCrc.c
*
* DESCRIPTION:
*       This file includes function definitions for CRC 8, 16 and 32 bit.
*
*       Inorder for the user to be able to use the above functionality, the file
*       "prvCpssCrc.h" must be included.
*
*        Part of this file (CRC tables) is from open source code under the license below:
*
* libFLAC - Free Lossless Audio Codec library
* Copyright (C) 2000,2001,2002,2003,2004,2005  Josh Coalson
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
*
* - Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the
* documentation and/or other materials provided with the distribution.
*
* - Neither the name of the Xiph.org Foundation nor the names of its
* contributors may be used to endorse or promote products derived from
* this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
* CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
* PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*
*******************************************************************************/


/********* Include files ******************************************************/
#include <cpssCommon/private/prvCpssCrc.h>


/********* Defines ************************************************************/

/* x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x^1+x^0 */
#define CRC_32_POLYNOMIAL 0x04c11db7

/* x^16+x^15+x^2+x^0.*/
#define CRC_16_POLYNOMIAL 0x00008005

/* x^8+x^2+x^1+x^0 */
#define CRC_8_POLYNOMIAL  0x00000007



/********* Local variables ****************************************************/
static GT_U32 crc32Table[256] =
{
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
    0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
    0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
    0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
    0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
    0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
    0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
    0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
    0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
    0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
    0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
    0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
    0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
    0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
    0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
    0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
    0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
    0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
    0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
    0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
    0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
    0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
    0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
    0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
    0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
    0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
    0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
    0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
    0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
    0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
    0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
    0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
    0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

static GT_U16 crc16Table[256] =
{
    0x0000, 0x8005, 0x800F, 0x000A, 0x801B, 0x001E, 0x0014, 0x8011,
    0x8033, 0x0036, 0x003C, 0x8039, 0x0028, 0x802D, 0x8027, 0x0022,
    0x8063, 0x0066, 0x006C, 0x8069, 0x0078, 0x807D, 0x8077, 0x0072,
    0x0050, 0x8055, 0x805F, 0x005A, 0x804B, 0x004E, 0x0044, 0x8041,
    0x80C3, 0x00C6, 0x00CC, 0x80C9, 0x00D8, 0x80DD, 0x80D7, 0x00D2,
    0x00F0, 0x80F5, 0x80FF, 0x00FA, 0x80EB, 0x00EE, 0x00E4, 0x80E1,
    0x00A0, 0x80A5, 0x80AF, 0x00AA, 0x80BB, 0x00BE, 0x00B4, 0x80B1,
    0x8093, 0x0096, 0x009C, 0x8099, 0x0088, 0x808D, 0x8087, 0x0082,
    0x8183, 0x0186, 0x018C, 0x8189, 0x0198, 0x819D, 0x8197, 0x0192,
    0x01B0, 0x81B5, 0x81BF, 0x01BA, 0x81AB, 0x01AE, 0x01A4, 0x81A1,
    0x01E0, 0x81E5, 0x81EF, 0x01EA, 0x81FB, 0x01FE, 0x01F4, 0x81F1,
    0x81D3, 0x01D6, 0x01DC, 0x81D9, 0x01C8, 0x81CD, 0x81C7, 0x01C2,
    0x0140, 0x8145, 0x814F, 0x014A, 0x815B, 0x015E, 0x0154, 0x8151,
    0x8173, 0x0176, 0x017C, 0x8179, 0x0168, 0x816D, 0x8167, 0x0162,
    0x8123, 0x0126, 0x012C, 0x8129, 0x0138, 0x813D, 0x8137, 0x0132,
    0x0110, 0x8115, 0x811F, 0x011A, 0x810B, 0x010E, 0x0104, 0x8101,
    0x8303, 0x0306, 0x030C, 0x8309, 0x0318, 0x831D, 0x8317, 0x0312,
    0x0330, 0x8335, 0x833F, 0x033A, 0x832B, 0x032E, 0x0324, 0x8321,
    0x0360, 0x8365, 0x836F, 0x036A, 0x837B, 0x037E, 0x0374, 0x8371,
    0x8353, 0x0356, 0x035C, 0x8359, 0x0348, 0x834D, 0x8347, 0x0342,
    0x03C0, 0x83C5, 0x83CF, 0x03CA, 0x83DB, 0x03DE, 0x03D4, 0x83D1,
    0x83F3, 0x03F6, 0x03FC, 0x83F9, 0x03E8, 0x83ED, 0x83E7, 0x03E2,
    0x83A3, 0x03A6, 0x03AC, 0x83A9, 0x03B8, 0x83BD, 0x83B7, 0x03B2,
    0x0390, 0x8395, 0x839F, 0x039A, 0x838B, 0x038E, 0x0384, 0x8381,
    0x0280, 0x8285, 0x828F, 0x028A, 0x829B, 0x029E, 0x0294, 0x8291,
    0x82B3, 0x02B6, 0x02BC, 0x82B9, 0x02A8, 0x82AD, 0x82A7, 0x02A2,
    0x82E3, 0x02E6, 0x02EC, 0x82E9, 0x02F8, 0x82FD, 0x82F7, 0x02F2,
    0x02D0, 0x82D5, 0x82DF, 0x02DA, 0x82CB, 0x02CE, 0x02C4, 0x82C1,
    0x8243, 0x0246, 0x024C, 0x8249, 0x0258, 0x825D, 0x8257, 0x0252,
    0x0270, 0x8275, 0x827F, 0x027A, 0x826B, 0x026E, 0x0264, 0x8261,
    0x0220, 0x8225, 0x822F, 0x022A, 0x823B, 0x023E, 0x0234, 0x8231,
    0x8213, 0x0216, 0x021C, 0x8219, 0x0208, 0x820D, 0x8207, 0x0202
};



static GT_U8 crc8Table[256] =
{
    0x00, 0x07, 0x0e, 0x09,0x1c, 0x1b, 0x12, 0x15,0x38, 0x3f, 0x36, 0x31,
    0x24, 0x23, 0x2a, 0x2d,0x70, 0x77, 0x7e, 0x79,0x6c, 0x6b, 0x62, 0x65,
    0x48, 0x4f, 0x46, 0x41,0x54, 0x53, 0x5a, 0x5d,0xe0, 0xe7, 0xee, 0xe9,
    0xfc, 0xfb, 0xf2, 0xf5,0xd8, 0xdf, 0xd6, 0xd1,0xc4, 0xc3, 0xca, 0xcd,
    0x90, 0x97, 0x9e, 0x99,0x8c, 0x8b, 0x82, 0x85,0xa8, 0xaf, 0xa6, 0xa1,
    0xb4, 0xb3, 0xba, 0xbd,0xc7, 0xc0, 0xc9, 0xce,0xdb, 0xdc, 0xd5, 0xd2,
    0xff, 0xf8, 0xf1, 0xf6,0xe3, 0xe4, 0xed, 0xea,0xb7, 0xb0, 0xb9, 0xbe,
    0xab, 0xac, 0xa5, 0xa2,0x8f, 0x88, 0x81, 0x86,0x93, 0x94, 0x9d, 0x9a,
    0x27, 0x20, 0x29, 0x2e,0x3b, 0x3c, 0x35, 0x32,0x1f, 0x18, 0x11, 0x16,
    0x03, 0x04, 0x0d, 0x0a,0x57, 0x50, 0x59, 0x5e,0x4b, 0x4c, 0x45, 0x42,
    0x6f, 0x68, 0x61, 0x66,0x73, 0x74, 0x7d, 0x7a,0x89, 0x8e, 0x87, 0x80,
    0x95, 0x92, 0x9b, 0x9c,0xb1, 0xb6, 0xbf, 0xb8,0xad, 0xaa, 0xa3, 0xa4,
    0xf9, 0xfe, 0xf7, 0xf0,0xe5, 0xe2, 0xeb, 0xec,0xc1, 0xc6, 0xcf, 0xc8,
    0xdd, 0xda, 0xd3, 0xd4,0x69, 0x6e, 0x67, 0x60,0x75, 0x72, 0x7b, 0x7c,
    0x51, 0x56, 0x5f, 0x58,0x4d, 0x4a, 0x43, 0x44,0x19, 0x1e, 0x17, 0x10,
    0x05, 0x02, 0x0b, 0x0c,0x21, 0x26, 0x2f, 0x28,0x3d, 0x3a, 0x33, 0x34,
    0x4e, 0x49, 0x40, 0x47,0x52, 0x55, 0x5c, 0x5b,0x76, 0x71, 0x78, 0x7f,
    0x6a, 0x6d, 0x64, 0x63,0x3e, 0x39, 0x30, 0x37,0x22, 0x25, 0x2c, 0x2b,
    0x06, 0x01, 0x08, 0x0f,0x1a, 0x1d, 0x14, 0x13,0xae, 0xa9, 0xa0, 0xa7,
    0xb2, 0xb5, 0xbc, 0xbb,0x96, 0x91, 0x98, 0x9f,0x8a, 0x8d, 0x84, 0x83,
    0xde, 0xd9, 0xd0, 0xd7,0xc2, 0xc5, 0xcc, 0xcb,0xe6, 0xe1, 0xe8, 0xef,
    0xfa, 0xfd, 0xf4, 0xf3,
};



/********* Public functions ***************************************************/


/*******************************************************************************
* prvCpssCalcCrc32
*
* DESCRIPTION:
*       Calculate CRC 32 bit for input data.
*
* INPUTS:
*       crc     - CRC start value.
*       buffer  - pointer to the buffer.
*       byteNum - number of bytes in the buffer.
*
* OUTPUTS:
*       none
*
* RETURNS:
*       32 bit CRC.
*
* COMMENTS:
*   For calculation a new CRC the value of CRC should be 0xffffffff.
*
*******************************************************************************/
GT_U32 prvCpssCalcCrc32
(
    IN GT_U32 crc,
    IN GT_U8  *buffer,
    IN GT_U32 byteNum
)
{
    GT_U32 i;           /* Loop counter                      */
    GT_U8  newEntry;    /* The new entry in the lookup table */
    GT_U32 result = 0;  /* The result crc                    */


    for ( i = 0;  i < byteNum;  i++ )
    {
        newEntry = (GT_U8)( ( crc ^ (*buffer) ) & 0xff );
        crc = crc32Table[newEntry] ^ (crc >> 8);
        buffer++;
    }

    crc = crc ^ 0XFFFFFFFF;


    result |= (crc & 0xFF000000) >> 24;
    result |= (crc & 0x00FF0000) >> 8;
    result |= (crc & 0x0000FF00) << 8;
    result |= (crc & 0x000000FF) << 24;

    return result;
}

/*******************************************************************************
* prvCpssCalcCrc16
*
* DESCRIPTION:
*       Calculate CRC 16 bit for input data.
*
* INPUTS:
*       crc     - CRC start value.
*       buffer  - pointer to the buffer.
*       byteNum - number of bytes in the buffer.
*
* OUTPUTS:
*       none
*
* RETURNS:
*       32 bit CRC.
*
* COMMENTS:
*   For calculation a new CRC the value of CRC should be 0xffff.
*
*******************************************************************************/
GT_U16 prvCpssCalcCrc16
(
    IN GT_U16 crc,
    IN GT_U8  *buffer,
    IN GT_U32 byteNum
)
{
    GT_U32 i;
    GT_U32 newEntry; /* entry index in CRC table.
                      maximal value is 0xFF = 255 */

    for ( i = 0;  i < byteNum;  i++ )
    {
        newEntry = ((crc >> 8) ^ (*buffer)) & 0xff;
        crc = (GT_U16)(((crc << 8)) ^ crc16Table[newEntry]);
        buffer++;
    }

    return crc;

}

/*******************************************************************************
* prvCpssCalcCrc8
*
* DESCRIPTION:
*       Calculate CRC 8 bit for input data.
*
* INPUTS:
*       crc     - CRC start value.
*       buffer  - pointer to the buffer.
*       byteNum - number of bytes in the buffer.
*
* OUTPUTS:
*       none
*
* RETURNS:
*       32 bit CRC.
*
* COMMENTS:
*   For calculation a new CRC the value of CRC should be 0xff.
*
*******************************************************************************/
GT_U8 prvCpssCalcCrc8
(
    IN GT_U8  crc,
    IN GT_U8  *buffer,
    IN GT_U32 byteNum
)
{
    GT_U32 i;

    for ( i = 0;  i < byteNum;  i++ )
    {
        crc = crc8Table[ (crc ^ (*buffer) )];
        buffer++;
    }

    return crc;
}


/********* Private functions **************************************************/


#ifdef CREATE_CRC_TABLE /* functions below not called by anyone and so cause 
            warnings in VC, define this flag, if you need rebuild CRC tables*/

/*******************************************************************************
* swapBits
*
* DESCRIPTION: Swap bit 0 for bit 7. bit 1 for bit 6 etc.
*
* INPUTS:
*       data - data to swap.
*       num  - Number of bits to swap.
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       The new value
*
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_U32 swapBits
(
    IN GT_U32 data,
    IN GT_U8 num
)
{
    GT_U32 value = 0;
    GT_U32 i;

    /* Swap bit 0 for bit 7  */
    /* bit 1 for bit 6, etc. */
    for(i = 1; i < (GT_U32)(num + 1); i++)
    {
        if( (data & 0x1) != 0)
            value |= 1 << (num - i);
        data >>= 1;
    }

    return (value);
}

/*******************************************************************************
* prvCpssCreateCrc32Table
*
* DESCRIPTION: Create the remainders look up table to CRC 32
*
* INPUTS:
*       None.
*
* OUTPUTS:
*
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_VOID prvCpssCreateCrc32Table
(
    GT_VOID
)
{
    GT_U32 i,j;      /* Loop counter */

    for(i = 0; i < 256; i++)
    {
        crc32Table[i]= swapBits(i, 8) << 24;

        for (j = 0; j < 8; j++)
        {
            crc32Table[i] = (crc32Table[i] << 1) ^ (crc32Table[i] & (1 << 31) ?
                                                    CRC_32_POLYNOMIAL : 0);
        }

        crc32Table[i] = swapBits(crc32Table[i], 32);
    }
}

/*******************************************************************************
* prvCpssCreateCrc16Table
*
* DESCRIPTION: Create the remainders look up table to CRC 16
*
* INPUTS:
*       None.
*
* OUTPUTS:
*
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_VOID prvCpssCreateCrc16Table
(
    GT_VOID
)
{
    GT_32  i,j;      /* Loop counter */
    GT_U16 crcAccum; /* crc          */

    for ( i = 0;  i < 256;  i++ )
    {
        crcAccum = (GT_U16) (i << 8 );
        for ( j = 0;  j < 8;  j++ )
        {
            if ( (crcAccum & 0x00008000) != 0 )
                   crcAccum = (GT_U16)(( crcAccum << 1 ) ^ CRC_16_POLYNOMIAL);
            else
                   crcAccum = (GT_U16)( crcAccum << 1 );
        }
        crc16Table[i] = crcAccum;
    }


}

/*******************************************************************************
* prvCpssCreateCrc8Table
*
* DESCRIPTION: Create the remainders look up table to CRC 8
*
* INPUTS:
*       None.
*
* OUTPUTS:
*
*       None.
*
* RETURNS :
*       None.
*
* COMMENTS:
*
*
*******************************************************************************/
static GT_VOID prvCpssCreateCrc8Table
(
    GT_VOID
)
{

    GT_U32 i,j;      /* Loop counter */
    GT_U8  crcAccum; /* crc          */

    for ( i = 0;  i < 256;  i++ )
    {
        crcAccum = (GT_U8)i;
        for ( j = 0;  j < 8;  j++ )
        {
            if ( (crcAccum & 0x00000080) != 0 )
                   crcAccum = (GT_U8)(( crcAccum << 1 ) ^ CRC_8_POLYNOMIAL);
            else
                   crcAccum = (GT_U8)(( crcAccum << 1 ));
        }
        crc8Table[i] = crcAccum;
    }

}

#endif 





