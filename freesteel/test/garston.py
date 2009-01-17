from  FreesteelPython import *
import FreesteelWindow
import sys

import postprocessor
#from postprocessor import postprocess
from coreRough import coreRough


def makeRectBoundary(sx):
	bdy = PathXSeries()
	bdy.Add(P2(sx.gxrg.lo, sx.gyrg.lo))
	bdy.Add(P2(sx.gxrg.hi, sx.gyrg.lo))
	bdy.Add(P2(sx.gxrg.hi, sx.gyrg.hi))
	bdy.Add(P2(sx.gxrg.lo, sx.gyrg.hi))
	bdy.Add(P2(sx.gxrg.lo, sx.gyrg.lo))
	bdy.z = sx.gzrg.hi + 5
	bdy.Break()
	return bdy

def makeParams(stepdown=15.0, toolcornerrad=3.0, toolflatrad=0.0, retractheight=50.0):
	params = MachineParams()
	
	# linking parameters
	params.leadoffdz = 0.1
	params.leadofflen = 1.1
	params.leadoffrad = 2.0
	params.retractzheight = retractheight
	params.leadoffsamplestep = 0.6
	
	# cutting parameters
	params.toolcornerrad = toolcornerrad
	params.toolflatrad = toolflatrad
	params.samplestep = 0.4
	params.stepdown = stepdown
	params.clearcuspheight = params.stepdown / 3.0
	
	# weave parameters
	params.triangleweaveres = 0.51
	params.flatradweaveres = 0.71
	
	# stearing parameters
	# fixed values controlling the step-forward of the tool and 
	# changes of direction.  
	params.dchangright = 0.17
	params.dchangrightoncontour = 0.37
	params.dchangleft = -0.41
	params.dchangefreespace = -0.6
	params.sidecutdisplch = 0.0
	params.fcut = 1000
	params.fretract = 5000
	params.thintol = 0.0001

	return params



mainframe = FreesteelWindow.MainFrame()
vtkwindow = mainframe.vtkwindow


surfhandle = vtkwindow.LoadSTL("mm.stl")
vtkwindow.showAll() # zoom to fit

surfx = SurfX()

vtkwindow.PushTrianglesIntoSurface(surfhandle, surfx)
surfx.BuildComponents()


boundary = makeRectBoundary(surfx)
vtkwindow.addPathxSeries(boundary)
vtkwindow.render()

params = makeParams()
params.retractheight = surfx.gzrg.hi + 2

z = surfx.gzrg.lo + 2
pathx = PathXSeries()
coreRough(pathx, surfx, boundary, params, z)
vtkwindow.addPathxSeries(pathx)

pathx2 = pathx
z = surfx.gzrg.lo + 20
coreRough(pathx2, surfx, boundary, params, z)
vtkwindow.addPathxSeries(pathx2)

vtkwindow.render()

# send to post processor
postprocessor.postprocess(pathx, "ghgh.tap") 

if ('interactive' not in sys.argv):
	# Running mainloop. Run interactively (-i) with argument 'interactive' to access the interactive console.
	mainframe.mainloop()

