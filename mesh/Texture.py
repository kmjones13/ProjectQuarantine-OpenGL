
from OpenGL.GL import *
import png
from ctypes import *
import array

class Texture2D:
    def __init__(self,**kw):
        R=png.Reader(filename=kw["filename"])
        
        #boxed row, flat pixel format
        w,h,pix1,metadata = R.read()
        
        #need to invert the y coordinates
        pix1=list(pix1)
        pix2 = array.array("B")
        for row in reversed(pix1):
            pix2 += row
        
        self.tex = glGenTextures(1)
        glBindTexture(GL_TEXTURE_2D,self.tex)
        if len(pix2) == w*h*3:
            fmt=GL_RGB
        elif len(pix2) == w*h*4:
            fmt=GL_RGBA
        else:
            print("Bad format:",w*h,len(pix2),metadata)
            assert 0
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,w,h,0,fmt,GL_UNSIGNED_BYTE,
            pix2.tobytes())
        
        if self.isPowerOf2(w) and self.isPowerOf2(h):
            glGenerateMipmap(GL_TEXTURE_2D)
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT)
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT)
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR)
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR)
        else:
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE)
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE)
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR)
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR)
            
        self.w=w
        self.h=h
        
    def isPowerOf2(self,x):
        return  ((x-1)&x) == 0
        
