/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssDrvPpConInit.c
*
* DESCRIPTION:
*       Low level driver initialization of PPs, and declarations of global
*       variables
*
* FILE REVISION NUMBER:
*       $Revision: 79 $
*
*******************************************************************************/

#include <cpssDriver/pp/interrupts/generic/prvCpssDrvInterrupts.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpssDriver/pp/prvCpssDrvPpDefs.h>
#include <cpssDriver/pp/hardware/prvCpssDrvHwCntl.h>
#include <cpssDriver/pp/hardware/prvCpssDrvObj.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvDxExMxInterrupts.h>
#include <cpssDriver/pp/hardware/prvCpssDrvHwPpPortGroupCntl.h>

extern GT_U16   prvCpssDrvDebugDeviceId[PRV_CPSS_MAX_PP_DEVICES_CNS];

#ifdef ASIC_SIMULATION
    /* Simulation H files */
    #include <asicSimulation/SInit/sinit.h>
#endif /*ASIC_SIMULATION*/

/* address of SDMA configuration register addresses */
#define SDMA_CFG_REG_ADDR       0x2800

/* DX address of PCI Pad Calibration Configuration register addresses */
#define DX_PCI_PAD_CALIB_REG_ADDR       0x0108

/* DX address of PCI Status and Command Register addresses */
#define DX_PCI_STATUS_CMD_REG_ADDR       0x04

/* DX address of the vendor id register address */
#define VENDOR_ID_REG_ADDR           0x50

/* EXMX address of the class code and the revision id register address */
#define EXMX_PCI_CLASS_CODE_AND_REV_REG_ADDR    0x08

/* PORT_GROUPS_INFO_STC - info about the port groups
        NOTE: assumption that all port groups of the device are the same !
    numOfPortGroups - number of port groups
    portsBmp        - the actual ports bmp of each port group
                        for example: Lion 0xFFF (12 ports)
    maxNumPorts      - number of ports in range
                        for example: Lion 16 ports (12 out of 16 in each port group)
*/
typedef struct {
    GT_U32  numOfPortGroups;
    GT_U32  portsBmp;
    GT_U32  maxNumPorts;
} PORT_GROUPS_INFO_STC;

#ifdef GM_USED
extern GT_STATUS prvCpssDrvPpIntDefDxChBobcat2Init__GM(GT_U8 devNum);
#endif

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
#define BMP_CONTINUES_PORTS_MAC(x)  ((1<<(x)) - 1)

/* macro to set range of ports in bmp --
   NOTE :  it must be ((startPort & 0x1f) + numPorts) <= 31
*/
#define PORT_RANGE_MAC(startPort,numPorts)\
    (GT_U32)(BMP_CONTINUES_PORTS_MAC(numPorts) << ((startPort) & 0x1f ))

static const CPSS_PORTS_BMP_STC portsBmp24_25_27 = {{(PORT_RANGE_MAC(24,2) |
                                                      PORT_RANGE_MAC(27,1)) ,0}};

static const CPSS_PORTS_BMP_STC portsBmp0to23_27 = {{(PORT_RANGE_MAC(0,24) |
                                                      PORT_RANGE_MAC(27,1)) ,0}};

static const CPSS_PORTS_BMP_STC portsBmpCh3_8_Xg = {{(PORT_RANGE_MAC(24,4) |
                                                      PORT_RANGE_MAC(12,1) |
                                                      PORT_RANGE_MAC(10,1) |
                                                      PORT_RANGE_MAC(4 ,1) |
                                                      PORT_RANGE_MAC(0 ,1)) ,0}};

static const CPSS_PORTS_BMP_STC portsBmpCh3Xg =    {{(PORT_RANGE_MAC(24,4) |
                                                      PORT_RANGE_MAC(22,1) |
                                                      PORT_RANGE_MAC(16,1) |
                                                      PORT_RANGE_MAC(12,1) |
                                                      PORT_RANGE_MAC(10,1) |
                                                      PORT_RANGE_MAC(4 ,1) |
                                                      PORT_RANGE_MAC(0 ,1)) ,0}};

static const CPSS_PORTS_BMP_STC portsBmp0to3_24_25 = {{(PORT_RANGE_MAC(0,4) |
                                                        PORT_RANGE_MAC(24,2)) ,0}};

static const CPSS_PORTS_BMP_STC portsBmp0to7_24_25 = {{(PORT_RANGE_MAC(0,8) |
                                                        PORT_RANGE_MAC(24,2)) ,0}};

static const CPSS_PORTS_BMP_STC portsBmp0to15_24_25 = {{(PORT_RANGE_MAC(0,16) |
                                                         PORT_RANGE_MAC(24,2)) ,0}};

static const CPSS_PORTS_BMP_STC portsBmp0to15_24to27 = {{(PORT_RANGE_MAC(0,16) |
                                                          PORT_RANGE_MAC(24,4)) ,0}};

static const CPSS_PORTS_BMP_STC portsBmp0to12_24to27 = {{(PORT_RANGE_MAC(0,12) |
                                                          PORT_RANGE_MAC(24,4)) ,0}};

static const CPSS_PORTS_BMP_STC portsBmp0to7_24to27 = {{(PORT_RANGE_MAC(0,8) |
                                                        PORT_RANGE_MAC(24,4)) ,0}};

static const CPSS_PORTS_BMP_STC portsBmp0to5_24to27 = {{(PORT_RANGE_MAC(0,6) |
                                                        PORT_RANGE_MAC(24,4)) ,0}};

static const CPSS_PORTS_BMP_STC portsBmp_0to11_16to27_32_36_40_41_48_52_56_57_15_31 =
    {{(PORT_RANGE_MAC(0,12) /*0..11 */| PORT_RANGE_MAC(16,12) /*16..27 */),
      (BIT_0 |BIT_4 |BIT_8 |BIT_9 |/* 32,36,40,41*/ BIT_16 |BIT_20 |BIT_24 |BIT_25)/*48,52,56,57*/}};

/* macro for bmp of ports : 0..47 , 56..59 , 64..71 */
#define PORTS_BMP_0_TO_47_56_TO_59_64_TO_71                 \
    {{PORT_RANGE_MAC(0,16) | PORT_RANGE_MAC(16,16) ,        \
      PORT_RANGE_MAC(32,16) | PORT_RANGE_MAC(56,4) ,        \
      PORT_RANGE_MAC(64,8) , 0 }}

/* bmp of ports : 0..47 , 56..59 , 64..71 */
static const CPSS_PORTS_BMP_STC portsBmp0to47_56to59_64to71 =
    PORTS_BMP_0_TO_47_56_TO_59_64_TO_71;

/* macro for bmp of ports : 0..47 , 56..59 , 62, 64..71 */
#define PORTS_BMP_0_TO_47_56_TO_59_62_64_TO_71                              \
    {{PORT_RANGE_MAC(0,16) | PORT_RANGE_MAC(16,16) ,                        \
      PORT_RANGE_MAC(32,16) | PORT_RANGE_MAC(56,4) | PORT_RANGE_MAC(62,1),  \
      PORT_RANGE_MAC(64,8) , 0 }}

/* bmp of ports : 0..47 , 56..59 , 62, 64..71 */
static const CPSS_PORTS_BMP_STC portsBmp0to47_62_56to59_64to71 =
    PORTS_BMP_0_TO_47_56_TO_59_62_64_TO_71;


/* macro for bmp of ports : 0..23 , 56..59 , 64..71 */
#define PORTS_BMP_0_TO_23_56_TO_59_64_TO_71                 \
    {{PORT_RANGE_MAC(0,24),                                 \
      PORT_RANGE_MAC(56,4),                                 \
      PORT_RANGE_MAC(64,8) , 0 }}

/* macro for bmp of ports : 0..23 , 56..59 , 62, 64..71 */
#define PORTS_BMP_0_TO_23_56_TO_59_62_64_TO_71              \
    {{PORT_RANGE_MAC(0,24),                                 \
      PORT_RANGE_MAC(56,4) | PORT_RANGE_MAC(62,1),          \
      PORT_RANGE_MAC(64,8) , 0 }}

/* macro for bmp of ports :  56..59 , 64..71 */
#define PORTS_BMP_56_TO_59_64_TO_71                         \
    {{0 ,                                                   \
      PORT_RANGE_MAC(56,4) ,                                \
      PORT_RANGE_MAC(64,8) , 0 }}

/* bmp of ports :         56..59 , 64..71 */
static const CPSS_PORTS_BMP_STC portsBmp56to59_64to71 =
    PORTS_BMP_56_TO_59_64_TO_71;

/* macro for bmp of ports :  56..59 , 62, 64..71 */
#define PORTS_BMP_56_TO_59_62_64_TO_71                      \
    {{0 ,                                                   \
      PORT_RANGE_MAC(56,4) | PORT_RANGE_MAC(62,1),          \
      PORT_RANGE_MAC(64,8) , 0 }}

/* bmp of ports :         56..59 , 62, 64..71 */
static const CPSS_PORTS_BMP_STC portsBmp56to59_62_64to71 =
    PORTS_BMP_56_TO_59_62_64_TO_71;

/*****************************************************************************
* Global
******************************************************************************/
/* pointer to the object that control ExMxDx */
static PRV_CPSS_DRV_GEN_OBJ_STC drvGenExMxDxObj;
PRV_CPSS_DRV_GEN_OBJ_STC *prvCpssDrvGenExMxDxObjPtr = &drvGenExMxDxObj;

/* pointer to the object that control PCI */
PRV_CPSS_DRV_MNG_INF_OBJ_STC *prvCpssDrvMngInfPciPtr = NULL;

/* pointer to the object that control PEX with 8 address completion regions */
PRV_CPSS_DRV_MNG_INF_OBJ_STC *prvCpssDrvMngInfPexMbusPtr = NULL;

/* pointer to the object that control PCI -- for HA standby */
PRV_CPSS_DRV_MNG_INF_OBJ_STC *prvCpssDrvMngInfPciHaStandbyPtr = NULL;

/* pointer to the object that control SMI */
PRV_CPSS_DRV_MNG_INF_OBJ_STC *prvCpssDrvMngInfSmiPtr = NULL;

/* pointer to the object that control TWSI */
PRV_CPSS_DRV_MNG_INF_OBJ_STC *prvCpssDrvMngInfTwsiPtr = NULL;

/* pointer to the object that control PEX */
/*PRV_CPSS_DRV_MNG_INF_OBJ_STC *prvCpssDrvMngInfPexPtr = NULL;*/

/*****************************************************************************
* Externals
******************************************************************************/


/*****************************************************************************
* Forward declarations
******************************************************************************/

/*****************************************************************************
* Local variables
******************************************************************************/
#define END_OF_TABLE    0xFFFFFFFF
/*tiger*/
static const CPSS_PP_DEVICE_TYPE tgXGlegalDevTypes[] =
{   PRV_CPSS_TIGER_1_DEVICES_MAC , END_OF_TABLE};
static const  CPSS_PP_DEVICE_TYPE tg12legalDevTypes[] =
{   PRV_CPSS_TIGER_12_DEVICES_MAC , END_OF_TABLE};
static const  CPSS_PP_DEVICE_TYPE tg52legalDevTypes[] =
{   PRV_CPSS_TIGER_52_DEVICES_MAC , END_OF_TABLE};
/*samba*/
static const  CPSS_PP_DEVICE_TYPE sambaXGlegalDevTypes[] =
{   PRV_CPSS_SAMBA_1_DEVICES_MAC  , END_OF_TABLE};
static const  CPSS_PP_DEVICE_TYPE samba12legalDevTypes[] =
{   PRV_CPSS_SAMBA_12_DEVICES_MAC , END_OF_TABLE};
static const  CPSS_PP_DEVICE_TYPE samba52legalDevTypes[] =
{   PRV_CPSS_SAMBA_52_DEVICES_MAC , END_OF_TABLE};
/*twistd*/
static const  CPSS_PP_DEVICE_TYPE twistdXGlegalDevTypes[] =
{   PRV_CPSS_TWISTD_1_DEVICES_MAC , END_OF_TABLE};
static const  CPSS_PP_DEVICE_TYPE twistd12legalDevTypes[] =
{   PRV_CPSS_TWISTD_12_DEVICES_MAC, END_OF_TABLE };
static const  CPSS_PP_DEVICE_TYPE twistd52legalDevTypes[] =
{   PRV_CPSS_TWISTD_52_DEVICES_MAC, END_OF_TABLE };
/*twistc*/
static const  CPSS_PP_DEVICE_TYPE twistc10legalDevTypes[] =
{   PRV_CPSS_TWISTC_10_DEVICES_MAC, END_OF_TABLE };
static const  CPSS_PP_DEVICE_TYPE twistc12legalDevTypes[] =
{   PRV_CPSS_TWISTC_12_DEVICES_MAC, END_OF_TABLE };
static const  CPSS_PP_DEVICE_TYPE twistc52legalDevTypes[] =
{   PRV_CPSS_TWISTC_52_DEVICES_MAC, END_OF_TABLE };

/* salsa */
static const  CPSS_PP_DEVICE_TYPE salsa24legalDevTypes[] =
{   PRV_CPSS_SALSA_24_DEVICES_MAC , END_OF_TABLE};
static const  CPSS_PP_DEVICE_TYPE salsa16legalDevTypes[] =
{   PRV_CPSS_SALSA_16_DEVICES_MAC , END_OF_TABLE};
static const  CPSS_PP_DEVICE_TYPE salsa12legalDevTypes[] =
{   PRV_CPSS_SALSA_12_DEVICES_MAC , END_OF_TABLE};
/* cheetah */
static const  CPSS_PP_DEVICE_TYPE ch_27legalDevTypes[] =
{   PRV_CPSS_CH_27_DEVICES_MAC , END_OF_TABLE};
static const  CPSS_PP_DEVICE_TYPE ch_26legalDevTypes[] =
{   PRV_CPSS_CH_26_DEVICES_MAC , END_OF_TABLE};
static const  CPSS_PP_DEVICE_TYPE ch_24legalDevTypes[] =
{   PRV_CPSS_CH_24_DEVICES_MAC , END_OF_TABLE};
static const  CPSS_PP_DEVICE_TYPE ch_25legalDevTypes[] =
{   PRV_CPSS_CH_25_DEVICES_MAC , END_OF_TABLE};
static const  CPSS_PP_DEVICE_TYPE ch_16legalDevTypes[] =
{   PRV_CPSS_CH_16_DEVICES_MAC , END_OF_TABLE};
static const  CPSS_PP_DEVICE_TYPE ch_10legalDevTypes[] =
{   PRV_CPSS_CH_10_DEVICES_MAC , END_OF_TABLE};
/* cheetah-2 */
static const  CPSS_PP_DEVICE_TYPE ch2_28legalDevTypes[] =
{   PRV_CPSS_CH2_28_DEVICES_MAC, END_OF_TABLE };
static const  CPSS_PP_DEVICE_TYPE ch2_27legalDevTypes[] =
{   PRV_CPSS_CH2_27_DEVICES_MAC, END_OF_TABLE };
static const  CPSS_PP_DEVICE_TYPE ch2_26legalDevTypes[] =
{   PRV_CPSS_CH2_26_DEVICES_MAC, END_OF_TABLE };
static const  CPSS_PP_DEVICE_TYPE ch2_24legalDevTypes[] =
{   PRV_CPSS_CH2_24_DEVICES_MAC, END_OF_TABLE };
/* cheetah-3 */
static const  CPSS_PP_DEVICE_TYPE ch3_28legalDevTypes[] =
{   PRV_CPSS_CH3_28_DEVICES_MAC, END_OF_TABLE };
static const  CPSS_PP_DEVICE_TYPE ch3_26legalDevTypes[] =
{   PRV_CPSS_CH3_26_DEVICES_MAC, END_OF_TABLE };
static const  CPSS_PP_DEVICE_TYPE ch3_24legalDevTypes[] =
{   PRV_CPSS_CH3_24_DEVICES_MAC, END_OF_TABLE };
static const  CPSS_PP_DEVICE_TYPE ch3_6legalDevTypes[] =
{   PRV_CPSS_CH3_6_DEVICES_MAC, END_OF_TABLE };
static const  CPSS_PP_DEVICE_TYPE ch3_10legalDevTypes[] =
{   PRV_CPSS_CH3_10_DEVICES_MAC, END_OF_TABLE };
static const  CPSS_PP_DEVICE_TYPE ch3_18legalDevTypes[] =
{   PRV_CPSS_CH3_18_DEVICES_MAC, END_OF_TABLE };
static const  CPSS_PP_DEVICE_TYPE ch3_20legalDevTypes[] =
{   PRV_CPSS_CH3_20_DEVICES_MAC, END_OF_TABLE };
static const  CPSS_PP_DEVICE_TYPE ch3_24_2legalDevTypes[] =
{   PRV_CPSS_CH3_24_2_DEVICES_MAC, END_OF_TABLE };
static const  CPSS_PP_DEVICE_TYPE ch3_24_4legalDevTypes[] =
{   PRV_CPSS_CH3_24_4_DEVICES_MAC, END_OF_TABLE };
static const  CPSS_PP_DEVICE_TYPE ch3_8_XGlegalDevTypes[] =
{   PRV_CPSS_CH3_8_XG_DEVICES_MAC, END_OF_TABLE };
static const  CPSS_PP_DEVICE_TYPE ch3_XGlegalDevTypes[] =
{   PRV_CPSS_CH3_XG_DEVICES_MAC, END_OF_TABLE };
/* Puma */
static const  CPSS_PP_DEVICE_TYPE puma_26legalDevTypes[] =
{   PRV_CPSS_PUMA_26_DEVICES_MAC, END_OF_TABLE };
static const  CPSS_PP_DEVICE_TYPE puma_24legalDevTypes[] =
{   PRV_CPSS_PUMA_24_DEVICES_MAC, END_OF_TABLE };
/* Puma3 */
static const  CPSS_PP_DEVICE_TYPE puma3_legalDevTypes[] =
{   PRV_CPSS_PUMA3_DEVICES_MAC, END_OF_TABLE };
/****************/
/* xCat devices */
/****************/
static const  CPSS_PP_DEVICE_TYPE xcat_24_4legalDevTypes[] =
{   PRV_CPSS_XCAT_24GE_4STACK_PORTS_DEVICES ,
    PRV_CPSS_XCAT_24FE_4STACK_PORTS_DEVICES ,
    END_OF_TABLE };

static const  CPSS_PP_DEVICE_TYPE xcat_24_2legalDevTypes[] =
{   PRV_CPSS_XCAT_24GE_2STACK_PORTS_DEVICES ,
    END_OF_TABLE };

static const  CPSS_PP_DEVICE_TYPE xcat_24_0legalDevTypes[] =
{   PRV_CPSS_XCAT_24GE_NO_STACK_PORTS_DEVICES ,
    END_OF_TABLE };

static const  CPSS_PP_DEVICE_TYPE xcat_16_4legalDevTypes[] =
{   PRV_CPSS_XCAT_16GE_4STACK_PORTS_DEVICES,
    END_OF_TABLE };

static const  CPSS_PP_DEVICE_TYPE xcat_16_2legalDevTypes[] =
{   PRV_CPSS_XCAT_16GE_2STACK_PORTS_DEVICES ,
    END_OF_TABLE };

/* xcat device with 12+2*/
static const  CPSS_PP_DEVICE_TYPE xcat_12_2legalDevTypes[] =
{   PRV_CPSS_XCAT_12GE_2STACK_PORTS_26_27_DEVICES , /*where 2 XG are 26,27*/
    END_OF_TABLE };


static const  CPSS_PP_DEVICE_TYPE xcat_8_4legalDevTypes[] =
{   PRV_CPSS_XCAT_8GE_4STACK_PORTS_DEVICES,
    PRV_CPSS_XCAT_8FE_4STACK_PORTS_DEVICES,
    END_OF_TABLE };

static const  CPSS_PP_DEVICE_TYPE xcat_8_2legalDevTypes[] =
{   PRV_CPSS_XCAT_8GE_2STACK_PORTS_DEVICES,
    PRV_CPSS_XCAT_8FE_2STACK_PORTS_DEVICES,
    END_OF_TABLE };

/*****************/
/* xCat3 devices */
/*****************/
static const  CPSS_PP_DEVICE_TYPE xcat3_24_2legalDevTypes[] =
{   PRV_CPSS_XCAT3_24GE_2STACK_PORTS_DEVICES,
    END_OF_TABLE };

static const  CPSS_PP_DEVICE_TYPE xcat3_24_4legalDevTypes[] =
{   PRV_CPSS_XCAT3_24GE_4STACK_PORTS_DEVICES,
    END_OF_TABLE };

static const  CPSS_PP_DEVICE_TYPE xcat3_16_4legalDevTypes[] =
{   PRV_CPSS_XCAT3_16GE_4STACK_PORTS_DEVICES,
    END_OF_TABLE };

static const  CPSS_PP_DEVICE_TYPE xcat3_8_4legalDevTypes[] =
{   PRV_CPSS_XCAT3_8GE_4STACK_PORTS_DEVICES,
    END_OF_TABLE };

static const  CPSS_PP_DEVICE_TYPE xcat3_8_2legalDevTypes[] =
{   PRV_CPSS_XCAT3_8GE_2STACK_PORTS_DEVICES,
    END_OF_TABLE };


/****************/
/* lion devices */
/****************/
/* lion with port groups 0,1,2,3 */
static const  CPSS_PP_DEVICE_TYPE lion_port_groups0123legalDevTypes[] = {CPSS_LION_PORT_GROUPS_0123_CNS , END_OF_TABLE };
/* lion with port groups 0,1 */
static const  CPSS_PP_DEVICE_TYPE lion_port_groups01__legalDevTypes[] = {CPSS_LION_PORT_GROUPS_01___CNS , END_OF_TABLE };

/* lion2 with port groups 0,1,2,3,4,5,6,7 */
static const  CPSS_PP_DEVICE_TYPE lion2_port_groups01234567legalDevTypes[] = {CPSS_LION2_PORT_GROUPS_01234567_CNS , END_OF_TABLE };

/* lion2-hooper with port groups 0,1,2,3 */
static const  CPSS_PP_DEVICE_TYPE lion2_hooper_port_groups0123legalDevTypes[] = {CPSS_LION2_HOOPER_PORT_GROUPS_0123_CNS , END_OF_TABLE };

static const PORT_GROUPS_INFO_STC lionPortGroups =
{
    4 /*numOfPortGroups*/ ,
    PORT_RANGE_MAC(0,12) /*portsBmp*/   ,
    16 /*maxNumPorts*/
};

static const PORT_GROUPS_INFO_STC lion2PortGroups =
{
    8 /*numOfPortGroups*/ ,
    PORT_RANGE_MAC(0,12) /*portsBmp*/   ,
    16 /*maxNumPorts*/
};

static const PORT_GROUPS_INFO_STC puma3PortGroups =
{
    2 /*numOfPortGroups*/ ,
    0 ,/*portsBmp*/ /* will be set by portsBmp_0to11_16to27_32_36_40_41_48_52_56_57_15_31 */
    32 /*maxNumPorts*/
};

/* macro to set any of 4 port groups as active(exists) in a bitmap */
#define SET_4_PORT_GROUPS_MAC(portGroup0Exists,portGroup1Exists,portGroup2Exists,portGroup3Exists) \
    ((portGroup3Exists) << 3) | ((portGroup2Exists) << 2) |                        \
    ((portGroup1Exists) << 1) | (portGroup0Exists)

/* bmp of lion port groups , for 'known' devices */
static const GT_U32 portGroups01__bmp = SET_4_PORT_GROUPS_MAC(1,1,0,0);
static const GT_U32 portGroups0123bmp = SET_4_PORT_GROUPS_MAC(1,1,1,1);
static const GT_U32 portGroups01234567bmp = 0xff;

/* number of ports in the lion device according to the last active port group */
#define LION_NUM_PORTS_FROM_LAST_PORT_GROUP_MAC(lastPortGroup) (((lastPortGroup + 1) * 16)-4)

/******************/
/* xCat2 devices */
/******************/
static const  CPSS_PP_DEVICE_TYPE xcat2_8_2legalDevTypes[] =
{   PRV_CPSS_XCAT2_8FE_2STACK_PORTS_DEVICES,
    PRV_CPSS_XCAT2_8GE_2STACK_PORTS_DEVICES,
    END_OF_TABLE};

static const  CPSS_PP_DEVICE_TYPE xcat2_8_4legalDevTypes[] =
{   PRV_CPSS_XCAT2_8FE_4STACK_PORTS_DEVICES,
    PRV_CPSS_XCAT2_8GE_4STACK_PORTS_DEVICES,
    END_OF_TABLE};

static const  CPSS_PP_DEVICE_TYPE xcat2_16_2legalDevTypes[] =
{   PRV_CPSS_XCAT2_16GE_2STACK_PORTS_DEVICES,
    END_OF_TABLE};

static const  CPSS_PP_DEVICE_TYPE xcat2_16_4legalDevTypes[] =
{
    PRV_CPSS_XCAT2_16FE_4STACK_PORTS_DEVICES,
    PRV_CPSS_XCAT2_16GE_4STACK_PORTS_DEVICES,
    END_OF_TABLE};

static const  CPSS_PP_DEVICE_TYPE xcat2_24_2legalDevTypes[] =
{   PRV_CPSS_XCAT2_24GE_2STACK_PORTS_DEVICES,
    END_OF_TABLE};

static const  CPSS_PP_DEVICE_TYPE xcat2_24_4legalDevTypes[] =
{   PRV_CPSS_XCAT2_24FE_4STACK_PORTS_DEVICES,
    PRV_CPSS_XCAT2_24GE_4STACK_PORTS_DEVICES,
    END_OF_TABLE};

/* Bobcat2 devices */
static const  CPSS_PP_DEVICE_TYPE bobcat2_legalDevTypes[] =
{   PRV_CPSS_BOBCAT2_DEVICES,
    END_OF_TABLE};
/* Bobk caelum devices */
static const  CPSS_PP_DEVICE_TYPE bobkCaelum_legalDevTypes[] =
{   PRV_CPSS_BOBK_CAELUM_DEVICES,
    END_OF_TABLE};
/* Bobk cetus devices */
static const  CPSS_PP_DEVICE_TYPE bobkCetus_legalDevTypes[] =
{   PRV_CPSS_BOBK_CETUS_DEVICES,
    END_OF_TABLE};

/* Aldrin devices */
static const  CPSS_PP_DEVICE_TYPE aldrin_legalDevTypes[] =
{   PRV_CPSS_ALDRIN_DEVICES,
    END_OF_TABLE};

/* Bobcat3 devices */
static const  CPSS_PP_DEVICE_TYPE bobcat3_legalDevTypes[] =
{   PRV_CPSS_BOBCAT3_ALL_DEVICES,
    END_OF_TABLE};


/* alias to make line shorter */
#define TWISTC_CNS          CPSS_PP_FAMILY_TWISTC_E
#define TWISTD_CNS          CPSS_PP_FAMILY_TWISTD_E
#define SAMBA_CNS           CPSS_PP_FAMILY_SAMBA_E
#define TIGER_CNS           CPSS_PP_FAMILY_TIGER_E
#define SALSA_CNS           CPSS_PP_FAMILY_SALSA_E
#define CHEETAH_CNS         CPSS_PP_FAMILY_CHEETAH_E
#define CHEETAH2_CNS        CPSS_PP_FAMILY_CHEETAH2_E
#define CHEETAH3_CNS        CPSS_PP_FAMILY_CHEETAH3_E
#define XCAT_CNS            CPSS_PP_FAMILY_DXCH_XCAT_E
#define PUMA_CNS            CPSS_PP_FAMILY_PUMA_E
#define LION_CNS            CPSS_PP_FAMILY_DXCH_LION_E
#define XCAT2_CNS           CPSS_PP_FAMILY_DXCH_XCAT2_E
#define LION2_CNS           CPSS_PP_FAMILY_DXCH_LION2_E
#define BOBCAT2_CNS         CPSS_PP_FAMILY_DXCH_BOBCAT2_E
#define BOBCAT3_CNS         CPSS_PP_FAMILY_DXCH_BOBCAT3_E
#define ALDRIN_CNS          CPSS_PP_FAMILY_DXCH_ALDRIN_E
#define PUMA3_CNS           CPSS_PP_FAMILY_PUMA3_E
#define XCAT3_CNS           CPSS_PP_FAMILY_DXCH_XCAT3_E

static GT_U32   debug_force_numOfPorts = 0;
void debug_force_numOfPorts_set(GT_U32 num)
{
    debug_force_numOfPorts = num;
}

/* cpssSupportedTypes :
    purpose :
        DB to hold the device types that the CPSS support

fields :
    devFamily - device family (cheetah/salsa/tiger/../puma)
    numOfPorts - number of ports in the device
    devTypeArray - array of devices for the device family that has same number
                   of ports
    defaultPortsBmpPtr - pointer to special ports bitmap.
                         if this is NULL , that means that the default bitmap
                         defined by the number of ports that considered to be
                         sequential for 0 to the numOfPorts
    portGroupInfoPtr - port groups info.
                    NULL --> meaning 'non multi port groups' device
                    otherwise hold info about the port groups of the device
    activePortGroupsBmpPtr - active port groups bitmap.
                    relevant only when portGroupInfoPtr != NULL
                    can't be NULL when portGroupInfoPtr != NULL
*/
const struct {
    CPSS_PP_FAMILY_TYPE_ENT     devFamily;
    GT_U8                       numOfPorts;
    const CPSS_PP_DEVICE_TYPE   *devTypeArray;
    const CPSS_PORTS_BMP_STC    *defaultPortsBmpPtr;
    const PORT_GROUPS_INFO_STC  *portGroupInfoPtr;
    const GT_U32                *activePortGroupsBmpPtr;
}cpssSupportedTypes[] =
{
    {TWISTC_CNS   ,52 ,twistc52legalDevTypes ,NULL,NULL,NULL},
    {TWISTC_CNS   ,12 ,twistc12legalDevTypes ,NULL,NULL,NULL},
    {TWISTC_CNS   ,10 ,twistc10legalDevTypes ,NULL,NULL,NULL},

    {TWISTD_CNS   ,52 , twistd52legalDevTypes,NULL,NULL,NULL},
    {TWISTD_CNS   ,12 , twistd12legalDevTypes,NULL,NULL,NULL},
    {TWISTD_CNS   , 1 , twistdXGlegalDevTypes,NULL,NULL,NULL},

    {SAMBA_CNS    ,52 ,samba52legalDevTypes, NULL,NULL,NULL},
    {SAMBA_CNS    ,12 ,samba12legalDevTypes, NULL,NULL,NULL},
    {SAMBA_CNS    , 1 ,sambaXGlegalDevTypes, NULL,NULL,NULL},

    {TIGER_CNS    ,52 ,tg52legalDevTypes, NULL,NULL,NULL},
    {TIGER_CNS    ,12 ,tg12legalDevTypes, NULL,NULL,NULL},
    {TIGER_CNS    , 1 ,tgXGlegalDevTypes, NULL,NULL,NULL},

    {SALSA_CNS    ,12 ,salsa12legalDevTypes, NULL,NULL,NULL},
    {SALSA_CNS    ,16 ,salsa16legalDevTypes, NULL,NULL,NULL},
    {SALSA_CNS    ,24 ,salsa24legalDevTypes, NULL,NULL,NULL},

    {CHEETAH_CNS  ,27 ,ch_27legalDevTypes, NULL,NULL,NULL},
    {CHEETAH_CNS  ,26 ,ch_26legalDevTypes, NULL,NULL,NULL},
    {CHEETAH_CNS  ,24 ,ch_24legalDevTypes, NULL,NULL,NULL},
    {CHEETAH_CNS  ,25 ,ch_25legalDevTypes, NULL,NULL,NULL},
    {CHEETAH_CNS  ,16 ,ch_16legalDevTypes, NULL,NULL,NULL},
    {CHEETAH_CNS  ,10 ,ch_10legalDevTypes, NULL,NULL,NULL},

    {CHEETAH2_CNS ,28 ,ch2_28legalDevTypes, NULL,NULL,NULL},
    {CHEETAH2_CNS ,27 ,ch2_27legalDevTypes, NULL,NULL,NULL},
    {CHEETAH2_CNS ,26 ,ch2_26legalDevTypes, NULL,NULL,NULL},
    {CHEETAH2_CNS ,24 ,ch2_24legalDevTypes, NULL,NULL,NULL},

    {CHEETAH3_CNS ,28 ,ch3_28legalDevTypes, NULL,NULL,NULL},
    {CHEETAH3_CNS ,26 ,ch3_26legalDevTypes, NULL,NULL,NULL},
    {CHEETAH3_CNS ,24 ,ch3_24legalDevTypes, NULL,NULL,NULL},
    {CHEETAH3_CNS ,26 ,ch3_6legalDevTypes,  &portsBmp0to3_24_25,NULL,NULL},
    {CHEETAH3_CNS ,26 ,ch3_10legalDevTypes, &portsBmp0to7_24_25,NULL,NULL},
    {CHEETAH3_CNS ,26 ,ch3_18legalDevTypes, &portsBmp0to15_24_25,NULL,NULL},
    {CHEETAH3_CNS ,28 ,ch3_20legalDevTypes, &portsBmp0to15_24to27,NULL,NULL},
    {CHEETAH3_CNS ,26 ,ch3_24_2legalDevTypes, NULL,NULL,NULL},
    {CHEETAH3_CNS ,28 ,ch3_24_4legalDevTypes, NULL,NULL,NULL},
    {CHEETAH3_CNS ,28 ,ch3_8_XGlegalDevTypes, &portsBmpCh3_8_Xg,NULL,NULL},
    {CHEETAH3_CNS ,28 ,ch3_XGlegalDevTypes, &portsBmpCh3Xg,NULL,NULL},

    /* xCat devices  - 24network+0,2,4stack */
    {XCAT_CNS     ,24+4 ,xcat_24_4legalDevTypes, NULL,NULL,NULL},/*ports 0..27 */
    {XCAT_CNS     ,24+2 ,xcat_24_2legalDevTypes, NULL,NULL,NULL},/*ports 0..25 */
    {XCAT_CNS     ,24+0 ,xcat_24_0legalDevTypes, NULL,NULL,NULL},/*ports 0..23 */

    /* xCat devices  - 16network+0,2,4stack */
    {XCAT_CNS     ,24+4 ,xcat_16_4legalDevTypes, &portsBmp0to15_24to27,NULL,NULL}, /*ports 0..15,24..27 */
    {XCAT_CNS     ,24+2 ,xcat_16_2legalDevTypes, &portsBmp0to15_24to27,NULL,NULL}, /*ports 0..15,24..25 */

    /* xCat devices  - 12network+0,2,4stack */
    {XCAT_CNS     ,24+4 ,xcat_12_2legalDevTypes, &portsBmp0to12_24to27,NULL,NULL}, /*ports 0..11,24..27 */

    /* xCat devices  - 8network+2,4stack */
    {XCAT_CNS     ,24+4 ,xcat_8_4legalDevTypes, &portsBmp0to7_24to27,NULL,NULL}, /*ports 0..7,24..27 */
    {XCAT_CNS     ,24+2 ,xcat_8_2legalDevTypes, &portsBmp0to7_24to27,NULL,NULL}, /*ports 0..7,24..25 */

    /* puma devices */
    {PUMA_CNS     ,28 ,puma_26legalDevTypes, &portsBmp24_25_27,NULL,NULL},
    {PUMA_CNS     ,28 ,puma_24legalDevTypes, &portsBmp0to23_27,NULL,NULL},

    /* puma3 devices */
    {PUMA3_CNS    ,60 ,puma3_legalDevTypes, &portsBmp_0to11_16to27_32_36_40_41_48_52_56_57_15_31,&puma3PortGroups,&portGroups01__bmp},

    /* Lion devices */                                             /* active port groups */
    {LION_CNS ,LION_NUM_PORTS_FROM_LAST_PORT_GROUP_MAC(3) ,/*0,1,2,3*/lion_port_groups0123legalDevTypes, NULL ,&lionPortGroups,&portGroups0123bmp},
    {LION_CNS ,LION_NUM_PORTS_FROM_LAST_PORT_GROUP_MAC(3) ,/*0,1,   */lion_port_groups01__legalDevTypes, NULL ,&lionPortGroups,&portGroups01__bmp},

     /* Lion2 devices */                                             /* active port groups */
    {LION2_CNS ,LION_NUM_PORTS_FROM_LAST_PORT_GROUP_MAC(7) ,lion2_port_groups01234567legalDevTypes, NULL ,&lion2PortGroups,&portGroups01234567bmp},

     /* Lion2 Hooper devices */                                             /* active port groups */
    {LION2_CNS ,LION_NUM_PORTS_FROM_LAST_PORT_GROUP_MAC(3) ,lion2_hooper_port_groups0123legalDevTypes, NULL ,&lionPortGroups,&portGroups0123bmp},

    /* Bobcat2 supports 72 MACs, 74 DMAs. */
    {BOBCAT2_CNS ,72 ,bobcat2_legalDevTypes, NULL,NULL,NULL},

    /* Bobk supports range of 72 MACs, 74 DMAs. */
    /* bobk Caelum : */
    /* actual macs are in : 0..47 (GE) , 56..59 (XLG) , 64..71 (XLG) */
    /* there are also 2 TX DMA : 72(cpu),73(TM) */
    /* there is  also 1 RX DMA : 72(cpu) */
    /* NOTE: same MACs as for 'CPSS_98DX4221_CNS' */
    {BOBCAT2_CNS ,72 ,bobkCaelum_legalDevTypes, &portsBmp0to47_62_56to59_64to71,NULL,NULL},

    /* bobk Cetus : */
    /* actual macs are in : 56..59 (XLG) , 64..71 (XLG) */
    /* there are also 2 TX DMA : 72(cpu),73(TM) */
    /* there is  also 1 RX DMA : 72(cpu) */
    {BOBCAT2_CNS ,72 ,bobkCetus_legalDevTypes,  &portsBmp56to59_62_64to71      ,NULL,NULL},

    /* Aldrin : */
    /* actual macs are in : 0..31 */
    /* there are also 1 external CPU DMA (MG port) : 32*/ 
    {ALDRIN_CNS ,32 ,aldrin_legalDevTypes,  NULL,NULL,NULL}, 

#ifndef GM_USED
    /* Bobcat3 supports 72 MACs, 74 DMAs. */
    {BOBCAT3_CNS ,72 ,bobcat3_legalDevTypes, NULL,NULL,NULL},
#else /*GM_USED*/
    /* Bobcat3 GM supports single DP : 12 MACs, 13 DMAs. */
    {BOBCAT3_CNS ,12 ,bobcat3_legalDevTypes, NULL,NULL,NULL},
#endif  /*GM_USED*/


    /* xCat2 devices  - 8network, 2stack */
    {XCAT2_CNS     ,24+2 ,xcat2_8_2legalDevTypes, &portsBmp0to7_24_25,NULL,NULL},

    /* xCat2 devices  - 8network, 4stack */
    {XCAT2_CNS     ,24+4 ,xcat2_8_4legalDevTypes, &portsBmp0to7_24to27,NULL,NULL},

    /* xCat2 devices  - 16network, 2stack */
    {XCAT2_CNS     ,24+2 ,xcat2_16_2legalDevTypes, &portsBmp0to15_24_25,NULL,NULL},

    /* xCat2 devices  - 16network, 4stack */
    {XCAT2_CNS     ,24+4 ,xcat2_16_4legalDevTypes, &portsBmp0to15_24to27,NULL,NULL},

    /* xCat2 devices  - 24network, 2stack */
    {XCAT2_CNS     ,24+2 ,xcat2_24_2legalDevTypes, NULL,NULL,NULL},

    /* xCat2 devices  - 24network, 4stack */
    {XCAT2_CNS     ,24+4 ,xcat2_24_4legalDevTypes, NULL,NULL,NULL},

    /* xCat3 devices  - 24network, 2stack */
    {XCAT3_CNS    ,24+2 ,xcat3_24_2legalDevTypes, NULL,NULL,NULL},

    /* xCat3 devices  - 24network, 4stack */
    {XCAT3_CNS    ,24+4 ,xcat3_24_4legalDevTypes, NULL,NULL,NULL},/*ports 0..27 */

    /* xCat3 devices  - 16network, 2stack */
    {XCAT3_CNS    ,24+4 ,xcat3_16_4legalDevTypes, &portsBmp0to15_24to27,NULL,NULL}, /*ports 0..15,24..27 */

    /* xCat3 devices  - 8network, 4stack */
    {XCAT3_CNS    ,24+4 ,xcat3_8_4legalDevTypes, &portsBmp0to7_24to27,NULL,NULL}, /*ports 0..7,24..27 */

    /* xCat3 devices  - 8network, 2stack */
    {XCAT3_CNS    ,24+2 ,xcat3_8_2legalDevTypes, &portsBmp0to7_24_25,NULL,NULL}, /*ports 0..7,24..25 */

    /* End of list      */
    {(CPSS_PP_FAMILY_TYPE_ENT)END_OF_TABLE   ,0 , NULL,NULL, NULL, NULL}
};

/* cpssSpecialDevicesBmp -
 * Purpose : DB to hold the devices with special ports BMP
 *
 * NOTE : devices that his port are sequential for 0 to the numOfPorts
 *        not need to be in this Array !!!
 *
 * fields :
 *  devType - device type that has special ports bmp (that is different from
 *            those of other devices of his family with the same numOfPort)
 *  existingPorts - the special ports bmp of the device
 *
*/
static const struct {
    CPSS_PP_DEVICE_TYPE         devType;
    CPSS_PORTS_BMP_STC          existingPorts;
}cpssSpecialDevicesBmp[] =
{
    /* 12 Giga ports (0.11) and XG 24,25 */
    {CPSS_98DX145_CNS , {{PORT_RANGE_MAC(0,12) | PORT_RANGE_MAC(24,2) ,0}}},
    /* 12 Giga ports (0.11) and XG 24 */
    {CPSS_98DX133_CNS , {{PORT_RANGE_MAC(0,12) | PORT_RANGE_MAC(24,1) ,0}}},
    /* No Giga ports and 3XG 24..26 */
    {CPSS_98DX803_CNS , {{PORT_RANGE_MAC(24,3) ,0}}},
    /* No Giga ports and 4XG 24..27 */
    {CPSS_98DX804_CNS , {{PORT_RANGE_MAC(24,4) ,0}}},

    /* bobcat2 MACs : 0..23 , 56..59 , 64..71 */
    {CPSS_98DX4220_CNS , PORTS_BMP_0_TO_23_56_TO_59_64_TO_71},

    /* bobcat2 MACs : 0..47, 56..59 , 64..71 */
    {CPSS_98DX4221_CNS , PORTS_BMP_0_TO_47_56_TO_59_64_TO_71},

    /* bobcat2 MACs : 0..47, 60..71 */
    {CPSS_98DX4222_CNS , {{PORT_RANGE_MAC(0,16) | PORT_RANGE_MAC(16,16) , PORT_RANGE_MAC(32,16) | PORT_RANGE_MAC(60,4) , PORT_RANGE_MAC(64,8) , 0 }}},

    /* bobcat2 MACs : 52..55 , 56..59, 64..67, 68..71 */
    {CPSS_98DX8216_CNS , {{0, PORT_RANGE_MAC(52,8) , PORT_RANGE_MAC(64,8) , 0 }}},

    /* bobcat2 MACs : 0...8, 48..63, 64..71 */
    {CPSS_98DX8219_CNS , {{PORT_RANGE_MAC(0,8), PORT_RANGE_MAC(48,16),  PORT_RANGE_MAC(64,8) , 0 }}},

    /* bobcat2 MACs : 48..51 , 52..55 , 56..59, 60..63, 64..67, 68..71 */
    {CPSS_98DX8224_CNS , {{0, PORT_RANGE_MAC(48,16) , PORT_RANGE_MAC(64,8) , 0 }}},

    /* bobcat2 MACs : 32..47, 52..59 */
    {CPSS_98DX4253_CNS , {{0, PORT_RANGE_MAC(32,16) | PORT_RANGE_MAC(52,8) , 0 , 0 }}},

    /* Cetus MACs : 64..67, 68..71 */
    {CPSS_98DX8208_CNS , {{0, PORT_RANGE_MAC(62,1), PORT_RANGE_MAC(64,8) , 0 }}},

    /* Caelum MACs : 0..23 , 56..59 , 62, 64..71 */
    {CPSS_98DX4204_CNS , PORTS_BMP_0_TO_23_56_TO_59_62_64_TO_71},

    /* Caelum MACs : 0..23 , 56..59 , 62, 64..71 */
    {CPSS_98DX4210_CNS , PORTS_BMP_0_TO_23_56_TO_59_62_64_TO_71},

    /* End of list      */
    {END_OF_TABLE   ,{{0,0}} }
};

#ifdef CHX_FAMILY

static GT_STATUS prvCpssDrvLion2FirstPacketWA
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN PRV_CPSS_DRV_PP_PHASE_1_INPUT_INFO_STC  *ppInInfoPtr
);

static GT_STATUS prvCpssDrvLion2MMPcsUnReset
(
    GT_U8                   devNum,
    GT_U32                  portGroupId
);

#endif /* CHX_FAMILY */

#ifdef ASIC_SIMULATION
typedef enum{
    BIG_ENDIAN_E = 1, /* HOST is 'Big endian' */
    LITTLE_ENDIAN_E   /* HOST is 'Little endian' */
}ENDIAN_ENT;

/*******************************************************************************
* checkHostEndian
*
* DESCRIPTION:
*       This function check if the HOST is 'Big endian' or 'Little endian'
*
* INPUTS:
*       none
*
* OUTPUTS:
*       none
*
* RETURNS:
*       one of ENDIAN_ENT -  BIG_ENDIAN_E /  LITTLE_ENDIAN_E
*
* COMMENTS:
*
*******************************************************************************/
static ENDIAN_ENT  checkHostEndian(void)
{
    GT_U32  wordMem = 0x11223344;
    GT_U8  *charMemPtr = (GT_U8*)(&wordMem);

    if(charMemPtr[0] == 0x11)
    {
        /* 'Big endian' - the bits 24..31 sit in byte0 */
        return BIG_ENDIAN_E;
    }

    /* 'Little endian' - the bits 24..31 sit in byte3 */
    return LITTLE_ENDIAN_E;
}
#endif /*ASIC_SIMULATION*/

/*******************************************************************************
* phase1Part1Init
*
* DESCRIPTION:
*       This function is called by prvCpssDrvHwPpPhase1Init()
*       the function bind the PCI/SMI/TWSI driver functions to the driver object
*       functions for this device.
*
* INPUTS:
*       devNum      - The PP's device number to be initialized.
*       ppInInfoPtr    - (pointer to)   the info needed for initialization of
*                        the driver for this PP
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_BAD_PARAM - bad management interface value
*       GT_NOT_IMPLEMENTED - the needed driver was not compiled correctly
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS phase1Part1Init
(
    IN  GT_U8                devNum,
    IN  PRV_CPSS_DRV_PP_PHASE_1_INPUT_INFO_STC  *ppInInfoPtr
)
{
    GT_STATUS rc;

    /********************************************/
    /* bind the BUS part to the ExMxDx BUS part */
    /********************************************/


    /* set rc as "not implemented" to suggest that the specific SMI/PCI/TWSI is
       not part of the image although needed in the image */
    rc = GT_NOT_IMPLEMENTED;
    /* driver object bind */
    if(ppInInfoPtr->mngInterfaceType == CPSS_CHANNEL_PCI_E)
    {
#if defined GT_PCI  /*|| defined GT_PEX*/
        rc = GT_OK;
        if(ppInInfoPtr->ppHAState == CPSS_SYS_HA_MODE_ACTIVE_E)
        {
            prvCpssDrvPpObjConfig[devNum]->busPtr = prvCpssDrvMngInfPciPtr;
        }
        else
        {
            prvCpssDrvPpObjConfig[devNum]->busPtr = prvCpssDrvMngInfPciHaStandbyPtr;
        }
#endif
    }
    /* SMI */
    else if (ppInInfoPtr->mngInterfaceType == CPSS_CHANNEL_SMI_E)
    {
#ifdef GT_SMI
        rc = GT_OK;
        prvCpssDrvPpObjConfig[devNum]->busPtr = prvCpssDrvMngInfSmiPtr;
#endif
    }
 /* TWSI */
    else if (ppInInfoPtr->mngInterfaceType == CPSS_CHANNEL_TWSI_E)
    {
#ifdef GT_I2C
        rc = GT_OK;
        prvCpssDrvPpObjConfig[devNum]->busPtr = prvCpssDrvMngInfTwsiPtr;
#endif
    }
    else if(CPSS_CHANNEL_IS_PEX_MAC(ppInInfoPtr->mngInterfaceType))
    {
#if defined GT_PCI  /*|| defined GT_PEX*/
        rc = GT_OK;
        if(ppInInfoPtr->ppHAState == CPSS_SYS_HA_MODE_ACTIVE_E)
        {
            prvCpssDrvPpObjConfig[devNum]->busPtr = prvCpssDrvMngInfPciPtr;
        }
        else
        {
            prvCpssDrvPpObjConfig[devNum]->busPtr = prvCpssDrvMngInfPciHaStandbyPtr;
        }
#endif
    }
    else if(CPSS_CHANNEL_IS_PEX_MBUS_MAC(ppInInfoPtr->mngInterfaceType))
    {
#if defined GT_PCI  /*|| defined GT_PEX*/
        rc = GT_OK;
        if(ppInInfoPtr->ppHAState == CPSS_SYS_HA_MODE_ACTIVE_E)
        {
            prvCpssDrvPpObjConfig[devNum]->busPtr = prvCpssDrvMngInfPexMbusPtr;
        }
        else
        {
            prvCpssDrvPpObjConfig[devNum]->busPtr = prvCpssDrvMngInfPciHaStandbyPtr;
        }
#endif
    }
    else
    {
        /* unknown interface type */
        rc = GT_BAD_PARAM;
    }
    return rc;
}

/*******************************************************************************
* phase1Part2Init
*
* DESCRIPTION:
*       This function is called by prvCpssDrvHwPpPhase1Init()
*       This function allocate the memory for the DB of the driver for this
*       device , and set some of it's values.
*
* INPUTS:
*       devNum      - The PP's device number to be initialized.
*       ppInInfoPtr    - (pointer to)   the info needed for initialization of
*                        the driver for this PP
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_OUT_OF_CPU_MEM - failed to allocate CPU memory,
*       GT_BAD_STATE - the driver is not in state that ready for initialization
*                      for the device
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS phase1Part2Init
(
    IN  GT_U8                devNum,
    IN  PRV_CPSS_DRV_PP_PHASE_1_INPUT_INFO_STC  *ppInInfoPtr
)
{
    PRV_CPSS_DRIVER_PP_CONFIG_STC *ppConfigPtr;/* Points to the Device's Driver*/
    GT_U32      portGroupId;/* the port group Id */
    GT_BOOL     activePortGroupFound = GT_FALSE; /*indication that there is active port group */

    if(prvCpssDrvPpConfig[devNum] != NULL)
    {
        /* the device already exists ?! */
        /* we can't override it ! , the device must be empty before we
           initialize it */
        /* check that "remove device" was done properly */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    /* allocate the device a DB */
    ppConfigPtr = cpssOsMalloc(sizeof(PRV_CPSS_DRIVER_PP_CONFIG_STC));
    if(ppConfigPtr == NULL)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
    }

    /* set default values */
    cpssOsMemSet(ppConfigPtr,0,sizeof(PRV_CPSS_DRIVER_PP_CONFIG_STC));
    /* bind the memory to the global DB */
    prvCpssDrvPpConfig[devNum] = ppConfigPtr;

    /* set value */
    ppConfigPtr->hwDevNum = devNum;
    ppConfigPtr->drvHwIfSupportedBmp = PRV_CPSS_HW_IF_BMP_MAC(ppInInfoPtr->mngInterfaceType);
    ppConfigPtr->mngInterfaceType = ppInInfoPtr->mngInterfaceType;
    if (CPSS_CHANNEL_IS_PCI_COMPATIBLE_MAC(ppInInfoPtr->mngInterfaceType))
    {
        /* in most cases we will treat the "PCI" and "PEX" the same */
        ppConfigPtr->drvHwIfSupportedBmp |= PRV_CPSS_HW_IF_BMP_MAC(CPSS_CHANNEL_PCI_E);
    }

    prvCpssDrvPpObjConfig[devNum]->HAState = ppInInfoPtr->ppHAState;

    for(portGroupId = 0; portGroupId < ppInInfoPtr->numOfPortGroups; portGroupId++)
    {
        ppConfigPtr->hwCtrl[portGroupId].baseAddr = ppInInfoPtr->busBaseAddr[portGroupId];
        ppConfigPtr->hwCtrl[portGroupId].internalPciBase = ppInInfoPtr->internalPciBase[portGroupId];
        ppConfigPtr->hwCtrl[portGroupId].hwAddr = ppInInfoPtr->hwAddr[portGroupId];

        if(ppConfigPtr->hwCtrl[portGroupId].baseAddr != CPSS_PARAM_NOT_USED_CNS)
        {
            /* active port group */
            activePortGroupFound = GT_TRUE;
        }

        ppConfigPtr->hwCtrl[portGroupId].isrAddrCompletionRegionsBmp  =
            ppInInfoPtr->isrAddrCompletionRegionsBmp;
        ppConfigPtr->hwCtrl[portGroupId].appAddrCompletionRegionsBmp  =
            ppInInfoPtr->appAddrCompletionRegionsBmp;

    }

    if(activePortGroupFound == GT_FALSE)
    {
        /* at least one port group need to be active */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
    }

    ppConfigPtr->resetAndInitControllerBase =
                                    ppInInfoPtr->resetAndInitControllerBase;

    return GT_OK;
}

/*******************************************************************************
* phase1Part3Init
*
* DESCRIPTION:
*       This function is called by prvCpssDrvHwPpPhase1Init()
*       This function call driverHwCntlInit to initialize the HW control.
*       The function get the deviceType from HW , revision id , doByteSwap
*
*       Note : for standby HA -- the doByteSwap must be figured when the system
*              changed to HA active
*
* INPUTS:
*       devNum      - The PP's device number to be initialized.
*       portGroupId      - port group Id.
*       ppInInfoPtr    - (pointer to)   the info needed for initialization of
*                        the driver for this PP
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS phase1Part3Init
(
    IN  GT_U8                devNum,
    IN  GT_U32               portGroupId,
    IN  PRV_CPSS_DRV_PP_PHASE_1_INPUT_INFO_STC  *ppInInfoPtr
)
{
    GT_STATUS   rc;
    PRV_CPSS_DRIVER_PP_CONFIG_STC *ppConfigPtr;/* Points to the Device's Driver*/
    GT_U32  readDeviceId ,readVendorId;/*deviceId,vendor Id as read from HW */
    GT_U32  tempDevId = 0;/* the device type built from device id , vendor id */
    GT_U32  regData;     /* register data */
    GT_U32  ii;
    GT_U32  address;/* address to access */

    ppConfigPtr = prvCpssDrvPpConfig[devNum];

    if(0 == PRV_CPSS_DRV_HW_IF_PCI_COMPATIBLE_MAC(devNum))
    {
        ppConfigPtr->hwCtrl[portGroupId].internalPciBase = 0; /* don't care in non-PCI */

        /* The "do byte swap" relate only to the PCI bus , since on PCI we read
           4 BYTES and don't know if was read correctly

           so for non-PCI we set doByteSwap = GT_FALSE
        */

        /* Init Hw Cntl parameters. --
           must be done prior to any read/write registers ...*/
        rc = prvCpssDrvHwCntlInit(devNum,portGroupId,
                         ppConfigPtr->hwCtrl[portGroupId].baseAddr,
                         ppConfigPtr->hwCtrl[portGroupId].internalPciBase,
                         GT_FALSE);/* isDiag */
        if (rc!= GT_OK)
            return rc;

        /* read DeviceID */
        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId ,PRV_CPSS_DEVICE_ID_AND_REV_REG_ADDR_CNS,
                                                4, 16, &readDeviceId);
        if (rc!= GT_OK)
            return rc;
        /* read VendorID */
        rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId ,VENDOR_ID_REG_ADDR, 0, 16, &readVendorId) ;
        if (rc!= GT_OK)
            return rc;

        tempDevId = (readDeviceId << 16) | readVendorId ;
    }
    else  /* PCI/PEX compatible */
    {
        /* Init Hw Cntl parameters. The value of doByteSwap is unknown
         * Assume only prvCpssDrvHwPpReadInternalPciReg() will be used till
         * correct value of doByteSwap known */
        rc = prvCpssDrvHwCntlInit(devNum,portGroupId,
                         ppConfigPtr->hwCtrl[portGroupId].baseAddr,
                         ppConfigPtr->hwCtrl[portGroupId].internalPciBase,
                         GT_FALSE);/* isDiag */
        if (rc!= GT_OK)
            return rc;

        /* Read from the PCI channel */
        if(ppInInfoPtr->ppHAState == CPSS_SYS_HA_MODE_ACTIVE_E)
        {
            switch(ppInInfoPtr->devIdLocationType)
            {
                case PRV_CPSS_DRV_DEV_ID_LOCATION_PCI_CONFIG_CYCLE_ADDR_0x00000000_E:
                case PRV_CPSS_DRV_DEV_ID_LOCATION_PEX_CONFIGURATION_HEADER_ADDR_0X00070000_E:
                    if(ppInInfoPtr->devIdLocationType ==
                        PRV_CPSS_DRV_DEV_ID_LOCATION_PEX_CONFIGURATION_HEADER_ADDR_0X00070000_E)
                    {
                        address = 0x70000;
                    }
                    else
                    {
                        address = 0;
                    }

                    /* perform the direct PCI/PEX memory read access */
                    rc = prvCpssDrvHwPpPortGroupReadInternalPciReg(devNum, portGroupId,address, &tempDevId);
                    if (rc!= GT_OK)
                        return rc;

                    if((tempDevId & 0xFFFF) != MARVELL_VENDOR_ID)
                    {
                        /* not expected value for the register */
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
                    }
                    break;

                case PRV_CPSS_DRV_DEV_ID_LOCATION_DEVICE_ADDR_0x0000004C_E:
                /* try to read register 0x50 to get the 0x11AB (MARVELL_VENDOR_ID) */

                    /* read VendorID */
                    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId,VENDOR_ID_REG_ADDR, &readVendorId) ;
                    if (rc!= GT_OK)
                        return rc;

                    if((readVendorId & 0xFFFF) != MARVELL_VENDOR_ID)
                    {
                        /* not expected value for the register */
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
                    }

                    /* read DeviceID */
                    rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId,PRV_CPSS_DEVICE_ID_AND_REV_REG_ADDR_CNS,
                                                            4,16, &readDeviceId);
                    if (rc!= GT_OK)
                        return rc;

                    tempDevId = (readDeviceId << 16) | readVendorId ;

                    break;
                default:
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_SUPPORTED, LOG_ERROR_NO_MSG);
            }/*switch(ppInInfoPtr->devIdLocationType) */

            if(PRV_CPSS_DRV_ERRATA_GET_MAC(devNum,PRV_CPSS_DRV_ERRATA_PCI_SLAVE_UNABLE_TO_ABORT_TRANSACTION_E) == GT_TRUE)
            {
                /* Initialize the PCI Timer and Retry register with 0xFF.   */
                /* Perform a read modify write on the register.             */
                rc = prvCpssDrvHwPpPortGroupReadInternalPciReg(devNum,portGroupId,0x104,&regData);
                if (rc!= GT_OK)
                    return rc;

                regData = (regData & 0xFFFF0000) | 0xFFFF;  /*set both timer values to maximum */

                rc = prvCpssDrvHwPpPortGroupWriteInternalPciReg(devNum,portGroupId,0x104,regData);
                if (rc!= GT_OK)
                    return rc;
            }
        }
        /* now doByteSwap has correct value */
    }/* PCI/PEX compatible */

    if(ppInInfoPtr->ppHAState == CPSS_SYS_HA_MODE_STANDBY_E)
    {
        /* get the info that the high availability holds for that device */
        tempDevId  = sysGenGlobalInfo.cpssPpHaDevData[devNum]->deviceId;
    }

    /* device type was stored in the internal DB */
    if (prvCpssDrvDebugDeviceId[devNum] != 0x0)
    {
        tempDevId = ((prvCpssDrvDebugDeviceId[devNum] << 16) | MARVELL_VENDOR_ID);
    }

#ifdef CHX_FAMILY
#ifndef GM_USED /* BC2 GM does not support DFX registers */
    /* devFamily isn't defined yet */
    if(((tempDevId>>16)&0xFC00) == 0xFC00)
    {
        rc = prvCpssDrvHwPpResetAndInitControllerGetRegField(devNum,
                                                             0xF8240,
                                                             0,
                                                             16,
                                                             &regData);
        if(rc != GT_OK)
        {
            return rc;
        }
        tempDevId |= (regData << 16);

    }
#endif /* GM_USED */

    if(((tempDevId>>16) == 0xEC00) || ((tempDevId>>16) == 0x8000))
    {
        if(1 <= portGroupId)
        {/* In current revision of Lion2/Hooper Register 0x4C is always 0xEC00/0x8000.
            Read DFX S@R register bits [24-21] to get proper device type,
            DFX Unit exists only in port group 1 */
            if(GT_OK != prvCpssDrvHwPpPortGroupGetRegField(devNum, 1,
                                                            0x18F8200, 21, 4, &regData))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
            }
            else
            {
                tempDevId |= (regData << 16);
            }
        }

        if((tempDevId>>20) != 0x800)
        {/* run only for Lion2 A0 */ /* Lion2 Hooper has also revision 0 - but WA not needed */
            /* RevisionID read */
            prvCpssDrvHwPpGetRegField(devNum,
                            PRV_CPSS_DEVICE_ID_AND_REV_REG_ADDR_CNS, 0, 4, &regData);
            if(0 == regData)
            {
                rc = prvCpssDrvLion2FirstPacketWA(devNum, portGroupId, ppInInfoPtr);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }
        }
    }
#endif


#ifdef EXMXPM_FAMILY
    /* devFamily isn't defined yet */
    if((tempDevId>>16) == 0xE000)
    {
        if(0 == portGroupId)
        {/* In Puma3 Register 0x4C is always 0xE000.
            Read DFX S@R register bits [2-0] to get proper device type,
            DFX Unit exists only in port group 0 */
            if(GT_OK != prvCpssDrvHwPpPortGroupGetRegField(devNum, 0,
                                                            0x44F8230, 0, 3, &regData))
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
            }
            else
            {
                tempDevId |= (regData << 16);
            }
        }
    }
#endif


    /* temporary setting that may change if the CPSS decide to address this
       device with other device type ! */
    ppConfigPtr->devType = tempDevId;

    /* Init the Gpp Isrs pointers array.    */
    for(ii = 0; ii < CPSS_EVENT_GPP_MAX_NUM_E; ii++)
    {
        ppConfigPtr->intCtrl.gppIsrFuncs[ii].cookie     = NULL;
        ppConfigPtr->intCtrl.gppIsrFuncs[ii].gppIsrPtr  = NULL;
    }

    return GT_OK;
}


/*******************************************************************************
* deviceTypeInfoGet
*
* DESCRIPTION:
*       This function sets the device info :
*       number of ports ,
*       deviceType ,
*       and return bmp of ports , for a given device.
*
* INPUTS:
*       devNum      - The Pp's device number.
*
* OUTPUTS:
*       existingPortsPtr - (pointer to)the BMP of existing ports
* RETURNS:
*       GT_OK on success,
*       GT_BAD_STATE - the 'multi port groups' support of the device has differences
*                      between the application and the cpssDrv DB
*       GT_NOT_FOUND if the given pciDevType is illegal.
*       GT_NOT_SUPPORTED - not properly supported device in DB
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS deviceTypeInfoGet
(
    IN  GT_U8       devNum,
    OUT CPSS_PORTS_BMP_STC  *existingPortsPtr
)
{
    GT_U32   ii;/* index in cpssSupportedTypes */
    GT_U32   jj;/* index in cpssSupportedTypes[ii].devTypeArray */
    GT_U32   kk;/* index in cpssSpecialDevicesBmp */
    GT_BOOL found = GT_FALSE;
    PRV_CPSS_DRIVER_PP_CONFIG_STC *ppConfigPtr;/* Points to the Device's Driver*/
    GT_U32  port;/*port iterator*/
    GT_U32  portInCurrentPortGroup;/*port number in current port group*/
    CPSS_PORTS_BMP_STC portsInPortGroupBmp;/*bitmap of the local ports of the port group */
    CPSS_PORTS_BMP_STC fullPortsBmp; /* full ports bmp of this device */
    GT_BOOL            needToForceOutPorts = GT_FALSE;/* indication if ports need to be forced out of the 'existing ports' */
    CPSS_PORTS_BMP_STC forcedOutPortsBmp;/* bmp of ports to be forced out of the 'existing ports' */

    ppConfigPtr = prvCpssDrvPpConfig[devNum];

    /* get the info about our device */
    ii = 0;
    while(cpssSupportedTypes[ii].devFamily != END_OF_TABLE)
    {
        jj = 0;
        while(cpssSupportedTypes[ii].devTypeArray[jj] != END_OF_TABLE)
        {
            if(ppConfigPtr->devType == cpssSupportedTypes[ii].devTypeArray[jj])
            {
                found = GT_TRUE;
                break;
            }
            jj++;
        }

        if(found == GT_TRUE)
        {
            break;
        }
        ii++;
    }

    if(found == GT_FALSE)
    {
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_FOUND, LOG_ERROR_NO_MSG);
    }

    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&forcedOutPortsBmp);

    /* do 'multi port groups' device info check */
    if(cpssSupportedTypes[ii].portGroupInfoPtr)
    {
        /*************************************************/
        /* the recognized device is 'multi port groups' device */
        /*************************************************/

        /* check that application stated that the device is 'multi port groups' device */
        if(prvCpssDrvPpConfig[devNum]->portGroupsInfo.isMultiPortGroupDevice == GT_FALSE)
        {
            /* application not aware to the 'multi port groups' device ?! */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        if(prvCpssDrvPpConfig[devNum]->portGroupsInfo.numOfPortGroups >
            cpssSupportedTypes[ii].portGroupInfoPtr->numOfPortGroups)
        {
            /* application think more port groups then really exists */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }

        if(cpssSupportedTypes[ii].activePortGroupsBmpPtr == NULL)
        {
            /* the DB of the device not hold the active port groups bitmap */
            /* internal DB error ! */
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
        }


        if(*cpssSupportedTypes[ii].activePortGroupsBmpPtr !=
           prvCpssDrvPpConfig[devNum]->portGroupsInfo.activePortGroupsBmp)
        {
            if(prvCpssDrvPpConfig[devNum]->portGroupsInfo.activePortGroupsBmp == 0)
            {
                /* the device must have at least one active port group */
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
            }

            /* application allowed to force CPSS to not access existing port group.
               (this port group may exist in the device but not connected by the PEX) */
            for(kk = 0 ; kk < cpssSupportedTypes[ii].portGroupInfoPtr->numOfPortGroups; kk++)
            {
                if((0 == ((*cpssSupportedTypes[ii].activePortGroupsBmpPtr) & (1 << kk))) &&
                   (     prvCpssDrvPpConfig[devNum]->portGroupsInfo.activePortGroupsBmp & (1 << kk)))
                {
                    /* application think that this is active port group but it is not */
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
                }

                if((     ((*cpssSupportedTypes[ii].activePortGroupsBmpPtr) & (1 << kk))) &&
                   (0 == (prvCpssDrvPpConfig[devNum]->portGroupsInfo.activePortGroupsBmp & (1 << kk))))
                {
                    /* application force CPSS to not access existing port group.
                       (this port group may exist in the device but not connected by the PEX) */
                    needToForceOutPorts = GT_TRUE;

                    /* update the bmp of 'forced out ports' */
                    forcedOutPortsBmp.ports[kk >> 1] |= (0xFFFF << ((kk & 1) * 16));
                }
            }
        }
    }
    else if(prvCpssDrvPpConfig[devNum]->portGroupsInfo.isMultiPortGroupDevice == GT_TRUE)
    {
        /* application stated that this is 'multi port groups' device */
        /* but according to DB this is 'non multi port groups' device */
        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_STATE, LOG_ERROR_NO_MSG);
    }

    ppConfigPtr->numOfPorts = cpssSupportedTypes[ii].numOfPorts;
    ppConfigPtr->devFamily = cpssSupportedTypes[ii].devFamily;

    if(debug_force_numOfPorts)
    {
        /* allow to test GM limitations on simulation :
           BC3 the GM supports only 13 ports and not full 74 ports
        */
        ppConfigPtr->numOfPorts = debug_force_numOfPorts;
    }

    if(cpssSupportedTypes[ii].defaultPortsBmpPtr == NULL)
    {
        if(cpssSupportedTypes[ii].portGroupInfoPtr == NULL)
        {
            /* no special default for the ports BMP , use continuous ports bmp */
            prvCpssDrvPortsFullMaskGet(cpssSupportedTypes[ii].numOfPorts,existingPortsPtr);
        }
        else
        {
            /* 'multi port groups' device */
            /* by default we build it's bmp according to the port groups info */

            portInCurrentPortGroup = 0;
            CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(existingPortsPtr);
            CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsInPortGroupBmp);

            portsInPortGroupBmp.ports[0] = cpssSupportedTypes[ii].portGroupInfoPtr->portsBmp;

            for(port = 0 ; port < cpssSupportedTypes[ii].numOfPorts ; port++,portInCurrentPortGroup++)
            {
                if(portInCurrentPortGroup == cpssSupportedTypes[ii].portGroupInfoPtr->maxNumPorts)
                {
                    portInCurrentPortGroup = 0;
                }

                if(ppConfigPtr->hwCtrl[((port >> 4) & (CPSS_MAX_PORT_GROUPS_CNS - 1))/*portGroupId*/].baseAddr == CPSS_PARAM_NOT_USED_CNS)
                {
                    /* non active port group */
                    continue;
                }

                if(CPSS_PORTS_BMP_IS_PORT_SET_MAC((&portsInPortGroupBmp),portInCurrentPortGroup))
                {
                    CPSS_PORTS_BMP_PORT_SET_MAC(existingPortsPtr,port);
                }
            }
        }
    }
    else
    {
        /* use special default for the ports BMP */
        *existingPortsPtr = *cpssSupportedTypes[ii].defaultPortsBmpPtr;
    }

    /****************************************************************/
    /* add here specific devices BMP of ports that are not standard */
    /****************************************************************/
    kk = 0;
    while(cpssSpecialDevicesBmp[kk].devType != END_OF_TABLE)
    {
        if(cpssSpecialDevicesBmp[kk].devType == ppConfigPtr->devType)
        {
            *existingPortsPtr = cpssSpecialDevicesBmp[kk].existingPorts;
            break;
        }
        kk++;
    }

    /* clear from the existing ports the ports beyond the 'Number of ports' */
    prvCpssDrvPortsFullMaskGet(ppConfigPtr->numOfPorts,&fullPortsBmp);

    CPSS_PORTS_BMP_BITWISE_AND_MAC(existingPortsPtr,existingPortsPtr,&fullPortsBmp);

    if(needToForceOutPorts == GT_TRUE)
    {
        /* remove the 'forced out ports' from the 'existing ports bmp'*/
        CPSS_PORTS_BMP_BITWISE_AND_NOT_MAC(existingPortsPtr,existingPortsPtr,&forcedOutPortsBmp);
    }


    return GT_OK;
}

/*******************************************************************************
* phase1Part4Init
*
* DESCRIPTION:
*       This function is called by prvCpssDrvHwPpPhase1Init()
*       This function do PCI and DMA calibration .
*
* INPUTS:
*       devNum      - The PP's device number to be initialized.
*       ppInInfoPtr    - (pointer to)   the info needed for initialization of
*                        the driver for this PP
*
* OUTPUTS:
*       revisionIdPtr - (pointer to)the revision of the PP
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS phase1Part4Init
(
    IN  GT_U8       devNum,
    IN  PRV_CPSS_DRV_PP_PHASE_1_INPUT_INFO_STC  *ppInInfoPtr,
    OUT GT_U8       *revisionIdPtr
)
{
    PRV_CPSS_DRIVER_PP_CONFIG_STC *ppConfigPtr;/* Points to the Device's Driver*/
    GT_U32  tempData = 0;/* data from the register */

    ppConfigPtr = prvCpssDrvPpConfig[devNum];

    /* set Cheetah's PCI settings */
    if((PRV_CPSS_DRV_HW_IF_PCI_COMPATIBLE_MAC(devNum))
               &&
       (ppConfigPtr->devFamily == CPSS_PP_FAMILY_CHEETAH_E ||
        ppConfigPtr->devFamily == CPSS_PP_FAMILY_CHEETAH2_E))
    {
         /* PCI Pad Calibration Configuration -
         Bits [3:0, NDrive => 0x0 (Fingers) , The value set to l_io_pad_n_drive.
         Bits [7:4], Pdrive => 0x4 (Fingers), The value set to l_io_pad_p_drive.
         */
         prvCpssDrvHwPpWriteInternalPciReg(devNum,DX_PCI_PAD_CALIB_REG_ADDR,0x40);

         /* PCI Status and Command Register:
            Bit 0, IOSpaceEn, => Enable (Internal)
            Bit 1, MemorySpaceEn, => Enable
            Bit 2, BusMasterEn, => Enable
            Bit 4, MemWrInv, => Enable  */
         prvCpssDrvHwPpWriteInternalPciReg(devNum,DX_PCI_STATUS_CMD_REG_ADDR,0x02000017);
    }

    /* Set the Rx / Tx glue byte swapping configuration     */
    if (PRV_CPSS_DRV_HW_IF_PCI_COMPATIBLE_MAC(devNum))
    {
        /* SDMA LE/BE should works the same since the CPU get the data as raw
           byte */
        /* Enable byte swap, Enable word swap  */
        prvCpssDrvHwPpSetRegField(devNum,SDMA_CFG_REG_ADDR,6,2,3);

        prvCpssDrvHwPpSetRegField(devNum,SDMA_CFG_REG_ADDR,23,2,0);

#ifdef ASIC_SIMULATION
        /*
            this section is needed for the distributed simulation.
            when the read/write DMA from Asic are done via socket.

            and because both descriptors and frames are on the DMA memory,
            we need to set the way that Device put frame on the socket.

            because the DMA parser on the CPU side will set the data from the
            socket the same way for descriptors (that are little endian format)
            and the frames .
        */
        if(sasicgSimulationRoleIsApplication == GT_TRUE)
        {
            GT_U32  swapBytes;
            /* override the setting of SDMA swap/not swap bytes */
            if(BIG_ENDIAN_E == checkHostEndian())
            {
                /* when PP read/write frame form/to DMA it should do it non-swapped
                    I.E the frame should be placed on socket 'Big endean'
                 */
                swapBytes = 0;
            }
            else
            {
                /* when PP read/write frame form/to DMA it should do it swapped
                    I.E the frame should be placed on socket 'Little endean'
                 */
                swapBytes = 1;
            }

            prvCpssDrvHwPpSetRegField(devNum,SDMA_CFG_REG_ADDR, 6,1,swapBytes);
            prvCpssDrvHwPpSetRegField(devNum,SDMA_CFG_REG_ADDR,23,1,swapBytes);
        }
#endif /*ASIC_SIMULATION*/
    }

    /* get the revision id of the PP */
    if(!CPSS_IS_EXMX_FAMILY_MAC(ppConfigPtr->devFamily))
    {
        /* RevisionID read */
        prvCpssDrvHwPpGetRegField(devNum,
                        PRV_CPSS_DEVICE_ID_AND_REV_REG_ADDR_CNS, 0, 4, &tempData) ;

        if(ppConfigPtr->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
        {
            if(tempData == 0)
            {/* if revision shows 0 read real revision from JTAG
                Revision Status */
                if(GT_OK != prvCpssDrvHwPpGetRegField(devNum, 0x8f8240,
                                                       20, 4, &tempData))
                    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);

                if(3 == tempData)
                {/* XCAT2 B0 and B1 could be distinguished just by Port Status
                                                            Register1 bit 15 */
                    if(GT_OK != prvCpssDrvHwPpGetRegField(devNum, 0xA800040,
                                                           15, 1, &tempData))
                        CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);

                    tempData = (0 == tempData) ? 3 : 4;
                }
            }
        }
#ifndef ASIC_SIMULATION
        if(ppConfigPtr->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT2_E ||
           ppConfigPtr->devFamily == CPSS_PP_FAMILY_DXCH_ALDRIN_E ||
           ppConfigPtr->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT3_E )
        {
            if(prvCpssDrvHwPpResetAndInitControllerGetRegField(devNum,
                                                                 0xF8244,
                                                                 28,
                                                                 4,
                                                                 &tempData) != GT_OK)
            {
                CPSS_LOG_ERROR_AND_RETURN_MAC(GT_HW_ERROR, LOG_ERROR_NO_MSG);
            }
        }
#endif /*ASIC_SIMULATION*/
    }
    else
    {
        /* get revision id from the PCI registers */
        prvCpssDrvHwPpReadInternalPciReg(devNum,
                        EXMX_PCI_CLASS_CODE_AND_REV_REG_ADDR,&tempData);
        tempData &= 0xFF;/* 8 bits of the register */
    }

    if(ppInInfoPtr->ppHAState == CPSS_SYS_HA_MODE_ACTIVE_E)
    {
        *revisionIdPtr = (GT_U8)tempData;
    }
    else
    {
        *revisionIdPtr = sysGenGlobalInfo.cpssPpHaDevData[devNum]->revision;
    }

    /* !!! PATCH:
       TwistD-XG behaves as TwistD revision 1, but the value written in PCI
       status register is 0. Thus, to  initialize correctly functionalities
       such as L2 learning, FDB hash function and so on, the revision number
       of TwistD-XG must not be 0.
     */
    if((ppConfigPtr->devFamily == CPSS_PP_FAMILY_TWISTD_E) &&/* Twist-D*/
       (ppConfigPtr->numOfPorts == 1) &&                    /* XG */
       (*revisionIdPtr == 0))                               /* rev 0 */
    {
        *revisionIdPtr = 1;                               /* modify to rev 1 */
    }

    return GT_OK;
}
/*******************************************************************************
* prvCpssDrvHwPpPhase1Init
*
* DESCRIPTION:
*       This function is called by cpssExMxHwPpPhase1Init() or other
*       cpss "phase 1" family functions, in order to enable PP Hw access,
*       the device number provided to this function may
*       be changed when calling prvCpssDrvPpHwPhase2Init().
*
* INPUTS:
*       devNum      - The PP's device number to be initialized.
*       ppInInfoPtr    - (pointer to)   the info needed for initialization of
*                        the driver for this PP
*
* OUTPUTS:
*       ppOutInfoPtr  - (pointer to)the info that driver return to caller.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_OUT_OF_CPU_MEM - failed to allocate CPU memory,
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*       function also bound to the "pci-standby" for HA
*
*******************************************************************************/
static GT_STATUS driverDxExMxHwPpPhase1Init
(
    IN  GT_U8                devNum,
    IN  PRV_CPSS_DRV_PP_PHASE_1_INPUT_INFO_STC  *ppInInfoPtr,
    OUT PRV_CPSS_DRV_PP_PHASE_1_OUTPUT_INFO_STC *ppOutInfoPtr
)
{
    GT_STATUS   rc;
    PRV_CPSS_DRIVER_PP_CONFIG_STC *ppConfigPtr;/* Points to the Device's Driver*/
    GT_U32      portGroupId;/* the port group Id */
    GT_U32      dummyReg;/* dummy reg address */

    /* the function bind the PCI/SMI/TWSI driver functions to the driver object
       functions for this device.*/
    rc = phase1Part1Init(devNum,ppInInfoPtr);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* NOTE :
        since the function of phase1Part2Init(...) will allocate dynamic memory
        we need to free it and restore NULL state if error occur in this current
        function driverDxExMxHwPpPhase1Init(...) after calling
        phase1Part2Init(...)

        see the use of label exit_cleanly_lbl
    */


   /* This function allocate the memory for the DB of the driver for this
      device , and set some of it's values.*/
    rc = phase1Part2Init(devNum,ppInInfoPtr);
    if(rc != GT_OK)
    {
        goto exit_cleanly_lbl;
    }

    if (CPSS_CHANNEL_PCI_E == ppInInfoPtr->mngInterfaceType)
    {
        /* relevant only for PCI devices */
        /* all PCI devices need it */
        PRV_CPSS_DRV_ERRATA_SET_MAC(
            devNum,
            PRV_CPSS_DRV_ERRATA_PCI_SLAVE_UNABLE_TO_ABORT_TRANSACTION_E);
    }


    ppConfigPtr = prvCpssDrvPpConfig[devNum];

    ppConfigPtr->portGroupsInfo.isMultiPortGroupDevice =
        (ppInInfoPtr->numOfPortGroups == 1)
            ? GT_FALSE : GT_TRUE;

    for(portGroupId = 0; portGroupId < ppInInfoPtr->numOfPortGroups; portGroupId++)
    {
        if(ppConfigPtr->hwCtrl[portGroupId].baseAddr == CPSS_PARAM_NOT_USED_CNS
                )
        {
            /* non active port group */
            continue;
        }

        /* This function call driverHwCntlInit to initialize the HW control.
           The function get the deviceType from HW , revision id , doByteSwap */
        rc = phase1Part3Init(devNum,portGroupId,ppInInfoPtr);
        if(rc != GT_OK)
        {
            goto exit_cleanly_lbl;
        }
    }

    /* starting from this point the DB of : prvCpssDrvPpConfig[devNum]->portGroupsInfo
       is ready because initialized in the prvCpssDrvHwCntlInit(..) that called
       from phase1Part3Init(..)

       so we can start use the macros for loops on port groups (supports also 'non multi port group' device):
       PRV_CPSS_DRV_START_LOOP_PORT_GROUPS_MAC
       PRV_CPSS_DRV_END_LOOP_PORT_GROUPS_MAC
    */


    /* This function sets the device info : number of ports , deviceType ,
      and return bmp of ports , for a given device. */
    rc = deviceTypeInfoGet(devNum,
                           &ppOutInfoPtr->existingPorts);
    if(rc != GT_OK)
    {
        goto exit_cleanly_lbl;
    }

    ppOutInfoPtr->devType = ppConfigPtr->devType;
    ppOutInfoPtr->numOfPorts = ppConfigPtr->numOfPorts;
    ppOutInfoPtr->devFamily = ppConfigPtr->devFamily;

    /*only summary interrupts are unmasked by default */
    /* according to the interrupt scan tree. */
    ppConfigPtr->intCtrl.needInterruptMaskDefaultValueInit = GT_TRUE;
            /* bind the ISR callback routine done before signalling app. */
    ppConfigPtr->intCtrl.bindTheIsrCbRoutineToFdbTrigEnded = GT_FALSE;

    if (((ppOutInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT2_E) ||
         (ppOutInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT3_E) ||
         (ppOutInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_ALDRIN_E) ||
         (ppOutInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E))
        && !CPSS_CHANNEL_IS_PEX_MBUS_MAC(ppInInfoPtr->mngInterfaceType))
    {
        /* Set address completion for backward compatible */
        rc = prvCpssDrvHwPpSetRegField(devNum, 0x140, 16, 1, 1);
        if(rc != GT_OK)
        {
            goto exit_cleanly_lbl;
        }
    }

    for(portGroupId = 0; portGroupId < ppInInfoPtr->numOfPortGroups; portGroupId++)
    {
        prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].doReadAfterWrite =
            PRV_CPSS_DRV_ERRATA_GET_MAC(devNum,PRV_CPSS_DRV_ERRATA_PCI_READ_AFTER_WRITE_E);
    }

    if ((ppOutInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)||
        (ppOutInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E))
    {
#ifdef PRV_CPSS_DRV_ERRATA_REFERENCE_CNS  /* macro never defined --> for references purpose only  */
    PRV_CPSS_DXCH_LION_RM_SDMA_ACTIVATION_WA_E /*Lion RM#2701*/
#endif /*PRV_CPSS_DRV_ERRATA_REFERENCE_CNS*/
        /* Lion RM#2701: SDMA activation */
        /* the code must be before calling phase1Part4Init(...) because attempt
           to access register 0x2800 will cause the PEX to hang */
        rc = prvCpssDrvHwPpSetRegField(devNum,0x58,20,1,1);
        if(rc != GT_OK)
        {
            goto exit_cleanly_lbl;
        }
    }

    /* do part 4 before init of interrupts , as the interrupts may need info
        about the 'revision' of the PP */
    rc = phase1Part4Init(devNum,ppInInfoPtr,&ppOutInfoPtr->revision);
    if(rc != GT_OK)
    {
        goto exit_cleanly_lbl;
    }

    switch(ppOutInfoPtr->devFamily)
    {
        case CPSS_PP_FAMILY_SALSA_E:
            #ifdef SAL_FAMILY
            prvCpssDrvPpIntDefSalsaInit();
            break;
            #else
            rc = GT_NOT_SUPPORTED;
            goto exit_cleanly_lbl;
            #endif

        case CPSS_PP_FAMILY_CHEETAH_E:
            if(ppOutInfoPtr->devFamily == CPSS_PP_FAMILY_CHEETAH_E)
            {
            #ifdef CHX_FAMILY
                /*Interrupts initialization for the cheetah devices.*/
                prvCpssDrvPpIntDefCheetahInit();
            #else
                rc = GT_NOT_SUPPORTED;
                goto exit_cleanly_lbl;
            #endif
            }
            /* no break ... */
        case CPSS_PP_FAMILY_CHEETAH2_E:
            if(ppOutInfoPtr->devFamily == CPSS_PP_FAMILY_CHEETAH2_E)
            {
            #ifdef CHX_FAMILY
                /*Interrupts initialization for the cheetah-2 devices.*/
                prvCpssDrvPpIntDefCheetah2Init();
            #else
                rc = GT_NOT_SUPPORTED;
                goto exit_cleanly_lbl;
            #endif
            }
            /* no break ... */
        case CPSS_PP_FAMILY_CHEETAH3_E:
            if(ppOutInfoPtr->devFamily == CPSS_PP_FAMILY_CHEETAH3_E)
            {
            #ifdef CHX_FAMILY
                /*Interrupts initialization for the cheetah-3 devices.*/
                prvCpssDrvPpIntDefCheetah3Init();
            #else
                rc = GT_NOT_SUPPORTED;
                goto exit_cleanly_lbl;
            #endif
            }
            /* no break ... */
        case CPSS_PP_FAMILY_DXCH_XCAT_E:
            if(ppOutInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_XCAT_E)
            {
            #ifdef CHX_FAMILY
                /*Interrupts initialization for the xCat devices.*/
                prvCpssDrvPpIntDefDxChXcatInit();
            #else
                rc = GT_NOT_SUPPORTED;
                goto exit_cleanly_lbl;
            #endif
            }
            /* no break ... */
        case CPSS_PP_FAMILY_DXCH_XCAT3_E:
            if(ppOutInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_XCAT3_E)
            {
            #ifdef CHX_FAMILY
                /*Interrupts initialization for the xCat3 devices.*/
                prvCpssDrvPpIntDefDxChXcat3Init(devNum);
            #else
                rc = GT_NOT_SUPPORTED;
                goto exit_cleanly_lbl;
            #endif
            }
            /* no break ... */

        case CPSS_PP_FAMILY_DXCH_XCAT2_E:
            if(ppOutInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_XCAT2_E)
            {
            #ifdef CHX_FAMILY
                /*Interrupts initialization for the xCat2 devices.*/
                prvCpssDrvPpIntDefDxChXcat2Init();
            #else
                rc = GT_NOT_SUPPORTED;
                goto exit_cleanly_lbl;
            #endif
            }
            /* no break ... */

        case CPSS_PP_FAMILY_DXCH_LION_E:
            if(ppOutInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_LION_E)
            {
            #ifdef CHX_FAMILY
                /*Interrupts initialization for the Lion devices.*/
                prvCpssDrvPpIntDefDxChLionInit();
            #else
                rc = GT_NOT_SUPPORTED;
                goto exit_cleanly_lbl;
            #endif
            }
            /* no break ... */

        case CPSS_PP_FAMILY_DXCH_LION2_E:
            if(ppOutInfoPtr->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E)
            {
            #ifdef CHX_FAMILY
                /*Interrupts initialization for the Lion2 devices.*/
                prvCpssDrvPpIntDefDxChLion2Init(devNum);
            #else
                rc = GT_NOT_SUPPORTED;
                goto exit_cleanly_lbl;
            #endif
            }

            /*only summary interrupts are unmasked by default */
            /* according to the interrupt scan tree. */
            ppConfigPtr->intCtrl.needInterruptMaskDefaultValueInit = GT_FALSE;

            break;

        case CPSS_PP_FAMILY_DXCH_BOBCAT2_E:
            switch(ppOutInfoPtr->devType)
            {
                case CPSS_BOBK_ALL_DEVICES_CASES_MAC:
                    /* indicate the sub family */
                    ppConfigPtr->devSubFamily = CPSS_PP_SUB_FAMILY_BOBCAT2_BOBK_E;
                    break;
                default:
                    break;
            }
            #ifdef CHX_FAMILY

#ifdef GM_USED
                /* Interrupts initialization for Bobcat2 devices */
                rc = prvCpssDrvPpIntDefDxChBobcat2Init__GM(devNum); /* in Lion2 file */
#else
                if (ppConfigPtr->devSubFamily == CPSS_PP_SUB_FAMILY_BOBCAT2_BOBK_E)
                {
                    rc = prvCpssDrvPpIntDefDxChBobKInit(devNum, ppOutInfoPtr->revision);
                }
                else
                {
                    rc = prvCpssDrvPpIntDefDxChBobcat2Init(devNum, ppOutInfoPtr->revision); /* in BC2 file */
                }
#endif
                if(rc != GT_OK)
                {
                    goto exit_cleanly_lbl;
                }

                /*only summary interrupts are unmasked by default */
                /* according to the interrupt scan tree. */
                ppConfigPtr->intCtrl.needInterruptMaskDefaultValueInit = GT_FALSE;
                break;
            #else
                rc = GT_NOT_SUPPORTED;
                goto exit_cleanly_lbl;
            #endif

            /* ...fall through ...*/
        case CPSS_PP_FAMILY_DXCH_BOBCAT3_E:
#ifdef GM_USED
                /* Interrupts initialization for Bobcat2 devices */
                rc = prvCpssDrvPpIntDefDxChBobcat2Init__GM(devNum); /* in Lion2 file */
#else /*!GM_USED*/
                rc = prvCpssDrvPpIntDefDxChBobcat3Init(devNum,ppOutInfoPtr->revision); /* in BC2 file */
#endif /*!GM_USED*/
                if(rc != GT_OK)
                {
                    goto exit_cleanly_lbl;
                }

                /*only summary interrupts are unmasked by default */
                /* according to the interrupt scan tree. */
                ppConfigPtr->intCtrl.needInterruptMaskDefaultValueInit = GT_FALSE;
                break;
        case CPSS_PP_FAMILY_DXCH_ALDRIN_E:
#ifdef GM_USED
                /* Interrupts initialization for Bobcat2 devices */
                rc = prvCpssDrvPpIntDefDxChBobcat2Init__GM(devNum); /* in Lion2 file */
#else
                CPSS_TBD_BOOKMARK_ALDRIN /* use bobk definitions so far */
                rc = prvCpssDrvPpIntDefDxChBobKInit(devNum, ppOutInfoPtr->revision);
#endif
                if(rc != GT_OK)
                {
                    goto exit_cleanly_lbl;
                }

                /*only summary interrupts are unmasked by default */
                /* according to the interrupt scan tree. */
                ppConfigPtr->intCtrl.needInterruptMaskDefaultValueInit = GT_FALSE;
                break;

        case CPSS_PP_FAMILY_TWISTD_E:
            if(ppOutInfoPtr->devFamily == CPSS_PP_FAMILY_TWISTD_E)
            {
            #if defined(EX_FAMILY) || defined(MX_FAMILY)
                PRV_CPSS_DRV_ERRATA_SET_MAC(devNum,PRV_CPSS_DRV_ERRATA_PCI_READ_AFTER_WRITE_E);
                /*Interrupts initialization for the Twist-D devices.*/
                prvCpssDrvPpIntDefTwistDInit();
            #else
                rc = GT_NOT_SUPPORTED;
                goto exit_cleanly_lbl;
            #endif
            }
            /* no break ... */
        case CPSS_PP_FAMILY_TWISTC_E:
            if(ppOutInfoPtr->devFamily == CPSS_PP_FAMILY_TWISTC_E)
            {
            #if defined(EX_FAMILY) || defined(MX_FAMILY)
                PRV_CPSS_DRV_ERRATA_SET_MAC(devNum,PRV_CPSS_DRV_ERRATA_PCI_READ_AFTER_WRITE_E);
                /*Interrupts initialization for the Twist-C devices.*/
                prvCpssDrvPpIntDefTwistCInit(GT_TRUE);
            #else
                rc = GT_NOT_SUPPORTED;
                goto exit_cleanly_lbl;
            #endif
            }
            /* no break ... */
        case CPSS_PP_FAMILY_TIGER_E:
            if(ppOutInfoPtr->devFamily == CPSS_PP_FAMILY_TIGER_E)
            {
            #ifdef TG_FAMILY
                /*Interrupts initialization for the Tiger devices.*/
                prvCpssDrvPpIntDefTigerInit();
            #else
                rc = GT_NOT_SUPPORTED;
                goto exit_cleanly_lbl;
            #endif
            }
            /* no break ... */
        case CPSS_PP_FAMILY_SAMBA_E:
            if(ppOutInfoPtr->devFamily == CPSS_PP_FAMILY_SAMBA_E)
            {
                PRV_CPSS_DRV_ERRATA_SET_MAC(devNum,PRV_CPSS_DRV_ERRATA_PCI_READ_AFTER_WRITE_E);
                /*prvCpssDrvPpIntDefSambaInit();*/
            }

            ppConfigPtr->hwCtrl[CPSS_NON_MULTI_PORT_GROUP_DEVICE_INDEX_CNS].dummyReadAfterWriteRegAddr= 0x0380001C;/*relevant to PCI */

            break;
        case CPSS_PP_FAMILY_PUMA_E:
        #ifdef EXMXPM_FAMILY
            /*Interrupts initialization for the Puma devices.*/
            prvCpssDrvPpIntDefPumaInit();

            /*only summary interrupts are unmasked by default */
            /* according to the interrupt scan tree. */
            ppConfigPtr->intCtrl.needInterruptMaskDefaultValueInit = GT_FALSE;
            break;
        #else
            rc = GT_NOT_SUPPORTED;
            goto exit_cleanly_lbl;
        #endif
        case CPSS_PP_FAMILY_PUMA3_E:
        #ifdef EXMXPM_FAMILY
            /*Interrupts initialization for the Puma devices.*/
            prvCpssDrvPpIntDefPuma3Init();
            /*only summary interrupts are unmasked by default */
            /* according to the interrupt scan tree. */
            ppConfigPtr->intCtrl.needInterruptMaskDefaultValueInit = GT_FALSE;
            break;
        #else
            rc = GT_NOT_SUPPORTED;
            goto exit_cleanly_lbl;
        #endif
        default:
            rc = GT_NOT_SUPPORTED;
            goto exit_cleanly_lbl;
    }
    /* save the sub family */
    ppOutInfoPtr->devSubFamily = ppConfigPtr->devSubFamily;

    /* bind the ISR callback routine done before signalling app. */
    rc = prvCpssDrvExMxDxHwPpPexAddrGet(ppOutInfoPtr->devFamily,&dummyReg,&ppConfigPtr->intCtrl.intSumMaskRegAddr);
    if(rc != GT_OK)
    {
        goto exit_cleanly_lbl;
    }

exit_cleanly_lbl:
    if(rc != GT_OK)
    {
        if(rc != GT_BAD_STATE && /* another device is in this memory --
                                    don't free the memory */
           prvCpssDrvPpConfig[devNum])
        {
            /* release the allocation */
            cpssOsFree(prvCpssDrvPpConfig[devNum]);
            prvCpssDrvPpConfig[devNum] = NULL;
        }
    }

    return rc;
}

/*******************************************************************************
* driverDxExMxHwPpRamBurstConfigSet
*
* DESCRIPTION:
*       Sets the Ram burst information for a given device.
*
* INPUTS:
*       devNum          - The Pp's device number.
*       ramBurstInfoPtr - A list of Ram burst information for this device.
*       burstInfoLen    - Number of valid entries in ramBurstInfo.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_OUT_OF_CPU_MEM - failed to allocate CPU memory,
*       GT_ALREADY_EXIST - the ram already initialized
*
* COMMENTS:
*
*       function also bound to the "pci-standby" for HA
*
*******************************************************************************/
static GT_STATUS driverDxExMxHwPpRamBurstConfigSet
(
    IN  GT_U8               devNum,
    IN  PRV_CPSS_DRV_RAM_BURST_INFO_STC   *ramBurstInfoPtr,
    IN  GT_U8               burstInfoLen
)
{
    GT_U32  portGroupId;

    if(CPSS_IS_DXCH_FAMILY_MAC(prvCpssDrvPpConfig[devNum]->devFamily) ||
       CPSS_IS_DXSAL_FAMILY_MAC(prvCpssDrvPpConfig[devNum]->devFamily) )
    {
        /* no more to do */
        return GT_OK;
    }

    PRV_CPSS_DRV_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        if(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].ramBurstInfo != NULL ||
           prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].ramBurstInfoLen != 0 )
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_BAD_PARAM, LOG_ERROR_NO_MSG);
        }

        prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].ramBurstInfo =
            cpssOsMalloc(sizeof(PRV_CPSS_DRV_RAM_BURST_INFO_STC)* burstInfoLen);
        if(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].ramBurstInfo == NULL)
        {
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_OUT_OF_CPU_MEM, LOG_ERROR_NO_MSG);
        }

        cpssOsMemCpy(prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].ramBurstInfo,ramBurstInfoPtr,
                 sizeof(PRV_CPSS_DRV_RAM_BURST_INFO_STC) * burstInfoLen);

        prvCpssDrvPpConfig[devNum]->hwCtrl[portGroupId].ramBurstInfoLen = burstInfoLen;
    }
    PRV_CPSS_DRV_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* driverExMxDxHwPpHaModeSet
*
* DESCRIPTION:
*       function to set CPU High Availability mode of operation.
*
*  APPLICABLE DEVICES:  all ExMx devices
*
* INPUTS:
*       devNum       - the device number.
*       mode - active or standby
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - on failure.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS driverExMxDxHwPpHaModeSet
(
    IN  GT_U8   devNum,
    IN  CPSS_SYS_HA_MODE_ENT mode
)
{
    GT_STATUS   rc;
    PRV_CPSS_DRIVER_PP_CONFIG_STC *ppConfigPtr;/* Points to the Device's Driver*/
    GT_U32      portGroupId;/* the port group Id */

    if(0 == PRV_CPSS_DRV_HW_IF_PCI_COMPATIBLE_MAC(devNum))
    {
        /* nothing to update -- not supported option yet */
        return GT_OK;
    }

    ppConfigPtr =  prvCpssDrvPpConfig[devNum];

    PRV_CPSS_DRV_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        if(mode == CPSS_SYS_HA_MODE_ACTIVE_E)
        {
            /* update the pointer to the functions object -- to use the PCI */
            prvCpssDrvPpObjConfig[devNum]->busPtr = prvCpssDrvMngInfPciPtr;
        }
        else
        {
            /* update the pointer to the functions object -- to emulate the PCI */
            prvCpssDrvPpObjConfig[devNum]->busPtr = prvCpssDrvMngInfPciHaStandbyPtr;
        }

        /* we need to re-initialize the HW control driver --
           because we changed the OBJ of the prvCpssDrvHwCntlInit */
        /* Init Hw Cntl parameters. Now we know the value of doByteSwap */
        rc = prvCpssDrvHwCntlInit(devNum,portGroupId,
                         ppConfigPtr->hwCtrl[portGroupId].baseAddr,
                         ppConfigPtr->hwCtrl[portGroupId].internalPciBase,
                         GT_FALSE);/* isDiag */
        if (rc!= GT_OK)
            return rc;
    }
    PRV_CPSS_DRV_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    prvCpssDrvPpObjConfig[devNum]->HAState = mode;

    return GT_OK;
}

#ifdef CHX_FAMILY

/*******************************************************************************
* prvCpssDrvLion2FirstPacketWA
*
* DESCRIPTION:
*      This function engages Lion2 A0 517 (DIP) FE-3832472 "First Packet drop"
*      erratum WA, which must be before any device init activity
*
* INPUTS:
*       devNum          - The PP device number to read from.
*       portGroupId     - number of core(port group)
*       ppInInfoPtr    - (pointer to)   the info needed for initialization of
*                        the driver for this PP
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success
*       GT_HW_ERROR - if HW access failed
*
* COMMENTS:
*       Should be engaged only for A0 revision.
*
*******************************************************************************/
static GT_STATUS prvCpssDrvLion2FirstPacketWA
(
    IN GT_U8    devNum,
    IN GT_U32   portGroupId,
    IN PRV_CPSS_DRV_PP_PHASE_1_INPUT_INFO_STC  *ppInInfoPtr
)
{
    GT_STATUS   rc;     /* return code */
    GT_U32      pcsNum; /* number of PCS lane */
    GT_U32      regAddr;/* register address */
#ifndef ASIC_SIMULATION
    GT_U32      timeOut;/* counter for wait after reast */
#endif
    GT_U32      regData;/* register data */

    /* Lion2 517 (DIP) FE-3832472 "First Packet drop" erratum WA */
    rc = prvCpssDrvLion2MMPcsUnReset(devNum,portGroupId);
    if(rc != GT_OK)
        return rc;
    /* stop MAC clock */
    for(pcsNum = 0; pcsNum < 16; pcsNum++)
    {
        regAddr = 0x88C0414 + pcsNum * 0x1000;
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId, regAddr,
                                                12, 1, 0);
        if (rc!= GT_OK)
            return rc;
    }

    /* execute device reset just when coming to last port group */
    if(portGroupId == ppInInfoPtr->numOfPortGroups-1)
    {
        /* MG Soft Reset */
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, 1,0x18F805C,
                                                8, 1, 0);
        if (rc!= GT_OK)
            return rc;

        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, 1,0x18F8060,
                                                8, 1, 0);
        if (rc!= GT_OK)
            return rc;

        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, 1,0x18F800C,
                                                1, 1, 0);
        if (rc!= GT_OK)
            return rc;

        cpssOsTimerWkAfter(10);

        /* dummy reads to ensure right delay */
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, 1, 0x58,
                                                 &regData);
        if (rc!= GT_OK)
            return rc;

        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, 1, 0xF80002C,
                                                 &regData);
        if (rc!= GT_OK)
            return rc;

        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, 1, 0xF800038,
                                                 &regData);
        if (rc!= GT_OK)
            return rc;

#ifndef ASIC_SIMULATION
        /* Verify SW reset finished */
        timeOut = 10;
        do
        {
            rc = prvCpssDrvHwPpPortGroupGetRegField(devNum, 1,0x18F800C,
                                            1, 1, &regData);
            if (rc!= GT_OK)
                return rc;

            timeOut--;
            cpssOsTimerWkAfter(10);
        }while((regData != 1) && (timeOut > 0));

        if((regData != 1) && (timeOut == 0))
            CPSS_LOG_ERROR_AND_RETURN_MAC(GT_TIMEOUT, LOG_ERROR_NO_MSG);
#endif
    }/* end if(portGroupId == */

    return GT_OK;
}

/*******************************************************************************
* prvCpssDrvLion2MMPcsUnReset
*
* DESCRIPTION:
*       Unreset Lion2 MMPCS
*
* INPUTS:
*       devNum    - system device number
*       portGroupId - port group (core) number
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS prvCpssDrvLion2MMPcsUnReset
(
    GT_U8                   devNum,
    GT_U32                  portGroupId
)
{
    GT_STATUS   rc;     /* return code */
    GT_U32      regAddr; /* address of register */
    GT_U32      pcsNum; /* PCS lane number */

    for(pcsNum = 0; pcsNum < 16; pcsNum++)
    {
        regAddr = 0x088c054c + pcsNum * 0x1000;
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId ,regAddr,
                                                0, 1, 1);
        if (rc!= GT_OK)
            return rc;
    }

    for(pcsNum = 0; pcsNum < 16; pcsNum++)
    {
        regAddr = 0x088c054c + pcsNum * 0x1000;
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId ,regAddr,
                                                1, 1, 1);
        if (rc!= GT_OK)
            return rc;
    }

    for(pcsNum = 0; pcsNum < 16; pcsNum++)
    {
        regAddr = 0x088c054c + pcsNum * 0x1000;
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId ,regAddr,
                                                2, 1, 1);
        if (rc!= GT_OK)
            return rc;
    }

    for(pcsNum = 0; pcsNum < 16; pcsNum++)
    {
        regAddr = 0x088c054c + pcsNum * 0x1000;
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId ,regAddr,
                                                5, 1, 1);
        if (rc!= GT_OK)
            return rc;
    }

    for(pcsNum = 0; pcsNum < 16; pcsNum++)
    {
        regAddr = 0x088c054c + pcsNum * 0x1000;
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId ,regAddr,
                                                6, 1, 1);
        if (rc!= GT_OK)
            return rc;
    }

    for(pcsNum = 0; pcsNum < 16; pcsNum++)
    {
        regAddr = 0x088c054c + pcsNum * 0x1000;
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId ,regAddr,
                                                4, 1, 1);
        if (rc!= GT_OK)
            return rc;
    }

    for(pcsNum = 0; pcsNum < 16; pcsNum++)
    {
        regAddr = 0x088c054c + pcsNum * 0x1000;
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId ,regAddr,
                                                3, 1, 1);
        if (rc!= GT_OK)
            return rc;
    }

    for(pcsNum = 0; pcsNum < 16; pcsNum++)
    {
        regAddr = 0x088c054c + pcsNum * 0x1000;
        rc = prvCpssDrvHwPpPortGroupSetRegField(devNum, portGroupId ,regAddr,
                                                7, 4, 0x4);
        if (rc!= GT_OK)
            return rc;
    }

    return GT_OK;
}

#endif /* CHX_FAMILY */

/*******************************************************************************
* prvCpssDrvDxExMxInitObject
*
* DESCRIPTION:
*      This function creates and initializes ExMxDx device driver object
*
* INPUTS:
*       devNum          - The PP device number to read from.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - on success
*       GT_ALREADY_EXIST - if the driver object have been created before
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDrvDxExMxInitObject
(
     IN GT_U8   devNum
)
{
    GT_STATUS rc;

    /****************************************************/
    /* bind the generic part to the ExMxDx generic part */
    /****************************************************/
    prvCpssDrvPpObjConfig[devNum]->genPtr = prvCpssDrvGenExMxDxObjPtr;
    /* the bus part will be initialized in "phase 1" -
       when we get the parameter */
    prvCpssDrvPpObjConfig[devNum]->busPtr = NULL;
    prvCpssDrvPpObjConfig[devNum]->HAState = CPSS_SYS_HA_MODE_ACTIVE_E;

    /* driver object initialization */
    prvCpssDrvGenExMxDxObjPtr->drvHwPpCfgPhase1Init = driverDxExMxHwPpPhase1Init;
    prvCpssDrvGenExMxDxObjPtr->drvHwPpSetRamBurstConfig = driverDxExMxHwPpRamBurstConfigSet;
    prvCpssDrvGenExMxDxObjPtr->drvHwPpHaModeSet = driverExMxDxHwPpHaModeSet;
    prvCpssDrvGenExMxDxObjPtr->drvHwPpInterruptsTreeGet = prvCpssDrvExMxDxHwPpInterruptsTreeGet;
    rc = prvCpssDrvDxExMxEventsInitObject();

    if(rc != GT_OK)
        return rc;

    rc = prvCpssDrvDxExMxIntInitObject();

    return rc;
}


