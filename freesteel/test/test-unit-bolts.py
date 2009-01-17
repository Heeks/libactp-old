# Test cases for classes in freesteel/src/bolts
from math import *
from FreesteelPython import *

i1 = I1(-1.2, 1.2)
assert(i1.lo == -1.2)
assert(i1.hi == 1.2)
assert(i1.Leng() == 2.4)

p2 = P2(2, 2)
assert(p2.u == 2)
assert(p2.v == 2)
assert(p2.Len() == sqrt(2**2+2**2))
assert(Dot(p2, p2) == 2*2+2*2)

p3 = P3(2,2,2)
assert(p3.x == 2)
assert(p3.y == 2)
assert(p3.z == 2)
assert(p3.Len() == sqrt(2**2+2**2+2**2))
assert(Dot(p3, P3(1.1,2.2,3.3)) == 2*1.1+2*2.2+2*3.3)
assert(EqualP2(P2(1,1), P2(1,1), P2(1,1)))

try:
	assert(False)
	print "ERROR: Assertions disabled!"
except AssertionError:
	print "All tests successful!"
