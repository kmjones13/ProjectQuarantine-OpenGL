
from math3d import *

class Sphere:
    def __init__(self,c,r):
        self.c = c.xyz
        self.r = r
        
    def overlaps(self,other):
        tmp  = (self.c-other.c)
        dist2 = dot(tmp,tmp)
        rs = self.r+other.r
        rs2 = rs*rs
        #if dist < rs  === if dist**2 < rs**2
        if dist2 < rs2:
            return True
        else:
            return False
            
