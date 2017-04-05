#include <madCopyright.h>

/********************************************************************************
* madSysConfig.c
*
* DESCRIPTION:
*       API definitions for system configuration, and enabling.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#include <madApi.h>
#include <madApiInternal.h>
#include <madDrvConfig.h>
#include <madDevConfig.h>
#include <madUtils.h>
#include <madHwCntl.h>
#include <madSem.h>
#include <madPlatformDeps.h>
#ifdef MAD_DSP_VCT
#include <madDSPVctApi.h>
#include <madDSPVct.h>
#endif

extern int madSmiIfReadRegister (MAD_DEV     *dev, unsigned int smiAddr, 
                    unsigned int reg, unsigned short * value);
extern int madSmiIfWriteRegister (MAD_DEV     *dev, unsigned int smiAddr, 
                    unsigned int reg, unsigned short value);


static MAD_BOOL fnRegister(MAD_DEV *dev, MAD_BSP_FUNCTIONS* pBSPFunctions);

/*******************************************************************************
* mdLoadDriver
*
* DESCRIPTION:
*       Marvell Alaska Driver Initialization Routine. 
*       This is the first routine that needs be called by system software. 
*       It takes *cfg from system software, and retures a pointer (*dev) 
*       to a data structure which includes infomation related to this Marvell Phy
*       device. This pointer (*dev) is then used for all the API functions. 
*       The following is the job performed by this routine:
*           1. store SMI read/write function into the given MAD_DEV structure
*           2. look for the Marvell Device
*           3. run Device specific initialization routine
*           4. disable Marvell Device interrupt
*           5. create semaphore if required
*
* INPUTS:
*       cfg  - Holds device configuration parameters provided by system software.
*
* OUTPUTS:
*       dev  - pointer to Holds device information to be used for each API call.
*
* RETURNS:
*       MAD_OK               - on success
*       MAD_[ERROR_STATUS]  - on error
*
* COMMENTS:
*       mdUnloadDriver is also provided to do driver cleanup.
*
*******************************************************************************/
MAD_STATUS mdLoadDriver
(
    IN  MAD_SYS_CONFIG  *cfg,
    OUT MAD_DEV         *dev
)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_INT_TYPE intType;

    MAD_DBG_INFO(("madLoadDriver Called.\n"));

    /* Check for parameters validity        */
    if(dev == NULL)
    {
        MAD_DBG_ERROR(("driver struture is NULL.\n"));
        return MAD_API_ERR_DEV;
    }

    /* Check for parameters validity        */
    if(cfg == NULL)
    {
        MAD_DBG_ERROR(("sys config is NULL.\n"));
        return MAD_API_ERR_SYS_CFG;
    }
    /* The initialization was already done. */
    if(dev->devEnabled)
    {
        MAD_DBG_ERROR(("Device Driver already loaded.\n"));
        return MAD_API_ERR_DEV_ALREADY_EXIST;
    }

    if(fnRegister(dev,&(cfg->BSPFunctions)) != MAD_TRUE)
    {
       MAD_DBG_ERROR(("madRegister Failed.\n"));
       return MAD_API_ERR_REGISTERD_FUNC;
    }

    dev->baseRegAddr = (MAD_U8)cfg->smiBaseAddr;

    /* Initialize the driver    */
    retVal = madDriverConfig(dev);
    if(retVal != MAD_OK)
    {
        MAD_DBG_ERROR(("Initialize driverConfig Failed.\n"));
        return (retVal | MAD_API_FAIL_INIT_DRV_CFG);
    }

    /* Initialize dev fields.         */
    dev->devGroup = 0;
    dev->flags = 0;

    /* Get the number of ports */
    switch (dev->deviceId)
    {
        case MAD_88E10X0:   
        case MAD_88E10X0S:   
        case MAD_88E1011:   
                dev->numOfPorts = 1;
                dev->validPortVec = 1;
                break;

        case MAD_88E104X:   
                dev->numOfPorts = 4;
                dev->validPortVec = 0xf;
                break;
        case MAD_88E1116:
                dev->numOfPorts = 1;
                dev->validPortVec = 1;
                dev->flags = MAD_FLAG_SHORT_CABLE_FIX;  
                break;

        case MAD_88E3016:
/*        case MAD_88E3019: */
                dev->numOfPorts = 1;
                dev->validPortVec = 1;
                break;

        case MAD_88E3082:
                dev->numOfPorts = 8;
                dev->validPortVec = 0xff;
                break;

        case MAD_88E1121:
                dev->numOfPorts = 2;
                dev->validPortVec = 0x3;
                if (dev->revision > 1)
                  dev->flags = MAD_FLAG_SHORT_CABLE_FIX; 
              break;

        case MAD_88E1149:
        case MAD_88E1149R:
        case MAD_SWG65G : 
        case MAD_MELODY : 
        case MAD_88E1240:
        case MAD_88E1340S:
        case MAD_88E1340:
        case MAD_88E1340M:
        case MAD_88E1540:
        case MAD_88E1545:
        case MAD_88E1548:
                dev->numOfPorts = 4;
                dev->validPortVec = 0xf;
                dev->flags = MAD_FLAG_SHORT_CABLE_FIX; 
              break;

        case MAD_88E1680:
        case MAD_88E1680M:
                dev->numOfPorts = 8;
                dev->validPortVec = 0xff;
                dev->flags = MAD_FLAG_SHORT_CABLE_FIX; 
              break;
#if 1
        case 0:   
			if(cfg->switchType == MAD_SYS_SW_TYPE_1)
			{
                dev->numOfPorts = 5;
                dev->validPortVec = 0x18;
                dev->baseRegAddr = 0;
                dev->flags = MAD_FLAG_SHORT_CABLE_FIX; 
			    dev->deviceId = MAD_SW1680;
			}
			break;
#endif

		case MAD_88E151x:
			{
				MAD_U16 data;
                dev->numOfPorts = 1;
                dev->validPortVec = 1;
                dev->flags = MAD_FLAG_SHORT_CABLE_FIX; 
 /* to get sub_device ID. For some phy have same device ID, but use register 30_18 to set differnt type */
#if 1
    retVal = madSmiIfWriteRegister(dev,dev->baseRegAddr,MAD_REG_PAGE_ADDR,18);
    if (retVal != MAD_OK)
    {
        MAD_DBG_ERROR(("SMI Write Failed\n"));
        return MAD_DRV_FAIL_WRITE_REG;
    }

    retVal = madSmiIfReadRegister(dev,dev->baseRegAddr,30,&data);
    if (retVal != MAD_OK)
    {
        MAD_DBG_ERROR(("SMI Read Failed\n"));
        return MAD_DRV_FAIL_READ_REG;
    }
    dev->subDevId = madShort2Char(data);
#endif
			}
              break;
        case MAD_88E1181:
                dev->numOfPorts = 8;
                dev->validPortVec = 0xff;
                if (dev->revision >= 4)
                {
                    dev->flags = MAD_FLAG_SHORT_CABLE_FIX;
                }
                break;

        case MAD_88E1111:
                dev->numOfPorts = 1;
                dev->validPortVec = 1;
            break;
        case MAD_88E1112:
                dev->numOfPorts = 1;
                dev->validPortVec = 1;
            break;
        case MAD_88E114X:
                dev->numOfPorts = 4;
                dev->validPortVec = 0xf;
            break;
        case MAD_88E1119R:
        case MAD_88E1310:
                dev->numOfPorts = 1;
                dev->validPortVec = 1;
                break;

        default:
                MAD_DBG_ERROR(("Unknown Device(%#x). Initialization failed\n",dev->deviceId));
                dev = NULL;
                return MAD_API_UNKNOWN_DEV;
    }

    if(madFindPhyInformation(dev,&dev->phyInfo) != MAD_OK)
    {
        MAD_DBG_ERROR(("Unknown Device Information(%#x). Initialization failed\n",dev->deviceId));
        return MAD_API_UNKNOWN_DEV;
    }

    retVal = madDevPreInit(dev);
    if(retVal != MAD_OK)
    {
        MAD_DBG_ERROR(("device Pre Init Failed.\n"));
        return (retVal | MAD_API_FAIL_PRE_INIT_DEV);
    }

    /* disable interrupt */
    intType.intGroup0 = 0;
    intType.intGroup1 = 0;

    for(port=0; port<dev->numOfPorts; port++)
    {
        if((retVal = mdIntSetEnable(dev,port,&intType)) != MAD_OK)
        {
			if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
            MAD_DBG_ERROR(("mdIntSetEnable returned fail.\n"));
            return (retVal | MAD_API_FAIL_SET_INT_ST);
        }
    }

    /* Initialize the MultiAddress Register Access semaphore.    */
    if((dev->multiAddrSem = madSemCreate(dev,MAD_SEM_FULL)) == 0)
    {
        MAD_DBG_ERROR(("semCreate Failed.\n"));
        return MAD_API_FAIL_SEM_CREATE;
    }

    dev->devEnabled = 1;


#ifdef MAD_SM_VCT
    madVCTDataInit(dev);  
#endif

#ifdef MAD_DSP_VCT
 /*   madImplDSPVctInitData (); */
#endif

    /* enable all 8 PHYs */
    for(port=0; port<dev->numOfPorts; port++)
    {
        if((retVal=mdSysSetPhyEnable(dev,port,MAD_TRUE)) != MAD_OK)
        {
			if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
            MAD_DBG_ERROR(("Failed to Enable Phy (port %i)\n",port));
            return MAD_FAIL;
        }
        retVal = mdSysSetDetectPowerDownMode(dev,port,MAD_ENERGE_DETECT_MODE_NO);
        if (retVal != MAD_OK)
        {
		  if((dev->deviceId==MAD_SW1680)&&(retVal==MAD_API_ERROR_OVER_PORT_RANGE)) break;
          MAD_DBG_ERROR(("Failed to set power up Phy (port %i)\n",port));
          return MAD_FAIL;
        }
    }


    MAD_DBG_INFO(("OK.\n"));
    return MAD_OK;
}

/*******************************************************************************
* mdUnloadDriver
*
* DESCRIPTION:
*       This function frees semaphore created by Marvell Alaska Driver,
*       disables Device interrupt, and clears MAD_DEV structure.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MAD_OK           - on success
*       MAD_[ERROR_STATUS]         - on error
*
* COMMENTS:
*       1.  This function should be called only after successful execution of
*           madLoadDriver().
*
*******************************************************************************/
MAD_STATUS mdUnloadDriver
(
    IN MAD_DEV* dev
)
{
    MAD_STATUS  retVal;
    MAD_LPORT   port;
    MAD_INT_TYPE intType;

    MAD_DBG_INFO(("madUnloadDriver Called.\n"));

    /* disable interrupt */
    intType.intGroup0 = 0;
    intType.intGroup1 = 0;

    for(port=0; port<dev->numOfPorts; port++)
    {
        if((retVal = mdIntSetEnable(dev,port,&intType)) != MAD_OK)
        {
            MAD_DBG_ERROR(("mdIntSetEnable returned fail.\n"));
            return (retVal | MAD_API_FAIL_SET_INT_ST);
        }
    }

    /* Delete the MultiAddress mode reagister access semaphore.    */
    if(madSemDelete(dev,dev->multiAddrSem) != MAD_OK)
    {
        MAD_DBG_ERROR(("SemDelete Failed.\n"));
        return MAD_API_FAIL_SEM_DELETE;
    }
 
    madMemSet(dev,0,sizeof(MAD_DEV));
    return MAD_OK;
}


/*******************************************************************************
* fnRegister
*
* DESCRIPTION:
*       BSP should register the following functions:
*        1) MII Read - (Input, must provide)
*            allows driver to read device registers.
*        2) MII Write - (Input, must provice)
*            allows driver to write to device registers.
*        3) Semaphore Create - (Input, optional)
*            OS specific Semaphore Creat function.
*        4) Semaphore Delete - (Input, optional)
*            OS specific Semaphore Delete function.
*        5) Semaphore Take - (Input, optional)
*            OS specific Semaphore Take function.
*        6) Semaphore Give - (Input, optional)
*            OS specific Semaphore Give function.
*        Notes: 3) ~ 6) should be provided all or should not be provided at all.
*
* INPUTS:
*        pBSPFunctions - pointer to the structure for above functions.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MAD_TRUE, if input is valid. MAD_FALSE, otherwise.
*
* COMMENTS:
*
*
*******************************************************************************/
static MAD_BOOL fnRegister(MAD_DEV *dev, MAD_BSP_FUNCTIONS* pBSPFunctions)
{
    dev->fmadReadMii  =  pBSPFunctions->readMii;
    dev->fmadWriteMii  = pBSPFunctions->writeMii;
    dev->fmadReadXMii  =  pBSPFunctions->readXMii;
    dev->fmadWriteXMii = pBSPFunctions->writeXMii;
    
    dev->semCreate = pBSPFunctions->semCreate;
    dev->semDelete = pBSPFunctions->semDelete;
    dev->semTake   = pBSPFunctions->semTake;
    dev->semGive   = pBSPFunctions->semGive;
    
    return MAD_TRUE;
}

static MAD_U8 mad32_2_8[256] = {
0,1,2,3,4,5,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,
20,21,22,23,24,25,26,27,28,29,
30,31,32,33,34,35,36,37,38,39,
40,41,42,43,44,45,46,47,48,49,
50,51,52,53,54,55,56,57,58,59,
60,61,62,63,64,65,66,67,68,69,
70,71,72,73,74,75,76,77,78,79,
80,81,82,83,84,85,86,87,88,89,
90,91,92,93,94,95,96,97,98,99,
100,101,102,103,104,105,106,107,108,109,
110,111,112,113,114,115,116,117,118,119,
120,121,122,123,124,125,126,127,128,129,
130,131,132,133,134,135,136,137,138,139,
140,141,142,143,144,145,146,147,148,149,
150,151,152,153,154,155,156,157,158,159,
160,161,162,163,164,165,166,167,168,169,
170,171,172,173,174,175,176,177,178,179,
180,181,182,183,184,185,186,187,188,189,
190,191,192,193,194,195,196,197,198,199,
200,201,202,203,204,205,206,207,208,209,
210,211,212,213,214,215,216,217,218,219,
220,221,222,223,224,225,226,227,228,229,
230,231,232,233,234,235,236,237,238,239,
240,241,242,243,244,245,246,247,248,249,
250,251,252,253,254,255};



MAD_U8 madLong2Char(MAD_U32 data)
{
    return mad32_2_8[data&0xff];
}

unsigned char madShort2Char(unsigned short data)
{
    unsigned int dataL = data;
    return mad32_2_8[dataL&0xff];
}

unsigned short madLong2Short(unsigned int data)
{
#if 0
  unsigned int data1= 1;
  if( *((unsigned short *)&data1) )
    return *((unsigned short *)&data);
  else
    return *((unsigned short *)&data + 1);
#else
  if((data&0xffff)==data)
    return (data&0xffff);
  else
    return ((data>>16)&0xffff);

#endif
} 

unsigned long madLonglong2Long(unsigned long long data)
{
  unsigned long long data1= 1LL;
  if( *((unsigned long *)&data1) )
    return *((unsigned long *)&data);
  else
    return *((unsigned long *)&data + 1);
} 

