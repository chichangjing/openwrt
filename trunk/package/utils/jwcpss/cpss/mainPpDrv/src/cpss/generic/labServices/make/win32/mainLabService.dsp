# Microsoft Developer Studio Project File - Name="mainLabService" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=mainLabService - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mainLabService.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mainLabService.mak" CFG="mainLabService - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mainLabService - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mainLabService - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/SW/LabSystem/mainLabServices/make/win32", BHRJAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mainLabService - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
LINK32=link.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "mainLabService - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
LINK32=link.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\..\mainCommon\h" /I "..\..\..\mainOs\h" /I "..\h" /I "..\..\..\mainLabServices\h" /I "..\..\..\mainLabServices\h\port" /I "..\..\..\mainCmd\h" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /D "HWS_DEBUG" /D "AP_SHOW" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "mainLabService - Win32 Release"
# Name "mainLabService - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\common\configElementDb\mvCfgSeqDbIf.c
# End Source File
# Begin Source File

SOURCE=..\..\src\common\configElementDb\mvCfgSeqExecuter.c
# End Source File
# Begin Source File

SOURCE=..\..\src\common\coCpu\mvHwsPortApCoCpuIf.c
# End Source File
# Begin Source File

SOURCE=..\..\src\common\siliconIf\mvHwsSiliconIf.c
# End Source File
# End Group
# Begin Group "hwServices"

# PROP Default_Filter ""
# Begin Group "pcs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\port\pcs\cgPcs\mvHwsCgPcsDb.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\pcs\cgPcs\mvHwsCgPcsIf.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\pcs\gPcs\mvHwsGPcsDb.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\pcs\gPcs\mvHwsGPcsIf.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\pcs\hglPcs\mvHwsHglPcsDb.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\pcs\hglPcs\mvHwsHglPcsIf.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\pcs\ilknPcs\mvHwsIlknPcsDb.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\pcs\ilknPcs\mvHwsIlknPcsIf.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\pcs\mmPcs\mvHwsMMPcsDb.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\pcs\mmPcs\mvHwsMMPcsDb_v3.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\pcs\mmPcs\mvHwsMMPcsIf.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\pcs\mmPcs\mvHwsMMPcsIf_v2_0.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\pcs\mmPcs\mvHwsMMPcsIf_v3.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\pcs\mvHwsPcsIf.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\pcs\tcamPcs\mvHwsTcamPcsDb.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\pcs\tcamPcs\mvHwsTcamPcsIf.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\pcs\xPcs\mvHwsXPcsDb.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\pcs\xPcs\mvHwsXPcsIf.c
# End Source File
# End Group
# Begin Group "mac"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\port\mac\cgMac\mvHwsCgMacDb.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\mac\cgMac\mvHwsCgMacIf.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\mac\geMac\mvHwsGeMacDb.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\mac\geMac\mvHwsGeMacIf.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\mac\geMac\mvHwsGeMacSgIf.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\mac\hglMac\mvHwsHglMacDb.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\mac\hglMac\mvHwsHglMacIf.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\mac\hglMac\mvHwsHglMacRev2If.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\mac\ilknMac\mvHwsIlknMacDb.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\mac\ilknMac\mvHwsIlknMacIf.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\mac\mvHwsMacIf.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\mac\tcamMac\mvHwsTcamMacDb.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\mac\tcamMac\mvHwsTcamMacIf.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\mac\xgMac\mvHwsXgMacDb.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\mac\xgMac\mvHwsXgMacIf.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\mac\xgMac\mvHwsXgMacRev2If.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\mac\xlgMac\mvHwsXlgMacDb.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\mac\xlgMac\mvHwsXlgMacDbRev3.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\mac\xlgMac\mvHwsXlgMacIf.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\mac\xlgMac\mvHwsXlgMacRev2If.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\mac\xlgMac\mvHwsXlgMacRev3If.c
# End Source File
# End Group
# Begin Group "serdes"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\port\serdes\comPhyH\mvComPhyHDb_v1_0.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\serdes\comPhyHRev2\mvComPhyHDb_v2_0.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\serdes\comPhyH\mvComPhyHEom.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\serdes\comPhyH\mvComPhyHIf_v1_0.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\serdes\comPhyHRev2\mvComPhyHIf_v2_0.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\serdes\mvHwsSerdesIf.c
# End Source File
# End Group
# Begin Group "port"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\port\mvHwsPortApIf.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\mvHwsPortCfgIf.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\mvHwsPortInitIf.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\private\mvHwsPortMiscIf.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\private\mvPortModeElements.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\private\mvPortModeElementsLion2.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\private\mvPortModeElementsPuma3.c
# End Source File
# End Group
# Begin Group "silicon"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\port\silicon\mvHwsHooperIf.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\silicon\mvHwsLion2If.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\silicon\mvHwsPuma3If.c
# End Source File
# End Group
# Begin Group "apEngine"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\port\apEngine\mvHwsApEngine.c
# End Source File
# Begin Source File

SOURCE=..\..\src\port\apEngine\mvZ80Code.c
# End Source File
# End Group
# End Group
# Begin Group "ddr3"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\ddr3\private\mvDdr3SdramIf.c
# End Source File
# Begin Source File

SOURCE=..\..\src\ddr3\private\mvHwsDdr3Dqs.c
# End Source File
# Begin Source File

SOURCE=..\..\src\ddr3\mvHwsDdr3InitIf.c
# End Source File
# Begin Source File

SOURCE=..\..\src\ddr3\private\mvHwsDdr3Pbs.c
# End Source File
# Begin Source File

SOURCE=..\..\src\ddr3\private\mvHwsDdr3PrvIf.c
# End Source File
# Begin Source File

SOURCE=..\..\src\ddr3\private\mvHwsDdr3ReadLeveling.c
# End Source File
# Begin Source File

SOURCE=..\..\src\ddr3\private\mvHwsDdr3StaticCFg.c
# End Source File
# Begin Source File

SOURCE=..\..\src\ddr3\private\mvHwsDdr3TrainingIf.c
# End Source File
# End Group
# Begin Group "galtisAgent"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\galtisAgent\wrDDR3.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\galtisAgent\wrHwsMac.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\galtisAgent\wrHwsPcs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\galtisAgent\wrHwsPort.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\galtisAgent\wrHwsPortAp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\galtisAgent\wrHwsSerdes.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\src\generalAccess.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "common No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\h\common\configElementDb\mvCfgElementDb.h
# End Source File
# Begin Source File

SOURCE=..\..\h\common\siliconIf\mvSiliconIf.h
# End Source File
# End Group
# Begin Group "hwServices No. 1"

# PROP Default_Filter ""
# Begin Group "serdes No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\h\port\serdes\comPhyH\mvComPhyEomIf.h
# End Source File
# Begin Source File

SOURCE=..\..\h\port\serdes\comPhyH\mvComPhyHDb.h
# End Source File
# Begin Source File

SOURCE=..\..\h\port\serdes\comPhyH\mvComPhyHIf.h
# End Source File
# Begin Source File

SOURCE=..\..\h\port\serdes\comPhyHRev2\mvComPhyHRev2Db.h
# End Source File
# Begin Source File

SOURCE=..\..\h\port\serdes\comPhyHRev2\mvComPhyHRev2If.h
# End Source File
# Begin Source File

SOURCE=..\..\h\port\serdes\mvHwsSerdesPrvIf.h
# End Source File
# End Group
# Begin Group "pcs No. 1"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\h\port\pcs\mmPcs\mvHwsMMPcsDb.h
# End Source File
# Begin Source File

SOURCE=..\..\h\port\pcs\mmPcs\mvHwsMMPcsIf.h
# End Source File
# Begin Source File

SOURCE=..\..\h\port\pcs\mmPcs\mvHwsMMPcsV2If.h
# End Source File
# Begin Source File

SOURCE=..\..\h\port\pcs\mvHwsPcsIf.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\h\port\private\mvHwsPortApCoCpuIf.h
# End Source File
# Begin Source File

SOURCE=..\..\h\port\mvHwsPortApInitIf.h
# End Source File
# Begin Source File

SOURCE=..\..\h\port\private\mvHwsPortApInitIfPrv.h
# End Source File
# Begin Source File

SOURCE=..\..\h\port\mvHwsPortInitIf.h
# End Source File
# Begin Source File

SOURCE=..\..\h\port\private\mvHwsPortMiscIf.h
# End Source File
# Begin Source File

SOURCE=..\..\h\port\private\mvHwsPortPrvIf.h
# End Source File
# Begin Source File

SOURCE=..\..\h\port\private\mvHwsPortTypes.h
# End Source File
# Begin Source File

SOURCE=..\..\h\port\private\mvPortModeElements.h
# End Source File
# End Group
# End Group
# End Target
# End Project
