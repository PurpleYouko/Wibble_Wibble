# Microsoft Developer Studio Project File - Name="LIB_UTIL" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=LIB_UTIL - Win32 Compaq Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "LIB_Util.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "LIB_Util.mak" CFG="LIB_UTIL - Win32 Compaq Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "LIB_UTIL - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "LIB_UTIL - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "LIB_UTIL - Win32 Debug2" (based on "Win32 (x86) Static Library")
!MESSAGE "LIB_UTIL - Win32 Xeon Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "LIB_UTIL - Win32 Compaq Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/LIB_Util", BAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "LIB_UTIL - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "LIB_UTIL___Win32_Release"
# PROP BASE Intermediate_Dir "LIB_UTIL___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\Client\Util"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /ZI /I "..\Client\Util" /I "..\znzin11\3rdparty\lua-4.0.1\include" /I "..\Server\SHO_GS\SHO_GS_LIB\srv_COMMON" /I "..\Server\SHO_GS\SHO_GS_LIB\Common" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:".\VC_LIB\Util_r.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=copy libs
PostBuild_Cmds=copy               VC_LIB\*.lib               ..\Client\util\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LIB_UTIL - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "LIB_UTIL___Win32_Debug"
# PROP BASE Intermediate_Dir "LIB_UTIL___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\Client\util"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\Client\Network" /I "..\Client\Util" /I "..\znzin11\3rdparty\lua-4.0.1\include" /I "..\Server\SHO_GS\SHO_GS_LIB\srv_COMMON" /I "..\Server\SHO_GS\SHO_GS_LIB\Common" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:".\VC_LIB\Util_d.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=copy libs
PostBuild_Cmds=copy               VC_LIB\*.lib               ..\Client\util\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LIB_UTIL - Win32 Debug2"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "LIB_UTIL___Win32_Debug20"
# PROP BASE Intermediate_Dir "LIB_UTIL___Win32_Debug20"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug2"
# PROP Intermediate_Dir "Debug2"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\Client\Util" /I "..\Client\Network" /I "..\znzin11\3rdparty\lua-4.0.1\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\Client\Network" /I "..\Client\Util" /I "..\znzin11\3rdparty\lua-4.0.1\include" /I "..\Server\SHO_GS\SHO_GS_LIB\srv_COMMON" /I "..\Server\SHO_GS\SHO_GS_LIB\Common" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:".\VC_LIB\Util_d.lib"
# ADD LIB32 /nologo /out:".\VC_LIB\Util_d.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=copy libs
PostBuild_Cmds=copy               VC_LIB\*.lib               ..\Client\util\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LIB_UTIL - Win32 Xeon Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "LIB_UTIL___Win32_Xeon_Debug0"
# PROP BASE Intermediate_Dir "LIB_UTIL___Win32_Xeon_Debug0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Xeon_Debug"
# PROP Intermediate_Dir "Xeon_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\Client\Util" /I "..\Client\Network" /I "..\znzin11\3rdparty\lua-4.0.1\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\Client\Network" /I "..\Client\Util" /I "..\znzin11\3rdparty\lua-4.0.1\include" /I "..\Server\SHO_GS\SHO_GS_LIB\srv_COMMON" /I "..\Server\SHO_GS\SHO_GS_LIB\Common" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:".\VC_LIB\Util_d.lib"
# ADD LIB32 /nologo /out:".\VC_LIB\Util_d.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=copy libs
PostBuild_Cmds=copy               VC_LIB\*.lib               ..\Client\util\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "LIB_UTIL - Win32 Compaq Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "LIB_UTIL___Win32_Compaq_Debug"
# PROP BASE Intermediate_Dir "LIB_UTIL___Win32_Compaq_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Compaq_Debug"
# PROP Intermediate_Dir "Compaq_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\Client\Util" /I "..\Client\Network" /I "..\znzin11\3rdparty\lua-4.0.1\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\Client\Network" /I "..\Client\Util" /I "..\znzin11\3rdparty\lua-4.0.1\include" /I "..\Server\SHO_GS\SHO_GS_LIB\srv_COMMON" /I "..\Server\SHO_GS\SHO_GS_LIB\Common" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:".\VC_LIB\Util_d.lib"
# ADD LIB32 /nologo /out:".\VC_LIB\Util_d.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Desc=copy libs
PostBuild_Cmds=copy               VC_LIB\*.lib               ..\Client\util\ 
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "LIB_UTIL - Win32 Release"
# Name "LIB_UTIL - Win32 Debug"
# Name "LIB_UTIL - Win32 Debug2"
# Name "LIB_UTIL - Win32 Xeon Debug"
# Name "LIB_UTIL - Win32 Compaq Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Network"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\CClientSOCKET.cpp
# End Source File
# Begin Source File

SOURCE=..\Client\Util\CClientSOCKET.h
# End Source File
# Begin Source File

SOURCE=.\CPacketCODEC.cpp
# End Source File
# Begin Source File

SOURCE=.\CPacketCODEC.h
# End Source File
# Begin Source File

SOURCE=.\CRawSOCKET.cpp
# End Source File
# Begin Source File

SOURCE=..\Client\Util\CRawSOCKET.h
# End Source File
# Begin Source File

SOURCE=.\CshoSOCKET.cpp
# End Source File
# Begin Source File

SOURCE=.\CSocket.cpp
# End Source File
# Begin Source File

SOURCE=..\Client\Util\CSocket.h
# End Source File
# Begin Source File

SOURCE=.\CSocketWND.cpp
# End Source File
# Begin Source File

SOURCE=..\Client\Util\CSocketWND.h
# End Source File
# Begin Source File

SOURCE=.\PacketHEADER.cpp
# End Source File
# End Group
# Begin Group "Thread"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\classSYNCOBJ.cpp
# End Source File
# Begin Source File

SOURCE=.\classSYNCOBJ.h
# End Source File
# Begin Source File

SOURCE=.\classTHREAD.cpp
# End Source File
# Begin Source File

SOURCE=.\classTHREAD.h
# End Source File
# End Group
# Begin Group "OGG"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\classOGG.cpp
# End Source File
# Begin Source File

SOURCE=.\classOGG.h
# End Source File
# End Group
# Begin Group "Trace"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\classTRACE.cpp
# End Source File
# Begin Source File

SOURCE=.\classTRACE.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\CBITArray.cpp
# End Source File
# Begin Source File

SOURCE=.\cdxHPC.cpp
# End Source File
# Begin Source File

SOURCE=.\classCRC.cpp
# End Source File
# Begin Source File

SOURCE=.\classHASH.cpp
# End Source File
# Begin Source File

SOURCE=.\classHTTP.cpp
# End Source File
# Begin Source File

SOURCE=.\classIME.cpp
# End Source File
# Begin Source File

SOURCE=.\classLOG.cpp
# End Source File
# Begin Source File

SOURCE=.\classLUA.cpp
# End Source File
# Begin Source File

SOURCE=.\classMD5.cpp
# End Source File
# Begin Source File

SOURCE=.\classSTB.cpp
# End Source File
# Begin Source File

SOURCE=.\classSTR.cpp
# End Source File
# Begin Source File

SOURCE=.\classTIME.cpp
# End Source File
# Begin Source File

SOURCE=.\classUTIL.cpp
# End Source File
# Begin Source File

SOURCE=.\CMMPlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\CRandom.cpp
# End Source File
# Begin Source File

SOURCE=.\CRandom.h
# End Source File
# Begin Source File

SOURCE=.\LIB_IJL.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\Client\Util\CBITArray.h
# End Source File
# Begin Source File

SOURCE=.\CDXHPC.H
# End Source File
# Begin Source File

SOURCE=.\classCRC.h
# End Source File
# Begin Source File

SOURCE=..\Client\Util\classHASH.h
# End Source File
# Begin Source File

SOURCE=..\Client\Util\classHTTP.h
# End Source File
# Begin Source File

SOURCE=..\Client\Util\classIME.h
# End Source File
# Begin Source File

SOURCE=..\Client\Util\classLOG.h
# End Source File
# Begin Source File

SOURCE=..\Client\Util\classLUA.h
# End Source File
# Begin Source File

SOURCE=..\Client\Util\classMD5.h
# End Source File
# Begin Source File

SOURCE=.\classMD5Defines.h
# End Source File
# Begin Source File

SOURCE=..\Client\Util\classSTB.h
# End Source File
# Begin Source File

SOURCE=..\Client\Util\classSTR.h
# End Source File
# Begin Source File

SOURCE=..\Client\Util\classTIME.h
# End Source File
# Begin Source File

SOURCE=..\Client\Util\classUTIL.h
# End Source File
# Begin Source File

SOURCE=..\Client\Util\CMMPlayer.h
# End Source File
# Begin Source File

SOURCE=.\CWaveFILE.h
# End Source File
# Begin Source File

SOURCE=..\Client\Util\DLLIST.h
# End Source File
# Begin Source File

SOURCE=.\DynamicData.h
# End Source File
# Begin Source File

SOURCE=.\LIB_IJL.h
# End Source File
# Begin Source File

SOURCE=.\LIB_Util.h
# End Source File
# Begin Source File

SOURCE=..\Client\Util\SLLIST.h
# End Source File
# End Group
# End Target
# End Project
