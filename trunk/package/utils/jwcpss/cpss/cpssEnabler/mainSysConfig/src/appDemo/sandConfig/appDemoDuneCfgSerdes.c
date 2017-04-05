/*******************************************************************************
*              Copyright 2002, GALILEO TECHNOLOGY, LTD.
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
* appDemoDuneCfgSerdes.c
*
* DESCRIPTION:
*       Includes initialization functions for Dune system to be used by the
*       application demo configuration functions.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#include <appDemo/sandConfig/appDemoFe200Config.h>

/*===========================================================================*/
/*                                 SERDES                                    */
/*===========================================================================*/

#define FAP10M_NUMBER_OF_LINKS 9

/*
 * These defines should be in H file to common use (See also utils_fe200.c)
 */
#define NOT_VALID       (unsigned char)0
#define VALID           (unsigned char)1
#define VCMS_00         (unsigned char)0
#define VCMS_01         (unsigned char)1
#define VCMS_02         (unsigned char)2
#define VCMS_03         (unsigned char)3
#define MEN_00          (unsigned char)0
#define MEN_01          (unsigned char)1
#define MEN_02          (unsigned char)2
#define MEN_03          (unsigned char)3
#define PEN_00          (unsigned char)0
#define PEN_01          (unsigned char)1
#define PEN_02          (unsigned char)2
#define PEN_03          (unsigned char)3
#define AMP_00          (unsigned char)0
#define AMP_01          (unsigned char)1
#define AMP_02          (unsigned char)2
#define AMP_03          (unsigned char)3
#define AMP_04          (unsigned char)4
#define AMP_05          (unsigned char)5
#define AMP_06          (unsigned char)6
#define AMP_07          (unsigned char)7
#define POWER_UP        (unsigned char)0
#define POWER_DOWN      (unsigned char)1
#define NOT_RESET       (unsigned char)0 /* the same that POWER_UP   */
#define RESET           (unsigned char)1 /* the same that POWER_DOWN */

/* COMPRESSED SERDES PARAMETERS */
/* name    bits-length bits-offset     */
/* VALID   1           0               */
/* VCMS    2           1               */
/* MEN     2           3               */
/* PEN     2           5               */
/* AMP     3           7               */
/* POWER   1           10              */
/* total:              11              */

/* PACKED_SP - packed serdes parameters */
typedef GT_U16 PACKED_SP;

#define PACK_VALID(_v)      ((_v) & 1)
#define UNPACK_VALID(_v)    ((_v) & 1)
#define PACK_VCMS(_v)       (((_v) & 3) << 1)
#define UNPACK_VCMS(_v)     (((_v) >> 1) & 3)
#define PACK_MEN(_v)        (((_v) & 3) << 3)
#define UNPACK_MEN(_v)      (((_v) >> 3) & 3)
#define PACK_PEN(_v)        (((_v) & 3) << 5)
#define UNPACK_PEN(_v)      (((_v) >> 5) & 3)
#define PACK_AMP(_v)        (((_v) & 7) << 7)
#define UNPACK_AMP(_v)      (((_v) >> 7) & 7)
#define PACK_POWER(_v)      (((_v) & 1) << 10)
#define UNPACK_POWER(_v)    (((_v) >> 10) & 1)

#define PACK_SP(_valid, _vcmc, _men, _pen, _amp, _power) \
    (PACKED_SP)( \
    PACK_VALID(_valid) \
    | PACK_VCMS(_vcmc) \
    | PACK_MEN(_men) \
    | PACK_PEN(_pen) \
    | PACK_AMP(_amp) \
    | PACK_POWER(_power) \
    )

static void unpackSp
(
    IN  PACKED_SP            packedSp,
    OUT SINGLE_SERDES_STATUS *spPtr
)
{
    spPtr->valid            = UNPACK_VALID(packedSp);
    spPtr->common_mode      = UNPACK_VCMS(packedSp);
    spPtr->main_control     = UNPACK_MEN(packedSp);
    spPtr->pre_emphasis     = UNPACK_PEN(packedSp);
    spPtr->amplitude        = UNPACK_AMP(packedSp);
    spPtr->power_down_rx_tx = UNPACK_POWER(packedSp);
}

#define NUM_FAPS_ON_BRD0012A 1
#define NUM_FES_ON_BRD002A   2

static PACKED_SP defaultSP =
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_07, POWER_UP);


/* Negev - 2 possible slots, 1 fap10m on each card */
static PACKED_SP Negev_FAP_BRD0012A_BRD0002A_serdes_params[2]
    [NUM_FAPS_ON_BRD0012A]
    [FAP10M_NUMBER_OF_LINKS] =
{
  {/* Slot 1 */
    {/* FAP0 */
      PACK_SP(VALID, VCMS_02, MEN_01, PEN_02, AMP_04, POWER_UP),  /* serdes 0  */
      PACK_SP(VALID, VCMS_02, MEN_01, PEN_01, AMP_04, POWER_UP),  /* serdes 1  */
      PACK_SP(VALID, VCMS_02, MEN_00, PEN_02, AMP_03, POWER_UP),  /* serdes 2  */
      PACK_SP(VALID, VCMS_02, MEN_00, PEN_02, AMP_03, POWER_UP),  /* serdes 3  */
      PACK_SP(VALID, VCMS_02, MEN_01, PEN_02, AMP_03, POWER_UP),  /* serdes 4  */
      PACK_SP(VALID, VCMS_02, MEN_01, PEN_02, AMP_04, POWER_UP),  /* serdes 5  */
      PACK_SP(VALID, VCMS_02, MEN_01, PEN_02, AMP_04, POWER_UP),  /* serdes 6  */
      PACK_SP(VALID, VCMS_02, MEN_00, PEN_03, AMP_04, POWER_UP),  /* serdes 7  */
      PACK_SP(VALID, VCMS_02, MEN_01, PEN_02, AMP_04, POWER_UP),  /* serdes 8  */
    }
  },
  {/* Slot 2 */
    {/* FAP0 */
      PACK_SP(VALID, VCMS_02, MEN_01, PEN_02, AMP_01, POWER_UP),  /* serdes 0  */
      PACK_SP(VALID, VCMS_02, MEN_01, PEN_01, AMP_04, POWER_UP),  /* serdes 1  */
      PACK_SP(VALID, VCMS_02, MEN_00, PEN_01, AMP_04, POWER_UP),  /* serdes 2  */
      PACK_SP(VALID, VCMS_02, MEN_01, PEN_01, AMP_04, POWER_UP),  /* serdes 3  */
      PACK_SP(VALID, VCMS_02, MEN_00, PEN_02, AMP_04, POWER_UP),  /* serdes 4  */
      PACK_SP(VALID, VCMS_02, MEN_00, PEN_02, AMP_01, POWER_UP),  /* serdes 5  */
      PACK_SP(VALID, VCMS_02, MEN_00, PEN_02, AMP_03, POWER_UP),  /* serdes 6  */
      PACK_SP(VALID, VCMS_02, MEN_00, PEN_02, AMP_02, POWER_UP),  /* serdes 7  */
      PACK_SP(VALID, VCMS_02, MEN_00, PEN_02, AMP_04, POWER_UP),  /* serdes 8  */
    }
  }
} ;

/* Gobi - 16 possible slots, 1 fap10m on each card */
static PACKED_SP Gobi_FAP_BRD0012A_BRD0002A_serdes_params[16]
    [NUM_FAPS_ON_BRD0012A]
    [FAP10M_NUMBER_OF_LINKS] =
{
     /*VALID, VCMS, MEN, PEN, AMP, POWER_DOWN */
  {/* Slot 1 */
    {/* FAP0 */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 0  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 1  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 2  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 3  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_UP),  /* serdes 4  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 5  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_UP),  /* serdes 6  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_UP),  /* serdes 7  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_DOWN),  /* serdes 8  */
    }
  },
  { /* Slot 2 */
    {/* FAP0 */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 0  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 1  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 2  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 3  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_UP),  /* serdes 4  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_UP),  /* serdes 5  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_UP),  /* serdes 6  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_UP),  /* serdes 7  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_DOWN),  /* serdes 8  */
    }
  },
  {/* Slot 3 */
    {/* FAP0 */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 0  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 1  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 2  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 3  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_UP),  /* serdes 4  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 5  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_UP),  /* serdes 6  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_UP),  /* serdes 7  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_DOWN),  /* serdes 8  */
    }
  },
  {/* Slot 4 */
    {/* FAP0 */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 0  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 1  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 2  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 3  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 4  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 5  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_UP),  /* serdes 6  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_UP),  /* serdes 7  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_DOWN),  /* serdes 8  */
    }
  },
  /*VALID, VCMS, MEN, PEN, AMP, POWER_DOWN */
  {/* Slot 5 */
    {/* FAP0 */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 0  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 1  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 2  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 3  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 4  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 5  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_UP),  /* serdes 6  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 7  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_DOWN),  /* serdes 8  */
    }
  },
  {/* Slot 6 */
    {/* FAP0 */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 0  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 1  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 2  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 3  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 4  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 5  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 6  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 7  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_DOWN),  /* serdes 8  */
    }
  },
  {/* Slot 7 */
    {/* FAP0 */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 0  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 1  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 2  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 3  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 4  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 5  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 6  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 7  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_DOWN),  /* serdes 8  */
    }
  },
  {/* Slot 8 */
    {/* FAP0 */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 0  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 1  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 2  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 3  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 4  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 5  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 6  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 7  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_DOWN),  /* serdes 8  */
    }
  },
  {/* Slot 9 */
    {/* FAP0 */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 0  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 1  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 2  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 3  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 4  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 5  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 6  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 7  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_DOWN),  /* serdes 8  */
    }
  },
  /*VALID, VCMS, MEN, PEN, AMP, POWER_DOWN */
  {/* Slot 10 */
    {/* FAP0 */
      PACK_SP(VALID, VCMS_02, MEN_00, PEN_01, AMP_04, POWER_UP),  /* serdes 0  */
      PACK_SP(VALID, VCMS_02, MEN_00, PEN_01, AMP_04, POWER_UP),  /* serdes 1  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 2  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 3  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 4  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 5  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 6  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_UP),  /* serdes 7  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_DOWN),  /* serdes 8  */
    }
  },
  {/* Slot 11 */
    {/* FAP0 */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 0  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 1  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 2  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 3  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 4  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 5  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 6  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_UP),  /* serdes 7  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_DOWN),  /* serdes 8  */
    }
  },
  {/* Slot 12 */
    {/* FAP0 */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 0  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 1  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 2  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 3  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 4  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 5  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 6  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_UP),  /* serdes 7  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_DOWN),  /* serdes 8  */
    }
  },
  {/* Slot 13 */
    {/* FAP0 */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 0  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 1  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 2  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 3  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 4  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 5  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 6  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_UP),  /* serdes 7  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_DOWN),  /* serdes 8  */
    }
  },
  {/* Slot 14 */
    {/* FAP0 */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 0  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 1  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 2  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 3  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 4  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 5  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 6  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_UP),  /* serdes 7  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_DOWN),  /* serdes 8  */
    }
  },
  /*VALID, VCMS, MEN, PEN, AMP, POWER_DOWN */
  {/* Slot 15 */
    {/* FAP0 */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 0  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 1  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 2  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 3  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 4  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_UP),  /* serdes 5  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_UP),  /* serdes 6  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_UP),  /* serdes 7  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_DOWN),  /* serdes 8  */
    }
  },
  {/* Slot 16 */
    {/* FAP0 */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 0  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 1  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 2  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_01, AMP_04, POWER_UP),  /* serdes 3  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_02, AMP_04, POWER_UP),  /* serdes 4  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_UP),  /* serdes 5  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_UP),  /* serdes 6  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_UP),  /* serdes 7  */
      PACK_SP(VALID, VCMS_02, MEN_03, PEN_03, AMP_04, POWER_DOWN),  /* serdes 8  */
    }
  },
} ;

/* Negev - possible slots, 2 fes on each card */
static PACKED_SP Negev_FE_BRD0012A_BRD0002A_serdes_params[1]
    [NUM_FES_ON_BRD002A]
    [FE200_NUM_OF_SERDESES] =
{
  { /* slot 17 */
    { /* fe0 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET),  /* serdes 0  */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET),  /* serdes 1  */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET),  /* serdes 2  */
      PACK_SP(VALID, VCMS_03, MEN_01, PEN_02, AMP_05, NOT_RESET), /* serdes 3  - slot#2 link#7 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 4  */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 5  */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 6  */
      PACK_SP(VALID, VCMS_03, MEN_01, PEN_03, AMP_04, NOT_RESET), /* serdes 7  - slot#1 link#7 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 8  */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 9  */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 10 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_00, AMP_01, NOT_RESET), /* serdes 11 - slot#2 link#5 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 12 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 13 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 14 */
      PACK_SP(VALID, VCMS_03, MEN_01, PEN_00, AMP_03, NOT_RESET), /* serdes 15 - slot#1 link#5 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 16 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 17 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 18 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 19 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 20 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 21 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 22 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 23 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 24 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 25 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 26 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 27 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 28 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 29 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 30 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 31 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 32 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 33 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 34 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 35 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 36 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 37 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 38 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 39 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 40 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 41 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 42 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 43 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 44 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 45 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 46 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 47 */
      PACK_SP(VALID, VCMS_03, MEN_01, PEN_01, AMP_04, NOT_RESET), /* serdes 48 - slot#1 link#4 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 49 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 50 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 51 */
      PACK_SP(VALID, VCMS_03, MEN_01, PEN_00, AMP_03, NOT_RESET), /* serdes 52 - slot#2 link#4 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 53 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 54 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 55 */
      PACK_SP(VALID, VCMS_03, MEN_01, PEN_02, AMP_05, NOT_RESET), /* serdes 56 - slot#1 link#6 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 57 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 58 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 59 */
      PACK_SP(VALID, VCMS_03, MEN_01, PEN_01, AMP_04, NOT_RESET), /* serdes 60 - slot#2 link#6 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 61 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 62 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET)  /* serdes 63 */
    },
    { /* fe1 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 0  */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 1  */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 2  */
      PACK_SP(VALID, VCMS_03, MEN_01, PEN_02, AMP_05, NOT_RESET), /* serdes 3  - slot#2 link#8 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 4  */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 5  */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 6  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 7  - slot#1 link#8 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 8  */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 9  */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 10 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 11 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 12 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 13 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 14 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 15 */
      PACK_SP(VALID, VCMS_03, MEN_01, PEN_01, AMP_04, NOT_RESET), /* serdes 16 - slot#1 link#1 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 17 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 18 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 19 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_00, AMP_04, NOT_RESET), /* serdes 20 - slot#2 link#1 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 21 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 22 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 23 */
      PACK_SP(VALID, VCMS_03, MEN_01, PEN_00, AMP_04, NOT_RESET), /* serdes 24 - slot#1 link#3 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 25 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 26 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 27 */
      PACK_SP(VALID, VCMS_03, MEN_01, PEN_01, AMP_04, NOT_RESET), /* serdes 28 - slot#2 link#3 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 29 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 30 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 31 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 32 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 33 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 34 */
      PACK_SP(VALID, VCMS_03, MEN_01, PEN_00, AMP_04, NOT_RESET), /* serdes 35 - slot#2 link#2 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 36 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 37 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 38 */
      PACK_SP(VALID, VCMS_03, MEN_01, PEN_01, AMP_04, NOT_RESET), /* serdes 39 - slot#1 link#2 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 40 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 41 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 42 */
      PACK_SP(VALID, VCMS_03, MEN_01, PEN_01, AMP_04, NOT_RESET), /* serdes 43 - slot#2 link#0 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 44 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 45 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 46 */
      PACK_SP(VALID, VCMS_03, MEN_01, PEN_02, AMP_04, NOT_RESET), /* serdes 47 - slot#1 link#0 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 48 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 49 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 50 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 51 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 52 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 53 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 54 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 55 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 56 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 57 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 58 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 59 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 60 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 61 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET), /* serdes 62 */
      PACK_SP(VALID, VCMS_03, MEN_00, PEN_02, AMP_01, RESET)  /* serdes 63 */
    }
  }
} ;


static PACKED_SP Gobi_FE_BRD0012A_BRD0002A_serdes_params[8]
    [NUM_FES_ON_BRD002A]
    [FE200_NUM_OF_SERDESES] =
{
  { /* slot 17 */
    { /* fe0 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 0  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 1  - slot#4 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 2  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 3  - slot#3 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 4  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 5  - slot#2 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 6  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 7  - slot#1 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 8  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 9  - slot#8 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 10 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 11 - slot#7 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 12 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 13 - slot#6 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 14 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 15 - slot#5 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 16 - slot#13*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 17 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 18 - slot#14*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 19 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 20 - slot#15*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 21 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 22 - slot#16*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 23 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 24 - slot#9*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 25 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 26 - slot#10*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 27 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 28 - slot#11*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 29 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 30 - slot#12*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 31 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 32 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 33 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 34 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 35 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 36 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 37 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 38 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 39 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 40 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 41 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 42 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 43 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 44 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 45 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 46 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 47 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 48 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 49 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 50 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 51 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 52 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 53 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 54 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 55 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 56 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 57 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 58 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 59 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 60 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 61 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 62 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 63 */
    },
    { /* fe1 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 0  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 1  - slot#4 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 2  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 3  - slot#3 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 4  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 5  - slot#2 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 6  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 7  - slot#1 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 8  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 9  - slot#8 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 10 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 11 - slot#7 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 12 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 13 - slot#6 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 14 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 15 - slot#5 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 16 - slot#13*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 17 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 18 - slot#14*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 19 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 20 - slot#15*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 21 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 22 - slot#16*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 23 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 24 - slot#9*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 25 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 26 - slot#10*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 27 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 28 - slot#11*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 29 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 30 - slot#12*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 31 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 32 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 33 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 34 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 35 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 36 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 37 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 38 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 39 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 40 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 41 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 42 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 43 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 44 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 45 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 46 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 47 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 48 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 49 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 50 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 51 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 52 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 53 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 54 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 55 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 56 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 57 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 58 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 59 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 60 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 61 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 62 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 63 */
    }
  },
  { /* slot 18 */
    { /* fe0 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 0  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 1  - slot#4 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 2  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 3  - slot#3 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 4  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 5  - slot#2 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 6  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 7  - slot#1 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 8  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 9  - slot#8 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 10 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 11 - slot#7 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 12 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 13 - slot#6 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 14 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 15 - slot#5 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 16 - slot#13*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 17 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 18 - slot#14*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 19 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 20 - slot#15*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 21 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 22 - slot#16*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 23 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 24 - slot#9*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 25 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 26 - slot#10*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 27 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 28 - slot#11*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 29 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 30 - slot#12*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 31 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 32 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 33 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 34 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 35 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 36 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 37 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 38 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 39 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 40 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 41 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 42 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 43 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 44 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 45 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 46 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 47 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 48 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 49 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 50 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 51 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 52 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 53 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 54 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 55 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 56 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 57 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 58 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 59 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 60 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 61 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 62 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 63 */
    },
    { /* fe1 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 0  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 1  - slot#4 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 2  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 3  - slot#3 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 4  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 5  - slot#2 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 6  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 7  - slot#1 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 8  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 9  - slot#8 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 10 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 11 - slot#7 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 12 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 13 - slot#6 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 14 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 15 - slot#5 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 16 - slot#13*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 17 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 18 - slot#14*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 19 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 20 - slot#15*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 21 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 22 - slot#16*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 23 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 24 - slot#9*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 25 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 26 - slot#10*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 27 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 28 - slot#11*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 29 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 30 - slot#12*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 31 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 32 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 33 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 34 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 35 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 36 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 37 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 38 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 39 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 40 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 41 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 42 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 43 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 44 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 45 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 46 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 47 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 48 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 49 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 50 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 51 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 52 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 53 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 54 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 55 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 56 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 57 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 58 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 59 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 60 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 61 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 62 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 63 */
    }
  },
  { /* slot 19 */
    { /* fe0 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 0  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 1  - slot#4 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 2  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 3  - slot#3 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 4  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 5  - slot#2 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 6  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 7  - slot#1 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 8  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 9  - slot#8 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 10 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 11 - slot#7 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 12 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 13 - slot#6 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 14 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 15 - slot#5 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 16 - slot#13*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 17 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 18 - slot#14*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 19 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 20 - slot#15*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 21 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 22 - slot#16*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 23 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 24 - slot#9*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 25 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 26 - slot#10*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 27 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 28 - slot#11*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 29 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 30 - slot#12*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 31 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 32 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 33 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 34 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 35 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 36 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 37 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 38 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 39 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 40 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 41 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 42 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 43 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 44 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 45 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 46 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 47 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 48 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 49 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 50 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 51 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 52 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 53 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 54 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 55 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 56 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 57 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 58 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 59 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 60 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 61 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 62 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 63 */
    },
    { /* fe1 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 0  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 1  - slot#4 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 2  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 3  - slot#3 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 4  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 5  - slot#2 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 6  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 7  - slot#1 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 8  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 9  - slot#8 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 10 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 11 - slot#7 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 12 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 13 - slot#6 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 14 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 15 - slot#5 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 16 - slot#13*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 17 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 18 - slot#14*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 19 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 20 - slot#15*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 21 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 22 - slot#16*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 23 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 24 - slot#9*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 25 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 26 - slot#10*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 27 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 28 - slot#11*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 29 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 30 - slot#12*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 31 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 32 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 33 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 34 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 35 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 36 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 37 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 38 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 39 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 40 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 41 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 42 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 43 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 44 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 45 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 46 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 47 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 48 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 49 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 50 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 51 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 52 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 53 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 54 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 55 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 56 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 57 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 58 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 59 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 60 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 61 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 62 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 63 */
    }
  },
  { /* slot 20 */
    { /* fe0 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 0  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 1  - slot#4 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 2  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 3  - slot#3 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 4  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 5  - slot#2 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 6  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 7  - slot#1 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 8  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 9  - slot#8 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 10 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 11 - slot#7 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 12 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 13 - slot#6 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 14 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 15 - slot#5 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 16 - slot#13*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 17 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 18 - slot#14*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 19 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 20 - slot#15*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 21 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 22 - slot#16*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 23 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 24 - slot#9*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 25 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 26 - slot#10*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 27 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 28 - slot#11*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 29 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 30 - slot#12*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 31 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 32 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 33 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 34 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 35 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 36 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 37 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 38 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 39 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 40 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 41 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 42 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 43 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 44 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 45 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 46 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 47 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 48 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 49 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 50 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 51 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 52 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 53 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 54 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 55 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 56 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 57 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 58 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 59 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 60 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 61 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 62 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 63 */
    },
    { /* fe1 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 0  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 1  - slot#4 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 2  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 3  - slot#3 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 4  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 5  - slot#2 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 6  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 7  - slot#1 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 8  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 9  - slot#8 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 10 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 11 - slot#7 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 12 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 13 - slot#6 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 14 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 15 - slot#5 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 16 - slot#13*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 17 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 18 - slot#14*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 19 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 20 - slot#15*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 21 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 22 - slot#16*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 23 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 24 - slot#9*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 25 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 26 - slot#10*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 27 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 28 - slot#11*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 29 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 30 - slot#12*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 31 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 32 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 33 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 34 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 35 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 36 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 37 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 38 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 39 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 40 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 41 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 42 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 43 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 44 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 45 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 46 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 47 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 48 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 49 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 50 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 51 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 52 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 53 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 54 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 55 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 56 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 57 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 58 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 59 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 60 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 61 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 62 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 63 */
    }
  },
  { /* slot 21 */
    {/* fe0 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 0  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 1  - slot#4 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 2  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 3  - slot#3 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 4  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 5  - slot#2 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 6  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 7  - slot#1 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 8  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 9  - slot#8 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 10 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 11 - slot#7 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 12 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 13 - slot#6 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 14 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 15 - slot#5 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 16 - slot#13*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 17 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 18 - slot#14*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 19 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 20 - slot#15*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 21 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 22 - slot#16*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 23 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 24 - slot#9*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 25 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 26 - slot#10*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 27 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 28 - slot#11*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 29 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 30 - slot#12*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 31 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 32 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 33 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 34 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 35 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 36 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 37 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 38 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 39 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 40 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 41 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 42 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 43 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 44 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 45 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 46 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 47 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 48 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 49 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 50 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 51 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 52 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 53 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 54 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 55 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 56 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 57 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 58 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 59 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 60 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 61 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 62 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 63 */
    },
    { /* fe1 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 0  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 1  - slot#4 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 2  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 3  - slot#3 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 4  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 5  - slot#2 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 6  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 7  - slot#1 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 8  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 9  - slot#8 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 10 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 11 - slot#7 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 12 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 13 - slot#6 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 14 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 15 - slot#5 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 16 - slot#13*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 17 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 18 - slot#14*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 19 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 20 - slot#15*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 21 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 22 - slot#16*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 23 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 24 - slot#9*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 25 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 26 - slot#10*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 27 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 28 - slot#11*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 29 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_01, AMP_04, NOT_RESET), /* serdes 30 - slot#12*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 31 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 32 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 33 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 34 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 35 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 36 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 37 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 38 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 39 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 40 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 41 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 42 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 43 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 44 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 45 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 46 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 47 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 48 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 49 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 50 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 51 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 52 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 53 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 54 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 55 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 56 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 57 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 58 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 59 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 60 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 61 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 62 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 63 */
    }
  },
  { /* slot 22 */
    { /* fe0 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 0  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 1  - slot#4 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 2  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 3  - slot#3 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 4  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 5  - slot#2 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 6  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 7  - slot#1 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 8  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 9  - slot#8 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 10 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 11 - slot#7 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 12 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 13 - slot#6 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 14 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 15 - slot#5 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 16 - slot#13*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 17 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 18 - slot#14*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 19 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 20 - slot#15*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 21 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 22 - slot#16*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 23 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 24 - slot#9*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 25 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 26 - slot#10*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 27 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 28 - slot#11*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 29 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 30 - slot#12*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 31 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 32 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 33 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 34 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 35 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 36 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 37 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 38 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 39 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 40 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 41 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 42 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 43 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 44 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 45 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 46 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 47 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 48 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 49 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 50 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 51 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 52 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 53 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 54 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 55 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 56 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 57 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 58 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 59 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 60 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 61 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 62 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 63 */
    },
    { /* fe1 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 0  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 1  - slot#4 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 2  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 3  - slot#3 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 4  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 5  - slot#2 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 6  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 7  - slot#1 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 8  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 9  - slot#8 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 10 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 11 - slot#7 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 12 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 13 - slot#6 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 14 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 15 - slot#5 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 16 - slot#13*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 17 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 18 - slot#14*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 19 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 20 - slot#15*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 21 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 22 - slot#16*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 23 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 24 - slot#9*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 25 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 26 - slot#10*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 27 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 28 - slot#11*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 29 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 30 - slot#12*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 31 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 32 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 33 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 34 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 35 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 36 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 37 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 38 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 39 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 40 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 41 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 42 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 43 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 44 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 45 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 46 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 47 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 48 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 49 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 50 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 51 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 52 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 53 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 54 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 55 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 56 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 57 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 58 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 59 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 60 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 61 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 62 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 63 */
    }
  },
  { /* slot 23 */
    { /* fe0 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 0  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 1  - slot#4 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 2  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 3  - slot#3 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 4  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 5  - slot#2 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 6  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 7  - slot#1 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 8  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 9  - slot#8 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 10 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 11 - slot#7 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 12 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 13 - slot#6 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 14 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 15 - slot#5 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 16 - slot#13*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 17 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 18 - slot#14*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 19 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 20 - slot#15*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 21 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 22 - slot#16*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 23 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 24 - slot#9*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 25 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 26 - slot#10*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 27 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 28 - slot#11*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 29 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 30 - slot#12*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 31 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 32 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 33 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 34 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 35 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 36 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 37 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 38 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 39 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 40 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 41 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 42 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 43 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 44 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 45 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 46 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 47 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 48 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 49 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 50 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 51 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 52 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 53 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 54 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 55 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 56 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 57 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 58 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 59 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 60 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 61 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 62 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 63 */
    },
    { /* fe1 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 0  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 1  - slot#4 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 2  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 3  - slot#3 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 4  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 5  - slot#2 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 6  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 7  - slot#1 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 8  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 9  - slot#8 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 10 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 11 - slot#7 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 12 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 13 - slot#6 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 14 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 15 - slot#5 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 16 - slot#13*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 17 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 18 - slot#14*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 19 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 20 - slot#15*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 21 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 22 - slot#16*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 23 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 24 - slot#9*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 25 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 26 - slot#10*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 27 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 28 - slot#11*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 29 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_02, AMP_04, NOT_RESET), /* serdes 30 - slot#12*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 31 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 32 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 33 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 34 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 35 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 36 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 37 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 38 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 39 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 40 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 41 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 42 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 43 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 44 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 45 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 46 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 47 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 48 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 49 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 50 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 51 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 52 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 53 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 54 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 55 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 56 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 57 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 58 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 59 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 60 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 61 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 62 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 63 */
    }
  },
  { /* slot 24 */
    { /* fe0 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 0  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 1  - slot#4 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 2  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 3  - slot#3 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 4  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 5  - slot#2 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 6  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 7  - slot#1 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 8  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 9  - slot#8 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 10 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 11 - slot#7 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 12 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 13 - slot#6 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 14 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 15 - slot#5 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 16 - slot#13*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 17 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 18 - slot#14*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 19 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 20 - slot#15*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 21 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 22 - slot#16*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 23 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 24 - slot#9*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 25 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 26 - slot#10*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 27 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 28 - slot#11*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 29 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 30 - slot#12*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 31 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 32 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 33 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 34 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 35 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 36 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 37 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 38 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 39 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 40 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 41 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 42 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 43 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 44 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 45 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 46 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 47 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 48 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 49 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 50 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 51 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 52 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 53 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 54 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 55 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 56 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 57 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 58 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 59 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 60 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 61 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 62 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 63 */
    },
    { /* fe0 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 0  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 1  - slot#4 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 2  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 3  - slot#3 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 4  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 5  - slot#2 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 6  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 7  - slot#1 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 8  */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 9  - slot#8 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 10 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 11 - slot#7 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 12 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 13 - slot#6 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 14 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 15 - slot#5 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 16 - slot#13*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 17 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 18 - slot#14*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 19 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 20 - slot#15*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 21 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 22 - slot#16*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 23 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 24 - slot#9*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 25 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 26 - slot#10*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 27 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 28 - slot#11*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 29 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, NOT_RESET), /* serdes 30 - slot#12*/
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET),     /* serdes 31 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 32 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 33 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 34 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 35 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 36 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 37 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 38 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 39 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 40 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 41 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 42 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 43 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 44 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 45 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 46 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 47 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 48 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 49 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 50 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 51 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 52 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 53 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 54 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 55 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 56 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 57 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 58 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 59 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 60 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 61 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 62 */
      PACK_SP(VALID, VCMS_03, MEN_03, PEN_03, AMP_04, RESET), /* serdes 63 */
    }
  }
} ;

/*******************************************************************************
* appDemoDuneGetSerdesParameters
*
* DESCRIPTION:
*       Gets parameters of serdes lane initial configuration.
*
* INPUTS:
*     chassysType    -   DUNE_CHASSY_NEGEV or DUNE_CHASSY_GOBY ...
*     fapBoardType   -   DUNE_BOARD_BRD0012A or another FAP card
*     feBoardType    -   DUNE_BOARD_BRD0002A (others not supported)
*     serdesOnFap    -   GT_FALSE - port is on FE card, otherwise on FAP
*     slotNum        -   number of slot (from 0) in FAP or in FE slots
*     devRelNum      -   relatine device number on the card
*     portNum        -   port number on the device
*
* OUTPUTS:
*       serdesStatusPtr  - see SINGLE_SERDES_STATUS description.
*
* RETURNS:
*       GT_STATUS value
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoDuneGetSerdesParameters
(
    IN  GT_U32               chassysType,
    IN  GT_U32               fapBoardType,
    IN  GT_U32               feBoardType,
    IN  GT_BOOL              serdesOnFap,
    IN  GT_U32               slotNum,
    IN  GT_U32               devRelNum,
    IN  GT_U32               portNum,
    OUT SINGLE_SERDES_STATUS *serdesStatusPtr
)
{
    GT_32 caseCode = (chassysType << 16)
        | (fapBoardType << 8)
        | (feBoardType << 1)
        | ((serdesOnFap != GT_FALSE) ? 1 : 0);

    /* default configuration */
    unpackSp(defaultSP, serdesStatusPtr);

    switch (caseCode)
    {
        /* FE port on Negev with BRD0012A and BRD0002A */
        case ((DUNE_CHASSY_NEGEV << 16) | (DUNE_BOARD_BRD0012A << 8)
              | (DUNE_BOARD_BRD0002A << 1)):

            if ((slotNum >= 1)
                || (devRelNum >= NUM_FES_ON_BRD002A)
                || (portNum >= FE200_NUM_OF_SERDESES))
            {
                return GT_BAD_PARAM;
            }
            unpackSp(
                Negev_FE_BRD0012A_BRD0002A_serdes_params
                    [slotNum][devRelNum][portNum],
                serdesStatusPtr);
            return GT_OK;

        /* FAP port on Negev with BRD0012A and BRD0002A */
        case ((DUNE_CHASSY_NEGEV << 16) | (DUNE_BOARD_BRD0012A << 8)
              | (DUNE_BOARD_BRD0002A << 1) | 1):

            if ((slotNum >= 2)
                || (devRelNum >= NUM_FAPS_ON_BRD0012A)
                || (portNum >= FAP10M_NUMBER_OF_LINKS))
            {
                return GT_BAD_PARAM;
            }
            unpackSp(
                Negev_FAP_BRD0012A_BRD0002A_serdes_params
                    [slotNum][devRelNum][portNum],
                serdesStatusPtr);
            return GT_OK;

        /* FE port on Goby with BRD0012A and BRD0002A */
        case ((DUNE_CHASSY_GOBY << 16) | (DUNE_BOARD_BRD0012A << 8)
              | (DUNE_BOARD_BRD0002A << 1)):

            if ((slotNum >= 8)
                || (devRelNum >= NUM_FES_ON_BRD002A)
                || (portNum >= FE200_NUM_OF_SERDESES))
            {
                return GT_BAD_PARAM;
            }
            unpackSp(
                Gobi_FE_BRD0012A_BRD0002A_serdes_params
                    [slotNum][devRelNum][portNum],
                serdesStatusPtr);
            return GT_OK;

        /* FAP port on Goby with BRD0012A and BRD0002A */
        case ((DUNE_CHASSY_GOBY << 16) | (DUNE_BOARD_BRD0012A << 8)
              | (DUNE_BOARD_BRD0002A << 1) | 1):

            if ((slotNum >= 16)
                || (devRelNum >= NUM_FAPS_ON_BRD0012A)
                || (portNum >= FAP10M_NUMBER_OF_LINKS))
            {
                return GT_BAD_PARAM;
            }
            unpackSp(
                Gobi_FAP_BRD0012A_BRD0002A_serdes_params
                    [slotNum][devRelNum][portNum],
                serdesStatusPtr);
            return GT_OK;

        default: break;
    }

    /* unsupported case: default configuration */
    return GT_OK;
}



