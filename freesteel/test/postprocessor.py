from  FreesteelPython import *
import FreesteelWindow
import sys
import string

class pp:

	def __init__(self):
		self.lx0 = ""
		self.ly0 = ""
		self.lz0 = ""

	def writeheading(self, fout):
		fout.write("BEGINPGM\n")
		fout.write("LOADTL1\n")

	def writeline(self, fout, x, y, z):
		res = [ 'L' ]
		sx = "X%.3f" % x
		if sx != self.lx0:
			res.append(sx)
			self.lx0 = sx
		sy = "Y%.3f" % y
		if sy != self.ly0:
			res.append(sy)
			self.ly0 = sy
		sz = "Z%.3f" % z
		if sz != self.lz0:
			res.append(sz)
			self.lz0 = sz
		if len(res) > 1:
			res.append('\n')
			fout.write(string.join(res, ''))

	def writePath(self, fout, pathx):
		j = 0
		restart = 1
		for i in xrange(pathx.GetNpts()):
			if (j == pathx.GetNbrks()) or (i < pathx.GetBrkIndex(j)):
				if restart == 1:
					fout.write("//////// Begin new path ///////\n")
					self.writeline(fout, pathx.GetX(i - 1), pathx.GetY(i - 1), pathx.z)
					restart = 0
				self.writeline(fout, pathx.GetX(i), pathx.GetY(i), pathx.z)
			else:
				while (j < pathx.GetNbrks()) and (i == pathx.GetBrkIndex(j)):
					if pathx.GetNlnks(j) > 0:
						restart == 1
						fout.write("//////// Begin new link ///////\n")

					for il in xrange(pathx.GetNlnks(j)):
						self.writeline(fout, pathx.GetLinkX(j, il), pathx.GetLinkY(j, il), pathx.GetLinkZ(j, il))

					j = j + 1


	
def postprocess(pathx, fname): 
	lpp = pp()
	fout = open(fname, "w")
	lpp.writeheading(fout)


	fout.write("G0\n")
	lpp.writePath(fout, pathx)
	#for i in xrange(pathx.GetNpts()):
	#	lpp.writeline(fout, pathx.GetX(i), pathx.GetY(i), pathx.z)

	fout.write("ENDPGM\n")
	fout.close()

