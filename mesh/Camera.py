
from math3d import *
import math

class Camera:
    def __init__(self,**kw):
        self.fov_h = kw.get("fov",45)
        self.hither = kw.get("hither",0.1)
        self.yon = kw.get("yon",1000)
        self.aspect_ratio = kw.get("aspect_ratio",1)
        self.fov_v = 1.0/self.aspect_ratio * self.fov_h
        self.eye = kw.get("eye",vec4(0,0,0,1))
        self.U=vec4(1,0,0,0)
        self.V=vec4(0,1,0,0)
        self.W=vec4(0,0,1,0)
        self.compute_proj_matrix()
        self.compute_view_matrix()
        
    def compute_proj_matrix(self):
        self.projmatrix = mat4( 
            1/math.tan(math.radians(self.fov_h)),0,0,0,
            0,1/math.tan(math.radians(self.fov_v)),0,0,
            0,0,1+2*self.yon/(self.hither-self.yon),-1,
            0,0,2*self.hither*self.yon/(self.hither-self.yon),0)
            
    def compute_view_matrix(self):
        U=self.U
        V=self.V
        W=self.W
        self.viewmatrix = translation(-1*self.eye) * \
                mat4( U.x,V.x,W.x,0,  
                      U.y,V.y,W.y,0,
                      U.z,V.z,W.z,0,
                      0,0,0,1)
        
        
    def draw(self,prog):
        prog.setUniform("projMatrix",self.projmatrix)
        prog.setUniform("viewMatrix",self.viewmatrix)
        
        #we could just get these from the viewmatrix
        #in the shader, but this is a little easier
        #to understand...
        prog.setUniform("eyePos",self.eye.xyz)
        prog.setUniform("cameraU",self.U.xyz)
        prog.setUniform("cameraV",self.V.xyz)
        prog.setUniform("cameraW",self.W.xyz)
        
    def turn(self,a):
        M = axisRotation( self.V, a )
        self.U = self.U*M
        self.W = self.W*M
        self.compute_view_matrix()
        
    def pitch(self,a):
        M = axisRotation( self.U, a )
        self.V = self.V*M
        self.W = self.W*M
        self.compute_view_matrix()

    def axisTurn(self,axis,a):
        M = axisRotation( axis, a )
        self.U = self.U*M
        self.V = self.V*M
        self.W = self.W*M
        self.compute_view_matrix()
        
    def tilt(self,a):
        M = axisRotation( self.W, a )
        self.V = self.V*M
        self.U = self.U*M
        self.compute_view_matrix()
        
    def walk(self,a):
        self.eye = self.eye + -a * self.W
        self.compute_view_matrix()
        
    def strafe(self,v):
        self.eye = self.eye + v.x * self.U + v.y * self.V + -v.w * self.W
        self.compute_view_matrix()
        
    def lookAt(self,eye,coi,up):
        self.eye = vec4(eye.xyz,1)
        look = normalize(vec4(coi.xyz,1)-vec4(eye.xyz,1))
        up = vec4(up.xyz,0)
        self.W = -look
        self.U = cross(look,up)
        self.V = cross(self.U,look)
        self.compute_view_matrix()
    
