/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChHwRegAddrVer1_ppDb.c
*
* DESCRIPTION:
*       This file includes definitions of DB of units base addresses.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwRegAddrVer1_ppDb.h>

/*--------------------------------------------------------------*
 * Unit Data Base for various  PP                               *
 *--------------------------------------------------------------*/
static PRV_CPSS_DXCH_UNITID_UNITBASEADDR_STC prvDxChBobcat2UnitsIdUnitBaseAddrArr[] =
{
     { PRV_CPSS_DXCH_UNIT_TTI_E,                PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TTI_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_PCL_E,                PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_PCL_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_L2I_E,                PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_L2I_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_FDB_E,                PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_FDB_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_EQ_E,                 PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_EQ_CNS                         }
    ,{ PRV_CPSS_DXCH_UNIT_LPM_E,                PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LPM_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_EGF_EFT_E,            PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_EGF_EFT_CNS                    }
    ,{ PRV_CPSS_DXCH_UNIT_EGF_QAG_E,            PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_EGF_QAG_CNS                    }
    ,{ PRV_CPSS_DXCH_UNIT_EGF_SHT_E,            PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_EGF_SHT_CNS                    }
    ,{ PRV_CPSS_DXCH_UNIT_HA_E,                 PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_HA_CNS                         }
    ,{ PRV_CPSS_DXCH_UNIT_ETS_E,                PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_ETS_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_MLL_E,                PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_MLL_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_IPLR_E,               PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_IPLR_CNS                       }
    ,{ PRV_CPSS_DXCH_UNIT_EPLR_E,               PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_EPLR_CNS                       }
    ,{ PRV_CPSS_DXCH_UNIT_IPVX_E,               PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_IPVX_CNS                       }
    ,{ PRV_CPSS_DXCH_UNIT_IOAM_E,               PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_IOAM_CNS                       }
    ,{ PRV_CPSS_DXCH_UNIT_EOAM_E,               PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_EOAM_CNS                       }
    ,{ PRV_CPSS_DXCH_UNIT_TCAM_E,               PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TCAM_CNS                       }
    ,{ PRV_CPSS_DXCH_UNIT_RXDMA_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_RXDMA_CNS                      }
    ,{ PRV_CPSS_DXCH_UNIT_EPCL_E,               PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_EPCL_CNS                       }
    ,{ PRV_CPSS_DXCH_UNIT_TM_FCU_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TM_FCU_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_TM_DROP_E,            PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TM_DROP_CNS                    }
    ,{ PRV_CPSS_DXCH_UNIT_TM_QMAP_E,            PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TM_QMAP_CNS                    }
    ,{ PRV_CPSS_DXCH_UNIT_TM_E,                 PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TM_CNS                         }
    ,{ PRV_CPSS_DXCH_UNIT_GOP_E,                PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_GOP_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_MIB_E,                PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_MIB_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_SERDES_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_SERDES_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_ERMRK_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_ERMRK_CNS                      }
    ,{ PRV_CPSS_DXCH_UNIT_BM_E,                 PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_BM_CNS                         }
    ,{ PRV_CPSS_DXCH_UNIT_TM_INGRESS_GLUE_E,    PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TM_INGRESS_GLUE_CNS            }
    ,{ PRV_CPSS_DXCH_UNIT_TM_EGRESS_GLUE_E,     PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TM_EGRESS_GLUE_CNS             }
    ,{ PRV_CPSS_DXCH_UNIT_ETH_TXFIFO_E,         PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_ETH_TXFIFO_CNS                 }
    ,{ PRV_CPSS_DXCH_UNIT_ILKN_TXFIFO_E,        PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_ILKN_TXFIFO_CNS                }
    ,{ PRV_CPSS_DXCH_UNIT_ILKN_E,               PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_ILKN_CNS                       }
    ,{ PRV_CPSS_DXCH_UNIT_LMS0_0_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LMS0_0_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_LMS0_1_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LMS0_1_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_LMS0_2_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LMS0_2_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_LMS0_3_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LMS0_3_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_TX_FIFO_E,            PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TX_FIFO_CNS                    }
    ,{ PRV_CPSS_DXCH_UNIT_BMA_E,                PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_BMA_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_CNC_0_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_CNC_0_CNS                      }
    ,{ PRV_CPSS_DXCH_UNIT_CNC_1_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_CNC_1_CNS                      }
    ,{ PRV_CPSS_DXCH_UNIT_TXQ_QUEUE_E,          PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TXQ_QUEUE_CNS                  }
    ,{ PRV_CPSS_DXCH_UNIT_TXQ_LL_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TXQ_LL_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_TXQ_PFC_E,            PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TXQ_PFC_CNS                    }
    ,{ PRV_CPSS_DXCH_UNIT_TXQ_QCN_E,            PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TXQ_QCN_CNS                    }
    ,{ PRV_CPSS_DXCH_UNIT_TXQ_DQ_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TXQ_DQ_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_DFX_SERVER_E,         PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_DFX_SERVER_CNS                 }
    ,{ PRV_CPSS_DXCH_UNIT_MPPM_E,               PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_MPPM_CNS                       }
    ,{ PRV_CPSS_DXCH_UNIT_LMS1_0_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LMS1_0_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_LMS1_1_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LMS1_1_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_LMS1_2_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LMS1_2_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_LMS1_3_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LMS1_3_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_LMS2_0_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LMS2_0_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_LMS2_1_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LMS2_1_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_LMS2_2_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LMS2_2_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_LMS2_3_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LMS2_3_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_MPPM_1_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_MPPM_1_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_CTU_0_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_CTU_0_CNS                      }
    ,{ PRV_CPSS_DXCH_UNIT_CTU_1_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_CTU_1_CNS                      }
    ,{ PRV_CPSS_DXCH_UNIT_TXQ_SHT_E ,           PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TXQ_SHT_CNS                    }
    ,{ PRV_CPSS_DXCH_UNIT_TXQ_EGR0_E,           PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TXQ_EGR0_CNS                   }
    ,{ PRV_CPSS_DXCH_UNIT_TXQ_EGR1_E,           PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TXQ_EGR1_CNS                   }
    ,{ PRV_CPSS_DXCH_UNIT_TXQ_DIST_E,           PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TXQ_DIST_CNS                   }
    ,{ PRV_CPSS_DXCH_UNIT_IPLR_1_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_IPLR_1_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_TXDMA_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TXDMA_CNS                      }
    ,{ PRV_CPSS_DXCH_UNIT_MG_E,                 PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_MG_CNS                         }
    ,{ PRV_CPSS_DXCH_UNIT_TCC_IPCL_E,           PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TCC_PCL_CNS                    }
    ,{ PRV_CPSS_DXCH_UNIT_TCC_IPVX_E,           PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TCC_IPVX_CNS                   }
     /* B0 subunits */
    ,{ PRV_CPSS_DXCH_UNIT_SMI_0_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_SMI_0_E                        }
    ,{ PRV_CPSS_DXCH_UNIT_SMI_1_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_SMI_1_E                        }
    ,{ PRV_CPSS_DXCH_UNIT_SMI_2_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_SMI_2_E                        }
    ,{ PRV_CPSS_DXCH_UNIT_SMI_3_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_SMI_3_E                        }
    ,{ PRV_CPSS_DXCH_UNIT_LED_0_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LED_0_E                        }
    ,{ PRV_CPSS_DXCH_UNIT_LED_1_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LED_1_E                        }
    ,{ PRV_CPSS_DXCH_UNIT_LED_2_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LED_2_E                        }
    ,{ PRV_CPSS_DXCH_UNIT_LED_3_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LED_3_E                        }
    ,{ PRV_CPSS_DXCH_UNIT_LED_4_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LED_4_E                        }
    ,{ PRV_CPSS_DXCH_UNIT_LAST_E,               NON_VALID_ADDR_CNS                                           }
};


static PRV_CPSS_DXCH_UNITID_UNITBASEADDR_STC prvDxChBobKUnitsIdUnitBaseAddrArr[] =
{
     { PRV_CPSS_DXCH_UNIT_TTI_E,                PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TTI_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_PCL_E,                PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_PCL_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_L2I_E,                PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_L2I_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_FDB_E,                PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_FDB_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_EQ_E,                 PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_EQ_CNS                         }
    ,{ PRV_CPSS_DXCH_UNIT_LPM_E,                PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LPM_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_EGF_EFT_E,            PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_EGF_EFT_CNS                    }
    ,{ PRV_CPSS_DXCH_UNIT_EGF_QAG_E,            PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_EGF_QAG_CNS                    }
    ,{ PRV_CPSS_DXCH_UNIT_EGF_SHT_E,            PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_EGF_SHT_CNS                    }
    ,{ PRV_CPSS_DXCH_UNIT_HA_E,                 PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_HA_CNS                         }
    ,{ PRV_CPSS_DXCH_UNIT_ETS_E,                PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_ETS_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_MLL_E,                PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_MLL_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_IPLR_E,               PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_IPLR_CNS                       }
    ,{ PRV_CPSS_DXCH_UNIT_EPLR_E,               PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_EPLR_CNS                       }
    ,{ PRV_CPSS_DXCH_UNIT_IPVX_E,               PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_IPVX_CNS                       }
    ,{ PRV_CPSS_DXCH_UNIT_IOAM_E,               PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_IOAM_CNS                       }
    ,{ PRV_CPSS_DXCH_UNIT_EOAM_E,               PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_EOAM_CNS                       }
    ,{ PRV_CPSS_DXCH_UNIT_TCAM_E,               PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TCAM_CNS                       }
    ,{ PRV_CPSS_DXCH_UNIT_RXDMA_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_RXDMA_CNS                      }
    ,{ PRV_CPSS_DXCH_UNIT_EPCL_E,               PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_EPCL_CNS                       }
    ,{ PRV_CPSS_DXCH_UNIT_TM_FCU_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TM_FCU_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_TM_DROP_E,            PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TM_DROP_CNS                    }
    ,{ PRV_CPSS_DXCH_UNIT_TM_QMAP_E,            PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TM_QMAP_CNS                    }
    ,{ PRV_CPSS_DXCH_UNIT_TM_E,                 PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TM_CNS                         }
    ,{ PRV_CPSS_DXCH_UNIT_GOP_E,                PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_GOP_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_MIB_E,                PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_MIB_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_SERDES_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_SERDES_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_ERMRK_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_ERMRK_CNS                      }
    ,{ PRV_CPSS_DXCH_UNIT_BM_E,                 PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_BM_CNS                         }
    ,{ PRV_CPSS_DXCH_UNIT_TM_INGRESS_GLUE_E,    PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TM_INGRESS_GLUE_CNS            }
    ,{ PRV_CPSS_DXCH_UNIT_TM_EGRESS_GLUE_E,     PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TM_EGRESS_GLUE_CNS             }
    ,{ PRV_CPSS_DXCH_UNIT_ETH_TXFIFO_E,         PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_ETH_TXFIFO_CNS                 }
    ,{ PRV_CPSS_DXCH_UNIT_ILKN_TXFIFO_E,        PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_ILKN_TXFIFO_CNS                }
    ,{ PRV_CPSS_DXCH_UNIT_ILKN_E,               PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_ILKN_CNS                       }
    ,{ PRV_CPSS_DXCH_UNIT_LMS0_0_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LMS0_0_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_LMS0_1_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LMS0_1_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_LMS0_2_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LMS0_2_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_LMS0_3_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LMS0_3_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_TX_FIFO_E,            PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TX_FIFO_CNS                    }
    ,{ PRV_CPSS_DXCH_UNIT_BMA_E,                PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_BMA_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_CNC_0_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_CNC_0_CNS                      }
    ,{ PRV_CPSS_DXCH_UNIT_CNC_1_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_CNC_1_CNS                      }
    ,{ PRV_CPSS_DXCH_UNIT_TXQ_QUEUE_E,          PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TXQ_QUEUE_CNS                  }
    ,{ PRV_CPSS_DXCH_UNIT_TXQ_LL_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TXQ_LL_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_TXQ_PFC_E,            PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TXQ_PFC_CNS                    }
    ,{ PRV_CPSS_DXCH_UNIT_TXQ_QCN_E,            PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TXQ_QCN_CNS                    }
    ,{ PRV_CPSS_DXCH_UNIT_TXQ_DQ_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TXQ_DQ_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_DFX_SERVER_E,         PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_DFX_SERVER_CNS                 }
    ,{ PRV_CPSS_DXCH_UNIT_MPPM_E,               PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_MPPM_CNS                       }
    ,{ PRV_CPSS_DXCH_UNIT_LMS1_0_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LMS1_0_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_LMS1_1_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LMS1_1_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_LMS1_2_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LMS1_2_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_LMS1_3_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LMS1_3_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_LMS2_0_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LMS2_0_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_LMS2_1_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LMS2_1_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_LMS2_2_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LMS2_2_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_LMS2_3_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LMS2_3_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_MPPM_1_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_MPPM_1_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_CTU_0_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_CTU_0_CNS                      }
    ,{ PRV_CPSS_DXCH_UNIT_CTU_1_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_CTU_1_CNS                      }
    ,{ PRV_CPSS_DXCH_UNIT_TXQ_SHT_E ,           PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TXQ_SHT_CNS                    }
    ,{ PRV_CPSS_DXCH_UNIT_TXQ_EGR0_E,           PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TXQ_EGR0_CNS                   }
    ,{ PRV_CPSS_DXCH_UNIT_TXQ_EGR1_E,           PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TXQ_EGR1_CNS                   }
    ,{ PRV_CPSS_DXCH_UNIT_TXQ_DIST_E,           PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TXQ_DIST_CNS                   }
    ,{ PRV_CPSS_DXCH_UNIT_IPLR_1_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_IPLR_1_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_TXDMA_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TXDMA_CNS                      }
    ,{ PRV_CPSS_DXCH_UNIT_MG_E,                 PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_MG_CNS                         }
    ,{ PRV_CPSS_DXCH_UNIT_TCC_IPCL_E,           PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TCC_PCL_CNS                    }
    ,{ PRV_CPSS_DXCH_UNIT_TCC_IPVX_E,           PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TCC_IPVX_CNS                   }
    ,{ PRV_CPSS_DXCH_UNIT_SMI_0_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_SMI_0_E                        }
    ,{ PRV_CPSS_DXCH_UNIT_SMI_1_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_SMI_1_E                        }
    ,{ PRV_CPSS_DXCH_UNIT_SMI_2_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_SMI_2_E                        }
    ,{ PRV_CPSS_DXCH_UNIT_SMI_3_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_SMI_3_E                        }
    ,{ PRV_CPSS_DXCH_UNIT_LED_0_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LED_0_E                        }
    ,{ PRV_CPSS_DXCH_UNIT_LED_1_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LED_1_E                        }
    ,{ PRV_CPSS_DXCH_UNIT_LED_2_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LED_2_E                        }
    ,{ PRV_CPSS_DXCH_UNIT_LED_3_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LED_3_E                        }
    ,{ PRV_CPSS_DXCH_UNIT_LED_4_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LED_4_E                        }
     /*  bobk units */
    ,{ PRV_CPSS_DXCH_UNIT_RXDMA1_E,             PRV_CPSS_DXCH_BOBK_BASE_ADDRESS_RXDMA1_E                          }
    ,{ PRV_CPSS_DXCH_UNIT_TXDMA1_E,             PRV_CPSS_DXCH_BOBK_BASE_ADDRESS_TXDMA1_E                          }
    ,{ PRV_CPSS_DXCH_UNIT_TX_FIFO1_E,           PRV_CPSS_DXCH_BOBK_BASE_ADDRESS_TX_FIFO1_E                        }
    ,{ PRV_CPSS_DXCH_UNIT_ETH_TXFIFO1_E,        PRV_CPSS_DXCH_BOBK_BASE_ADDRESS_ETH_TX_FIFO1_CNS                  }

    ,{ PRV_CPSS_DXCH_UNIT_RXDMA_GLUE_E,         PRV_CPSS_DXCH_BOBK_BASE_ADDRESS_RXDMA_GLUE_CNS                    }
    ,{ PRV_CPSS_DXCH_UNIT_TXDMA_GLUE_E,         PRV_CPSS_DXCH_BOBK_BASE_ADDRESS_TXDMA_GLUE_CNS                    }
    ,{ PRV_CPSS_DXCH_UNIT_TAI_E,                PRV_CPSS_DXCH_BOBK_BASE_ADDRESS_TAI_CNS                           }
    ,{ PRV_CPSS_DXCH_UNIT_LAST_E,               NON_VALID_ADDR_CNS                                                }
};

static PRV_CPSS_DXCH_UNITID_UNITBASEADDR_STC prvDxChAldrinUnitsIdUnitBaseAddrArr[] =
{
     { PRV_CPSS_DXCH_UNIT_TTI_E,                PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TTI_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_PCL_E,                PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_PCL_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_L2I_E,                PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_L2I_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_FDB_E,                PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_FDB_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_EQ_E,                 PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_EQ_CNS                         }
    ,{ PRV_CPSS_DXCH_UNIT_LPM_E,                PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LPM_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_EGF_EFT_E,            PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_EGF_EFT_CNS                    }
    ,{ PRV_CPSS_DXCH_UNIT_EGF_QAG_E,            PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_EGF_QAG_CNS                    }
    ,{ PRV_CPSS_DXCH_UNIT_EGF_SHT_E,            PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_EGF_SHT_CNS                    }
    ,{ PRV_CPSS_DXCH_UNIT_HA_E,                 PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_HA_CNS                         }
    ,{ PRV_CPSS_DXCH_UNIT_ETS_E,                PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_ETS_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_MLL_E,                PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_MLL_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_IPLR_E,               PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_IPLR_CNS                       }
    ,{ PRV_CPSS_DXCH_UNIT_EPLR_E,               PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_EPLR_CNS                       }
    ,{ PRV_CPSS_DXCH_UNIT_IPVX_E,               PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_IPVX_CNS                       }
    ,{ PRV_CPSS_DXCH_UNIT_IOAM_E,               PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_IOAM_CNS                       }
    ,{ PRV_CPSS_DXCH_UNIT_EOAM_E,               PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_EOAM_CNS                       }
    ,{ PRV_CPSS_DXCH_UNIT_TCAM_E,               PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TCAM_CNS                       }
    ,{ PRV_CPSS_DXCH_UNIT_RXDMA_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_RXDMA_CNS                      }
    ,{ PRV_CPSS_DXCH_UNIT_EPCL_E,               PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_EPCL_CNS                       }
    ,{ PRV_CPSS_DXCH_UNIT_GOP_E,                PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_GOP_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_MIB_E,                PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_MIB_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_SERDES_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_SERDES_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_ERMRK_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_ERMRK_CNS                      }
    ,{ PRV_CPSS_DXCH_UNIT_BM_E,                 PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_BM_CNS                         }
    ,{ PRV_CPSS_DXCH_UNIT_ETH_TXFIFO_E,         PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_ETH_TXFIFO_CNS                 }
    ,{ PRV_CPSS_DXCH_UNIT_LMS0_0_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LMS0_0_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_LMS0_1_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LMS0_1_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_TX_FIFO_E,            PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TX_FIFO_CNS                    }
    ,{ PRV_CPSS_DXCH_UNIT_BMA_E,                PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_BMA_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_CNC_0_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_CNC_0_CNS                      }
    ,{ PRV_CPSS_DXCH_UNIT_CNC_1_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_CNC_1_CNS                      }
    ,{ PRV_CPSS_DXCH_UNIT_TXQ_QUEUE_E,          PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TXQ_QUEUE_CNS                  }
    ,{ PRV_CPSS_DXCH_UNIT_TXQ_LL_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TXQ_LL_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_TXQ_PFC_E,            PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TXQ_PFC_CNS                    }
    ,{ PRV_CPSS_DXCH_UNIT_TXQ_QCN_E,            PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TXQ_QCN_CNS                    }
    ,{ PRV_CPSS_DXCH_UNIT_TXQ_DQ_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TXQ_DQ_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_DFX_SERVER_E,         PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_DFX_SERVER_CNS                 }
    ,{ PRV_CPSS_DXCH_UNIT_MPPM_E,               PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_MPPM_CNS                       }
    ,{ PRV_CPSS_DXCH_UNIT_LMS1_0_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LMS1_0_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_LMS1_1_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LMS1_1_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_LMS1_2_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LMS1_2_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_LMS1_3_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LMS1_3_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_LMS2_0_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LMS2_0_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_LMS2_1_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LMS2_1_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_LMS2_2_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LMS2_2_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_LMS2_3_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LMS2_3_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_MPPM_1_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_MPPM_1_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_CTU_0_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_CTU_0_CNS                      }
    ,{ PRV_CPSS_DXCH_UNIT_CTU_1_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_CTU_1_CNS                      }
    ,{ PRV_CPSS_DXCH_UNIT_TXQ_SHT_E ,           PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TXQ_SHT_CNS                    }
    ,{ PRV_CPSS_DXCH_UNIT_TXQ_EGR0_E,           PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TXQ_EGR0_CNS                   }
    ,{ PRV_CPSS_DXCH_UNIT_TXQ_EGR1_E,           PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TXQ_EGR1_CNS                   }
    ,{ PRV_CPSS_DXCH_UNIT_TXQ_DIST_E,           PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TXQ_DIST_CNS                   }
    ,{ PRV_CPSS_DXCH_UNIT_IPLR_1_E,             PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_IPLR_1_CNS                     }
    ,{ PRV_CPSS_DXCH_UNIT_TXDMA_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TXDMA_CNS                      }
    ,{ PRV_CPSS_DXCH_UNIT_MG_E,                 PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_MG_CNS                         }
    ,{ PRV_CPSS_DXCH_UNIT_TCC_IPCL_E,           PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TCC_PCL_CNS                    }
    ,{ PRV_CPSS_DXCH_UNIT_TCC_IPVX_E,           PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_TCC_IPVX_CNS                   }
    ,{ PRV_CPSS_DXCH_UNIT_SMI_0_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_SMI_0_E                        }
    ,{ PRV_CPSS_DXCH_UNIT_SMI_1_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_SMI_1_E                        }
    ,{ PRV_CPSS_DXCH_UNIT_SMI_2_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_SMI_2_E                        }
    ,{ PRV_CPSS_DXCH_UNIT_SMI_3_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_SMI_3_E                        }
    ,{ PRV_CPSS_DXCH_UNIT_LED_0_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LED_0_E                        }
    ,{ PRV_CPSS_DXCH_UNIT_LED_1_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LED_1_E                        }
    ,{ PRV_CPSS_DXCH_UNIT_LED_2_E,              PRV_CPSS_DXCH_BOBCAT2_BASE_ADDRESS_LED_2_E                        }
     /*  bobk units */
    ,{ PRV_CPSS_DXCH_UNIT_RXDMA1_E,             PRV_CPSS_DXCH_BOBK_BASE_ADDRESS_RXDMA1_E                          }
    ,{ PRV_CPSS_DXCH_UNIT_TXDMA1_E,             PRV_CPSS_DXCH_BOBK_BASE_ADDRESS_TXDMA1_E                          }
    ,{ PRV_CPSS_DXCH_UNIT_TX_FIFO1_E,           PRV_CPSS_DXCH_BOBK_BASE_ADDRESS_TX_FIFO1_E                        }
    ,{ PRV_CPSS_DXCH_UNIT_RXDMA_GLUE_E,         PRV_CPSS_DXCH_BOBK_BASE_ADDRESS_RXDMA_GLUE_CNS                    }
    ,{ PRV_CPSS_DXCH_UNIT_TXDMA_GLUE_E,         PRV_CPSS_DXCH_BOBK_BASE_ADDRESS_TXDMA_GLUE_CNS                    }
    ,{ PRV_CPSS_DXCH_UNIT_TAI_E,                PRV_CPSS_DXCH_BOBK_BASE_ADDRESS_TAI_CNS                           }
     /* aldrin units */
    ,{ PRV_CPSS_DXCH_UNIT_RXDMA2_E,             PRV_CPSS_DXCH_ALDRIN_BASE_ADDRESS_RXDMA2_E                        }
    ,{ PRV_CPSS_DXCH_UNIT_TXDMA2_E,             PRV_CPSS_DXCH_ALDRIN_BASE_ADDRESS_TXDMA2_E                        }
    ,{ PRV_CPSS_DXCH_UNIT_TX_FIFO2_E,           PRV_CPSS_DXCH_ALDRIN_BASE_ADDRESS_TX_FIFO2_E                      }

    ,{ PRV_CPSS_DXCH_UNIT_LAST_E,               NON_VALID_ADDR_CNS                                                }
};

static PRV_CPSS_DXCH_UNITID_UNITBASEADDR_STC prvDxChBobcat3UnitsIdUnitBaseAddrArr[] =
{
     { PRV_CPSS_DXCH_UNIT_TTI_E,                PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_TTI_CNS                              }
    ,{ PRV_CPSS_DXCH_UNIT_PCL_E,                PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_IPCL_CNS                             }
    ,{ PRV_CPSS_DXCH_UNIT_L2I_E,                PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_L2I_CNS                              }
    ,{ PRV_CPSS_DXCH_UNIT_FDB_E,                PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_FDB_CNS                              }
    ,{ PRV_CPSS_DXCH_UNIT_EQ_E,                 PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_EQ_CNS                               }
    ,{ PRV_CPSS_DXCH_UNIT_LPM_E,                PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_LPM_CNS                              }
    ,{ PRV_CPSS_DXCH_UNIT_EGF_EFT_E,            PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_EGF_EFT_CNS                          }
    ,{ PRV_CPSS_DXCH_UNIT_EGF_QAG_E,            PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_EGF_QAG_CNS                          }
    ,{ PRV_CPSS_DXCH_UNIT_EGF_SHT_E,            PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_EGF_SHT_CNS                          }
    ,{ PRV_CPSS_DXCH_UNIT_HA_E,                 PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_HA_CNS                               }
    ,{ PRV_CPSS_DXCH_UNIT_MLL_E,                PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_MLL_CNS                              }
    ,{ PRV_CPSS_DXCH_UNIT_IPLR_E,               PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_IPLR_CNS                             }
    ,{ PRV_CPSS_DXCH_UNIT_EPLR_E,               PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_EPLR_CNS                             }
    ,{ PRV_CPSS_DXCH_UNIT_IPVX_E,               PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_IPVX_CNS                             }
    ,{ PRV_CPSS_DXCH_UNIT_IOAM_E,               PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_IOAM_CNS                             }
    ,{ PRV_CPSS_DXCH_UNIT_EOAM_E,               PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_EOAM_CNS                             }
    ,{ PRV_CPSS_DXCH_UNIT_TCAM_E,               PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_TCAM_CNS                             }
    ,{ PRV_CPSS_DXCH_UNIT_RXDMA_E,              PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_RX_DMA_CNS                           }
    ,{ PRV_CPSS_DXCH_UNIT_EPCL_E,               PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_EPCL_CNS                             }
    ,{ PRV_CPSS_DXCH_UNIT_GOP_E,                PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_GOP_CNS                              }
    ,{ PRV_CPSS_DXCH_UNIT_MIB_E,                PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_MIB_CNS                              }
    ,{ PRV_CPSS_DXCH_UNIT_SERDES_E,             PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_SERDES_CNS                           }
    ,{ PRV_CPSS_DXCH_UNIT_ERMRK_E,              PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_ERMRK_CNS                            }
    ,{ PRV_CPSS_DXCH_UNIT_BM_E,                 PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_BM_CNS                               }
    ,{ PRV_CPSS_DXCH_UNIT_TX_FIFO_E,            PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_TX_FIFO_CNS                          }
    ,{ PRV_CPSS_DXCH_UNIT_BMA_E,                PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_BMA_CNS                              }
    ,{ PRV_CPSS_DXCH_UNIT_CNC_0_E,              PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_CNC_0_CNS                            }
    ,{ PRV_CPSS_DXCH_UNIT_CNC_1_E,              PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_CNC_1_CNS                            }
    ,{ PRV_CPSS_DXCH_UNIT_TXQ_QUEUE_E,          PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_TXQ_QUEUE_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_TXQ_LL_E,             PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_TXQ_LL_CNS                           }
    ,{ PRV_CPSS_DXCH_UNIT_TXQ_PFC_E,            PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_CPFC_CNS                             }
    ,{ PRV_CPSS_DXCH_UNIT_TXQ_QCN_E,            PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_TXQ_QCN_CNS                          }
    ,{ PRV_CPSS_DXCH_UNIT_TXQ_DQ_E,             PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_TXQ_DQ_CNS                           }
    ,{ PRV_CPSS_DXCH_UNIT_MPPM_E,               PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_MPPM_CNS                             }
    ,{ PRV_CPSS_DXCH_UNIT_IPLR_1_E,             PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_IPLR1_CNS                            }
    ,{ PRV_CPSS_DXCH_UNIT_TXDMA_E,              PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_TX_DMA_CNS                           }
    ,{ PRV_CPSS_DXCH_UNIT_MG_E,                 PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_MG_CNS                               }

    ,{ PRV_CPSS_DXCH_UNIT_SMI_0_E,              PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_GOP_SMI_0_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_SMI_1_E,              PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_GOP_SMI_1_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_SMI_2_E,              BOBCAT3_PIPE1_MAC(PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_GOP_SMI_0_CNS)     }
    ,{ PRV_CPSS_DXCH_UNIT_SMI_3_E,              BOBCAT3_PIPE1_MAC(PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_GOP_SMI_1_CNS)     }
    ,{ PRV_CPSS_DXCH_UNIT_LED_0_E,              PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_GOP_LED_0_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_LED_1_E,              PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_GOP_LED_1_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_LED_2_E,              BOBCAT3_PIPE1_MAC(PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_GOP_LED_0_CNS)     }
    ,{ PRV_CPSS_DXCH_UNIT_LED_3_E,              BOBCAT3_PIPE1_MAC(PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_GOP_LED_1_CNS)     }

    ,{ PRV_CPSS_DXCH_UNIT_RXDMA1_E,             PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_RX_DMA_1_CNS                         }
    ,{ PRV_CPSS_DXCH_UNIT_TXDMA1_E,             PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_TX_DMA_1_CNS                         }
    ,{ PRV_CPSS_DXCH_UNIT_TX_FIFO1_E,           PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_TX_FIFO_1_CNS                        }

    ,{ PRV_CPSS_DXCH_UNIT_RXDMA2_E,             PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_RX_DMA_2_CNS                         }
    ,{ PRV_CPSS_DXCH_UNIT_RXDMA3_E,             PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_RX_DMA_3_CNS                         }
    ,{ PRV_CPSS_DXCH_UNIT_RXDMA4_E,             PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_RX_DMA_4_CNS                         }
    ,{ PRV_CPSS_DXCH_UNIT_RXDMA5_E,             PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_RX_DMA_5_CNS                         }

    ,{ PRV_CPSS_DXCH_UNIT_TXDMA2_E,             PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_TX_DMA_2_CNS                         }
    ,{ PRV_CPSS_DXCH_UNIT_TXDMA3_E,             PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_TX_DMA_3_CNS                         }
    ,{ PRV_CPSS_DXCH_UNIT_TXDMA4_E,             PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_TX_DMA_4_CNS                         }
    ,{ PRV_CPSS_DXCH_UNIT_TXDMA5_E,             PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_TX_DMA_5_CNS                         }

    ,{ PRV_CPSS_DXCH_UNIT_TX_FIFO2_E,           PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_TX_FIFO_2_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_TX_FIFO3_E,           PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_TX_FIFO_3_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_TX_FIFO4_E,           PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_TX_FIFO_4_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_TX_FIFO5_E,           PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_TX_FIFO_5_CNS                        }

    ,{ PRV_CPSS_DXCH_UNIT_TXQ_DQ1_E,            PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_TXQ_DQ_1_CNS                         }
    ,{ PRV_CPSS_DXCH_UNIT_TXQ_DQ2_E,            PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_TXQ_DQ_2_CNS                         }
    ,{ PRV_CPSS_DXCH_UNIT_TXQ_DQ3_E,            PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_TXQ_DQ_3_CNS                         }
    ,{ PRV_CPSS_DXCH_UNIT_TXQ_DQ4_E,            PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_TXQ_DQ_4_CNS                         }
    ,{ PRV_CPSS_DXCH_UNIT_TXQ_DQ5_E,            PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_TXQ_DQ_5_CNS                         }

    ,{ PRV_CPSS_DXCH_UNIT_MIB1_E,               PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_MIB1_CNS                             }
    ,{ PRV_CPSS_DXCH_UNIT_GOP1_E,               PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_GOP1_CNS                             }

    ,{ PRV_CPSS_DXCH_UNIT_TAI_E,                PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_TAI_CNS                              }
    ,{ PRV_CPSS_DXCH_UNIT_TAI1_E,               PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_TAI1_CNS                             }

    ,{ PRV_CPSS_DXCH_UNIT_EGF_SHT_1_E,          PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_EGF_SHT_1_CNS                        }

    ,{ PRV_CPSS_DXCH_UNIT_TTI_1_E               ,PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_TTI_1_CNS                           }
    ,{ PRV_CPSS_DXCH_UNIT_IPCL_1_E              ,PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_IPCL_1_CNS                          }
    ,{ PRV_CPSS_DXCH_UNIT_L2I_1_E               ,PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_L2I_1_CNS                           }
    ,{ PRV_CPSS_DXCH_UNIT_IPVX_1_E              ,PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_IPVX_1_CNS                          }
    ,{ PRV_CPSS_DXCH_UNIT_IPLR_0_1_E            ,PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_IPLR_0_1_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_IPLR_1_1_E            ,PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_IPLR_1_1_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_IOAM_1_E              ,PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_IOAM_1_CNS                          }
    ,{ PRV_CPSS_DXCH_UNIT_MLL_1_E               ,PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_MLL_1_CNS                           }
    ,{ PRV_CPSS_DXCH_UNIT_EQ_1_E                ,PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_EQ_1_CNS                            }
    ,{ PRV_CPSS_DXCH_UNIT_EGF_EFT_1_E           ,PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_EGF_EFT_1_CNS                       }
    ,{ PRV_CPSS_DXCH_UNIT_CNC_0_1_E             ,PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_CNC_0_1_CNS                         }
    ,{ PRV_CPSS_DXCH_UNIT_CNC_1_1_E             ,PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_CNC_1_1_CNS                         }
    ,{ PRV_CPSS_DXCH_UNIT_SERDES_1_E            ,PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_SERDES_1_CNS                        }
    ,{ PRV_CPSS_DXCH_UNIT_HA_1_E                ,PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_HA_1_CNS                            }
    ,{ PRV_CPSS_DXCH_UNIT_ERMRK_1_E             ,PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_ERMRK_1_CNS                         }
    ,{ PRV_CPSS_DXCH_UNIT_EPCL_1_E              ,PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_EPCL_1_CNS                          }
    ,{ PRV_CPSS_DXCH_UNIT_EPLR_1_E              ,PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_EPLR_1_CNS                          }
    ,{ PRV_CPSS_DXCH_UNIT_EOAM_1_E              ,PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_EOAM_1_CNS                          }
    ,{ PRV_CPSS_DXCH_UNIT_RX_DMA_GLUE_1_E       ,PRV_CPSS_DXCH_BOBCAT3_BASE_ADDRESS_RX_DMA_GLUE_1_CNS                   }

    ,{ PRV_CPSS_DXCH_UNIT_LAST_E,               NON_VALID_ADDR_CNS                                                 }
};

/* GT_U32 prvDxChLion2UnitsBaseAddr  [PRV_CPSS_DXCH_UNIT_LAST_E + 1];  *//* Lion2 unit base address array */
GT_U32 prvDxChBobcat2UnitsBaseAddr[PRV_CPSS_DXCH_UNIT_LAST_E + 1];  /* Bobcat2 unit base address array */
GT_U32 prvDxChBobkUnitsBaseAddr   [PRV_CPSS_DXCH_UNIT_LAST_E + 1];  /* Bobk unit base address array */
GT_U32 prvDxChBobcat3UnitsBaseAddr[PRV_CPSS_DXCH_UNIT_LAST_E + 1];  /* Bobcat3 unit base address array */
GT_U32 prvDxChAldrinUnitsBaseAddr [PRV_CPSS_DXCH_UNIT_LAST_E + 1];  /* Aldrin unit base address array */

static PRV_CPSS_DXCH_UNITID_UNITBASEADDR_x_TABLE2FILL_STC prv_UnitId_UnitBaseDrrList_x_UnitBasedArrTbl []  =
{
      /* { CPSS_PP_FAMILY_DXCH_LION2_E,    CPSS_PP_SUB_FAMILY_NONE_E,          &prvDxChLion2UnitsIdUnitBaseAddrArr[0],       &prvDxChLion2UnitsBaseAddr[0]   } */
      { CPSS_PP_FAMILY_DXCH_BOBCAT2_E,  CPSS_PP_SUB_FAMILY_NONE_E,          &prvDxChBobcat2UnitsIdUnitBaseAddrArr[0],     &prvDxChBobcat2UnitsBaseAddr[0] }
     ,{ CPSS_PP_FAMILY_DXCH_BOBCAT2_E,  CPSS_PP_SUB_FAMILY_BOBCAT2_BOBK_E,  &prvDxChBobKUnitsIdUnitBaseAddrArr[0],        &prvDxChBobkUnitsBaseAddr[0]    }
     ,{ CPSS_PP_FAMILY_DXCH_BOBCAT3_E,  CPSS_PP_SUB_FAMILY_NONE_E,          &prvDxChBobcat3UnitsIdUnitBaseAddrArr[0],     &prvDxChBobcat3UnitsBaseAddr[0] }
     ,{ CPSS_PP_FAMILY_DXCH_ALDRIN_E,   CPSS_PP_SUB_FAMILY_NONE_E,          &prvDxChAldrinUnitsIdUnitBaseAddrArr[0],      &prvDxChAldrinUnitsBaseAddr[0]    }
     ,{ CPSS_MAX_FAMILY,                CPSS_BAD_SUB_FAMILY,                (PRV_CPSS_DXCH_UNITID_UNITBASEADDR_STC*)NULL, (GT_U32 *)NULL                  }
};


static PRV_CPSS_DXCH_UNITID_UNITBASEADDR_x_TABLE2FILL_WS_STC prv_unitBaseWs =
{
    GT_FALSE,
    &prv_UnitId_UnitBaseDrrList_x_UnitBasedArrTbl[0]
};


/*******************************************************************************
* prvCpssDxChUnitBaseTableInit
*
* DESCRIPTION:
*       init based address lists for all ASICs
*
* APPLICABLE DEVICES:
*
* NOT APPLICABLE DEVICES:
*
* INPUTS:
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChUnitBaseTableInit
(
    GT_VOID
)
{
    GT_U32 ppIdx;
    GT_U32 unitIdx;
    PRV_CPSS_DXCH_UNIT_ENT unitType;
    PRV_CPSS_DXCH_UNITID_UNITBASEADDR_STC  * unitTypeBaseAdrrPtr;
    GT_U32                                 * prvDxChLion2UnitsBaseAddr;

    if (prv_unitBaseWs.isInit == GT_FALSE)
    {
        for (ppIdx = 0 ; prv_unitBaseWs.initTblList[ppIdx].devFamily != CPSS_MAX_FAMILY; ppIdx++)
        {
            unitTypeBaseAdrrPtr       = prv_unitBaseWs.initTblList[ppIdx].unitId2UnitBaseAdrrList;
            prvDxChLion2UnitsBaseAddr = prv_unitBaseWs.initTblList[ppIdx].prvDxChLion2UnitsBaseAddr;

            /*---------------------------------------------------------------------------------*
             *init default value                                                               *
             *  all but last (0:PRV_CPSS_DXCH_UNIT_LAST_E-1) --    NON_VALID_BASE_ADDR_CNS     *
             *  last(idx = PRV_CPSS_DXCH_UNIT_LAST_E)        --    END_OF_TABLE_INDICATOR_CNS  *
             *---------------------------------------------------------------------------------*/
            for (unitIdx = 0 ; unitIdx < PRV_CPSS_DXCH_UNIT_LAST_E + 1 ; unitIdx++)
            {
                prvDxChLion2UnitsBaseAddr[unitIdx] = NON_VALID_BASE_ADDR_CNS;
            }
            prvDxChLion2UnitsBaseAddr[PRV_CPSS_DXCH_UNIT_LAST_E] = END_OF_TABLE_INDICATOR_CNS;

            /*--------------------------------------------------*
             *  init spectfic values : from init list           *
             *--------------------------------------------------*/
            for (unitIdx = 0 ; unitTypeBaseAdrrPtr[unitIdx].unitId != PRV_CPSS_DXCH_UNIT_LAST_E ; unitIdx++)
            {
                unitType = unitTypeBaseAdrrPtr[unitIdx].unitId;
                prvDxChLion2UnitsBaseAddr[unitType] = unitTypeBaseAdrrPtr[unitIdx].unitBaseAdrr;
            }
        }
        prv_unitBaseWs.isInit = GT_TRUE;
    }
    return GT_OK;
}



/*******************************************************************************
* prvCpssDxChUnitBaseTableGet
*
* DESCRIPTION:
*       get based address lists for all ASICs
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - The PP's device number to init the struct for.
*
* OUTPUTS:
*       prvDxChLion2UnitsBaseAddrPtrPtr - ptr to point to table
*
* RETURNS:
*       GT_OK on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvCpssDxChUnitBaseTableGet
(
    IN  GT_U8    devNum,
    OUT GT_U32 **prvDxChLion2UnitsBaseAddrPtrPtr
)
{
    GT_STATUS rc;
    GT_U32 ppIdx;
    CPSS_PP_FAMILY_TYPE_ENT                devFamily;
    CPSS_PP_SUB_FAMILY_TYPE_ENT            devSubFamily;

    rc = prvCpssDxChUnitBaseTableInit();
    if (rc != GT_OK)
    {
        return rc;
    }

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum,
          CPSS_CH1_E| CPSS_CH1_DIAMOND_E | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
          CPSS_LION_E | CPSS_XCAT2_E | CPSS_LION2_E);

    CPSS_NULL_PTR_CHECK_MAC(prvDxChLion2UnitsBaseAddrPtrPtr);


    devFamily    = PRV_CPSS_PP_MAC(devNum)->devFamily;
    devSubFamily = PRV_CPSS_PP_MAC(devNum)->devSubFamily;

    for (ppIdx = 0 ; prv_unitBaseWs.initTblList[ppIdx].devFamily != CPSS_MAX_FAMILY; ppIdx++)
    {
        if (prv_unitBaseWs.initTblList[ppIdx].devFamily    == devFamily &&
            prv_unitBaseWs.initTblList[ppIdx].devSubfamily == devSubFamily)
        {
            *prvDxChLion2UnitsBaseAddrPtrPtr = prv_unitBaseWs.initTblList[ppIdx].prvDxChLion2UnitsBaseAddr;
            return GT_OK;
        }
    }
    CPSS_LOG_ERROR_AND_RETURN_MAC(GT_NOT_APPLICABLE_DEVICE, LOG_ERROR_NO_MSG);
}









