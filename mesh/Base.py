import os
import sys
import time
from ctypes import *
import array

import OpenGL
from OpenGL.GL import *
from math3d import *
from Sphere import Sphere

#I need a better name for this...
class Base:
    def __init__(self,mesh,position,radius):
        self.M=mesh
        self.pos = position
        self.hostile = True
        self.state=0        #0=normal, 1=being destroyed, 2=dead
        self.radius=radius
        self.alpha=1
        
    def destroy(self):
        print("DESTROY! COBRA!")
        if self.state == 0:
            self.state=1
        
    def draw(self,prog):
        if self.state == 2:
            return

        prog.setUniform("worldMatrix",translation(self.pos))
        prog.setUniform("alpha",self.alpha)
        self.M.draw(prog)

    def bounding_sphere(self):
        return Sphere( self.pos, self.radius )
        
    def update(self,elapsed):
        if self.state == 1:
            self.alpha -= 0.5*elapsed
            if self.alpha <= 0:
                self.state=2
        
    def is_dead(self):
        return self.state == 2
