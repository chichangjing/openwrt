#ifndef _TM_REGISTERS_DESCRIPTION_H_
#define _TM_REGISTERS_DESCRIPTION_H_

/*
 * (c), Copyright 2009-2014, Marvell International Ltd.  (Marvell)
 *
 * This code contains confidential information of Marvell.
 * No rights are granted herein under any patent, mask work right or copyright
 * of Marvell or any third party. Marvell reserves the right at its sole
 * discretion to request that this code be immediately returned to Marvell.
 * This code is provided "as is". Marvell makes no warranties, expressed,
 * implied or otherwise, regarding its accuracy, completeness or performance.
 */
/**
 * @brief generated file with macros   of TM registers description
 *
 * @file tm_regs_description.h
 *
 * $Revision: 2.0 $
 */

#define FOO(...) __VA_ARGS__
#define foo(x) x


#define		TM_Gen_ExcSrc(operation, ...) \
    TM_REG_ROW_##operation( EvQMgr, 49, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvDrop, 48, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvRCB, 47, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvBAP0, 46, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvBAP1, 45, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvBAP2, 44, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvBAP3, 43, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvBAP4, 42, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvBAP5, 41, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvBAP6, 40, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvBAP7, 39, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvBAP8, 38, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvBAP9, 37, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvBAP10, 36, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvBAP11, 35, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvSched, 34, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvTM2TM, 33, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QMgr, 17, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Drop, 16, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( RCB, 15, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BAP0, 14, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BAP1, 13, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BAP2, 12, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BAP3, 11, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BAP4, 10, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BAP5, 9, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BAP6, 8, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BAP7, 7, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BAP8, 6, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BAP9, 5, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BAP10, 4, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BAP11, 3, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Sched, 2, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( TM2TM, 1, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( TMInt, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_Gen_IntMask(operation, ...) \
    TM_REG_ROW_##operation( EvQMgr, 49, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvDrop, 48, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvRCB, 47, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvBAP0, 46, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvBAP1, 45, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvBAP2, 44, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvBAP3, 43, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvBAP4, 42, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvBAP5, 41, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvBAP6, 40, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvBAP7, 39, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvBAP8, 38, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvBAP9, 37, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvBAP10, 36, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvBAP11, 35, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvSched, 34, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvTM2TM, 33, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QMgr, 17, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Drop, 16, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( RCB, 15, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BAP0, 14, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BAP1, 13, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BAP2, 12, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BAP3, 11, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BAP4, 10, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BAP5, 9, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BAP6, 8, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BAP7, 7, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BAP8, 6, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BAP9, 5, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BAP10, 4, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BAP11, 3, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Sched, 2, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( TM2TM, 1, 1 , 0x0, __VA_ARGS__)\

#define		TM_Gen_FirstExc(operation, ...) \
    TM_REG_ROW_##operation( EvQMgr, 49, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvDrop, 48, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvRCB, 47, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvBAP0, 46, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvBAP1, 45, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvBAP2, 44, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvBAP3, 43, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvBAP4, 42, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvBAP5, 41, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvBAP6, 40, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvBAP7, 39, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvBAP8, 38, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvBAP9, 37, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvBAP10, 36, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvBAP11, 35, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvSched, 34, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( EvTM2TM, 33, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QMgr, 17, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Drop, 16, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( RCB, 15, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BAP0, 14, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BAP1, 13, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BAP2, 12, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BAP3, 11, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BAP4, 10, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BAP5, 9, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BAP6, 8, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BAP7, 7, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BAP8, 6, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BAP9, 5, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BAP10, 4, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BAP11, 3, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Sched, 2, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( TM2TM, 1, 1 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_HWInitPorts(operation, ...) \
    TM_REG_ROW_##operation( Ports, 0, 8 , 0x80, __VA_ARGS__)\

#define		TM_QMgr_PgHdrAdmission(operation, ...) \
    TM_REG_ROW_##operation( Admission, 0, 30 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PkgHdrAdmission(operation, ...) \
    TM_REG_ROW_##operation( Admission, 0, 30 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_Internal_Uncorrectable_ECC_Error_status(operation, ...) \
    TM_REG_ROW_##operation( ECCUncorErr, 0, 64 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PkgFIFOOvrflowErrStus(operation, ...) \
    TM_REG_ROW_##operation( Overflow, 0, 12 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PgFIFOOvrflowErrStus(operation, ...) \
    TM_REG_ROW_##operation( Overflow, 0, 19 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_InitSnapshot(operation, ...) \
    TM_REG_ROW_##operation( InitStus, 0, 16 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PkgBPSnapshot(operation, ...) \
    TM_REG_ROW_##operation( BP, 0, 39 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PkgBPSnapshotMask(operation, ...) \
    TM_REG_ROW_##operation( Mask, 0, 39 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PkgBPSnapshotCnt(operation, ...) \
    TM_REG_ROW_##operation( Clks, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BPClks, 0, 32 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PkgBPSnapshotConfig(operation, ...) \
    TM_REG_ROW_##operation( PortSel, 0, 4 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PgBPSnapshot(operation, ...) \
    TM_REG_ROW_##operation( BP, 0, 52 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PgBPSnapshotMask(operation, ...) \
    TM_REG_ROW_##operation( Mask, 0, 52 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PgBPSnapshotCnt(operation, ...) \
    TM_REG_ROW_##operation( Clks, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BPClks, 0, 32 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PgBPSnapshotConfig(operation, ...) \
    TM_REG_ROW_##operation( PortSel, 0, 3 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PktEnqFSMFIFOLevel(operation, ...) \
    TM_REG_ROW_##operation( Level, 0, 10 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_MessageFIFOLevel(operation, ...) \
    TM_REG_ROW_##operation( Level, 0, 10 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PkgSnpAvailFreeCnt(operation, ...) \
    TM_REG_ROW_##operation( FreePkg, 0, 24 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PgSnpAvailFreeCnt(operation, ...) \
    TM_REG_ROW_##operation( FreePg, 0, 24 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PkgContECCErrLogLSB(operation, ...) \
    TM_REG_ROW_##operation( ECCErrLog, 0, 64 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PgContECCErrLogLSB(operation, ...) \
    TM_REG_ROW_##operation( ECCErrLog, 0, 64 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PkgContECCErrLogMSB(operation, ...) \
    TM_REG_ROW_##operation( ECCErrLog, 0, 18 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PgContECCErrLogMSB(operation, ...) \
    TM_REG_ROW_##operation( ECCErrLog, 0, 18 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_ErrStus(operation, ...) \
    TM_REG_ROW_##operation( DeqFromEmptyQErr, 19, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( CtlPktCRCErr, 18, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( DataParErr, 17, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PktVerMis, 16, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( IntUncECCErr, 15, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( IntCorrECCErr, 14, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ExtUncECCErr, 13, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ExtCorrECCErr, 12, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PktSMSErrPas, 11, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PktSMSErrDrp, 10, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( OORpagesDrp, 9, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( OORpkgesDrp, 8, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( OORpagesPanic, 7, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( OORpkgesPanic, 6, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( FIFOOvrflowErr, 5, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QInstFailErr, 4, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QRmvFailFIFOFullErr, 3, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QRmvFailnonEmptyErr, 2, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( UnInstEnqErr, 1, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ForcedErr, 0, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( mg2tm, 20, 1 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_FirstExc(operation, ...) \
    TM_REG_ROW_##operation( DeqFromEmptyQErr, 19, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( CtlPktCRCErr, 18, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( DataParErr, 17, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PktVerMis, 16, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( IntUncECCErr, 15, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( IntCorrECCErr, 14, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ExtUncECCErr, 13, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ExtCorrECCErr, 12, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PktSMSErrPas, 11, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PktSMSErrDrp, 10, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( OORpagesDrp, 9, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( OORpkgesDrp, 8, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( OORpagesPanic, 7, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( OORpkgesPanic, 6, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( FIFOOvrflowErr, 5, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QInstFailErr, 4, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QRmvFailFIFOFullErr, 3, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QRmvFailnonEmptyErr, 2, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( UnInstEnqErr, 1, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ForcedErr, 0, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( mg2tm, 20, 1 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_ErrCnt(operation, ...) \
    TM_REG_ROW_##operation( Cnt, 0, 16 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_ExcCnt(operation, ...) \
    TM_REG_ROW_##operation( Cnt, 0, 16 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_ExcMask(operation, ...) \
    TM_REG_ROW_##operation( DeqFromEmptyQErr, 19, 1 , 0x1, __VA_ARGS__)\
    TM_REG_ROW_##operation( CtlPktCRCErr, 18, 1 , 0x3, __VA_ARGS__)\
    TM_REG_ROW_##operation( DataParErr, 17, 1 , 0x3, __VA_ARGS__)\
    TM_REG_ROW_##operation( PktVerMis, 16, 1 , 0x3, __VA_ARGS__)\
    TM_REG_ROW_##operation( IntUncECCErr, 15, 1 , 0x3, __VA_ARGS__)\
    TM_REG_ROW_##operation( IntCorrECCErr, 14, 1 , 0x3, __VA_ARGS__)\
    TM_REG_ROW_##operation( ExtUncECCErr, 13, 1 , 0x3, __VA_ARGS__)\
    TM_REG_ROW_##operation( ExtCorrECCErr, 12, 1 , 0x3, __VA_ARGS__)\
    TM_REG_ROW_##operation( PktSMSErrPas, 11, 1 , 0x3, __VA_ARGS__)\
    TM_REG_ROW_##operation( PktSMSErrDrp, 10, 1 , 0x3, __VA_ARGS__)\
    TM_REG_ROW_##operation( OORpagesDrp, 9, 1 , 0x3, __VA_ARGS__)\
    TM_REG_ROW_##operation( OORpkgesDrp, 8, 1 , 0x3, __VA_ARGS__)\
    TM_REG_ROW_##operation( OORpagesPanic, 7, 1 , 0x3, __VA_ARGS__)\
    TM_REG_ROW_##operation( OORpkgesPanic, 6, 1 , 0x3, __VA_ARGS__)\
    TM_REG_ROW_##operation( FIFOOvrflowErr, 5, 1 , 0x3, __VA_ARGS__)\
    TM_REG_ROW_##operation( QInstFailErr, 4, 1 , 0x3, __VA_ARGS__)\
    TM_REG_ROW_##operation( QRmvFailFIFOFullErr, 3, 1 , 0x3, __VA_ARGS__)\
    TM_REG_ROW_##operation( QRmvFailnonEmptyErr, 2, 1 , 0x3, __VA_ARGS__)\
    TM_REG_ROW_##operation( UnInstEnqErr, 1, 1 , 0x3, __VA_ARGS__)\
    TM_REG_ROW_##operation( ForcedErr, 0, 1 , 0x3, __VA_ARGS__)\
    TM_REG_ROW_##operation( mg2tm_error, 20, 1 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_ID(operation, ...) \
    TM_REG_ROW_##operation( SUID, 8, 8 , 0x1, __VA_ARGS__)\
    TM_REG_ROW_##operation( UID, 0, 8 , 0x133, __VA_ARGS__)\

#define		TM_QMgr_ForceErr(operation, ...) \
    TM_REG_ROW_##operation( ForcedErr, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_MgmtEvtStus(operation, ...) \
    TM_REG_ROW_##operation( TMExtMemInitComp, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_MgmtEvtMask(operation, ...) \
    TM_REG_ROW_##operation( TMExtMemInitComp, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_NumOfLADs(operation, ...) \
    TM_REG_ROW_##operation( LADs, 0, 3 , 0x4, __VA_ARGS__)\

#define		TM_QMgr_NumOfPagesPerBank(operation, ...) \
    TM_REG_ROW_##operation( Pages, 0, 15 , 0x1f00, __VA_ARGS__)\

#define		TM_QMgr_MinPkgSize(operation, ...) \
    TM_REG_ROW_##operation( Size, 0, 8 , 0x20, __VA_ARGS__)\

#define		TM_QMgr_NumOfPkgsPerBank(operation, ...) \
    TM_REG_ROW_##operation( Pkgs, 0, 19 , 0x2000, __VA_ARGS__)\

#define		TM_QMgr_DDRChunnel5Map(operation, ...) \
    TM_REG_ROW_##operation( Map, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_CRC16En(operation, ...) \
    TM_REG_ROW_##operation( En, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_TMHeaderProtocolVersion(operation, ...) \
    TM_REG_ROW_##operation( Ver, 0, 4 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_OORPkgDropThresh(operation, ...) \
    TM_REG_ROW_##operation( Thresh, 0, 19 , 0x100, __VA_ARGS__)\

#define		TM_QMgr_OORPageDropThresh(operation, ...) \
    TM_REG_ROW_##operation( Thresh, 0, 15 , 0x100, __VA_ARGS__)\

#define		TM_QMgr_OORPkgPanicThresh(operation, ...) \
    TM_REG_ROW_##operation( Thresh, 0, 19 , 0x20, __VA_ARGS__)\

#define		TM_QMgr_OORPagePanicThresh(operation, ...) \
    TM_REG_ROW_##operation( Thresh, 0, 15 , 0x20, __VA_ARGS__)\

#define		TM_QMgr_StartTMExtMemInit(operation, ...) \
    TM_REG_ROW_##operation( Start, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_TMExtMemInitDone(operation, ...) \
    TM_REG_ROW_##operation( Done, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_QueueInstallCmd(operation, ...) \
    TM_REG_ROW_##operation( Queue, 0, 16 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_QueueRemoveCmd(operation, ...) \
    TM_REG_ROW_##operation( Queue, 0, 16 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_TMtoSMSbp(operation, ...) \
    TM_REG_ROW_##operation( bp, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_FreePkgHdrFIFOBp(operation, ...) \
    TM_REG_ROW_##operation( SetThresh, 32, 4 , 0x3, __VA_ARGS__)\
    TM_REG_ROW_##operation( ClrThresh, 0, 4 , 0x4, __VA_ARGS__)\

#define		TM_QMgr_FreePageHdrFIFOBp(operation, ...) \
    TM_REG_ROW_##operation( SetThresh, 32, 4 , 0x4, __VA_ARGS__)\
    TM_REG_ROW_##operation( ClrThresh, 0, 4 , 0x5, __VA_ARGS__)\

#define		TM_QMgr_FreePkgHdrRtnFIFOBp(operation, ...) \
    TM_REG_ROW_##operation( SetThresh, 32, 5 , 0xa, __VA_ARGS__)\
    TM_REG_ROW_##operation( ClrThresh, 0, 5 , 0x9, __VA_ARGS__)\

#define		TM_QMgr_FreePageHdrRtnFIFOBp(operation, ...) \
    TM_REG_ROW_##operation( SetThresh, 32, 6 , 0x11, __VA_ARGS__)\
    TM_REG_ROW_##operation( ClrThresh, 0, 6 , 0x10, __VA_ARGS__)\

#define		TM_QMgr_PkgHdrRespFIFOBp(operation, ...) \
    TM_REG_ROW_##operation( SetThresh, 32, 5 , 0x2, __VA_ARGS__)\
    TM_REG_ROW_##operation( ClrThresh, 0, 5 , 0x3, __VA_ARGS__)\

#define		TM_QMgr_PageHdrRespFIFOBp(operation, ...) \
    TM_REG_ROW_##operation( SetThresh, 32, 5 , 0x3, __VA_ARGS__)\
    TM_REG_ROW_##operation( ClrThresh, 0, 5 , 0x4, __VA_ARGS__)\

#define		TM_QMgr_RemoveQueueFIFOBp(operation, ...) \
    TM_REG_ROW_##operation( SetThresh, 32, 4 , 0x6, __VA_ARGS__)\
    TM_REG_ROW_##operation( ClrThresh, 0, 4 , 0x5, __VA_ARGS__)\

#define		TM_QMgr_PktEnqFSMFIFOBp(operation, ...) \
    TM_REG_ROW_##operation( SetThresh, 32, 5 , 0x8, __VA_ARGS__)\
    TM_REG_ROW_##operation( ClrThresh, 0, 5 , 0x7, __VA_ARGS__)\

#define		TM_QMgr_TMtoTMIncPktsFIFOBp(operation, ...) \
    TM_REG_ROW_##operation( SetThresh, 32, 5 , 0xe, __VA_ARGS__)\
    TM_REG_ROW_##operation( ClrThresh, 0, 5 , 0xd, __VA_ARGS__)\

#define		TM_QMgr_PgCtlRdCacheFIFOBp(operation, ...) \
    TM_REG_ROW_##operation( SetThresh, 32, 6 , 0x3d, __VA_ARGS__)\
    TM_REG_ROW_##operation( ClrThresh, 0, 6 , 0x3c, __VA_ARGS__)\

#define		TM_QMgr_InputSynchFifoConf(operation, ...) \
    TM_REG_ROW_##operation( AlmstFullWm, 0, 5 , 0xd, __VA_ARGS__)\

#define		TM_QMgr_FromPipeNonErrPktCnt(operation, ...) \
    TM_REG_ROW_##operation( Cnt, 0, 64 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_FromPipeErrDrpPktCnt(operation, ...) \
    TM_REG_ROW_##operation( Cnt, 0, 64 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_FromPipeErrPasPktCnt(operation, ...) \
    TM_REG_ROW_##operation( Cnt, 0, 64 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_UninstalledQueueDrpPktCnt(operation, ...) \
    TM_REG_ROW_##operation( Cnt, 0, 64 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_VerMisDrpPktCnt(operation, ...) \
    TM_REG_ROW_##operation( Cnt, 0, 64 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_TMtoTMCRCErrDrpPktCnt(operation, ...) \
    TM_REG_ROW_##operation( Cnt, 0, 64 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PerfCntrsPortAlloc(operation, ...) \
    TM_REG_ROW_##operation( Set3Map, 24, 6 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Set2Map, 16, 6 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Set1Map, 8, 6 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Set0Map, 0, 6 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PerfCntrsTimerPer(operation, ...) \
    TM_REG_ROW_##operation( Period, 0, 14 , 0x3fff, __VA_ARGS__)\

#define		TM_QMgr_UninstalledQueueLog(operation, ...) \
    TM_REG_ROW_##operation( QueueLog, 0, 16 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_Mg2tm_error_log(operation, ...) \
    TM_REG_ROW_##operation( Address, 0, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( suid, 32, 8 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_Eco_Reg(operation, ...) \
    TM_REG_ROW_##operation( Eco1, 0, 32 , 0x1, __VA_ARGS__)\
    TM_REG_ROW_##operation( Eco0, 32, 32 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_EccConfig(operation, ...) \
    TM_REG_ROW_##operation( Lock_First_Erronous_Event, 0, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Ecc_Insert_Mode, 1, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PktEnqEccEccInsEnable, 2, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PkgBothEccEccInsEnable, 4, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PkgTailEccEccInsEnable, 5, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PkgHeadEccEccInsEnable, 6, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( FreeRdHeadEccEccInsEnable, 7, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( CRDRamEccEccInsEnable, 8, 5 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( FreeRdNextEccEccInsEnable, 13, 5 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PageEnqEccEccInsEnable, 18, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PageDeqCacheEccEccInsEnable, 19, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PageDeqNextEccEccInsEnable, 20, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PageDeqCurrEccEccInsEnable, 21, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PkgFreeWrEccTailEccInsEnable, 22, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PkgFreeWrEccFifoEccInsEnable, 23, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PageFreeWrEccTailEccInsEnable, 24, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PageFreeWrEccFifoEccInsEnable, 25, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PageFreeRdHeadEccEccInsEnable, 26, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PageFreeRdNextEccEccInsEnable, 27, 5 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PageCRDRamEccEccInsEnable, 32, 5 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PktSyncRanEccEccInsEnable, 3, 1 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_Internal_Correctable_ECC_Error_status(operation, ...) \
    TM_REG_ROW_##operation( ECCCorErr, 0, 64 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PkgEccMemParams0(operation, ...) \
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Address, 8, 24 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PkgEccMemParams1(operation, ...) \
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Address, 8, 24 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PkgEccMemParams2(operation, ...) \
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Address, 8, 24 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PkgEccMemParams3(operation, ...) \
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Address, 8, 24 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PkgEccMemParams4(operation, ...) \
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Address, 8, 24 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PkgEccMemParams5(operation, ...) \
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Address, 8, 24 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PkgEccMemParams6(operation, ...) \
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Address, 8, 24 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PkgEccMemParams7(operation, ...) \
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Address, 8, 24 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PkgEccMemParams8(operation, ...) \
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Address, 8, 24 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PkgEccMemParams9(operation, ...) \
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Address, 8, 24 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PkgEccMemParams10(operation, ...) \
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Address, 8, 24 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PkgEccMemParams11(operation, ...) \
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Address, 8, 24 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PkgEccMemParams12(operation, ...) \
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Address, 8, 24 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PkgEccMemParams13(operation, ...) \
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Address, 8, 24 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PkgEccMemParams14(operation, ...) \
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Address, 8, 24 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PkgEccMemParams15(operation, ...) \
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Address, 8, 24 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PageEccMemParams0(operation, ...) \
    TM_REG_ROW_##operation( Defected_Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PageEccMemParams1(operation, ...) \
    TM_REG_ROW_##operation( Defected_Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PageEccMemParams2(operation, ...) \
    TM_REG_ROW_##operation( Defected_Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PageEccMemParams3(operation, ...) \
    TM_REG_ROW_##operation( Defected_Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PageEccMemParams4(operation, ...) \
    TM_REG_ROW_##operation( Defected_Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PageEccMemParams5(operation, ...) \
    TM_REG_ROW_##operation( Defected_Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PageEccMemParams6(operation, ...) \
    TM_REG_ROW_##operation( Defected_Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PageEccMemParams7(operation, ...) \
    TM_REG_ROW_##operation( Defected_Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PageEccMemParams8(operation, ...) \
    TM_REG_ROW_##operation( Defected_Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PageEccMemParams9(operation, ...) \
    TM_REG_ROW_##operation( Defected_Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PageEccMemParams10(operation, ...) \
    TM_REG_ROW_##operation( Defected_Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PageEccMemParams11(operation, ...) \
    TM_REG_ROW_##operation( Defected_Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PageEccMemParams12(operation, ...) \
    TM_REG_ROW_##operation( Defected_Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PageEccMemParams13(operation, ...) \
    TM_REG_ROW_##operation( Defected_Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PageEccMemParams14(operation, ...) \
    TM_REG_ROW_##operation( Defected_Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PageEccMemParams15(operation, ...) \
    TM_REG_ROW_##operation( Defected_Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PageEccMemParams16(operation, ...) \
    TM_REG_ROW_##operation( Defected_Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PageEccMemParams17(operation, ...) \
    TM_REG_ROW_##operation( Defected_Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PageEccMemParams18(operation, ...) \
    TM_REG_ROW_##operation( Defected_Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Defected_Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_ROC_Shadow(operation, ...) \
    TM_REG_ROW_##operation( Shadow, 0, 64 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PageAdmission(operation, ...) \
    TM_REG_ROW_##operation( Admission, 0, 14 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PgDeqFSMCache(operation, ...) \
    TM_REG_ROW_##operation( PgDeqFSMCache, 0, 30 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PkgBoth(operation, ...) \
    TM_REG_ROW_##operation( Status, 0, 57 , 0x3ffffff, __VA_ARGS__)\

#define		TM_QMgr_PkgTail(operation, ...) \
    TM_REG_ROW_##operation( Status, 0, 27 , 0x7ffffff, __VA_ARGS__)\

#define		TM_QMgr_PkgHead(operation, ...) \
    TM_REG_ROW_##operation( Status, 0, 52 , 0x1fffff, __VA_ARGS__)\

#define		TM_QMgr_PgEnq(operation, ...) \
    TM_REG_ROW_##operation( Status, 0, 30 , 0x3fffffff, __VA_ARGS__)\

#define		TM_QMgr_PgDeq(operation, ...) \
    TM_REG_ROW_##operation( Status, 0, 48 , 0x1ffff, __VA_ARGS__)\

#define		TM_QMgr_PgDeqCacheLSB(operation, ...) \
    TM_REG_ROW_##operation( Status, 0, 60 , 0x1fffffff, __VA_ARGS__)\

#define		TM_QMgr_PgDeqCacheMSB(operation, ...) \
    TM_REG_ROW_##operation( Status, 0, 31 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PortChunksEmitPerSel(operation, ...) \
    TM_REG_ROW_##operation( Chunks, 0, 4 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_ReorderBufferAdmission(operation, ...) \
    TM_REG_ROW_##operation( Chunks, 0, 13 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_ReorderBufferAdmission(operation, ...) \
    TM_REG_ROW_##operation( Chunks, 0, 13 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PerfCntrsSets(operation, ...) \
    TM_REG_ROW_##operation( PerfCntSet, 0, 64 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_FreePkgStus_Grp(operation, ...) \
    TM_REG_ROW_##operation( Pkgs, 0, 19 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_FreePgStus_Grp(operation, ...) \
    TM_REG_ROW_##operation( Pgs, 0, 15 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_PortFlush(operation, ...) \
    TM_REG_ROW_##operation( Flush, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_QueueFlush(operation, ...) \
    TM_REG_ROW_##operation( Flush, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_QMgr_QueueLength(operation, ...) \
    TM_REG_ROW_##operation( Len, 0, 29 , 0x0, __VA_ARGS__)\

/*
   to  change     all entries  with  .Table,  Cos, & Color  to structures
*/
#define		TM_Drop_ErrStus(operation, ...) \
    TM_REG_ROW_##operation( AgingFifoOverflow, 5, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QueryReqFifoOverflow, 4, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QuesryRespSyncFifoFull, 3, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( UncECCErr, 2, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( CorrECCErr, 1, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ForcedErr, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_Drop_FirstExc(operation, ...) \
    TM_REG_ROW_##operation( AgingFifoOverflow, 5, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QueryReqFifoOverflow, 4, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QuesryRespSyncFifoFull, 3, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( UncECCErr, 2, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( CorrECCErr, 1, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ForcedErr, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_Drop_ErrCnt(operation, ...) \
    TM_REG_ROW_##operation( Cnt, 0, 16 , 0x0, __VA_ARGS__)\

#define		TM_Drop_ExcCnt(operation, ...) \
    TM_REG_ROW_##operation( Cnt, 0, 16 , 0x0, __VA_ARGS__)\

#define		TM_Drop_ExcMask(operation, ...) \
    TM_REG_ROW_##operation( AgingFifoOverflow, 5, 1 , 0x1, __VA_ARGS__)\
    TM_REG_ROW_##operation( QueryReqFifoOverflow, 4, 1 , 0x3, __VA_ARGS__)\
    TM_REG_ROW_##operation( QuesryRespSyncFifoFull, 3, 1 , 0x3, __VA_ARGS__)\
    TM_REG_ROW_##operation( UncECCErr, 2, 1 , 0x3, __VA_ARGS__)\
    TM_REG_ROW_##operation( CorrECCErr, 1, 1 , 0x3, __VA_ARGS__)\
    TM_REG_ROW_##operation( ForcedErr, 0, 1 , 0x3, __VA_ARGS__)\

#define		TM_Drop_Id(operation, ...) \
    TM_REG_ROW_##operation( SUID, 8, 8 , 0x2, __VA_ARGS__)\
    TM_REG_ROW_##operation( UID, 0, 8 , 0x33, __VA_ARGS__)\

#define		TM_Drop_ForceErr(operation, ...) \
    TM_REG_ROW_##operation( ForcedErr, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_Drop_WREDDropProbMode(operation, ...) \
    TM_REG_ROW_##operation( Port, 32, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Clvl, 24, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Blvl, 16, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Alvl, 8, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Queue, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_Drop_WREDMaxProbModePerColor(operation, ...) \
    TM_REG_ROW_##operation( Port, 32, 6 , 0x2a, __VA_ARGS__)\
    TM_REG_ROW_##operation( Clvl, 24, 6 , 0x2a, __VA_ARGS__)\
    TM_REG_ROW_##operation( Blvl, 16, 6 , 0x2a, __VA_ARGS__)\
    TM_REG_ROW_##operation( Alvl, 8, 6 , 0x2a, __VA_ARGS__)\
    TM_REG_ROW_##operation( Queue, 0, 6 , 0x2a, __VA_ARGS__)\

#define		TM_Drop_DPSource(operation, ...) \
    TM_REG_ROW_##operation( PortSrc, 32, 3 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ClvlSrc, 24, 3 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BlvlSrc, 16, 3 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( AlvlSrc, 8, 3 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QueueSrc, 0, 3 , 0x0, __VA_ARGS__)\

#define		TM_Drop_RespLocalDPSel(operation, ...) \
    TM_REG_ROW_##operation( PortDPSel, 8, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( DPSel, 0, 3 , 0x0, __VA_ARGS__)\

#define		TM_Drop_TMtoTMPktGenQuantum(operation, ...) \
    TM_REG_ROW_##operation( Quantum, 0, 16 , 0x1000, __VA_ARGS__)\

#define		TM_Drop_TMtoTMDPCoSSel(operation, ...) \
    TM_REG_ROW_##operation( CDPCoSSel, 0, 3 , 0x0, __VA_ARGS__)\

#define		TM_Drop_AgingUpdEnable(operation, ...) \
    TM_REG_ROW_##operation( En, 0, 1 , 0x1, __VA_ARGS__)\

#define		TM_Drop_Drp_Decision_to_Query_debug(operation, ...) \
    TM_REG_ROW_##operation( Debug_En, 32, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Decision_fields, 0, 27 , 0x0, __VA_ARGS__)\

#define		TM_Drop_Drp_Decision_hierarchy_to_Query_debug(operation, ...) \
    TM_REG_ROW_##operation( Hierarchy_fields, 0, 40 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccConfig(operation, ...) \
    TM_REG_ROW_##operation( LockFirstErronousEvent, 0, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ErrInsMode, 1, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QAqlErrInsEnable, 2, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( AAqlErrInsEnable, 3, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BAqlErrInsEnable, 4, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( CAqlErrInsEnable, 5, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PAqlErrInsEnable, 6, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PAqlCoSErrInsEnable, 7, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QCoSErrInsEnable, 8, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( AProfPtrErrInsEnable, 9, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BProfPtrErrInsEnable, 10, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( CProfPtrErrInsEnable, 11, 8 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QCurveErrInsEnable, 19, 3 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ACurveErrInsEnable, 22, 3 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QProfErrInsEnable, 25, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( AProfErrInsEnable, 26, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BProfErrInsEnable, 27, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( CProfErrInsEnable, 28, 8 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PProfErrInsEnable, 36, 8 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PGProfErrInsEnable, 44, 1 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams0(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams1(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams2(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams3(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams4(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams5(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams6(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams7(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams8(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams9(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams10(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams11(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams12(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams13(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams14(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams15(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams16(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams17(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams18(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams19(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams20(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams21(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams22(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams23(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams24(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams25(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams26(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams27(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams28(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams29(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams30(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams31(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams32(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams33(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams34(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams35(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams36(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams37(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams38(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams39(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams40(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams41(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_EccMemParams42(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_PortDropPrfWREDParams_CoS(operation, ...) \
    TM_REG_ROW_##operation( AQLExp, 56, 4 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ColorTDEn, 48, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ScaleExpColor2, 40, 5 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ScaleExpColor1, 32, 5 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ScaleExpColor0, 24, 5 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( CurveIndexColor2, 16, 2 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( CurveIndexColor1, 8, 2 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( CurveIndexColor0, 0, 2 , 0x0, __VA_ARGS__)\

#define		TM_Drop_PortDropPrfWREDScaleRatio_CoS(operation, ...) \
    TM_REG_ROW_##operation( ScaleRatioColor2, 32, 10 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ScaleRatioColor1, 16, 10 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ScaleRatioColor0, 0, 10 , 0x0, __VA_ARGS__)\

#define		TM_Drop_PortDropPrfWREDMinThresh_CoS(operation, ...) \
    TM_REG_ROW_##operation( MinTHColor2, 32, 10 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( MinTHColor1, 16, 10 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( MinTHColor0, 0, 10 , 0x0, __VA_ARGS__)\

#define		TM_Drop_PortDropPrfWREDDPRatio_CoS(operation, ...) \
    TM_REG_ROW_##operation( DPRatio2, 16, 4 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( DPRatio1, 8, 6 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( DPRatio0, 0, 6 , 0x0, __VA_ARGS__)\

#define		TM_Drop_PortDropPrfTailDrpThresh_CoS(operation, ...) \
    TM_REG_ROW_##operation( TailDropThreshRes, 32, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( TailDropThresh, 0, 19 , 0x0, __VA_ARGS__)\


#define		TM_Drop_PortREDCurve_CoS(operation, ...) \
    TM_REG_ROW_##operation( Prob, 0, 6 , 0x0, __VA_ARGS__)\

#define		TM_Drop_PortDropPrfWREDParamsG(operation, ...) \
    TM_REG_ROW_##operation( AQLExp, 56, 4 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ColorTDEn, 48, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ScaleExpColor2, 40, 5 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ScaleExpColor1, 32, 5 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ScaleExpColor0, 24, 5 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( CurveIndexColor2, 16, 2 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( CurveIndexColor1, 8, 2 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( CurveIndexColor0, 0, 2 , 0x0, __VA_ARGS__)\

#define		TM_Drop_PortDropPrfWREDScaleRatioG(operation, ...) \
    TM_REG_ROW_##operation( ScaleRatioColor2, 32, 10 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ScaleRatioColor1, 16, 10 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ScaleRatioColor0, 0, 10 , 0x0, __VA_ARGS__)\

#define		TM_Drop_PortDropPrfWREDMinThreshG(operation, ...) \
    TM_REG_ROW_##operation( MinTHColor2, 32, 10 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( MinTHColor1, 16, 10 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( MinTHColor0, 0, 10 , 0x0, __VA_ARGS__)\

#define		TM_Drop_PortDropPrfWREDDPRatioG(operation, ...) \
    TM_REG_ROW_##operation( DPRatio2, 16, 4 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( DPRatio1, 8, 6 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( DPRatio0, 0, 6 , 0x0, __VA_ARGS__)\

#define		TM_Drop_PortDropPrfTailDrpThreshG(operation, ...) \
    TM_REG_ROW_##operation( TailDropThreshRes, 32, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( TailDropThresh, 0, 19 , 0x0, __VA_ARGS__)\

#define		TM_Drop_PortREDCurveG(operation, ...) \
    TM_REG_ROW_##operation( Prob, 0, 6 , 0x0, __VA_ARGS__)\

#define		TM_Drop_ClvlDropProfPtr_CoS(operation, ...) \
    TM_REG_ROW_##operation( ProfPtr7, 56, 6 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ProfPtr6, 48, 6 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ProfPtr5, 40, 6 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ProfPtr4, 32, 6 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ProfPtr3, 24, 6 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ProfPtr2, 16, 6 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ProfPtr1, 8, 6 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ProfPtr0, 0, 6 , 0x0, __VA_ARGS__)\

#define		TM_Drop_ClvlDropPrfWREDParams_CoS(operation, ...) \
    TM_REG_ROW_##operation( AQLExp, 56, 4 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ColorTDEn, 48, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ScaleExpColor2, 40, 5 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ScaleExpColor1, 32, 5 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ScaleExpColor0, 24, 5 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( CurveIndexColor2, 16, 2 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( CurveIndexColor1, 8, 2 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( CurveIndexColor0, 0, 2 , 0x0, __VA_ARGS__)\

#define		TM_Drop_ClvlDropPrfWREDScaleRatio_CoS(operation, ...) \
    TM_REG_ROW_##operation( ScaleRatioColor2, 32, 10 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ScaleRatioColor1, 16, 10 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ScaleRatioColor0, 0, 10 , 0x0, __VA_ARGS__)\

#define		TM_Drop_ClvlDropPrfWREDMinThresh_CoS(operation, ...) \
    TM_REG_ROW_##operation( MinTHColor2, 32, 10 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( MinTHColor1, 16, 10 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( MinTHColor0, 0, 10 , 0x0, __VA_ARGS__)\

#define		TM_Drop_ClvlDropPrfWREDDPRatio_CoS(operation, ...) \
    TM_REG_ROW_##operation( DPRatio2, 16, 4 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( DPRatio1, 8, 6 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( DPRatio0, 0, 6 , 0x0, __VA_ARGS__)\

#define		TM_Drop_ClvlDropPrfTailDrpThresh_CoS(operation, ...) \
    TM_REG_ROW_##operation( TailDropThreshRes, 32, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( TailDropThresh, 0, 19 , 0x0, __VA_ARGS__)\

#define		TM_Drop_ClvlREDCurve_CoS(operation, ...) \
    TM_REG_ROW_##operation( Prob, 0, 6 , 0x0, __VA_ARGS__)\

#define		TM_Drop_BlvlDropProfPtr(operation, ...) \
    TM_REG_ROW_##operation( ProfPtr7, 56, 6 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ProfPtr6, 48, 6 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ProfPtr5, 40, 6 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ProfPtr4, 32, 6 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ProfPtr3, 24, 6 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ProfPtr2, 16, 6 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ProfPtr1, 8, 6 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ProfPtr0, 0, 6 , 0x0, __VA_ARGS__)\

#define		TM_Drop_BlvlDropPrfWREDParams(operation, ...) \
    TM_REG_ROW_##operation( AQLExp, 56, 4 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ColorTDEn, 48, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ScaleExpColor2, 40, 5 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ScaleExpColor1, 32, 5 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ScaleExpColor0, 24, 5 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( CurveIndexColor2, 16, 2 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( CurveIndexColor1, 8, 2 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( CurveIndexColor0, 0, 2 , 0x0, __VA_ARGS__)\

#define		TM_Drop_BlvlDropPrfWREDScaleRatio(operation, ...) \
    TM_REG_ROW_##operation( ScaleRatioColor2, 32, 10 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ScaleRatioColor1, 16, 10 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ScaleRatioColor0, 0, 10 , 0x0, __VA_ARGS__)\

#define		TM_Drop_BlvlDropPrfWREDMinThresh(operation, ...) \
    TM_REG_ROW_##operation( MinTHColor2, 32, 10 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( MinTHColor1, 16, 10 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( MinTHColor0, 0, 10 , 0x0, __VA_ARGS__)\

#define		TM_Drop_BlvlDropPrfWREDDPRatio(operation, ...) \
    TM_REG_ROW_##operation( DPRatio2, 16, 4 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( DPRatio1, 8, 6 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( DPRatio0, 0, 6 , 0x0, __VA_ARGS__)\

#define		TM_Drop_BlvlDropPrfTailDrpThresh(operation, ...) \
    TM_REG_ROW_##operation( TailDropThreshRes, 32, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( TailDropThresh, 0, 19 , 0x0, __VA_ARGS__)\

#define		TM_Drop_BlvlREDCurve_Table(operation, ...) \
    TM_REG_ROW_##operation( Prob, 0, 6 , 0x0, __VA_ARGS__)\

#define		TM_Drop_AlvlDropProfPtr(operation, ...) \
    TM_REG_ROW_##operation( ProfPtr3, 48, 8 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ProfPtr2, 32, 8 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ProfPtr1, 16, 8 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ProfPtr0, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Drop_AlvlDropPrfWREDParams(operation, ...) \
    TM_REG_ROW_##operation( AQLExp, 56, 4 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ColorTDEn, 48, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ScaleExpColor2, 40, 5 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ScaleExpColor1, 32, 5 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ScaleExpColor0, 24, 5 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( CurveIndexColor2, 16, 3 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( CurveIndexColor1, 8, 3 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( CurveIndexColor0, 0, 3 , 0x0, __VA_ARGS__)\

#define		TM_Drop_AlvlDropPrfWREDScaleRatio(operation, ...) \
    TM_REG_ROW_##operation( ScaleRatioColor2, 32, 10 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ScaleRatioColor1, 16, 10 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ScaleRatioColor0, 0, 10 , 0x0, __VA_ARGS__)\

#define		TM_Drop_AlvlDropPrfWREDMinThresh(operation, ...) \
    TM_REG_ROW_##operation( MinTHColor2, 32, 10 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( MinTHColor1, 16, 10 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( MinTHColor0, 0, 10 , 0x0, __VA_ARGS__)\

#define		TM_Drop_AlvlDropPrfWREDDPRatio(operation, ...) \
    TM_REG_ROW_##operation( DPRatio2, 16, 4 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( DPRatio1, 8, 6 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( DPRatio0, 0, 6 , 0x0, __VA_ARGS__)\

#define		TM_Drop_AlvlDropPrfTailDrpThresh(operation, ...) \
    TM_REG_ROW_##operation( TailDropThreshRes, 32, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( TailDropThresh, 0, 19 , 0x0, __VA_ARGS__)\

#define		TM_Drop_AlvlREDCurve_Color(operation, ...) \
    TM_REG_ROW_##operation( Prob, 0, 6 , 0x0, __VA_ARGS__)\

#define		TM_Drop_QueueDropProfPtr(operation, ...) \
    TM_REG_ROW_##operation( ProfPtr3, 48, 11 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ProfPtr2, 32, 11 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ProfPtr1, 16, 11 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ProfPtr0, 0, 11 , 0x0, __VA_ARGS__)\

#define		TM_Drop_QueueDropPrfWREDParams(operation, ...) \
    TM_REG_ROW_##operation( AQLExp, 56, 4 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ColorTDEn, 48, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ScaleExpColor2, 40, 5 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ScaleExpColor1, 32, 5 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ScaleExpColor0, 24, 5 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( CurveIndexColor2, 16, 3 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( CurveIndexColor1, 8, 3 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( CurveIndexColor0, 0, 3 , 0x0, __VA_ARGS__)\

#define		TM_Drop_QueueDropPrfWREDScaleRatio(operation, ...) \
    TM_REG_ROW_##operation( ScaleRatioColor2, 32, 10 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ScaleRatioColor1, 16, 10 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ScaleRatioColor0, 0, 10 , 0x0, __VA_ARGS__)\

#define		TM_Drop_QueueDropPrfWREDMinThresh(operation, ...) \
    TM_REG_ROW_##operation( MinTHColor2, 32, 10 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( MinTHColor1, 16, 10 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( MinTHColor0, 0, 10 , 0x0, __VA_ARGS__)\

#define		TM_Drop_QueueDropPrfWREDDPRatio(operation, ...) \
    TM_REG_ROW_##operation( DPRatio2, 16, 4 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( DPRatio1, 8, 6 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( DPRatio0, 0, 6 , 0x0, __VA_ARGS__)\

#define		TM_Drop_QueueDropPrfTailDrpThresh(operation, ...) \
    TM_REG_ROW_##operation( TailDropThreshRes, 32, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( TailDropThresh, 0, 19 , 0x0, __VA_ARGS__)\

#define		TM_Drop_QueueREDCurve_Color(operation, ...) \
    TM_REG_ROW_##operation( Prob, 0, 6 , 0x0, __VA_ARGS__)\

#define		TM_Drop_QueueCoSConf(operation, ...) \
    TM_REG_ROW_##operation( QueueCos3, 9, 3 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QueueCos2, 6, 3 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QueueCos1, 3, 3 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QueueCos0, 0, 3 , 0x0, __VA_ARGS__)\

#define		TM_Drop_PortInstAndAvgQueueLength(operation, ...) \
    TM_REG_ROW_##operation( AQL, 32, 29 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QL, 0, 29 , 0x0, __VA_ARGS__)\

#define		TM_Drop_PortDropProb(operation, ...) \
    TM_REG_ROW_##operation( DropProb, 0, 13 , 0x0, __VA_ARGS__)\

#define		TM_Drop_PortInstAndAvgQueueLengthPerCoS(operation, ...) \
    TM_REG_ROW_##operation( AQL, 32, 29 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QL, 0, 29 , 0x0, __VA_ARGS__)\

#define		TM_Drop_PortDropProbPerCoS(operation, ...) \
    TM_REG_ROW_##operation( DropProb, 0, 13 , 0x0, __VA_ARGS__)\

#define		TM_Drop_ClvlInstAndAvgQueueLength(operation, ...) \
    TM_REG_ROW_##operation( AQL, 32, 29 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QL, 0, 29 , 0x0, __VA_ARGS__)\

#define		TM_Drop_ClvlDropProb(operation, ...) \
    TM_REG_ROW_##operation( DropProb, 0, 13 , 0x0, __VA_ARGS__)\

#define		TM_Drop_BlvlInstAndAvgQueueLength(operation, ...) \
    TM_REG_ROW_##operation( AQL, 32, 29 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QL, 0, 29 , 0x0, __VA_ARGS__)\

#define		TM_Drop_BlvlDropProb(operation, ...) \
    TM_REG_ROW_##operation( DropProb, 0, 13 , 0x0, __VA_ARGS__)\

#define		TM_Drop_AlvlInstAndAvgQueueLength(operation, ...) \
    TM_REG_ROW_##operation( AQL, 32, 29 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QL, 0, 29 , 0x0, __VA_ARGS__)\

#define		TM_Drop_AlvlDropProb(operation, ...) \
    TM_REG_ROW_##operation( DropProb, 0, 13 , 0x0, __VA_ARGS__)\

#define		TM_Drop_QueueAvgQueueLength(operation, ...) \
    TM_REG_ROW_##operation( AQL, 0, 29 , 0x0, __VA_ARGS__)\

#define		TM_Drop_QueueDropProb(operation, ...) \
    TM_REG_ROW_##operation( DropProb, 0, 13 , 0x0, __VA_ARGS__)\

/****************************************************************************************************/
#define		TM_RCB_DummyResponseMode(operation, ...) \
    TM_REG_ROW_##operation( Mode, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_RCB_DecoderFIFOLevel(operation, ...) \
    TM_REG_ROW_##operation( Level, 0, 10 , 0x0, __VA_ARGS__)\

#define		TM_RCB_BPSnapshot0(operation, ...) \
    TM_REG_ROW_##operation( BP, 0, 58 , 0x0, __VA_ARGS__)\

#define		TM_RCB_BPSnapshot1(operation, ...) \
    TM_REG_ROW_##operation( BP, 0, 58 , 0x0, __VA_ARGS__)\

#define		TM_RCB_BPSnapshot2(operation, ...) \
    TM_REG_ROW_##operation( BP, 0, 58 , 0x0, __VA_ARGS__)\

#define		TM_RCB_BPSnapshot3(operation, ...) \
    TM_REG_ROW_##operation( BP, 0, 58 , 0x0, __VA_ARGS__)\

#define		TM_RCB_BPSnapshotMask0(operation, ...) \
    TM_REG_ROW_##operation( BPMask, 0, 64 , 0x0, __VA_ARGS__)\

#define		TM_RCB_BPSnapshotMask1(operation, ...) \
    TM_REG_ROW_##operation( BPMask, 0, 64 , 0x0, __VA_ARGS__)\

#define		TM_RCB_BPSnapshotMask2(operation, ...) \
    TM_REG_ROW_##operation( BPMask, 0, 64 , 0x0, __VA_ARGS__)\

#define		TM_RCB_BPSnapshotMask3(operation, ...) \
    TM_REG_ROW_##operation( BPMask, 0, 64 , 0x0, __VA_ARGS__)\

#define		TM_RCB_BPSnapshotCnt(operation, ...) \
    TM_REG_ROW_##operation( Clks, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BPClks, 0, 32 , 0x0, __VA_ARGS__)\

#define		TM_RCB_FIFOOvrflowErrStus(operation, ...) \
    TM_REG_ROW_##operation( Ovrflow, 0, 15 , 0x0, __VA_ARGS__)\

#define		TM_RCB_CRCErrLog(operation, ...) \
    TM_REG_ROW_##operation( Log, 0, 45 , 0x0, __VA_ARGS__)\

#define		TM_RCB_ExtECCErrCtlLog(operation, ...) \
    TM_REG_ROW_##operation( Log, 0, 53 , 0x0, __VA_ARGS__)\

#define		TM_RCB_ErrStus(operation, ...) \
    TM_REG_ROW_##operation( PortMemUncECCErr, 8, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PortMemCorrECCErr, 7, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PktHeaderUncECCErr, 6, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PktHeaderCorrECCErr, 5, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PktCRCErr, 4, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( TxSyncFifoCorrEccErr, 2, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( FIFOOvrflowErr, 1, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ForcedErr, 0, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( TxSyncFifoUnCorrEccErr, 3, 1 , 0x0, __VA_ARGS__)\

#define		TM_RCB_FirstExc(operation, ...) \
    TM_REG_ROW_##operation( PortMemUncECCErr, 8, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PortMemCorrECCErr, 7, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PktHeaderUncECCErr, 6, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PktHeaderCorrECCErr, 5, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PktCRCErr, 4, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( TxSyncFifoCorrEccErr, 2, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( FIFOOvrflowErr, 1, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ForcedErr, 0, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( TxSyncFifoUnCorrEccErr, 3, 1 , 0x0, __VA_ARGS__)\

#define		TM_RCB_ErrCnt(operation, ...) \
    TM_REG_ROW_##operation( Cnt, 0, 16 , 0x0, __VA_ARGS__)\

#define		TM_RCB_ExcCnt(operation, ...) \
    TM_REG_ROW_##operation( Cnt, 0, 16 , 0x0, __VA_ARGS__)\

#define		TM_RCB_ExcMask(operation, ...) \
    TM_REG_ROW_##operation( PortMemUncECCErr, 8, 1 , 0x1, __VA_ARGS__)\
    TM_REG_ROW_##operation( PortMemCorrECCErr, 7, 1 , 0x3, __VA_ARGS__)\
    TM_REG_ROW_##operation( PktHeaderUncECCErr, 6, 1 , 0x3, __VA_ARGS__)\
    TM_REG_ROW_##operation( PktHeaderCorrECCErr, 5, 1 , 0x3, __VA_ARGS__)\
    TM_REG_ROW_##operation( PktCRCErr, 4, 1 , 0x3, __VA_ARGS__)\
    TM_REG_ROW_##operation( TxSyncFifoCorrEccErr, 2, 1 , 0x1, __VA_ARGS__)\
    TM_REG_ROW_##operation( FIFOOvrflowErr, 1, 1 , 0x3, __VA_ARGS__)\
    TM_REG_ROW_##operation( ForcedErr, 0, 1 , 0x3, __VA_ARGS__)\
    TM_REG_ROW_##operation( TxSyncFifoUnCorrEccErr, 3, 1 , 0x2, __VA_ARGS__)\

#define		TM_RCB_ID(operation, ...) \
    TM_REG_ROW_##operation( SUID, 8, 8 , 0x3, __VA_ARGS__)\
    TM_REG_ROW_##operation( UID, 0, 8 , 0x133, __VA_ARGS__)\

#define		TM_RCB_ForceErr(operation, ...) \
    TM_REG_ROW_##operation( ForcedErr, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_RCB_PerfCntrsPortAlloc(operation, ...) \
    TM_REG_ROW_##operation( Set3Map, 24, 6 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Set2Map, 16, 6 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Set1Map, 8, 6 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Set0Map, 0, 6 , 0x0, __VA_ARGS__)\

#define		TM_RCB_PerfCntrsTimerPer(operation, ...) \
    TM_REG_ROW_##operation( Period, 0, 14 , 0x3fff, __VA_ARGS__)\

#define		TM_RCB_NonErrToSMSPktCnt(operation, ...) \
    TM_REG_ROW_##operation( Cnt, 0, 64 , 0x0, __VA_ARGS__)\

#define		TM_RCB_CRCErrPktCnt(operation, ...) \
    TM_REG_ROW_##operation( Cnt, 0, 64 , 0x0, __VA_ARGS__)\

#define		TM_RCB_SMSErrPasPktCnt(operation, ...) \
    TM_REG_ROW_##operation( Cnt, 0, 64 , 0x0, __VA_ARGS__)\

#define		TM_RCB_PortFlushDrpDataPktCnt(operation, ...) \
    TM_REG_ROW_##operation( Cnt, 0, 64 , 0x0, __VA_ARGS__)\

#define		TM_RCB_RCBTxFIFOBp(operation, ...) \
    TM_REG_ROW_##operation( SetThresh, 32, 5 , 0x19, __VA_ARGS__)\
    TM_REG_ROW_##operation( ClrThresh, 0, 5 , 0x18, __VA_ARGS__)\

#define		TM_RCB_ChunkRdDataDecoderFIFOBp(operation, ...) \
    TM_REG_ROW_##operation( SetThresh, 32, 6 , 0x18, __VA_ARGS__)\
    TM_REG_ROW_##operation( ClrThresh, 0, 6 , 0x17, __VA_ARGS__)\

#define		TM_RCB_Ecc_Config(operation, ...) \
    TM_REG_ROW_##operation( LockFirstErronousEvent, 0, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ErrInsMode, 1, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PortRd_ErrInsEnable, 2, 1 , 0x0, __VA_ARGS__)\

#define		TM_RCB_EccMemParams0(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_RCB_ROC_shadow(operation, ...) \
    TM_REG_ROW_##operation( Sample_Long_Regs_for_MG, 0, 64 , 0x0, __VA_ARGS__)\

#define		TM_RCB_Message_Packet_via_Sync_FIFO(operation, ...) \
    TM_REG_ROW_##operation( Message_Packet_via_Sync_FIFO, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_RCB_ExtECCErrDataLog(operation, ...) \
    TM_REG_ROW_##operation( Log, 0, 64 , 0x0, __VA_ARGS__)\

#define		TM_RCB_PerfCntrsSets(operation, ...) \
    TM_REG_ROW_##operation( PerfCntSet, 0, 64 , 0x0, __VA_ARGS__)\

/****************************************************************************************************/
#define		TM_Sched_GeneralEccErrStatus(operation, ...) \
    TM_REG_ROW_##operation( CorrEccErr, 16, 6 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( UncEccErr, 0, 6 , 0x0, __VA_ARGS__)\

#define		TM_Sched_ScrubDisable(operation, ...) \
    TM_REG_ROW_##operation( Dis, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_Sched_TreeRRDWRRStatus(operation, ...) \
    TM_REG_ROW_##operation( Status, 0, 64 , 0x1, __VA_ARGS__)\

#define		TM_Sched_PortPerRateShpPrmsInt(operation, ...) \
    TM_REG_ROW_##operation( I, 32, 3 , 0x1, __VA_ARGS__)\
    TM_REG_ROW_##operation( B, 0, 3 , 0x1, __VA_ARGS__)\

#define		TM_Sched_PortEccErrStatus(operation, ...) \
    TM_REG_ROW_##operation( CorrEccErr, 16, 2 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( UncEccErr, 0, 2 , 0x0, __VA_ARGS__)\

#define		TM_Sched_PortBankEccErrStatus(operation, ...) \
    TM_REG_ROW_##operation( CorrEccErr, 16, 2 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( UncEccErr, 0, 2 , 0x0, __VA_ARGS__)\

#define		TM_Sched_PortPerStatus(operation, ...) \
    TM_REG_ROW_##operation( PerPointer, 0, 6 , 0x0, __VA_ARGS__)\

#define		TM_Sched_ClvlPerRateShpPrmsInt(operation, ...) \
    TM_REG_ROW_##operation( I, 32, 3 , 0x1, __VA_ARGS__)\
    TM_REG_ROW_##operation( B, 0, 3 , 0x1, __VA_ARGS__)\

#define		TM_Sched_ClvlEccErrStatus(operation, ...) \
    TM_REG_ROW_##operation( CorrEccErr, 16, 5 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( UncEccErr, 0, 5 , 0x0, __VA_ARGS__)\

#define		TM_Sched_ClvlMyQEccErrStatus(operation, ...) \
    TM_REG_ROW_##operation( CorrEccErr, 16, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( UncEccErr, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_Sched_ClvlBankEccErrStatus(operation, ...) \
    TM_REG_ROW_##operation( CorrEccErr, 16, 4 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( UncEccErr, 0, 4 , 0x0, __VA_ARGS__)\

#define		TM_Sched_ClvlPerStatus(operation, ...) \
    TM_REG_ROW_##operation( PerPointer, 32, 9 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PerRoundCnt, 0, 10 , 0x0, __VA_ARGS__)\

#define		TM_Sched_BlvlPerRateShpPrmsInt(operation, ...) \
    TM_REG_ROW_##operation( I, 32, 3 , 0x1, __VA_ARGS__)\
    TM_REG_ROW_##operation( B, 0, 3 , 0x1, __VA_ARGS__)\

#define		TM_Sched_BlvlEccErrStatus(operation, ...) \
    TM_REG_ROW_##operation( CorrEccErr, 16, 6 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( UncEccErr, 0, 6 , 0x0, __VA_ARGS__)\

#define		TM_Sched_BlvlMyQEccErrStatus(operation, ...) \
    TM_REG_ROW_##operation( CorrEccErr, 16, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( UncEccErr, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_Sched_BlvlBankEccErrStatus(operation, ...) \
    TM_REG_ROW_##operation( CorrEccErr, 16, 5 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( UncEccErr, 0, 5 , 0x0, __VA_ARGS__)\

#define		TM_Sched_BlvlPerStatus(operation, ...) \
    TM_REG_ROW_##operation( PerPointer, 32, 11 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PerRoundCnt, 0, 10 , 0x0, __VA_ARGS__)\

#define		TM_Sched_AlvlPerRateShpPrmsInt(operation, ...) \
    TM_REG_ROW_##operation( I, 32, 3 , 0x1, __VA_ARGS__)\
    TM_REG_ROW_##operation( B, 0, 3 , 0x1, __VA_ARGS__)\

#define		TM_Sched_AlvlEccErrStatus(operation, ...) \
    TM_REG_ROW_##operation( CorrEccErr, 16, 7 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( UncEccErr, 0, 7 , 0x0, __VA_ARGS__)\

#define		TM_Sched_AlvlMyQEccErrStatus(operation, ...) \
    TM_REG_ROW_##operation( CorrEccErr, 16, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( UncEccErr, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_Sched_AlvlBankEccErrStatus(operation, ...) \
    TM_REG_ROW_##operation( CorrEccErr, 16, 5 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( UncEccErr, 0, 5 , 0x0, __VA_ARGS__)\

#define		TM_Sched_AlvlPerStatus(operation, ...) \
    TM_REG_ROW_##operation( PerPointer, 32, 11 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PerRoundCnt, 0, 10 , 0x0, __VA_ARGS__)\

#define		TM_Sched_QueueEccErrStatus(operation, ...) \
    TM_REG_ROW_##operation( CorrEccErr, 16, 7 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( UncEccErr, 0, 7 , 0x0, __VA_ARGS__)\

#define		TM_Sched_QueuePerRateShpPrmsInt(operation, ...) \
    TM_REG_ROW_##operation( I, 32, 3 , 0x1, __VA_ARGS__)\
    TM_REG_ROW_##operation( B, 0, 3 , 0x1, __VA_ARGS__)\

#define		TM_Sched_QueueBank0EccErrStatus(operation, ...) \
    TM_REG_ROW_##operation( CorrEccErr, 16, 5 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( UncEccErr, 0, 5 , 0x0, __VA_ARGS__)\

#define		TM_Sched_QueueBank1EccErrStatus(operation, ...) \
    TM_REG_ROW_##operation( CorrEccErr, 16, 5 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( UncEccErr, 0, 5 , 0x0, __VA_ARGS__)\

#define		TM_Sched_QueueBank2EccErrStatus(operation, ...) \
    TM_REG_ROW_##operation( CorrEccErr, 16, 5 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( UncEccErr, 0, 5 , 0x0, __VA_ARGS__)\

#define		TM_Sched_QueueBank3EccErrStatus(operation, ...) \
    TM_REG_ROW_##operation( CorrEccErr, 16, 5 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( UncEccErr, 0, 5 , 0x0, __VA_ARGS__)\

#define		TM_Sched_ErrStus(operation, ...) \
    TM_REG_ROW_##operation( FIFOOvrflowErr, 5, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( TBNegSat, 4, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BPBSat, 3, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( UncECCErr, 2, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( CorrECCErr, 1, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ForcedErr, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_Sched_FirstExc(operation, ...) \
    TM_REG_ROW_##operation( FIFOOvrflowErr, 5, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( TBNegSat, 4, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BPBSat, 3, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( UncECCErr, 2, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( CorrECCErr, 1, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ForcedErr, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_Sched_ErrCnt(operation, ...) \
    TM_REG_ROW_##operation( Cnt, 0, 16 , 0x0, __VA_ARGS__)\

#define		TM_Sched_ExcCnt(operation, ...) \
    TM_REG_ROW_##operation( Cnt, 0, 16 , 0x0, __VA_ARGS__)\

#define		TM_Sched_ExcMask(operation, ...) \
    TM_REG_ROW_##operation( FIFOOvrflowErr, 5, 1 , 0x1, __VA_ARGS__)\
    TM_REG_ROW_##operation( TBNegSat, 4, 1 , 0x3, __VA_ARGS__)\
    TM_REG_ROW_##operation( BPBSat, 3, 1 , 0x3, __VA_ARGS__)\
    TM_REG_ROW_##operation( UncECCErr, 2, 1 , 0x3, __VA_ARGS__)\
    TM_REG_ROW_##operation( CorrECCErr, 1, 1 , 0x3, __VA_ARGS__)\
    TM_REG_ROW_##operation( ForcedErr, 0, 1 , 0x3, __VA_ARGS__)\

#define		TM_Sched_Id(operation, ...) \
    TM_REG_ROW_##operation( SUID, 8, 8 , 0x10, __VA_ARGS__)\
    TM_REG_ROW_##operation( UID, 0, 8 , 0x33, __VA_ARGS__)\

#define		TM_Sched_ForceErr(operation, ...) \
    TM_REG_ROW_##operation( ForcedErr, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_Sched_ScrubSlotAlloc(operation, ...) \
    TM_REG_ROW_##operation( PortSlots, 32, 6 , 0x4, __VA_ARGS__)\
    TM_REG_ROW_##operation( ClvlSlots, 24, 6 , 0x4, __VA_ARGS__)\
    TM_REG_ROW_##operation( BlvlSlots, 16, 6 , 0x8, __VA_ARGS__)\
    TM_REG_ROW_##operation( AlvlSlots, 8, 6 , 0x10, __VA_ARGS__)\
    TM_REG_ROW_##operation( QueueSlots, 0, 6 , 0x20, __VA_ARGS__)\

#define		TM_Sched_TMtoTMBpFIFOBp(operation, ...) \
    TM_REG_ROW_##operation( SetThresh, 32, 5 , 0x8, __VA_ARGS__)\
    TM_REG_ROW_##operation( ClrThresh, 0, 5 , 0x7, __VA_ARGS__)\

#define		TM_Sched_TreeDeqEn(operation, ...) \
    TM_REG_ROW_##operation( En, 0, 1 , 0x1, __VA_ARGS__)\

#define		TM_Sched_TreeDWRRPrioEn(operation, ...) \
    TM_REG_ROW_##operation( PrioEn, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_PortPerConf(operation, ...) \
    TM_REG_ROW_##operation( DecEn, 48, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( TokenResExp, 32, 4 , 0x2, __VA_ARGS__)\
    TM_REG_ROW_##operation( PerInterval, 16, 8 , 0x50, __VA_ARGS__)\
    TM_REG_ROW_##operation( PerEn, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_Sched_PortPerRateShpPrms(operation, ...) \
    TM_REG_ROW_##operation( L, 32, 14 , 0x16e, __VA_ARGS__)\
    TM_REG_ROW_##operation( K, 16, 14 , 0x66f, __VA_ARGS__)\
    TM_REG_ROW_##operation( N, 0, 14 , 0x2, __VA_ARGS__)\

#define		TM_Sched_PortExtBPEn(operation, ...) \
    TM_REG_ROW_##operation( En, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_Sched_PortDWRRBytesPerBurstsLimit(operation, ...) \
    TM_REG_ROW_##operation( limit, 0, 7 , 0x20, __VA_ARGS__)\

#define		TM_Sched_ClvlPerConf(operation, ...) \
    TM_REG_ROW_##operation( DecEn, 48, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( TokenResExp, 32, 4 , 0x2, __VA_ARGS__)\
    TM_REG_ROW_##operation( PerInterval, 16, 12 , 0x271, __VA_ARGS__)\
    TM_REG_ROW_##operation( PerEn, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_Sched_ClvlPerRateShpPrms(operation, ...) \
    TM_REG_ROW_##operation( L, 32, 14 , 0x16e, __VA_ARGS__)\
    TM_REG_ROW_##operation( K, 16, 14 , 0x66f, __VA_ARGS__)\
    TM_REG_ROW_##operation( N, 0, 14 , 0x2, __VA_ARGS__)\

#define		TM_Sched_BlvlPerConf(operation, ...) \
    TM_REG_ROW_##operation( DecEn, 48, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( TokenResExp, 32, 4 , 0x2, __VA_ARGS__)\
    TM_REG_ROW_##operation( PerInterval, 16, 12 , 0x9c4, __VA_ARGS__)\
    TM_REG_ROW_##operation( PerEn, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_Sched_BlvlPerRateShpPrms(operation, ...) \
    TM_REG_ROW_##operation( L, 32, 14 , 0x16e, __VA_ARGS__)\
    TM_REG_ROW_##operation( K, 16, 14 , 0x66f, __VA_ARGS__)\
    TM_REG_ROW_##operation( N, 0, 14 , 0x2, __VA_ARGS__)\

#define		TM_Sched_AlvlPerConf(operation, ...) \
    TM_REG_ROW_##operation( DecEn, 48, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( TokenResExp, 32, 4 , 0x2, __VA_ARGS__)\
    TM_REG_ROW_##operation( PerInterval, 16, 12 , 0x9c4, __VA_ARGS__)\
    TM_REG_ROW_##operation( PerEn, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_Sched_AlvlPerRateShpPrms(operation, ...) \
    TM_REG_ROW_##operation( L, 32, 14 , 0x16e, __VA_ARGS__)\
    TM_REG_ROW_##operation( K, 16, 14 , 0x66f, __VA_ARGS__)\
    TM_REG_ROW_##operation( N, 0, 14 , 0x2, __VA_ARGS__)\

#define		TM_Sched_QueuePerConf(operation, ...) \
    TM_REG_ROW_##operation( DecEn, 48, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( TokenResExp, 32, 4 , 0x2, __VA_ARGS__)\
    TM_REG_ROW_##operation( PerInterval, 16, 12 , 0x9c4, __VA_ARGS__)\
    TM_REG_ROW_##operation( PerEn, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_Sched_QueuePerRateShpPrms(operation, ...) \
    TM_REG_ROW_##operation( L, 32, 14 , 0x16e, __VA_ARGS__)\
    TM_REG_ROW_##operation( K, 16, 14 , 0x66f, __VA_ARGS__)\
    TM_REG_ROW_##operation( N, 0, 14 , 0x2, __VA_ARGS__)\

#define		TM_Sched_EccConfig(operation, ...) \
    TM_REG_ROW_##operation( ADWRRErrInsEnable, 34, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( LockFirstErronousEvent, 0, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ErrInsMode, 1, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QtoAErrInsEnable, 2, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( AIDErrInsEnable, 3, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( CPerConfErrInsEnable, 4, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BPerConfErrInsEnable, 5, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( CTBErrInsEnable, 6, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BTBErrInsEnable, 7, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( CTBNegErrInsEnable, 8, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( CTBNeg2ErrInsEnable, 9, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BTBNegErrInsEnable, 10, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BTBNeg2ErrInsEnable, 11, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ATBNegErrInsEnable, 12, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ATBNeg2ErrInsEnable, 13, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QTBNegErrInsEnable, 14, 4 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QTBNeg2ErrInsEnable, 18, 4 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QWFSErrInsEnable, 22, 4 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( AWFSErrInsEnable, 26, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BWFSErrInsEnable, 27, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( CMyQErrInsEnable, 28, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BMyQErrInsEnable, 29, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( AMyQErrInsEnable, 30, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QFuncErrInsEnable, 42, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( CLastErrInsEnable, 38, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BLastErrInsEnable, 39, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ALastErrInsEnable, 40, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QLastErrInsEnable, 41, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BGrandParentErrInsEnable, 45, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( AGrandParentErrInsEnable, 46, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QGrandParentErrInsEnable, 47, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( CParentErrInsEnable, 35, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BParentErrInsEnable, 36, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( AParentErrInsEnable, 37, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( AStateErrInsEnable, 43, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QStateErrInsEnable, 44, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PDWRRErrInsEnable, 31, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( CDWRRErrInsEnable, 32, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BDWRRErrInsEnable, 33, 1 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams0(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams1(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams2(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams3(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams4(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams5(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams6(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams7(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams8(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams9(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams10(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams11(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams12(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams13(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams14(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams15(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams16(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams17(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams18(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams19(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams20(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams21(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams22(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams23(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams24(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams25(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams26(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams27(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams28(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams29(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams30(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams31(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams32(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams33(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams34(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams35(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams36(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams37(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams38(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams39(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams40(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams41(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams42(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams43(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams44(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_EccMemParams45(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_PortNodeState(operation, ...) \
    TM_REG_ROW_##operation( State, 0, 18 , 0x1fe, __VA_ARGS__)\

#define		TM_Sched_PortMyQ(operation, ...) \
    TM_REG_ROW_##operation( Status, 0, 40 , 0x0, __VA_ARGS__)\

#define		TM_Sched_PortRRDWRRStatusLSB(operation, ...) \
    TM_REG_ROW_##operation( Status, 0, 36 , 0x1f, __VA_ARGS__)\

#define		TM_Sched_PortRRDWRRStatusMSB(operation, ...) \
    TM_REG_ROW_##operation( Status, 0, 36 , 0x1f, __VA_ARGS__)\

#define		TM_Sched_PortWFS(operation, ...) \
    TM_REG_ROW_##operation( WFS, 0, 32 , 0x0, __VA_ARGS__)\

#define		TM_Sched_PortBPFromSTF(operation, ...) \
    TM_REG_ROW_##operation( BP, 0, 64 , 0x0, __VA_ARGS__)\

#define		TM_Sched_PortBPFromQMgr(operation, ...) \
    TM_REG_ROW_##operation( BP, 0, 64 , 0x0, __VA_ARGS__)\

#define		TM_Sched_ClvlNodeState(operation, ...) \
    TM_REG_ROW_##operation( State, 0, 11 , 0x2, __VA_ARGS__)\

#define		TM_Sched_ClvlMyQ(operation, ...) \
    TM_REG_ROW_##operation( Status, 0, 34 , 0x0, __VA_ARGS__)\

#define		TM_Sched_ClvlRRDWRRStatusLSB(operation, ...) \
    TM_REG_ROW_##operation( Status, 0, 44 , 0x1fff, __VA_ARGS__)\

#define		TM_Sched_ClvlRRDWRRStatusMSB(operation, ...) \
    TM_REG_ROW_##operation( Status, 0, 44 , 0x1fff, __VA_ARGS__)\

#define		TM_Sched_ClvlWFS(operation, ...) \
    TM_REG_ROW_##operation( WFS, 0, 32 , 0x0, __VA_ARGS__)\

#define		TM_Sched_ClvlL0ClusterState(operation, ...) \
    TM_REG_ROW_##operation( Status, 0, 64 , 0x0, __VA_ARGS__)\

#define		TM_Sched_ClvlBPFromSTF(operation, ...) \
    TM_REG_ROW_##operation( BP, 0, 64 , 0x0, __VA_ARGS__)\

#define		TM_Sched_TMtoTMClvlBPState(operation, ...) \
    TM_REG_ROW_##operation( BPState, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_Sched_BlvlNodeState(operation, ...) \
    TM_REG_ROW_##operation( State, 0, 11 , 0x2, __VA_ARGS__)\

#define		TM_Sched_BlvlMyQ(operation, ...) \
    TM_REG_ROW_##operation( Status, 0, 28 , 0x0, __VA_ARGS__)\

#define		TM_Sched_BlvlRRDWRRStatusLSB(operation, ...) \
    TM_REG_ROW_##operation( Status, 0, 48 , 0x1ffff, __VA_ARGS__)\

#define		TM_Sched_BlvlRRDWRRStatusMSB(operation, ...) \
    TM_REG_ROW_##operation( Status, 0, 48 , 0x1ffff, __VA_ARGS__)\

#define		TM_Sched_BlvlWFS(operation, ...) \
    TM_REG_ROW_##operation( WFS, 0, 32 , 0x0, __VA_ARGS__)\

#define		TM_Sched_BlvlL0ClusterState(operation, ...) \
    TM_REG_ROW_##operation( Status, 0, 64 , 0x0, __VA_ARGS__)\

#define		TM_Sched_BlvlL1ClusterStateLo(operation, ...) \
    TM_REG_ROW_##operation( Status, 0, 40 , 0x0, __VA_ARGS__)\

#define		TM_Sched_BlvlL1ClusterStateHi(operation, ...) \
    TM_REG_ROW_##operation( Status, 0, 42 , 0x0, __VA_ARGS__)\

#define		TM_Sched_AlvlNodeState(operation, ...) \
    TM_REG_ROW_##operation( State, 0, 11 , 0x2, __VA_ARGS__)\

#define		TM_Sched_AlvlMyQ(operation, ...) \
    TM_REG_ROW_##operation( Status, 0, 22 , 0x0, __VA_ARGS__)\

#define		TM_Sched_AlvlRRDWRRStatusLSB(operation, ...) \
    TM_REG_ROW_##operation( Status, 0, 56 , 0x1ffffff, __VA_ARGS__)\

#define		TM_Sched_AlvlRRDWRRStatusMSB(operation, ...) \
    TM_REG_ROW_##operation( Status, 0, 56 , 0x1ffffff, __VA_ARGS__)\

#define		TM_Sched_AlvlWFS(operation, ...) \
    TM_REG_ROW_##operation( WFS, 0, 32 , 0x0, __VA_ARGS__)\

#define		TM_Sched_AlvlL0ClusterState(operation, ...) \
    TM_REG_ROW_##operation( Status, 0, 60 , 0x40000, __VA_ARGS__)\

#define		TM_Sched_AlvlL1ClusterStateLo(operation, ...) \
    TM_REG_ROW_##operation( Status, 0, 36 , 0x0, __VA_ARGS__)\

#define		TM_Sched_AlvlL1ClusterStateHi(operation, ...) \
    TM_REG_ROW_##operation( Status, 0, 36 , 0x0, __VA_ARGS__)\

#define		TM_Sched_AlvlL2ClusterStateLo(operation, ...) \
    TM_REG_ROW_##operation( Status, 0, 44 , 0x0, __VA_ARGS__)\

#define		TM_Sched_AlvlL2ClusterStateHi(operation, ...) \
    TM_REG_ROW_##operation( Status, 0, 44 , 0x0, __VA_ARGS__)\

#define		TM_Sched_QueueNodeState(operation, ...) \
    TM_REG_ROW_##operation( State, 0, 11 , 0x2, __VA_ARGS__)\

#define		TM_Sched_QueueWFS(operation, ...) \
    TM_REG_ROW_##operation( WFS, 0, 32 , 0x0, __VA_ARGS__)\

#define		TM_Sched_QueueL0ClusterState(operation, ...) \
    TM_REG_ROW_##operation( Status, 0, 64 , 0x0, __VA_ARGS__)\

#define		TM_Sched_QueueL1ClusterStateLo(operation, ...) \
    TM_REG_ROW_##operation( Status, 0, 40 , 0x0, __VA_ARGS__)\

#define		TM_Sched_QueueL1ClusterStateHi(operation, ...) \
    TM_REG_ROW_##operation( Status, 0, 40 , 0x0, __VA_ARGS__)\

#define		TM_Sched_QueueL2ClusterStateLo(operation, ...) \
    TM_REG_ROW_##operation( Status, 0, 48 , 0x0, __VA_ARGS__)\

#define		TM_Sched_QueueL2ClusterStateHi(operation, ...) \
    TM_REG_ROW_##operation( Status, 0, 48 , 0x0, __VA_ARGS__)\

#define		TM_Sched_TMtoTMQueueBPState(operation, ...) \
    TM_REG_ROW_##operation( BPState, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_Sched_QueuePerStatus(operation, ...) \
    TM_REG_ROW_##operation( PerPointer, 32, 11 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PerRoundCnt, 0, 10 , 0x0, __VA_ARGS__)\

#define		TM_Sched_PortEligPrioFunc_Entry(operation, ...) \
    TM_REG_ROW_##operation( FuncOut3, 48, 9 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( FuncOut2, 32, 9 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( FuncOut1, 16, 9 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( FuncOut0, 0, 9 , 0x0, __VA_ARGS__)\

#define		TM_Sched_PortEligPrioFuncPtr(operation, ...) \
    TM_REG_ROW_##operation( Ptr, 0, 6 , 0x0, __VA_ARGS__)\

#define		TM_Sched_PortTokenBucketTokenEnDiv(operation, ...) \
    TM_REG_ROW_##operation( PerEn, 48, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Periods, 32, 13 , 0x1, __VA_ARGS__)\
    TM_REG_ROW_##operation( MaxToken, 16, 12 , 0x7ff, __VA_ARGS__)\
    TM_REG_ROW_##operation( MinToken, 0, 12 , 0x7ff, __VA_ARGS__)\

#define		TM_Sched_PortTokenBucketBurstSize(operation, ...) \
    TM_REG_ROW_##operation( MaxBurstSz, 32, 17 , 0x1ffff, __VA_ARGS__)\
    TM_REG_ROW_##operation( MinBurstSz, 0, 17 , 0x1ffff, __VA_ARGS__)\

#define		TM_Sched_PortDWRRPrioEn(operation, ...) \
    TM_REG_ROW_##operation( En, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_PortQuantumsPriosLo(operation, ...) \
    TM_REG_ROW_##operation( Quantum3, 48, 9 , 0x10, __VA_ARGS__)\
    TM_REG_ROW_##operation( Quantum2, 32, 9 , 0x10, __VA_ARGS__)\
    TM_REG_ROW_##operation( Quantum1, 16, 9 , 0x10, __VA_ARGS__)\
    TM_REG_ROW_##operation( Quantum0, 0, 9 , 0x10, __VA_ARGS__)\

#define		TM_Sched_PortQuantumsPriosHi(operation, ...) \
    TM_REG_ROW_##operation( Quantum7, 48, 9 , 0x10, __VA_ARGS__)\
    TM_REG_ROW_##operation( Quantum6, 32, 9 , 0x10, __VA_ARGS__)\
    TM_REG_ROW_##operation( Quantum5, 16, 9 , 0x10, __VA_ARGS__)\
    TM_REG_ROW_##operation( Quantum4, 0, 9 , 0x10, __VA_ARGS__)\

#define		TM_Sched_PortRangeMap(operation, ...) \
    TM_REG_ROW_##operation( Hi, 16, 9 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Lo, 0, 9 , 0x0, __VA_ARGS__)\

#define		TM_Sched_ClvlEligPrioFunc_Entry(operation, ...) \
    TM_REG_ROW_##operation( FuncOut3, 48, 9 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( FuncOut2, 32, 9 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( FuncOut1, 16, 9 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( FuncOut0, 0, 9 , 0x0, __VA_ARGS__)\

#define		TM_Sched_ClvlEligPrioFuncPtr(operation, ...) \
    TM_REG_ROW_##operation( Ptr, 0, 6 , 0x0, __VA_ARGS__)\

#define		TM_Sched_ClvlTokenBucketTokenEnDiv(operation, ...) \
    TM_REG_ROW_##operation( PerEn, 48, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( MaxDivExp, 40, 3 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( MinDivExp, 32, 3 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( MaxToken, 16, 12 , 0x7ff, __VA_ARGS__)\
    TM_REG_ROW_##operation( MinToken, 0, 12 , 0x7ff, __VA_ARGS__)\

#define		TM_Sched_ClvlTokenBucketBurstSize(operation, ...) \
    TM_REG_ROW_##operation( MaxBurstSz, 32, 12 , 0xfff, __VA_ARGS__)\
    TM_REG_ROW_##operation( MinBurstSz, 0, 12 , 0xfff, __VA_ARGS__)\

#define		TM_Sched_ClvlDWRRPrioEn(operation, ...) \
    TM_REG_ROW_##operation( En, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_ClvlQuantum(operation, ...) \
    TM_REG_ROW_##operation( Quantum, 0, 14 , 0x40, __VA_ARGS__)\

#define		TM_Sched_ClvltoPortAndBlvlRangeMap(operation, ...) \
    TM_REG_ROW_##operation( Port, 32, 8 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BlvlHi, 16, 11 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( BlvlLo, 0, 11 , 0x0, __VA_ARGS__)\

#define		TM_Sched_BlvlEligPrioFunc_Entry(operation, ...) \
    TM_REG_ROW_##operation( FuncOut3, 48, 9 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( FuncOut2, 32, 9 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( FuncOut1, 16, 9 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( FuncOut0, 0, 9 , 0x0, __VA_ARGS__)\

#define		TM_Sched_BlvlEligPrioFuncPtr(operation, ...) \
    TM_REG_ROW_##operation( Ptr, 0, 6 , 0x0, __VA_ARGS__)\

#define		TM_Sched_BlvlTokenBucketTokenEnDiv(operation, ...) \
    TM_REG_ROW_##operation( PerEn, 48, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( MaxDivExp, 40, 3 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( MinDivExp, 32, 3 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( MaxToken, 16, 12 , 0x7ff, __VA_ARGS__)\
    TM_REG_ROW_##operation( MinToken, 0, 12 , 0x7ff, __VA_ARGS__)\

#define		TM_Sched_BlvlTokenBucketBurstSize(operation, ...) \
    TM_REG_ROW_##operation( MaxBurstSz, 32, 12 , 0xfff, __VA_ARGS__)\
    TM_REG_ROW_##operation( MinBurstSz, 0, 12 , 0xfff, __VA_ARGS__)\

#define		TM_Sched_BlvlDWRRPrioEn(operation, ...) \
    TM_REG_ROW_##operation( En, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_BlvlQuantum(operation, ...) \
    TM_REG_ROW_##operation( Quantum, 0, 14 , 0x40, __VA_ARGS__)\

#define		TM_Sched_BLvltoClvlAndAlvlRangeMap(operation, ...) \
    TM_REG_ROW_##operation( Clvl, 32, 9 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( AlvlHi, 16, 12 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( AlvlLo, 0, 12 , 0x0, __VA_ARGS__)\

#define		TM_Sched_AlvlEligPrioFunc_Entry(operation, ...) \
    TM_REG_ROW_##operation( FuncOut3, 48, 9 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( FuncOut2, 32, 9 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( FuncOut1, 16, 9 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( FuncOut0, 0, 9 , 0x0, __VA_ARGS__)\

#define		TM_Sched_AlvlEligPrioFuncPtr(operation, ...) \
    TM_REG_ROW_##operation( Ptr, 0, 6 , 0x0, __VA_ARGS__)\

#define		TM_Sched_AlvlTokenBucketTokenEnDiv(operation, ...) \
    TM_REG_ROW_##operation( PerEn, 48, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( MaxDivExp, 40, 3 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( MinDivExp, 32, 3 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( MaxToken, 16, 12 , 0x7ff, __VA_ARGS__)\
    TM_REG_ROW_##operation( MinToken, 0, 12 , 0x7ff, __VA_ARGS__)\

#define		TM_Sched_AlvlTokenBucketBurstSize(operation, ...) \
    TM_REG_ROW_##operation( MaxBurstSz, 32, 12 , 0xfff, __VA_ARGS__)\
    TM_REG_ROW_##operation( MinBurstSz, 0, 12 , 0xfff, __VA_ARGS__)\

#define		TM_Sched_AlvlDWRRPrioEn(operation, ...) \
    TM_REG_ROW_##operation( En, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_Sched_AlvlQuantum(operation, ...) \
    TM_REG_ROW_##operation( Quantum, 0, 14 , 0x40, __VA_ARGS__)\

#define		TM_Sched_ALvltoBlvlAndQueueRangeMap(operation, ...) \
    TM_REG_ROW_##operation( Blvl, 32, 11 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QueueHi, 16, 14 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QueueLo, 0, 14 , 0x0, __VA_ARGS__)\

#define		TM_Sched_QueueEligPrioFunc(operation, ...) \
    TM_REG_ROW_##operation( FuncOut3, 48, 9 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( FuncOut2, 32, 9 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( FuncOut1, 16, 9 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( FuncOut0, 0, 9 , 0x0, __VA_ARGS__)\

#define		TM_Sched_QueueEligPrioFuncPtr(operation, ...) \
    TM_REG_ROW_##operation( Ptr, 0, 6 , 0x0, __VA_ARGS__)\

#define		TM_Sched_QueueTokenBucketTokenEnDiv(operation, ...) \
    TM_REG_ROW_##operation( PerEn, 48, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( MaxDivExp, 40, 3 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( MinDivExp, 32, 3 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( MaxToken, 16, 12 , 0x7ff, __VA_ARGS__)\
    TM_REG_ROW_##operation( MinToken, 0, 12 , 0x7ff, __VA_ARGS__)\

#define		TM_Sched_QueueTokenBucketBurstSize(operation, ...) \
    TM_REG_ROW_##operation( MaxBurstSz, 32, 12 , 0xfff, __VA_ARGS__)\
    TM_REG_ROW_##operation( MinBurstSz, 0, 12 , 0xfff, __VA_ARGS__)\

#define		TM_Sched_QueueQuantum(operation, ...) \
    TM_REG_ROW_##operation( Quantum, 0, 14 , 0x40, __VA_ARGS__)\

#define		TM_Sched_QueueAMap(operation, ...) \
    TM_REG_ROW_##operation( Alvl, 0, 12 , 0x0, __VA_ARGS__)\

#define		TM_Sched_PortShpBucketLvls(operation, ...) \
    TM_REG_ROW_##operation( MaxLvl, 32, 29 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( MinLvl, 0, 29 , 0x0, __VA_ARGS__)\

#define		TM_Sched_PortDefPrioHi(operation, ...) \
    TM_REG_ROW_##operation( Deficit3, 48, 16 , 0x1, __VA_ARGS__)\
    TM_REG_ROW_##operation( Deficit2, 32, 16 , 0x10001, __VA_ARGS__)\
    TM_REG_ROW_##operation( Deficit1, 16, 16 , 0x10001, __VA_ARGS__)\
    TM_REG_ROW_##operation( Deficit0, 0, 16 , 0x10001, __VA_ARGS__)\

#define		TM_Sched_PortDefPrioLo(operation, ...) \
    TM_REG_ROW_##operation( Deficit3, 48, 16 , 0x1, __VA_ARGS__)\
    TM_REG_ROW_##operation( Deficit2, 32, 16 , 0x10001, __VA_ARGS__)\
    TM_REG_ROW_##operation( Deficit1, 16, 16 , 0x10001, __VA_ARGS__)\
    TM_REG_ROW_##operation( Deficit0, 0, 16 , 0x10001, __VA_ARGS__)\

#define		TM_Sched_ClvlShpBucketLvls(operation, ...) \
    TM_REG_ROW_##operation( MaxLvl, 32, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( MinLvl, 0, 24 , 0x0, __VA_ARGS__)\

#define		TM_Sched_CLvlDef(operation, ...) \
    TM_REG_ROW_##operation( Deficit, 0, 23 , 0x1, __VA_ARGS__)\

#define		TM_Sched_BlvlShpBucketLvls(operation, ...) \
    TM_REG_ROW_##operation( MaxLvl, 32, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( MinLvl, 0, 24 , 0x0, __VA_ARGS__)\

#define		TM_Sched_BlvlDef(operation, ...) \
    TM_REG_ROW_##operation( Deficit, 0, 23 , 0x1, __VA_ARGS__)\

#define		TM_Sched_AlvlShpBucketLvls(operation, ...) \
    TM_REG_ROW_##operation( MaxLvl, 32, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( MinLvl, 0, 24 , 0x0, __VA_ARGS__)\

#define		TM_Sched_AlvlDef(operation, ...) \
    TM_REG_ROW_##operation( Deficit, 0, 23 , 0x1, __VA_ARGS__)\

#define		TM_Sched_QueueShpBucketLvls(operation, ...) \
    TM_REG_ROW_##operation( MaxLvl, 32, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( MinLvl, 0, 24 , 0x0, __VA_ARGS__)\

#define		TM_Sched_QueueDef(operation, ...) \
    TM_REG_ROW_##operation( Deficit, 0, 23 , 0x1, __VA_ARGS__)\

#define		TM_Sched_QueueDef(operation, ...) \
    TM_REG_ROW_##operation( Deficit, 0, 23 , 0x1, __VA_ARGS__)\
/****************************************************************************************************/
#define		TM_TM2TM_ErrStus(operation, ...) \
    TM_REG_ROW_##operation( ForcedErr, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_TM2TM_FirstExc(operation, ...) \
    TM_REG_ROW_##operation( ForcedErr, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_TM2TM_ErrCnt(operation, ...) \
    TM_REG_ROW_##operation( Cnt, 0, 16 , 0x0, __VA_ARGS__)\

#define		TM_TM2TM_ExcCnt(operation, ...) \
    TM_REG_ROW_##operation( Cnt, 0, 16 , 0x0, __VA_ARGS__)\

#define		TM_TM2TM_ExcMask(operation, ...) \
    TM_REG_ROW_##operation( ForcedErr, 0, 1 , 0x1, __VA_ARGS__)\

#define		TM_TM2TM_Id(operation, ...) \
    TM_REG_ROW_##operation( SUID, 8, 8 , 0x11, __VA_ARGS__)\
    TM_REG_ROW_##operation( UID, 0, 8 , 0x133, __VA_ARGS__)\

#define		TM_TM2TM_ForceErr(operation, ...) \
    TM_REG_ROW_##operation( ForcedErr, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_TM2TM_TMtoTMEn(operation, ...) \
    TM_REG_ROW_##operation( MsgQmrPck_alw_not_full, 9, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( IngUpdEn, 8, 1 , 0x1, __VA_ARGS__)\
    TM_REG_ROW_##operation( En, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_TM2TM_TMtoTMPktCRCEn(operation, ...) \
    TM_REG_ROW_##operation( En, 0, 1 , 0x1, __VA_ARGS__)\

#define		TM_TM2TM_QmapHeaderCont(operation, ...) \
    TM_REG_ROW_##operation( QmapH, 0, 64 , 0x1, __VA_ARGS__)\

#define		TM_TM2TM_QmapHeaderEn(operation, ...) \
    TM_REG_ROW_##operation( En, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_TM2TM_ExtHeadersSize(operation, ...) \
    TM_REG_ROW_##operation( Size, 0, 5 , 0x7, __VA_ARGS__)\

#define		TM_TM2TM_TMHeader(operation, ...) \
    TM_REG_ROW_##operation( Header, 0, 8 , 0x1, __VA_ARGS__)\

#define		TM_TM2TM_ContHeader(operation, ...) \
    TM_REG_ROW_##operation( LnCard, 0, 7 , 0x0, __VA_ARGS__)\

#define		TM_TM2TM_TMtoTMPktStructure(operation, ...) \
    TM_REG_ROW_##operation( Ports, 8, 5 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Nodes, 0, 5 , 0x10, __VA_ARGS__)\

#define		TM_TM2TM_Delta2PrioMap(operation, ...) \
    TM_REG_ROW_##operation( UpperRange2, 16, 6 , 0xb, __VA_ARGS__)\
    TM_REG_ROW_##operation( UpperRange1, 8, 6 , 0x7, __VA_ARGS__)\
    TM_REG_ROW_##operation( UpperRange0, 0, 6 , 0x3, __VA_ARGS__)\

#define		TM_TM2TM_TMPort(operation, ...) \
    TM_REG_ROW_##operation( Port, 0, 8 , 0x37, __VA_ARGS__)\

#define		TM_TM2TM_RemoteDPMode(operation, ...) \
    TM_REG_ROW_##operation( PortMode, 8, 2 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( NodeMode, 0, 2 , 0x0, __VA_ARGS__)\

#define		TM_TM2TM_TMtoTMEgTMLevelMap(operation, ...) \
    TM_REG_ROW_##operation( Port, 8, 3 , 0x4, __VA_ARGS__)\
    TM_REG_ROW_##operation( Node, 0, 3 , 0x0, __VA_ARGS__)\

#define		TM_TM2TM_TMtoTMElementsPerEgTMLevel(operation, ...) \
    TM_REG_ROW_##operation( Ports, 16, 8 , 0x30, __VA_ARGS__)\
    TM_REG_ROW_##operation( Nodes, 0, 14 , 0x0, __VA_ARGS__)\

#define		TM_TM2TM_TM2TMGenModePerLevel(operation, ...) \
    TM_REG_ROW_##operation( PortMode, 8, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( NodeMode, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_TM2TM_TMtoTMBaseOffsetPerIngTMLevel(operation, ...) \
    TM_REG_ROW_##operation( PortBA, 16, 16 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( NodeBA, 0, 16 , 0x0, __VA_ARGS__)\

#define		TM_TM2TM_TMtoTMEgTMBPLevelMap(operation, ...) \
    TM_REG_ROW_##operation( Port, 8, 1 , 0x1, __VA_ARGS__)\
    TM_REG_ROW_##operation( Node, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_TM2TM_TM2TMXonXoffThresh(operation, ...) \
    TM_REG_ROW_##operation( PortXon, 24, 6 , 0x4, __VA_ARGS__)\
    TM_REG_ROW_##operation( PortXoff, 16, 6 , 0xc, __VA_ARGS__)\
    TM_REG_ROW_##operation( NodeXon, 8, 6 , 0x4, __VA_ARGS__)\
    TM_REG_ROW_##operation( NodeXoff, 0, 6 , 0xc, __VA_ARGS__)\

#define		TM_TM2TM_RespRemoteDPSel(operation, ...) \
    TM_REG_ROW_##operation( DPSel, 0, 1 , 0x0, __VA_ARGS__)\

#define		TM_TM2TM_EccConfig(operation, ...) \
    TM_REG_ROW_##operation( LockFirstErronousEvent, 0, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( ErrInsMode, 1, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QLvlDPErrInsEnable, 2, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QLvlStateErrInsEnable, 3, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( QLvlL0ErrInsEnable, 4, 1 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( PLvlDPErrInsEnable, 5, 1 , 0x0, __VA_ARGS__)\

#define		TM_TM2TM_EccMemParams0(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_TM2TM_EccMemParams1(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_TM2TM_EccMemParams2(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_TM2TM_EccMemParams3(operation, ...) \
    TM_REG_ROW_##operation( Address, 8, 24 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Syndrom, 32, 32 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( Counter, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_TM2TM_ExtHeadersContent(operation, ...) \
    TM_REG_ROW_##operation( Headers, 0, 64 , 0x0, __VA_ARGS__)\

#define		TM_TM2TM_PortDropProb(operation, ...) \
    TM_REG_ROW_##operation( DropProb3, 24, 8 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( DropProb2, 16, 8 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( DropProb1, 8, 8 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( DropProb0, 0, 8 , 0x0, __VA_ARGS__)\

#define		TM_TM2TM_NodeDropProb(operation, ...) \
    TM_REG_ROW_##operation( DropProb7, 56, 8 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( DropProb6, 48, 8 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( DropProb5, 40, 8 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( DropProb4, 32, 8 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( DropProb3, 24, 8 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( DropProb2, 16, 8 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( DropProb1, 8, 8 , 0x0, __VA_ARGS__)\
    TM_REG_ROW_##operation( DropProb0, 0, 8 , 0x0, __VA_ARGS__)\

/* manually  added  */
#define		TM_BAP_ErrCnt(operation, ...) \
    TM_REG_ROW_##operation( Cnt, 0, 16 , 0x0, __VA_ARGS__)\

#define		TM_BAP_ExcCnt(operation, ...) \
    TM_REG_ROW_##operation( Cnt, 0, 16 , 0x0, __VA_ARGS__)\



#endif
