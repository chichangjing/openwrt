#include <madCopyright.h>
/********************************************************************************
* madPtpTest.c
*
* DESCRIPTION:
*       PTP API test functions
*
* DEPENDENCIES:
*       MAD Driver should be loaded and Phys should be enabled
*       before running sample test functions as specified in
*       sample\Initialization\madApiInit.c.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/
#include "madSample.h"
#include "madHwCntl.h"

static void madWait (int waitTime)
{
	volatile int count=waitTime*100000;
    while (count--);
}

#define MAX_TEST_RUN            16
#define StepWaitTime    0x20
#define WaitTime    0x10

static void qdSleep(int time)
{
  madWait(time*10); 
}


/* Common test functions */

MAD_STATUS testBoolType(MAD_DEV *dev, TEST_API* api)
{
    MAD_STATUS status;
    MAD_BOOL orgValue, tmpValue;
    MAD_STATUS testResult = MAD_OK;
    
    /* Get the Original value */
    if((status=api->getFunc.bool(dev,&orgValue)) != MAD_OK)
    {
        MSG_PRINT(("Reading Bool Value Failed 0 (%#x).\n", status));
        return status;
    }
    qdSleep(WaitTime);

    /* Set to 1 */
    if((status=api->setFunc.bool(dev,MAD_TRUE)) != MAD_OK)
    {
        MSG_PRINT(("Writing Bool Value Failed 0 (%#x).\n", status));
        return status;
    }
    qdSleep(WaitTime);

    /* Get the modified value */
    if((status=api->getFunc.bool(dev,&tmpValue)) != MAD_OK)
    {
        MSG_PRINT(("Reading Bool Value Failed 1 (%#x).\n", status));
        return status;
    }

    if(tmpValue != MAD_TRUE)
    {
        MSG_PRINT(("Test Value Mismatch (expected MAD_TRUE) %x.\n", tmpValue));
        testResult = MAD_FAIL;
    }

    /* Set to 0 */
    if((status=api->setFunc.bool(dev,MAD_FALSE)) != MAD_OK)
    {
        MSG_PRINT(("Writing Bool Value Failed 1 (%#x).\n", status));
        return status;
    }
    qdSleep(WaitTime);

    /* Get the modified value */
    if((status=api->getFunc.bool(dev,&tmpValue)) != MAD_OK)
    {
        MSG_PRINT(("Reading Bool Value Failed 2 (%#x).\n", status));
        return status;
    }

    if(tmpValue != MAD_FALSE)
    {
        MSG_PRINT(("Test Value Mismatch (expected MAD_FALSE).\n"));
        testResult = MAD_FAIL;
    }

    /* Set to original value */
    if((status=api->setFunc.bool(dev,orgValue)) != MAD_OK)
    {
        MSG_PRINT(("Writing Bool Value Failed 2 (%#x).\n", status));
        return status;
    }
    qdSleep(WaitTime);

    return testResult;        
}


MAD_STATUS testU8Type(MAD_DEV *dev, TEST_API* api, int testLimit)
{
    MAD_STATUS status;
    MAD_U8 orgValue, tmpValue, i;
    MAD_STATUS testResult = MAD_OK;
        
    /* Get the Original value */
    if((status=api->getFunc.u8(dev,&orgValue)) != MAD_OK)
    {
        MSG_PRINT(("Reading U8 Value Failed (%#x).\n", status));
        return status;
    }
    qdSleep(WaitTime);

    /* Set to 0 */
    for(i=0; i<(MAD_U8)testLimit; i++)
    {
        if((status=api->setFunc.u8(dev,i)) != MAD_OK)
        {
            MSG_PRINT(("Writing U8 Value Failed (%#x, value %d).\n", status,i));
            return status;
        }
    qdSleep(WaitTime);

        /* Get the modified value */
        if((status=api->getFunc.u8(dev,&tmpValue)) != MAD_OK)
        {
            MSG_PRINT(("Reading U8 Value Failed (%#x, value %d).\n", status,i));
            return status;
        }

        if(tmpValue != i)
        {
            MSG_PRINT(("Test Value Mismatch (write %d, read %d).\n",i,tmpValue));
            testResult = MAD_FAIL;
        }
    }

    /* Set to original value */
    if((status=api->setFunc.u8(dev,orgValue)) != MAD_OK)
    {
        MSG_PRINT(("Writing U8 Value Failed (%#x, org value %d).\n", status, orgValue));
        return status;
    }
    qdSleep(WaitTime);

    return testResult;        
}

MAD_STATUS testU16Type(MAD_DEV *dev, TEST_API* api, int testLimit)
{
    MAD_STATUS status;
    MAD_U16 orgValue, tmpValue, i;
    MAD_STATUS testResult = MAD_OK;
        
    /* Get the Original value */
    if((status=api->getFunc.u16(dev,&orgValue)) != MAD_OK)
    {
        MSG_PRINT(("Reading U16 Value Failed (%#x).\n", status));
        return status;
    }
    qdSleep(WaitTime);

    /* Set to 0 */
    for(i=0; i<(MAD_U16)testLimit; i++)
    {
        if((status=api->setFunc.u16(dev,i)) != MAD_OK)
        {
            MSG_PRINT(("Writing U16 Value Failed (%#x, value %d).\n", status,i));
            return status;
        }
    qdSleep(WaitTime);

        /* Get the modified value */
        if((status=api->getFunc.u16(dev,&tmpValue)) != MAD_OK)
        {
            MSG_PRINT(("Reading U16 Value Failed (%#x, value %d).\n", status,i));
            return status;
        }

        if(tmpValue != i)
        {
            MSG_PRINT(("Test Value Mismatch (write %d, read %d).\n",i,tmpValue));
            testResult = MAD_FAIL;
        }
    }

    /* Set to original value */
    if((status=api->setFunc.u16(dev,orgValue)) != MAD_OK)
    {
        MSG_PRINT(("Writing U16 Value Failed (%#x, org value %d).\n", status, orgValue));
        return status;
    }
    qdSleep(WaitTime);

    return testResult;        
}

MAD_STATUS testU32Type(MAD_DEV *dev, TEST_API* api, int testLimit)
{
    MAD_STATUS status;
    MAD_U32 orgValue, tmpValue, i;
    MAD_STATUS testResult = MAD_OK;
        
    /* Get the Original value */
    if((status=api->getFunc.u32(dev,&orgValue)) != MAD_OK)
    {
        MSG_PRINT(("Reading U32 Value Failed (%#x).\n", status));
        return status;
    }
    qdSleep(50);

    /* Set to 0 */
    for(i=0; i<(MAD_U32)testLimit; i++)
    {
        if((status=api->setFunc.u32(dev,i)) != MAD_OK)
        {
            MSG_PRINT(("Writing U32 Value Failed (%#x, value %d).\n", (int)status,(int)i));
            return status;
        }
    qdSleep(WaitTime);

        /* Get the modified value */
        if((status=api->getFunc.u32(dev,&tmpValue)) != MAD_OK)
        {
            MSG_PRINT(("Reading U32 Value Failed (%#x, value %d).\n", (int)status,(int)i));
            return status;
        }
	qdSleep(WaitTime);
        if(tmpValue != i)
        {
            MSG_PRINT(("Test Value Mismatch (write %d, read %d).\n",(int)i,(int)tmpValue));
            testResult = MAD_FAIL;
        }
    }

    /* Set to original value */
    if((status=api->setFunc.u32(dev,orgValue)) != MAD_OK)
    {
        MSG_PRINT(("Writing U32 Value Failed (%#x, org value %d).\n", status, (int)orgValue));
        return status;
    }
    qdSleep(WaitTime);

    return testResult;        
}

MAD_STATUS testU8TypeWithStep(MAD_DEV *dev, TEST_API* api, int testLimit, int step)
{
    MAD_STATUS status;
    MAD_U8 orgValue, tmpValue, i;
    MAD_STATUS testResult = MAD_OK;
        
    /* Get the Original value */
    if((status=api->getFunc.u8(dev,&orgValue)) != MAD_OK)
    {
        MSG_PRINT(("Reading U8 Value Failed (%#x).\n", status));
        return status;
    }
    qdSleep(WaitTime);

    /* Set to 0 */
    for(i=0; i<(MAD_U8)testLimit; i+=step)
    {
        if((status=api->setFunc.u8(dev,i)) != MAD_OK)
        {
            MSG_PRINT(("Writing U8 Value Failed (%#x, value %d).\n", status,i));
            return status;
        }
    qdSleep(StepWaitTime);

        /* Get the modified value */
        if((status=api->getFunc.u8(dev,&tmpValue)) != MAD_OK)
        {
            MSG_PRINT(("Reading U8 Value Failed (%#x, value %d).\n", status,i));
            return status;
        }

        tmpValue &=0xff;
        if(tmpValue != i)
        {
            MSG_PRINT(("Test Value Mismatch (write 0x%x, read 0x%x).\n",i,tmpValue));
            testResult = MAD_FAIL;
        }
    }

    /* Set to original value */
    if((status=api->setFunc.u8(dev,orgValue)) != MAD_OK)
    {
        MSG_PRINT(("Writing U8 Value Failed (%#x, org value %d).\n", status, orgValue));
        return status;
    }
    qdSleep(WaitTime);

    return testResult;        
}

MAD_STATUS testU16TypeWithStep(MAD_DEV *dev, TEST_API* api, int testLimit, int step)
{
    MAD_STATUS status;
    MAD_U16 orgValue, tmpValue, i;
    MAD_STATUS testResult = MAD_OK;
        
    /* Get the Original value */
    if((status=api->getFunc.u16(dev,&orgValue)) != MAD_OK)
    {
        MSG_PRINT(("Reading U16 Value Failed (%#x).\n", status));
        return status;
    }
    qdSleep(WaitTime);

    /* Set to 0 */
    for(i=0; i<testLimit; i+=step)
    {
        if((status=api->setFunc.u16(dev,i)) != MAD_OK)
        {
            MSG_PRINT(("Writing U16 Value Failed (%#x, value %d).\n", status,i));
            return status;
        }
    qdSleep(StepWaitTime*3);

        /* Get the modified value */
        if((status=api->getFunc.u16(dev,&tmpValue)) != MAD_OK)
        {
            MSG_PRINT(("Reading U16 Value Failed (%#x, value %d).\n", status,i));
            return status;
        }

        if(tmpValue != i)
        {
            MSG_PRINT(("Test Value Mismatch (write 0x%x, read 0x%x).\n",i,tmpValue));
            testResult = MAD_FAIL;
        }
    }

    /* Set to original value */
    if((status=api->setFunc.u16(dev,orgValue)) != MAD_OK)
    {
        MSG_PRINT(("Writing U16 Value Failed (%#x, org value %d).\n", status, orgValue));
        return status;
    }
    qdSleep(WaitTime);

    return testResult;        
}

MAD_STATUS testU32TypeWithStep(MAD_DEV *dev, TEST_API* api, int testLimit, int step)
{
    MAD_STATUS status;
    MAD_U32 orgValue, tmpValue, i;
    MAD_STATUS testResult = MAD_OK;
        
    /* Get the Original value */
    if((status=api->getFunc.u32(dev,&orgValue)) != MAD_OK)
    {
        MSG_PRINT(("Reading U32 Value Failed (%#x).\n", status));
        return status;
    }
    qdSleep(WaitTime);

    /* Set to 0 */
    for(i=0; i<(MAD_U32)testLimit; i+=step)
    {
        if((status=api->setFunc.u32(dev,i)) != MAD_OK)
        {
            MSG_PRINT(("Writing U32 Value Failed (%#x, value %d).\n", status,(int)i));
            return status;
        }
    qdSleep(StepWaitTime);

        /* Get the modified value */
        if((status=api->getFunc.u32(dev,&tmpValue)) != MAD_OK)
        {
            MSG_PRINT(("Reading U32 Value Failed (%#x, value %d).\n", status,(int)i));
            return status;
        }

        if(tmpValue != i)
        {
            MSG_PRINT(("Test Value Mismatch (write %d, read %d).\n",(int)i,(int)tmpValue));
            testResult = MAD_FAIL;
        }
    }

    /* Set to original value */
    if((status=api->setFunc.u32(dev,orgValue)) != MAD_OK)
    {
        MSG_PRINT(("Writing U32 Value Failed (%#x, org value %d).\n", status, (int)orgValue));
        return status;
    }
    qdSleep(WaitTime);

    return testResult;        
}

#if 0

MAD_STATUS testMacType(MAD_DEV *dev, TEST_API* api)
{
    MAD_STATUS status;
    MAD_ETHERADDR orgMac, tmpMacIn, tmpMacOut;
    MAD_STATUS testResult = MAD_OK;
        
    /* Get the Discard Excessive state */
    if((status=api->getFunc.mac(dev,&orgMac)) != MAD_OK)
    {
        MSG_PRINT(("Reading MAC Address Failed (%#x).\n", status));
        return status;
    }
    qdSleep(WaitTime);

    tmpMacIn.arEther[0] = 0xAA;
    tmpMacIn.arEther[1] = 0xAA;
    tmpMacIn.arEther[2] = 0xAA;
    tmpMacIn.arEther[3] = 0xAA;
    tmpMacIn.arEther[4] = 0xAA;
    tmpMacIn.arEther[5] = 0xAA;

    /* Set the Discard Excessive state */
    if((status=api->setFunc.mac(dev,&tmpMacIn)) != MAD_OK)
    {
        MSG_PRINT(("Writing MAC Address Failed (%#x).\n", status));
        return status;
    }
    qdSleep(WaitTime);

    /* Get the Discardl Excessive state */
    if((status=api->getFunc.mac(dev,&tmpMacOut)) != MAD_OK)
    {
        MSG_PRINT(("Reading MAC Address Failed (%#x).\n", status));
        return status;
    }

    if(memcmp(&tmpMacIn,&tmpMacOut,6) != 0)
    {
        MSG_PRINT(("Unexpected MAC address(%#x-%#x-%#x-%#x-%#x-%#x)\n",
                    tmpMacOut.arEther[0],
                    tmpMacOut.arEther[1],
                    tmpMacOut.arEther[2],
                    tmpMacOut.arEther[3],
                    tmpMacOut.arEther[4],
                    tmpMacOut.arEther[5]));

        testResult = MAD_FAIL;
    }

    tmpMacIn.arEther[0] = 0x54;
    tmpMacIn.arEther[1] = 0x55;
    tmpMacIn.arEther[2] = 0x55;
    tmpMacIn.arEther[3] = 0x55;
    tmpMacIn.arEther[4] = 0x55;
    tmpMacIn.arEther[5] = 0x55;

    /* Set the Discard Excessive state */
    if((status=api->setFunc.mac(dev,&tmpMacIn)) != MAD_OK)
    {
        MSG_PRINT(("Writing MAC Address Failed (%#x).\n", status));
        return status;
    }
    qdSleep(WaitTime);

    /* Get the Discardl Excessive state */
    if((status=api->getFunc.mac(dev,&tmpMacOut)) != MAD_OK)
    {
        MSG_PRINT(("Reading MAC Address Failed (%#x).\n", status));
        return status;
    }

    if(memcmp(&tmpMacIn,&tmpMacOut,6) != 0)
    {
        MSG_PRINT(("Unexpected MAC address(%#x-%#x-%#x-%#x-%#x-%#x)\n",
                    tmpMacOut.arEther[0],
                    tmpMacOut.arEther[1],
                    tmpMacOut.arEther[2],
                    tmpMacOut.arEther[3],
                    tmpMacOut.arEther[4],
                    tmpMacOut.arEther[5]));

        testResult = MAD_FAIL;
    }

    tmpMacIn.arEther[0] = 0x00;
    tmpMacIn.arEther[1] = 0x00;
    tmpMacIn.arEther[2] = 0x00;
    tmpMacIn.arEther[3] = 0x00;
    tmpMacIn.arEther[4] = 0x00;
    tmpMacIn.arEther[5] = 0x01;

    /* Set the Discard Excessive state */
    if((status=api->setFunc.mac(dev,&tmpMacIn)) != MAD_OK)
    {
        MSG_PRINT(("Writing MAC Address Failed (%#x).\n", status));
        return status;
    }
    qdSleep(WaitTime);

    /* Get the Discardl Excessive state */
    if((status=api->getFunc.mac(dev,&tmpMacOut)) != MAD_OK)
    {
        MSG_PRINT(("Reading MAC Address Failed (%#x).\n", status));
        return status;
    }

    if(memcmp(&tmpMacIn,&tmpMacOut,6) != 0)
    {
        MSG_PRINT(("Unexpected MAC address(%#x-%#x-%#x-%#x-%#x-%#x)\n",
                    tmpMacOut.arEther[0],
                    tmpMacOut.arEther[1],
                    tmpMacOut.arEther[2],
                    tmpMacOut.arEther[3],
                    tmpMacOut.arEther[4],
                    tmpMacOut.arEther[5]));

        testResult = MAD_FAIL;
    }

    /* Set the Discard Excessive state with original value */
    if((status=api->setFunc.mac(dev,&orgMac)) != MAD_OK)
    {
        MSG_PRINT(("Writing MAC Address Failed (%#x).\n", status));
        return status;
    }
    qdSleep(WaitTime);

    return testResult;        
}
#endif

MAD_STATUS testPortBoolType(MAD_DEV *dev, TEST_API* api)
{
    MAD_STATUS status;
    MAD_BOOL orgValue, tmpValue;
    MAD_STATUS testResult = MAD_OK;
    MAD_LPORT port;
    int portIndex;
        
    for(portIndex=0; portIndex<dev->numOfPorts; portIndex++)
    {
        port = portIndex;

        /* Get the Original value */
        if((status=api->getFunc.port_bool(dev,port,&orgValue)) != MAD_OK)
        {
            MSG_PRINT(("Reading Bool Value Failed (%#x).\n", status));
            return status;
        }
    qdSleep(WaitTime);

        /* Set to 1 */
        if((status=api->setFunc.port_bool(dev,port,MAD_TRUE)) != MAD_OK)
        {
            MSG_PRINT(("Writing Bool Value Failed (%#x).\n", status));
            return status;
        }
    qdSleep(WaitTime);

        /* Get the modified value */
        if((status=api->getFunc.port_bool(dev,port,&tmpValue)) != MAD_OK)
        {
            MSG_PRINT(("Reading Bool Value Failed (%#x).\n", status));
            return status;
        }

        if(tmpValue != MAD_TRUE)
        {
            MSG_PRINT(("Test Value Mismatch (expected MAD_TRUE).\n"));
            testResult = MAD_FAIL;
        }

        /* Set to 0 */
        if((status=api->setFunc.port_bool(dev,port,MAD_FALSE)) != MAD_OK)
        {
            MSG_PRINT(("Writing Bool Value Failed (%#x).\n", status));
            return status;
        }
    qdSleep(WaitTime);

        /* Get the modified value */
        if((status=api->getFunc.port_bool(dev,port,&tmpValue)) != MAD_OK)
        {
            MSG_PRINT(("Reading Bool Value Failed (%#x).\n", status));
            return status;
        }

        if(tmpValue != MAD_FALSE)
        {
            MSG_PRINT(("Test Value Mismatch (expected MAD_FALSE).\n"));
            testResult = MAD_FAIL;
        }

        /* Set to original value */
        if((status=api->setFunc.port_bool(dev,port,orgValue)) != MAD_OK)
        {
            MSG_PRINT(("Writing Bool Value Failed (%#x).\n", status));
            return status;
        }
    qdSleep(WaitTime);

        if (testResult != MAD_OK)
            return testResult;

    }
    return testResult;        
}


MAD_STATUS testPortU8Type(MAD_DEV *dev, TEST_API* api, int testLimit)
{
    MAD_STATUS status;
    MAD_U8 orgValue, tmpValue, i;
    MAD_STATUS testResult = MAD_OK;
    MAD_LPORT port;
    int portIndex;
        
    for(portIndex=0; portIndex<dev->numOfPorts; portIndex++)
    {
        port = portIndex;

        /* Get the Original value */
        if((status=api->getFunc.port_u8(dev,port,&orgValue)) != MAD_OK)
        {
            MSG_PRINT(("Reading U8 Value Failed (%#x).\n", status));
            return status;
        }
    qdSleep(WaitTime);

        /* Set to 0 */
        for(i=0; i<(MAD_U8)testLimit; i++)
        {
            if((status=api->setFunc.port_u8(dev,port,i)) != MAD_OK)
            {
                MSG_PRINT(("Writing U8 Value Failed (%#x).\n", status));
                return status;
            }
    qdSleep(WaitTime);

            /* Get the modified value */
            if((status=api->getFunc.port_u8(dev,port,&tmpValue)) != MAD_OK)
            {
                MSG_PRINT(("Reading U8 Value Failed (%#x).\n", status));
                return status;
            }

            if(tmpValue != i)
            {
                MSG_PRINT(("U16 Value Mismatch (port %d, write %#x, read %#x).\n", portIndex,i,tmpValue));
                testResult = MAD_FAIL;
            }
        }

        /* Set to original value */
        if((status=api->setFunc.port_u8(dev,port,orgValue)) != MAD_OK)
        {
            MSG_PRINT(("Writing U8 Value Failed (%#x).\n", status));
            return status;
        }
    qdSleep(WaitTime);

        if (testResult != MAD_OK)
            return testResult;

    }
    return testResult;        
}

MAD_STATUS testPortU16Type(MAD_DEV *dev, TEST_API* api, int testLimit)
{
    MAD_STATUS status;
    MAD_U16 orgValue, tmpValue, i;
    MAD_STATUS testResult = MAD_OK;
    MAD_LPORT port;
    int portIndex;
        
    for(portIndex=0; portIndex<dev->numOfPorts; portIndex++)
    {
        port = portIndex;

        /* Get the Original value */
        if((status=api->getFunc.port_u16(dev,port,&orgValue)) != MAD_OK)
        {
            MSG_PRINT(("Reading U16 Value Failed (%#x).\n", status));
            return status;
        }
    qdSleep(WaitTime);

        /* Set to 0 */
        for(i=0; i<(MAD_U16)testLimit; i++)
        {
            if((status=api->setFunc.port_u16(dev,port,i)) != MAD_OK)
            {
                MSG_PRINT(("Writing U16 Value Failed (%#x).\n", status));
                return status;
            }
    qdSleep(WaitTime);

            /* Get the modified value */
            if((status=api->getFunc.port_u16(dev,port,&tmpValue)) != MAD_OK)
            {
                MSG_PRINT(("Reading U16 Value Failed (%#x).\n", status));
                return status;
            }

            if(tmpValue != i)
            {
                MSG_PRINT(("U16 Value Mismatch (port %d, write %#x, read %#x).\n", portIndex,i,tmpValue));
                testResult = MAD_FAIL;
                return MAD_FAIL; /* MJ Temp */
            }
        }

        /* Set to original value */
        if((status=api->setFunc.port_u16(dev,port,orgValue)) != MAD_OK)
        {
            MSG_PRINT(("Writing U16 Value Failed (%#x).\n", status));
            return status;
        }
    qdSleep(WaitTime);

        if (testResult != MAD_OK)
            return testResult;

    }
    return testResult;        
}

MAD_STATUS testPortU32Type(MAD_DEV *dev, TEST_API* api, int testLimit)
{
    MAD_STATUS status;
    MAD_U32 orgValue, tmpValue, i;
    MAD_STATUS testResult = MAD_OK;
    MAD_LPORT port;
    int portIndex;
 
    for(portIndex=0; portIndex<dev->numOfPorts; portIndex++)
    {
        port = portIndex;
        
        /* Get the Original value */
        if((status=api->getFunc.port_u32(dev,port,&orgValue)) != MAD_OK)
        {
            MSG_PRINT(("Reading U32 ori Value Failed (%#x).\n", status));
            return status;
        }
		qdSleep(WaitTime);

        /* Set to 0 */
        for(i=0; i<(MAD_U32)testLimit; i++)
        {
            if((status=api->setFunc.port_u32(dev,port,i)) != MAD_OK)
            {
                MSG_PRINT(("Writing U32 Value Failed (port%d,data%d,%#x).\n", (int)port,(int)i,status));
                return status;
            }
			qdSleep(WaitTime);

            /* Get the modified value */
            if((status=api->getFunc.port_u32(dev,port,&tmpValue)) != MAD_OK)
            {
                MSG_PRINT(("Reading U32 Value Failed (%#x).\n", status));
                return status;
            }

            if(tmpValue != i)
            {
                MSG_PRINT(("U16 Value Mismatch (port %d, write %#x, read %#x).\n", (int)port,(int)i,(int)tmpValue));
                testResult = MAD_FAIL;
            }
        }

        /* Set to original value */
        if((status=api->setFunc.port_u32(dev,port,orgValue)) != MAD_OK)
        {
            MSG_PRINT(("Writing Org Value Failed (value %d,%#x).\n", (int)orgValue,status));
            return status;
        }
    qdSleep(WaitTime);

        if (testResult != MAD_OK)
            return testResult;

    }
    return testResult;        
}

MAD_STATUS testPortU16TypeWithStep(MAD_DEV *dev, TEST_API* api, int testLimit, int step)
{
    MAD_STATUS status;
    MAD_U16 orgValue, tmpValue, i;
    MAD_STATUS testResult = MAD_OK;
    MAD_LPORT port;
    int portIndex;
        
    for(portIndex=0; portIndex<dev->numOfPorts; portIndex++)
    {
        port = portIndex;
        
        /* Get the Original value */
        if((status=api->getFunc.port_u16(dev,port,&orgValue)) != MAD_OK)
        {
            MSG_PRINT(("Reading U16 Value Failed (%#x).\n", status));
            return status;
        }
    qdSleep(StepWaitTime);

        /* Set to 0 */
        for(i=0; i<(MAD_U16)testLimit; i+=step)
        {
            if((status=api->setFunc.port_u16(dev,port,i)) != MAD_OK)
            {
                MSG_PRINT(("Writing U16 Value Failed (port%d,data%d,%#x).\n", (int)port,(int)i,status));
                return status;
            }

            /* Get the modified value */
            if((status=api->getFunc.port_u16(dev,port,&tmpValue)) != MAD_OK)
            {
                MSG_PRINT(("Reading U16 Value Failed (%#x).\n", status));
                return status;
            }
    qdSleep(WaitTime);

            if(tmpValue != i)
            {
                MSG_PRINT(("U16 Value Mismatch (port %d, write %#x, read %#x).\n", (int)port,(int)i,tmpValue));
                testResult = MAD_FAIL;
            }
        }

        /* Set to original value */
        if((status=api->setFunc.port_u16(dev,port,orgValue)) != MAD_OK)
        {
            MSG_PRINT(("Writing Org Value Failed (value %d,%#x).\n", orgValue,status));
            return status;
        }
    qdSleep(WaitTime);

        if (testResult != MAD_OK)
            return testResult;

    }
    return testResult;        
}

MAD_STATUS testPortU32TypeWithStep(MAD_DEV *dev, TEST_API* api, int testLimit, int step)
{
    MAD_STATUS status;
    MAD_U32 orgValue, tmpValue, i;
    MAD_STATUS testResult = MAD_OK;
    MAD_LPORT port;
    int portIndex;
        
    for(portIndex=0; portIndex<dev->numOfPorts; portIndex++)
    {
        port = portIndex;
        
        /* Get the Original value */
        if((status=api->getFunc.port_u32(dev,port,&orgValue)) != MAD_OK)
        {
            MSG_PRINT(("Reading U32 Value Failed (%#x).\n", status));
            return status;
        }
    qdSleep(StepWaitTime);

        /* Set to 0 */
        for(i=0; i<(MAD_U32)testLimit; i+=step)
        {
            if((status=api->setFunc.port_u32(dev,port,i)) != MAD_OK)
            {
                MSG_PRINT(("Writing U32 Value Failed (port%d,data%d,%#x).\n", (int)port,(int)i,status));
                return status;
            }
    qdSleep(WaitTime);

            /* Get the modified value */
            if((status=api->getFunc.port_u32(dev,port,&tmpValue)) != MAD_OK)
            {
                MSG_PRINT(("Reading U32 Value Failed (%#x).\n", status));
                return status;
            }

            if(tmpValue != i)
            {
                MSG_PRINT(("U16 Value Mismatch (port %d, write %#x, read %#x).\n", (int)port,(int)i,(int)tmpValue));
                testResult = MAD_FAIL;
            }
        }

        /* Set to original value */
        if((status=api->setFunc.port_u32(dev,port,orgValue)) != MAD_OK)
        {
            MSG_PRINT(("Writing Org Value Failed (value %d,%#x).\n", (int)orgValue,status));
            return status;
        }
    qdSleep(WaitTime);

        if (testResult != MAD_OK)
            return testResult;

    }
    return testResult;        
}

MAD_STATUS testU32U32Type(MAD_DEV *dev, TEST_API* api, int indexLimit, int testLimit)
{
    MAD_STATUS status;
    MAD_U32 orgValue, tmpValue, i;
    MAD_STATUS testResult = MAD_OK;
    int index;

    for(index=0; index<indexLimit; index++)
    {
        /* Get the Original value */
        if((status=api->getFunc.port_u32(dev,index,&orgValue)) != MAD_OK)
        {
            MSG_PRINT(("Reading U32 Org Value Failed (%#x).\n", status));
            return status;
        }
    qdSleep(WaitTime);

        /* Set to 0 */
        for(i=0; i<(MAD_U32)testLimit; i++)
        {
            if((status=api->setFunc.port_u32(dev,index,i)) != MAD_OK)
            {
                MSG_PRINT(("Writing U32 Value Failed (%#x) index:%d,value:%d.\n", status,index,(int)i));
                return status;
            }
    qdSleep(WaitTime);

            /* Get the modified value */
            if((status=api->getFunc.port_u32(dev,index,&tmpValue)) != MAD_OK)
            {
                MSG_PRINT(("Reading U32 Value Failed (%#x) index:%d,value:%d.\n", status,index,(int)i));
                return status;
            }

            if(tmpValue != i)
            {
                MSG_PRINT(("U16 Value Mismatch (index %d, write %#x, read %#x).\n", index,(int)i,(int)tmpValue));
                testResult = MAD_FAIL;
            }
        }

        /* Set to original value */
        if((status=api->setFunc.port_u32(dev,index,orgValue)) != MAD_OK)
        {
            MSG_PRINT(("Writing U32 Org Value Failed (%#x) index:%d,value:%d.\n", status,index,(int)orgValue));
            return status;
        }
    qdSleep(WaitTime);

        if (testResult != MAD_OK)
            return testResult;

    }
    return testResult;        
}

MAD_STATUS testU32U32TypeWithStep(MAD_DEV *dev, TEST_API* api, int indexLimit, int testLimit, int step)
{
    MAD_STATUS status;
    MAD_U32 orgValue, tmpValue, i;
    MAD_STATUS testResult = MAD_OK;
    int index;

    for(index=0; index<indexLimit; index++)
    {
        /* Get the Original value */
        if((status=api->getFunc.port_u32(dev,index,&orgValue)) != MAD_OK)
        {
            MSG_PRINT(("Reading U32 Org Value Failed (%#x).\n", status));
            return status;
        }
    qdSleep(WaitTime);

        /* Set to 0 */
/*        for(i=0; i<(MAD_U32)testLimit; i++) */
        for(i=0; i<(MAD_U32)testLimit; i+=step)
        {
            if((status=api->setFunc.port_u32(dev,index,i)) != MAD_OK)
            {
                MSG_PRINT(("Writing U32 Value Failed (%#x) index:%d,value:%d.\n", status,index,(int)i));
                return status;
            }
    qdSleep(StepWaitTime);

            /* Get the modified value */
            if((status=api->getFunc.port_u32(dev,index,&tmpValue)) != MAD_OK)
            {
                MSG_PRINT(("Reading U32 Value Failed (%#x) index:%d,value:%d.\n", status,index,(int)i));
                return status;
            }

            if(tmpValue != i)
            {
                MSG_PRINT(("U16 Value Mismatch (index %d, write %#x, read %#x).\n", index,(int)i,(int)tmpValue));
                testResult = MAD_FAIL;
            }
        }

        /* Set to original value */
        if((status=api->setFunc.port_u32(dev,index,orgValue)) != MAD_OK)
        {
            MSG_PRINT(("Writing U32 Org Value Failed (%#x) index:%d,value:%d.\n", status,index,(int)orgValue));
            return status;
        }
    qdSleep(WaitTime);

        if (testResult != MAD_OK)
            return testResult;

    }
    return testResult;        
}

MAD_STATUS testU32U32Type1(MAD_DEV *dev, TEST_API* api, int indexStart, int indexLimit, int testStart, int testLimit)
{
    MAD_STATUS status;
    MAD_U32 orgValue, tmpValue, i;
    MAD_STATUS testResult = MAD_OK;
    int index;

    for(index=indexStart; index<indexLimit; index++)
    {
        /* Get the Original value */
        if((status=api->getFunc.port_u32(dev,index,&orgValue)) != MAD_OK)
        {
            MSG_PRINT(("Reading U32 Org Value Failed (%#x).\n", status));
            return status;
        }
    qdSleep(WaitTime);

        /* Set to 0 */
        for(i=testStart; i<(MAD_U32)testLimit; i++)
        {
            if((status=api->setFunc.port_u32(dev,index,i)) != MAD_OK)
            {
                MSG_PRINT(("Writing U32 Value Failed (%#x) index:%d,value:%d.\n", status,index,(int)i));
                return status;
            }
    qdSleep(WaitTime);

            /* Get the modified value */
            if((status=api->getFunc.port_u32(dev,index,&tmpValue)) != MAD_OK)
            {
                MSG_PRINT(("Reading U32 Value Failed (%#x) index:%d,value:%d.\n", status,index,(int)i));
                return status;
            }

            if(tmpValue != i)
            {
                MSG_PRINT(("U16 Value Mismatch (index %d, write %#x, read %#x).\n", index,(int)i,(int)tmpValue));
                testResult = MAD_FAIL;
            }
        }

        /* Set to original value */
        if((status=api->setFunc.port_u32(dev,index,orgValue)) != MAD_OK)
        {
            MSG_PRINT(("Writing U32 Org Value Failed (%#x) index:%d,value:%d.\n", status,index,(int)orgValue));
            return status;
        }
    qdSleep(WaitTime);

        if (testResult != MAD_OK)
            return testResult;

    }
    return testResult;        
}






MAD_STATUS samplePTPInit(MAD_DEV *dev)
{
    MAD_PTP_CONFIG			ptpCfg;
//    MAD_LPORT				port;
    MAD_STATUS				status;

    ptpCfg.ptpEType = 0x88F7;
    ptpCfg.msgIdTSEn = 0xd;        /* id 0, 2, and 3 */
    ptpCfg.tsArrPtr = 0x8;        /* id 0 and 2 for ARR0, id 3 for ARR1 */

    /* Transport specific bits present in PTP Common Header */
    ptpCfg.transSpec = 1;		/* 802.1AS */

    /* starting bit location for the Message ID field in the PTP Common Header */
    ptpCfg.msgIdStartBit = 4;    
    ptpCfg.ptpArrIntEn = 0x3F;
    ptpCfg.ptpDepIntEn = 0x3F;
    ptpCfg.disTSOverwrite = 0;

	if ((status = mdPtpSetPTPEn(dev, MAD_TRUE)) != MAD_OK)
    {
        MSG_PRINT(("mdPtpSetPTPEn return failed\n"));
        return status;
    }

    if ((status = mdPtpSetConfig(dev, &ptpCfg)) != MAD_OK)
    {
        MSG_PRINT(("mdPtpSetConfig return failed\n"));
        return status;
    }
    
    return MAD_OK;
}


MAD_STATUS samplePTP_BC_OneStep_IgrAct_msgId_0_3_IntPTPTime_0(MAD_DEV *dev)
{
    MAD_STATUS				status;
	MAD_LPORT				port, sPort, dPort;
	MAD_LPORT				portList[] = {0};
	MAD_LPORT				srcPortList[] = {0};
	MAD_LPORT				destPortList[] = {0};
	int						pListLen = 1;
	MAD_PTP_TOD_OP_CTRL		todOpCtrl;
	MAD_U8					timeArr[] = {0, 1, 2, 3};
	MAD_U8					domain[] = {0, 1, 2, 3};
	MAD_PTP_TS_STRUCT		ts;
	unsigned long long		ts_1722;
	MAD_U32					comp;
	MAD_U32					loadPoint;
	MAD_U32					glbTime;
	MAD_PTP_MODE_CONFIG		mode;
	MAD_U8					mstr[] = {0, 0, 1, 1};
	MAD_U8					altScheme[] = {0, 1, 0, 1};
	MAD_PTP_EXTERNAL_TIME_FORM	format;
	MAD_PTP_EG_ACT_VEC			vec_1588,vec_802_1AS;
	MAD_PTP_CONFIG			ptpConfig;
	MAD_PTP_SPEC			transSpecList[] = {0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1};
	MAD_PTP_TS_STATUS		arr0,arr1,dep;
	int						lp;
	int						i,j;

	if (srcPortList[0] == destPortList[0])
		lp = 1;
	else
		lp = 0;

	for (i = 0; i < pListLen; i++)
	{
		port = portList[i];
		/* Init Phy */
		if((status = madHwWritePagedPhyReg(dev,port,18,27,0)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed writing register.\n"));
			return MAD_FAIL;
		}
		if((status = madHwWritePagedPhyReg(dev,port,0,0,0x9140)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed writing register.\n"));
			return MAD_FAIL;
		}
		if((status = madHwWritePagedPhyReg(dev,0,2,21,0x1046)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed writing register.\n"));
			return MAD_FAIL;
		}
		if((status = madHwWritePagedPhyReg(dev,0,0,0,0x9140)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed writing register.\n"));
			return MAD_FAIL;
		}

		/* Enable PTP */
		if((status = mdPtpSetEnable_port(dev,port,MAD_TRUE)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed power up PTP.\n"));
			return MAD_FAIL;
		}

		/* Set MAC Loopback */
		if((status = mdDiagSetLineLoopback(dev,port,MAD_TRUE)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed set MAC loopback.\n"));
			return MAD_FAIL;
		}
	}

	MSG_PRINT(("Active used domain related time array.\n"));
	

	for (i = 0; i < pListLen; i++)
	{
		port = portList[i];
		todOpCtrl.ptpOpCtrlClkValid = 1;
		todOpCtrl.ptpOpCtrlTodOp = 3;	/* clean */
		for (j = 0; j < 4; j++)
		{
			todOpCtrl.ptpOpCtrlTimeArray = timeArr[j];
			todOpCtrl.ptpOpCtrlDomainNum = domain[j];

			ts.nanoseconds = rand()*0xFFFFFFFF;
			ts.seconds = rand()*0xFFFFFFFF;
			ts.topseconds = rand()*0xFFFF;
			ts_1722 = rand()*0xFFFFFFFF;
			comp = rand()*0xFFFFFFFF;
			loadPoint = rand()*0xFFFFFFFF;

			ts.nanoseconds = 0;
			ts.seconds = 0;
			ts.topseconds = 0;
			ts_1722 = 0;
			comp = 0;
			loadPoint = 0xFFFFFFFF;

			MSG_PRINT(("Configure timeArr for all ports.\n"));
			if((status = mdPtpGetPTPGlobalTime_port(dev,port,&glbTime)) != MAD_OK)
			{
				MAD_DBG_INFO(("Failed reading PTP Global Time.\n"));
				return MAD_FAIL;
			}
			loadPoint = glbTime+0x6000000;
			MSG_PRINT(("global time: %d, todLoadPt: 0x%x.\n", glbTime, loadPoint));
			if((status = mdPtpTODTimeSet_port(dev,port,&ts)) != MAD_OK)
			{
				MAD_DBG_INFO(("Failed writing ToD.\n"));
				return MAD_FAIL;
			}
			if((status = mdPtp1722TimeSet_port(dev,port,ts_1722)) != MAD_OK)
			{
				MAD_DBG_INFO(("Failed writing 1722 ToD.\n"));
				return MAD_FAIL;
			}			
			if((status = mdPtpTODCompSet_port(dev,port,comp)) != MAD_OK)
			{
				MAD_DBG_INFO(("Failed writing ToD compensation.\n"));
				return MAD_FAIL;
			}
			if((status =  mdPtpTODOpCtrlSet_port(dev,port,&todOpCtrl)) != MAD_OK)
			{
				MAD_DBG_INFO(("Failed writing ToD operation control.\n"));
				return MAD_FAIL;
			}
			MSG_PRINT(("load timeArr: %d, domain: %d, todNano: %d, todSec: %d, todTopSec: %d, \
				  nano_1722: %d, todComp: %d, todLoadPt: %x", todOpCtrl.ptpOpCtrlTimeArray, \
				  todOpCtrl.ptpOpCtrlDomainNum, ts.nanoseconds, ts.seconds, ts.topseconds, \
				  ts_1722, comp, loadPoint));		
			if((status = mdPtpGetPTPGlobalTime_port(dev,port,&glbTime)) != MAD_OK)
			{
				MAD_DBG_INFO(("Failed reading PTP Global Time.\n"));
				return MAD_FAIL;
			}
			MSG_PRINT(("global time: %d, todLoadPt: 0x%x.\n", glbTime, loadPoint));
		}

		for (j = 0; j < 4; j++)
		{
			todOpCtrl.ptpOpCtrlTimeArray = timeArr[j];
			todOpCtrl.ptpOpCtrlDomainNum = domain[j];	
			MSG_PRINT(("Capture timeArray %d, domain %d, todOp 4.\n", todOpCtrl.ptpOpCtrlTimeArray, todOpCtrl.ptpOpCtrlDomainNum));			
			if((status = mdPtpGetPTPGlobalTime_port(dev,port,&glbTime)) != MAD_OK)
			{
				MAD_DBG_INFO(("Failed reading PTP Global Time.\n"));
				return MAD_FAIL;
			}
			loadPoint = glbTime;
			MSG_PRINT(("global time %d, todLoadPt %d.\n", glbTime, loadPoint));
			todOpCtrl.ptpOpCtrlTodOp = 4;
			if((status =  mdPtpTODOpCtrlSet_port(dev,port,&todOpCtrl)) != MAD_OK)
			{
				MAD_DBG_INFO(("Failed writing ToD operation control.\n"));
				return MAD_FAIL;
			}
			if((status = mdPtpTODTimeGet_port(dev,port,&ts)) != MAD_OK)
			{
				MAD_DBG_INFO(("Failed reading ToD.\n"));
				return MAD_FAIL;
			}
			if((status = mdPtp1722TimeGet_port(dev,port,&ts_1722)) != MAD_OK)
			{
				MAD_DBG_INFO(("Failed reading 1722 ToD.\n"));
				return MAD_FAIL;
			}
			if((status = mdPtpTODCompGet_port(dev,port,&comp)) != MAD_OK)
			{
				MAD_DBG_INFO(("Failed reading ToD compensation.\n"));
				return MAD_FAIL;
			}
			MSG_PRINT(("todNano: %d, todSec: %d, todTopSec: %d, nano_1722: %d, todComp: %d", \
				ts.nanoseconds, ts.seconds, ts.topseconds, ts_1722, comp));
		}
	}
	
	madWait(2000);

	for (i = 0; i < 4; i++)
	{
		mode.grandMstr = mstr[i];
		mode.altScheme = altScheme[i];
		/* clear PTP action table */
		/*
		status |= mdPtpSetIngActVec_port(dev,port,MAD_PTP_IEEE_1588,0);
		status |= mdPtpSetIngActVec_port(dev,port,MAD_PTP_IEEE_802_1AS,0);
		*/
		status |= mdPtpSetEgActVec_port(dev,port,MAD_PTP_IEEE_1588,0);
		status |= mdPtpSetEgActVec_port(dev,port,MAD_PTP_IEEE_802_1AS,0);
		if (status != MAD_OK)
		{
			MAD_DBG_INFO(("Failed set action vectors"));
			return MAD_FAIL;
		}

		MSG_PRINT(("Clear all Action Table!!!\n"));
		mode.PTPMode = 0;
		mode.oneStep = 1;
		if((status = mdPtpModeSet_port(dev,port,&mode)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed set PTP mode.\n"));
			return MAD_FAIL;
		}
		if((status = mdPtpModeGet_port(dev,port,&mode)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed get PTP mode.\n"));
			return MAD_FAIL;
		}
		MSG_PRINT(("Port %d Set PTP Mode to %d.\n", (int)port, mode.PTPMode));
		MSG_PRINT(("Port %d Set PTP OneStep to %d.\n", (int)port, mode.oneStep));
		MSG_PRINT(("Port %d Set PTP GrandMaster to %d.\n", (int)port, mode.grandMstr));
		MSG_PRINT(("Port %d Set PTP AltScheme to %d.\n", (int)port, mode.altScheme));
		MSG_PRINT(("Port %d Dump PTP Action Table.\n", (int)port));

		/* dump PTP action table*/
		if((status = mdPtpGetEgActVec_port(dev,port,MAD_PTP_IEEE_1588,&vec_1588)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed get PTP 1588 egress action vector.\n"));
			return MAD_FAIL;
		}
		if((status = mdPtpGetEgActVec_port(dev,port,MAD_PTP_IEEE_802_1AS,&vec_802_1AS)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed get PTP 802.1AS egress action vector.\n"));
			return MAD_FAIL;
		}

		format.intPTPTime = 0;
		if((status = mdPtpSetExtTimeFormat_port(dev,port,&format)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed set PTP external time format.\n"));
			return MAD_FAIL;
		}
		if((status = mdPtpGetExtTimeFormat_port(dev,port,&format)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed get PTP external time format.\n"));
			return MAD_FAIL;
		}
		MSG_PRINT(("Port %d Set PTPIntTime to %d.\n", (int)port, format.intPTPTime));
	}
	
	madWait(1000);

	for (i = 0; i < pListLen; i++)
	{
		sPort = srcPortList[i];
		dPort = destPortList[i];
		MSG_PRINT(("ngress Port: %d, Egress Port: %d", sPort, dPort));

		ptpConfig.ptpEType = 0x88F7;
		ptpConfig.ptpPortConfig[sPort].hwAccPtp = 1;
		ptpConfig.ptpPortConfig[dPort].hwAccPtp = 0;

		if((status = mdPtpSetConfig_port(dev,sPort,&ptpConfig)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed set PTP config.\n"));
			return MAD_FAIL;
		}
		if((status = mdPtpSetConfig_port(dev,dPort,&ptpConfig)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed set PTP config.\n"));
			return MAD_FAIL;
		}

		MSG_PRINT(("Get PTP Configuration: \n"));
		if((status = mdPtpGetConfig_port(dev,sPort,&ptpConfig)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed get PTP config.\n"));
			return MAD_FAIL;
		}
		MSG_PRINT(("Get_PTPEtype port(%d) 0x%x\n", sPort, ptpConfig.ptpEType));
		MSG_PRINT(("Enable Ingress port(%d) HWAccel, Get_PTPHWAccel %d\n", sPort, ptpConfig.ptpPortConfig[sPort].hwAccPtp));

		if((status = mdPtpGetConfig_port(dev,dPort,&ptpConfig)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed get PTP config.\n"));
			return MAD_FAIL;
		}
		MSG_PRINT(("Enable Egress port(%d) HWAccel, Get_PTPHWAccel %d\n", dPort, ptpConfig.ptpPortConfig[dPort].hwAccPtp));

		for (j = 0; j < 22; j++)
		{
			ptpConfig.transSpec = transSpecList[j];
			ptpConfig.msgIdTSEn = 0xFFFF;
			if((status = mdPtpSetConfig_port(dev,sPort,&ptpConfig)) != MAD_OK)
			{
				MAD_DBG_INFO(("Failed set PTP config.\n"));
				return MAD_FAIL;
			}

			if((status = mdPtpSetConfig_port(dev,dPort,&ptpConfig)) != MAD_OK)
			{
				MAD_DBG_INFO(("Failed set PTP config.\n"));
				return MAD_FAIL;
			}

			MSG_PRINT(("Set PTP Etype %x transSpec %d.\n", 0x88F7, transSpecList[j]));

			if((status = mdPtpGetConfig_port(dev,sPort,&ptpConfig)) != MAD_OK)
			{
				MAD_DBG_INFO(("Failed get PTP config.\n"));
				return MAD_FAIL;
			}

			MSG_PRINT(("Get_PTPPortTransSpec %d.\n", ptpConfig.transSpec));

			/* Clear PTP Status register */
			status |= mdPtpResetTimeStamp(dev,sPort,MAD_PTP_ARR0_TIME);
			status |= mdPtpResetTimeStamp(dev,sPort,MAD_PTP_ARR1_TIME);
			status |= mdPtpResetTimeStamp(dev,sPort,MAD_PTP_DEP_TIME);
			status |= mdPtpResetTimeStamp(dev,dPort,MAD_PTP_ARR0_TIME);
			status |= mdPtpResetTimeStamp(dev,dPort,MAD_PTP_ARR1_TIME);
			status |= mdPtpResetTimeStamp(dev,dPort,MAD_PTP_DEP_TIME);

			if(status != MAD_OK)
			{
				MAD_DBG_INFO(("Failed clear PTP status.\n"));
				return MAD_FAIL;
			}
			
			/* collect PTP status data */
			if((status = mdPtpGetTimeStamped(dev,sPort,MAD_PTP_ARR0_TIME,&arr0)) != MAD_OK)
			{
				MAD_DBG_INFO(("Failed get PTP time stamp value.\n"));
				return MAD_FAIL;
			}
			if((status = mdPtpGetTimeStamped(dev,sPort,MAD_PTP_ARR1_TIME,&arr1)) != MAD_OK)
			{
				MAD_DBG_INFO(("Failed get PTP time stamp value.\n"));
				return MAD_FAIL;
			}
			if((status = mdPtpGetTimeStamped(dev,dPort,MAD_PTP_DEP_TIME,&dep)) != MAD_OK)
			{
				MAD_DBG_INFO(("Failed get PTP time stamp value.\n"));
				return MAD_FAIL;
			}

			MSG_PRINT(("\nPTPArr0IntStatus %d.\n", (int)arr0.status));
			MSG_PRINT(("PTPArr0TimeValid %d.\n", (int)arr0.isValid));
			MSG_PRINT(("PTPArr0SeqId %d.\n", (int)arr0.ptpSeqId));

			MSG_PRINT(("PTPDepIntStatus %d.\n", (int)dep.status));
			MSG_PRINT(("PTPDepTimeValid %d.\n", (int)dep.isValid));
			MSG_PRINT(("PTPDepSeqId %d.\n", (int)dep.ptpSeqId));
		}
	}

	/* clean up. ATU_FlushAll */
	for (i = 0; i < pListLen; i++)
	{
		sPort = srcPortList[i];
		dPort = destPortList[i];
		ptpConfig.ptpEType = 0x88F7;
		ptpConfig.transSpec = 1;
		ptpConfig.msgIdTSEn = 0;

		if((status = mdPtpSetConfig_port(dev,sPort,&ptpConfig)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed set PTP config.\n"));
			return MAD_FAIL;
		}
		if((status = mdPtpSetConfig_port(dev,dPort,&ptpConfig)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed set PTP config.\n"));
			return MAD_FAIL;
		}
	}

	return MAD_OK;
}


MAD_STATUS samplePTP_BC_OneStep_EgrAct_msgId_0_1588_TwoStepFlag(MAD_DEV *dev)
{
    MAD_STATUS				status;
	MAD_LPORT				port, sPort, dPort;
	MAD_LPORT				portList[] = {0};
	MAD_LPORT				srcPortList[] = {0};
	MAD_LPORT				destPortList[] = {0};
	int						pListLen = 1;
	MAD_PTP_TOD_OP_CTRL		todOpCtrl;
	MAD_U8					timeArr[] = {0, 1, 2, 3};
	MAD_U8					domain[] = {0, 1, 2, 3};
	MAD_PTP_TS_STRUCT		ts;
	unsigned long long		ts_1722;
	MAD_U32					comp;
	MAD_U32					loadPoint;
	MAD_U32					glbTime;
	MAD_PTP_MODE_CONFIG		mode;
	MAD_U8					mstr[] = {0, 0, 1, 1};
	MAD_U8					altScheme[] = {0, 1, 0, 1};
	MAD_PTP_EXTERNAL_TIME_FORM	format;
	MAD_PTP_EG_ACT_VEC			vec_1588,vec_802_1AS;
	MAD_PTP_CONFIG			ptpConfig;
	MAD_PTP_SPEC			transSpecList[] = {0,0,0,0};
	MAD_PTP_TS_STATUS		arr0,arr1,dep;
	int						lp;
	int						i,j;

	if (srcPortList[0] == destPortList[0])
		lp = 1;
	else
		lp = 0;

	for (i = 0; i < pListLen; i++)
	{
		port = portList[i];
		/* Init Phy */
		if((status = madHwWritePagedPhyReg(dev,port,18,27,0)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed writing register.\n"));
			return MAD_FAIL;
		}
		if((status = madHwWritePagedPhyReg(dev,port,0,0,0x9140)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed writing register.\n"));
			return MAD_FAIL;
		}
		if((status = madHwWritePagedPhyReg(dev,0,2,21,0x1046)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed writing register.\n"));
			return MAD_FAIL;
		}
		if((status = madHwWritePagedPhyReg(dev,0,0,0,0x9140)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed writing register.\n"));
			return MAD_FAIL;
		}

		/* Enable PTP */
		if((status = mdPtpSetEnable_port(dev,port,MAD_TRUE)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed power up PTP.\n"));
			return MAD_FAIL;
		}

		/* Set MAC Loopback */
		if((status = mdDiagSetLineLoopback(dev,port,MAD_TRUE)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed set MAC loopback.\n"));
			return MAD_FAIL;
		}
	}

	MSG_PRINT(("Active used domain related time array.\n"));
	

	for (i = 0; i < pListLen; i++)
	{
		port = portList[i];
		todOpCtrl.ptpOpCtrlClkValid = 1;
		todOpCtrl.ptpOpCtrlTodOp = 3;	/* clean */
		for (j = 0; j < 4; j++)
		{
			todOpCtrl.ptpOpCtrlTimeArray = timeArr[j];
			todOpCtrl.ptpOpCtrlDomainNum = domain[j];

			ts.nanoseconds = rand()*0xFFFFFFFF;
			ts.seconds = rand()*0xFFFFFFFF;
			ts.topseconds = rand()*0xFFFF;
			ts_1722 = rand()*0xFFFFFFFF;
			comp = rand()*0xFFFFFFFF;
			loadPoint = rand()*0xFFFFFFFF;

			ts.nanoseconds = 0;
			ts.seconds = 0;
			ts.topseconds = 0;
			ts_1722 = 0;
			comp = 0;
			loadPoint = 0xFFFFFFFF;

			MSG_PRINT(("Configure timeArr for all ports.\n"));
			if((status = mdPtpGetPTPGlobalTime_port(dev,port,&glbTime)) != MAD_OK)
			{
				MAD_DBG_INFO(("Failed reading PTP Global Time.\n"));
				return MAD_FAIL;
			}
			loadPoint = glbTime+0x6000000;
			MSG_PRINT(("global time: %d, todLoadPt: 0x%x.\n", glbTime, loadPoint));
			if((status = mdPtpTODTimeSet_port(dev,port,&ts)) != MAD_OK)
			{
				MAD_DBG_INFO(("Failed writing ToD.\n"));
				return MAD_FAIL;
			}
			if((status = mdPtp1722TimeSet_port(dev,port,ts_1722)) != MAD_OK)
			{
				MAD_DBG_INFO(("Failed writing 1722 ToD.\n"));
				return MAD_FAIL;
			}			
			if((status = mdPtpTODCompSet_port(dev,port,comp)) != MAD_OK)
			{
				MAD_DBG_INFO(("Failed writing ToD compensation.\n"));
				return MAD_FAIL;
			}
			if((status =  mdPtpTODOpCtrlSet_port(dev,port,&todOpCtrl)) != MAD_OK)
			{
				MAD_DBG_INFO(("Failed writing ToD operation control.\n"));
				return MAD_FAIL;
			}
			MSG_PRINT(("load timeArr: %d, domain: %d, todNano: %d, todSec: %d, todTopSec: %d, \
				  nano_1722: %d, todComp: %d, todLoadPt: %x", todOpCtrl.ptpOpCtrlTimeArray, \
				  todOpCtrl.ptpOpCtrlDomainNum, ts.nanoseconds, ts.seconds, ts.topseconds, \
				  ts_1722, comp, loadPoint));		
			if((status = mdPtpGetPTPGlobalTime_port(dev,port,&glbTime)) != MAD_OK)
			{
				MAD_DBG_INFO(("Failed reading PTP Global Time.\n"));
				return MAD_FAIL;
			}
			MSG_PRINT(("global time: %d, todLoadPt: 0x%x.\n", glbTime, loadPoint));
		}

		for (j = 0; j < 4; j++)
		{
			todOpCtrl.ptpOpCtrlTimeArray = timeArr[j];
			todOpCtrl.ptpOpCtrlDomainNum = domain[j];	
			MSG_PRINT(("Capture timeArray %d, domain %d, todOp 4.\n", todOpCtrl.ptpOpCtrlTimeArray, todOpCtrl.ptpOpCtrlDomainNum));			
			if((status = mdPtpGetPTPGlobalTime_port(dev,port,&glbTime)) != MAD_OK)
			{
				MAD_DBG_INFO(("Failed reading PTP Global Time.\n"));
				return MAD_FAIL;
			}
			loadPoint = glbTime;
			MSG_PRINT(("global time %d, todLoadPt %d.\n", glbTime, loadPoint));
			todOpCtrl.ptpOpCtrlTodOp = 4;
			if((status =  mdPtpTODOpCtrlSet_port(dev,port,&todOpCtrl)) != MAD_OK)
			{
				MAD_DBG_INFO(("Failed writing ToD operation control.\n"));
				return MAD_FAIL;
			}
			if((status = mdPtpTODTimeGet_port(dev,port,&ts)) != MAD_OK)
			{
				MAD_DBG_INFO(("Failed reading ToD.\n"));
				return MAD_FAIL;
			}
			if((status = mdPtp1722TimeGet_port(dev,port,&ts_1722)) != MAD_OK)
			{
				MAD_DBG_INFO(("Failed reading 1722 ToD.\n"));
				return MAD_FAIL;
			}
			if((status = mdPtpTODCompGet_port(dev,port,&comp)) != MAD_OK)
			{
				MAD_DBG_INFO(("Failed reading ToD compensation.\n"));
				return MAD_FAIL;
			}
			MSG_PRINT(("todNano: %d, todSec: %d, todTopSec: %d, nano_1722: %d, todComp: %d", \
				ts.nanoseconds, ts.seconds, ts.topseconds, ts_1722, comp));
		}
	}
	
	madWait(2000);

	for (i = 0; i < 4; i++)
	{
		mode.grandMstr = mstr[i];
		mode.altScheme = altScheme[i];
		/* clear PTP action table */
		/*
		status |= mdPtpSetIngActVec_port(dev,port,MAD_PTP_IEEE_1588,0);
		status |= mdPtpSetIngActVec_port(dev,port,MAD_PTP_IEEE_802_1AS,0);
		*/
		status |= mdPtpSetEgActVec_port(dev,port,MAD_PTP_IEEE_1588,0);
		status |= mdPtpSetEgActVec_port(dev,port,MAD_PTP_IEEE_802_1AS,0);
		if (status != MAD_OK)
		{
			MAD_DBG_INFO(("Failed set action vectors"));
			return MAD_FAIL;
		}

		MSG_PRINT(("Clear all Action Table!!!\n"));
		mode.PTPMode = 0;
		mode.oneStep = 1;
		if((status = mdPtpModeSet_port(dev,port,&mode)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed set PTP mode.\n"));
			return MAD_FAIL;
		}
		if((status = mdPtpModeGet_port(dev,port,&mode)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed get PTP mode.\n"));
			return MAD_FAIL;
		}
		MSG_PRINT(("Port %d Set PTP Mode to %d.\n", (int)port, mode.PTPMode));
		MSG_PRINT(("Port %d Set PTP OneStep to %d.\n", (int)port, mode.oneStep));
		MSG_PRINT(("Port %d Set PTP GrandMaster to %d.\n", (int)port, mode.grandMstr));
		MSG_PRINT(("Port %d Set PTP AltScheme to %d.\n", (int)port, mode.altScheme));
		MSG_PRINT(("Port %d Dump PTP Action Table.\n", (int)port));

		/* dump PTP action table*/
		if((status = mdPtpGetEgActVec_port(dev,port,MAD_PTP_IEEE_1588,&vec_1588)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed get PTP 1588 egress action vector.\n"));
			return MAD_FAIL;
		}
		if((status = mdPtpGetEgActVec_port(dev,port,MAD_PTP_IEEE_802_1AS,&vec_802_1AS)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed get PTP 802.1AS egress action vector.\n"));
			return MAD_FAIL;
		}

		format.intPTPTime = 0;
		if((status = mdPtpSetExtTimeFormat_port(dev,port,&format)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed set PTP external time format.\n"));
			return MAD_FAIL;
		}
		if((status = mdPtpGetExtTimeFormat_port(dev,port,&format)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed get PTP external time format.\n"));
			return MAD_FAIL;
		}
		MSG_PRINT(("Port %d Set PTPIntTime to %d.\n", (int)port, format.intPTPTime));
	}

	for (i = 0; i < pListLen; i++)
	{
		sPort = srcPortList[i];
		dPort = destPortList[i];
		MSG_PRINT(("ngress Port: %d, Egress Port: %d", sPort, dPort));

		ptpConfig.ptpEType = 0x88F7;
		ptpConfig.ptpPortConfig[sPort].hwAccPtp = 0;
		ptpConfig.ptpPortConfig[dPort].hwAccPtp = 1;

		if((status = mdPtpSetConfig_port(dev,sPort,&ptpConfig)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed set PTP config.\n"));
			return MAD_FAIL;
		}
		if((status = mdPtpSetConfig_port(dev,dPort,&ptpConfig)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed set PTP config.\n"));
			return MAD_FAIL;
		}

		MSG_PRINT(("Get PTP Configuration: \n"));
		if((status = mdPtpGetConfig_port(dev,sPort,&ptpConfig)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed get PTP config.\n"));
			return MAD_FAIL;
		}
		MSG_PRINT(("Get_PTPEtype port(%d) 0x%x\n", sPort, ptpConfig.ptpEType));
		MSG_PRINT(("Enable Ingress port(%d) HWAccel, Get_PTPHWAccel %d\n", sPort, ptpConfig.ptpPortConfig[sPort].hwAccPtp));

		if((status = mdPtpGetConfig_port(dev,dPort,&ptpConfig)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed get PTP config.\n"));
			return MAD_FAIL;
		}
		MSG_PRINT(("Enable Egress port(%d) HWAccel, Get_PTPHWAccel %d\n", dPort, ptpConfig.ptpPortConfig[dPort].hwAccPtp));

		for (j = 0; j < 4; j++)
		{
			ptpConfig.transSpec = transSpecList[j];
			ptpConfig.msgIdTSEn = 0xFFFF;
			if((status = mdPtpSetConfig_port(dev,sPort,&ptpConfig)) != MAD_OK)
			{
				MAD_DBG_INFO(("Failed set PTP config.\n"));
				return MAD_FAIL;
			}

			if((status = mdPtpSetConfig_port(dev,dPort,&ptpConfig)) != MAD_OK)
			{
				MAD_DBG_INFO(("Failed set PTP config.\n"));
				return MAD_FAIL;
			}

			MSG_PRINT(("Set PTP Etype %x transSpec %d.\n", 0x88F7, transSpecList[j]));

			if((status = mdPtpGetConfig_port(dev,sPort,&ptpConfig)) != MAD_OK)
			{
				MAD_DBG_INFO(("Failed get PTP config.\n"));
				return MAD_FAIL;
			}

			MSG_PRINT(("Get_PTPPortTransSpec %d.\n", ptpConfig.transSpec));

			/* Clear PTP Status register */
			status |= mdPtpResetTimeStamp(dev,sPort,MAD_PTP_ARR0_TIME);
			status |= mdPtpResetTimeStamp(dev,sPort,MAD_PTP_ARR1_TIME);
			status |= mdPtpResetTimeStamp(dev,sPort,MAD_PTP_DEP_TIME);
			status |= mdPtpResetTimeStamp(dev,dPort,MAD_PTP_ARR0_TIME);
			status |= mdPtpResetTimeStamp(dev,dPort,MAD_PTP_ARR1_TIME);
			status |= mdPtpResetTimeStamp(dev,dPort,MAD_PTP_DEP_TIME);

			if(status != MAD_OK)
			{
				MAD_DBG_INFO(("Failed clear PTP status.\n"));
				return MAD_FAIL;
			}
			
			/* collect PTP status data */
			if((status = mdPtpGetTimeStamped(dev,sPort,MAD_PTP_ARR0_TIME,&arr0)) != MAD_OK)
			{
				MAD_DBG_INFO(("Failed get PTP time stamp value.\n"));
				return MAD_FAIL;
			}
			if((status = mdPtpGetTimeStamped(dev,sPort,MAD_PTP_ARR1_TIME,&arr1)) != MAD_OK)
			{
				MAD_DBG_INFO(("Failed get PTP time stamp value.\n"));
				return MAD_FAIL;
			}
			if((status = mdPtpGetTimeStamped(dev,dPort,MAD_PTP_DEP_TIME,&dep)) != MAD_OK)
			{
				MAD_DBG_INFO(("Failed get PTP time stamp value.\n"));
				return MAD_FAIL;
			}

			MSG_PRINT(("\nPTPArr0IntStatus %d.\n", (int)arr0.status));
			MSG_PRINT(("PTPArr0TimeValid %d.\n", (int)arr0.isValid));
			MSG_PRINT(("PTPArr0SeqId %d.\n", (int)arr0.ptpSeqId));

			MSG_PRINT(("PTPDepIntStatus %d.\n", (int)dep.status));
			MSG_PRINT(("PTPDepTimeValid %d.\n", (int)dep.isValid));
			MSG_PRINT(("PTPDepSeqId %d.\n", (int)dep.ptpSeqId));
		}
	}

	/* clean up. ATU_FlushAll */
	for (i = 0; i < pListLen; i++)
	{
		sPort = srcPortList[i];
		dPort = destPortList[i];
		ptpConfig.ptpEType = 0x88F7;
		ptpConfig.transSpec = 1;
		ptpConfig.msgIdTSEn = 0;

		if((status = mdPtpSetConfig_port(dev,sPort,&ptpConfig)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed set PTP config.\n"));
			return MAD_FAIL;
		}
		if((status = mdPtpSetConfig_port(dev,dPort,&ptpConfig)) != MAD_OK)
		{
			MAD_DBG_INFO(("Failed set PTP config.\n"));
			return MAD_FAIL;
		}
	}

	return MAD_OK;
}



/* test PTP */
MAD_U32 testPTP(MAD_DEV *dev)
{
	MAD_STATUS status, testResult;
    MAD_U32 testResults = 0;
	TEST_API testAPI;
	MAD_PTP_CONFIG	orgCfg, inCfg, outCfg;
	MAD_PTP_GLOBAL_CONFIG orgGlbCfg, inGlbCfg, outGlbCfg;
	MAD_PTP_PORT_CONFIG orgPortCfg, inPortCfg, outPortCfg;
	MAD_PTP_TOD_OP_CTRL orgTodOp, inTodOp, outTodOp;
	MAD_PTP_PULSE_STRUCT orgPulse, inPulse, outPulse;
	MAD_PTP_TS_STRUCT orgTs, inTs, outTs;
	MAD_PTP_MODE_CONFIG orgPtpMode, inPtpMode, outPtpMode;
	MAD_PTP_EXTERNAL_TIME_FORM orgExtTime, inExtTime, outExtTime;
	MAD_PTP_TS_MODE	orgTsMode, inTsMode, outTsMode;
	MAD_LPORT		port;
	MAD_PTP_SPEC		spec;
    MAD_PTP_EG_ACT_VEC		orgVec, inVec, outVec;

	status = MAD_OK;

	/*
	 * PTP configuration test
	 */
	MSG_PRINT(("******** PTP API test ********\n"));
	MSG_PRINT(("PTP configuration API test "));


	if (status = mdPtpGetConfig(dev, &orgCfg) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on mdPtpGetConfig call\n"));
        return testResults;
	}

	inCfg.ptpEType = 0xffff;
	inCfg.msgIdTSEn = 0xffff;
	inCfg.tsArrPtr = 0xffff;

	inCfg.ptpArrIntEn = (1 << (dev->numOfPorts)) - 1;
	inCfg.ptpDepIntEn = (1 << (dev->numOfPorts)) - 1;
	inCfg.transSpec = 1;
	inCfg.disTSOverwrite = 1;

	for(port = 0; port < dev->numOfPorts; port++)
		inCfg.ptpPortConfig[port].arrTSMode = 1;

	if (status = mdPtpSetConfig(dev, &inCfg) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on mdPtpSetConfig call\n"));
        return testResults;
	}

	if (status = mdPtpGetConfig(dev, &outCfg) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on mdPtpGetConfig call\n"));
        return testResults;
	}

	if ((inCfg.ptpEType != outCfg.ptpEType) ||
		(inCfg.msgIdTSEn != outCfg.msgIdTSEn) ||
		(inCfg.tsArrPtr != outCfg.tsArrPtr) ||
		(inCfg.ptpArrIntEn != outCfg.ptpArrIntEn) ||
		(inCfg.ptpDepIntEn != outCfg.ptpDepIntEn) ||
		(inCfg.transSpec != outCfg.transSpec) ||
		(inCfg.disTSOverwrite != outCfg.disTSOverwrite))
	{
		testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
#if 1
        MSG_PRINT(("Data Mismatch (%x,%x,%x,%x,%x,%x,%x), expect (%x,%x,%x,%x,%x,%x,%x)\n",
        outCfg.ptpEType, outCfg.msgIdTSEn, outCfg.tsArrPtr, outCfg.ptpArrIntEn, 
		outCfg.ptpDepIntEn, outCfg.transSpec, outCfg.disTSOverwrite,
        inCfg.ptpEType, inCfg.msgIdTSEn, inCfg.tsArrPtr, inCfg.ptpArrIntEn, 
		inCfg.ptpDepIntEn, inCfg.transSpec, inCfg.disTSOverwrite));
#endif
        return testResults;
	}

//	if (IS_IN_DEV_GROUP(dev, DEV_ARRV_TS_MODE))
	{
		for(port = 0; port < dev->numOfPorts; port++)
		{
			if (outCfg.ptpPortConfig[port].arrTSMode !=  inCfg.ptpPortConfig[port].arrTSMode)
			{
				testResults |= 1 << MAD_FAIL;
				testMADDisplayStatus(MAD_FAIL);
				MSG_PRINT(("Port %d Data Mismatch (%d), expect (%d)\n",
				port, outCfg.ptpPortConfig[port].arrTSMode, inCfg.ptpPortConfig[port].arrTSMode));
				return testResults;
			}
		}
	}

	inCfg.ptpEType = 0;
	inCfg.msgIdTSEn = 0;
	inCfg.tsArrPtr = 0;

	inCfg.ptpArrIntEn = 0;
	inCfg.ptpDepIntEn = 0;
	inCfg.transSpec = 0;
	inCfg.disTSOverwrite = 0;

	for(port = 0; port < dev->numOfPorts; port++)
		inCfg.ptpPortConfig[port].arrTSMode = 0;

	if (status = mdPtpSetConfig(dev, &inCfg) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on mdPtpSetConfig call\n"));
        return testResults;
	}

	if (status = mdPtpGetConfig(dev, &outCfg) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on mdPtpGetConfig call\n"));
        return testResults;
	}

	if ((inCfg.ptpEType != outCfg.ptpEType) ||
		(inCfg.msgIdTSEn != outCfg.msgIdTSEn) ||
		(inCfg.tsArrPtr != outCfg.tsArrPtr) ||
		(inCfg.ptpArrIntEn != outCfg.ptpArrIntEn) ||
		(inCfg.ptpDepIntEn != outCfg.ptpDepIntEn) ||
		(inCfg.transSpec != outCfg.transSpec) ||
		(inCfg.disTSOverwrite != outCfg.disTSOverwrite))
	{
		testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d,%d,%d,%d,%d,%d,%d), expect (%d,%d,%d,%d,%d,%d,%d)\n",
        outCfg.ptpEType, outCfg.msgIdTSEn, outCfg.tsArrPtr, outCfg.ptpArrIntEn, 
		outCfg.ptpDepIntEn, outCfg.transSpec, outCfg.disTSOverwrite,
        inCfg.ptpEType, inCfg.msgIdTSEn, inCfg.tsArrPtr, inCfg.ptpArrIntEn, 
		inCfg.ptpDepIntEn, inCfg.transSpec, inCfg.disTSOverwrite));
        return testResults;
	}

//	if (IS_IN_DEV_GROUP(dev, DEV_ARRV_TS_MODE))
	{
		for(port = 0; port < dev->numOfPorts; port++)
		{
			if (outCfg.ptpPortConfig[port].arrTSMode !=  inCfg.ptpPortConfig[port].arrTSMode)
			{
				testResults |= 1 << MAD_FAIL;
				testMADDisplayStatus(MAD_FAIL);
				MSG_PRINT(("Port %d Data Mismatch (%d), expect (%d)\n",
				port, outCfg.ptpPortConfig[port].arrTSMode, inCfg.ptpPortConfig[port].arrTSMode));
				return testResults;
			}
		}
	}

	if (status = mdPtpSetConfig(dev, &orgCfg) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on mdPtpSetConfig call\n"));
        return testResults;
	}

	MSG_PRINT(("PTP configuration API test "));
    testMADDisplayStatus(status);


	/*
	 * PTP Global Configuration test
	 */
	MSG_PRINT(("PTP Global Configuration API test "));

	if (status = mdPtpGetGlobalConfig(dev, &orgGlbCfg) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpGetGlobalConfig call\n"));
        return testResults;
	}

	inGlbCfg.ptpEType = 0xffff;
	inGlbCfg.msgIdTSEn = 0xffff;
	inGlbCfg.tsArrPtr = 0xffff;

	if (status = mdPtpSetGlobalConfig(dev, &inGlbCfg) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpSetGlobalConfig call\n"));
        return testResults;
	}

	if (status = mdPtpGetGlobalConfig(dev, &outGlbCfg) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpGetGlobalConfig call\n"));
        return testResults;
	}

	if ((inGlbCfg.ptpEType != outGlbCfg.ptpEType) ||
		(inGlbCfg.msgIdTSEn != outGlbCfg.msgIdTSEn) ||
		(inGlbCfg.tsArrPtr != outGlbCfg.tsArrPtr))
	{
		testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d,%d,%d), expect (%d,%d,%d)\n",
        outGlbCfg.ptpEType, outGlbCfg.msgIdTSEn, outGlbCfg.tsArrPtr,
        inGlbCfg.ptpEType, inGlbCfg.msgIdTSEn, inGlbCfg.tsArrPtr));
        return testResults;
	}

	inGlbCfg.ptpEType = 0;
	inGlbCfg.msgIdTSEn = 0;
	inGlbCfg.tsArrPtr = 0;

	if (status = mdPtpSetGlobalConfig(dev, &inGlbCfg) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpSetGlobalConfig call\n"));
        return testResults;
	}

	if (status = mdPtpGetGlobalConfig(dev, &outGlbCfg) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpGetGlobalConfig call\n"));
        return testResults;
	}

	if ((inGlbCfg.ptpEType != outGlbCfg.ptpEType) ||
		(inGlbCfg.msgIdTSEn != outGlbCfg.msgIdTSEn) ||
		(inGlbCfg.tsArrPtr != outGlbCfg.tsArrPtr))
	{
		testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d,%d,%d), expect (%d,%d,%d)\n",
        outGlbCfg.ptpEType, outGlbCfg.msgIdTSEn, outGlbCfg.tsArrPtr,
        inGlbCfg.ptpEType, inGlbCfg.msgIdTSEn, inGlbCfg.tsArrPtr));
        return testResults;
	}

	if (status = mdPtpSetGlobalConfig(dev, &orgGlbCfg) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpSetGlobalConfig call\n"));
        return testResults;
	}
	
	MSG_PRINT(("PTP Global Configuration API test "));
    testMADDisplayStatus(status);

	/*
	 * PTP Port Configuration test
	 */
	MSG_PRINT(("PTP Port Configuration API test "));

	port = 0;

	if (status = mdPtpGetPortConfig(dev, port, &orgPortCfg) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpGetPortConfig call\n"));
        return testResults;
	}

	inPortCfg.transSpec = 1;
	inPortCfg.disTSpec = 1;
	inPortCfg.disTSOverwrite = 1;
	inPortCfg.ipJump = 0x3f;
	inPortCfg.etJump = 0x1f;
	inPortCfg.ptpArrIntEn = 1;
	inPortCfg.ptpDepIntEn = 1;
	inPortCfg.arrTSMode = 1;
	inPortCfg.filterAct = 1;
	inPortCfg.hwAccPtp = 1;
	inPortCfg.kpFramwSa = 1;
	inPortCfg.fullCheck = 1;
	inPortCfg.noCorrupt = 1;
	inPortCfg.extHwAcc = 1;
	inPortCfg.arrLedCtrl = 0xff;
	inPortCfg.depLedCtrl = 0xff;

	if (status = mdPtpSetPortConfig(dev, port, &inPortCfg) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpSetPortConfig call\n"));
        return testResults;
	}

	if (status = mdPtpGetPortConfig(dev, port, &outPortCfg) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpGetPortConfig call\n"));
        return testResults;
	}
	if ((inPortCfg.transSpec != outPortCfg.transSpec) ||
		(inPortCfg.disTSpec != outPortCfg.disTSpec) ||
		(inPortCfg.disTSOverwrite != outPortCfg.disTSOverwrite) ||
		(inPortCfg.ipJump != outPortCfg.ipJump) ||
		(inPortCfg.etJump != outPortCfg.etJump) ||
		(inPortCfg.ptpArrIntEn != outPortCfg.ptpArrIntEn) ||
		(inPortCfg.ptpDepIntEn != outPortCfg.ptpDepIntEn))
	{
		testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d,%d,%d,%d,%d,%d,%d), \
			expect (%d,%d,%d,%d,%d,%d,%d)\n",
        outPortCfg.transSpec, outPortCfg.disTSpec, outPortCfg.disTSOverwrite, 
		outPortCfg.ipJump, outPortCfg.etJump, outPortCfg.ptpArrIntEn, 
		outPortCfg.ptpDepIntEn,
		inPortCfg.transSpec, inPortCfg.disTSpec, inPortCfg.disTSOverwrite, 
		inPortCfg.ipJump, inPortCfg.etJump, inPortCfg.ptpArrIntEn, 
		inPortCfg.ptpDepIntEn));
        return testResults;
	}
	
	
//	if (IS_IN_DEV_GROUP(dev, DEV_ARRV_TS_MODE))
    {
      if (inPortCfg.arrTSMode != outPortCfg.arrTSMode)
	  {
		testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d), expect (%d)\n",outPortCfg.arrTSMode,inPortCfg.arrTSMode));
		return testResults;
	  }
    }
	
//    if (IS_IN_DEV_GROUP(dev, DEV_LED_FILTER))
    {
      if (inPortCfg.arrTSMode != outPortCfg.arrTSMode)
	  {
		testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d), expect (%d)\n",outPortCfg.filterAct,inPortCfg.filterAct));
		return testResults;
	  }
	}
//    if (IS_IN_DEV_GROUP(dev, DEV_HW_ACC_PTP))
	if (0)
	{
	  if ((inPortCfg.hwAccPtp != outPortCfg.hwAccPtp) ||
		(inPortCfg.kpFramwSa != outPortCfg.kpFramwSa) ||
		(inPortCfg.fullCheck != outPortCfg.fullCheck) ||
		(inPortCfg.noCorrupt != outPortCfg.noCorrupt))
	  {
		testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d,%d,%d,%d), expect (%d,%d,%d,%d)\n",
			outPortCfg.hwAccPtp, outPortCfg.kpFramwSa, outPortCfg.fullCheck, 
			outPortCfg.noCorrupt, inPortCfg.hwAccPtp, inPortCfg.kpFramwSa, 
			inPortCfg.fullCheck, inPortCfg.noCorrupt));
		return testResults;
	  }
//	  if (IS_IN_DEV_GROUP(dev, DEV_EXT_HW_ACC_PTP))
	  {
		if (inPortCfg.extHwAcc != outPortCfg.extHwAcc)
		{
			testResults |= 1 << MAD_FAIL;
			testMADDisplayStatus(MAD_FAIL);
			MSG_PRINT(("Data Mismatch (%d), expect (%d)\n",outPortCfg.extHwAcc,inPortCfg.extHwAcc));
			return testResults;
		}
	  }
	}
//	if (IS_IN_DEV_GROUP(dev, DEV_LED_CONTROL))
    {
	  if ((inPortCfg.arrLedCtrl != outPortCfg.arrLedCtrl) ||
		(inPortCfg.depLedCtrl != outPortCfg.depLedCtrl))
	  {
		testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d,%d), expect (%d,%d)\n",
			outPortCfg.arrLedCtrl, outPortCfg.depLedCtrl, 
			inPortCfg.arrLedCtrl, inPortCfg.depLedCtrl));
		return testResults;
	  }
	}
	
	inPortCfg.transSpec = 0;
	inPortCfg.disTSpec = 0;
	inPortCfg.disTSOverwrite = 0;
	inPortCfg.ipJump = 0;
	inPortCfg.etJump = 0;
	inPortCfg.ptpArrIntEn = 0;
	inPortCfg.ptpDepIntEn = 0;
	inPortCfg.arrTSMode = 0;
	inPortCfg.filterAct = 0;
	inPortCfg.hwAccPtp = 0;
	inPortCfg.kpFramwSa = 0;
	inPortCfg.fullCheck = 0;
	inPortCfg.noCorrupt = 0;
	inPortCfg.extHwAcc = 0;
	inPortCfg.arrLedCtrl = 0;
	inPortCfg.depLedCtrl = 0;

	if (status = mdPtpSetPortConfig(dev, port, &inPortCfg) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpSetPortConfig call\n"));
        return testResults;
	}

	if (status = mdPtpGetPortConfig(dev, port, &outPortCfg) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpGetPortConfig call\n"));
        return testResults;
	}
	
	if ((inPortCfg.transSpec != outPortCfg.transSpec) ||
		(inPortCfg.disTSpec != outPortCfg.disTSpec) ||
		(inPortCfg.disTSOverwrite != outPortCfg.disTSOverwrite) ||
		(inPortCfg.ipJump != outPortCfg.ipJump) ||
		(inPortCfg.etJump != outPortCfg.etJump) ||
		(inPortCfg.ptpArrIntEn != outPortCfg.ptpArrIntEn) ||
		(inPortCfg.ptpDepIntEn != outPortCfg.ptpDepIntEn))
	{
		testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d,%d,%d,%d,%d,%d,%d), \
			expect (%d,%d,%d,%d,%d,%d,%d)\n",
        outPortCfg.transSpec, outPortCfg.disTSpec, outPortCfg.disTSOverwrite, 
		outPortCfg.ipJump, outPortCfg.etJump, outPortCfg.ptpArrIntEn, 
		outPortCfg.ptpDepIntEn,
		inPortCfg.transSpec, inPortCfg.disTSpec, inPortCfg.disTSOverwrite, 
		inPortCfg.ipJump, inPortCfg.etJump, inPortCfg.ptpArrIntEn, 
		inPortCfg.ptpDepIntEn));
        return testResults;
	}
	
	
//	if (IS_IN_DEV_GROUP(dev, DEV_ARRV_TS_MODE))
    {
      if (inPortCfg.arrTSMode != outPortCfg.arrTSMode)
	  {
		testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d), expect (%d)\n",outPortCfg.arrTSMode,inPortCfg.arrTSMode));
		return testResults;
	  }
    }
	
//    if (IS_IN_DEV_GROUP(dev, DEV_LED_FILTER))
    {
      if (inPortCfg.arrTSMode != outPortCfg.arrTSMode)
	  {
		testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d), expect (%d)\n",outPortCfg.filterAct,inPortCfg.filterAct));
		return testResults;
	  }
	}
//    if (IS_IN_DEV_GROUP(dev, DEV_HW_ACC_PTP))
	if (0)
	{
	  if ((inPortCfg.hwAccPtp != outPortCfg.hwAccPtp) ||
		(inPortCfg.kpFramwSa != outPortCfg.kpFramwSa) ||
		(inPortCfg.fullCheck != outPortCfg.fullCheck) ||
		(inPortCfg.noCorrupt != outPortCfg.noCorrupt))
	  {
		testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d,%d,%d,%d), expect (%d,%d,%d,%d)\n",
			outPortCfg.hwAccPtp, outPortCfg.kpFramwSa, outPortCfg.fullCheck, 
			outPortCfg.noCorrupt, inPortCfg.hwAccPtp, inPortCfg.kpFramwSa, 
			inPortCfg.fullCheck, inPortCfg.noCorrupt));
		return testResults;
	  }
//	  if (IS_IN_DEV_GROUP(dev, DEV_EXT_HW_ACC_PTP))
	  {
		testResults |= 1 << MAD_FAIL;
		testMADDisplayStatus(MAD_FAIL);
		MSG_PRINT(("Data Mismatch (%d), expect (%d)\n",outPortCfg.extHwAcc,inPortCfg.extHwAcc));
		return testResults;
	  }
	}
//	if (IS_IN_DEV_GROUP(dev, DEV_LED_CONTROL))
    {
	  if ((inPortCfg.arrLedCtrl != outPortCfg.arrLedCtrl) ||
		(inPortCfg.depLedCtrl != outPortCfg.depLedCtrl))
	  {
		testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d,%d), expect (%d,%d)\n",
			outPortCfg.arrLedCtrl, outPortCfg.depLedCtrl, 
			inPortCfg.arrLedCtrl, inPortCfg.depLedCtrl));
		return testResults;
	  }
	}
	
	if (status = mdPtpSetPortConfig(dev, port, &orgPortCfg) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpSetPortConfig call\n"));
        return testResults;
	}

	MSG_PRINT(("PTP Port Configuration API test "));
    testMADDisplayStatus(status);

	/*
	 * PTP Enable test
	 */
	MSG_PRINT(("PTP Enable API test "));

	testAPI.getFunc.bool = mdPtpGetPTPEn;
    testAPI.setFunc.bool = mdPtpSetPTPEn;
    if((status = testBoolType(dev,&testAPI)) != MAD_OK)
    {
        testResult = MAD_FAIL;
        testResults |= 1 << status;
    }
    MSG_PRINT(("PTP Enable API test "));
    testMADDisplayStatus(status);

	/*
	 * PTP Port Enable test
	 */
	MSG_PRINT(("PTP Port Enable API test "));

	testAPI.getFunc.port_bool = mdPtpGetPortPTPEn;
    testAPI.setFunc.port_bool = mdPtpSetPortPTPEn;
    if((status = testPortBoolType(dev,&testAPI)) != MAD_OK)
    {
        testResult = MAD_FAIL;
        testResults |= 1 << status;
    }
    MSG_PRINT(("PTP Port Enable API test "));
    testMADDisplayStatus(status);

	/*
	 * PTP TS Mode test
	 */
	MSG_PRINT(("PTP TS Mode API test "));

	for(port = 0; port < dev->numOfPorts; port++)
	{
		if(status = mdPtpGetTsMode_port(dev, port, &orgTsMode) != MAD_OK)
		{
			testResults |= 1 << status;
			testMADDisplayStatus(status);
			MSG_PRINT(("Failed on madPtpGetPortTsMode call\n"));
			return testResults;
		}

		for(inTsMode = MAD_PTP_TS_MODE_IN_REG; inTsMode <= MAD_PTP_TS_MODE_IN_FRAME_END; inTsMode++)
		{
			if(status = mdPtpSetTsMode_port(dev, port, inTsMode) != MAD_OK)
			{
				testResults |= 1 << status;
				testMADDisplayStatus(status);
				MSG_PRINT(("Failed on madPtpSetPortTsMode call\n"));
				return testResults;
			}
			if(status = mdPtpGetTsMode_port(dev, port, &outTsMode) != MAD_OK)
			{
				testResults |= 1 << status;
				testMADDisplayStatus(status);
				MSG_PRINT(("Failed on mdPtpGetTsMode_port call\n"));
				return testResults;
			}
			if(inTsMode != outTsMode)
			{
				testResults |= 1 << MAD_FAIL;
				testMADDisplayStatus(MAD_FAIL);
				MSG_PRINT(("Data Mismatch (%d), expect (%d)\n",outTsMode, inTsMode));
				return testResults;
			}
		}

		if(status = mdPtpSetTsMode_port(dev, port, orgTsMode) != MAD_OK)
		{
			testResults |= 1 << status;
			testMADDisplayStatus(status);
			MSG_PRINT(("Failed on mdPtpSetTsMode_port call\n"));
			return testResults;
		}
	}

    MSG_PRINT(("PTP TS Mode API test "));
    testMADDisplayStatus(status);

	/*
	 * TOD Load Point test
	 */
	MSG_PRINT(("PTP TOD Load Point API test "));

	testAPI.getFunc.u32 = mdPtpTODLoadPointGet;
    testAPI.setFunc.u32 = mdPtpTODLoadPointSet;
    if((status = testU32Type(dev,&testAPI,0x100)) != MAD_OK)
    {
        testResult = MAD_FAIL;
        testResults |= 1 << status;
    }
    MSG_PRINT(("PTP TOD Load Point API test "));
    testMADDisplayStatus(status);

	/*
	 * TOD Operation Control test
	 */
	MSG_PRINT(("PTP TOD Operation Control API test "));

	if ((status = mdPtpTODOpCtrlGet(dev, &orgTodOp)) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpTODOpCtrlGet call\n"));
        return testResults;
	}

	inTodOp.ptpOpCtrlTodOp = 0x7;
	inTodOp.ptpOpCtrlTimeArray = 0x3;
	inTodOp.ptpOpCtrlClkValid = 1;
	inTodOp.ptpOpCtrlDomainNum = 0xff;
	
	if (status = mdPtpTODOpCtrlSet(dev, &inTodOp) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpTODOpCtrlSet call\n"));
        return testResults;
	}

	if (status = mdPtpTODOpCtrlGet(dev, &outTodOp) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpTODOpCtrlGet call\n"));
        return testResults;
	}

	if ((inTodOp.ptpOpCtrlTodOp != outTodOp.ptpOpCtrlTodOp) ||
		(inTodOp.ptpOpCtrlTimeArray != outTodOp.ptpOpCtrlTimeArray) ||
		(inTodOp.ptpOpCtrlClkValid != outTodOp.ptpOpCtrlClkValid) ||
		(inTodOp.ptpOpCtrlDomainNum != outTodOp.ptpOpCtrlDomainNum))
	{
		testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d,%d,%d,%d), expect (%d,%d,%d,%d)\n",
        outTodOp.ptpOpCtrlTodOp, outTodOp.ptpOpCtrlTimeArray, 
		outTodOp.ptpOpCtrlClkValid, outTodOp.ptpOpCtrlDomainNum,
		inTodOp.ptpOpCtrlTodOp, inTodOp.ptpOpCtrlTimeArray, 
		inTodOp.ptpOpCtrlClkValid, inTodOp.ptpOpCtrlDomainNum));
        return testResults;
	}

	inTodOp.ptpOpCtrlTodOp = 0;
	inTodOp.ptpOpCtrlTimeArray = 0;
	inTodOp.ptpOpCtrlClkValid = 0;
	inTodOp.ptpOpCtrlDomainNum = 0;

	if (status = mdPtpTODOpCtrlSet(dev, &inTodOp) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpTODOpCtrlSet call\n"));
        return testResults;
	}

	if (status = mdPtpTODOpCtrlGet(dev, &outTodOp) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpTODOpCtrlGet call\n"));
        return testResults;
	}

	if ((inTodOp.ptpOpCtrlTodOp != outTodOp.ptpOpCtrlTodOp) ||
		(inTodOp.ptpOpCtrlTimeArray != outTodOp.ptpOpCtrlTimeArray) ||
		(inTodOp.ptpOpCtrlClkValid != outTodOp.ptpOpCtrlClkValid) ||
		(inTodOp.ptpOpCtrlDomainNum != outTodOp.ptpOpCtrlDomainNum))
	{
		testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d,%d,%d,%d), expect (%d,%d,%d,%d)\n",
        outTodOp.ptpOpCtrlTodOp, outTodOp.ptpOpCtrlTimeArray, 
		outTodOp.ptpOpCtrlClkValid, outTodOp.ptpOpCtrlDomainNum,
		inTodOp.ptpOpCtrlTodOp, inTodOp.ptpOpCtrlTimeArray, 
		inTodOp.ptpOpCtrlClkValid, inTodOp.ptpOpCtrlDomainNum));
        return testResults;
	}

	if (status = mdPtpTODOpCtrlSet(dev, &orgTodOp) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpTODOpCtrlSet call\n"));
        return testResults;
	}

	MSG_PRINT(("PTP TOD Operation Control API test "));
    testMADDisplayStatus(status);

	/*
	 * PTP Pulse test
	 */
	MSG_PRINT(("PTP Pulse API test "));
	
	if (status = mdPtpPulseGet(dev, &orgPulse) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpPulseGet call\n"));
        return testResults;
	}

	inPulse.ptpPulseWidth = 0xf;
	inPulse.ptp1ppsWidthRange = 7;
	inPulse.ptp1ppsPhase = 1;
	inPulse.ptp1ppsSelect = 3;

	if (status = mdPtpPulseSet(dev, &inPulse) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on mdPtpPulseSet call\n"));
        return testResults;
	}

	if (status = mdPtpPulseGet(dev, &outPulse) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpPulseGet call\n"));
        return testResults;
	}

	if ((inPulse.ptpPulseWidth != outPulse.ptpPulseWidth) ||
		(inPulse.ptp1ppsWidthRange != outPulse.ptp1ppsWidthRange) ||
		(inPulse.ptp1ppsPhase != outPulse.ptp1ppsPhase) ||
		(inPulse.ptp1ppsSelect != outPulse.ptp1ppsSelect))
	{
		testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d,%d,%d,%d), expect (%d,%d,%d,%d)\n",
        outPulse.ptpPulseWidth, outPulse.ptp1ppsWidthRange, 
		outPulse.ptp1ppsPhase, outPulse.ptp1ppsSelect,
		inPulse.ptpPulseWidth, inPulse.ptp1ppsWidthRange, 
		inPulse.ptp1ppsPhase, inPulse.ptp1ppsSelect));
        return testResults;
	}

	inPulse.ptpPulseWidth = 0;
	inPulse.ptp1ppsWidthRange = 0;
	inPulse.ptp1ppsPhase = 0;
	inPulse.ptp1ppsSelect = 0;

	if (status = mdPtpPulseSet(dev, &inPulse) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on mdPtpPulseSet call\n"));
        return testResults;
	}

	if (status = mdPtpPulseGet(dev, &outPulse) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpPulseGet call\n"));
        return testResults;
	}

	if ((inPulse.ptpPulseWidth != outPulse.ptpPulseWidth) ||
		(inPulse.ptp1ppsWidthRange != outPulse.ptp1ppsWidthRange) ||
		(inPulse.ptp1ppsPhase != outPulse.ptp1ppsPhase) ||
		(inPulse.ptp1ppsSelect != outPulse.ptp1ppsSelect))
	{
		testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d,%d,%d,%d), expect (%d,%d,%d,%d)\n",
        outPulse.ptpPulseWidth, outPulse.ptp1ppsWidthRange, 
		outPulse.ptp1ppsPhase, outPulse.ptp1ppsSelect,
		inPulse.ptpPulseWidth, inPulse.ptp1ppsWidthRange, 
		inPulse.ptp1ppsPhase, inPulse.ptp1ppsSelect));
        return testResults;
	}

	if (status = mdPtpPulseSet(dev, &orgPulse) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on mdPtpPulseSet call\n"));
        return testResults;
	}

	MSG_PRINT(("PTP Pulse API test "));
    testMADDisplayStatus(status);

	/*
	 * PTP TOD Time test
	 */
	MSG_PRINT(("PTP TOD Time API test "));
	
	if (status = mdPtpTODTimeGet(dev, &orgTs) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpTODTimeGet call\n"));
        return testResults;
	}

	inTs.nanoseconds = 0xffffffff;
	inTs.seconds = 0xffffffff;
	inTs.topseconds = 0xffff;

	if (status = mdPtpTODTimeSet(dev, &inTs) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpTODTimeSet call\n"));
        return testResults;
	}

	if (status = mdPtpTODTimeGet(dev, &outTs) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpTODTimeGet call\n"));
        return testResults;
	}

	if ((inTs.nanoseconds != outTs.nanoseconds) ||
		(inTs.seconds != outTs.seconds) ||
		(inTs.topseconds != outTs.topseconds))
	{
		testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d,%d,%d,%d), expect (%d,%d,%d,%d)\n",
        outTs.nanoseconds, outTs.seconds, outTs.topseconds,
		inTs.nanoseconds, inTs.seconds, inTs.topseconds));
        return testResults;
	}

	inTs.nanoseconds = 0;
	inTs.seconds = 0;
	inTs.topseconds = 0;

	if (status = mdPtpTODTimeSet(dev, &inTs) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpTODTimeSet call\n"));
        return testResults;
	}

	if (status = mdPtpTODTimeGet(dev, &outTs) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpTODTimeGet call\n"));
        return testResults;
	}

	if ((inTs.nanoseconds != outTs.nanoseconds) ||
		(inTs.seconds != outTs.seconds) ||
		(inTs.topseconds != outTs.topseconds))
	{
		testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d,%d,%d,%d), expect (%d,%d,%d,%d)\n",
        outTs.nanoseconds, outTs.seconds, outTs.topseconds,
		inTs.nanoseconds, inTs.seconds, inTs.topseconds));
        return testResults;
	}

	if (status = mdPtpTODTimeSet(dev, &orgTs) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on mdPtpTODTimeSet call\n"));
        return testResults;
	}

	MSG_PRINT(("PTP TOD Time API test "));
    testMADDisplayStatus(status);

	/*
	 * PTP Time Array 1722 TOD test
	 */
	MSG_PRINT(("PTP Time Array 1722 TOD API test "));

	testAPI.getFunc.u32 = (MAD_API_GET_U32)mdPtp1722TimeGet;
    testAPI.setFunc.u32 = (MAD_API_SET_U32)mdPtp1722TimeSet;
    if((status = testU32Type(dev,&testAPI,0x100)) != MAD_OK)
    {
        testResult = MAD_FAIL;
        testResults |= 1 << status;
    }
    MSG_PRINT(("PTP Time Array 1722 TOD API test "));
    testMADDisplayStatus(status);

	/*
	 * PTP TOD Compensation test
	 */
	MSG_PRINT(("PTP TOD Compensation API test "));

	testAPI.getFunc.u32 = (MAD_API_GET_U32)mdPtpTODCompGet;
    testAPI.setFunc.u32 = (MAD_API_SET_U32)mdPtpTODCompSet;
    if((status = testU32Type(dev,&testAPI,0x100)) != MAD_OK)
    {
        testResult = MAD_FAIL;
        testResults |= 1 << status;
    }
    MSG_PRINT(("PTP TOD Compensation API test "));
    testMADDisplayStatus(status);

	/*
	 * PTP Mean Path Delay test
	 */
	MSG_PRINT(("PTP Mean Path Delay API test "));

	testAPI.getFunc.port_u16 = (MAD_API_GET_PORT_U16)mdPtpGetMeanPathDelay;
    testAPI.setFunc.port_u16 = (MAD_API_SET_PORT_U16)mdPtpSetMeanPathDelay;
    if((status = testU16Type(dev,&testAPI,0x100)) != MAD_OK)
    {
        testResult = MAD_FAIL;
        testResults |= 1 << status;
    }
    MSG_PRINT(("PTP Mean Path Delay API test "));
    testMADDisplayStatus(status);

	/*
	 * PTP Ingress Path Delay Asymmetry test
	 */
	MSG_PRINT(("PTP Ingress Path Delay Asymmetry test "));

	testAPI.getFunc.port_u16 = (MAD_API_GET_PORT_U16)mdPtpGetIngPathDelayAsym;
    testAPI.setFunc.port_u16 = (MAD_API_SET_PORT_U16)mdPtpSetIngPathDelayAsym;
    if((status = testU16Type(dev,&testAPI,0x100)) != MAD_OK)
    {
        testResult = MAD_FAIL;
        testResults |= 1 << status;
    }
    MSG_PRINT(("PTP Ingress Path Delay Asymmetry API test "));
    testMADDisplayStatus(status);

	/*
	 * PTP Egress Path Delay Asymmetry test
	 */
	MSG_PRINT(("PTP Egress Path Delay Asymmetry test "));

	testAPI.getFunc.port_u16 = (MAD_API_GET_PORT_U16)mdPtpGetEgPathDelayAsym;
    testAPI.setFunc.port_u16 = (MAD_API_SET_PORT_U16)mdPtpSetEgPathDelayAsym;
    if((status = testU16Type(dev,&testAPI,0x100)) != MAD_OK)
    {
        testResult = MAD_FAIL;
        testResults |= 1 << status;
    }
    MSG_PRINT(("PTP Egress Path Delay Asymmetry API test "));
    testMADDisplayStatus(status);

	/*
	 * PTP Mode Config API test
	 */
	MSG_PRINT(("PTP Mode Config API test "));
	
	if (status = mdPtpModeGet(dev, &orgPtpMode) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpModeGet call\n"));
        return testResults;
	}

	inPtpMode.tsAtSFD = 1;
	inPtpMode.disPtp2Switch = 1;
	inPtpMode.accelNewPTP = 1;
	inPtpMode.altScheme = 1;
	inPtpMode.grandMstr = 1;
	inPtpMode.oneStep = 1;
	inPtpMode.PTPMode = 1;

	if (status = mdPtpModeSet(dev, &inPtpMode) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpModeSet call\n"));
        return testResults;
	}
	
	if (status = mdPtpModeGet(dev, &outPtpMode) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpModeGet call\n"));
        return testResults;
	}

	if((inPtpMode.tsAtSFD != outPtpMode.tsAtSFD) ||    
		  (inPtpMode.disPtp2Switch != outPtpMode.disPtp2Switch) ||
		  (inPtpMode.accelNewPTP != outPtpMode.accelNewPTP) ||    
		  (inPtpMode.altScheme != outPtpMode.altScheme) ||
		  (inPtpMode.grandMstr != outPtpMode.grandMstr) ||
		  (inPtpMode.oneStep != outPtpMode.oneStep) ||    
		  (inPtpMode.PTPMode != outPtpMode.PTPMode))
    {
        testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d,%d,%d,%d,%d,%d,%d), expect (%d,%d,%d,%d,%d,%d,%d)\n",
        outPtpMode.tsAtSFD, outPtpMode.disPtp2Switch, outPtpMode.accelNewPTP, outPtpMode.altScheme,
		outPtpMode.grandMstr, outPtpMode.oneStep, outPtpMode.PTPMode,
        inPtpMode.tsAtSFD, inPtpMode.disPtp2Switch, inPtpMode.accelNewPTP, inPtpMode.altScheme,
		inPtpMode.grandMstr, inPtpMode.oneStep, inPtpMode.PTPMode));
        return testResults;
    }

	inPtpMode.tsAtSFD = 0;
	inPtpMode.disPtp2Switch = 0;
	inPtpMode.accelNewPTP = 0;
	inPtpMode.altScheme = 0;
	inPtpMode.grandMstr = 0;
	inPtpMode.oneStep = 0;
	inPtpMode.PTPMode = 0;

	if (status = mdPtpModeSet(dev, &inPtpMode) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpModeSet call\n"));
        return testResults;
	}
	
	if (status = mdPtpModeGet(dev, &outPtpMode) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpModeGet call\n"));
        return testResults;
	}

	if((inPtpMode.tsAtSFD != outPtpMode.tsAtSFD) ||    
		  (inPtpMode.disPtp2Switch != outPtpMode.disPtp2Switch) ||
		  (inPtpMode.accelNewPTP != outPtpMode.accelNewPTP) ||    
		  (inPtpMode.altScheme != outPtpMode.altScheme) ||
		  (inPtpMode.grandMstr != outPtpMode.grandMstr) ||
		  (inPtpMode.oneStep != outPtpMode.oneStep) ||    
		  (inPtpMode.PTPMode != outPtpMode.PTPMode))
    {
        testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d,%d,%d,%d,%d,%d,%d), expect (%d,%d,%d,%d,%d,%d,%d)\n",
        outPtpMode.tsAtSFD, outPtpMode.disPtp2Switch, outPtpMode.accelNewPTP, outPtpMode.altScheme,
		outPtpMode.grandMstr, outPtpMode.oneStep, outPtpMode.PTPMode,
        inPtpMode.tsAtSFD, inPtpMode.disPtp2Switch, inPtpMode.accelNewPTP, inPtpMode.altScheme,
		inPtpMode.grandMstr, inPtpMode.oneStep, inPtpMode.PTPMode));
        return testResults;
    }

	if (status = mdPtpModeSet(dev, &orgPtpMode) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on mdPtpModeSet call\n"));
        return testResults;
	}

	MSG_PRINT(("PTP Mode Config API test "));
    testMADDisplayStatus(status);


	/*
	 * PTP External Time Format API test
	 */
	MSG_PRINT(("PTP External Time Format API test "));
	
	if (status = mdPtpGetExtTimeFormat(dev, &orgExtTime) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpGetExtTimeFormat call\n"));
        return testResults;
	}

	inExtTime.intPTPTime = 1;
	inExtTime.extMask = 3;
	inExtTime.ext1722Time = 1;
	inExtTime.extTASel = 3;

	if (status = mdPtpSetExtTimeFormat(dev, &inExtTime) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpSetExtTimeFormat call\n"));
        return testResults;
	}
	
	if (status = mdPtpGetExtTimeFormat(dev, &outExtTime) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpGetExtTimeFormat call\n"));
        return testResults;
	}

	if((inExtTime.intPTPTime != outExtTime.intPTPTime) ||
        (inExtTime.extMask != outExtTime.extMask) ||
        (inExtTime.ext1722Time != outExtTime.ext1722Time) ||
		(inExtTime.extTASel != outExtTime.extTASel))
    {
        testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d,%d,%d,%d), expect (%d,%d,%d,%d)\n",
        outExtTime.intPTPTime, outExtTime.extMask, outExtTime.ext1722Time, outExtTime.extTASel,
        inExtTime.intPTPTime, inExtTime.extMask, inExtTime.ext1722Time, inExtTime.extTASel));
        return testResults;
    }

	inExtTime.intPTPTime = 0;
	inExtTime.extMask = 0;
	inExtTime.ext1722Time = 0;
	inExtTime.extTASel = 0;

	if (status = mdPtpSetExtTimeFormat(dev, &inExtTime) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpSetExtTimeFormat call\n"));
        return testResults;
	}
	
	if (status = mdPtpGetExtTimeFormat(dev, &outExtTime) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on mdPtpGetExtTimeFormat call\n"));
        return testResults;
	}

	if((inExtTime.intPTPTime != outExtTime.intPTPTime) ||
        (inExtTime.extMask != outExtTime.extMask) ||
        (inExtTime.ext1722Time != outExtTime.ext1722Time) ||
		(inExtTime.extTASel != outExtTime.extTASel))
    {
        testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d,%d,%d,%d), expect (%d,%d,%d,%d)\n",
        outExtTime.intPTPTime, outExtTime.extMask, outExtTime.ext1722Time, outExtTime.extTASel,
        inExtTime.intPTPTime, inExtTime.extMask, inExtTime.ext1722Time, inExtTime.extTASel));
        return testResults;
    }

	if (status = mdPtpSetExtTimeFormat(dev, &orgExtTime) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on mdPtpSetExtTimeFormat call\n"));
        return testResults;
	}

	MSG_PRINT(("PTP External Time Format API test "));
    testMADDisplayStatus(status);

	/*
	 * PTP Ingress Action Vector API test
	 */
	/*
	MSG_PRINT(("PTP Ingress Action Vector API test "));
	
	spec = 1;
	if (status = madPtpGetIngActVec(dev, spec, &orgVec) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpGetIngActVec call\n"));
        return testResults;
	}
	
	inVec = 0xffffff;

	if (status = madPtpSetIngActVec(dev, spec, inVec) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpSetIngActVec call\n"));
        return testResults;
	}

	if (status = madPtpGetIngActVec(dev, spec, &outVec) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpGetIngActVec call\n"));
        return testResults;
	}

	if (outVec != 0xffffff)
	{
		testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d), expect 0xffffff\n",outVec));
        return testResults;
	}

	inVec = 0;

	if (status = madPtpSetIngActVec(dev, spec, inVec) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpSetIngActVec call\n"));
        return testResults;
	}

	if (status = madPtpGetIngActVec(dev, spec, &outVec) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpGetIngActVec call\n"));
        return testResults;
	}

	if (outVec != 0)
	{
		testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d), expect 0\n",outVec));
        return testResults;
	}

	if (status = madPtpSetIngActVec(dev, spec, orgVec) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpSetIngActVec call\n"));
        return testResults;
	}

	spec = 0;
	if (status = madPtpGetIngActVec(dev, spec, &orgVec) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpGetIngActVec call\n"));
        return testResults;
	}

	inVec = 0xffffff;

	if (status = madPtpSetIngActVec(dev, spec, inVec) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpSetIngActVec call\n"));
        return testResults;
	}

	if (status = madPtpGetIngActVec(dev, spec, &outVec) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpGetIngActVec call\n"));
        return testResults;
	}

	if (outVec != 0xffffff)
	{
		testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d), expect 0xffffff\n",outVec));
        return testResults;
	}

	inVec = 0;

	if (status = madPtpSetIngActVec(dev, spec, inVec) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpSetIngActVec call\n"));
        return testResults;
	}

	if (status = madPtpGetIngActVec(dev, spec, &outVec) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpGetIngActVec call\n"));
        return testResults;
	}

	if (outVec != 0)
	{
		testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d), expect 0\n",outVec));
        return testResults;
	}

	if (status = madPtpSetIngActVec(dev, spec, orgVec) != MAD_OK)
	{
		testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madPtpSetIngActVec call\n"));
        return testResults;
	}

	MSG_PRINT(("PTP Ingress Action Vector API test "));
    testMADDisplayStatus(status);

	*/

	/*
	 * PTP Egress Action Vector API test
	 */
	MSG_PRINT(("PTP Egress Action Vector API test "));
	
	for (spec = 0; spec < 2; spec++)
	{
		if (status = mdPtpGetEgActVec(dev, spec, &orgVec) != MAD_OK)
		{
			testResults |= 1 << status;
			testMADDisplayStatus(status);
			MSG_PRINT(("Failed on madPtpGetEgActVec call\n"));
			return testResults;
		}
	
		inVec = 0x7007eb67;

		if (status = mdPtpSetEgActVec(dev, spec, inVec) != MAD_OK)
		{
			testResults |= 1 << status;
			testMADDisplayStatus(status);
			MSG_PRINT(("Failed on madPtpSetEgActVec call\n"));
			return testResults;
		}

		if (status = mdPtpGetEgActVec(dev, spec, &outVec) != MAD_OK)
		{
			testResults |= 1 << status;
			testMADDisplayStatus(status);
			MSG_PRINT(("Failed on adPtpGetEgActVec call\n"));
			return testResults;
		}

		if (outVec != 0x7007eb67)
		{
			testResults |= 1 << MAD_FAIL;
			testMADDisplayStatus(MAD_FAIL);
			MSG_PRINT(("Data Mismatch (%d), expect 0xffffff\n",outVec));
			return testResults;
		}

		inVec = 0;

		if (status = mdPtpSetEgActVec(dev, spec, inVec) != MAD_OK)
		{
			testResults |= 1 << status;
			testMADDisplayStatus(status);
			MSG_PRINT(("Failed on maPtpSetEgActVec call\n"));
			return testResults;
		}

		if (status = mdPtpGetEgActVec(dev, spec, &outVec) != MAD_OK)
		{
			testResults |= 1 << status;
			testMADDisplayStatus(status);
			MSG_PRINT(("Failed on mdPtpGetEgActVec call\n"));
			return testResults;
		}

		if (outVec != 0)
		{
			testResults |= 1 << MAD_FAIL;
			testMADDisplayStatus(MAD_FAIL);
			MSG_PRINT(("Data Mismatch (%d), expect 0\n",outVec));
			return testResults;
		}

		if (status = mdPtpSetEgActVec(dev, spec, orgVec) != MAD_OK)
		{
			testResults |= 1 << status;
			testMADDisplayStatus(status);
			MSG_PRINT(("Failed on mdPtpSetEgActVec call\n"));
			return testResults;
		}
	}

	MSG_PRINT(("PTP Egress Action Vector API test "));
    testMADDisplayStatus(status);
	return testResults;
}

MAD_U32 testTAI(MAD_DEV *dev)
{
    MAD_STATUS status, testResult;
    MAD_U32 testResults = 0;
    MAD_TAI_EVENT_CONFIG    inEventCfg, outEventCfg;
    MAD_TAI_TRIGGER_CONFIG inTrigCfg, outTrigCfg;
	MAD_TAI_CLOCK_CONFIG inClkCfg, outClkCfg;
    MAD_BOOL    en, inc;
    MAD_U32    data;
    TEST_API testAPI;

    status = MAD_OK;

    /*
     * Event Config Setup test
    */
    MSG_PRINT(("Event Config Setup API test "));

    inEventCfg.eventOverwrite = 1;
    inEventCfg.eventCtrStart = 1;
    inEventCfg.intEn = 1;
    inEventCfg.captTrigEvent= 1;
    inEventCfg.eventPhase=1;

    if((status = madTaiSetEventConfig(dev,&inEventCfg)) != MAD_OK)
    {
        testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madTaiSetEventConfig call\n"));
        return testResults;
    }

    if((status = madTaiGetEventConfig(dev,&outEventCfg)) != MAD_OK)
    {
        testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madTaiGetEventConfig call\n"));
        return testResults;
    }

    if((inEventCfg.eventOverwrite != outEventCfg.eventOverwrite) ||
        (inEventCfg.eventCtrStart != outEventCfg.eventCtrStart) ||
        (inEventCfg.intEn != outEventCfg.intEn))
    {
        testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d,%d,%d), expect (%d,%d,%d)\n",
        outEventCfg.eventOverwrite, outEventCfg.eventCtrStart, outEventCfg.intEn,
        inEventCfg.eventOverwrite, inEventCfg.eventCtrStart, inEventCfg.intEn));
        return testResults;
    }
//    if (IS_IN_DEV_GROUP(dev,DEV_TAI_EXT_CLK))
    {
      if(inEventCfg.eventPhase != outEventCfg.eventPhase)
      {
        testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d), expect (%d)\n",
        outEventCfg.eventPhase, inEventCfg.eventPhase));
        return testResults;
      }
    }
	
//	if (IS_IN_DEV_GROUP(dev,DEV_TAI_TRIG_GEN))
	{
	  if(inEventCfg.captTrigEvent != outEventCfg.captTrigEvent)
	  {
		testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d), expect (%d)\n",
        outEventCfg.captTrigEvent, inEventCfg.captTrigEvent));
        return testResults;
      }
	}

    inEventCfg.eventOverwrite = 0;
    inEventCfg.eventCtrStart = 0;
    inEventCfg.intEn = 0;
    inEventCfg.captTrigEvent= 0;
    inEventCfg.eventPhase=0;

    if((status = madTaiSetEventConfig(dev,&inEventCfg)) != MAD_OK)
    {
        testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madTaiSetEventConfig call\n"));
        return testResults;
    }

    if((status = madTaiGetEventConfig(dev,&outEventCfg)) != MAD_OK)
    {
        testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madTaiGetEventConfig call\n"));
        return testResults;
    }

    if((inEventCfg.eventOverwrite != outEventCfg.eventOverwrite) ||
        (inEventCfg.eventCtrStart != outEventCfg.eventCtrStart) ||
        (inEventCfg.intEn != outEventCfg.intEn))
    {
        testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d,%d,%d), expect (%d,%d,%d)\n",
        outEventCfg.eventOverwrite, outEventCfg.eventCtrStart, outEventCfg.intEn,
        inEventCfg.eventOverwrite, inEventCfg.eventCtrStart, inEventCfg.intEn));
        return testResults;
    }
//    if (IS_IN_DEV_GROUP(dev,DEV_TAI_EXT_CLK))
    {
      if(inEventCfg.eventPhase != outEventCfg.eventPhase)
      {
        testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT((" 1 Data Mismatch (%d), expect (%d)\n",
        outEventCfg.eventPhase, inEventCfg.eventPhase));
        return testResults;
      }
    }

//	if (IS_IN_DEV_GROUP(dev,DEV_TAI_TRIG_GEN))
	{
	  if(inEventCfg.captTrigEvent != outEventCfg.captTrigEvent)
	  {
		testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d), expect (%d)\n",
        outEventCfg.captTrigEvent, inEventCfg.captTrigEvent));
        return testResults;
      }
	}

    testMADDisplayStatus(status);

    /*
     * Trigger Config Setup test
    */
    MSG_PRINT(("Trigger Config Setup API test "));

    /* Disable Trigger Setup */
    if((status = madTaiSetTrigConfig(dev,0, &inTrigCfg)) != MAD_OK)
    {
        testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madTaiSetTrigConfig call\n"));
        return testResults;
    }

    if((status = madTaiGetTrigConfig(dev,&en, &outTrigCfg)) != MAD_OK)
    {
        testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madTaiGetTrigConfig call\n"));
        return testResults;
    }

    if(en != 0)
    {
        testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (1, expect 0)\n"));
        return testResults;
    }


    /* Enable Trigger Setup */

    inTrigCfg.mode = 0;
    inTrigCfg.trigGenAmt = 0x12345678;
/*    inTrigCfg.pulseWidth = 0xF; The mode is 0, there is no width */
    inTrigCfg.intEn = 1;
    inTrigCfg.lockCorrect = 0x5;
    inTrigCfg.trigClkComp = 0x5555;
    inTrigCfg.trigGenTime = 0x5555aaaa;
    inTrigCfg.trigGenDelay = 0x5555;
	inTrigCfg.trigWindow = 1;
	inTrigCfg.trig2Signal = 1;
	inTrigCfg.trigPhase = 0;
	inTrigCfg.timeDec = 1;
	inTrigCfg.incDecAmt = 0x7ff;

//	if (IS_IN_DEV_GROUP(dev,DEV_IRL_CLK))
	{
		inTrigCfg.irlGenTime = 0x5555aaaa;	
	}

    if((status = madTaiSetTrigConfig(dev, 1, &inTrigCfg)) != MAD_OK)
    {
        testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madTaiSetTrigConfig call\n"));
        return testResults;
    }

    if((status = madTaiGetTrigConfig(dev,&en, &outTrigCfg)) != MAD_OK)
    {
        testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madTaiGetTrigConfig call\n"));
        return testResults;
    }

    if ((outTrigCfg.mode != inTrigCfg.mode) ||
        (outTrigCfg.trigGenAmt != inTrigCfg.trigGenAmt) ||
/*        (outTrigCfg.pulseWidth != inTrigCfg.pulseWidth) || */
        (outTrigCfg.intEn != inTrigCfg.intEn) ||
        (en != 1) ||
		 (outTrigCfg.trigGenTime != inTrigCfg.trigGenTime) ||
        (outTrigCfg.trigGenDelay != inTrigCfg.trigGenDelay) ||
		(outTrigCfg.trigPhase != inTrigCfg.trigPhase) ||
		(outTrigCfg.timeDec != inTrigCfg.timeDec) ||
		(outTrigCfg.incDecAmt != inTrigCfg.incDecAmt))
    {
        testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data 0 Mismatch (%d,%d,%d,%d,%d,%d,%d), expect (%d,%d,%d,%d,%d,%d,%d)\n",
			(int)outTrigCfg.mode, (int)outTrigCfg.trigGenAmt, (int)outTrigCfg.pulseWidth, (int)outTrigCfg.intEn,(int)en,(int)outTrigCfg.trigPhase,(int)outTrigCfg.timeDec,
			(int)inTrigCfg.mode, (int)inTrigCfg.trigGenAmt, (int)inTrigCfg.pulseWidth, (int)inTrigCfg.intEn,1,(int)inTrigCfg.trigPhase),(int)inTrigCfg.timeDec);
        MSG_PRINT(("Data 1 Mismatch (%x,%x,%x), expect (%x,%x,%x)\n",
		   (int)outTrigCfg.trigGenTime, (int)outTrigCfg.trigGenDelay,(int)outTrigCfg.incDecAmt,
		   (int)inTrigCfg.trigGenTime, (int)inTrigCfg.trigGenDelay,(int)inTrigCfg.incDecAmt)
		 );
        return testResults;
    }

//	if (IS_IN_DEV_GROUP(dev,DEV_IRL_CLK))
	{
		if (outTrigCfg.irlGenTime != inTrigCfg.irlGenTime)
			MSG_PRINT(("Data Mismatch (%d), expect (%d)\n",
            (int)outTrigCfg.irlGenTime, (int)inTrigCfg.irlGenTime));	
	}

	inTrigCfg.mode = 1;
    inTrigCfg.trigGenAmt = 0x5a5a5a5a;
    inTrigCfg.pulseWidth = 0x1;
    inTrigCfg.intEn = 0;
    inTrigCfg.lockCorrect = 0x0;   /* read only */
    inTrigCfg.trigClkComp = 0xaaaa;
    inTrigCfg.trigGenTime = 0xaaaa5555;
    inTrigCfg.trigGenDelay = 0xaaaa;
	inTrigCfg.trigWindow = 0;
	inTrigCfg.trig2Signal = 0;
	inTrigCfg.timeDec = 0;
	inTrigCfg.incDecAmt = 0;

//    if (IS_IN_DEV_GROUP(dev,DEV_TAI_EXT_CLK))
    {
      inTrigCfg.trigPhase=1;
    }

//	if (IS_IN_DEV_GROUP(dev,DEV_IRL_CLK))
	{
		inTrigCfg.irlGenTime = 0xaaaa5555;	
	}

    if((status = madTaiSetTrigConfig(dev, 1, &inTrigCfg)) != MAD_OK)
    {
        testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madTaiSetTrigConfig call\n"));
        return testResults;
    }

    if((status = madTaiGetTrigConfig(dev,&en, &outTrigCfg)) != MAD_OK)
    {
        testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madTaiGetTrigConfig call\n"));
        return testResults;
    }

   if((outTrigCfg.mode != inTrigCfg.mode) ||
        (outTrigCfg.trigGenAmt != inTrigCfg.trigGenAmt) ||
        (outTrigCfg.pulseWidth != inTrigCfg.pulseWidth) ||
        (outTrigCfg.intEn != inTrigCfg.intEn) || 
	    (en != 1) ||
        (outTrigCfg.trigGenTime != inTrigCfg.trigGenTime) ||
        (outTrigCfg.trigGenDelay != inTrigCfg.trigGenDelay) ||
		(outTrigCfg.trigPhase != inTrigCfg.trigPhase) ||
		(outTrigCfg.irlGenTime != inTrigCfg.irlGenTime) ||
		(outTrigCfg.timeDec != inTrigCfg.timeDec) ||
		(outTrigCfg.incDecAmt != inTrigCfg.incDecAmt))
    {
        testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data 0 Mismatch (%d,%d,%d,%d,%d,%d), expect (%d,%d,%d,%d,%d,%d)\n",
			(int)outTrigCfg.mode, (int)outTrigCfg.trigGenAmt, (int)outTrigCfg.pulseWidth, (int)outTrigCfg.intEn,(int)en,(int)outTrigCfg.timeDec,
			(int)inTrigCfg.mode, (int)inTrigCfg.trigGenAmt, (int)inTrigCfg.pulseWidth, (int)inTrigCfg.intEn,1),(int)inTrigCfg.timeDec);
        MSG_PRINT(("Data 1 Mismatch (%x,%x,%x), expect (%x,%x,%x)\n",
		   (int)outTrigCfg.trigGenTime, (int)outTrigCfg.trigGenDelay,(int)outTrigCfg.incDecAmt,
		   (int)inTrigCfg.trigGenTime, (int)inTrigCfg.trigGenDelay,(int)inTrigCfg.incDecAmt)
		 );
        return testResults;
    }
//    if (IS_IN_DEV_GROUP(dev,DEV_TAI_EXT_CLK))
    {
      if(inTrigCfg.trigPhase != outTrigCfg.trigPhase)
      {
        testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d), expect (%d)\n",
        inTrigCfg.trigPhase, outTrigCfg.trigPhase));
        return testResults;
      }
    }

//	if (IS_IN_DEV_GROUP(dev,DEV_IRL_CLK))
	{
		if (outTrigCfg.irlGenTime != inTrigCfg.irlGenTime)
			MSG_PRINT(("Data Mismatch (%d), expect (%d)\n",
            (int)outTrigCfg.irlGenTime, (int)inTrigCfg.irlGenTime));	
	}

    testMADDisplayStatus(status);

    /*
     * Trigger Lock test
    */
//    if(IS_IN_DEV_GROUP(dev,DEV_TAI_TRIG_GEN))
	{
	  MAD_TAI_TRIG_LOCK_CONFIG trigLockCfg, tmpTrigLockCfg;
      MSG_PRINT(("Trigger Lock API test "));

    trigLockCfg.trigLock = 1;
    trigLockCfg.trigLockRange = 5;
    if((status = madTaiSetTrigLock(dev, &trigLockCfg)) != MAD_OK)
    {
        testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madTaiSetTrigLock call\n"));
        return testResults;
    }

    if((status = madTaiGetTrigLock(dev,&tmpTrigLockCfg)) != MAD_OK)
    {
        testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madTaiGetTrigLock call\n"));
        return testResults;
    }

 /*   if((tmpTrigLockCfg.trigLock != trigLockCfg.trigLock) ||
        (tmpTrigLockCfg.trigLockRange != trigLockCfg.trigLockRange))
		*/
    if((tmpTrigLockCfg.trigLockRange != trigLockCfg.trigLockRange))
    {
        testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d,%d), expect (%d,%d)\n",
        tmpTrigLockCfg.trigLock,tmpTrigLockCfg.trigLockRange, 
		trigLockCfg.trigLock, trigLockCfg.trigLockRange));
        return testResults;
    }

    trigLockCfg.trigLock = 0;
    trigLockCfg.trigLockRange = 0x5;

    if((status = madTaiSetTrigLock(dev, &trigLockCfg)) != MAD_OK)
    {
        testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madTaiSetTrigLock call\n"));
        return testResults;
    }

    if((status = madTaiGetTrigLock(dev,&tmpTrigLockCfg)) != MAD_OK)
    {
        testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madTaiGetTrigLock call\n"));
        return testResults;
    }

/*    if((tmpTrigLockCfg.trigLock != trigLockCfg.trigLock) ||
        (tmpTrigLockCfg.trigLockRange != trigLockCfg.trigLockRange))
		*/
    if((tmpTrigLockCfg.trigLockRange != trigLockCfg.trigLockRange))
    {
        testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d,%d), expect (%d,%d)\n",
        tmpTrigLockCfg.trigLock,tmpTrigLockCfg.trigLockRange, 
		trigLockCfg.trigLock, trigLockCfg.trigLockRange));
        return testResults;
    }

    testMADDisplayStatus(status);
    }
    /*
     * TAI Clock select test
    */
	MSG_PRINT(("TAI Clock Select API test "));
//    if (IS_IN_DEV_GROUP(dev,DEV_TAI_EXT_CLK))
    {
      MAD_TAI_CLOCK_SELECT    clkSelect, tmpClkSelect;
      clkSelect.priRecClkSel = 5;
      clkSelect.syncRecClkSel = 5;
      clkSelect.ptpExtClk = 1;
      if((status = madTaiSetClockSelect(dev, &clkSelect)) != MAD_OK)
      {
        testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madTaiSetClockSelect call\n"));
        return testResults;
      }

      if((status = madTaiGetClockSelect(dev, &tmpClkSelect)) != MAD_OK)
      {
        testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madTaiGetTrigConfig call\n"));
        return testResults;
      }

      if ((clkSelect.priRecClkSel != tmpClkSelect.priRecClkSel) ||
          (clkSelect.syncRecClkSel != tmpClkSelect.syncRecClkSel) ||
          (clkSelect.ptpExtClk != tmpClkSelect.ptpExtClk) )
      {
        testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d,%d,%d), expect (%d,%d,%d)\n",
            tmpClkSelect.priRecClkSel, tmpClkSelect.syncRecClkSel, tmpClkSelect.ptpExtClk,
            clkSelect.priRecClkSel, clkSelect.syncRecClkSel, clkSelect.ptpExtClk));
        return testResults;
      }

	  clkSelect.priRecClkSel = 5;
      clkSelect.syncRecClkSel = 5;
      clkSelect.ptpExtClk = 1;
      if((status = madTaiSetClockSelect(dev, &clkSelect)) != MAD_OK)
      {
        testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madTaiSetClockSelect call\n"));
        return testResults;
      }

      if((status = madTaiGetClockSelect(dev, &tmpClkSelect)) != MAD_OK)
      {
        testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madTaiGetTrigConfig call\n"));
        return testResults;
      }

      if ((clkSelect.priRecClkSel != tmpClkSelect.priRecClkSel) ||
          (clkSelect.syncRecClkSel != tmpClkSelect.syncRecClkSel) ||
          (clkSelect.ptpExtClk != tmpClkSelect.ptpExtClk) )
      {
        testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d,%d,%d), expect (%d,%d,%d)\n",
            tmpClkSelect.priRecClkSel, tmpClkSelect.syncRecClkSel, tmpClkSelect.ptpExtClk,
            clkSelect.priRecClkSel, clkSelect.syncRecClkSel, clkSelect.ptpExtClk));
        return testResults;
      }

	  clkSelect.priRecClkSel = 2;
      clkSelect.syncRecClkSel = 2;
      clkSelect.ptpExtClk = 0;
      if((status = madTaiSetClockSelect(dev, &clkSelect)) != MAD_OK)
      {
        testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madTaiSetClockSelect call\n"));
        return testResults;
      }

      if((status = madTaiGetClockSelect(dev, &tmpClkSelect)) != MAD_OK)
      {
        testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madTaiGetTrigConfig call\n"));
        return testResults;
      }

      if ((clkSelect.priRecClkSel != tmpClkSelect.priRecClkSel) ||
          (clkSelect.syncRecClkSel != tmpClkSelect.syncRecClkSel) ||
          (clkSelect.ptpExtClk != tmpClkSelect.ptpExtClk) )
      {
        testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d,%d,%d), expect (%d,%d,%d)\n",
            tmpClkSelect.priRecClkSel, tmpClkSelect.syncRecClkSel, tmpClkSelect.ptpExtClk,
            clkSelect.priRecClkSel, clkSelect.syncRecClkSel, clkSelect.ptpExtClk));
        return testResults;
      }

      testMADDisplayStatus(status);
    }

	/*
	 * TAI Clock Config test
	 */

	MSG_PRINT(("TAI Clock Config API test "));
//	if(!IS_IN_DEV_GROUP(dev,DEV_IRL_CLK))
	if (1)
	{
		inClkCfg.irlClkEn = 1;
		inClkCfg.tsClkPer = 0xffff;
		inClkCfg.compDir = 1;
		inClkCfg.irlClkGenAmt = 0xffff;
		inClkCfg.irlClkComp = 0x7fff;
		inClkCfg.irlClkCompSubPs = 0xff;

		if((status = madTaiSetClockConfig(dev, &inClkCfg)) != MAD_OK)
      {
        testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madTaiSetClockConfig call\n"));
        return testResults;
      }

		if((status = madTaiGetClockConfig(dev, &outClkCfg)) != MAD_OK)
      {
        testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madTaiGetClockConfig call\n"));
        return testResults;
      }

		if ((inClkCfg.tsClkPer != outClkCfg.tsClkPer) ||
		  (inClkCfg.irlClkEn != outClkCfg.irlClkEn) ||
		  (inClkCfg.compDir != outClkCfg.compDir) ||
          (inClkCfg.irlClkGenAmt != outClkCfg.irlClkGenAmt) ||
          (inClkCfg.irlClkComp != outClkCfg.irlClkComp) ||
		  (inClkCfg.irlClkCompSubPs != outClkCfg.irlClkCompSubPs))
      {
        testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d,%d,%d,%d,%d,%d), expect (%d,%d,%d,%d,%d,%d)\n",
			outClkCfg.tsClkPer,outClkCfg.irlClkEn, outClkCfg.compDir, outClkCfg.irlClkGenAmt, outClkCfg.irlClkComp, outClkCfg.irlClkCompSubPs,
            inClkCfg.tsClkPer,inClkCfg.irlClkEn, inClkCfg.compDir, inClkCfg.irlClkGenAmt, inClkCfg.irlClkComp, inClkCfg.irlClkCompSubPs));
        return testResults;
      }

		inClkCfg.tsClkPer = 0;
		inClkCfg.irlClkEn = 0;
		inClkCfg.compDir = 0;
		inClkCfg.irlClkGenAmt = 0;
		inClkCfg.irlClkComp = 0;
		inClkCfg.irlClkCompSubPs = 0;

		if((status = madTaiSetClockConfig(dev, &inClkCfg)) != MAD_OK)
      {
        testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madTaiSetClockConfig call\n"));
        return testResults;
      }

		if((status = madTaiGetClockConfig(dev, &outClkCfg)) != MAD_OK)
      {
        testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madTaiGetClockConfig call\n"));
        return testResults;
      }

		if ((inClkCfg.tsClkPer != outClkCfg.tsClkPer) ||
		  (inClkCfg.irlClkEn != outClkCfg.irlClkEn) ||
		  (inClkCfg.compDir != outClkCfg.compDir) ||
          (inClkCfg.irlClkGenAmt != outClkCfg.irlClkGenAmt) ||
          (inClkCfg.irlClkComp != outClkCfg.irlClkComp) ||
		  (inClkCfg.irlClkCompSubPs != outClkCfg.irlClkCompSubPs))
      {
        testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d,%d,%d,%d,%d,%d), expect (%d,%d,%d,%d,%d,%d)\n",
			outClkCfg.tsClkPer,outClkCfg.irlClkEn, outClkCfg.compDir, outClkCfg.irlClkGenAmt, outClkCfg.irlClkComp, outClkCfg.irlClkCompSubPs,
			inClkCfg.tsClkPer,inClkCfg.irlClkEn, inClkCfg.compDir, inClkCfg.irlClkGenAmt, inClkCfg.irlClkComp, inClkCfg.irlClkCompSubPs));
        return testResults;
      }
	}
	else
	{
		inClkCfg.irlClkEn = 1;
		inClkCfg.compDir = 1;
		inClkCfg.irlClkGenAmt = 0xffff;
		inClkCfg.irlClkComp = 0x7fff;
		inClkCfg.irlClkCompSubPs = 0xff;

		if((status = madTaiSetClockConfig(dev, &inClkCfg)) != MAD_OK)
      {
        testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madTaiSetClockConfig call\n"));
        return testResults;
      }

		if((status = madTaiGetClockConfig(dev, &outClkCfg)) != MAD_OK)
      {
        testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madTaiGetClockConfig call\n"));
        return testResults;
      }

		if ((inClkCfg.irlClkEn != outClkCfg.irlClkEn) ||
		  (inClkCfg.compDir != outClkCfg.compDir) ||
          (inClkCfg.irlClkGenAmt != outClkCfg.irlClkGenAmt) ||
          (inClkCfg.irlClkComp != outClkCfg.irlClkComp) ||
		  (inClkCfg.irlClkCompSubPs != outClkCfg.irlClkCompSubPs))
      {
        testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d,%d,%d,%d,%d), expect (%d,%d,%d,%d,%d)\n",
            outClkCfg.irlClkEn, outClkCfg.compDir, outClkCfg.irlClkGenAmt, outClkCfg.irlClkComp, outClkCfg.irlClkCompSubPs,
            inClkCfg.irlClkEn, inClkCfg.compDir, inClkCfg.irlClkGenAmt, inClkCfg.irlClkComp, inClkCfg.irlClkCompSubPs));
        return testResults;
      }
		inClkCfg.irlClkEn = 0;
		inClkCfg.compDir = 0;
		inClkCfg.irlClkGenAmt = 0;
		inClkCfg.irlClkComp = 0;
		inClkCfg.irlClkCompSubPs = 0;

		if((status = madTaiSetClockConfig(dev, &inClkCfg)) != MAD_OK)
      {
        testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madTaiSetClockConfig call\n"));
        return testResults;
      }

		if((status = madTaiGetClockConfig(dev, &outClkCfg)) != MAD_OK)
      {
        testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madTaiGetClockConfig call\n"));
        return testResults;
      }

		if ((inClkCfg.irlClkEn != outClkCfg.irlClkEn) ||
		  (inClkCfg.compDir != outClkCfg.compDir) ||
          (inClkCfg.irlClkGenAmt != outClkCfg.irlClkGenAmt) ||
          (inClkCfg.irlClkComp != outClkCfg.irlClkComp) ||
		  (inClkCfg.irlClkCompSubPs != outClkCfg.irlClkCompSubPs))
      {
        testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d,%d,%d,%d,%d), expect (%d,%d,%d,%d,%d)\n",
            outClkCfg.irlClkEn, outClkCfg.compDir, outClkCfg.irlClkGenAmt, outClkCfg.irlClkComp, outClkCfg.irlClkCompSubPs,
            inClkCfg.irlClkEn, inClkCfg.compDir, inClkCfg.irlClkGenAmt, inClkCfg.irlClkComp, inClkCfg.irlClkCompSubPs));
        return testResults;
      }
	}

    /*
     * TSCLK Period Setup test
    */
    testAPI.getFunc.u32 = (MAD_API_GET_U32)madTaiGetTSClkPer;
    testAPI.setFunc.u32 = (MAD_API_SET_U32)madTaiSetTSClkPer;
    if((status = testU32Type(dev,&testAPI,0xFF)) != MAD_OK)
    {
        testResult = MAD_FAIL;
        testResults |= 1 << status;
    }
    MSG_PRINT(("TSCLK Period Setup API test "));
    testMADDisplayStatus(status);


    /*
     * Multi PTP Sync Mode Setup test
    */
    MSG_PRINT(("Multi PTP Sync Mode Setup API test "));

    /* Enable Multi PTP sync mode Setup */

    if((status = madTaiSetMultiPTPSync(dev, 1, 0x12345678)) != MAD_OK)
    {
        testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madTaiSetMultiPTPSync call\n"));
        return testResults;
    }

    if((status = madTaiGetMultiPTPSync(dev, &en, &data)) != MAD_OK)
    {
        testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madTaiGetMultiPTPSync call\n"));
        return testResults;
    }

    if ((data != 0x12345678) ||
        (en != 1))
    {
        testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d,%d), expect (%d,%d)\n",
            (int)data, en, 0x12345678, 1));
        return testResults;
    }

    /* Disable Multi PTP sync mode */
    if((status = madTaiSetMultiPTPSync(dev,0,0)) != MAD_OK)
    {
        testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madTaiSetMultiPTPSync call\n"));
        return testResults;
    }

    if((status = madTaiGetMultiPTPSync(dev,&en, &data)) != MAD_OK)
    {
        testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madTaiGetMultiPTPSync call\n"));
        return testResults;
    }

    if(en != 0)
    {
        testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (1, expect 0)\n"));
        return testResults;
    }
    testMADDisplayStatus(status);


    /*
     * Time Inc/Dec Setup test
    */
    MSG_PRINT(("Time INC-DEC Setup API test "));

    /* Time increment Setup */
    if((status = madTaiSetTimeInc(dev, 0x123)) != MAD_OK)
    {
        testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madTaiSetTimeInc call\n"));
        return testResults;
    }

    if((status = madTaiGetTimeIncDec(dev, &en, &inc, &data)) != MAD_OK)
    {
        testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madTaiGetTimeIncDec call\n"));
        return testResults;
    }

    if ((data != 0x123) ||
        (inc != 1))
    {
        testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d,%d), expect (%d,%d)\n",
            (int)data, inc, 0x123, 1));
        return testResults;
    }

    /* Time decrement Setup */
    if((status = madTaiSetTimeDec(dev, 0x4c)) != MAD_OK)
    {
        testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madTaiSetTimeInc call\n"));
        return testResults;
    }

    if((status = madTaiGetTimeIncDec(dev, &en, &inc, &data)) != MAD_OK)
    {
        testResults |= 1 << status;
        testMADDisplayStatus(status);
        MSG_PRINT(("Failed on madTaiGetTimeIncDec call\n"));
        return testResults;
    }

    if ((data != 0x4c) ||
        (inc != 0))
    {
        testResults |= 1 << MAD_FAIL;
        testMADDisplayStatus(MAD_FAIL);
        MSG_PRINT(("Data Mismatch (%d,%d), expect (%d,%d)\n",
            (int)data, inc, 0x4c, 0));
        return testResults;
    }

    MSG_PRINT(("(expired %d)",en));
    
    testMADDisplayStatus(status);

    return testResults;
}







