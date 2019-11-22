from ctypes import *
from OpenGL.GL import *
from math3d import *
import Texture 

class Program:
    def __init__(self,vsfname,fsfname):
        vs,vsins,vsouts = self.make_shader(vsfname, GL_VERTEX_SHADER)
        fs,fsins,fsouts = self.make_shader(fsfname, GL_FRAGMENT_SHADER)
        
        self.match_variables(vsouts,fsins)
        
        prog = glCreateProgram()
        glAttachShader(prog,vs)
        glAttachShader(prog,fs)
        glLinkProgram(prog)
        infolog = glGetProgramInfoLog(prog)
        if type(infolog) != str:
            infolog = infolog.decode()
        if len(infolog) > 0:
            print("Linking",vsfname,"+",fsfname,":")
            print(infolog)
        i=GLint()
        glGetProgramiv( prog, GL_LINK_STATUS, byref(i) )
        if not i.value:
            raise RuntimeError("Could not link shaders")
        glGetProgramiv( prog, GL_VALIDATE_STATUS, byref(i) )
        #if not i.value:
        #    raise RuntimeError("Could not validate")
        
        self.prog = prog
        
        self.attrs={}
        glGetProgramiv( prog, GL_ACTIVE_ATTRIBUTES, byref(i) )
        for j in range(i.value):
            length = GLuint()
            size = GLint()
            typ=GLenum()
            name = create_string_buffer(200)
            glGetActiveAttrib( prog, j, 200, byref(length), byref(size),
                    byref(typ), name)
            name = name[:length.value]
            name = name.decode()
            
            if size.value == 1 and typ.value == GL_FLOAT_VEC2:
                self.attrs[name] = [
                        glGetAttribLocation(self.prog,name),
                        2, GL_FLOAT 
                ]
            elif size.value == 1 and typ.value == GL_FLOAT_VEC3:
                self.attrs[name] = [
                        glGetAttribLocation(self.prog,name),
                        3, GL_FLOAT 
                ]
            elif size.value == 1 and typ.value == GL_FLOAT:
                self.attrs[name] = [
                        glGetAttribLocation(self.prog,name),
                        1, GL_FLOAT
                ]
            else:
                print("size=",size.value,"typ=",typ,)
                raise RuntimeError("I don't know what to do about the attribute "+name)
              
        numuniforms = GLuint()
        glGetProgramiv( prog,GL_ACTIVE_UNIFORMS, 
            byref(numuniforms))
        numuniforms = numuniforms.value
        self.uniforms = {}
        
        texcount=0
        for i in range(numuniforms):
            ty=GLint()
            sz=GLint()
            glGetActiveUniformsiv(prog,1,byref(GLuint(i)),
                    GL_UNIFORM_TYPE, byref(ty))
            glGetActiveUniformsiv(prog,1,byref(GLuint(i)),
                    GL_UNIFORM_SIZE, byref(sz))
            
            nm = create_string_buffer(128)
            le = GLsizei()
            glGetActiveUniformName( prog, i, len(nm),
                    byref(le), byref(nm) )
            nm=[chr(q) for q in nm[:le.value]]
            nm = ''.join(nm)
            
            loc = glGetUniformLocation(prog,nm)
            print("location=",i,loc)
            
            if ty.value == GL_FLOAT_MAT4 and sz.value == 1:
                setter = self.make_mat4_setter(loc)
            elif ty.value == GL_SAMPLER_2D and sz.value == 1:
                setter = self.make_sampler2d_setter(loc,texcount)
                texcount+=1
            elif ty.value == GL_FLOAT and sz.value == 1:
                setter = self.make_float_setter(loc)
            elif ty.value == GL_FLOAT_VEC3 and sz.value == 1:
                setter = self.make_vec3_setter(loc)
            elif ty.value == GL_FLOAT_VEC3 and sz.value > 1:
                #print("setter for",nm,"@",i,"is the array thing")
                setter = self.make_vec3_array_setter(loc,sz.value)
            else:
                print("Don't know how to do uniform of whatever type '"+nm+"' is")
                print("sz=",sz.value,"type=",ty.value)
                print("GL_FLOAT_VEC3=",GL_FLOAT_VEC3)
                raise RuntimeError()
                
            self.uniforms[nm] = setter
            print(nm,"is uniform #",i,"at",loc)
            
        print("Uniforms:",self.uniforms)
        
    def make_mat4_setter(self,i):
        def func(value):
            if type(value) != mat4:
                raise RuntimeError("Not a mat4")
            glUniformMatrix4fv( i, 1, True, value.tobytes())
        return func
    
    def make_float_setter(self,i):
        def func(value):
            #we'll catch type errors when we do the c_float constructor
            q = c_float(value)
            glUniform1f( i, q )
        return func
        
    def make_vec3_setter(self,i):
        def func(value):
            if type(value) != vec3:
                raise RuntimeError("Not a vec3")
            x = c_float(value.x)
            y = c_float(value.y)
            z = c_float(value.z)
            glUniform3f( i, x,y,z )
        return func
        
    def make_vec3_array_setter(self,i,numentries):
        def func(value):
            if type(value) != list or len(value) != numentries:
                raise RuntimeError("Not a list of "+str(numentries)+" vec3's (only has "+str(len(value))+" entries)")

            tmp=[]
            j=0
            for j in range(numentries):
                if type(value[j]) != vec3:
                    raise RuntimeError("Not a list of vec3's ("+str(type(value[j]))+")")
                    
                tmp.append(value[j].x)
                tmp.append(value[j].y)
                tmp.append(value[j].z)

            assert len(tmp) == 3*numentries
            tmp = array.array("f",tmp)
            
#            x = c_float(value.x)
#            y = c_float(value.y)
#            z = c_float(value.z)
            #print("glUniform3fv(",i,numentries,"...")
            glUniform3fv( i, numentries, tmp.tobytes() )
        return func
        
    def make_sampler2d_setter(self,i,unit):
        def func(value):
            if type(value) != Texture.Texture2D:
                raise RuntimeError("Not a Texture2D")
            glActiveTexture(GL_TEXTURE0+unit)
            glBindTexture(GL_TEXTURE_2D,value.tex)
            glUniform1i( i, unit )
        return func
        
    def setUniform(self, name, value ):
        #print("setUniform",name)
        if Program.active != self:
            raise RuntimeError("This is not active")
        if name in self.uniforms:
            func = self.uniforms[name]
            func(value)
    def use(self):
        glUseProgram(self.prog)
        Program.active = self
        
    def make_shader(self, filename, shadertype ):
        sdata = open(filename).read()
        s = glCreateShader( shadertype )
        glShaderSource( s,sdata )
        glCompileShader( s )
        infolog = glGetShaderInfoLog( s )
        if type(infolog) != str:
            infolog = infolog.decode()
        if len(infolog) > 0 :
            print("When compiling",shadertype,filename,":")
            print(infolog)
        i = GLint()
        glGetShaderiv( s, GL_COMPILE_STATUS, byref(i) )
        if not i.value:
            raise RuntimeError("Cannot compile "+filename)
            
        ins={}
        outs={}
        lst=sdata.split("\n")
        for line in lst:
            tmp=line.strip().split()
            if len(tmp) >= 3 and (tmp[0] == "in" or tmp[0] == "out"):
                vtype=tmp[1]
                vname=tmp[2].replace(";","")
                if tmp[0] == "in":
                    ins[vname]=vtype
                else:
                    outs[vname]=vtype


        return s,ins,outs
                        
    def match_variables(self,outs,ins):
        for k in ins:
            if k not in outs:
                raise RuntimeError("Variable "+k+" read but not written")
            else:
                if ins[k] != outs[k]:
                    raise RuntimeError("Variable "+k+" declared as "+ins[k]+" and "+outs[k])
        
