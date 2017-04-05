/********************************************************************
This file contains functions and global data for
higher-level functions using MDIO access to perform resets, perform
queries of the phy, etc. that are necessary to control and read
status of the Solarflare SFX7101 and SFT9001 10GBASE-T ethernet phy.


Copyright (c) 2010, Solarflare Communications
See copyright notice at the bottom of this file for details.
********************************************************************/
#include <appDemo/boardConfig/appDemoBoardConfig.h>
#include <appDemo/boardConfig/appDemoCfgMisc.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfigDefaults.h>
#include <appDemo/sysHwConfig/appDemoDb.h>
#include <appDemo/userExit/dxCh/appDemoDxChEventHandle.h>

#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>

/* structures definition for the use of Dx Cheetah Prestera core SW */
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>

#include <cpss/generic/smi/cpssGenSmi.h>
#include <cpss/generic/phy/cpssGenPhySmi.h>
#include <cpss/dxCh/dxChxGen/phy/cpssDxChPhySmi.h>

#include <string.h>
#include <stdlib.h>


#include <appDemo/sysHwConfig/gtAppDemoSfPhyConfig.h>
#include <appDemo/sysHwConfig/gtAppDemoSfPhyIf.h>

/* port 0 ~ 63 phyAddr mapping */
GT_U8 lionMacTo3140Map[64] = {
4, 5, 6, 7, 	8, 9, 10, 11, 	12, 13, 14, 15, 	0,0,0,0,
4, 5, 6, 7,	    8, 9, 10, 11,   28, 29, 29, 30,		0,0,0,0,
4, 5, 6, 7, 	8, 9, 10, 11,	12, 13, 14, 15, 	0,0,0,0,
4, 5, 6, 7,	    0, 0,  0,  0,	 0,  0,  0,  0,		0,0,0,0
};

unsigned short SFPhyMdioRead(CTX_PTR_TYPE contextPtr, unsigned long port, unsigned long dev, 
                             unsigned long reg)
{   
	GT_U8 		devNum = 0;
   	GT_U16 		data;
   	
   	if (contextPtr){}

	cpssDxChPhyPort10GSmiRegisterRead(devNum,
                                       port,
                                       lionMacTo3140Map[port],
                                       GT_TRUE,
                                       (GT_U16)reg,
                                       (GT_U8)dev,
                                       &data);
/*										   
	if (rc != GT_OK)
		cpssOsPrintf("\n %s fail", __func__);
*/
	return data;
}

void SFPhyMdioWrite(CTX_PTR_TYPE contextPtr, unsigned long port, unsigned long dev, 
                    unsigned long reg, unsigned short value)
{
	GT_U8 		devNum = 0;

	if (contextPtr) {}
	
    cpssDxChPhyPort10GSmiRegisterWrite(devNum,
                                        port,
                                        lionMacTo3140Map[port],
                                        GT_TRUE,
                                        (GT_U16)reg,
                                        (GT_U8)dev,
                                        value);
}

unsigned short SFPhyGetAPIVersion(void)
{
    return((((unsigned short)VERSION_MAJOR)<<8) | VERSION_MINOR);
}

/****************************************************************************/

/*******************************************************************/
/*******************************************************************
  Resets
  Checking Boot Status
  Download Mode
  Low Power Mode
 *******************************************************************/

/****************************************************************************/
void SFPhySpecialSoftwareReset(CTX_PTR_TYPE contextPtr, unsigned long port)
{
    unsigned short val;
    val = SFPhyMdioRead(contextPtr, port,1,49152);
    SFPhyMdioWrite(contextPtr, port,1,49152,(unsigned short)(val | (1<<15))); /* Writes a self-
                                                               clearing reset
                                                            */
}

/*  Definitions for Boot and Flash Upload Status Register 3.53248 */
#define FATAL_BOOT_ERROR_POS (0)
#define BOOT_PROGRESS_POS    (1)
#define BOOT_COMPLETE_POS    (3)
#define CODE_STARTED_POS     (4)
#define UL_CHECKSUM_OK_POS   (5)
#define CODE_DOWNLOADED_POS  (6)
#define UL_PROGRESS_POS      (7)
#define RESET_CAUSE_POS      (8)

#define FATAL_BOOT_ERROR_MASK (0x1<<FATAL_BOOT_ERROR_POS)
#define BOOT_PROGRESS_MASK    (0x3<<BOOT_PROGRESS_POS)
#define BOOT_COMPLETE_MASK    (0x1<<BOOT_COMPLETE_POS)
#define CODE_STARTED_MASK     (0x1<<CODE_STARTED_POS)
#define UL_CHECKSUM_OK_MASK   (0x1<<UL_CHECKSUM_OK_POS)
#define CODE_DOWNLOADED_MASK  (0x1<<CODE_DOWNLOADED_POS)
#define UL_PROGRESS_MASK      (0x1<<UL_PROGRESS_POS)
#define RESET_CAUSE_MASK      (0x3<<RESET_CAUSE_POS)

/* Definitions for Boot Progress field */
#define BOOT_INIT        0x00
#define WAIT_MDIO        0x01
#define    CHECKSUM_STARTED 0x02
#define JUMPING_TO_APP   0x03

unsigned short SFPhyIsPhyInMdioDownloadMode(CTX_PTR_TYPE contextPtr, 
                                            unsigned long port)
{    
    unsigned short val;
    val = SFPhyMdioRead(contextPtr, port,3,53248);
    if (((val & BOOT_PROGRESS_MASK)>>BOOT_PROGRESS_POS) == WAIT_MDIO)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

unsigned short SFPhyGetPhyResetReason(CTX_PTR_TYPE contextPtr, 
                                      unsigned long port)
{
    unsigned short val;
    val = SFPhyMdioRead(contextPtr, port,3,53248);
    return (unsigned short)((val & RESET_CAUSE_MASK)>>RESET_CAUSE_POS);
}

unsigned short SFPhyDidPhyAppCodeStart(CTX_PTR_TYPE contextPtr, 
                                       unsigned long port)
{
    unsigned short val;
    val = SFPhyMdioRead(contextPtr, port,3,53248);
    return (unsigned short)((val & CODE_STARTED_MASK)>>CODE_STARTED_POS);
}

unsigned short SFPhyFlashUploadedChecksumGood(CTX_PTR_TYPE contextPtr, 
                                              unsigned long port)
{
    unsigned short val;
    val = SFPhyMdioRead(contextPtr, port,3,53248);
    if (((val & BOOT_PROGRESS_MASK)>>BOOT_PROGRESS_POS) == JUMPING_TO_APP)
    {
        return 1;
    }
    else
    {
        return 0;
    }    
}

unsigned short SFPhyCheckForFatalBootError(CTX_PTR_TYPE contextPtr,
                                         unsigned long port)
{
    unsigned short val;
    val = SFPhyMdioRead(contextPtr, port,3,53248);
    return (unsigned short)((val & FATAL_BOOT_ERROR_MASK)>>FATAL_BOOT_ERROR_POS);    
}

unsigned short SFPhyIsPhyReadyAfterReset(CTX_PTR_TYPE contextPtr, unsigned long port)              
{
    unsigned short val, result;
    
    val = SFPhyMdioRead(contextPtr, port,1,0);
    result = (val & 0x8000);    /* check if firmware is downloaded and ready to run */
    if(result)
    {
        return 0;   /* if still in reset return '0' */
    }
    else
    {
        return 1;   /* if Phy is in normal operation */
    }
}

unsigned short SFPhyIsPhyReadyAfterHWReset(CTX_PTR_TYPE contextPtr, unsigned long port)
{
    unsigned short val, result;
    val = SFPhyMdioRead(contextPtr, port,1,49152);
    result = (val & 0x8000);    /* check if bit 15 is '1' */
    if(result)
    {
        return 0;   /* if still in reset return '0' */
    }
    else
    {
        return 1;
    }
}

void SFPhyPutInFwLowPowerModeFirmware(CTX_PTR_TYPE contextPtr, unsigned long port)
{
    unsigned short val;
    val = SFPhyMdioRead(contextPtr, port,1,49152);
    SFPhyMdioWrite(contextPtr, port,1,49152,(unsigned short)(val | (1<<12)));     
}

void SFPhyRemoveFwLowPowerMode(CTX_PTR_TYPE contextPtr, unsigned long port)
{
    SFPhySpecialSoftwareReset(contextPtr, port);  
}

/****************************************************************************/
/******************************************************************* 
 Firmware Version
 *******************************************************************/
/****************************************************************************/
void SFPhyGetFirmwareVersion(CTX_PTR_TYPE contextPtr, unsigned long port, 
                             unsigned short *major, unsigned short *minor, 
                             unsigned short *inc, unsigned short *test)
{
    unsigned short reg_49190, reg_49191;
    
    reg_49190 = SFPhyMdioRead(contextPtr, port,1,49190);
    *major = (reg_49190 & 0xFF00) >> 8;
    *minor = (reg_49190 & 0x00FF);
    reg_49191 = SFPhyMdioRead(contextPtr, port,1,49191);
    *inc = (reg_49191 &0xFF00) >> 8;
    *test = (reg_49191 &0x00FF);
}

unsigned short SFPhyIsSolarflarePhy(CTX_PTR_TYPE contextPtr, unsigned long port, 
                                    unsigned short *modelNo, unsigned short *revNo)
{
    unsigned short reg_1_2, reg_1_3;
 
    reg_1_3 = SFPhyMdioRead(contextPtr, port,1,3);
    *revNo = (reg_1_3 & 0x000F);
    *modelNo = (reg_1_3 & 0x03F0) >> 4;
    reg_1_2 = SFPhyMdioRead(contextPtr, port,1,2);

    /*  Check the OUI bits to see if they match SolarFlare's OUI */
    if(reg_1_2 == 0x0140 &&
       (((reg_1_3 & 0xFC00)>>10) == 0x16))
    {
        return 1;
    }
    else
    {
        return 0;
    }

}

/****************************************************************************/
/*******************************************************************
  Enabling speeds
  Reading enabled speeds
 *******************************************************************/
/****************************************************************************/
void SFPhyEnableSpeeds(CTX_PTR_TYPE contextPtr, unsigned long port,
                       unsigned short speed_bits, unsigned short anRestart)
{
    unsigned short val;
    
    if(speed_bits == SPEED_OFF)
    {
        val = SFPhyMdioRead(contextPtr, port,1,49192);
        SFPhyMdioWrite(contextPtr, port,1,49192,(unsigned short)(val & speed_bits));
    }
    else
    {
        SFPhyMdioWrite(contextPtr, port,1,49192,speed_bits);
    }

    if(anRestart)
    {
        SFPhyAutonegEnable(contextPtr, port);
        SFPhyAutonegRestart(contextPtr, port);
    }

}

unsigned short SFPhyGetSpeedsEnabled(CTX_PTR_TYPE contextPtr, unsigned long port)
{
    unsigned short val, result;
    val = SFPhyMdioRead(contextPtr, port,1,49192);
    result = (val & 0x010E);
    return result;
}

/****************************************************************************/
/*******************************************************************
   802.3 Clause 28
   Clause 45 Autoneg Related Control & Status
 *******************************************************************/
/****************************************************************************/
void SFPhyAutonegRestart(CTX_PTR_TYPE contextPtr, unsigned long port)
{
    unsigned short val;
    
    val = SFPhyMdioRead(contextPtr, port,7,0);
    SFPhyMdioWrite(contextPtr, port,7,0,(unsigned short)(val | 0x0200));    /*  restart AN (bit 9) */
    
}

void SFPhyAutonegDisable(CTX_PTR_TYPE contextPtr, unsigned long port)
{
    unsigned short val;
    val = SFPhyMdioRead(contextPtr, port,7,0);
    SFPhyMdioWrite(contextPtr, port,7,0,(unsigned short)(val & 0xEFFF));    /* disable autoneg (bit 12 = 0) */
}

void SFPhyAutonegEnable(CTX_PTR_TYPE contextPtr, unsigned long port)
{
    unsigned short val;
    val = SFPhyMdioRead(contextPtr, port,7,0);
    SFPhyMdioWrite(contextPtr, port,7,0,(unsigned short)(val | (1<<12))); /* enable autoneg (bit 12 = 1) */
}

void SFPhyAutonegForceMaster(CTX_PTR_TYPE contextPtr, unsigned long port, unsigned short anRestart)
{
    unsigned short val;
    if(anRestart)
    {
        val = SFPhyMdioRead(contextPtr, port,7,32);
        /* set the manual master/slave bit to '1'
           and bit 14 to '1' forces phy to master */
        SFPhyMdioWrite(contextPtr, port,7,32,(unsigned short)(val | 0xC000)); 
        SFPhyAutonegEnable(contextPtr, port);
        SFPhyAutonegRestart(contextPtr, port);
    }else
    {
        val = SFPhyMdioRead(contextPtr, port,7,32);
        /* set the manual master/slave bit to '1'
           and bit 14 to '1' forces phy to master */
        SFPhyMdioWrite(contextPtr, port,7,32,(unsigned short)(val | 0xC000));  
    }
}

void SFPhyAutonegForceSlave(CTX_PTR_TYPE contextPtr, unsigned long port, unsigned short anRestart)
{
    unsigned short val;
    if(anRestart)
    {
        val = SFPhyMdioRead(contextPtr, port,7,32);
        /* set the manual master/slave bit to '1' */
        SFPhyMdioWrite(contextPtr, port,7,32,(unsigned short)(val | (1<<15)));    
        val = SFPhyMdioRead(contextPtr, port,7,32);
        /* bit 14 to '0' forces phy to slave */
        SFPhyMdioWrite(contextPtr, port,7,32,(unsigned short)(val & 0xBFFF));   
        SFPhyAutonegEnable(contextPtr, port);
        SFPhyAutonegRestart(contextPtr, port);
    }else
    {
        val = SFPhyMdioRead(contextPtr, port,7,32);        
        /* set the manual master/slave bit to '1' */
        SFPhyMdioWrite(contextPtr, port,7,32,(unsigned short)(val | (1<<15)));    
        val = SFPhyMdioRead(contextPtr, port,7,32);
        /* bit 14 to '0' forces phy to slave */
        SFPhyMdioWrite(contextPtr, port,7,32,(unsigned short)(val & 0xBFFF));   
    }
}

void SFPhyAutonegSetAutoMasterSlave(CTX_PTR_TYPE contextPtr, unsigned long port,
                             unsigned short anRestart)                                    
{
    unsigned short val;
    if(anRestart)
    {
        val = SFPhyMdioRead(contextPtr, port,7,32);
        /* set the manual master/slave bit to '0' */
        SFPhyMdioWrite(contextPtr, port,7,32,(unsigned short)(val & 0x7FFF));   
        SFPhyAutonegEnable(contextPtr, port);
        SFPhyAutonegRestart(contextPtr, port);
    }else
    {
        val = SFPhyMdioRead(contextPtr, port,7,32);
        /* set the manual master/slave bit to '0' */
        SFPhyMdioWrite(contextPtr, port,7,32,(unsigned short)(val & 0x7FFF));   
    }
}


void SFPhyDisablePhy(CTX_PTR_TYPE contextPtr, unsigned long port, unsigned short anRestart)
{
    if(anRestart)
    {
        SFPhyEnableSpeeds(contextPtr, port, SPEED_OFF, 1);
        SFPhyAutonegDisable(contextPtr, port);
    }else
    {
        SFPhyAutonegDisable(contextPtr, port);
    }
}

void SFPhyAutonegSetMasterPreference(CTX_PTR_TYPE contextPtr, unsigned long port,
                                     unsigned short anRestart)                                     
{
    unsigned short val;
    if(anRestart)
    {
        val = SFPhyMdioRead(contextPtr, port,7,32);
        /* set the manual master/slave bit to '0' */
        SFPhyMdioWrite(contextPtr, port,7,32,(unsigned short)(val & 0x7FFF));   
        val = SFPhyMdioRead(contextPtr, port,7,32);
        /* bit 13 to '1' chooses multi port. Preference is Master */
        SFPhyMdioWrite(contextPtr, port,7,32,(unsigned short)(val | (1<<13)));    
        SFPhyAutonegEnable(contextPtr, port);
        SFPhyAutonegRestart(contextPtr, port);
    }else
    {
        val = SFPhyMdioRead(contextPtr, port,7,32);
        /* set the manual master/slave bit to '0' */
        SFPhyMdioWrite(contextPtr, port,7,32,(unsigned short)(val & 0x7FFF));   
        val = SFPhyMdioRead(contextPtr, port,7,32);
        /* bit 13 to '1' chooses multi port. Preference is Master */
        SFPhyMdioWrite(contextPtr, port,7,32,(unsigned short)(val | (1<<13)));    
    }
}

void SFPhyAutonegSetSlavePreference(CTX_PTR_TYPE contextPtr, unsigned long port,
                                    unsigned short anRestart)
                                    
{
    unsigned short val;
    if(anRestart)
    {
        val = SFPhyMdioRead(contextPtr, port,7,32);        
        /* set the manual master/slave bit to '0' */
        SFPhyMdioWrite(contextPtr, port,7,32,(unsigned short)(val & 0x7FFF)); 
        val = SFPhyMdioRead(contextPtr, port,7,32);
        /* bit 13 to '0' chooses single port. Preference is Slave */
        SFPhyMdioWrite(contextPtr, port,7,32,(unsigned short)(val & 0xDFFF)); 
        SFPhyAutonegEnable(contextPtr, port);
        SFPhyAutonegRestart(contextPtr, port);
    }else
    {
        val = SFPhyMdioRead(contextPtr, port,7,32);
        /* set the manual master/slave bit to '0' */
        SFPhyMdioWrite(contextPtr, port,7,32,(unsigned short)(val & 0x7FFF));   
        val = SFPhyMdioRead(contextPtr, port,7,32);
        /* bit 13 to '0' chooses single port. Preference is Slave */
        SFPhyMdioWrite(contextPtr, port,7,32,(unsigned short)(val & 0xDFFF)); 
    }
}

void SFPhyGetANCompleteAndLASIAlarmStatus(CTX_PTR_TYPE contextPtr, 
                                          unsigned long port, 
                                          unsigned short *latchedValue, 
                                          unsigned short *currentValue) 
{
    /* reads it once to get the latched value */
    *latchedValue = SFPhyMdioRead(contextPtr, port,1,36869);    
    /* reads it again to get the current value */
    *currentValue = SFPhyMdioRead(contextPtr, port,1,36869);    
}

unsigned short SFPhyIsAutonegComplete(CTX_PTR_TYPE contextPtr, unsigned short latchedValue)
{
    unsigned short result;
    /* bit 3 is the autoneg complete bit in Lasi status register 1.36869 (check that bit) */
    result = (latchedValue & 0x0008)>>3;
    return result;      /* if result is '1' then complete */
}

unsigned short SFPhyGetMasterSlaveConfigFault(CTX_PTR_TYPE contextPtr, unsigned long port)
{
    unsigned short val, result;
    val = SFPhyMdioRead(contextPtr, port,7,33);
    result = (val & 0x8000) >> 15;
    return result;  /* if result is '1' then fault is detected */
}

unsigned short SFPhyReadMasterOrSlaveBit(CTX_PTR_TYPE contextPtr, unsigned long port)
{
    unsigned short val, result;
    val = SFPhyMdioRead(contextPtr, port,7,33);
    result = (val & 0x4000) >> 14;
    return result;  /* if result is '1' then local PHY is a Master */
}

unsigned short SFPhyAutonegIsResolutionMasterOrSlave(CTX_PTR_TYPE contextPtr, unsigned long port)
{
    unsigned short result;

    if(SFPhyAutonegIsSpeedResolutionDone(contextPtr, port))
    {
        result = SFPhyReadMasterOrSlaveBit(contextPtr, port);
        return result;
    }else
    {
        result = 0xFF;
        return result;
    }
}

unsigned short SFPhyAutonegIsSpeedResolutionDone(CTX_PTR_TYPE contextPtr, unsigned long port)
{
    unsigned short val, result;
    val = SFPhyMdioRead(contextPtr, port,1,49192);
    result = (val & 0x00F0) >> 4;   /* bits 7:4 gives the speed */
    if(result)
    {
        return 1;
    }else
    {
        /* speed is unknown - either autoneg is not completeted yet or the 
           config is not set yet */
        return 0;   
    }
}

unsigned short SFPhyGetAutonegSpeedResolution(CTX_PTR_TYPE contextPtr, unsigned long port)
{
    unsigned short val, result;
    val = SFPhyMdioRead(contextPtr, port,1,49192);
    result = (val & 0x00F0) >> 4;   /* bits 7:4 gives the speed */
    return result;  /* returns the 4 bits */
}

void SFPhySetPauseAdvertisement(CTX_PTR_TYPE contextPtr, unsigned long port,
                                unsigned short anRestart,
                                unsigned short pauseType)
{
    unsigned short val;

    val = SFPhyMdioRead(contextPtr, port,7,16);
    val &= 0xF3FF; /*  clear both bits */

    if(pauseType == SYM_PAUSE)
    {
        /* set bit 10 (A5 in the tech bit field) */
        SFPhyMdioWrite(contextPtr, port, 7, 16, (unsigned short)(val|0x0400));    
    }else if(pauseType == ASYM_PAUSE)
    {
        /* set bit 11 (A6 in the tech bit field) */
        SFPhyMdioWrite(contextPtr, port, 7, 16, (unsigned short)(val|0x0800));    
    }else if(pauseType == SYM_ASYM_PAUSE)
    {
        /* set bit 10 and bit11 (A5,A6 in the tech bit field) */
        SFPhyMdioWrite(contextPtr, port, 7, 16, (unsigned short)(val|0x0C00));    
    }
    else if (pauseType == CLEAR_PAUSE)
    {
        /*  clears both bits */
        SFPhyMdioWrite(contextPtr, port, 7, 16, val);   
    }

    if(anRestart)
    {
        SFPhyAutonegEnable(contextPtr, port);
        SFPhyAutonegRestart(contextPtr, port);
    }
}

unsigned short SFPhyGetLPAdvertisedPause(CTX_PTR_TYPE contextPtr, unsigned long port)
{
    unsigned short val, result;
    val = SFPhyMdioRead(contextPtr, port,7,19);
    result = (val & 0x0C00) >> 10;
    return result;
}

/****************************************************************************/
/*******************************************************************
   Link Alarm Status Indicator (LASI)
 *******************************************************************/
/****************************************************************************/
unsigned short SFPhyLasiQueryLinkChangedState(CTX_PTR_TYPE contextPtr, 
                                              unsigned short latchedValue)
{
    unsigned short result;
    result = latchedValue & 0x0001;
    return result;  
    /*  If'1'the link has either gone down or up. '0' means no change in state since last read */ 

}


unsigned short SFPhyLasiIsLinkUp(CTX_PTR_TYPE contextPtr, unsigned long port,
                                 unsigned short mac_interface,
                                 unsigned short speed)
{
    unsigned short result;
    unsigned short speedResolution;

    /*  First check if phy is in the middle of autoneg, if so, link */
    /*  is definitely  not up */
    speedResolution = SFPhyGetAutonegSpeedResolution(contextPtr, port);
    if (speedResolution == AN_IN_PROGRESS)
    {
        return SPEED_NOT_RESOLVED;
    }

    if(speedResolution == speed)   /*  if the speed resolved is the input speed */
    {
        result = SFPhyLasiIsBaseTUp(contextPtr, port, speed);
        if (result == 1) /*  base-t link is up  */
        {
            result = SFPhyLasiIsMacUp(contextPtr, port, mac_interface, speed);
            return result;
        }
        else
        {
            return result;
        }
    }else
    {
        return SPEED_MISMATCH;   /* if the speed enabled is not the input speed */
    }
}

unsigned short SFPhyLasiIsMacUp(CTX_PTR_TYPE contextPtr, unsigned long port,
                                unsigned short mac_interface,
                                unsigned short speed)
{
    unsigned short val, result;
    unsigned short speedResolution;

    /*  First check if phy is in the middle of autoneg, if so, mac */
    /*  is definitely  not up */
    speedResolution = SFPhyGetAutonegSpeedResolution(contextPtr, port);
    if (speedResolution == AN_IN_PROGRESS)
    {
        return SPEED_NOT_RESOLVED;
    }
    
    if(speedResolution == speed)   /*  if the speed resolved is the input speed */
    {
        if(speed == SPEED_RES_10GIG)
        {
            if(mac_interface == XAUI_INTERFACE)
            {
                val = SFPhyMdioRead(contextPtr, port,4, 24);
                result = (val & 0x1000) >> 12;
                return result;  /* '1' xuai lane aligned so link is up */

            }
            else if(mac_interface == XFI_INTERFACE)
            {
                val = SFPhyMdioRead(contextPtr, port,31, 32);   
                result = val & 0x0001;
                return result;  /*  '1' is block lock, '0' is no block lock */
            }
            else
            {
                return MISC_ERROR;    /* in valid input */
            }
        
        }
        else if((speed == SPEED_RES_1GIG_FD) || 
                (speed == SPEED_RES_100M_HD) || 
                (speed == SPEED_RES_100M_FD))
        {
            result = SFPhyLasiIsSgmiiUp(contextPtr, port, mac_interface);
            return result; /* if '1' interface is up else dow */

        }else
        {
            return MISC_ERROR;    /* invalid input */
        }
    }
    else
    {
        return SPEED_MISMATCH;   /* if the speed enabled is not the input speed */
    }
}

unsigned short SFPhyLasiIsBaseTUp(CTX_PTR_TYPE contextPtr, unsigned long port,
                                  unsigned short speed)                                  
{
    unsigned short val, result;
    unsigned short speedResolution;

    /*  First check if phy is in the middle of autoneg, if so, base-t */
    /*  is definitely  not up */
    speedResolution = SFPhyGetAutonegSpeedResolution(contextPtr, port);
    if (speedResolution == AN_IN_PROGRESS)
    {
        return SPEED_NOT_RESOLVED;
    }
    
    if(speedResolution == speed)
    {
        if (speed == SPEED_RES_10GIG)
        {
            val = SFPhyMdioRead(contextPtr, port,3, 32);            
            result = val & 0x0001;  /* bit '0' */   
            return result;  /* '1' if PCS block lock, '0' if there is no block lock */ 
        }
        else if((speed == SPEED_RES_1GIG_FD) || 
                (speed == SPEED_RES_100M_HD) || 
                (speed == SPEED_RES_100M_FD))
        {
        
            val = SFPhyMdioRead(contextPtr, port,29, 49153);
            result = (val & 0x0004) >> 2;   /* bit 2 */
            return result;  /* '1' is link up, '0' is link down */
        }
        else
        {
            return MISC_ERROR;    /* in valid input */
        }
    }else
    {
        return SPEED_MISMATCH;   /* if the speed enabled is not the input speed */
    }   
}

unsigned short SFPhyLasiIsSgmiiUp(CTX_PTR_TYPE contextPtr, unsigned long port,
                                  unsigned short mac_interface) 
                                  
{
    unsigned short val, result;
    if (mac_interface == XAUI_INTERFACE)
    {
        val = SFPhyMdioRead(contextPtr, port,29,49184);
        result = (val & 0x0004) >> 2;   /* bit 2 */
        return result;  /* if '1' interface is up else down */
    }
    else if(mac_interface == XFI_INTERFACE)
    {
        val = SFPhyMdioRead(contextPtr, port,29,49185); /* this is only in rev B */
        result = (val & 0x0004) >> 2;   /* bit 2 */
        return result;  /* if '1' interface is up else down */
    }else
    {
        return MISC_ERROR;    /* in valid input */
    }
}

int SFPhyIsSFT9001RevBorLater(CTX_PTR_TYPE contextPtr, unsigned long port)
{
    unsigned short val, val2, val3;

    /*  read the device/package id and look at the manufacturer's model number */
    val = SFPhyMdioRead(contextPtr,port,1,3); 
    val >>= 4;
    val &= 0x003F; /*  it's bits 9-4 */
    if (val >= 8)
    {
        /*  It's an SFT9001 or SFT910X */
        val = SFPhyMdioRead(contextPtr, port,3,53249); /*  read the chip id */
        val &= 0x000F; /*  it's in bits 3-0 */
        if (val <= 2)
        {
            val2 = SFPhyMdioRead(contextPtr,port,1,49216); /* read the new spare LL register */
            val3 = SFPhyMdioRead(contextPtr,port,1,49217); /* read the new spare LH register */
            if (val2 == 0xFFFF && val3 == 0x0000)
            {
                return 1; /* It's a SFT910X part */
            }
            else
            {
                return 0; /* it's an SFT9001 Rev AX  */
            }
        }
        else
        {
            return 1; /*  it's an SFT9001 Rev BX or later */
        }
    }
    else
    {
        /*  It's either a SFX7101 or an SFT9001 Rev AX with no firmware running */
        return 0;
    }
}

#define SFT910X_PHY_CONFIGURATION_RDY_POS   (14)
#define SFT9001_PHY_CONFIGURATION_RDY_POS   (12)

#define SFT910X_PHY_CONFIGURATION_RDY_MASK  (0x1<<SFT910X_PHY_CONFIGURATION_RDY_POS)
#define SFT910X_PHY_CONFIGURATION_MASK      (0x1F)
#define SFT9001_PHY_CONFIGURATION_RDY_MASK  (0x1<<SFT9001_PHY_CONFIGURATION_RDY_POS)
#define SFT9001_PHY_CONFIGURATION_MASK      (0x7)

unsigned short SFPhySetDevCfgSFT910X(CTX_PTR_TYPE contextPtr, 
                                        unsigned long port, 
                                        SFT910X_DEVCFG_TYPE dev_cfg)
{
    unsigned short val;
    
    val = SFPhyMdioRead(contextPtr, port,1,49154);  /* PHY configuration register */
    if (val & SFT910X_PHY_CONFIGURATION_RDY_MASK)
    {
        SFPhyMdioWrite(contextPtr, port,1,49154, (unsigned short)((val & ~SFT910X_PHY_CONFIGURATION_MASK) | (dev_cfg & SFT910X_PHY_CONFIGURATION_MASK)));
        return(1);  /* PHY configuration successful */
    }
    else
    {
        return(0);  /* PHY not ready for configuration */
    }
}

SFT910X_DEVCFG_TYPE SFPhyGetDevCfgSFT910X(CTX_PTR_TYPE contextPtr, 
                                            unsigned long port)
{
    unsigned short val;
    val = SFPhyMdioRead(contextPtr, port,1,49154);
    return((SFT910X_DEVCFG_TYPE)(val & SFT910X_PHY_CONFIGURATION_MASK));
}

unsigned short SFPhySetDevCfgSFT9001(CTX_PTR_TYPE contextPtr, 
                                        unsigned long port, 
                                        SFT9001_DEVCFG_TYPE dev_cfg)
{
    unsigned short val;
    
    val = SFPhyMdioRead(contextPtr, port,1,49154);  /* PHY configuration register */
    if (val & SFT9001_PHY_CONFIGURATION_RDY_MASK)
    {
        SFPhyMdioWrite(contextPtr, port,1,49154, (unsigned short)((val & ~SFT9001_PHY_CONFIGURATION_MASK) | (dev_cfg & SFT9001_PHY_CONFIGURATION_MASK)));
        return(1);  /* PHY configuration successful */
    }
    else
    {
        return(0);  /* PHY not ready for configuration */
    }
}

SFT9001_DEVCFG_TYPE SFPhyGetDevCfgSFT9001(CTX_PTR_TYPE contextPtr, 
                                        unsigned long port)
{
    unsigned short val;

    val = SFPhyMdioRead(contextPtr, port,1,49154);
    return((SFT9001_DEVCFG_TYPE)(val & SFT9001_PHY_CONFIGURATION_MASK));
}

void SFPhyMDIXControl(CTX_PTR_TYPE contextPtr, unsigned long port,
         unsigned short MDIXOptions, unsigned short anRestart)
{
    unsigned short val;

    val = SFPhyMdioRead(contextPtr, port,7,49157);
    val = val | MDIXOptions;
    SFPhyMdioWrite(contextPtr, port, 7, 49157, val);
    if(anRestart)
    {
        SFPhyAutonegEnable(contextPtr, port);
        SFPhyAutonegRestart(contextPtr, port);
    }
}

void SFPhyForce100BTMode(CTX_PTR_TYPE contextPtr, unsigned long port,
                      unsigned short mode, unsigned short anRestart)
{
    unsigned short val;

    val = SFPhyMdioRead(contextPtr, port,7,49157);
    if(mode == ENABLE_FORCING)
    {
        val = val | 0x0020;
    }
    else
    {
        val = val & 0xFFDF;
    }
    SFPhyMdioWrite(contextPtr, port, 7, 49157, val);
    if(anRestart)
    {
        SFPhyAutonegEnable(contextPtr, port);
        SFPhyAutonegRestart(contextPtr, port);
    }
}
/********************************************************************
Copyright (c) 2010, Solarflare Communications

Redistribution and use in source form, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions are permitted only for the express purpose of
executing Solarflare Communications provided binary code.

2. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR OR HIS EMPLOYER BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/
