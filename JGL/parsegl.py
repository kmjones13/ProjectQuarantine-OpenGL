#!/usr/bin/env python3

import sys,re
import subprocess
import shutil
import os

quiet=False

def tprint(*args):
    if not quiet:
        print(" ".join([str(q) for q in args]))
        
def main():
        
    fp=open("glcorearb.h")
    dat=fp.read()


    m = re.search(r"(?s)/\*.*?\*/.*?/\*.*?\*/",dat)
    arbcopyright = m.group(0)
    

    #remove line continuations
    dat = re.sub(r"\\\n"," ",dat)

    #remove comments
    dat = re.sub(r"/\*.*?\*/","",dat)

    #change APIENTRY * to APIENTRYP
    dat = re.sub(r"\bAPIENTRY\s*\*","APIENTRYP ",dat)


    #skip initial stuff
    i = dat.find("#ifndef GL_VERSION_1_0")

    #key = alias, value = real type
    typedefs={
        "__int32":"int",
        "unsigned char":"byte",
        "unsigned int":"int",
        "__int64":"long",
        "ptrdiff_t":"int",      #depends on platform's sizeof pointer
        "unsigned __int64":"long",
        "GLuint64EXT":"long",
        "signed char":"byte",
        "unsigned short":"short",
        "void":"void",
        "GLsync":"long",
        "GLboolean":"boolean",
        "boolean":"boolean",
        "GLuint64":"long"
    }

    defines=[]

    #(funcname, return type, params, original prototype)
    functions = []


    #key=PFN name
    #value = (return type,params)
    ftypedefs={}

    definerex = re.compile(r"#define (\w+)\s+([^\n]+)")
    typedefrex = re.compile(r"typedef (.*)\s(\w+);")
    functypedefrex = re.compile(r"typedef (.*)\(APIENTRYP (\w+)\)\s*(.*);")
    funcrex = re.compile(r"GLAPI\b(.+)\bAPIENTRY\s+(\w+)(.*);")
    structtypedefrex = re.compile(r"typedef struct (.*)")
    emptydefine = re.compile(r"#define (\w+)$")

    for line in dat[i:].split("\n"):
        line=line.strip()
        
        if len(line) == 0:
            continue
            
        if line.startswith("#ifndef") or line.startswith("#ifdef ") or line.startswith("#endif") or line.startswith("#include"):
            continue
            
        if line.startswith("#if defined"):
            continue
        if line.startswith("#elif defined"):
            continue    
        if line.startswith("#else"):
            continue
            
        m=definerex.match(line)
        if m:
            k=m.group(1).strip()
            v=m.group(2).strip()
            if v.startswith("0x") and v.endswith("u"):
                v=v[:-1]
            if v.startswith("0x") and v.endswith("ull"):
                v=v[:-3]
                
            defines.append( ( k,v) )
            continue
            
        m = functypedefrex.match(line)
        if m:
            ftypedefs[m.group(2).strip()] = (m.group(1).strip(),m.group(3).strip() )
            continue
            
        m = typedefrex.match(line)
        if m:
            alias=m.group(2).strip()
            if alias not in typedefs:
                realtype = m.group(1).strip()
                while realtype in typedefs and typedefs[realtype] != realtype:
                    realtype = typedefs[realtype]
                typedefs[alias]=realtype
            continue
        
        m = funcrex.match(line)
        if m:
            functions.append( (m.group(2),m.group(1),m.group(3), line) )
            continue 
            
        m = structtypedefrex.match(line)
        if m:
            tprint("Ignoring",line)
            continue 
            
        m = emptydefine.match(line)
        if m:
            tprint("Ignoring",line)
            continue
            
        if line.startswith("struct "):
            tprint("Ignoring",line)
            continue 
            
        if line == "}":
            continue
            
        print(line)
        assert 0
            

#    print(typedefs)

    outfp = open("JGL.java","w")

    outfp.write("""
    package JGL;
    import java.io.*;
    import java.util.*;
    import java.nio.*;
    import JGLSDLUtils.JGLSDLUtils;
    """)
    
    outfp.write("//Code based on glcorearb.h, which has the following copyright:\n")
    outfp.write(arbcopyright)
    outfp.write("""
    
    public class JGL {
        
        static{
            JGLSDLUtils.extractAndLoad(JGL.class,"JGL");
        }

        private static class SA{
            byte[] B;
            int[] offsets;
        }
        
        private static SA arrayOfStringsToArrayOfBytes(String[] s){
            java.util.ArrayList<Byte> A = new java.util.ArrayList<>();
            int[] offsets = new int[s.length];
            for(int j=0;j<s.length;++j){
                offsets[j] = A.size();
                byte[] b = s[j].getBytes();
                for(int i=0;i<b.length;++i){
                    A.add(b[i]);
                }
                A.add((byte)0); //null terminator
            }
            
            byte[] B = new byte[A.size()];
            for(int i=0;i<A.size();++i){
                B[i]=A.get(i);
            }
            SA rv = new SA();
            rv.B=B;
            rv.offsets=offsets;
            return rv;
        }
        
        public static void glShaderSource(int shader, int count, String[] string, int[] length){
            SA tmp = arrayOfStringsToArrayOfBytes(string);
            my_glShaderSource(shader,count,tmp.B,tmp.offsets,length);
        }
        public static native void my_glShaderSource(int shader, int count,
            byte[] src, int[] offsets, int[] length);
        
        public static String glGetString(int name){
            byte[] b = my_glGetString(name);
            return new String(b);
        }
        private static native byte[] my_glGetString(int name);

        public static String glGetStringi(int name,int which){
            byte[] b = my_glGetStringi(name,which);
            return new String(b);
        }
        private static native byte[] my_glGetStringi(int name, int which);

        public static native void jglEnableTrace(boolean enabled);
        
        public static void glGetUniformIndices(int program, int uniformCount,
            String[] uniformNames, int[] uniformIndices){
            SA tmp = arrayOfStringsToArrayOfBytes(uniformNames);
            my_glGetUniformIndices(program,uniformCount,tmp.B, tmp.offsets, uniformIndices);
        }
        private static native void my_glGetUniformIndices(
            int program, int uniformcount, byte[] unames, int[] offsets, int[] indices);
            
        public static void glTransformFeedbackVaryings(int prog, int count, 
            String[] varyings, int buffermode){
            SA tmp = arrayOfStringsToArrayOfBytes(varyings);
            my_glTransformFeedbackVaryings(prog,count,tmp.B,tmp.offsets,buffermode);
        }
        private static native void my_glTransformFeedbackVaryings(int prog, int count,
            byte[] b, int[] offsets, int buffermode);
            
        //for testing undefined function handling
        public static native void glTestForUndefinedFunction();
        
        public interface DebugMessageCallback{
            public void debugCallback(int source, int type, int id, int severity, String message, Object param);
        }
        
        private static native void my_glDebugMessageCallback();
        private static native void my_glDebugMessageCallbackARB();
        private static DebugMessageCallback dmc;
        private static Object debugobj;
        public static void glDebugMessageCallback(DebugMessageCallback callback, Object param){
            dmc=callback;
            debugobj=param;
            my_glDebugMessageCallback();
        }
        public static void glDebugMessageCallbackARB(DebugMessageCallback callback, Object param){
            dmc=callback;
            debugobj=param;
            my_glDebugMessageCallbackARB();
        }

        public static int debugmessagecallbackhelper(int a, int b, int c, int d, int e, byte[] f){
            String s = new String(f);
            if(dmc != null)
                dmc.debugCallback(a,b,c,d,s,debugobj);
            //System.out.println("DEBUG: "+a+" "+b+" "+c+" "+d+" "+e+" "+s);
            return 0;
        }
        
        public static native ByteBuffer glMapBufferRange(int target, int offset, int length, int access);
        
        
    """)


        
        
    cfp = open("JGL.c","w")
    
    cfp.write("//Code based on glcorearb.h, which has the following copyright:\n")
    cfp.write(arbcopyright)


    cfp.write(r"""
    #include "JGL_JGL.h"
    #ifdef WIN32
    #include <windows.h>
    #else
    #include <dlfcn.h>
    #endif
    #include "glcorearb.h"
    #include <stdlib.h>
    #include <string.h>

    static int do_trace=0;
    
    //https://www.opengl.org/wiki/Load_OpenGL_Functions
    static void* mygetprocaddr(JNIEnv* env, const char* funcname){
        void* x;
        if( do_trace )
            fprintf(stderr,"trace: Looking up %s\n",funcname);

        #ifdef WIN32
            x = (void*)wglGetProcAddress(funcname);
            if( !x || x == (void*)1 || x == (void*)2 || x == (void*)3 || x == (void*)-1 ){
                if(do_trace)
                    fprintf(stderr,"trace: Did not find %s -- using GetProcAddress\n",funcname);
                HMODULE mod = LoadLibraryA("opengl32.dll");
                x = (void*) GetProcAddress(mod,funcname);
            }
        #else
            static void* gllib=0;
            if( !gllib ){
                gllib = dlopen("libGL.so",RTLD_LAZY);
                if(!gllib){
                    fprintf(stderr,"ERROR: Could not load GL library\n");
                    x=0;
                }
            }
            if( gllib )
                x = dlsym(gllib,funcname);
        #endif
        
        if( do_trace)
            fprintf(stderr,"trace: Lookup of %s: %p\n", funcname, x );
        if(!x){
            fprintf(stderr,"ERROR: Could not load function %s\n",funcname);
            jclass rec = (*env)->FindClass(env,"java/lang/RuntimeException");
            (*env)->ThrowNew(env, rec , "Could not load function");
            return 0;
        }
        return x;
    }

    JNIEXPORT void JNICALL Java_JGL_JGL_jglEnableTrace(JNIEnv* env, jclass klass, jboolean active){
        do_trace = active;
    }


    static void make_string_array( 
        JNIEnv* env,
        jbyteArray* ba, jintArray* offsets,
        jbyte** bap, jint** op, 
        GLchar*** stringarray ){
        
        if(do_trace)
            fprintf(stderr,"Making string array\n");
        
        jint* offsets_p = (*env)->GetIntArrayElements(env,*offsets,NULL);
        jbyte* b_p = (*env)->GetByteArrayElements(env,*ba,NULL);
        jsize len = (*env)->GetArrayLength(env,*offsets);

        GLchar** cp = (GLchar**) malloc( sizeof(GLchar*) * len );
        int i;
        for(i=0;i<len;++i){
            cp[i] = b_p + offsets_p[i];
        }
        
        *bap = b_p;
        *op = offsets_p;
        *stringarray = cp;
        
        if(do_trace)
            fprintf(stderr,"Done making string array\n");
    }
    
    static void free_string_array( 
        JNIEnv* env,
        jbyteArray* ba, jintArray* offsets,
        jbyte* bap, jint* offsetsp, 
        GLchar** stringarray ){
        
        if(do_trace)
            fprintf(stderr,"freeing string array\n");
        (*env)->ReleaseByteArrayElements( env, *ba, bap,0 );
        (*env)->ReleaseIntArrayElements( env, *offsets, offsetsp,0 );
        free(stringarray);
        if(do_trace)
            fprintf(stderr,"Freed string array\n");
    }
    
    //private static native void my_glGetUniformIndices(
    //int program, int uniformcount, byte[] unames, int[] offsets, int[] indices);
    JNIEXPORT void JNICALL Java_JGL_JGL_my_1glGetUniformIndices( JNIEnv* env, jclass klass,
        jint program, jint ucount, jbyteArray unames, jintArray offsets, jintArray indices)
    {
        static PFNGLGETUNIFORMINDICESPROC impl;
        if(!impl ){
            impl = (PFNGLGETUNIFORMINDICESPROC) mygetprocaddr(env,"glGetUniformIndices");
            if(!impl)
                return;
        }
        jbyte* p;
        jint* o;
        GLchar** sa;
        make_string_array(env,&unames,&offsets,&p,&o,&sa);
        const GLchar* const* sa1;
        memcpy(&sa1,&sa,sizeof(sa));    //gcc...
        jint* ind = (*env)->GetIntArrayElements(env,indices,NULL);
        impl(program,ucount,sa1,ind);
        free_string_array(env,&unames,&offsets,p,o,sa);
        (*env)->ReleaseIntArrayElements(env,indices,ind,0);
        return;
    }
    
    //my_glShaderSource(shader,count,tmp.B,tmp.offsets,length);
    JNIEXPORT void JNICALL Java_JGL_JGL_my_1glShaderSource(JNIEnv* env, jclass klass, 
        jint shader, jint count, jbyteArray srcblob, jintArray offsets, jintArray length){
        static PFNGLSHADERSOURCEPROC impl;

        if(!impl ){
            impl = (PFNGLSHADERSOURCEPROC) mygetprocaddr(env,"glShaderSource");
            if(!impl)
                return;
        }
        
        if(do_trace)
            fprintf(stderr,"In my_glShaderSource\n");
            
        jbyte* p;
        jint* o;
        GLchar** sa;
        make_string_array(env, &srcblob, &offsets, &p, &o, &sa );
        const GLchar* const * sa1;
        memcpy(&sa1,&sa,sizeof(sa));    //spurious gcc warning
        jint* len = ( length ? ((*env)->GetIntArrayElements(env,length,NULL) ) : NULL );
        impl(shader,count,sa1,len);
        if( len )
            (*env)->ReleaseIntArrayElements(env,length,len,0);
        free_string_array(env,&srcblob, &offsets, p,o,sa);
    }

    JNIEXPORT jbyteArray JNICALL Java_JGL_JGL_my_1glGetString(JNIEnv* env, jclass klass, jint name){
        static PFNGLGETSTRINGPROC impl;
        
        if( do_trace )
            fprintf(stderr,"trace: glGetString\n");
        
        if(!impl){
            impl = (PFNGLGETSTRINGPROC) mygetprocaddr(env,"glGetString");
            if(!impl)
                return NULL;
        }
        const char* x = impl(name);
        if( !x ){
            fprintf(stderr,"glGetString returned null\n");
            return 0;
        }
        int len = (int)strlen(x);
        jbyteArray b = (*env)->NewByteArray(env,len);
        (*env)->SetByteArrayRegion(env,b,0,len,(jbyte*)x);
        return b;
    }

    JNIEXPORT void JNICALL Java_JGL_JGL_glTestForUndefinedFunction(JNIEnv* env, jclass klass){
        static void* impl;
        
        if(!impl){
            impl = mygetprocaddr(env,"glTestForUndefinedFunction");
            if(!impl)
                return;
        }
        fprintf(stderr,"Somehow we got the undefined function: %p\n",impl);
        return;
    }


    JNIEXPORT jbyteArray JNICALL Java_JGL_JGL_my_1glGetStringi(JNIEnv* env, jclass klass, jint name, jint which){
        static PFNGLGETSTRINGIPROC impl;
        
        if( do_trace )
            fprintf(stderr,"trace: glGetStringi\n");
        
        if(!impl){
            impl = (PFNGLGETSTRINGIPROC) mygetprocaddr(env,"glGetStringi");
            if(!impl)
                return NULL;
        }
        const char* x = impl(name,which);
        if( !x ){
            fprintf(stderr,"glGetStringi returned null\n");
            return 0;
        }
        int len = (int)strlen(x);
        jbyteArray b = (*env)->NewByteArray(env,len);
        (*env)->SetByteArrayRegion(env,b,0,len,(jbyte*)x);
        return b;
    }

     
    JNIEXPORT void JNICALL Java_JGL_JGL_my1glTransformFeedbackVaryings(JNIEnv* env, jclass klass, 
        jint program, jint count, jbyteArray blob, jintArray offsets, jint buffermode){
     
        if( do_trace )
            fprintf(stderr,"trace: glTransformFeedbackVaryings\n");
        static PFNGLTRANSFORMFEEDBACKVARYINGSPROC impl;
        if(!impl){
            impl = (PFNGLTRANSFORMFEEDBACKVARYINGSPROC) mygetprocaddr(env,"glTransformFeedbackVaryings");
            if(!impl)
                return ;
        }
        jbyte* b;
        jint* o;
        GLchar** sa;
        make_string_array( env, &blob, &offsets, &b, &o, &sa );
        const GLchar* const * sa1;  //spurious gcc warnings...
        memcpy(&sa1,&sa,sizeof(sa));
        impl(program,count,sa1,buffermode);
        free_string_array(env, &blob, &offsets, b,o,sa );
    }

    static JavaVM* jvm;
    
    static void APIENTRY my_debug_proc(GLenum source, GLenum type, GLuint id,
        GLenum severity, GLsizei length, const GLchar* message, void* userparam)
    {
        //fprintf(stderr,"GL debug message: %d %d %d %d %s\n",source,type,id,severity,message);
        //fflush(stderr);
        if( jvm ){
            JNIEnv* env=0;
            (*jvm)->AttachCurrentThread(jvm, (void**)&env, NULL );
            if(env){
                jclass klass;
                klass = (*env)->FindClass(env,"JGL/JGL");
                if(klass){
                    jmethodID meth;
                    meth = (*env)->GetStaticMethodID(env,klass,"debugmessagecallbackhelper","(IIIII[B)I");
                    if(meth){
                        jbyteArray arr = (*env)->NewByteArray(env,length);
                        jobjectRefType rt = (*env)->GetObjectRefType(env,arr);
                        //printf("arr type: %d\n",rt);
                        (*env)->SetByteArrayRegion(env,arr,0,length,(char*)message);
                        (*env)->CallStaticIntMethod(env,klass,meth,
                            source,type,id,severity,length,arr);
                        if(rt == JNIGlobalRefType )
                            (*env)->DeleteGlobalRef(env,arr);
                    }
                    else{
                        fprintf(stderr,"No meth\n");
                    }
                }
                else{
                    fprintf(stderr,"No klass\n");
                }
            }
            else{
                fprintf(stderr,"No env\n");
            }
            (*jvm)->DetachCurrentThread(jvm);
        }
        else{
            fprintf(stderr,"no jvm\n");
        }
    }
    
    JNIEXPORT void JNICALL Java_JGL_JGL_my_1glDebugMessageCallback(JNIEnv* env, jclass klass){
        static PFNGLDEBUGMESSAGECALLBACKPROC impl;
        
        if( do_trace )
            fprintf(stderr,"trace: glDebugMessageCallback\n");

        if(!impl ){
            impl = (PFNGLDEBUGMESSAGECALLBACKPROC) mygetprocaddr(env,"glDebugMessageCallback");
            if(!impl)
                return;
        }
        if(!jvm )
            (*env)->GetJavaVM(env,&jvm);
        impl((GLDEBUGPROC)my_debug_proc,NULL);
    }

    JNIEXPORT void JNICALL Java_JGL_JGL_my_1glDebugMessageCallbackARB(JNIEnv* env, jclass klass){
        static PFNGLDEBUGMESSAGECALLBACKARBPROC impl;
        
        if( do_trace )
            fprintf(stderr,"trace: glDebugMessageCallback\n");

        if(!impl ){
            impl = (PFNGLDEBUGMESSAGECALLBACKARBPROC) mygetprocaddr(env,"glDebugMessageCallbackARB");
            if(!impl)
                return;
        }
        if(!jvm )
            (*env)->GetJavaVM(env,&jvm);
        impl((GLDEBUGPROCARB)my_debug_proc,NULL);
    }
    
    JNIEXPORT jobject JNICALL Java_JGL_JGL_glMapBufferRange(JNIEnv* env, jclass klass,
            jint target, jint offset, jint length, jint access){
        static PFNGLMAPBUFFERRANGEPROC impl;
        
        if( do_trace )
            fprintf(stderr,"trace: glmapbufferrange\n");

        if(!impl ){
            impl = (PFNGLMAPBUFFERRANGEPROC) mygetprocaddr(env,"glMapBufferRange");
            if(!impl)
                return 0;
        }
        void* p = impl(target,offset,length,access);
        if(!p)
            return 0;
        jobject bb = (*env)->NewDirectByteBuffer(env,p,length);
        return bb;
    }

    JNIEXPORT void JNICALL Java_JGL_JGL_glMultiDrawElements( JNIEnv* env, jclass klass,
        jint mode, jintArray count, jint type, jintArray indices, int drawcount){

        static PFNGLMULTIDRAWELEMENTSPROC impl;
        if(!impl ){
            impl = (PFNGLMULTIDRAWELEMENTSPROC) mygetprocaddr(env,"glMultiDrawElements");
            if(!impl)
                return;
        }
        jint* count_p = (*env)->GetIntArrayElements(env,count,NULL);
        jint* indices_p = (*env)->GetIntArrayElements(env,indices,NULL);
        
        impl(mode,count_p,type,(const void* const*)indices_p,drawcount);
        
        (*env)->ReleaseIntArrayElements(env,count,count_p,0);
        (*env)->ReleaseIntArrayElements(env,indices,indices_p,0);
    }
    """
    )
        
    for k,v in defines:
        if v == "0xFFFFFFFFFFFFFFFF":
            outfp.write("    public static final long "+k+" = -1;\n")
        else:
            outfp.write("    public static final int "+k+" = "+v+";\n")



    #these parameters will be forced to GLint
    forced_parameters = {
        "glDrawElements,indices":"GLint",
        "glDrawElementsInstanced,indices":"GLint",
        "glVertexAttribPointer,pointer":"GLint",
        "glVertexAttribLPointer,pointer":"GLint",
        "glDrawRangeElements,indices":"GLint",
        "glDrawElementsBaseVertex,indices":"GLint",
        "glDrawElementsInstancedBaseVertex,indices":"GLint",
        "glDrawElementsInstancedBaseInstance,indices":"GLint",
        "glDrawElementsInstancedBaseVertexBaseInstance,indices":"GLint",
        "glDrawRangeElementsBaseVertex,indices":"GLint",
        "glDrawArraysIndirect,indirect":"GLint",
        "glDrawElementsIndirect,indirect":"GLint",
        "glVertexAttribPointer,normalized":"GLboolean",
        "glIsSync,sync":"GLsync",
        "glDeleteSync,sync":"GLsync",
        "glClientWaitSync,sync":"GLsync",
        "glWaitSync,sync":"GLsync",
        "glGetSynciv,sync":"GLsync"
    }


    #the first few need to be done in a special way since we'll likely need them,
    #but they have signatures that are difficult to handle automatically
    blacklist=[


        #implemented manually
        "glGetString","glGetStringi","glShaderSource",
        "glTransformFeedbackVaryings",
        "glDebugMessageCallback", "glDebugMessageCallbackARB",
        "glMultiDrawElements", 
        "glMapBufferRange",
        "glMapBuffer",
        "glGetUniformIndices",
        
        #not implemented
        "glCreateShaderProgramv",
        "glMapNamedBuffer","glMapNamedBufferRange",
        "glGetPointerv",
        "glGetBufferPointerv","glGetVertexAttribPointerv",
        "glMultiDrawElementsBaseVertex",
        "glMultiDrawElementsIndirect",
        "glMultiDrawArraysIndirect",
        "glGetNamedBufferPointerv",
        "glCreateSyncFromCLeventARB",
        "glCompileShaderIncludeARB",
        
        #note that glBindBufferRange is OK.
        "glBindBuffersRange",        #depends on sizeof(pointer) on target platform
        "glBindVertexBuffers",      #depends on sizeof(ptrdiff) on target platform
        "glVertexArrayVertexBuffers",      #depends on sizeof(ptrdiff) on target platform
        
    ]
    blacklist=set(blacklist)


    #(funcname, return type, params, istypedef)
    paramrex = re.compile(r"^(.+?)(\w+)$")
    for funcname,return_type,params,origdefinition in functions:
        #print("--------->"+funcname+"<----")
        #print(funcname,return_type,params,isTypedef)
        if funcname in blacklist :
            #tprint("Blacklist:",funcname)
            continue
            
        #if funcname.endswith("ARB"):
            #print("ARB:",funcname)
        #    continue
            
        return_type = return_type.strip()
#        print(params)
        params=params.strip()
        if params[0] != '(' or params[-1] != ')':
            print(params)
            assert 0
        params = params[1:-1]
        if params == "void":
            params=[]
        else:
            params = params.split(",")
            params = [q.strip() for q in params]
            params = [paramrex.match(q) for q in params]
            #MSVC defines "near" and "far" as keywords, but GL uses them in its
            #parameter list.
            params = [ (q.group(1).strip(),q.group(2).strip()+"_") for q in params ]
        
        
        outfp.write("    //"+origdefinition.strip()+"\n")
        cfp.write("//"+origdefinition.strip()+"\n")
        
        if return_type not in typedefs:
            print(origdefinition)
            assert 0
        
        rettype = typedefs[return_type]
        outfp.write("    public static native "+rettype+" ")
        outfp.write(funcname)
        outfp.write("(")
        
        
        if rettype == "void":
            jnirettype="void"
        elif rettype in ["int","float","byte","long","boolean"]:
            jnirettype = "j"+rettype
        else:
            print("rettype=",rettype)
            print(funcname)
            print(origdefinition)
            assert 0

        cfp.write("JNIEXPORT ")
        cfp.write(jnirettype)
        cfp.write(" JNICALL Java_JGL_JGL_"+funcname+"( JNIEnv* env, jclass klass")
        
        
        ptypes=[]
        for i in range(len(params)):
            if i > 0:
                outfp.write(", ")
            q=params[i]
            cty = q[0]
            nam = q[1]

            key = funcname+","+nam[:-1]
            #print(key)
            if key in forced_parameters:
                need_cast=True
                ty = forced_parameters[key]
                del forced_parameters[key]
            else:
                need_cast=False
                ty=cty
                ty=ty.replace("const","").strip()
            
            
            if ty.endswith("*"): 
                ty = ty[:-1].strip()
    #            if ty[-1] == '*':
    #                print("Double pointer")
    #                print(funcname)
    #                print(params)
    #                print(ty)
    #                assert 0
     
                if ty == "GLint" or ty == "GLuint" or ty == "GLsizei" or ty == "GLenum":
                    ty = "int[]" #"java.nio.IntBuffer"
                elif ty == "GLfloat":
                    ty = "float[]" #"java.nio.FloatBuffer"
                elif ty == "void":
                    ty = "byte[]" #"java.nio.Buffer"
                elif ty == "GLboolean":
                    ty = "byte[]" #"java.nio.ByteBuffer"
                elif ty == "GLdouble":
                    ty = "double[]" #"java.nio.DoubleBuffer"
                elif ty == "GLchar":
                    ty = "byte[]" #use String.getBytes() #"java.nio.CharBuffer"
                elif ty == "GLshort" or ty == "GLushort":
                    ty = "short[]" #"java.nio.ShortBuffer"
                elif ty == "GLbyte" or ty == "GLubyte":
                    ty = "byte[]" #"java.nio.ByteBuffer"
                elif ty == "GLint64" or ty == "GLuint64" or ty == "GLuint64EXT":
                    ty = "long[]" #"java.nio.LongBuffer"
                elif ty == "GLintptr" or ty == "GLsizeiptr":
                    ty = "int[]"
    #            elif origty == "constGLchar*const*":
    #                #it's an immutable list of immutable strings
    #                #This is a special case below...
    #                ty = "String[]"     #how it comes into the Java code
                else:
                    print("parameter type:",ty)
                    print(params)
                    print(funcname)
                    print(origdefinition)
                    assert 0
            else:
                if ty not in typedefs:
                    print("Parameter type not known")
                    print("ty="+"--->"+ty+"<---")
                    print(origdefinition)
                    print(typedefs)
                    assert 0
                    
                ty = typedefs[ty]
                if ty == "~POINTER":
                    ty = "byte[]" #"java.nio.Buffer"      #Buffer
            outfp.write(ty+" "+nam)
            ptypes.append( (ty,nam,need_cast,cty) )
            
            cfp.write(",")
            if ty in ["int","float","double","byte","short","long","boolean"]:
                cfp.write("j"+ty+" ")
            elif ty == "String[]":
                cfp.write("jobjectArray")   #it will come to C as an array of *byte* arrays
            elif ty.find("[]") != -1:
                cfp.write("j"+ty[:-2]+"Array ")
            else:
                print(funcname)
                print(nam)
                print(ty)
                assert 0
                
            cfp.write(nam)
            
        outfp.write(");\n")
        
        #now go back and redo the Java function if we have any char arrays: Allow the use
        #of String's. Note that this doesn't work for arrays of strings; those are a 
        #special case and were handled above
        for ty,nam,need_cast,cty in ptypes:
            #this only works with immutable char arrays.
            #if we have mutable arrays, we can't modify the input String,
            #so that doesn't work...
            #print(funcname,ty,nam)
            if cty.replace(" ","") == "constGLchar*":
                #print("String alias:",funcname)
                outfp.write("    public static "+rettype+" ")
                outfp.write(funcname)
                outfp.write("(")
                flag=False
                for ty,nam,need_cast,cty in ptypes:
                    if flag:
                        outfp.write(",")
                    flag=True
                    if cty.replace(" ","") =="constGLchar*":
                        outfp.write("String "+nam)
                    else:
                        outfp.write(ty+" "+nam)
                outfp.write("){\n")
                if rettype != "void":
                    outfp.write("        "+rettype+" rv;\n")
                    outfp.write("        rv = ")
                else:
                    outfp.write("        ")
                outfp.write(funcname+"(")
                flag=False
                for ty,nam,need_cast,cty in ptypes:
                    if flag:
                        outfp.write(",")
                    flag=True
                    if cty.replace(" ","") =="constGLchar*":
                        outfp.write( '('+nam+'+"\\0").getBytes()')
                    else:
                        outfp.write(nam)
                outfp.write(");\n")
                if rettype != "void":
                    outfp.write("        return rv;\n")
                    
                outfp.write("    }\n")
                break
                
                    

                       
        
        cfp.write("){\n")
        if jnirettype != "void":
            cfp.write("    "+jnirettype+" rv = 0;\n")
            
        cfp.write("    if(do_trace)\n")
        cfp.write('        fprintf(stderr,"'+funcname+'\\n");\n')
        
        PFN = "PFN"+funcname.upper()+"PROC"
        if PFN in ftypedefs:
            cfp.write("    static "+PFN+" impl;\n")
            cfp.write("    if( !impl ){\n")
            cfp.write("       impl = ("+PFN+") mygetprocaddr(env,\""+funcname+"\");\n")
            cfp.write("       if(!impl)\n")
            if jnirettype != "void":
                cfp.write("            return rv;\n")
            else:
                cfp.write("            return;\n")
            cfp.write("    }\n")
            
            
            for typ,nam,need_cast,cty in ptypes:
    #            if origtyp == "constGLchar*const*":
    #                #the thing that's coming in is an array of Strings
                if typ.find("[]") != -1:
                    tmp=typ[:-2]
                    tmp = tmp[0].upper()+tmp[1:]
                    cfp.write("    j"+typ[:-2]+"* _"+nam+
                        " = ("+nam+" ? ((*env)->Get"+tmp+"ArrayElements(env,"+nam+",NULL) ) : NULL);\n")
                        
            
            cfp.write("    ")
            if jnirettype != "void":
                cfp.write("rv = ")
                
            cfp.write("impl(")
            
            flag=False
            for typ,nam,need_cast,cty in ptypes:
                if flag:
                    cfp.write(", ")
                flag=True
                if typ.find("[]") == -1:
                    if need_cast:
                        #print("needed cast: typ=",typ,"nam=",nam,"func=",funcname)
                        cfp.write("("+cty+")") #void*)")
                    #elif origtyp == "GLsync":
                    #    cfp.write("(GLsync)")
                    cfp.write(nam)
                else:
                    assert need_cast == False
                    cfp.write("_"+nam)
                    
            cfp.write(");\n")
            
            for typ,nam,need_cast,cty in ptypes:
                if typ.find("[]") != -1:
                    tmp=typ[:-2]
                    tmp=tmp[0].upper()+tmp[1:]
                    cfp.write("    if("+nam+")\n")
                    cfp.write("        (*env)->Release"+tmp+
                        "ArrayElements(env,"+nam+", _"+nam+",0);\n")
            
            if jnirettype != "void":
                cfp.write("    return rv;\n")
            
            cfp.write("}\n")
                    
        else:
            assert 0
        
        
        
    outfp.write("}\n")
    outfp.close()

    if len(forced_parameters) > 0:
        print(forced_parameters)
        assert 0

if __name__=='__main__':
    main()
