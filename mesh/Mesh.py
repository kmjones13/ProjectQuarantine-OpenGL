import array
import math
from ctypes import *
from OpenGL.GL import *
from math3d import *
import Texture

class Mesh:
    def __init__(self,**kw):

        self.texture=None
        
        if "vdata" in kw:
            #directly supplied data
            
            vdata = kw["vdata"]
            idata = kw["idata"]
            
            #8 floats per vertex -- assumed
            self.numvertices = len(vdata)//8
            self.floats_per_vertex = 8
            self.numindices = len(idata)
            self.texture = kw.get("texture",None)
            if idata.typecode == 'B':
                assert 0
            elif idata.typecode == 'H':
                bits_per_index = 16
            elif idata.typecode == 'I':
                bits_per_index = 32
            else:
                assert 0
            
            vdata = vdata.tobytes()
            idata = idata.tobytes()
            self.filename="(none)"
        else:
            self.filename = kw["filename"]

            idx = self.filename.rfind("/")
            if idx == -1:
                prefix=""
            else:
                prefix=self.filename[:idx]
                
            fp=open(self.filename,"rb")
            line=fp.readline()
            if not line.startswith(b"mesh_01_GL"):
                raise RuntimeError("Incorrect mesh format: "+str(line))
                
                
            while 1:
                line = fp.readline()
                if len(line) == 0:
                    break
                line = line.decode().strip()
                lst=line.split(" ")
                if len(line) == 0:
                    pass
                elif lst[0] == "num_vertices":
                    self.numvertices = int(lst[1])
                elif lst[0] == "floats_per_vertex":
                    self.floats_per_vertex = int(lst[1])
                elif lst[0] == "num_indices":
                    self.numindices = int(lst[1])
                elif lst[0] == "texture_file":
                    self.texture = Texture.Texture2D(filename=prefix+"/"+lst[1])
                elif lst[0] == "vertex_data":
                    numbytes=int(lst[1])
                    vdata = fp.read(numbytes)
                elif lst[0] == "bits_per_index":
                    bits_per_index = int(lst[1])
                elif lst[0] == "index_data":
                    numbytes = int(lst[1])
                    idata = fp.read(numbytes)
                elif lst[0] == "end":
                    break
                elif lst[0] == "total_vertex_floats":
                    pass
                else:
                    print("Warning: Ignoring",line)
                    pass
                    
        self.vbuff=c_uint()
        glGenBuffers(1,byref(self.vbuff))
        glBindBuffer(GL_ARRAY_BUFFER,self.vbuff)
        glBufferData(GL_ARRAY_BUFFER,vdata,GL_STATIC_DRAW)
    
        self.ibuff=c_uint()
        glGenBuffers(1,byref(self.ibuff))
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,self.ibuff)
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,idata,GL_STATIC_DRAW)
        
        if bits_per_index == 8:
            self.itype = GL_UNSIGNED_BYTE
        elif bits_per_index == 16:
            self.itype = GL_UNSIGNED_SHORT
        elif bits_per_index == 32:
            self.itype = GL_UNSIGNED_INT
        else:
            assert 0
            
    def draw(self,prog):
        glBindBuffer(GL_ARRAY_BUFFER, self.vbuff)

        if self.texture:
            prog.setUniform("texture",self.texture)
            
        for name,start in [ ("position",0),("texcoord",3), ("normal",5)]:
            idx,size,typ = prog.attrs[name]
            glEnableVertexAttribArray(idx)
            glVertexAttribPointer( 
                idx,size,typ,
                False,          #normalize?
                self.floats_per_vertex*4,            #size of each vertex
                c_void_p(start*4)     #starting location
            )
        
        if self.ibuff != None:
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,self.ibuff)
            glDrawElements(GL_TRIANGLES,self.numindices,self.itype,c_void_p(0))
        else:
            glDrawArrays(GL_TRIANGLES,0,self.numvertices)
                
