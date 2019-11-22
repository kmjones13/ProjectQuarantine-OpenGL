import os
import sys
import time
from ctypes import *
import array

import OpenGL
from OpenGL.GL import *
from math3d import *
from Mesh import Mesh
from Sphere import Sphere
from Base import Base
import Texture
import TetrahedronData

class Tetrahedron(Base):
    M=None
    
    def __init__(self,pos):
        if Tetrahedron.M == None:
            Tetrahedron.M = Mesh(filename="assets/tetrahedron.obj.mesh")
            
            """
                vdata=array.array("f",TetrahedronData.vdata),
                idata=array.array("H",TetrahedronData.idata),
                texture=Texture.Texture2D(filename="yellow.png"))
            """
            
        Base.__init__(self,mesh=Tetrahedron.M,position=pos,radius=1)
          
            
