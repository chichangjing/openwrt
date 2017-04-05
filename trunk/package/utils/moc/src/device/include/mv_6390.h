#include "gtDrvSwRegs.h"
#include "gtHwCntl.h"
#include "gtSem.h"
#include "msApi.h"

//#define SP_6097F
#define SP_6390X
#define SP_6390

#ifdef SP_6097F
#define SP_6097F_6390
#define SP_6097F_6390X
#define SP_6097F_6390X_6390
#endif

#ifdef SP_6390
#define SP_6097F_6390
#define SP_6390X_6390
#define SP_6097F_6390X_6390
#endif

#ifdef SP_6390X
#define SP_6097F_6390X
#define SP_6390X_6390
#define SP_6097F_6390X_6390
#endif



static GT_QD_DEV dev;
static GT_QoS_WEIGHT QosWeight;



#ifdef SP_6390X_6390
/*
 * 基于设备
 *
 * 6390: P403 G2 0x1C 0d28
 *
 * 配置QOS Weight Table
 *
 * */
SP_6390X_6390
GT_STATUS gsysSetQoSWeight
(
    IN  GT_QD_DEV         *dev,
    IN  GT_QoS_WEIGHT    *weight
);
/*
 * 基于设备
 *
 * 6390: P403 G2 0x1C 0d28
 *
 * 获取QOS Weight Table
 *
 * */
SP_6390X_6390
GT_STATUS gsysGetQoSWeight
(
    IN  GT_QD_DEV         *dev,
    OUT GT_QoS_WEIGHT    *weight
);

/*
 * 基于设备
 *
 * 6390: P305 G1 0x1C 0d28
 *
 * 设置CtrMode，用于debug
 *
 * */
SP_6390X_6390
GT_STATUS gsysSetCtrMode
(
    IN GT_QD_DEV    *dev,
    IN GT_U16       ctrMode
);
/*
 * 基于设备
 *
 * 6390: P305 G1 0x1C 0d28
 *
 * 获取CtrMode，用于debug
 *
 * */
SP_6390X_6390
GT_STATUS gsysGetCtrMode
(
    IN  GT_QD_DEV    *dev,
    OUT GT_U16       *ctrMode
);
/*
 * 基于设备
 *
 * 6390: P406 G2 0x1F 0d31
 *
 * 获取CtrMode，用于debug
 *
 * */
SP_6390X_6390
GT_STATUS gsysSetCutThroughHold
(
    IN GT_QD_DEV    *dev,
    IN GT_U8        number
);
/*
 * 基于设备
 *
 * 6390: P406 G1 0x1F 0d31
 *
 * 获取CtrMode，用于debug
 *
 * */
SP_6390X_6390
GT_STATUS gsysGetCutThroughHold
(
    IN GT_QD_DEV    *dev,
    OUT GT_U8       *number
);

/*
 * 基于设备
 *
 * 6390: P338 G2 0x0f 0d15
 *
 * 设置QPriOverrideTable
 *
 * */
SP_6390X_6390
GT_STATUS gsysSetQPriOverrideTable
(
    IN  GT_QD_DEV     *dev,
    IN  GT_PRI_OVERRIDE_FTYPE    fType,
    IN  GT_QPRI_TBL_ENTRY    *entry
);

/*
 * 基于设备
 *
 * 6390: P338 G2 0x0f 0d15
 *
 * 获取QPriOverrideTable
 *
 * */
SP_6390X_6390
GT_STATUS gsysGetQPriOverrideTable
(
    IN  GT_QD_DEV     *dev,
    IN  GT_PRI_OVERRIDE_FTYPE    fType,
    OUT GT_QPRI_TBL_ENTRY    *entry
);
/*
 * 基于设备
 *
 * 6390: P338 G2 0x0f 0d15
 *
 * 设置FPriOverrideTable
 *
 * */
SP_6390X_6390
GT_STATUS gsysSetFPriOverrideTable
(
    IN  GT_QD_DEV     *dev,
    IN  GT_PRI_OVERRIDE_FTYPE    fType,
    IN  GT_FPRI_TBL_ENTRY    *entry
);
/*
 * 基于设备
 *
 * 6390: P338 G2 0x0f 0d15
 *
 * 获取FPriOverrideTable
 *
 * */
SP_6390X_6390
GT_STATUS gsysGetFPriOverrideTable
(
    IN  GT_QD_DEV     *dev,
    IN  GT_PRI_OVERRIDE_FTYPE    fType,
    OUT GT_FPRI_TBL_ENTRY    *entry
);

/*
 * 基于端口
 *
 * 6390: P241 PORT 0x17 0d23
 *
 * 设置IpPrioMap
 *
 * */
SP_6390X_6390
GT_STATUS gprtSetIpPrioMap
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    IN  GT_U8       loc,
    IN  GT_PRIORITY_MAP_DATA    *mapdata
);

/*
 * 基于端口
 *
 * 6390: P241 PORT 0x17 0d23
 *
 * 获取IpPrioMap
 *
 * */
SP_6390X_6390
GT_STATUS gprtGetIpPrioMap
(
    IN  GT_QD_DEV   *dev,
    IN  GT_LPORT    port,
    IN  GT_U8       loc,
    OUT GT_PRIORITY_MAP_DATA    *mapdata
);


/*
 * 基于端口
 *
 * 6390: P242 PORT 0x18 0d24
 *
 * 设IEEEPrioMap
 *
 * */
SP_6390X_6390
GT_STATUS gprtSetIEEEPrioMap
(
    IN  GT_QD_DEV     *dev,
    IN  GT_LPORT    port,
    IN  GT_IEEE_PRIORITY_TABLE_SET       table,
    IN  GT_U8       loc,
    IN  GT_IEEE_PRIORITY_TABLE_STRUCT    *prioTblStruct
);

/*
 * 基于端口
 *
 * 6390: P242 PORT 0x18 0d24
 *
 * 获取IEEEPrioMap
 *
 * */
SP_6390X_6390
GT_STATUS gprtGetIEEEPrioMap
(
    IN  GT_QD_DEV   *dev,
    IN  GT_LPORT    port,
    IN  GT_IEEE_PRIORITY_TABLE_SET       table,
    IN  GT_U8       loc,
    OUT GT_IEEE_PRIORITY_TABLE_STRUCT    *prioTblStruct
);
#endif /* SP_6390X_6390 */









#ifdef SP_6097F
/*
 * 基于设备
 *
 * 6097: P287 G1 0x00 0d00
 *
 * 获取PPU状态
 *
 * */
SP_6097F
GT_STATUS gsysGetPPUState
(
    IN  GT_QD_DEV       *dev,
    OUT GT_PPU_STATE    *mode
);
/*
 * 基于设备
 *
 * 6097: P287 G1 0x00 0d00
 *
 * 获取PPU状态
 *
 * */
SP_6097F
GT_STATUS gsysGetPPUEn
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL      *en
);

/*
 * 基于设备
 *
 * 6097: P287 G1 0x00 0d00
 *
 * 设置PPU状态
 *
 * */
SP_6097F
GT_STATUS gsysSetPPUEn
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL         en
);

/*
 * 基于设备
 *
 * 6097: P287 G1 0x00 0d00
 *
 * 获取SWITCH_MODE
 *
 * */
SP_6097F
GT_STATUS gsysGetSW_Mode
(
    IN  GT_QD_DEV  *dev,
    OUT GT_SW_MODE *mode
);
/*
 * 基于设备
 *
 * 6097: P290 G1 0x04 0d04
 *
 * 设置最大帧长度：
 * 0：1522 for tagged frame，1518 for untagged frame
 * 1：1632
 *
 * */
SP_6097F
GT_STATUS gsysSetMaxFrameSize
(
    IN  GT_QD_DEV *dev,
    IN  GT_BOOL   mode
);

/*
 * 基于设备
 *
 * 6097: P290 G1 0x04 0d04
 *
 * 获取最大帧长度：
 * 0：1522 for tagged frame，1518 for untagged frame
 * 1：1632
 *
 * */
SP_6097F
GT_STATUS gsysGetMaxFrameSize
(
    IN  GT_QD_DEV *dev,
    OUT GT_BOOL   *mode
);

/*
 * 基于设备
 *
 * 6097: P290 G1 0x04 0d04
 *
 * 重新加载EEPROM数据
 *
 * */
SP_6097F
GT_STATUS gsysReLoad
(
    IN  GT_QD_DEV *dev
);
/*
 * 基于设备
 *
 * 6097: P321 G2 0x05 0d05
 *
 * 设置Rsvd2Cpu使能
 *
 * */
SP_6097F
GT_STATUS gsysSetRsvd2Cpu
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL        en
);
/*
 * 基于设备
 *
 * 6097: P321 G2 0x05 0d05
 *
 * 获取Rsvd2Cpu使能
 *
 * */
SP_6097F
GT_STATUS gsysGetRsvd2Cpu
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL      *en
);
/*
 * 基于设备
 *
 * 6097: P321 G2 0x05 0d05
 *
 * 设置强制FC优先级使能
 *
 * */
SP_6097F
GT_STATUS gsysSetForceFlowControlPri
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL        en
);
/*
 * 基于设备
 *
 * 6097: P321 G2 0x05 0d05
 *
 * 获取强制FC优先级使能
 *
 * */
SP_6097F
GT_STATUS gsysGetForceFlowControlPri
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL      *en
);
/*
 * 基于设备
 *
 * 6097: P321 G2 0x05 0d05
 *
 * 设置强制FC优先级
 *
 * */
SP_6097F
GT_STATUS gsysSetFCPri
(
    IN GT_QD_DEV    *dev,
    IN GT_U16        pri
);
/*
 * 基于设备
 *
 * 6097: P321 G2 0x05 0d05
 *
 * 获取强制FC优先级
 *
 * */
SP_6097F
GT_STATUS gsysGetFCPri
(
    IN  GT_QD_DEV    *dev,
    OUT GT_U16      *pri
);
/*
 * 基于设备
 *
 * 6097: P312 G1 0x1C 0d28
 *
 * 设置RMU端口
 *
 * */
SP_6097F
GT_STATUS gsysSetRMPort
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT     port
);
/*
 * 基于设备
 *
 * 6097: P312 G1 0x1C 0d28
 *
 * 获取RMU端口
 *
 * */
SP_6097F
GT_STATUS gsysGetRMPort
(
    IN  GT_QD_DEV    *dev,
    OUT GT_LPORT     *port
);
/*
 * 基于设备
 *
 * 6097: P312 G1 0x1C 0d28
 *
 * 设置RMU使能
 *
 * */
SP_6097F
GT_STATUS gsysSetRMEnable
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL         en
);
/*
 * 基于设备
 *
 * 6097: P312 G1 0x1C 0d28
 *
 * 获取RMU使能
 *
 * */
SP_6097F
GT_STATUS gsysGetRMEnable
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL     *en
);
/*
 * 基于设备
 *
 * 6097: P318 G1 0x02 0d02
 *
 * 设置Rsvd2Cpu Enables2X
 *
 * */
SP_6097F
GT_STATUS gsysSetRsvd2CpuEnables2X
(
    IN GT_QD_DEV    *dev,
    IN GT_U16        enBits
);
/*
 * 基于设备
 *
 * 6097: P318 G1 0x02 0d02
 *
 * 获取Rsvd2Cpu Enables2X
 *
 * */
SP_6097F
GT_STATUS gsysGetRsvd2CpuEnables2X
(
    IN  GT_QD_DEV    *dev,
    OUT GT_U16      *enBits
);
#endif /* SP_6097F */















#ifdef SP_6097F_6390X_6390
/*
 * 基于设备
 *
 * 6390: P277 G1 0x00 0d00
 *
 * 获取InitReady状态，当ATU，VTU，QC，TCAM，SC初始化结束置1
 * switch开始接收数据帧
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysGetInitReady
(
    IN  GT_QD_DEV  *dev,
    OUT GT_BOOL    *mode
);

/*
 * 基于设备
 *
 * 6390: P304 G1 0x1B 0d27
 * 6097: P311 G1 0x1B 0d27
 *
 * 获取当前switch有效buff
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysGetFreeQSize
(
    IN  GT_QD_DEV    *dev,
    OUT GT_U16         *count
);
/*
 * 基于设备
 *
 * 6390: P282 G1 0x04 0d04
 * 6097: P290 G1 0x04 0d04
 *
 * 软件重启
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysSwReset
(
    IN  GT_QD_DEV *dev
);

/*
 * 基于设备
 *
 * 6390: P282 G1 0x04 0d04
 * 6097: P290 G1 0x04 0d04
 *
 * 设置DiscardExcessive值
 * DiscardExcessive：丢弃16次线路冲突的数据帧
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysSetDiscardExcessive
(
    IN  GT_QD_DEV *dev,
    IN GT_BOOL en
);
/*
 * 基于设备
 *
 * 6390: P282 G1 0x04 0d04
 * 6097: P290 G1 0x04 0d04
 *
 * 获取DiscardExcessive值
 * DiscardExcessive：丢弃16次线路冲突的数据帧
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysGetDiscardExcessive
(
    IN  GT_QD_DEV *dev,
    IN GT_BOOL    *en
);

/*
 * 基于设备
 *
 * 6390: P282 G1 0x04 0d04
 * 6097: P290 G1 0x04 0d04
 *
 * 设置全局的中断使能
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysSetWatchDog
(
    IN  GT_QD_DEV *dev,
    IN  GT_BOOL   en
);

/*
 * 基于设备
 *
 * 6390: P282 G1 0x04 0d04
 * 6097: P290 G1 0x04 0d04
 *
 * 获取全局的中断使能
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysGetWatchDog
(
    IN  GT_QD_DEV *dev,
    OUT GT_BOOL   *en
);

/*
 * 基于设备
 *
 * 6390: P323 G2 0x0D 0d13
 * 6097: P327 G2 0x0D 0d13
 *
 * 设置pause帧SA
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysSetDuplexPauseMac
(
    IN GT_QD_DEV    *dev,
    IN GT_ETHERADDR *mac
);
/*
 * 基于设备
 *
 * 6390: P323 G2 0x0D 0d13
 * 6097: P327 G2 0x0D 0d13
 *
 * 获取pause帧SA
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysGetDuplexPauseMac
(
    IN  GT_QD_DEV    *dev,
    OUT GT_ETHERADDR *mac
);
/*
 * 基于设备
 *
 * 6390: P323 G2 0x0D 0d13
 * 6097: P327 G2 0x0D 0d13
 *
 * 设置pause帧SA，是所有端口使用一个MAC还是不同端口使用不同MAC
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysSetPerPortDuplexPauseMac
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL      en
);
/*
 * 基于设备
 *
 * 6390: P323 G2 0x0D 0d13
 * 6097: P327 G2 0x0D 0d13
 *
 * 获取pause帧SA，是所有端口使用一个MAC还是不同端口使用不同MAC
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysGetPerPortDuplexPauseMac
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL      *en
);

/*
 * 基于设备
 *
 * 读取MII寄存器值，无数据检验
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysReadMiiReg
(
    IN  GT_QD_DEV    *dev,
    IN  GT_U32         phyAddr,
    IN  GT_U32         regAddr,
    OUT GT_U32         *data
);

/*
 * 基于设备
 *
 * 写MII寄存器值，无数据检验
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysWriteMiiReg
(
    IN  GT_QD_DEV    *dev,
    IN  GT_U32         phyAddr,
    IN  GT_U32         regAddr,
    IN  GT_U16         data
);
/*
 * 基于设备
 *
 * 操作多个MII寄存器值，无数据检验
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysAccessMultiRegs
(
    IN  GT_QD_DEV    *dev,
    INOUT HW_DEV_REG_ACCESS *regList
);

/*
 * 基于设备
 *
 * 6390: P306 G1 0x1C 0d28
 * 6097: P312 G1 0x1C 0d28
 *
 * 设置device number，一般用于级联系统
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysSetDeviceNumber
(
    IN GT_QD_DEV    *dev,
    IN GT_U32          devNum
);
/*
 * 基于设备
 *
 * 6390: P306 G1 0x1C 0d28
 * 6097: P312 G1 0x1C 0d28
 *
 * 获取device number，一般用于级联系统
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysGetDeviceNumber
(
    IN  GT_QD_DEV    *dev,
    OUT GT_U32      *devNum
);
/*
 * 基于设备
 *
 * 6390: P295 G1 0x1A 0d26
 * 6097: P309 G1 0x1A 0d26
 *
 * 设置输入镜像目标端口
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysSetIngressMonitorDest
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT        port
);

/*
 * 基于设备
 *
 * 6390: P295 G1 0x1A 0d26
 * 6097: P309 G1 0x1A 0d26
 *
 * 获取输入镜像目标端口
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysGetIngressMonitorDest
(
    IN  GT_QD_DEV    *dev,
    OUT GT_LPORT      *port
);
/*
 * 基于设备
 *
 * 6390: P295 G1 0x1A 0d26
 * 6097: P309 G1 0x1A 0d26
 *
 * 设置输出镜像目标端口
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysSetEgressMonitorDest
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT        port
);
/*
 * 基于设备
 *
 * 6390: P295 G1 0x1A 0d26
 * 6097: P309 G1 0x1A 0d26
 *
 * 获取输出镜像目标端口
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysGetEgressMonitorDest
(
    IN  GT_QD_DEV    *dev,
    OUT GT_LPORT      *port
);
/*
 * 基于设备
 *
 * 6390: P295 G1 0x1A 0d26
 * 6097: P310 G1 0x1A 0d26
 *
 * 设置ARP镜像目标端口，一般是CPU端口
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysSetARPDest
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT        port
);

/*
 * 基于设备
 *
 * 6390: P295 G1 0x1A 0d26
 * 6097: P310 G1 0x1A 0d26
 *
 * 获取ARP镜像目标端口，一般是CPU端口
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysGetARPDest
(
    IN  GT_QD_DEV    *dev,
    OUT GT_LPORT      *port
);
/*
 * 基于设备
 *
 * 6390: P295 G1 0x1A 0d26
 * 6097: P318 G2 0x02 0d02
 *
 * 设置Rsvd2Cpu地址
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysSetRsvd2CpuEnables
(
    IN GT_QD_DEV    *dev,
    IN GT_U16        enBits
);
/*
 * 基于设备
 *
 * 6390: P295 G1 0x1A 0d26
 * 6097: P318 G2 0x02 0d02
 *
 * 获取Rsvd2Cpu地址
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysGetRsvd2CpuEnables
(
    IN  GT_QD_DEV    *dev,
    OUT GT_U16      *enBits
);
/*
 * 基于设备
 *
 * 6390: P302 G1 0x1A 0d26
 * 6097: P321 G2 0x05 0d05
 *
 * 设置MGMT优先级
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysSetMGMTPri
(
    IN GT_QD_DEV    *dev,
    IN GT_U16        pri
);
/*
 * 基于设备
 *
 * 6390: P302 G1 0x1A 0d26
 * 6097: P321 G2 0x05 0d05
 *
 * 获取MGMT优先级
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysGetMGMTPri
(
    IN  GT_QD_DEV    *dev,
    OUT GT_U16      *pri
);
/*
 * 基于设备
 *
 * 6390: P314 G2 0x05 0d05
 * 6097: P321 G2 0x05 0d05
 *
 * 设置全局FC控制
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysSetFlowControlMessage
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL        en
);
/*
 * 基于设备
 *
 * 6390: P314 G2 0x05 0d05
 * 6097: P321 G2 0x05 0d05
 *
 * 获取全局FC控制
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysGetFlowControlMessage
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL      *en
);

/*
 * 基于设备
 *
 * 6390: P313 G2 0x04 0d04
 * 6097: P319 G2 0x04 0d04
 *
 * 设置FC Delay
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysSetFlowCtrlDelay
(
    IN GT_QD_DEV            *dev,
    IN GT_PORT_SPEED_MODE    sp,
    IN GT_U32                delayTime
);
/*
 * 基于设备
 *
 * 6390: P313 G2 0x04 0d04
 * 6097: P319 G2 0x04 0d04
 *
 * 设置FC Delay
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysGetFlowCtrlDelay
(
    IN  GT_QD_DEV    *dev,
    IN  GT_PORT_SPEED_MODE    sp,
    OUT GT_U32        *delayTime
);
/*
 * 基于设备
 *
 * 6390: P316 G2 0x06 0d06
 * 6097: P322 G2 0x06 0d06
 *
 * 设置Dev Routing Table
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysSetDevRoutingTable
(
    IN GT_QD_DEV    *dev,
    IN GT_U32          devNum,
    IN GT_LPORT     port
);

/*
 * 基于设备
 *
 * 6390: P316 G2 0x06 0d06
 * 6097: P322 G2 0x06 0d06
 *
 * 获取Dev Routing Table
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysGetDevRoutingTable
(
    IN  GT_QD_DEV    *dev,
    IN  GT_U32         devNum,
    OUT GT_LPORT     *port
);
/*
 * 基于设备
 *
 * 6390: P317 G2 0x07 0d07
 * 6097: P323 G2 0x07 0d07
 *
 * 设置trunk mask table
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysSetTrunkMaskTable
(
    IN GT_QD_DEV    *dev,
    IN GT_U32          trunkNum,
    IN GT_U32        trunkMask
);

/*
 * 基于设备
 *
 * 6390: P317 G2 0x07 0d07
 * 6097: P323 G2 0x07 0d07
 *
 * 获取trunk mask table
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysGetTrunkMaskTable
(
    IN  GT_QD_DEV    *dev,
    IN  GT_U32         trunkNum,
    OUT GT_U32        *trunkMask
);
/*
 * 基于设备
 *
 * 6390: P317 G2 0x07 0d07
 * 6097: P323 G2 0x07 0d07
 *
 * 设置trunk负载均衡模式
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysSetHashTrunk
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL        en
);

/*
 * 基于设备
 *
 * 6390: P317 G2 0x07 0d07
 * 6097: P323 G2 0x07 0d07
 *
 * 获取trunk负载均衡模式
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysGetHashTrunk
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL      *en
);

/*
 * 基于设备
 *
 * 6390: P318 G2 0x08 0d08
 * 6097: P323 G2 0x08 0d08
 *
 * 设置trunk成员
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysSetTrunkRouting
(
    IN GT_QD_DEV    *dev,
    IN GT_U32          trunkId,
    IN GT_U32        trunkRoute
);

/*
 * 基于设备
 *
 * 6390: P318 G2 0x08 0d08
 * 6097: P323 G2 0x08 0d08
 *
 * 获取trunk成员
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysGetTrunkRouting
(
    IN  GT_QD_DEV    *dev,
    IN  GT_U32         trunkId,
    OUT GT_U32        *trunkRoute
);

/*
 * 基于设备
 *
 * 6390: P315 G2 0x05 0d05
 * 6097: P320 G2 0x05 0d05
 *
 * 设置老化中断使能
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysSetAgeInt
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL        en
);

/*
 * 基于设备
 *
 * 6390: P315 G2 0x05 0d05
 * 6097: P320 G2 0x05 0d05
 *
 * 获取老化中断使能
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysGetAgeInt
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL        *en
);

/*
 * 基于设备
 *
 * 6390: P338 G2 0x0f 0d15
 * 6097: P329 G2 0x0f 0d15
 *
 * 设置snoop pri override使能
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysSetForceSnoopPri
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL        en
);
/*
 * 基于设备
 *
 * 6390: P338 G2 0x0f 0d15
 * 6097: P329 G2 0x0f 0d15
 *
 * 获取snoop pri override使能
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysGetForceSnoopPri
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL      *en
);
/*
 * 基于设备
 *
 * 6390: P338 G2 0x0f 0d15
 * 6097: P329 G2 0x0f 0d15
 *
 * 设置snoop pri override值
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysSetSnoopPri
(
    IN GT_QD_DEV    *dev,
    IN GT_U16        pri
);
/*
 * 基于设备
 *
 * 6390: P338 G2 0x0f 0d15
 * 6097: P329 G2 0x0f 0d15
 *
 * 获取snoop pri override值
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysGetSnoopPri
(
    IN  GT_QD_DEV    *dev,
    OUT GT_U16      *pri
);
/*
 * 基于设备
 *
 * 6390: P338 G2 0x0f 0d15
 * 6097: P329 G2 0x0f 0d15
 *
 * 设置ARP pri override使能
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysSetForceARPPri
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL        en
);
/*
 * 基于设备
 *
 * 6390: P338 G2 0x0f 0d15
 * 6097: P329 G2 0x0f 0d15
 *
 * 获取ARP pri override使能
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysGetForceARPPri
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL      *en
);
/*
 * 基于设备
 *
 * 6390: P338 G2 0x0f 0d15
 * 6097: P329 G2 0x0f 0d15
 *
 * 设置ARP pri override值
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysSetARPPri
(
    IN GT_QD_DEV    *dev,
    IN GT_U16        pri
);
/*
 * 基于设备
 *
 * 6390: P338 G2 0x0f 0d15
 * 6097: P329 G2 0x0f 0d15
 *
 * 获取ARP pri override值
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysGetARPPri
(
    IN  GT_QD_DEV    *dev,
    OUT GT_U16      *pri
);
/*
 * 基于设备
 *
 * 6390: P295 G1 0x1A 0d26
 * 6097: P310 G1 0x1A 0d26
 *
 * 设置CPU端口
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysSetCPUDest
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT        port
);
/*
 * 基于设备
 *
 * 6390: P295 G1 0x1A 0d26
 * 6097: P310 G1 0x1A 0d26
 *
 * 获取CPU端口
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysGetCPUDest
(
    IN  GT_QD_DEV    *dev,
    OUT GT_LPORT      *port
);
/*
 * 基于设备
 *
 * 6390: P295 G1 0x1A 0d26
 * 6097: P310 G1 0x1A 0d26
 *
 * 设置Policy Mirror端口
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysSetMirrorDest
(
    IN GT_QD_DEV    *dev,
    IN GT_LPORT        port
);
/*
 * 基于设备
 *
 * 6390: P295 G1 0x1A 0d26
 * 6097: P310 G1 0x1A 0d26
 *
 * 设置Policy Mirror端口
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysGetMirrorDest
(
    IN  GT_QD_DEV    *dev,
    OUT GT_LPORT      *port
);
/*
 * 基于设备
 *
 * 6390: P305 G1 0x1C 0d28
 * 6097: P312 G1 0x1C 0d28
 *
 * 设置RMU DA Check使能
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysSetRMDACheck
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL         en
);
/*
 * 基于设备
 *
 * 6390: P305 G1 0x1C 0d28
 * 6097: P312 G1 0x1C 0d28
 *
 * 获取RMU DA Check使能
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysGetRMDACheck
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL     *en
);
/*
 * 基于设备
 *
 * 6390: P305 G1 0x1C 0d28
 * 6097: P312 G1 0x1C 0d28
 *
 * 设置RMU模式
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysSetRMUMode
(
    IN GT_QD_DEV    *dev,
    IN GT_RMU        *rmu
);
/*
 * 基于设备
 *
 * 6390: P305 G1 0x1C 0d28
 * 6097: P312 G1 0x1C 0d28
 *
 * 获取RMU模式
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysGetRMUMode
(
    IN  GT_QD_DEV    *dev,
    OUT GT_RMU        *rmu
);
/*
 * 基于设备
 *
 * 6390: P314 G1 0x1C 0d28
 * 6097: P320 G1 0x1C 0d28
 *
 * 设置broadcast是否泛洪
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysSetFloodBC
(
    IN GT_QD_DEV    *dev,
    IN GT_BOOL        en
);
/*
 * 基于设备
 *
 * 6390: P314 G1 0x1C 0d28
 * 6097: P320 G1 0x1C 0d28
 *
 * 获取broadcast是否泛洪
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysGetFloodBC
(
    IN  GT_QD_DEV    *dev,
    OUT GT_BOOL      *en
);
/*
 * 基于设备
 *
 * 6390: P282 G1 0x04 0d04
 * 6097: P290 G1 0x04 0d04
 *
 * 设置是否根据DA来检测ARP数据
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysSetARPwoBC
(
    IN  GT_QD_DEV    *dev,
    IN  GT_BOOL     en
);
/*
 * 基于设备
 *
 * 6390: P282 G1 0x04 0d04
 * 6097: P290 G1 0x04 0d04
 *
 * 获取是否根据DA来检测ARP数据
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gsysGetARPwoBC
(
    IN  GT_QD_DEV     *dev,
    IN  GT_BOOL        *en
);
/*
 * 基于设备
 *
 * 6390: P208 PORT 0x07 0d07
 * 6097: P262 PORT 0x07 0d07
 *
 * 设置端口默认FPri
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gcosSetPortDefaultTc
(
    IN  GT_QD_DEV *dev,
    IN GT_LPORT   port,
    IN GT_U8      trafClass
);
/*
 * 基于端口
 *
 * 6390: P208 PORT 0x07 0d07
 * 6097: P262 PORT 0x07 0d07
 *
 * 获取端口默认FPri
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gcosGetPortDefaultTc
(
    IN  GT_QD_DEV *dev,
    IN GT_LPORT   port,
    OUT GT_U8     *trafClass
);
/*
 * 基于端口
 *
 * 6390: P201 PORT 0x04 0d04
 * 6097: P257 PORT 0x04 0d04
 *
 * 设置TagIfBoth值
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gqosSetPrioMapRule
(
    IN  GT_QD_DEV *dev,
    IN GT_LPORT   port,
    IN GT_BOOL    mode
);
/*
 * 基于端口
 *
 * 6390: P201 PORT 0x04 0d04
 * 6097: P257 PORT 0x04 0d04
 *
 * 获取TagIfBoth值
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gqosGetPrioMapRule
(
    IN  GT_QD_DEV *dev,
    IN  GT_LPORT  port,
    OUT GT_BOOL   *mode
);
/*
 * 基于端口
 *
 * 6390: P208 PORT 0x07 0d07
 * 6097: P262 PORT 0x07 0d07
 *
 * 设置端口默认FPri
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gqosSetDefFPri
(
    IN  GT_QD_DEV  *dev,
    IN  GT_LPORT   port,
    IN  GT_U8      pri
);
/*
 * 基于端口
 *
 * 6390: P208 PORT 0x07 0d07
 * 6097: P262 PORT 0x07 0d07
 *
 * 获取端口默认FPri
 *
 * */
SP_6097F_6390X_6390
GT_STATUS gqosGetDefFPri
(
    IN  GT_QD_DEV  *dev,
    IN  GT_LPORT   port,
    OUT GT_U8      *pri
);
#endif  /* SP_6097F_6390X_6390 */

