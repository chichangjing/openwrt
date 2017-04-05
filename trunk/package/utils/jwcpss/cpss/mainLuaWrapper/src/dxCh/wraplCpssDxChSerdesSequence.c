 /*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wraplCpssDxChSerdesSequence.c
*
* DESCRIPTION:
*       A lua wrapper for serdes sequence functions.
*
*
* DEPENDENCIES:
*
* COMMENTS:
*
* FILE REVISION NUMBER:
*       $Revision: 11 $
*******************************************************************************/
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/generic/cpssTypes.h>
#include <generic/private/prvWraplGeneral.h>
#include <gtOs/gtOsIo.h>

#ifndef CLI_STANDALONE
#include <serdes/mvHwsSerdesPrvIf.h>
#include <mvHwsPortApInitIf.h>
#include <mvHwsPortCfgIf.h>
#include <mvHwsPortInitIf.h>
#endif
#include <stdlib.h>
#include <generic/private/prvWraplGeneral.h>

#include <extUtils/luaCLI/luaCLIEngine_genWrapper.h>



/*******************************************************************************
* prvCpssDxChSerdesConfigurationGet
*
* DESCRIPTION:
*       Serdes Configuration Set
*
*
*
* INPUTS:
*        devNum                   - device number
*        portGroup                - port-group number
*        seqType                  - serdes sequence type  
*
* OUTPUTS:
*
* RETURNS:
*       1; GT_OK if there are no errors
*       2; error code and error message, if error occures
*
* COMMENTS:
*
*******************************************************************************/
#ifndef CLI_STANDALONE
GT_STATUS prvCpssDxChSerdesConfigurationGet(
  /*!     INPUTS:             */
      IN  GT_U8       devNum,
      IN  GT_U32      portGroup,
      IN  GT_U32      seqType

)
{
    
    GT_STATUS             rc = GT_OK;         /* Return code  */
    MV_POLLING_OP_PARAMS *polParam;
    MV_WRITE_OP_PARAMS   *wrParam;
    MV_DELAY_OP_PARAMS   *delOp;
    MV_CFG_ELEMENT        cfgEl;
    GT_U32                baseAddr; 
    MV_HWS_COM_PHY_H_SUB_SEQ serdesType;
    static GT_U32 seqCurLine = 0;
    GT_U32 operationType = 0, unitIndex = 0, regAddress = 0, dataToWrite = 0, mask = 0, delay = 0, numOfLoops = 0;

    serdesType = seqType;
    osPrintf("\nline  operationType  baseAddress  regAddress   dataToWrite    mask    delay numOfLoops\n");
      osPrintf("----  -------------  -----------  ----------   ----------- ---------- ----- ----------\n");
    while (rc != GT_NO_MORE)
    {
        rc = hwsSerdesSeqGet(devNum, portGroup,(MV_HWS_COM_PHY_H_SUB_SEQ)serdesType, &cfgEl, seqCurLine);      

        /*osPrintf("hwsSerdesSeqGet status received: 0x%x devNum %d, serdesType %d, line %d\n"
                                                                                        ,rc
                                                                                        ,devNum
                                                                                        ,serdesType                                                                                                             
                                                                                        ,seqCurLine);
                                                                                        */
        operationType = cfgEl.op;

        switch(operationType)
        {
        case WRITE_OP:                               
          wrParam = (MV_WRITE_OP_PARAMS *)cfgEl.params;
          mvUnitInfoGet(devNum, (MV_HWS_UNITS_ID)wrParam->indexOffset, &baseAddr, &unitIndex);

          regAddress = wrParam->regOffset;
          dataToWrite = wrParam->data;
          mask = wrParam->mask;
          
         /* OSIOG_printf("operationType %d, wrParam->indexOffset %d, baseAddr 0x%x, unitIndex %d, regAddress 0x%x, dataToWrite 0x%x, mask 0x%x \n"
                                                                                        ,operationType
                                                                                        ,wrParam->indexOffset
                                                                                        ,baseAddr
                                                                                        ,unitIndex
                                                                                        ,regAddress
                                                                                        ,dataToWrite
                                                                                        ,mask);*/
          break;
        case DELAY_OP:
          delOp = (MV_DELAY_OP_PARAMS *)cfgEl.params;
          delay = delOp->delay;
          
          /*OSIOG_printf("operationType %d, delay %d \n"
                                                                                        ,operationType
                                                                                        ,delay);*/
          break;
        case POLLING_OP:
          polParam = (MV_POLLING_OP_PARAMS *)cfgEl.params;
          mvUnitInfoGet(devNum, (MV_HWS_UNITS_ID)polParam->indexOffset, &baseAddr, &unitIndex);
          
          regAddress         = polParam->regOffset;
          dataToWrite        = polParam->data;
          mask               = polParam->mask;
          delay              = polParam->waitTime;
          numOfLoops         = polParam->numOfLoops;
          
          /*OSIOG_printf("operationType %d, wrParam->indexOffset %d, baseAddr 0x%x, unitIndex %d, regAddress 0x%x, dataToWrite 0x%x, mask 0x%x, delay %d, numOfLoops %d \n"
                                                                                        ,operationType
                                                                                        ,polParam->indexOffset
                                                                                        ,baseAddr
                                                                                        ,unitIndex
                                                                                        ,regAddress
                                                                                        ,dataToWrite
                                                                                        ,mask
                                                                                        ,delay
                                                                                        ,numOfLoops);*/
          break;
        }
        
        osPrintf(" %02d    0x%08x    0x%08x   0x%08x   0x%08x  0x%08x   %d     %d  \n"
                                                                                        ,seqCurLine
                                                                                        ,operationType
                                                                                        ,baseAddr
                                                                                        ,regAddress
                                                                                        ,dataToWrite
                                                                                        ,mask
                                                                                        ,delay
                                                                                        ,numOfLoops);
        seqCurLine++;
        }
                
    seqCurLine = 0;
    return rc;
}
#else

GT_STATUS prvCpssDxChSerdesConfigurationGet(
  /*!     INPUTS:             */
      IN  GT_U8       devNum,
      IN  GT_U32      portGroup,
      IN  GT_U32      seqType

)
{
    /* treat warnnig*/
    portGroup = devNum+seqType;
    return GT_OK;
}
#endif

/*$ END OF <UNQP_debug_ap_configuration_func> */

/*******************************************************************************
* wrlCpssDxChSerdesConfigurationGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*
*
* INPUTS:
*        L                   - lua state
*
* OUTPUTS:
*
* RETURNS:
*       1; GT_OK is pused to lua stack if no errors occurs and such vlan is not
*       exists
*       2; error code and error message, if error occures
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS wrlCpssDxChSerdesConfigurationGet
(
    IN lua_State            *L
)
{

    
    GT_U8                                   devNum                  =
                                                    (GT_U8)  lua_tonumber(L, 1);
                                                                    /* devId */
    GT_U32                                  portGroup               =
                                                    (GT_U32)  lua_tonumber(L, 2);
                                                                    /* portGroup */
    GT_U32                                  seqType                 =
                                                    (GT_U32) lua_tonumber(L, 3);
                                                                    /* seqType */
  
    GT_STATUS                               status                  = GT_OK;
 
    status = prvCpssDxChSerdesConfigurationGet(devNum, portGroup, seqType);                                
    return status;
}


/*******************************************************************************
* prvCpssDxChSerdesConfigurationSet
*
* DESCRIPTION:
*       Serdes Configuration Set
*
*
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*        devNum                   - device number
*        portGroup                - port-group number
*        seqType                  - serdes sequence type  
*        lineNum                  - line number
*        operationType            - operation type
*        baseAddress              - base address
*        regAddress               - reg address
*        dataToWrite              - data to write
*        mask                     - mask
*        delay                    - delay
*        numOfLoops               - number of loops
*
* OUTPUTS:
*
* RETURNS:
*       1; GT_OK 
*       2; error code and error message, if error occures
*
* COMMENTS:
*
*******************************************************************************/
#ifndef CLI_STANDALONE
GT_STATUS prvCpssDxChSerdesConfigurationSet(
  /*!     INPUTS:             */
    IN GT_U8   devNum,
    IN GT_U32  portGroup,
    IN GT_U32  seqType,
    IN GT_U16  lineNum,
    IN GT_U32  operationType,
    IN GT_U32  baseAddress,
    IN GT_U32  regAddress,
    IN GT_U32  dataToWrite,
    IN GT_U32  mask,
    IN GT_U16  delay,
    IN GT_U16  numOfLoops
)
{
    GT_STATUS            rc   = GT_OK;         /* Return code  */
    MV_POLLING_OP_PARAMS polParam;
    MV_WRITE_OP_PARAMS   wrParam;
    MV_DELAY_OP_PARAMS   delOp;
    MV_CFG_ELEMENT       cfgEl;
    MV_HWS_UNITS_ID      unitId;
    GT_U32               unitIndex = baseAddress ;
    
    /*check for valid arguments - TBD */
                
    
    /*
    if (unitIndex == 0)
    {
                    sscanf(argv[6], "%X", &unitIndex);
    }
    //regAddress = atoi(argv[7]);
    if (regAddress == 0)
    {
                    sscanf(argv[7], "%X", &regAddress);
    }
    //dataToWrite = atoi(argv[8]);
    if (dataToWrite == 0)
    {
                    sscanf(argv[8], "%X", &dataToWrite);
    }
    //mask = atoi(argv[9]); 
    if (mask == 0)
    {
                    sscanf(argv[9], "%X", &mask);
    }
    //delay = atoi(argv[10]);
    //numOfLoops = atoi(argv[11]);
*/
    switch(operationType)
    {
        case WRITE_OP:
          mvUnitInfoGetByAddr(devNum, (GT_U32)unitIndex, &unitId);
          wrParam.indexOffset = unitId;
          wrParam.regOffset = regAddress;
          wrParam.data = dataToWrite;
          wrParam.mask = mask;
          cfgEl.params = &wrParam;
          cfgEl.op = WRITE_OP;
          /*
          OSIOG_printf("operationType %d, wrParam->indexOffset 0x%x, wrParam.regOffset 0x%x, wrParam.data 0x%x, unitIndex 0x%x, unitId 0x%x\n"
                                                                                                        ,operationType
                                                                                                        ,wrParam.indexOffset
                                                                                                        ,wrParam.regOffset
                                                                                                        ,wrParam.data
                                                                                                        ,unitIndex
                                                                                                        ,unitId);*/
                                                                                                        
          break;
        case DELAY_OP:
          delOp.delay = delay;
          cfgEl.params = &delOp;
          cfgEl.op = DELAY_OP;
          /*OSIOG_printf("operationType %d, delay %d\n"
                                                                                                        ,delay);*/
          break;
        case POLLING_OP:
          mvUnitInfoGetByAddr(devNum, (GT_U32)unitIndex, &unitId);
          polParam.indexOffset = unitId;
          polParam.regOffset = regAddress;
          polParam.data = dataToWrite;
          polParam.mask = mask;
          polParam.waitTime = delay;
          polParam.numOfLoops = numOfLoops;
          cfgEl.params = &polParam;
          cfgEl.op = POLLING_OP;
        /* 
          OSIOG_printf("operationType %d, wrParam->indexOffset 0x%x, wrParam.regOffset 0x%x, wrParam.data 0x%x, polParam.mask 0x%x, unitIndex 0x%x, unitId 0x%x\n"
                                                                                                                    ,operationType
                                                                                                                    ,polParam.indexOffset
                                                                                                                    ,polParam.regOffset                                                                                                      
                                                                                                                    ,polParam.data
                                                                                                                    ,polParam.mask
                                                                                                                    ,unitIndex
                                                                                                                    ,unitId);
          */
          break;
    }

    rc = hwsSerdesSeqSet(devNum, portGroup,((lineNum == 0) ? GT_TRUE : GT_FALSE),
      (MV_HWS_COM_PHY_H_SUB_SEQ)seqType, &cfgEl, lineNum);

                
    return rc;
}
/*$ END OF <UNQP_debug_ap_configuration_func> */

#else
GT_STATUS prvCpssDxChSerdesConfigurationSet(
  /*!     INPUTS:             */
    IN GT_U8   devNum,
    IN GT_U32  portGroup,
    IN GT_U32  seqType,
    IN GT_U16  lineNum,
    IN GT_U32  operationType,
    IN GT_U32  baseAddress,
    IN GT_U32  regAddress,
    IN GT_U32  dataToWrite,
    IN GT_U32  mask,
    IN GT_U16  delay,
    IN GT_U16  numOfLoops
)
{
    /*treat warnings*/
    numOfLoops = delay - mask - dataToWrite - regAddress + baseAddress + operationType + lineNum + seqType + portGroup - devNum;
    return GT_OK;
}
#endif

/*******************************************************************************
* wrlCpssDxChSerdesConfigurationSet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*
*
* INPUTS:
*        L                   - lua state
*
* OUTPUTS:
*
* RETURNS:
*       1; GT_OK is pused to lua stack if no errors occurs and such vlan is not
*       exists
*       2; error code and error message, if error occures
*
* COMMENTS:
*
*******************************************************************************/

GT_STATUS wrlCpssDxChSerdesConfigurationSet
(
    IN lua_State            *L
)
{

    
    GT_U8                                   devNum                  =
                                                    (GT_U8)  lua_tonumber(L, 1);
                                                                    /* devId */
    GT_U32                                  portGroup               =
                                                    (GT_U32) lua_tonumber(L, 2);
                                                                    /* portGroup */
    GT_U32                                  seqType                 =
                                                    (GT_U32) lua_tonumber(L, 3);
                                                                    /* seqType */
    GT_U16                                  lineNum                 =
                                                    (GT_U16) lua_tonumber(L, 4);
                                                                    /* lineNum */
    GT_U32                                  operationType           =
                                                    (GT_U32) lua_tonumber(L, 5);
                                                                    /* operationType */
    GT_U32                                  baseAddress             =
                                                    (GT_U32) lua_tonumber(L, 6);
                                                                    /* baseAddress */
    GT_U32                                  regAddress              =
                                                    (GT_U32) lua_tonumber(L, 7);
                                                                    /* regAddress */                                                                
    GT_U32                                  dataToWrite             =
                                                    (GT_U32) lua_tonumber(L, 8);
                                                                    /* dataToWrite */  
    GT_U32                                  mask                    =
                                                    (GT_U32) lua_tonumber(L, 9);
                                                                    /* mask */
    GT_U16                                  delay                   =
                                                    (GT_U16) lua_tonumber(L, 10);
                                                                    /* delay */                                                                
    GT_U16                                  numOfLoops              =
                                                    (GT_U16) lua_tonumber(L, 11);
                                                                    /* numOfLoops */ 


    GT_STATUS                               status                  = GT_OK;
 
    status = prvCpssDxChSerdesConfigurationSet(devNum, portGroup, seqType, lineNum, operationType, baseAddress, regAddress, dataToWrite, mask, delay, numOfLoops);                                
    return status;
}

