import os
import sys
import platform as pyplatform
import logging
import traceback
logging.basicConfig()
from ctypes import *
from wintools import *
import OpenGL
from OpenGL.GL import *
from OpenGL.GL.ARB.debug_output import *


def debugcallback(src,typ,ids,sev,cnt,msg,vp):
    print("GL driver message:")
    st = traceback.extract_stack()
    for fname,line,funcname,txt in st:
        if fname.find(os.path.sep+"OpenGL"+os.path.sep) != -1:
            break
        print("\tFrom",fname,":",line,"(",funcname,")")
    print("\t"+msg.decode())
    
def initGL():
           
            
    win,dc,version = CreateGLWindow("ETGG",20,20,512,512)
        
    #3.0 introduced transform feedback
    #3.2 introduced geometry shaders as a core feature
    #3.3 added 64 bit ints + instancing as core feature
    #4.0 introduced tesselation shader
        
    if glInitDebugOutputARB():
        pl=pyplatform.system()
        if pl.lower().find("windows") != -1:
            tmp=WINFUNCTYPE
        else:
            tmp=CFUNCTYPE
        glDebugMessageControlARB(GL_DONT_CARE,GL_DONT_CARE,GL_DONT_CARE,0,None,GL_TRUE)
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB)
        
        #this must be stored to a separate variable to
        #prevent garbage collection and crashes
        global _debug_callback_thing_
        _debug_callback_thing_=tmp(None,
                GLenum, #source
                GLenum, #type
                GLuint, #id
                GLenum, #severity
                GLsizei, #count
                c_char_p,   #message
                c_void_p)(debugcallback)
        glDebugMessageCallbackARB( _debug_callback_thing_, None )
        print("Set up debug callback")



    #use the default VAO for everything...
    #We have to have one bound in the core profile
    vao = GLuint()
    glGenVertexArrays(1,byref(vao))
    glBindVertexArray(vao)

    return win,dc
