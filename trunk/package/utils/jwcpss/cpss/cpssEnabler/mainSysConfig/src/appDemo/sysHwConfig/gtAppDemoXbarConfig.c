/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* gtAppDemoXbarConfig.c
*
* DESCRIPTION:
*       Crossbar configuration and initialization control.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.2.1 $
*
*******************************************************************************/

#include <appDemo/os/appOs.h>
#include <appDemo/sysHwConfig/gtAppDemoXbarConfig.h>


/*******************************************************************************
* internal definitions
*******************************************************************************/
#ifdef PRESTERA_DEBUG
#define XBAR_APP_DEBUG
#endif

#ifdef XBAR_APP_DEBUG
#define DBG_INFO(x)     osPrintf x
#else
#define DBG_INFO(x)
#endif

/* appDemoPpConfigList - Holds the Pp's configuration parameters.   */
APP_DEMO_XBAR_CONFIG  appDemoXbarConfigList[PRV_CPSS_MAX_PP_DEVICES_CNS];

static const GT_HG_INIT_PARAMS hgDefInitParamsGtFx900[4] =
{   /*lane 0 */
    {0,                              /* fport                    */
     GT_TRUE,                        /* Fport is valid           */
     GT_FALSE,                       /* swap lane order          */
     4,                              /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,/* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,          /* clock speed              */
     GT_FALSE,                       /* repeater mode            */
     GT_HG_NOT_VALID_VCM             /* voltage mode             */
    },
   /*lane 1 */
    {1,                              /* fport                    */
     GT_TRUE,                        /* Fport is valid           */
     GT_FALSE,                       /* swap lane order          */
     4,                              /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,/* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,          /* clock speed              */
     GT_FALSE,                       /* repeater mode            */
     GT_HG_NOT_VALID_VCM             /* voltage mode             */
    },
   /*lane 2 */
    {2,                              /* fport                    */
     GT_TRUE,                        /* Fport is valid           */
     GT_FALSE,                       /* swap lane order          */
     4,                              /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,/* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,          /* clock speed              */
     GT_FALSE,                       /* repeater mode            */
     GT_HG_NOT_VALID_VCM             /* voltage mode             */
    },
   /*lane 3 */
    {3,                              /* fport                    */
     GT_TRUE,                        /* Fport is valid           */
     GT_FALSE,                       /* swap lane order          */
     4,                              /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,/* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,          /* clock speed              */
     GT_FALSE,                       /* repeater mode            */
     GT_HG_NOT_VALID_VCM             /* voltage mode             */
    }
};


static const GT_HG_INIT_PARAMS hgDefInitParamsGtFx902[4] =
{   /*lane 0 */
    {0,                              /* fport                    */
     GT_TRUE,                        /* Fport is valid           */
     GT_FALSE,                       /* swap lane order          */
     6,                              /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,/* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,          /* clock speed              */
     GT_FALSE,                       /* repeater mode            */
     GT_HG_NOT_VALID_VCM             /* voltage mode             */
    },
   /*lane 1 */
    {1,                              /* fport                    */
     GT_TRUE,                        /* Fport is valid           */
     GT_FALSE,                       /* swap lane order          */
     4,                              /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,/* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,          /* clock speed              */
     GT_FALSE,                       /* repeater mode            */
     GT_HG_NOT_VALID_VCM             /* voltage mode             */
    },
   /*lane 2 */
    {2,                              /* fport                    */
     GT_TRUE,                        /* Fport is valid           */
     GT_FALSE,                       /* swap lane order          */
     4,                              /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,/* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,          /* clock speed              */
     GT_FALSE,                       /* repeater mode            */
     GT_HG_NOT_VALID_VCM             /* voltage mode             */
    },
      /*lane 3 */
    {3,                              /* fport                    */
     GT_FALSE,                       /* Fport is valid           */
     GT_FALSE,                       /* swap lane order          */
     4,                              /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,/* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,          /* clock speed              */
     GT_FALSE,                       /* repeater mode            */
     GT_HG_NOT_VALID_VCM             /* voltage mode             */
    }
};

static const GT_HG_INIT_PARAMS hgDefInitParamsGtFx910[4] =
{   /*lane 0 */
    {0,                              /* fport 0                  */
     GT_TRUE,                        /* Fport is valid           */
     GT_FALSE,                       /* swap lane order          */
     6,                              /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,/* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,          /* clock speed              */
     GT_FALSE,                       /* repeater mode            */
     GT_HG_NOT_VALID_VCM             /* voltage mode             */
    },
   /*lane 1 */
    {1,                              /* fport 0                  */
     GT_TRUE,                        /* Fport is valid           */
     GT_FALSE,                       /* swap lane order          */
     6,                              /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,/* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,          /*clock speed               */
     GT_FALSE,                       /*repeater mode             */
     GT_HG_NOT_VALID_VCM             /* voltage mode             */
    },

    /*lane 2 */
    {2,                              /* fport 0                  */
     GT_FALSE,                       /* Fport is valid           */
     GT_FALSE,                       /* swap lane order          */
     6,                              /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,/* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,          /*clock speed               */
     GT_FALSE,                       /*repeater mode             */
     GT_HG_NOT_VALID_VCM             /* voltage mode             */
    },

   /*lane 3 */
    {3,                              /* fport 0                  */
     GT_TRUE,                        /* Fport is valid           */
     GT_FALSE,                       /* swap lane order          */
     6,                              /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,/* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,          /*clock speed               */
     GT_FALSE,                       /*repeater mode             */
     GT_HG_NOT_VALID_VCM             /* voltage mode             */
    }
};

static const GT_HG_INIT_PARAMS hgDefInitParamsGtFxFX9210[12] =
{   /*lane 0 */
    {0,                                 /* fport                    */
     GT_TRUE,                           /* Fport is valid           */
     GT_FALSE,                          /* swap lane order          */
     6,                                 /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,   /* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,             /* clock speed              */
     GT_FALSE,                          /* repeater mode            */
     GT_HG_NOT_VALID_VCM                /* voltage mode             */
    },
    /*lane 1 */
    {1,                                 /* fport                    */
     GT_TRUE,                           /* Fport is valid           */
     GT_FALSE,                          /* swap lane order          */
     6,                                 /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,   /* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,             /* clock speed              */
     GT_FALSE,                          /* repeater mode            */
     GT_HG_NOT_VALID_VCM                /* voltage mode             */
    },
    /*lane 2 */
    {2,                                 /* fport                    */
     GT_TRUE,                           /* Fport is valid           */
     GT_FALSE,                          /* swap lane order          */
     6,                                 /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,   /* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,             /* clock speed              */
     GT_FALSE,                          /* repeater mode            */
     GT_HG_NOT_VALID_VCM                /* voltage mode             */
    },
    /*lane 3 */
    {3,                                 /* fport                    */
     GT_TRUE,                           /* Fport is valid           */
     GT_FALSE,                          /* swap lane order          */
     6,                                 /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,   /* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,             /* clock speed              */
     GT_FALSE,                          /* repeater mode            */
     GT_HG_NOT_VALID_VCM                /* voltage mode             */
    },
    /*lane 4 */
    {4,                                 /* fport                    */
     GT_TRUE,                           /* Fport is valid           */
     GT_FALSE,                          /* swap lane order          */
     6,                                 /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,   /* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,             /* clock speed              */
     GT_FALSE,                          /* repeater mode            */
     GT_HG_NOT_VALID_VCM                /* voltage mode             */
    },
    /*lane 5 */
    {5,                                 /* fport                    */
     GT_TRUE,                           /* Fport is valid           */
     GT_FALSE,                          /* swap lane order          */
     6,                                 /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,   /* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,             /* clock speed              */
     GT_FALSE,                          /* repeater mode            */
     GT_HG_NOT_VALID_VCM                /* voltage mode             */
    },
    /*lane 6 */
    {6,                                 /* fport                    */
     GT_TRUE,                           /* Fport is valid           */
     GT_FALSE,                          /* swap lane order          */
     6,                                 /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,   /* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,             /* clock speed              */
     GT_FALSE,                          /* repeater mode            */
     GT_HG_NOT_VALID_VCM                /* voltage mode             */
    },
    /*lane 7 */
    {7,                                 /* fport                    */
     GT_TRUE,                           /* Fport is valid           */
     GT_FALSE,                          /* swap lane order          */
     6,                                 /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,   /* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,             /* clock speed              */
     GT_FALSE,                          /* repeater mode            */
     GT_HG_NOT_VALID_VCM                /* voltage mode             */
    },
    /*lane 8 */
    {8,                                 /* fport                    */
     GT_TRUE,                           /* Fport is valid           */
     GT_FALSE,                          /* swap lane order          */
     6,                                 /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,   /* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,             /* clock speed              */
     GT_FALSE,                          /* repeater mode            */
     GT_HG_NOT_VALID_VCM                /* voltage mode             */
    },
    /*lane 9 */
    {9,                                 /* fport                    */
     GT_TRUE,                           /* Fport is valid           */
     GT_FALSE,                          /* swap lane order          */
     6,                                 /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,   /* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,             /* clock speed              */
     GT_FALSE,                          /* repeater mode            */
     GT_HG_NOT_VALID_VCM                /* voltage mode             */
    },
    /*lane 10 */
    {10,                                /* fport                    */
     GT_TRUE,                           /* Fport is valid           */
     GT_FALSE,                          /* swap lane order          */
     6,                                 /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,   /* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,             /* clock speed              */
     GT_FALSE,                          /* repeater mode            */
     GT_HG_NOT_VALID_VCM                /* voltage mode             */
    },
    /*lane 11 */
    {11,                                /* fport                    */
     GT_TRUE,                           /* Fport is valid           */
     GT_FALSE,                          /* swap lane order          */
     6,                                 /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,   /* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,             /* clock speed              */
     GT_FALSE,                          /* repeater mode            */
     GT_HG_NOT_VALID_VCM                /* voltage mode             */
    }

};

static const GT_HG_INIT_PARAMS hgDefInitParamsGtFxFX9110[8] =
{   /*lane 0 */
    {0,                                 /* fport                    */
     GT_TRUE,                           /* Fport is valid           */
     GT_FALSE,                          /* swap lane order          */
     6,                                 /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,   /* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,             /* clock speed              */
     GT_FALSE,                          /* repeater mode            */
     GT_HG_NOT_VALID_VCM                /* voltage mode             */
    },
    /*lane 1 */
    {1,                                 /* fport                    */
     GT_TRUE,                           /* Fport is valid           */
     GT_FALSE,                          /* swap lane order          */
     6,                                 /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,   /* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,             /* clock speed              */
     GT_FALSE,                          /* repeater mode            */
     GT_HG_NOT_VALID_VCM                /* voltage mode             */
    },
    /*lane 2 */
    {2,                                 /* fport                    */
     GT_TRUE,                           /* Fport is valid           */
     GT_FALSE,                          /* swap lane order          */
     6,                                 /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,   /* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,             /* clock speed              */
     GT_FALSE,                          /* repeater mode            */
     GT_HG_NOT_VALID_VCM                /* voltage mode             */
    },
    /*lane 3 */
    {3,                                 /* fport                    */
     GT_TRUE,                           /* Fport is valid           */
     GT_FALSE,                          /* swap lane order          */
     6,                                 /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,   /* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,             /* clock speed              */
     GT_FALSE,                          /* repeater mode            */
     GT_HG_NOT_VALID_VCM                /* voltage mode             */
    },
    /*lane 4 */
    {4,                                 /* fport                    */
     GT_TRUE,                           /* Fport is valid           */
     GT_FALSE,                          /* swap lane order          */
     6,                                 /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,   /* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,             /* clock speed              */
     GT_FALSE,                          /* repeater mode            */
     GT_HG_NOT_VALID_VCM                /* voltage mode             */
    },
    /*lane 5 */
    {5,                                 /* fport                    */
     GT_TRUE,                           /* Fport is valid           */
     GT_FALSE,                          /* swap lane order          */
     6,                                 /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,   /* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,             /* clock speed              */
     GT_FALSE,                          /* repeater mode            */
     GT_HG_NOT_VALID_VCM                /* voltage mode             */
    },
    /*lane 6 */
    {6,                                 /* fport                    */
     GT_TRUE,                           /* Fport is valid           */
     GT_FALSE,                          /* swap lane order          */
     6,                                 /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,   /* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,             /* clock speed              */
     GT_FALSE,                          /* repeater mode            */
     GT_HG_NOT_VALID_VCM                /* voltage mode             */
    },
    /*lane 7 */
    {7,                                 /* fport                    */
     GT_TRUE,                           /* Fport is valid           */
     GT_FALSE,                          /* swap lane order          */
     6,                                 /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,   /* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,             /* clock speed              */
     GT_FALSE,                          /* repeater mode            */
     GT_HG_NOT_VALID_VCM                /* voltage mode             */
    }
};

    /* 6-Port Xbar Cappoiera 98FX9206 */
static const GT_HG_INIT_PARAMS hgDefInitParamsGtFxFX9206[6] =
{   /*lane 0 */
    {0,                                 /* fport                    */
     GT_TRUE,                           /* Fport is valid           */
     GT_FALSE,                          /* swap lane order          */
     6,                                 /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,   /* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,             /* clock speed              */
     GT_FALSE,                          /* repeater mode            */
     GT_HG_NOT_VALID_VCM                /* voltage mode             */
    },
    /*lane 1 */
    {1,                                 /* fport                    */
     GT_TRUE,                           /* Fport is valid           */
     GT_FALSE,                          /* swap lane order          */
     6,                                 /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,   /* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,             /* clock speed              */
     GT_FALSE,                          /* repeater mode            */
     GT_HG_NOT_VALID_VCM                /* voltage mode             */
    },
    /*lane 5 */
    {5,                                 /* fport                    */
     GT_TRUE,                           /* Fport is valid           */
     GT_FALSE,                          /* swap lane order          */
     6,                                 /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,   /* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,             /* clock speed              */
     GT_FALSE,                          /* repeater mode            */
     GT_HG_NOT_VALID_VCM                /* voltage mode             */
    },
    /*lane 7 */
    {7,                                 /* fport                    */
     GT_TRUE,                           /* Fport is valid           */
     GT_FALSE,                          /* swap lane order          */
     6,                                 /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,   /* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,             /* clock speed              */
     GT_FALSE,                          /* repeater mode            */
     GT_HG_NOT_VALID_VCM                /* voltage mode             */
    },
    /*lane 8 */
    {8,                                 /* fport                    */
     GT_TRUE,                           /* Fport is valid           */
     GT_FALSE,                          /* swap lane order          */
     6,                                 /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,   /* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,             /* clock speed              */
     GT_FALSE,                          /* repeater mode            */
     GT_HG_NOT_VALID_VCM                /* voltage mode             */
    },
    /*lane 11 */
    {11,                                /* fport                    */
     GT_TRUE,                           /* Fport is valid           */
     GT_FALSE,                          /* swap lane order          */
     6,                                 /* Number of hg links lanes */
     GT_HG_DRIVER_PRE_EMPHASIS_33_PC,   /* pre emphasis             */
     GT_HG_CLOCK_3_125_GHZ,             /* clock speed              */
     GT_FALSE,                          /* repeater mode            */
     GT_HG_NOT_VALID_VCM                /* voltage mode             */
    }
};


/*******************************************************************************
* Core Xbar Hw Init related functions
*******************************************************************************/
/*******************************************************************************
* appDemoHgGetDefInitParams
*
* DESCRIPTION:
*       Retrieve the initialization parameters
*
* INPUTS:
*     xbarDevType - The crossbar device type
*     hgParamsLen - The number of elements in hgParams[] array.  This should
*                   be the maximum HyperG.link port on the device.
*
* OUTPUTS:
*     hgParamsLen - List length of initialization parameters.
*     hgParams  -   The HyperG.Link fabric port initialization paramters
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*
* COMMENTS:
*       The HyperG.Link fabric ports are initialized by the
*       crossbar initialiation API: coreXbarHwPhase1Init()
*
* GalTis:
*       Command - none.
*
*******************************************************************************/
GT_STATUS appDemoHgGetDefInitParams
(
    IN      GT_XBAR_DEVICE       xbarDevType,
    INOUT   GT_U8                *hgParamsLen,
    OUT     GT_HG_INIT_PARAMS    hgParams[]
)
{
    if ( (hgParamsLen == NULL) || (hgParams == NULL) )
        return (GT_BAD_PARAM);

    switch (xbarDevType)
    {
        case GT_XBAR_98FX900A:
        case GT_XBAR_98FX9010A:

            if ( (*hgParamsLen) < 4 )
                return (GT_BAD_SIZE);

            osMemCpy(hgParams,hgDefInitParamsGtFx900,
                     4 * sizeof(GT_HG_INIT_PARAMS));
            *hgParamsLen = 4;
            break;

        case GT_XBAR_98FX902A:

            if ( (*hgParamsLen) < 4 )
                return (GT_BAD_SIZE);

            osMemCpy(hgParams,hgDefInitParamsGtFx902,
                     4 * sizeof(GT_HG_INIT_PARAMS));
            *hgParamsLen = 4;
            break;

        case GT_XBAR_98FX910A:
        case GT_XBAR_98FX915A:

            if ( (*hgParamsLen) < 4 )
                return (GT_BAD_SIZE);

            osMemCpy(hgParams,hgDefInitParamsGtFx910,
                     4 * sizeof(GT_HG_INIT_PARAMS));
            *hgParamsLen = 4;
            break;

        case GT_XBAR_98FX9110:

            if ( (*hgParamsLen) < 8 )
                return (GT_BAD_SIZE);

            osMemCpy(hgParams,hgDefInitParamsGtFxFX9110,
                     8 * sizeof(GT_HG_INIT_PARAMS));
            *hgParamsLen = 8;
            break;

        case GT_XBAR_98FX9210:

            if ( (*hgParamsLen) < 12 )
                return (GT_BAD_SIZE);

            osMemCpy(hgParams,hgDefInitParamsGtFxFX9210,
                     12 * sizeof(GT_HG_INIT_PARAMS));
            *hgParamsLen = 12;
            break;

        case GT_XBAR_98FX9206:

            if ( (*hgParamsLen) < 6 )
                return (GT_BAD_SIZE);

            osMemCpy(hgParams,hgDefInitParamsGtFxFX9206,
                     6 * sizeof(GT_HG_INIT_PARAMS));
            *hgParamsLen = 6;
            break;

        default:
            return (GT_BAD_PARAM);

    }
    return GT_OK;
}


/*******************************************************************************
* appDemoGetXbarPhase1InitParams
*
* DESCRIPTION:
*       Returns Xbar init params.
*
* INPUTS:
*       devNum     - The device number to get the params for.
*       xbarFamily - Xbar family type FOX or CAP.
*
* OUTPUTS:
*       xbarInitParams - The xbar init params struct.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
* Galtis:
*       None.
*
* Toolkit:
*
*******************************************************************************/
GT_STATUS appDemoGetXbarPhase1InitParams
(
    IN  GT_U8                       devNum,
    IN  GT_XBAR_DEV_FAMILY          xbarFamily,
    OUT GT_XBAR_PHASE1_INIT_PARAMS  *xbarInitParams
)
{
    GT_XBAR_PHASE1_INIT_PARAMS  localFoxPhase1Params =
                                GT_XBAR_FOX_PHASE1_DEF_PARAMS;
    GT_XBAR_PHASE1_INIT_PARAMS  localCapPhase1Params =
                                GT_XBAR_CAP_PHASE1_DEF_PARAMS;


    if (xbarInitParams == NULL)
        return (GT_BAD_PARAM);

    /* switch on XBAR family types */
    switch (xbarFamily)
    {
        case (GT_XBAR_FOX_FAMILY):
            /* Copy the parameters */
            osMemCpy(xbarInitParams,&localFoxPhase1Params,
                                        sizeof(localFoxPhase1Params) );

            break;

        case (GT_XBAR_CAP_FAMILY):
            /* Copy the parameters */
            osMemCpy(xbarInitParams,&localCapPhase1Params,
                                    sizeof(localCapPhase1Params) );

            break;

        default:
            return (GT_BAD_PARAM);

    }


    xbarInitParams->devNum = devNum;
    xbarInitParams->hgParamsListLen = 0;
    xbarInitParams->hgParamsList = NULL;

    return GT_OK;
}

/*******************************************************************************
* appDemoSetXbarPhase1InitParams
*
* DESCRIPTION:
*       Performs Xbar initialization.
*
* INPUTS:
*       xbarInitParams - The xbar init params struct.
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
* Galtis:
*       None.
*
* Toolkit:
*
*******************************************************************************/
GT_STATUS appDemoSetXbarPhase1InitParams
(
    IN  GT_XBAR_PHASE1_INIT_PARAMS  *xbarInitParams
)
{
    GT_STATUS             status;
    GT_XBAR_DEVICE        deviceType;

    status = coreXbarHwPhase1Init(xbarInitParams,&deviceType);
    if(status != GT_OK)
    {
        return status;
    }


    osMemCpy(&(appDemoXbarConfigList[xbarInitParams->devNum].phase1Params),
             xbarInitParams,sizeof(GT_XBAR_PHASE1_INIT_PARAMS));

    appDemoXbarConfigList[xbarInitParams->devNum].valid         = GT_TRUE;
    appDemoXbarConfigList[xbarInitParams->devNum].xbarInitDone  = GT_TRUE;

    return GT_OK;
}


/*******************************************************************************
* appDemoSetXbarPhase2InitParams
*
* DESCRIPTION:
*       Performs Xbar initialization.
*
* INPUTS:
*       xbarInitParams - The xbar init params struct.
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
* Galtis:
*       None.
*
* Toolkit:
*
*******************************************************************************/
GT_STATUS appDemoSetXbarPhase2InitParams
(
    IN  GT_XBAR_PHASE2_INIT_PARAMS  *xbarInitParams
)
{
    GT_STATUS           status;

    appDemoXbarConfigList[xbarInitParams->devNum].oldDevNum =
        xbarInitParams->oldDevNum;
    appDemoXbarConfigList[xbarInitParams->devNum].devNum=xbarInitParams->devNum;

    status = coreXbarHwPhase2Init(xbarInitParams);
    if(status != GT_OK)
    {
        return status;
    }

    return GT_OK;
}

#if 0 /* [FINDME][SergeyR]: removed since there are a lot of TAPI code inside */
/*******************************************************************************
* appDemoFaXbarLocalConfig
*
* DESCRIPTION: Initialize the XBAR and FA for the demo. This function will
*              configure the routing tables to the XBARs and
*              the VOQs of the FA to enable bridgind/routing packets
*              from the to its own ports via FA and XBAR instead of loacl
*              switching.
*              This function does not configure the PP itself.
*
* INPUTS:
*           xbarDevNum  : device number of PP/FA/XBAR to configure
* OUTPUTS:
*           None.
*
* RETURNS :
*           GT_OK        - on success,
*           GT_FAIL      - Operation failed.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS appDemoFaXbarLocalConfig
(
    IN GT_U8 xbarDevNum
)
{
    GT_STATUS rc;
    GT_U16    i;
    GT_U8     mcFports[5] = {4,0,0,0,0};
    GT_U8     fport[LBH_SIZE] = {4,4,4,4};
    GT_U32    devList[1];

    devList[0] = xbarDevNum;

    /* enabling traffic via upLink */
    devList[0] = xbarDevNum;
    rc = sysIngressTraffic2UplinkEnable(
        1, &(devList[0]), GT_TRUE);
    if (rc != GT_OK)
        return (GT_FAIL);

    /* Enabling dequing of multicast VOQs */
    rc = gtFaVoqMcTxEn(xbarDevNum, GT_TRUE);
    if (rc != GT_OK)
        return (GT_FAIL);

    /* Uc packets mapped to VOQ 4 */
    /* Configure the VOQ target device table */
    rc = gtFaVoqSetDevTbl(xbarDevNum, xbarDevNum, GT_TRUE);
    if (rc != GT_OK)
        return (GT_FAIL);

    /* Enable dequing of fabric port */
    rc = gtFaVoqFportTxEnable(xbarDevNum, 4, GT_TRUE);
    if (rc != GT_OK)
        return (GT_FAIL);


    /* sets the VOQ target device to fabric port table */
    rc = gtFaVoqDevToFportMap(xbarDevNum, xbarDevNum, fport);
    if (rc != GT_OK)
        return (GT_FAIL);

    /* Enabling the local Fa Port */
    rc = gtXbarFportEnable(xbarDevNum, 4, GT_TRUE);
    if (rc != GT_OK)
        return (GT_FAIL);

    /* disable loopback on internal port */
    rc = gtXbarLocalSwitchingEn(xbarDevNum, 4, GT_TRUE);
    if (rc != GT_OK)
        return (GT_FAIL);

    /* Configure the XBAR unicast routing table for the local port */
    rc = gtXbarSetUcRoute(xbarDevNum, 4, 4, xbarDevNum, 0);
    if (rc != GT_OK)
        return (GT_FAIL);

    /* Configure the multicast routing tables in the xbars for all vidxs    */
    /* 0 -4095                                                              */
    for (i = 0 ; (i < 4095) ; i++)
    {
        /* Set multcast on all the Fa ports */
        rc = gtXbarSetMcRoute(xbarDevNum, i, mcFports, 1);
        if (rc != GT_OK)
            return (GT_FAIL);
    }

    return GT_OK;
}

/*******************************************************************************
* appDemoFaXbarConfig
*
* DESCRIPTION: Initialize the XBARS and FAs for the demo. This function will
*              configure the routing tables to the XBARs. Also it will configure
*              the VOQs of the FAs
*
* INPUTS:
*           xbarDevNum  : device number of FA/XBAR to configure
*           inOutFaPort : In and out FA port - leads to targetDev
*           targetDev   : remote Device Id
* OUTPUTS:
*           None.
*
* RETURNS :
*           GT_OK        - on success,
*           GT_FAIL      - Operation failed.
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS appDemoFaXbarConfig
(
    IN GT_U8 xbarDevNum,
    IN GT_U8 inOutFaPort,
    IN GT_U8 targetDev
)
{
    GT_STATUS rc;
    GT_U16    i;
    GT_U8     mcFports[5] = {4,0,0,0,0};
    GT_U8     fport[LBH_SIZE] = {0,0,0,0};

    /* local switchinc via FA/XBAR */
    rc = appDemoFaXbarLocalConfig(xbarDevNum);
    if (rc != GT_OK)
        return (GT_FAIL);

    if ((xbarDevNum == targetDev) && (inOutFaPort == 4))
    {
        return GT_OK;
    }

    /* Enabling the in out FA port */
    rc = gtXbarFportEnable(xbarDevNum,inOutFaPort,GT_TRUE);
    if (rc != GT_OK)
        return (GT_FAIL);

    /* Configure the XBAR unicast routing table for the local port */
    rc = gtXbarSetUcRoute(xbarDevNum,inOutFaPort,4,xbarDevNum,0);
    if (rc != GT_OK)
        return (GT_FAIL);

    /* Configure the XBAR unicast routing table */
    rc = gtXbarSetUcRoute(xbarDevNum,4,inOutFaPort,
                              targetDev,0);
    if (rc != GT_OK)
        return (GT_FAIL);



    mcFports[1] = inOutFaPort;
    /* Configure the multicast routing tables in the xbars for all vidxs    */
    /* 0 -4095                                                              */
    for (i = 0 ; i < 4095 ; i++)
    {
        /* Set multcast on all the Fa ports */
        rc = gtXbarSetMcRoute(xbarDevNum,i,mcFports,2);
        if (rc != GT_OK)
            return (GT_FAIL);
    }

    /* Enable dequing of fabric port */
    rc = gtFaVoqFportTxEnable(xbarDevNum, inOutFaPort, GT_TRUE);
    if (rc != GT_OK)
        return (GT_FAIL);

    /* For all the ports in the xbar */
    for (i = 0 ; (i < 4) ; i++ )
    {
        /* Disable local switching */
        rc = gtXbarLocalSwitchingEn(xbarDevNum,(GT_U8)i,GT_FALSE);
        if (rc != GT_OK)
            return (GT_FAIL);

    }

    /* Configure the VOQ target device table */
    rc = gtFaVoqSetDevTbl(xbarDevNum,
                          targetDev,
                          GT_TRUE);
    if (rc != GT_OK)
        return (GT_FAIL);

    fport[0] = inOutFaPort;
    /* sets the VOQ target device to fabric port table */
    rc = gtFaVoqDevToFportMap(xbarDevNum,
                              targetDev,
                              fport);
    if (rc != GT_OK)
        return (GT_FAIL);


    return (GT_OK);
}
#endif /* 0 */

