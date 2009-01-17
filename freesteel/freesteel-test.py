import freesteel
import core_rough
import threading


def CoreThread():
	core_rough.

thread = threading.Thread(target=ThreadFunc, args=tuple([5.0]))
thread.start()

freesteel.Run() # Start GUI


trpart = freesteel.LoadStl("part.stl")

surfx = core.SurfX()
freesteel.LoadTriangles(surfx, trpart)

surfx.BoxTriangles(5.0)
zlevels = []
for i in range(10):
	zlevels.append(zlo + (zhi + zlo) *i /10)

toolpaths = []
for z in zlevels: 
	toolpaths.append(kernel.corerough(surfx, z, toolshape, toolpaths))

toolshape = (cornerrad, flatrad, draftangle, height) 
toolshape.cornerrad = 10
freesteel.show(toolpath[1])
