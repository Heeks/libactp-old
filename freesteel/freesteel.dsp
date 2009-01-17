# Microsoft Developer Studio Project File - Name="freesteel" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=freesteel - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "freesteel.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "freesteel.mak" CFG="freesteel - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "freesteel - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "freesteel - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "freesteel - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I "$(WXWINDOWS)\include" /I "$(WXWINDOWS)\lib\mswdlld" /I "src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D WINVER=0x400 /D "_MT" /D wxUSE_GUI=1 /D WXUSINGDLL=1 /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 wxmsw24.lib vtkCommon.lib vtkRendering.lib vtkGraphics.lib vtkFiltering.lib vtkIO.lib vtkImaging.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib opengl32.lib /nologo /subsystem:windows /machine:I386 /libpath:"$(VTK_BIN)\Release"
# SUBTRACT LINK32 /pdb:none /map /debug

!ELSEIF  "$(CFG)" == "freesteel - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /GR /GX /Zi /Od /I "$(WXWINDOWS)\include" /I "$(WXWINDOWS)\lib\mswdll" /I "src" /D "MDEBUG" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D WINVER=0x400 /D "_MT" /D wxUSE_GUI=1 /D "__WXDEBUG__" /D WXDEBUG=1 /D WXUSINGDLL=1 /FD /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 wxmsw24d.lib vtkCommon.lib vtkRendering.lib vtkGraphics.lib vtkFiltering.lib vtkIO.lib vtkImaging.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib comctl32.lib rpcrt4.lib wsock32.lib opengl32.lib /nologo /subsystem:windows /pdb:"Debug/init_scraps.pdb" /debug /machine:I386 /pdbtype:sept /libpath:"$(VTK_BIN)\Debug"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "freesteel - Win32 Release"
# Name "freesteel - Win32 Debug"
# Begin Group "bolts"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\bolts\bolts.h
# End Source File
# Begin Source File

SOURCE=.\src\bolts\debugfuncs.h
# End Source File
# Begin Source File

SOURCE=.\src\bolts\I1.h
# End Source File
# Begin Source File

SOURCE=.\src\bolts\P2.h
# End Source File
# Begin Source File

SOURCE=.\src\bolts\P3.h
# End Source File
# Begin Source File

SOURCE=.\src\bolts\S1.cpp
# End Source File
# Begin Source File

SOURCE=.\src\bolts\S1.h
# End Source File
# Begin Source File

SOURCE=.\src\bolts\smallfuncs.h
# End Source File
# Begin Source File

SOURCE=.\src\bolts\vo.h
# End Source File
# End Group
# Begin Group "cages"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\cages\Area2_gen.h
# End Source File
# Begin Source File

SOURCE=.\src\cages\cages.h
# End Source File
# Begin Source File

SOURCE=.\src\cages\PathX.cpp
# End Source File
# Begin Source File

SOURCE=.\src\cages\PathX.h
# End Source File
# Begin Source File

SOURCE=.\src\cages\S1stockcircle.cpp
# End Source File
# Begin Source File

SOURCE=.\src\cages\S2weave.cpp
# End Source File
# Begin Source File

SOURCE=.\src\cages\S2weave.h
# End Source File
# Begin Source File

SOURCE=.\src\cages\SurfX.cpp
# End Source File
# Begin Source File

SOURCE=.\src\cages\SurfX.h
# End Source File
# End Group
# Begin Group "xenon"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\xenon\AnimationFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\src\xenon\AnimationFrame.h
# End Source File
# Begin Source File

SOURCE=.\src\xenon\BoundingBoxDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\xenon\BoundingBoxDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\xenon\coreroughdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\xenon\coreroughdlg.h
# End Source File
# Begin Source File

SOURCE=.\src\xenon\MainFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\src\xenon\MainFrame.h
# End Source File
# Begin Source File

SOURCE=.\src\xenon\MyApp.cpp
# End Source File
# Begin Source File

SOURCE=.\src\xenon\MyApp.h
# End Source File
# Begin Source File

SOURCE=.\src\xenon\wxhelpers.cpp
# End Source File
# Begin Source File

SOURCE=.\src\xenon\wxhelpers.h
# End Source File
# Begin Source File

SOURCE=.\src\xenon\wxVTKRenderWindowInteractor.cxx
# End Source File
# Begin Source File

SOURCE=.\src\xenon\wxVTKRenderWindowInteractor.h
# End Source File
# End Group
# Begin Group "pits"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\pits\CircCrossingStructure.h
# End Source File
# Begin Source File

SOURCE=.\src\pits\NormRay_gen.cpp
# End Source File
# Begin Source File

SOURCE=.\src\pits\pits.h
# End Source File
# Begin Source File

SOURCE=.\src\pits\S2weaveCell.cpp
# End Source File
# Begin Source File

SOURCE=.\src\pits\S2weaveCell.h
# End Source File
# Begin Source File

SOURCE=.\src\pits\S2weaveCellLinearCut.cpp
# End Source File
# Begin Source File

SOURCE=.\src\pits\S2weaveCellLinearCut.h
# End Source File
# Begin Source File

SOURCE=.\src\pits\S2weaveCircle.cpp
# End Source File
# Begin Source File

SOURCE=.\src\pits\SLi_gen.cpp
# End Source File
# Begin Source File

SOURCE=.\src\pits\SLi_gen.h
# End Source File
# Begin Source File

SOURCE=.\src\pits\SurfXbuildcomponents.cpp
# End Source File
# Begin Source File

SOURCE=.\src\pits\SurfXSliceRay.cpp
# End Source File
# Begin Source File

SOURCE=.\src\pits\toolshape.h
# End Source File
# End Group
# Begin Group "visuals"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\visuals\fsvtkToolpathMapper.cpp
# End Source File
# Begin Source File

SOURCE=.\src\visuals\fsvtkToolpathMapper.h
# End Source File
# Begin Source File

SOURCE=.\src\visuals\GeometryStuffTogether.cpp
# End Source File
# Begin Source File

SOURCE=.\src\visuals\gst.h
# End Source File
# Begin Source File

SOURCE=.\src\visuals\gstsurface.cpp
# End Source File
# Begin Source File

SOURCE=.\src\visuals\gstsurface.h
# End Source File
# Begin Source File

SOURCE=.\src\visuals\MakeToolpath.cpp
# End Source File
# Begin Source File

SOURCE=.\src\visuals\MakeToolpath.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\src\pits\S2weaveCellLinearCutTraverse.cpp
# End Source File
# Begin Source File

SOURCE=.\src\pits\toolshape.cpp
# End Source File
# End Target
# End Project
