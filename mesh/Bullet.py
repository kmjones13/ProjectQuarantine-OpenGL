import os
import sys
import time
from ctypes import *
import array

import OpenGL
from OpenGL.GL import *
from math3d import *
from Mesh import Mesh
import Texture 
from Sphere import Sphere 

class Bullet:
    M=None
    
    def __init__(self,pos,vel,lifetime,shooter):
        if Bullet.M == None:
            
            verts = [   0,0,0,  0,0,  0,0,0,
                        0,0,0,  0,1, 0,0,0,
                        0,0,0,  1,0, 0,0,0,
                        0,0,0,  1,1, 0,0,0 ]
            verts = array.array( 'f',verts )
            
            indices = [ 0,2,1,  2,3,1 ]
            indices = array.array( 'H', indices )

            tex = Texture.Texture2D(filename="nova.png")
            Bullet.M = Mesh(vdata=verts,idata=indices,texture=tex)
            
        self.pos = pos
        self.vel = vel
        self.lifeleft = lifetime
        self.shooter = shooter 
        
    def draw(self,prog):
        if self.lifeleft <= 0:
            return
        prog.setUniform("worldMatrix",translation(self.pos))
        a=self.lifeleft/0.25
        if a > 1:
            a=1
        prog.setUniform("alpha",a)
        Bullet.M.draw(prog)
        
    def bounding_sphere(self):
        return Sphere(self.pos,1)
        
    def update(self,elapsed):
        self.pos = self.pos + elapsed * self.vel
        self.lifeleft -= elapsed
        
    def is_dead(self):
        return self.lifeleft <= 0
            
