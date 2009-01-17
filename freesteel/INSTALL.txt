FreeSteel - Dependencies
------------------------

VTK 4.4
wxWindows 2.4
Python 2.3 (enthought?)
SWIG 1.3.21

Add VTK subdirs to VC's include directories.
Add wxWindows\includes to VC's include directories.

Environment variables:
VTK - VTK root. e.g. C:\VTK (binaries should be in bin\Release and bin\Debug)
WXWINDOWS - WxWindows root. e.g. C:\wxWindows-2.4.2
PYTHON_LIB - e.g. C:\Python23\libs\python23.lib
PYTHON_INCLUDE - e.g. C:\Python23\include

Path:

swig.exe
python.exe
pythonw.exe

VTK
---

Download latest source version.
Install CMake.
Run CMake specifying the VTK root dir for both source and destination, configure build files for desired compiler.
? change any options ?
Open VTK.sln and build ALL_BUILD 

NB! VTK must be built separately for VC6 and VC7 if both are to be used.

wxWindows
---------


Set up a new C++ DLL with SWIG-generated Python wrapper.
--------------------------------------------------------

Don't use precompiled header files - it causes unresolvable conflicts when used without the debug version of the python library.

Add XxxXxx.i file to project.
Custom build properties for XxxXxx.i:
Command Line: swig -c++ -python "$(InputPath)"
Outputs: $(InputName)_wrap.cxx

Add XxxXxx_wrap.cxx file to project.
For debug build to work without pythonXX_d.lib:
Custom build properties for XxxXxx_wrap.cxx (Debug):
C/C++/Advanced:
Disable specific warnings: 4025
Undefine Preprocessor Definitions: _DEBUG


Python
------

