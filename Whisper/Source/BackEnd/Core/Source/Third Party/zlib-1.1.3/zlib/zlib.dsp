# Microsoft Developer Studio Project File - Name="zlib" - Package Owner=<4>

# Microsoft Developer Studio Generated Build File, Format Version 6.00

# ** DO NOT EDIT **



# TARGTYPE "Win32 (x86) Static Library" 0x0104



CFG=zlib - Win32 Debug

!MESSAGE This is not a valid makefile. To build this project using NMAKE,

!MESSAGE use the Export Makefile command and run

!MESSAGE 

!MESSAGE NMAKE /f "zlib.mak".

!MESSAGE 

!MESSAGE You can specify a configuration when running NMAKE

!MESSAGE by defining the macro CFG on the command line. For example:

!MESSAGE 

!MESSAGE NMAKE /f "zlib.mak" CFG="zlib - Win32 Debug"

!MESSAGE 

!MESSAGE Possible choices for configuration are:

!MESSAGE 

!MESSAGE "zlib - Win32 Release" (based on "Win32 (x86) Static Library")

!MESSAGE "zlib - Win32 Debug" (based on "Win32 (x86) Static Library")

!MESSAGE 



# Begin Project

# PROP AllowPerConfigDependencies 0

# PROP Scc_ProjName ""

# PROP Scc_LocalPath ""

CPP=cl.exe

RSC=rc.exe



!IF  "$(CFG)" == "zlib - Win32 Release"



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

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c

# ADD CPP /nologo /MT /W3 /O2 /I "..\zlib 1.1.3 Folder" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c

# ADD BASE RSC /l 0x409

# ADD RSC /l 0x409

BSC32=bscmake.exe

# ADD BASE BSC32 /nologo

# ADD BSC32 /nologo

LIB32=link.exe -lib

# ADD BASE LIB32 /nologo

# ADD LIB32 /nologo



!ELSEIF  "$(CFG)" == "zlib - Win32 Debug"



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

# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c

# ADD CPP /nologo /MTd /W3 /Z7 /Od /I "..\zlib 1.1.3 Folder" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c

# ADD BASE RSC /l 0x409

# ADD RSC /l 0x409

BSC32=bscmake.exe

# ADD BASE BSC32 /nologo

# ADD BSC32 /nologo

LIB32=link.exe -lib

# ADD BASE LIB32 /nologo

# ADD LIB32 /nologo



!ENDIF 



# Begin Target



# Name "zlib - Win32 Release"

# Name "zlib - Win32 Debug"

# Begin Source File



SOURCE="..\zlib 1.1.3 Folder\adler32.c"

# End Source File

# Begin Source File



SOURCE="..\zlib 1.1.3 Folder\compress.c"

# End Source File

# Begin Source File



SOURCE="..\zlib 1.1.3 Folder\crc32.c"

# End Source File

# Begin Source File



SOURCE="..\zlib 1.1.3 Folder\deflate.c"

# End Source File

# Begin Source File



SOURCE="..\zlib 1.1.3 Folder\deflate.h"

# End Source File

# Begin Source File



SOURCE="..\zlib 1.1.3 Folder\gzio.c"

# End Source File

# Begin Source File



SOURCE="..\zlib 1.1.3 Folder\infblock.c"

# End Source File

# Begin Source File



SOURCE="..\zlib 1.1.3 Folder\infblock.h"

# End Source File

# Begin Source File



SOURCE="..\zlib 1.1.3 Folder\infcodes.c"

# End Source File

# Begin Source File



SOURCE="..\zlib 1.1.3 Folder\infcodes.h"

# End Source File

# Begin Source File



SOURCE="..\zlib 1.1.3 Folder\inffast.c"

# End Source File

# Begin Source File



SOURCE="..\zlib 1.1.3 Folder\inffast.h"

# End Source File

# Begin Source File



SOURCE="..\zlib 1.1.3 Folder\inffixed.h"

# End Source File

# Begin Source File



SOURCE="..\zlib 1.1.3 Folder\inflate.c"

# End Source File

# Begin Source File



SOURCE="..\zlib 1.1.3 Folder\inftrees.c"

# End Source File

# Begin Source File



SOURCE="..\zlib 1.1.3 Folder\inftrees.h"

# End Source File

# Begin Source File



SOURCE="..\zlib 1.1.3 Folder\infutil.c"

# End Source File

# Begin Source File



SOURCE="..\zlib 1.1.3 Folder\infutil.h"

# End Source File

# Begin Source File



SOURCE="..\zlib 1.1.3 Folder\trees.c"

# End Source File

# Begin Source File



SOURCE="..\zlib 1.1.3 Folder\trees.h"

# End Source File

# Begin Source File



SOURCE="..\zlib 1.1.3 Folder\uncompr.c"

# End Source File

# Begin Source File



SOURCE="..\zlib 1.1.3 Folder\zconf.h"

# End Source File

# Begin Source File



SOURCE="..\zlib 1.1.3 Folder\zlib.h"

# End Source File

# Begin Source File



SOURCE="..\zlib 1.1.3 Folder\zutil.c"

# End Source File

# Begin Source File



SOURCE="..\zlib 1.1.3 Folder\zutil.h"

# End Source File

# End Target

# End Project

