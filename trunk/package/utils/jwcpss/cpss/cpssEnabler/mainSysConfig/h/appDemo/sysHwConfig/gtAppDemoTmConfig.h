/*******************************************************************************
*              (c), Copyright 2013, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtAppDemoTmConfig.h
*
* DESCRIPTION:
*       Initialization functions for the TM.
*
* DEPENDENCIES:
*
*
* FILE REVISION NUMBER:
*       $Revision: 2.*
*******************************************************************************/
#ifndef __gtAppDemoTmConfigh
#define __gtAppDemoTmConfigh


/*******************************************************************************
* appDemoTmGeneral48PortsInit
*
* DESCRIPTION:
*      Implements general 48 Ports initialization for specific device.
*
* INPUTS:
*      dev     -   device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoTmGeneral48PortsInit
(
    IN  GT_U8   dev
);


/*******************************************************************************
* appDemoTmGeneral4PortsInit
*
* DESCRIPTION:
*      Implements general 4 Ports initialization for specific device.
*
* INPUTS:
*      dev     -   device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoTmGeneral4PortsInit
(
    IN  GT_U8   dev
);

/*******************************************************************************
* appDemoTmScenario2Init
*
* DESCRIPTION:
*      Implements scenario 2 initialization for specific device.
*      10 1G ports, 4 c node per port, 5 b node per c node, 10 a node per b node, 8 queues per a node
*      shaping on all levels (a,b,c,p)
*      Tail Drop on port and queue level
*
* INPUTS:
*      dev     -   device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoTmScenario2Init
(
    IN  GT_U8   dev
);

/*******************************************************************************
* appDemoTmScenario3Init
*
* DESCRIPTION:
*      Implements scenario 3 initialization for specific device.
*      10 1G ports, 4 c node per port, 5 b node per c node, 10 a node per b node, 8 queues per a node
*      shaping on all levels (a,b,c,p), propagated priority.
*      Tail Drop on port and queue level and color aware on  Q1 in each A node.
* INPUTS:
*      dev     -   device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoTmScenario3Init
(
    IN  GT_U8   dev
);

/*******************************************************************************
* appDemoTmScenario4Init
*
* DESCRIPTION:
*      Implements scenario 4 initialization for specific device.
*      10 1G ports, 4 c node per port, 5 b node per c node, 10 a node per b node, 8 queues per a node
*      shaping on all levels (a,b,c,p), propagated priority.
*      Tail Drop on port and queue level and color aware on Q1 in each A node and wred mode on queue0 in each A node
*
* INPUTS:
*      dev     -   device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoTmScenario4Init
(
    IN  GT_U8   dev
);

/*******************************************************************************
* appDemoTmScenario5Init
*
* DESCRIPTION:
*      Implements scenario 5 initialization for specific device.
*      10 1G ports, 4 c node per port, 5 b node per c node, 10 a node per b node, 4 queues per a node
*      shaping on all levels (a,b,c,p), propagated priority.
*      Tail Drop on port wred drop all queues
*
* INPUTS:
*      dev     -   device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoTmScenario5Init
(
    IN  GT_U8   dev
);
/*******************************************************************************
* appDemoTmScenario6Init
*
* DESCRIPTION:
*      Implements scenario 6 initialization for specific device.
*
* INPUTS:
*      dev     -   device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoTmScenario6Init
(
    IN  GT_U8   dev
);


/*******************************************************************************
* appDemoTmScenario7Init
*
* DESCRIPTION:
*      Implements scenario 5 initialization for specific device.
*      10 1G ports, 4 c node per port, 5 b node per c node, 10 a node per b node, 4 queues per a node
*      shaping on all levels (a,b,c,p), propagated priority.
*      Tail Drop on port wred drop all queues
*
* INPUTS:
*      dev     -   device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoTmScenario7Init
(
    IN  GT_U8   dev
);

/*******************************************************************************
* appDemoTmScenario8Init
*
* DESCRIPTION:
*      Implements scenario 8 initialization for specific device.
*  In this scenario there are shpaers for every (VLAN,TC) pair 
*
* INPUTS:
*      dev     -   device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoTmScenario8Init
(
    IN  GT_U8   dev
);

/*******************************************************************************
* appDemoTmScenario9Init
*
* DESCRIPTION:
*      Scenario for demonstration of PFC. There is one C-node for one Q-node, including nodes 
*      in between
*
* INPUTS:
*      dev     -   device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoTmScenario9Init
(
    IN  GT_U8   dev
);

/*******************************************************************************
* appDemoTmRev4ScenarioInit
*
* DESCRIPTION:
*      Scenario for demonstration of PFC. There is one C-node for one Q-node, including nodes
*      in between
*
* INPUTS:
*      dev     -   device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoTmRev4ScenarioInit
(
    IN  GT_U8   dev
);


/*******************************************************************************
* appDemoTmStressScenarioInit
*
* DESCRIPTION:
*      Implements strss test 
*
* INPUTS:
*      dev     -   device number,
*	   mode    -   select tree configuration  -  implementation nodes in  .c file
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoTmStressScenarioInit
(
    IN  GT_U8   dev,
    IN  GT_U8   mode
);


/*******************************************************************************
* appDemoTmScenario20Init
*
* DESCRIPTION:
*      Implements scenario 2 initialization for specific device.
*      10 1G ports, 4 c node per port, 5 b node per c node, 10 a node per b node, 8 queues per a node
*      shaping on all levels (a,b,c,p)
*      Tail Drop on port and queue level
*	   port 64 - 40G   -> C -> B -> A -> 252 Queues
*	   no shaping , no drop
*
* INPUTS:
*      dev     -   device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoTmScenario20Init
(
    IN  GT_U8   dev
);

/*******************************************************************************
* appDemoTmLibInitOnly
*
* DESCRIPTION:
*      Implements only cpssTmCtlLibInit() - default init configuration for TM*
* INPUTS:
*      dev     -   device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - otherwise
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoTmLibInitOnly
(
    IN  GT_U8   dev
);

/* get total Queues per port for TM scenarios 1-9 */
extern GT_U32 appDemoTmGetTotalQueuesPerPort(void);

#endif /*  __gtAppDemoTmConfigh */
