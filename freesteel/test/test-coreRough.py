from  FreesteelPython import *
import FreesteelWindow
from FreesteelWindow import inspect
from coreRough import *
import sys, time, locale

cstart = time.clock() 
clast = cstart
def status(msg):
	global cstart, clast
	print locale.format("%f", time.clock()-cstart, True), locale.format("%f", time.clock()-clast, True), msg
	clast = time.clock()

if len(sys.argv) == 2:
	stlname = sys.argv[1]
else:
	stlname = "upstands1.stl"

status("Loading STL \"" + stlname + "\"")
surface = GSTsurface()
surface.LoadSTL(stlname)

status("Creating boundary")
boundary = surface.MakeRectBoundary(0) # NB: a PathXSeries

status("Creating parameters")
params = makeParams(retractheight=surface.zrg.hi+5.0)

zlevels = []
zcount = 3
zhi = surface.zrg.hi
zlo = surface.zrg.lo
zstep = (zhi-zlo) / zcount
z = zhi + zstep*.1 	# offset by 1/10th of a step
for n in range(zcount):
	z -= zstep
	zlevels.append(z)
print "zlevels =", zlevels

paths = []
blpaths = []
status("Initializing CoreRougher")
cr = CoreRougher(surface, boundary, params)

for z in zlevels:
	status("Generating toolpath at Z%.2f" % z)
	path = PathXSeries()
	blpath = PathXSeries()
	cr.generateAt(z, path, blpath)
	paths.append(path)
	blpaths.append(blpath)

tool = ToolShape(params.toolflatrad, params.toolcornerrad, params.toolcornerrad, params.toolcornerrad / 10.0)

status("Done!")
inspect(surface, paths, boundary, tool)
