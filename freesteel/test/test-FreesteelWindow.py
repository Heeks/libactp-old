import FreesteelPython    # SWIG generated interface to C++ modules FreesteelKernel and FreesteelVTK
import FreesteelWindow   # pure Python
import sys

mainframe = FreesteelWindow.MainFrame()
#cone = FreesteelPython.VtkCone()
#mainframe.vtkwindow.Display(cone)
#mainframe.vtkwindow.Display(mainframe.spheredata)

if ('interactive' not in sys.argv):
	# Running mainloop. Run interactively (-i) with argument 'interactive' to access the interactive console.
	mainframe.mainloop()
