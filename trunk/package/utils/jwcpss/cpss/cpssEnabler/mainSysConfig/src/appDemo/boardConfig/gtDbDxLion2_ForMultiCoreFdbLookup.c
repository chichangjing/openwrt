/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtDbDxLion2_96XG_ForMultiCoreFdbLookup.c
*
* DESCRIPTION:
*       Initialization functions for the Lion2 board. -- ForMultiCoreFdbLookup specifics
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*
*******************************************************************************/

#include <appDemo/boardConfig/appDemoBoardConfig.h>
#include <appDemo/boardConfig/appDemoCfgMisc.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfig.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfigDefaults.h>
#include <appDemo/sysHwConfig/appDemoDb.h>
#include <appDemo/boardConfig/gtBoardsConfigFuncs.h>

/* add generic include files */
#include <cpss/dxCh/dxChxGen/multiPortGroup/cpssDxChMultiPortGroup.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h>
#include <cpss/dxCh/dxChxGen/trunk/cpssDxChTrunk.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/tti/cpssDxChTti.h>

/* max number of uplinks */
#define MAX_UPLINKS_CNS             8

/* max number of actual interconnection ports used */
#define MAX_NUM_OF_INTERCONNECTION_PORTS_CNS    (64 * 2)

extern GT_STATUS appDemoLionMultiCoreTtiLookupSupport(IN GT_U8   devNum);
/* max number of internal trunks */
#define MAX_INTERNAL_TRUNKS_CNS         (2*12)

/* array of internal trunks */
static GT_TRUNK_ID      internalTrunksArr[MAX_INTERNAL_TRUNKS_CNS];
/* current index in internalTrunksArr[]*/
static GT_U32           internalTrunksIndex = 0;

/* REV_ID_ENT : enum for supported board revisions
    REV_ID_RESERVED_1_E - reserved
    REV_ID_CALLER_CHOICE_E - the caller choose the exact infrastructure,
                             - the caller can choose not to do any special infrastructure.
    REV_ID_RESERVED_2_E - reserved
    REV_ID_ENHANCED_AUTO_4_FDB_256K_E - auto build for enhanced 256K mode -- quad FDB
    REV_ID_ENHANCED_AUTO_2_FDB_128K_E - auto build for enhanced 128K mode -- double FDB
*/
enum REV_ID_ENT{
    REV_ID_RESERVED_1_E      = 20 + 1,    /*21*/
    REV_ID_CALLER_CHOICE_E,                                          /*22*/
    REV_ID_RESERVED_2_E,                                             /*23*/
    REV_ID_ENHANCED_AUTO_4_FDB_256K_E,                              /*24*/
    REV_ID_ENHANCED_AUTO_2_FDB_128K_E,                              /*25*/
    REV_ID_LION2_360MHz_4x10G_3x40G_E,                              /*26*/
    REV_ID_LAST_E
};

/*******************************************************************************
 * External variables
 ******************************************************************************/
static GT_U8   genericBoardRevision = 1;

/* genericBoardConfigFun will hold the generic board configuration */
/* we will use those pointers as "set default" values that we can override */
static GT_BOARD_CONFIG_FUNCS genericBoardConfigFun;

static GT_U32  numOfUplinkPorts = 0;
static GT_PORT_NUM   uplinkPortsArr[MAX_UPLINKS_CNS] = {0,18,34,48 , 66,82,98,122};

static CPSS_DXCH_MULTI_PORT_GROUP_RING_INFO_STC   ringInfoArr[CPSS_MAX_PORT_GROUPS_CNS];
static GT_U32  ringInfoNum = 0;


/* current index in interconnectionPortsArr[]*/
static GT_U32           interconnectionIndex = 0;


typedef struct{
    GT_TRUNK_ID       trunkId;
    GT_U32            numOfPorts;
    CPSS_DXCH_MULTI_PORT_GROUP_SRC_PORT_HASH_PAIR_STC  *portsArr;
    CPSS_DXCH_MULTI_PORT_GROUP_SRC_HASH_MODE_ENT  mode;
}TRUNK_SRC_PORT_HASH_INFO_STC;

/* trunk : source port hash info */
static TRUNK_SRC_PORT_HASH_INFO_STC trunkSrcPortHashInfo[MAX_INTERNAL_TRUNKS_CNS];
/* number of info's in trunkSrcPortHashInfo[] */
static GT_U32   trunkSrcPortHashInfoNum = 0;


/* current index in pairsArrFor256k_enhanced[] */
static GT_U32   pairsIndex = 0;

/* fdb Mode */
static CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT fdbMode =
    CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_LINKED_E;

/******* start of enhanced mode settings **************************************/

static GT_TRUNK_ID      nextTrunkArr_enhanced[CPSS_MAX_PORT_GROUPS_CNS] = {
        0x41,0x42,0x43,0x40,
        0x45,0x46,0x47,0x44
        };

/* array of internal trunks - 256k mode */
static GT_TRUNK_ID      internalTrunks256kArr_enhanced[MAX_INTERNAL_TRUNKS_CNS] = {
            0x41,0x42,0x43,/* trunks order that used for traffic ingress port group 0 */
            0x42,0x43,0x40,/* trunks order that used for traffic ingress port group 1 */
            0x43,0x40,0x41,/* trunks order that used for traffic ingress port group 2 */
            0x40,0x41,0x42,/* trunks order that used for traffic ingress port group 3 */

            0x45,0x46,0x47,/* trunks order that used for traffic ingress port group 4 */
            0x46,0x47,0x44,/* trunks order that used for traffic ingress port group 5 */
            0x47,0x44,0x45,/* trunks order that used for traffic ingress port group 6 */
            0x44,0x45,0x46 /* trunks order that used for traffic ingress port group 7 */
            };

/* array of internal trunks - 128k mode */
static GT_TRUNK_ID      internalTrunks128kArr_enhanced[MAX_INTERNAL_TRUNKS_CNS] = {
            0x41,/* trunks order that used for traffic ingress port group 0 */
            0x40,/* trunks order that used for traffic ingress port group 1 */
            0x43,/* trunks order that used for traffic ingress port group 2 */
            0x42,/* trunks order that used for traffic ingress port group 3 */

            0x45,/* trunks order that used for traffic ingress port group 4 */
            0x44,/* trunks order that used for traffic ingress port group 5 */
            0x47,/* trunks order that used for traffic ingress port group 6 */
            0x46,/* trunks order that used for traffic ingress port group 7 */
            };

/*array for the interconnection ports (ring ports) - 256k mode*/
static GT_PHYSICAL_PORT_NUM            interconnectionPorts256kArr_enhanced[MAX_NUM_OF_INTERCONNECTION_PORTS_CNS] =
{
17,19,33,37,53,59,/* starting from port group 0 */
35,41,49,55, 1, 5,/* starting from port group 1 */
57,51, 3, 7,21,25,/* starting from port group 2 */
 9,11,23,27,39,43,/* starting from port group 3 */

17+64,19+64,33+64,37+64,53+64,59+64,/* starting from port group 4 */
35+64,41+64,49+64,55+64, 1+64, 5+64,/* starting from port group 5 */
57+64,51+64, 3+64, 7+64,21+64,25+64,/* starting from port group 6 */
 9+64,11+64,23+64,27+64,39+64,43+64,/* starting from port group 7 */

};

/*array for the interconnection ports (ring ports) - 128K mode*/
static GT_PHYSICAL_PORT_NUM            interconnectionPorts128kArr_enhanced[MAX_NUM_OF_INTERCONNECTION_PORTS_CNS] =
{
17,19,21,23,25,27,/* starting from port group 0 */
 1, 3, 5, 7, 9,11,/* starting from port group 1 */
49,51,53,55,57,59,/* starting from port group 2 */
33,35,37,39,41,43,/* starting from port group 3 */

17+64,19+64,21+64,23+64,25+64,27+64,/* starting from port group 4 */
 1+64, 3+64, 5+64, 7+64, 9+64,11+64,/* starting from port group 5 */
49+64,51+64,53+64,55+64,57+64,59+64,/* starting from port group 6 */
33+64,35+64,37+64,39+64,41+64,43+64,/* starting from port group 7 */

0 /* all others don't care */
};


/*array for the pairs of ports (source port + trunk port) , for 256K mode*/
static CPSS_DXCH_MULTI_PORT_GROUP_SRC_PORT_HASH_PAIR_STC   pairsArrFor256k_enhanced[MAX_NUM_OF_INTERCONNECTION_PORTS_CNS] =
    {
        {0  /* uplink  - port group 0*/ , 17 /*ring port group 1*/} ,
        {2  /* network - port group 0*/ , 19 /*ring port group 1*/} ,
        {4  /* network - port group 0*/ , 19 /*ring port group 1*/} ,
        {6  /* network - port group 0*/ , 19 /*ring port group 1*/} ,
        {8  /* network - port group 0*/ , 19 /*ring port group 1*/} ,
        {10 /* network - port group 0*/ , 19 /*ring port group 1*/} ,
        {3  /* ring    - port group 0*/ , 25 /*ring port group 1*/} ,
        {7  /* ring    - port group 0*/ , 21 /*ring port group 1*/} ,
        {9  /* ring    - port group 0*/ , 23 /*ring port group 1*/} ,
        {11 /* ring    - port group 0*/ , 27 /*ring port group 1*/} ,

        {16 /* network - port group 1*/ , 41 /*ring port group 2*/} ,
        {18 /* uplink  - port group 1*/ , 35 /*ring port group 2*/} ,
        {20 /* network - port group 1*/ , 41 /*ring port group 2*/} ,
        {22 /* network - port group 1*/ , 41 /*ring port group 2*/} ,
        {24 /* network - port group 1*/ , 41 /*ring port group 2*/} ,
        {26 /* network - port group 1*/ , 41 /*ring port group 2*/} ,
        {17 /* ring    - port group 0*/ , 33 /*ring port group 2*/} ,
        {19 /* ring    - port group 0*/ , 37 /*ring port group 2*/} ,
        {23 /* ring    - port group 0*/ , 39 /*ring port group 2*/} ,
        {27 /* ring    - port group 0*/ , 43 /*ring port group 2*/} ,

        {32 /* network - port group 2*/ , 51 /*ring port group 3*/} ,
        {34 /* uplink  - port group 2*/ , 57 /*ring port group 3*/} ,
        {36 /* network - port group 2*/ , 51 /*ring port group 3*/} ,
        {38 /* network - port group 2*/ , 51 /*ring port group 3*/} ,
        {40 /* network - port group 2*/ , 51 /*ring port group 3*/} ,
        {42 /* network - port group 2*/ , 51 /*ring port group 3*/} ,
        {33 /* ring    - port group 0*/ , 53 /*ring port group 3*/} ,
        {35 /* ring    - port group 0*/ , 55 /*ring port group 3*/} ,
        {37 /* ring    - port group 0*/ , 59 /*ring port group 3*/} ,
        {41 /* ring    - port group 0*/ , 49 /*ring port group 3*/} ,

        {48 /* uplink  - port group 3*/ ,  9 /*ring port group 0*/} ,
        {50 /* network - port group 3*/ , 11 /*ring port group 0*/} ,
        {52 /* network - port group 3*/ , 11 /*ring port group 0*/} ,
        {54 /* network - port group 3*/ , 11 /*ring port group 0*/} ,
        {56 /* network - port group 3*/ , 11 /*ring port group 0*/} ,
        {58 /* network - port group 3*/ , 11 /*ring port group 0*/} ,
        {49 /* ring    - port group 0*/ ,  1 /*ring port group 0*/} ,
        {51 /* ring    - port group 0*/ ,  3 /*ring port group 0*/} ,
        {55 /* ring    - port group 0*/ ,  5 /*ring port group 0*/} ,
        {57 /* ring    - port group 0*/ ,  7 /*ring port group 0*/} ,

/**************************************************************/

        {0  +64/* network - port group 0*/ , 19 +64/*ring port group 5*/} ,
        {2  +64/* uplink  - port group 0*/ , 17 +64/*ring port group 5*/} ,
        {4  +64/* network - port group 0*/ , 19 +64/*ring port group 5*/} ,
        {6  +64/* network - port group 0*/ , 19 +64/*ring port group 5*/} ,
        {8  +64/* network - port group 0*/ , 19 +64/*ring port group 5*/} ,
        {10 +64/* network - port group 0*/ , 19 +64/*ring port group 5*/} ,
        {3  +64/* ring    - port group 0*/ , 25 +64/*ring port group 5*/} ,
        {7  +64/* ring    - port group 0*/ , 21 +64/*ring port group 5*/} ,
        {9  +64/* ring    - port group 0*/ , 23 +64/*ring port group 5*/} ,
        {11 +64/* ring    - port group 0*/ , 27 +64/*ring port group 5*/} ,

        {16 +64/* network - port group 1*/ , 41 +64/*ring port group 6*/} ,
        {18 +64/* uplink  - port group 1*/ , 35 +64/*ring port group 6*/} ,
        {20 +64/* network - port group 1*/ , 41 +64/*ring port group 6*/} ,
        {22 +64/* network - port group 1*/ , 41 +64/*ring port group 6*/} ,
        {24 +64/* network - port group 1*/ , 41 +64/*ring port group 6*/} ,
        {26 +64/* network - port group 1*/ , 41 +64/*ring port group 6*/} ,
        {17 +64/* ring    - port group 0*/ , 33 +64/*ring port group 6*/} ,
        {19 +64/* ring    - port group 0*/ , 37 +64/*ring port group 6*/} ,
        {23 +64/* ring    - port group 0*/ , 39 +64/*ring port group 6*/} ,
        {27 +64/* ring    - port group 0*/ , 43 +64/*ring port group 6*/} ,

        {32 +64/* network - port group 2*/ , 51 +64/*ring port group 7*/} ,
        {34 +64/* uplink  - port group 2*/ , 57 +64/*ring port group 7*/} ,
        {36 +64/* network - port group 2*/ , 51 +64/*ring port group 7*/} ,
        {38 +64/* network - port group 2*/ , 51 +64/*ring port group 7*/} ,
        {40 +64/* network - port group 2*/ , 51 +64/*ring port group 7*/} ,
        {42 +64/* network - port group 2*/ , 51 +64/*ring port group 7*/} ,
        {33 +64/* ring    - port group 0*/ , 53 +64/*ring port group 7*/} ,
        {35 +64/* ring    - port group 0*/ , 55 +64/*ring port group 7*/} ,
        {37 +64/* ring    - port group 0*/ , 59 +64/*ring port group 7*/} ,
        {41 +64/* ring    - port group 0*/ , 49 +64/*ring port group 7*/} ,

        {48 +64/* network - port group 3*/ , 11 +64/*ring port group 4*/} ,
        {50 +64/* network - port group 3*/ , 11 +64/*ring port group 4*/} ,
        {52 +64/* network - port group 3*/ , 11 +64/*ring port group 4*/} ,
        {54 +64/* network - port group 3*/ , 11 +64/*ring port group 4*/} ,
        {56 +64/* network - port group 3*/ , 11 +64/*ring port group 4*/} ,
        {58 +64/* uplink  - port group 3*/ ,  9 +64/*ring port group 4*/} ,
        {49 +64/* ring    - port group 0*/ ,  1 +64/*ring port group 4*/} ,
        {51 +64/* ring    - port group 0*/ ,  3 +64/*ring port group 4*/} ,
        {55 +64/* ring    - port group 0*/ ,  5 +64/*ring port group 4*/} ,
        {57 +64/* ring    - port group 0*/ ,  7 +64/*ring port group 4*/} ,

        {0,0}
    };

/*array for the pairs of ports (source port + trunk port) , for 128K mode*/
static CPSS_DXCH_MULTI_PORT_GROUP_SRC_PORT_HASH_PAIR_STC   pairsArrFor128k_enhanced[MAX_NUM_OF_INTERCONNECTION_PORTS_CNS] =
{
        {0  /* uplink  - port group 0*/ , 17 /*ring port group 1*/} ,
        {2  /* network - port group 0*/ , 21 /*ring port group 1*/} ,
        {4  /* network - port group 0*/ , 23 /*ring port group 1*/} ,
        {6  /* network - port group 0*/ , 19 /*ring port group 1*/} ,
        {8  /* network - port group 0*/ , 25 /*ring port group 1*/} ,
        {10 /* network - port group 0*/ , 27 /*ring port group 1*/} ,

        {16 /* network - port group 1*/ ,  7 /*ring port group 0*/} ,
        {18 /* uplink  - port group 1*/ ,  5 /*ring port group 0*/} ,
        {20 /* network - port group 1*/ ,  9 /*ring port group 0*/} ,
        {22 /* network - port group 1*/ ,  1 /*ring port group 0*/} ,
        {24 /* network - port group 1*/ ,  3 /*ring port group 0*/} ,
        {26 /* network - port group 1*/ , 11 /*ring port group 0*/} ,

        {32 /* network - port group 2*/ , 53 /*ring port group 3*/} ,
        {34 /* uplink  - port group 2*/ , 57 /*ring port group 3*/} ,
        {36 /* network - port group 2*/ , 55 /*ring port group 3*/} ,
        {38 /* network - port group 2*/ , 59 /*ring port group 3*/} ,
        {40 /* network - port group 2*/ , 49 /*ring port group 3*/} ,
        {42 /* network - port group 2*/ , 51 /*ring port group 3*/} ,

        {48 /* uplink  - port group 3*/ , 39 /*ring port group 2*/} ,
        {50 /* network - port group 3*/ , 33 /*ring port group 2*/} ,
        {52 /* network - port group 3*/ , 35 /*ring port group 2*/} ,
        {54 /* network - port group 3*/ , 37 /*ring port group 2*/} ,
        {56 /* network - port group 3*/ , 41 /*ring port group 2*/} ,
        {58 /* network - port group 3*/ , 43 /*ring port group 2*/} ,

/**************************************************************/

        {0  +64/* network - port group 0*/ , 21 +64/*ring port group 5*/} ,
        {2  +64/* uplink  - port group 0*/ , 17 +64/*ring port group 5*/} ,
        {4  +64/* network - port group 0*/ , 23 +64/*ring port group 5*/} ,
        {6  +64/* network - port group 0*/ , 19 +64/*ring port group 5*/} ,
        {8  +64/* network - port group 0*/ , 25 +64/*ring port group 5*/} ,
        {10 +64/* network - port group 0*/ , 27 +64/*ring port group 5*/} ,

        {16 +64/* network - port group 1*/ ,  7 +64/*ring port group 4*/} ,
        {18 +64/* uplink  - port group 1*/ ,  5 +64/*ring port group 4*/} ,
        {20 +64/* network - port group 1*/ ,  9 +64/*ring port group 4*/} ,
        {22 +64/* network - port group 1*/ ,  1 +64/*ring port group 4*/} ,
        {24 +64/* network - port group 1*/ ,  3 +64/*ring port group 4*/} ,
        {26 +64/* network - port group 1*/ , 11 +64/*ring port group 4*/} ,

        {32 +64/* network - port group 2*/ , 53 +64/*ring port group 7*/} ,
        {34 +64/* uplink  - port group 2*/ , 57 +64/*ring port group 7*/} ,
        {36 +64/* network - port group 2*/ , 55 +64/*ring port group 7*/} ,
        {38 +64/* network - port group 2*/ , 59 +64/*ring port group 7*/} ,
        {40 +64/* network - port group 2*/ , 49 +64/*ring port group 7*/} ,
        {42 +64/* network - port group 2*/ , 51 +64/*ring port group 7*/} ,

        {48 +64/* network - port group 3*/ , 43 +64/*ring port group 6*/} ,
        {50 +64/* network - port group 3*/ , 33 +64/*ring port group 6*/} ,
        {52 +64/* network - port group 3*/ , 35 +64/*ring port group 6*/} ,
        {54 +64/* network - port group 3*/ , 37 +64/*ring port group 6*/} ,
        {56 +64/* network - port group 3*/ , 41 +64/*ring port group 6*/} ,
        {58 +64/* uplink  - port group 3*/ , 39 +64/*ring port group 6*/} ,

        {0,0}
};

static GT_PORT_NUM  networkPortsArr_enhanced[] =
{
 0, 2, 4, 6, 8,10,/* on port group 0*/
16,18,20,22,24,26,/* on port group 1*/
32,34,36,38,40,42,/* on port group 2*/
48,50,52,54,56,58,/* on port group 3*/

 0+64, 2+64, 4+64, 6+64, 8+64,10+64,/* on port group 4*/
16+64,18+64,20+64,22+64,24+64,26+64,/* on port group 5*/
32+64,34+64,36+64,38+64,40+64,42+64,/* on port group 6*/
48+64,50+64,52+64,54+64,56+64,58+64,/* on port group 7*/


0 /* all others don't care */
};

static GT_U32    networkPortsNum_enhanced = sizeof(networkPortsArr_enhanced)/sizeof(networkPortsArr_enhanced[0]);

/******* end of enhanced mode settings ****************************************/

/*******  256K FDB 1 ring port specific configuration       ***************************/
static GT_BOOL FDB256K_1Ring_setup = GT_FALSE;



/*******************************************************************************
* getBoardInfo
*
* DESCRIPTION:
*       Return the board configuration info.
*
* INPUTS:
*       boardRevId  - The board revision Id.
*
* OUTPUTS:
*       isB2bSystem - GT_TRUE, the system is a B2B system.
*       numOfPp     - Number of Packet processors in system.
*       numOfFa     - Number of Fabric Adaptors in system.
*       numOfXbar   - Number of Crossbar devices in system.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS getBoardInfo
(
    IN  GT_U8   boardRevId,
    OUT GT_U8   *numOfPp,
    OUT GT_U8   *numOfFa,
    OUT GT_U8   *numOfXbar,
    OUT GT_BOOL *isB2bSystem
)
{
    GT_STATUS rc;

    boardRevId = boardRevId;

    rc = genericBoardConfigFun.boardGetInfo(genericBoardRevision,numOfPp,numOfFa,
                                            numOfXbar,isB2bSystem);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* no extra/override info needed */

    return GT_OK;
}


/*******************************************************************************
* getPpPhase1Config
*
* DESCRIPTION:
*       Returns the configuration parameters for corePpHwPhase1Init().
*
* INPUTS:
*       boardRevId      - The board revision Id.
*       devIdx          - The Pp Index to get the parameters for.
*
* OUTPUTS:
*       ppPhase1Params  - Phase1 parameters struct.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS getPpPhase1Config
(
    IN  GT_U8                       boardRevId,
    IN  GT_U8                       devIdx,
    OUT CPSS_PP_PHASE1_INIT_PARAMS    *phase1Params
)
{
    GT_STATUS rc;

    boardRevId = boardRevId;

    rc = genericBoardConfigFun.boardGetPpPh1Params(genericBoardRevision,devIdx,phase1Params);
    if(rc != GT_OK)
    {
        return rc;
    }


    /* no extra/override info needed */

    return GT_OK;
}

/*******************************************************************************
* configBoardAfterPhase1
*
* DESCRIPTION:
*       This function performs all needed configurations that should be done
*       after phase1.
*
* INPUTS:
*       boardRevId      - The board revision Id.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS configBoardAfterPhase1
(
    IN  GT_U8   boardRevId
)
{
    GT_STATUS rc;

    boardRevId = boardRevId;

    rc = genericBoardConfigFun.boardAfterPhase1Config(genericBoardRevision);
    if(rc != GT_OK)
    {
        return rc;
    }


    /* no extra/override info needed */

    return GT_OK;
}

/*******************************************************************************
* getPpPhase2Config
*
* DESCRIPTION:
*       Returns the configuration parameters for corePpHwPhase2Init().
*
* INPUTS:
*       boardRevId      - The board revision Id.
*       oldDevNum       - The old Pp device number to get the parameters for.
*
* OUTPUTS:
*       ppPhase2Params  - Phase2 parameters struct.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS getPpPhase2Config
(
    IN  GT_U8                       boardRevId,
    IN  GT_U8                       oldDevNum,
    OUT CPSS_PP_PHASE2_INIT_PARAMS    *phase2Params
)
{
    GT_STATUS rc;

    boardRevId = boardRevId;

    rc = genericBoardConfigFun.boardGetPpPh2Params(genericBoardRevision,oldDevNum,phase2Params);
    if(rc != GT_OK)
    {
        return rc;
    }


    /* no extra/override info needed */

    return GT_OK;
}



/*******************************************************************************
* configBoardAfterPhase2
*
* DESCRIPTION:
*       This function performs all needed configurations that should be done
*       after phase2.
*
* INPUTS:
*       boardRevId      - The board revision Id.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS configBoardAfterPhase2
(
    IN  GT_U8   boardRevId
)
{
    GT_STATUS rc;

    boardRevId = boardRevId;

    rc = genericBoardConfigFun.boardAfterPhase2Config(genericBoardRevision);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* no extra/override info needed */

    return GT_OK;
}


/*******************************************************************************
* getPpLogicalInitParams
*
* DESCRIPTION:
*       Returns the parameters needed for sysPpLogicalInit() function.
*
* INPUTS:
*       boardRevId      - The board revision Id.
*       devNum          - The Pp device number to get the parameters for.
*
* OUTPUTS:
*       ppLogInitParams - Pp logical init parameters struct.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS getPpLogicalInitParams
(
    IN  GT_U8           boardRevId,
    IN  GT_U8           devNum,
    OUT CPSS_PP_CONFIG_INIT_STC    *ppLogInitParams
)
{
    GT_STATUS rc;

    boardRevId = boardRevId;

    rc = genericBoardConfigFun.boardGetPpLogInitParams(genericBoardRevision,devNum,ppLogInitParams);
    if(rc != GT_OK)
    {
        return rc;
    }


    /* no extra/override info needed */

    return GT_OK;
}


/*******************************************************************************
* getTapiLibInitParams
*
* DESCRIPTION:
*       Returns Tapi library initialization parameters.
*
* INPUTS:
*       boardRevId      - The board revision Id.
*       devNum          - The Pp device number to get the parameters for.
*
* OUTPUTS:
*       libInitParams   - Library initialization parameters.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS getTapiLibInitParams
(
    IN  GT_U8                       boardRevId,
    IN  GT_U8                       devNum,
    OUT APP_DEMO_LIB_INIT_PARAMS    *libInitParams
)
{
    GT_STATUS rc;

    boardRevId = boardRevId;

    rc = genericBoardConfigFun.boardGetLibInitParams(genericBoardRevision,devNum,libInitParams);
    if(rc != GT_OK)
    {
        return rc;
    }


    /* no extra/override info needed */

    return GT_OK;
}

/*******************************************************************************
* getMode
*
* DESCRIPTION:
*       get mode : basic/enhanced.
*
* INPUTS:
*       none
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_BAD_PARAM - on bad parameter
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS getMode
(
    IN  GT_U8                       boardRevId
)
{
    GT_STATUS   rc;
    GT_U32  value;
    char  *funcName =  "getMode";

    /* print the mode */
    osPrintf("mode = [%s] \n","enhanced");

    if(boardRevId != REV_ID_CALLER_CHOICE_E)
    {
        fdbMode = CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_LINKED_E;
    }
    else
    {
        if(appDemoDbEntryGet("multiPortGroupConfigSet.fdbMode", &value) != GT_OK)
        {
            osPrintf("missing : multiPortGroupConfigSet.fdbMode \n");
            rc = GT_NOT_INITIALIZED;
            CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
            return rc;
        }

        fdbMode = value;
    }

    /* print the fdbMode */
    osPrintf("fdbMode = [%d] \n",fdbMode);

    if(fdbMode > CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_LINKED_E)
    {
        rc = GT_BAD_PARAM;
        CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* getUplinks
*
* DESCRIPTION:
*       get uplinks.
*
* INPUTS:
*       none
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_BAD_PARAM - on bad parameter
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS getUplinks
(
    IN  GT_U8                       boardRevId
)
{
    GT_STATUS   rc;
    GT_U32  value;
    char   name[256];
    GT_U32  ii;
    char  *funcName =  "getUplinks";

    /*  256K FDB 1 ring port  support code. */
    if(FDB256K_1Ring_setup == GT_TRUE)
    {
      numOfUplinkPorts = 8;

      for(ii = 0 ; ii < numOfUplinkPorts ; ii++)
        uplinkPortsArr[ii] = ii*16;

      return GT_OK;
    }


    if(boardRevId != REV_ID_CALLER_CHOICE_E)
    {
        numOfUplinkPorts = 8;
    }
    else
    {
        if(appDemoDbEntryGet("multiPortGroupConfigSet.numOfUplinkPorts", &value) != GT_OK)
        {
            osPrintf("missing : multiPortGroupConfigSet.numOfUplinkPorts \n");
            rc = GT_NOT_INITIALIZED;
            CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
            return rc;
        }

        numOfUplinkPorts = value;
    }

    /* print the numOfUplinkPorts */
    osPrintf("numOfUplinkPorts = [%d] \n",numOfUplinkPorts);

    if(numOfUplinkPorts == 0 || numOfUplinkPorts > MAX_UPLINKS_CNS)
    {
        rc = GT_BAD_PARAM;
        CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
        return rc;
    }

    for(ii = 0 ; ii < numOfUplinkPorts ; ii++)
    {
        /* get the uplinks */
        if(boardRevId != REV_ID_CALLER_CHOICE_E)
        {
            /* dont modify uplinkPortsArr[] */
        }
        else
        {
            osSprintf(name,"multiPortGroupConfigSet.uplink_%d",ii);

            if(appDemoDbEntryGet(name, &value) != GT_OK)
            {
                osPrintf("missing : multiPortGroupConfigSet.uplink_%d \n",ii);
                rc = GT_NOT_INITIALIZED;
                CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);

                return rc;
            }

            if(value > 0xff)
            {
                rc = GT_BAD_PARAM;
                CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
                return rc;
            }

            uplinkPortsArr[ii] = (GT_PORT_NUM)value;
        }
        /* print the uplink num */
        osPrintf("uplinkPortsArr[%d] = [%d] \n",ii,uplinkPortsArr[ii]);
    }


    return GT_OK;
}

/*******************************************************************************
* getRingInfoNum
*
* DESCRIPTION:
*       get rings info.
*
* INPUTS:
*       none
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_BAD_PARAM - on bad parameter
*       GT_NOT_INITIALIZED - on missing value
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS getRingInfo
(
    IN  GT_U8                       boardRevId
)
{
    GT_STATUS   rc;
    GT_U32  value;
    char   name[256];
    GT_U32  ii,jj;
    char  *funcName =  "getRingInfo";

    /*   support code. 256K FDB */
    if(FDB256K_1Ring_setup == GT_TRUE)
        boardRevId = REV_ID_ENHANCED_AUTO_4_FDB_256K_E;

    if(boardRevId == REV_ID_ENHANCED_AUTO_4_FDB_256K_E)
    {
        ringInfoNum = 8;

        /* copy to the info from the enhanced mode values */
        for(ii = 0 ; ii < MAX_INTERNAL_TRUNKS_CNS; ii++)
        {
            internalTrunksArr[ii] = internalTrunks256kArr_enhanced[ii];
        }
    }
    else if(boardRevId == REV_ID_ENHANCED_AUTO_2_FDB_128K_E)
    {
        ringInfoNum = 8;

        /* copy to the info from the enhanced mode values */
        for(ii = 0 ; ii < MAX_INTERNAL_TRUNKS_CNS; ii++)
        {
            internalTrunksArr[ii] = internalTrunks128kArr_enhanced[ii];
        }
    }
    else
    {
        /* get the number of ring info */
        if(appDemoDbEntryGet("multiPortGroupConfigSet.ringInfoNum", &value) != GT_OK)
        {
            osPrintf("missing : multiPortGroupConfigSet.ringInfoNum \n");
            rc = GT_NOT_INITIALIZED;
            CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
            return rc;
        }

        ringInfoNum = value;
    }

    /* print the ringInfoNum */
    osPrintf("ringInfoNum = [%d] \n",ringInfoNum);

    if(ringInfoNum == 0 || ringInfoNum > CPSS_MAX_PORT_GROUPS_CNS)
    {
        rc = GT_BAD_PARAM;
        CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
        return rc;
    }

    for(ii = 0 ; ii < ringInfoNum ; ii++)
    {
        /* 256K FDB 1 ring port  support code. */
        if(FDB256K_1Ring_setup == GT_TRUE)
            ringInfoArr[ii].srcPortGroupId = ii;/*0..7*/
        else if(boardRevId == REV_ID_ENHANCED_AUTO_4_FDB_256K_E)
        {
            ringInfoArr[ii].srcPortGroupId = ii;/*0..7*/
        }
        else if(boardRevId == REV_ID_ENHANCED_AUTO_2_FDB_128K_E)
        {
            ringInfoArr[ii].srcPortGroupId = ii;/*0..7*/
        }
        else
        {
            /* get the ringInfoArr[ii].srcPortGroupId */
            osSprintf(name,"multiPortGroupConfigSet.ringInfoArr_%d.srcPortGroupId",ii);
            if(appDemoDbEntryGet(name, &value) != GT_OK)
            {
                rc = GT_BAD_PARAM;
                CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
                return rc;
            }

            ringInfoArr[ii].srcPortGroupId = value;
        }
        /* print ringInfoArr[ii].srcPortGroupId */
        osPrintf("ringInfoArr[%d].srcPortGroupId = [%d] \n",ii,ringInfoArr[ii].srcPortGroupId);


        if(FDB256K_1Ring_setup == GT_TRUE)
            ringInfoArr[ii].numOfRingPorts = 3;/* 1 ring port on each of other 3 port groups */
        else if(boardRevId == REV_ID_ENHANCED_AUTO_4_FDB_256K_E)
        {
            ringInfoArr[ii].numOfRingPorts = 6;/* 2 rings on each of other 3 port groups --> per chain --> for 8 chains */
        }
        else if(boardRevId == REV_ID_ENHANCED_AUTO_2_FDB_128K_E)
        {
            ringInfoArr[ii].numOfRingPorts = 6;/* 6 rings on other port group --> per chain --> for 8 chains */
        }
        else
        {
            /* get the ringInfoArr[ii].numOfRingPorts */
            osSprintf(name,"multiPortGroupConfigSet.ringInfoArr_%d.numOfRingPorts",ii);
            if(appDemoDbEntryGet(name, &value) != GT_OK)
            {
                osPrintf("missing : multiPortGroupConfigSet.ringInfoArr_%d.numOfRingPorts \n",ii);
                rc = GT_NOT_INITIALIZED;
                CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
                return rc;
            }


            ringInfoArr[ii].numOfRingPorts = value;
        }

        /* print ringInfoArr[ii].numOfRingPorts */
        osPrintf("ringInfoArr[%d].numOfRingPorts = [%d] \n",ii,ringInfoArr[ii].numOfRingPorts);

        /* bind the pointer to static array we pre-allocated for this */
        if(FDB256K_1Ring_setup == GT_TRUE)
            ringInfoArr[ii].ringPortsPtr = &interconnectionPorts256kArr_enhanced[interconnectionIndex];
        else if(boardRevId == REV_ID_ENHANCED_AUTO_4_FDB_256K_E)
        {
            ringInfoArr[ii].ringPortsPtr = &interconnectionPorts256kArr_enhanced[interconnectionIndex];
        }
        else if(boardRevId == REV_ID_ENHANCED_AUTO_2_FDB_128K_E)
        {
            ringInfoArr[ii].ringPortsPtr = &interconnectionPorts128kArr_enhanced[interconnectionIndex];
        }
        else
        {
            ringInfoArr[ii].ringPortsPtr = &interconnectionPorts256kArr_enhanced[interconnectionIndex];
        }

        interconnectionIndex += ringInfoArr[ii].numOfRingPorts;

        if(interconnectionIndex == 0 ||
           interconnectionIndex > MAX_NUM_OF_INTERCONNECTION_PORTS_CNS)
        {
            rc = GT_BAD_PARAM;
            CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
            return rc;
        }

        /* get the ring ports */
        for(jj = 0 ; jj < ringInfoArr[ii].numOfRingPorts ; jj++)
        {
            if(boardRevId != REV_ID_CALLER_CHOICE_E)
            {
                /* don't change ringInfoArr[ii].ringPortsPtr[jj] */
            }
            else
            {
                /* get the ringInfoArr[ii].ringPort[jj] */
                osSprintf(name,"multiPortGroupConfigSet.ringInfoArr_%d.ringPort_%d",ii,jj);
                if(appDemoDbEntryGet(name, &value) != GT_OK)
                {
                    osPrintf("missing : multiPortGroupConfigSet.ringInfoArr_%d.ringPort_%d \n",ii,jj);
                    rc = GT_NOT_INITIALIZED;
                    CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
                    return rc;
                }

                if(value > 0xff)
                {
                    rc = GT_BAD_PARAM;
                    CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
                    return rc;
                }

                ringInfoArr[ii].ringPortsPtr[jj] = (GT_PHYSICAL_PORT_NUM)value;
            }

            /* print ringInfoArr[ii].ringPortsPtr[jj] */
            osPrintf("ringInfoArr[%d].ringPortsPtr[%d] = [%d] \n",ii,jj,ringInfoArr[ii].ringPortsPtr[jj]);

        }/*jj*/


        if(FDB256K_1Ring_setup == GT_TRUE)
            ringInfoArr[ii].trunkIdNum = 0;/* trunk on other 3 port groups */
        else if(boardRevId == REV_ID_ENHANCED_AUTO_4_FDB_256K_E)
        {
            ringInfoArr[ii].trunkIdNum = 3;/* trunk on other 3 port groups */
        }
        else if(boardRevId == REV_ID_ENHANCED_AUTO_2_FDB_128K_E)
        {
            ringInfoArr[ii].trunkIdNum = 1;/* trunk on other port group */
        }
        else
        {
            /* get the ringInfoArr[ii].trunkIdNum */
            osSprintf(name,"multiPortGroupConfigSet.ringInfoArr_%d.trunkIdNum",ii);
            if(appDemoDbEntryGet(name, &value) != GT_OK)
            {
                osPrintf("missing : multiPortGroupConfigSet.ringInfoArr_%d.trunkIdNum \n",ii);
                rc = GT_NOT_INITIALIZED;
                CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
                return rc;
            }

            ringInfoArr[ii].trunkIdNum = value;
        }

        /* print ringInfoArr[ii].trunkIdNum */
        osPrintf("ringInfoArr[%d].trunkIdNum = [%d] \n",ii,ringInfoArr[ii].trunkIdNum);

        /* bind the pointer to static array we pre-allocated for this */
        ringInfoArr[ii].trunkIdPtr = &internalTrunksArr[internalTrunksIndex];
        internalTrunksIndex += ringInfoArr[ii].trunkIdNum;

        if(internalTrunksIndex > MAX_INTERNAL_TRUNKS_CNS)
        {
            rc = GT_BAD_PARAM;
            CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
            return rc;
        }

        /* get the trunkIds for the ring ports */
        for(jj = 0 ; jj < ringInfoArr[ii].trunkIdNum ; jj++)
        {

            if(boardRevId != REV_ID_CALLER_CHOICE_E)
            {
                /* don't change ringInfoArr[ii].trunkIdPtr[jj] */
            }
            else
            {
                /* get the ringInfoArr[ii].ringPort[jj] */
                osSprintf(name,"multiPortGroupConfigSet.ringInfoArr_%d.trunkId_%d",ii,jj);
                if(appDemoDbEntryGet(name, &value) != GT_OK)
                {
                    osPrintf("missing : multiPortGroupConfigSet.ringInfoArr_%d.trunkId_%d \n",ii,jj);
                    rc = GT_NOT_INITIALIZED;
                    CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
                    return rc;
                }

                if(value > (0xffff))
                {
                    rc = GT_BAD_PARAM;
                    CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
                    return rc;
                }

                ringInfoArr[ii].trunkIdPtr[jj] = (GT_TRUNK_ID)value;
            }

            /* print ringInfoArr[ii].trunkIdPtr[jj] */
            osPrintf("ringInfoArr[%d].trunkIdPtr[%d] = [%d] \n",ii,jj,ringInfoArr[ii].trunkIdPtr[jj]);
        }/*jj*/
    }/*ii*/

    return GT_OK;
}

/*******************************************************************************
* getTrunkSrcPortHashMapping
*
* DESCRIPTION:
*       get trunks 'source port' hash mapping.
*
* INPUTS:
*       none
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_BAD_PARAM - on bad parameter
*       GT_NOT_INITIALIZED - on missing value
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS getTrunkSrcPortHashMapping
(
    IN  GT_U8                       boardRevId
)
{
    GT_STATUS   rc;
    GT_U32  value;
    char   name[256];
    GT_U32  ii,jj;
    char  *funcName =  "getTrunkSrcPortHashMapping";

    if(FDB256K_1Ring_setup == GT_TRUE)
        trunkSrcPortHashInfoNum = 0;
    else if(boardRevId == REV_ID_ENHANCED_AUTO_4_FDB_256K_E)
    {
        /* trunk per chain --> all ports that connect to next port group
           are in the same trunk --> no need for 3 trunks per port group
           */
        trunkSrcPortHashInfoNum = 8;
    }
    else if(boardRevId == REV_ID_ENHANCED_AUTO_2_FDB_128K_E)
    {
        /* 1 trunk - one per chain */
        trunkSrcPortHashInfoNum = 8;
    }
    else
    {
        /* get the number trunks */
        if(appDemoDbEntryGet("trunkSrcPortHashMappingSet.numOfTrunks", &value) != GT_OK)
        {
            /* optional setting --> not necessarily needed */
            value = 0;
        }
        trunkSrcPortHashInfoNum = value;
    }

    /* print the trunkSrcPortHashInfoNum */
    osPrintf("trunkSrcPortHashInfoNum = [%d] \n",trunkSrcPortHashInfoNum);

    if(trunkSrcPortHashInfoNum > MAX_INTERNAL_TRUNKS_CNS)
    {
        rc = GT_BAD_PARAM;
        CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
        return rc;
    }

    for(ii = 0 ; ii < trunkSrcPortHashInfoNum ; ii++)
    {
        if(boardRevId == REV_ID_ENHANCED_AUTO_4_FDB_256K_E)
        {
            trunkSrcPortHashInfo[ii].trunkId = nextTrunkArr_enhanced[ii];
        }
        else if(boardRevId != REV_ID_CALLER_CHOICE_E)
        {
            trunkSrcPortHashInfo[ii].trunkId = internalTrunksArr[ii];
        }
        else
        {
            /* get the trunkSrcPortHashInfo[ii].trunkId_%d */
            osSprintf(name,"trunkSrcPortHashInfo_%d.trunkId",ii);
            if(appDemoDbEntryGet(name, &value) != GT_OK)
            {
                rc = GT_BAD_PARAM;
                CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
                return rc;
            }

            if(value > (0xffff))
            {
                rc = GT_BAD_PARAM;
                CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
                return rc;
            }

            trunkSrcPortHashInfo[ii].trunkId = (GT_TRUNK_ID)value;
        }
        /* print trunkSrcPortHashInfo[ii].trunkId */
        osPrintf("trunkSrcPortHashInfo[%d].trunkId = [%d] \n",ii,trunkSrcPortHashInfo[ii].trunkId);

        if(boardRevId == REV_ID_ENHANCED_AUTO_4_FDB_256K_E)
        {
            trunkSrcPortHashInfo[ii].numOfPorts = 10;/* 10 ports in the port group the jump to next port group */
                                                     /* 6 network(include uplink) and 4 ring ports */
        }
        else if(boardRevId == REV_ID_ENHANCED_AUTO_2_FDB_128K_E)
        {
            trunkSrcPortHashInfo[ii].numOfPorts = 6;/* 6 ports in the port group the jump to next port group */
        }
        else
        {
            /* get the trunkSrcPortHashInfo[ii].numOfPorts_%d */
            osSprintf(name,"trunkSrcPortHashInfo_%d.numOfPorts",ii);
            if(appDemoDbEntryGet(name, &value) != GT_OK)
            {
                rc = GT_BAD_PARAM;
                CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
                return rc;
            }

            trunkSrcPortHashInfo[ii].numOfPorts = value;
        }

        /* print trunkSrcPortHashInfo[ii].numOfPorts */
        osPrintf("trunkSrcPortHashInfo[%d].numOfPorts = [%d] \n",ii,trunkSrcPortHashInfo[ii].numOfPorts);

        if(boardRevId == REV_ID_ENHANCED_AUTO_4_FDB_256K_E)
        {
            trunkSrcPortHashInfo[ii].portsArr = &pairsArrFor256k_enhanced[pairsIndex];
        }
        else if(boardRevId == REV_ID_ENHANCED_AUTO_2_FDB_128K_E)
        {
            trunkSrcPortHashInfo[ii].portsArr = &pairsArrFor128k_enhanced[pairsIndex];
        }
        else
        {
            trunkSrcPortHashInfo[ii].portsArr = &pairsArrFor256k_enhanced[pairsIndex];
        }

        pairsIndex += trunkSrcPortHashInfo[ii].numOfPorts;
        if(pairsIndex > MAX_NUM_OF_INTERCONNECTION_PORTS_CNS )
        {
            rc = GT_BAD_PARAM;
            CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
            return rc;
        }

        if(boardRevId != REV_ID_CALLER_CHOICE_E)
        {
            trunkSrcPortHashInfo[ii].mode = CPSS_DXCH_MULTI_PORT_GROUP_SRC_HASH_MODE_MODULO_64_E;
        }
        else
        {
            /* get the trunkSrcPortHashInfo[ii].mode */
            osSprintf(name,"trunkSrcPortHashInfo_%d.mode",ii);
            if(appDemoDbEntryGet(name, &value) != GT_OK)
            {
                rc = GT_BAD_PARAM;
                CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
                return rc;
            }

            trunkSrcPortHashInfo[ii].mode = value;
        }

        /* print trunkSrcPortHashInfo[ii].trunkId */
        osPrintf("trunkSrcPortHashInfo[%d].mode = [%d] \n",ii,trunkSrcPortHashInfo[ii].mode);

        /* get the trunkIds for the ring ports */
        for(jj = 0 ; jj < trunkSrcPortHashInfo[ii].numOfPorts ; jj++)
        {
            if(boardRevId != REV_ID_CALLER_CHOICE_E)
            {
                /* don't change trunkSrcPortHashInfo[ii].portsArr[jj].srcPort */
            }
            else
            {
                /* get the trunkSrcPortHashInfo[ii].portsArr[jj].srcPort */
                osSprintf(name,"trunkSrcPortHashInfo_%d.portsArr_%d.srcPort",ii,jj);
                if(appDemoDbEntryGet(name, &value) != GT_OK)
                {
                    osPrintf("missing : trunkSrcPortHashInfo_%d.portsArr_%d.srcPort \n",ii,jj);
                    rc = GT_NOT_INITIALIZED;
                    CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
                    return rc;
                }
                if(value > 0xff)
                {
                    rc = GT_BAD_PARAM;
                    CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
                    return rc;
                }

                trunkSrcPortHashInfo[ii].portsArr[jj].srcPort = (GT_PHYSICAL_PORT_NUM)value;
            }

            /* print trunkSrcPortHashInfo[ii].portsArr[jj].srcPort */
            osPrintf("trunkSrcPortHashInfo[%d].portsArr[%d].srcPort = [%d] \n",ii,jj,
                      trunkSrcPortHashInfo[ii].portsArr[jj].srcPort);



            if(boardRevId != REV_ID_CALLER_CHOICE_E)
            {
                /* don't change trunkSrcPortHashInfo[ii].portsArr[jj].trunkPort */
            }
            else
            {
                /* get the trunkSrcPortHashInfo[ii].portsArr[jj].trunkPort */
                osSprintf(name,"trunkSrcPortHashInfo_%d.portsArr_%d.trunkPort",ii,jj);
                if(appDemoDbEntryGet(name, &value) != GT_OK)
                {
                    osPrintf("missing : trunkSrcPortHashInfo_%d.portsArr_%d.trunkPort \n",ii,jj);
                    rc = GT_NOT_INITIALIZED;
                    CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
                    return rc;
                }

                if(value > 0xff)
                {
                    rc = GT_BAD_PARAM;
                    CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
                    return rc;
                }

                trunkSrcPortHashInfo[ii].portsArr[jj].trunkPort = (GT_PHYSICAL_PORT_NUM)value;
            }

            /* print trunkSrcPortHashInfo[ii].portsArr[jj].trunkPort */
            osPrintf("trunkSrcPortHashInfo[%d].portsArr[%d].trunkPort = [%d] \n",ii,jj,
                      trunkSrcPortHashInfo[ii].portsArr[jj].trunkPort);
        }/*jj*/
    }/* ii*/


    return GT_OK;
}

/*******************************************************************************
* appDemoLion2TrunkSrcPortHashMappingSet_ForMultiCoreFdbLookup
*
* DESCRIPTION:
*       set the trunk srcPort hash mode settings - Lion2.
*
* INPUTS:
*       boardRevId      - The board revision Id.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoLion2TrunkSrcPortHashMappingSet_ForMultiCoreFdbLookup
(
    IN  GT_U8                       boardRevId
)
{
    GT_STATUS rc;
    GT_U8   devNum = SYSTEM_DEV_NUM_MAC(0);
    GT_U32  ii;
    char  *funcName =  "appDemoLion2TrunkSrcPortHashMappingSet_ForMultiCoreFdbLookup";

    boardRevId = boardRevId;

    /* loop on all internal trunks that need to be set with 'source port' hash */
    for(ii = 0 ; ii < trunkSrcPortHashInfoNum ; ii++)
    {
        rc = cpssDxChMultiPortGroupTrunkSrcPortHashMappingSet(devNum,
            trunkSrcPortHashInfo[ii].trunkId,
            trunkSrcPortHashInfo[ii].numOfPorts,
            trunkSrcPortHashInfo[ii].portsArr,
            trunkSrcPortHashInfo[ii].mode);
        if(rc != GT_OK)
        {
            CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
            return rc;
        }
    }

    if(trunkSrcPortHashInfoNum)
    {
        osPrintf("*** cpssDxChMultiPortGroupTrunkSrcPortHashMappingSet() -- done *** \n");
    }

    return GT_OK;
}

/*******************************************************************************
* afterInitBoardConfig
*
* DESCRIPTION:
*       CPSS Board specific configurations that should be done after board
*       initialization.
*
* INPUTS:
*       boardRevId      - The board revision Id.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS gtDbDxLion2_256kFdb1Ring_MultiCoreFdbPortSetup(GT_U8 devNum);
static GT_STATUS afterInitBoardConfig
(
    IN  GT_U8                       boardRevId
)
{
    GT_STATUS rc;
    GT_U8   devNum = SYSTEM_DEV_NUM_MAC(0);
    CPSS_PORTS_BMP_STC  portsMembers;
    CPSS_PORTS_BMP_STC  portsTagging;
    CPSS_DXCH_BRG_VLAN_INFO_STC  cpssVlanInfo;   /* cpss vlan info format    */
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC portsTaggingCmd; /* ports tagging command */
    GT_U16   vid = 1;/* default VLAN id          */
    GT_BOOL  isValid;/* is valid vlan */
    GT_U32  ii,jj;
    char  *funcName =  "afterInitBoardConfig";
    GT_BOOL enoughParamaters = GT_TRUE;/* did we get enough parameters to initialize infrastructure */
                                        /* when enoughParamaters = GT_FALSE : we allow UT test or RDE test to configure exact settings after cpssInitSystem */

    rc = genericBoardConfigFun.boardAfterInitConfig(genericBoardRevision);
    if(rc != GT_OK)
    {
        return rc;
    }

    osPrintf("*** start multi-port-group infrastructure settings *** \n");

    /* get mode */
    rc = getMode(boardRevId);
    if(rc == GT_NOT_INITIALIZED)
    {
        enoughParamaters = GT_FALSE;
    }
    else if(rc != GT_OK)
    {
        return rc;
    }


    osPrintf("*** getMode() -- done *** \n");

    /* get the uplinks */
    rc = getUplinks(boardRevId);
    if(rc == GT_NOT_INITIALIZED)
    {
        enoughParamaters = GT_FALSE;
    }
    else if(rc != GT_OK)
    {
        return rc;
    }

    osPrintf("*** getUplinks() -- done *** \n");

    /* get the rings info */
    rc = getRingInfo(boardRevId);
    if(rc == GT_NOT_INITIALIZED)
    {
        enoughParamaters = GT_FALSE;
    }
    else if(rc != GT_OK)
    {
        return rc;
    }

    osPrintf("*** getRingInfo() -- done *** \n");

    if(enoughParamaters == GT_TRUE)
    {
        /* set the multi-port-groups configuration */
        rc = cpssDxChMultiPortGroupConfigSet(devNum,
                        numOfUplinkPorts,uplinkPortsArr,
                        ringInfoNum,ringInfoArr);

        if(rc != GT_OK)
        {
            CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
            return rc;
        }

        osPrintf("*** cpssDxChMultiPortGroupConfigSet() -- done *** \n");
    }

    /* get the source port hash info for internal trunks */
    rc = getTrunkSrcPortHashMapping(boardRevId);
    if(rc == GT_NOT_INITIALIZED)
    {
        trunkSrcPortHashInfoNum = 0;
    }
    else if(rc != GT_OK)
    {
        return rc;
    }

    osPrintf("*** getTrunkSrcPortHashMapping() -- done *** \n");

    rc = appDemoLion2TrunkSrcPortHashMappingSet_ForMultiCoreFdbLookup(boardRevId);
    if(rc != GT_OK)
    {
        CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
        return rc;
    }

    rc = cpssDxChMultiPortGroupFdbModeSet(devNum,fdbMode);
    if(rc != GT_OK)
    {
        CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
        return rc;
    }

    osPrintf("*** cpssDxChMultiPortGroupFdbModeSet() -- done *** \n");

    /*save info to the AppDemo , so ' controlled learning' can be executed correctly */
    switch(fdbMode)
    {
        case CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_E:
            appDemoPpConfigList[devNum].fdbMode = APP_DEMO_CPSS_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_E;
            for(ii = 0 ; ii < appDemoPpConfigList[devNum].numOfPortGroups ; ii ++)
            {
                appDemoPpConfigList[devNum].portGroupsInfo[ii].unifiedFdbPortGroupsBmp =
                    CPSS_PORT_GROUP_UNAWARE_MODE_CNS;
            }
            break;
        case CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_LINKED_E:
            appDemoPpConfigList[devNum].fdbMode = APP_DEMO_CPSS_MULTI_PORT_GROUP_FDB_MODE_UNIFIED_LINKED_E;
            for(ii = 0 ; ii < appDemoPpConfigList[devNum].numOfPortGroups ; ii ++)
            {
                if(boardRevId == REV_ID_ENHANCED_AUTO_4_FDB_256K_E)
                {
                    /* port groups 0,4 will hold same FDB .
                       port groups 1,5 will hold same FDB .
                       port groups 2,6 will hold same FDB .
                       port groups 3,7 will hold same FDB .
                    */
                    switch(ii & 0x3)
                    {
                        case 0:
                            appDemoPpConfigList[devNum].portGroupsInfo[ii].unifiedFdbPortGroupsBmp =
                                BIT_0 | BIT_4;
                            break;
                        case 1:
                            appDemoPpConfigList[devNum].portGroupsInfo[ii].unifiedFdbPortGroupsBmp =
                                BIT_1 | BIT_5;
                            break;
                        case 2:
                            appDemoPpConfigList[devNum].portGroupsInfo[ii].unifiedFdbPortGroupsBmp =
                                BIT_2 | BIT_6;
                            break;
                        default:/*3*/
                            appDemoPpConfigList[devNum].portGroupsInfo[ii].unifiedFdbPortGroupsBmp =
                                BIT_3 | BIT_7;
                            break;
                    }
                }
                else
                {
                    /* port groups 0,2,4,6 will hold same FDB .
                       port groups 1,3,5,7 will hold same FDB .
                    */
                    appDemoPpConfigList[devNum].portGroupsInfo[ii].unifiedFdbPortGroupsBmp =
                        (ii & 1) ? (BIT_1 | BIT_3 | BIT_5 | BIT_7) :
                                   (BIT_0 | BIT_2 | BIT_4 | BIT_6) ;
                }
            }
            break;
        case CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_LINKED_E:
            appDemoPpConfigList[devNum].fdbMode = APP_DEMO_CPSS_MULTI_PORT_GROUP_FDB_MODE_LINKED_E;
            for(ii = 0 ; ii < appDemoPpConfigList[devNum].numOfPortGroups ; ii ++)
            {
                appDemoPpConfigList[devNum].portGroupsInfo[ii].unifiedFdbPortGroupsBmp =
                    (1 << ii);
            }
            break;
        default:
            break;
    }

    if(enoughParamaters == GT_FALSE)
    {
        osPrintf("*** infrastructure settings bypassed !!! *** \n");

        /* no need to do more */
        return GT_OK;
    }

    osPrintf("*** ended multi-port-group infrastructure settings *** \n");

    /* 'refresh' the vlan entry , that was set before we started the multi-port-groups support */
    /* update the default vlan , add interconnection ports as 'tagged' to vlan */

    /* Read default VLAN entry */
    rc = cpssDxChBrgVlanEntryRead(devNum, vid,
                                   &portsMembers,
                                   &portsTagging,
                                   &cpssVlanInfo,
                                   &isValid,
                                   &portsTaggingCmd);
    if(rc != GT_OK)
    {
        CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
        return rc;
    }

    if(isValid == GT_TRUE)
    {
        /* add all interconnection ports as tagged to vlan */
        for(ii = 0 ; ii < ringInfoNum ; ii++)
        {
            for(jj = 0 ; jj < ringInfoArr[ii].numOfRingPorts ; jj++)
            {
                /*members*/
                CPSS_PORTS_BMP_PORT_SET_MAC((&portsMembers),ringInfoArr[ii].ringPortsPtr[jj]);
                /*tagged*/
                CPSS_PORTS_BMP_PORT_SET_MAC((&portsTagging),ringInfoArr[ii].ringPortsPtr[jj]);

                /* egress with tag 0 */
                portsTaggingCmd.portsCmd[ringInfoArr[ii].ringPortsPtr[jj]] = CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E;
            }
        }

        /* 'refresh' default VLAN entry */
        rc = cpssDxChBrgVlanEntryWrite(devNum, vid,
                                       &portsMembers,
                                       &portsTagging,
                                       &cpssVlanInfo,
                                       &portsTaggingCmd);
        if(rc != GT_OK)
        {
            CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
            return rc;
        }
    }

    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&appDemoPpConfigList[devNum].multiPortGroupRingPortsBmp);

    /* set interconnection ports with shorter preamble because those are always
       'tagged' in vlan */
    for(ii = 0 ; ii < ringInfoNum ; ii++)
    {
        for(jj = 0 ; jj < ringInfoArr[ii].numOfRingPorts ; jj++)
        {
            /* save to DB the ring ports */
            CPSS_PORTS_BMP_PORT_SET_MAC((&appDemoPpConfigList[devNum].multiPortGroupRingPortsBmp),
                ringInfoArr[ii].ringPortsPtr[jj]);

            /* enable TTI lookup for MPLS key */
            rc = cpssDxChTtiPortLookupEnableSet(devNum,
                                                ringInfoArr[ii].ringPortsPtr[jj],
                                                CPSS_DXCH_TTI_KEY_MPLS_E,
                                                GT_TRUE);
            if(rc != GT_OK)
            {
                CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
                return rc;
            }
        }
    }

    /* set also the network ports as 'Eligible for multi-lookup' */
    for(ii = 0 ; ii < networkPortsNum_enhanced ; ii++)
    {
        for(jj = 0 ; jj < numOfUplinkPorts; jj++)
        {
            if(networkPortsArr_enhanced[ii] == uplinkPortsArr[jj])
            {
                break;
            }
        }

        if(jj != numOfUplinkPorts)
        {
            /* this is uplink --> skip it (already set by
                cpssDxChMultiPortGroupConfigSet)*/
            continue;
        }

        /* enable for multi-port groups lookup */
        rc = cpssDxChMultiPortGroupPortLookupEnableSet(devNum,networkPortsArr_enhanced[ii],GT_TRUE);
        if(rc != GT_OK)
        {
            CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
            return rc;
        }
    }

    /* enable for multi-port groups lookup -- also for CPU port */
    rc = cpssDxChMultiPortGroupPortLookupEnableSet(devNum,CPSS_CPU_PORT_NUM_CNS,GT_TRUE);
    if(rc != GT_OK)
    {
        CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
        return rc;
    }

    rc = gtDbDxLion2_256kFdb1Ring_MultiCoreFdbPortSetup(devNum);
    if(rc != GT_OK)
    {
        CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
        return rc;
    }

    rc = appDemoLionMultiCoreTtiLookupSupport(devNum);
    if(rc != GT_OK)
    {
        CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
        return rc;
    }

    return GT_OK;
}


/*******************************************************************************
* gtDbDxLion2_ForMultiCoreFdbLookup
*
* DESCRIPTION:
*       Registration function for the Lion 96XG ports board.
*       ForMultiCoreFdbLookup specifics
*
* INPUTS:
*       boardRevId      - The board revision Id.
*
* OUTPUTS:
*       boardCfgFuncs   - Board configuration functions.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS gtDbDxLion2_ForMultiCoreFdbLookup
(
    IN  GT_U8                   boardRevId,
    OUT GT_BOARD_CONFIG_FUNCS   *boardCfgFuncs
)
{
    GT_STATUS   rc;

    if(boardCfgFuncs == NULL)
    {
        return GT_FAIL;
    }

    switch(boardRevId)
    {
        case REV_ID_CALLER_CHOICE_E:                /*22*/
        case REV_ID_ENHANCED_AUTO_4_FDB_256K_E:     /*24*/
        case REV_ID_ENHANCED_AUTO_2_FDB_128K_E:     /*25*/
            break;
        default:
            /* this board revision not supported */
            return GT_NOT_SUPPORTED;
    }

    /* let the generic board configuration , give us the functions of it's
       implementation */
    /* we will use those pointers as "set default" values that we can override */
    rc = gtDbDxLion296XgBoardReg(genericBoardRevision,&genericBoardConfigFun);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* set the actual pointer to use for this board */
    boardCfgFuncs->boardGetInfo                 = getBoardInfo;
    boardCfgFuncs->boardGetPpPh1Params          = getPpPhase1Config;
    boardCfgFuncs->boardAfterPhase1Config       = configBoardAfterPhase1;
    boardCfgFuncs->boardGetPpPh2Params          = getPpPhase2Config;
    boardCfgFuncs->boardAfterPhase2Config       = configBoardAfterPhase2;
    boardCfgFuncs->boardGetPpLogInitParams      = getPpLogicalInitParams;
    boardCfgFuncs->boardGetLibInitParams        = getTapiLibInitParams;
    boardCfgFuncs->boardAfterInitConfig         = afterInitBoardConfig;

    return GT_OK;
}


static GT_PHYSICAL_PORT_NUM  ringPort_256K_FDB_1ring[] = {
5 ,6 ,7 , /* port group 0  */
17,18,19, /* port group 1  */
33,34,35, /* port group 2  */
53,54,55, /* port group 3  */
65,66,67, /* port group 4  */
81,82,83, /* port group 5  */
101,102,103, /* port group 6  */
117,118,119/* port group 7  */
};


GT_STATUS gtDbDxLion2_256kFdb_1Ring_MultiCoreFdbLookup(void)
{
    GT_U32 ii;

  GT_PHYSICAL_PORT_NUM  *interConnect = interconnectionPorts256kArr_enhanced;
 /*  GT_PHYSICAL_PORT_NUM   *interConnect = interconnectionPorts256kArr_enhanced;*/

  /* In 256K FDB 1 ring port setup
    Uplink: ports 0(10G), 16(10G),     32(40G),   48(40G),   64(10G),   80(10G),   96(40G),   112(40G)
    Ring ports     1,2,3     17, 18, 19   33,34,35   49,50,51   65,66,67   81,82,83    97,98,99   113,114,115

  */

  /*array for the interconnection ports (ring ports) - 256k mode*/
  /*
           PG0   PG1   PG2   PG3
   Port0    __     __     __   __
                   \       \       \     \
   Port1   \__  \__    \__   \__
                   \     \        \      \
   Port2   \__  \__  \__    \ __

  */
  osMemSet(interconnectionPorts256kArr_enhanced,0xFF,sizeof(interconnectionPorts256kArr_enhanced));
  ii = 0;
  /* starting from port group 0  */
  interConnect[ii++] = ringPort_256K_FDB_1ring[1*3 + 0]; /* Port group 1*/
  interConnect[ii++] = ringPort_256K_FDB_1ring[2*3 + 1]; /* Port group 2*/
  interConnect[ii++] = ringPort_256K_FDB_1ring[3*3 + 2]; /* Port group 3*/

  /* starting from port group 1  */
  interConnect[ii++] = ringPort_256K_FDB_1ring[2*3 + 0]; /* Port group 2*/
  interConnect[ii++] = ringPort_256K_FDB_1ring[3*3 + 1]; /* Port group 3*/
  interConnect[ii++] = ringPort_256K_FDB_1ring[0*3 + 2]; /* Port group 0*/

  /* starting from port group 2 */
  interConnect[ii++] = ringPort_256K_FDB_1ring[3*3 + 0]; /* Port group 3*/
  interConnect[ii++] = ringPort_256K_FDB_1ring[0*3 + 1]; /* Port group 0*/
  interConnect[ii++] = ringPort_256K_FDB_1ring[1*3 + 2]; /* Port group 1*/

  /* starting from port group 3  */
  interConnect[ii++] = ringPort_256K_FDB_1ring[0*3 + 0]; /* Port group 0*/
  interConnect[ii++] = ringPort_256K_FDB_1ring[1*3 + 1]; /* Port group 1*/
  interConnect[ii++] = ringPort_256K_FDB_1ring[2*3 + 2]; /* Port group 2*/

  /*
             PG4   PG5   PG6   PG7
   Port0    __     __     __   __
                   \       \       \     \
   Port1   \__  \__    \__    \__
                   \     \       \        \
   Port2   \__  \__  \__    \__

  */
  /* starting from port group 4  */
  interConnect[ii++] = ringPort_256K_FDB_1ring[5*3 + 0]; /* Port group 5*/
  interConnect[ii++] = ringPort_256K_FDB_1ring[6*3 + 1]; /* Port group 6*/
  interConnect[ii++] = ringPort_256K_FDB_1ring[7*3 + 2]; /* Port group 7*/

  /* starting from port group 5  */
  interConnect[ii++] = ringPort_256K_FDB_1ring[6*3 + 0]; /* Port group 6*/
  interConnect[ii++] = ringPort_256K_FDB_1ring[7*3 + 1]; /* Port group 7*/
  interConnect[ii++] = ringPort_256K_FDB_1ring[4*3 + 2]; /* Port group 4*/

  /* starting from port group 6  */
  interConnect[ii++] = ringPort_256K_FDB_1ring[7*3 + 0]; /* Port group 7*/
  interConnect[ii++] = ringPort_256K_FDB_1ring[4*3 + 1]; /* Port group 4*/
  interConnect[ii++] = ringPort_256K_FDB_1ring[5*3 + 2]; /* Port group 5*/

  /* starting from port group 7  */
  interConnect[ii++] = ringPort_256K_FDB_1ring[4*3 + 0]; /* Port group 4*/
  interConnect[ii++] = ringPort_256K_FDB_1ring[5*3 + 1]; /* Port group 5*/
  interConnect[ii++] = ringPort_256K_FDB_1ring[6*3 + 2]; /* Port group 6*/


  networkPortsNum_enhanced = 0;

  FDB256K_1Ring_setup = GT_TRUE;

  return GT_OK;
}


#include <mvHwsPortInitIf.h>

static GT_STATUS gtDbDxLion2_256kFdb1Ring_MultiCoreFdbPortSetup(GT_U8 devNum)
{
  GT_U32 ii;
  GT_STATUS rc;
  GT_U32                   portGroup;         /* port group number for multi-port-group devs */
  MV_HWS_PORT_STANDARD     portMode;          /* port interface mode */
  MV_HWS_REF_CLOCK_SUP_VAL refClock=MHz_156;  /* serdes reference clock */
  GT_PHYSICAL_PORT_NUM     localPort;         /* number of port in port group */
  CPSS_PORTS_BMP_STC       portsBmp;
  GT_PHYSICAL_PORT_NUM     ringPort;
  char  *funcName =  "gtDbDxLion2_256kFdb1Ring_MultiCoreFdbPortSetup";

  if(FDB256K_1Ring_setup == GT_FALSE)
    return GT_OK;

  /* Configure of all ring ports to local KR 10G. */

  /* Set ring ports to 10G, KR */
/*  osMemSet(&portsBmp, 0, sizeof(portsBmp));
  for(ii=0 ; ii<24 ; ii++)
    portsBmp.ports[interconnectionPorts256kArr_enhanced[ii]>>5] |= 1 << (interconnectionPorts256kArr_enhanced[ii]%32);

  rc = cpssDxChPortModeSpeedSet(devNum, portsBmp, GT_TRUE, CPSS_PORT_INTERFACE_MODE_SR_LR_E, CPSS_PORT_SPEED_10000_E);
  if(rc != GT_OK)
    return rc;

  return GT_OK;
*/

    /* Set rest of 8 ports to 10G, XGMII (to use all 4 seredeses)
          this ports is one port before each group of 3 ports in array ringPort_256K_FDB_1ring[] */
  osMemSet(&portsBmp, 0, sizeof(portsBmp));
  for(ii=0 ; ii<8 ; ii++)
   portsBmp.ports[(ringPort_256K_FDB_1ring[ii*3]-1)>>5] |= 1 << ((ringPort_256K_FDB_1ring[ii*3]-1)%32);
  rc = cpssDxChPortModeSpeedSet(devNum, portsBmp, GT_TRUE, CPSS_PORT_INTERFACE_MODE_XGMII_E, CPSS_PORT_SPEED_10000_E);
  if(rc != GT_OK)
   return rc;

  /* Now use hw services API to configure ring ports not to serdeses and loopback */
  portMode = _10GBase_SR_LR;
  for(ii=0 ; ii<24 ; ii++)
  {
    ringPort = interconnectionPorts256kArr_enhanced[ii];
    localPort = ringPort % 16;
    portGroup = ringPort / 16;
/*
    rc = mvHwsPortReset(devNum, portGroup, localPort, portMode,PORT_RESET);
    if(rc != GT_OK)
    {
      CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
      return rc;
    }
*/
    rc = mvHwsPortInit(devNum, portGroup, localPort, portMode, GT_TRUE,
                          refClock, PRIMARY_LINE_SRC);
    if(rc != GT_OK)
    {
        CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
      return rc;
    }


    /* enable the port */
    rc = cpssDxChPortEnableSet(devNum,ringPort,GT_TRUE);
    if(rc != GT_OK)
    {
      CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
      return rc;
    }

    /* force link up */
/*    rc = cpssDxChPortForceLinkPassEnableSet(devNum,ringPort,GT_TRUE);
    if(rc != GT_OK)
    {
      CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
      return rc;
    }
    */
    /* loopback mode */
    rc = cpssDxChPortInternalLoopbackEnableSet(devNum,ringPort,GT_TRUE);
    if(rc != GT_OK)
    {
        CPSS_ENABLER_DBG_TRACE_RC_MAC(funcName , rc);
      return rc;
    }
    osPrintSync("set ring port %d (%d,%d) \n",ringPort,portGroup,localPort);

  }

  return GT_OK;
}

