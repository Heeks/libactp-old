from FreesteelPython import *

"""
Issues:
	Crashes when toolflatrad != 0.0
"""

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

def coreRough(res, sx, bound, params, z, blpaths = PathXSeries()):
	# boxed surfaces 
	sxb = SurfXboxed(sx); 
	sxb.BuildBoxes(10.0); 

	# interior close to tool, or absolute intersections with triangle faces
	areaoversize = (params.toolcornerrad + params.toolflatrad) * 2 + 13; 

	# these can be reused
	a2g = Area2_gen()
	a2g.SetShape(sx.gxrg.Inflate(areaoversize), sx.gyrg.Inflate(areaoversize), params.triangleweaveres); 
	a2g.SetSurfaceTop(sxb, params.toolcornerrad); 
	
	a2gfl = Area2_gen()
	a2gfl.SetShape(sx.gxrg.Inflate(areaoversize), sx.gyrg.Inflate(areaoversize), params.flatradweaveres);

	# make the core roughing algorithm thing
	crg = CoreRoughGeneration(res, sx.gxrg.Inflate(10), sx.gyrg.Inflate(10)); 
	# the stock boundary 
	crg.tsbound.Append(bound.pths); 

	# the material boundary weave used in the core roughing.  
	if params.toolflatrad != 0.0:
		crg.pa2gg = a2gfl
	else:
		crg.pa2gg = a2g
	crg.trad = params.toolcornerrad * 0.9 + params.toolflatrad; # the clearing radius 
	crg.setWeave(crg.pa2gg); 

	# hack against the surfaces 
	a2g.HackDowntoZ(z); 
	a2g.MakeContours(blpaths); 

	# hack by the flatrad.  
	if (params.toolflatrad != 0.0):
		HackAreaOffset(a2gfl, blpaths, params.toolflatrad); 
		a2gfl.z = a2g.z; 

		# make it again so we can see
		blpaths = PathXSeries(); 
		a2gfl.MakeContours(blpaths); 

	crg.GrabberAlg(params); 

class CoreRougher:
	def __init__(self, surface, boundary, params):
		self.surface = surface
		self.params = params
		self.boundary = boundary

		# define the empty surface
		self.sx = SurfX(surface.xrg.Inflate(2), surface.yrg.Inflate(2), surface.zrg)
		surface.PushTrianglesIntoSurface(self.sx)
		self.sx.BuildComponents() # compress thing

		# boxed surfaces 
		self.sxb = SurfXboxed(self.sx); 
		self.sxb.BuildBoxes(10.0); 

		# interior close to tool, or absolute intersections with triangle faces
		areaoversize = (self.params.toolcornerrad + self.params.toolflatrad) * 2 + 13; 

		# weaves for non-flat/flat part of tool
		self.a2g = Area2_gen()
		self.a2g.SetShape(self.sx.gxrg.Inflate(areaoversize), self.sx.gyrg.Inflate(areaoversize), self.params.triangleweaveres); 
		self.a2g.SetSurfaceTop(self.sxb, params.toolcornerrad); 

		if (self.params.toolflatrad != 0.0):
			self.a2gfl = Area2_gen()
			self.a2gfl.SetShape(self.sx.gxrg.Inflate(areaoversize), self.sx.gyrg.Inflate(areaoversize), self.params.flatradweaveres);

	def generateAt(self, z, path, blpaths ):
		# make the core roughing algorithm thing
		crg = CoreRoughGeneration(path, self.sx.gxrg.Inflate(10), self.sx.gyrg.Inflate(10)); 

		# the stock boundary 
		crg.tsbound.Append(self.boundary.pths); 

		# the material boundary weave used in the core roughing.  
		if (self.params.toolflatrad != 0.0):
			crg.pa2gg = self.a2gfl
		else:
			crg.pa2gg = self.a2g
		crg.trad = self.params.toolcornerrad * 0.9 + self.params.toolflatrad; # the clearing radius 
		crg.setWeave(crg.pa2gg); 

		# hack against the surfaces 
		self.a2g.HackDowntoZ(z); 
		self.a2g.MakeContours(blpaths); 

		# hack by the flatrad.  
		if (self.params.toolflatrad != 0.0):
			HackAreaOffset(self.a2gfl, blpaths, self.params.toolflatrad); 
			self.a2gfl.z = self.a2g.z; 

			# make it again so we can see
			blpaths = PathXSeries(); 
			self.a2gfl.MakeContours(blpaths); 
		
		# generate toolpath
		crg.GrabberAlg(self.params); 
