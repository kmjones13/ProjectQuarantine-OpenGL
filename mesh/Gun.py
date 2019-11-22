


from ctypes import *
from OpenGL.GL import *
from math3d import *
import Texture 
from math3d import *
from Mesh import Mesh
import array
from Sphere import Sphere
import random
import Globals 
from Bullet import Bullet 
import Cylinder
import Base

class Gun(Base.Base):
    M=None
    def __init__(self,pos):
        
        if Gun.M == None:
            Gun.M = Mesh(filename="assets/tank.obj.mesh")
                #vdata=array.array('f',Cylinder.vdata), 
                #idata=array.array('H',Cylinder.idata),
                #texture=Texture.Texture2D(filename="camo.png")
        
        Base.Base.__init__(self,mesh=Gun.M,position=pos,radius=1)
        self.nextshot = random.randrange(5)
        self.hostile = True
        
    def update(self,elapsed):
        Base.Base.update(self,elapsed)
        self.nextshot -= elapsed
        if self.nextshot <= 0 :
            self.nextshot = 1 + random.randrange(50)*0.1
            Globals.bullets.append( Bullet(
                self.pos,
                vec3(0,0.1*random.randrange(20,30),0),
                4,
                self) )
