//Code based on glcorearb.h, which has the following copyright:
/*
** Copyright (c) 2013-2014 The Khronos Group Inc.
**
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and/or associated documentation files (the
** "Materials"), to deal in the Materials without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Materials, and to
** permit persons to whom the Materials are furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be included
** in all copies or substantial portions of the Materials.
**
** THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
*/
/*
** This header is generated from the Khronos OpenGL / OpenGL ES XML
** API Registry. The current version of the Registry, generator scripts
** used to make the header, and the header can be found at
**   http://www.opengl.org/registry/
**
** Khronos $Revision: 27684 $ on $Date: 2014-08-11 01:21:35 -0700 (Mon, 11 Aug 2014) $
*/
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
    //GLAPI void APIENTRY glCullFace (GLenum mode);
JNIEXPORT void JNICALL Java_JGL_JGL_glCullFace( JNIEnv* env, jclass klass,jint mode_){
    if(do_trace)
        fprintf(stderr,"glCullFace\n");
    static PFNGLCULLFACEPROC impl;
    if( !impl ){
       impl = (PFNGLCULLFACEPROC) mygetprocaddr(env,"glCullFace");
       if(!impl)
            return;
    }
    impl(mode_);
}
//GLAPI void APIENTRY glFrontFace (GLenum mode);
JNIEXPORT void JNICALL Java_JGL_JGL_glFrontFace( JNIEnv* env, jclass klass,jint mode_){
    if(do_trace)
        fprintf(stderr,"glFrontFace\n");
    static PFNGLFRONTFACEPROC impl;
    if( !impl ){
       impl = (PFNGLFRONTFACEPROC) mygetprocaddr(env,"glFrontFace");
       if(!impl)
            return;
    }
    impl(mode_);
}
//GLAPI void APIENTRY glHint (GLenum target, GLenum mode);
JNIEXPORT void JNICALL Java_JGL_JGL_glHint( JNIEnv* env, jclass klass,jint target_,jint mode_){
    if(do_trace)
        fprintf(stderr,"glHint\n");
    static PFNGLHINTPROC impl;
    if( !impl ){
       impl = (PFNGLHINTPROC) mygetprocaddr(env,"glHint");
       if(!impl)
            return;
    }
    impl(target_, mode_);
}
//GLAPI void APIENTRY glLineWidth (GLfloat width);
JNIEXPORT void JNICALL Java_JGL_JGL_glLineWidth( JNIEnv* env, jclass klass,jfloat width_){
    if(do_trace)
        fprintf(stderr,"glLineWidth\n");
    static PFNGLLINEWIDTHPROC impl;
    if( !impl ){
       impl = (PFNGLLINEWIDTHPROC) mygetprocaddr(env,"glLineWidth");
       if(!impl)
            return;
    }
    impl(width_);
}
//GLAPI void APIENTRY glPointSize (GLfloat size);
JNIEXPORT void JNICALL Java_JGL_JGL_glPointSize( JNIEnv* env, jclass klass,jfloat size_){
    if(do_trace)
        fprintf(stderr,"glPointSize\n");
    static PFNGLPOINTSIZEPROC impl;
    if( !impl ){
       impl = (PFNGLPOINTSIZEPROC) mygetprocaddr(env,"glPointSize");
       if(!impl)
            return;
    }
    impl(size_);
}
//GLAPI void APIENTRY glPolygonMode (GLenum face, GLenum mode);
JNIEXPORT void JNICALL Java_JGL_JGL_glPolygonMode( JNIEnv* env, jclass klass,jint face_,jint mode_){
    if(do_trace)
        fprintf(stderr,"glPolygonMode\n");
    static PFNGLPOLYGONMODEPROC impl;
    if( !impl ){
       impl = (PFNGLPOLYGONMODEPROC) mygetprocaddr(env,"glPolygonMode");
       if(!impl)
            return;
    }
    impl(face_, mode_);
}
//GLAPI void APIENTRY glScissor (GLint x, GLint y, GLsizei width, GLsizei height);
JNIEXPORT void JNICALL Java_JGL_JGL_glScissor( JNIEnv* env, jclass klass,jint x_,jint y_,jint width_,jint height_){
    if(do_trace)
        fprintf(stderr,"glScissor\n");
    static PFNGLSCISSORPROC impl;
    if( !impl ){
       impl = (PFNGLSCISSORPROC) mygetprocaddr(env,"glScissor");
       if(!impl)
            return;
    }
    impl(x_, y_, width_, height_);
}
//GLAPI void APIENTRY glTexParameterf (GLenum target, GLenum pname, GLfloat param);
JNIEXPORT void JNICALL Java_JGL_JGL_glTexParameterf( JNIEnv* env, jclass klass,jint target_,jint pname_,jfloat param_){
    if(do_trace)
        fprintf(stderr,"glTexParameterf\n");
    static PFNGLTEXPARAMETERFPROC impl;
    if( !impl ){
       impl = (PFNGLTEXPARAMETERFPROC) mygetprocaddr(env,"glTexParameterf");
       if(!impl)
            return;
    }
    impl(target_, pname_, param_);
}
//GLAPI void APIENTRY glTexParameterfv (GLenum target, GLenum pname, const GLfloat *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glTexParameterfv( JNIEnv* env, jclass klass,jint target_,jint pname_,jfloatArray params_){
    if(do_trace)
        fprintf(stderr,"glTexParameterfv\n");
    static PFNGLTEXPARAMETERFVPROC impl;
    if( !impl ){
       impl = (PFNGLTEXPARAMETERFVPROC) mygetprocaddr(env,"glTexParameterfv");
       if(!impl)
            return;
    }
    jfloat* _params_ = (params_ ? ((*env)->GetFloatArrayElements(env,params_,NULL) ) : NULL);
    impl(target_, pname_, _params_);
    if(params_)
        (*env)->ReleaseFloatArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glTexParameteri (GLenum target, GLenum pname, GLint param);
JNIEXPORT void JNICALL Java_JGL_JGL_glTexParameteri( JNIEnv* env, jclass klass,jint target_,jint pname_,jint param_){
    if(do_trace)
        fprintf(stderr,"glTexParameteri\n");
    static PFNGLTEXPARAMETERIPROC impl;
    if( !impl ){
       impl = (PFNGLTEXPARAMETERIPROC) mygetprocaddr(env,"glTexParameteri");
       if(!impl)
            return;
    }
    impl(target_, pname_, param_);
}
//GLAPI void APIENTRY glTexParameteriv (GLenum target, GLenum pname, const GLint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glTexParameteriv( JNIEnv* env, jclass klass,jint target_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glTexParameteriv\n");
    static PFNGLTEXPARAMETERIVPROC impl;
    if( !impl ){
       impl = (PFNGLTEXPARAMETERIVPROC) mygetprocaddr(env,"glTexParameteriv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(target_, pname_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glTexImage1D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void *pixels);
JNIEXPORT void JNICALL Java_JGL_JGL_glTexImage1D( JNIEnv* env, jclass klass,jint target_,jint level_,jint internalformat_,jint width_,jint border_,jint format_,jint type_,jbyteArray pixels_){
    if(do_trace)
        fprintf(stderr,"glTexImage1D\n");
    static PFNGLTEXIMAGE1DPROC impl;
    if( !impl ){
       impl = (PFNGLTEXIMAGE1DPROC) mygetprocaddr(env,"glTexImage1D");
       if(!impl)
            return;
    }
    jbyte* _pixels_ = (pixels_ ? ((*env)->GetByteArrayElements(env,pixels_,NULL) ) : NULL);
    impl(target_, level_, internalformat_, width_, border_, format_, type_, _pixels_);
    if(pixels_)
        (*env)->ReleaseByteArrayElements(env,pixels_, _pixels_,0);
}
//GLAPI void APIENTRY glTexImage2D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);
JNIEXPORT void JNICALL Java_JGL_JGL_glTexImage2D( JNIEnv* env, jclass klass,jint target_,jint level_,jint internalformat_,jint width_,jint height_,jint border_,jint format_,jint type_,jbyteArray pixels_){
    if(do_trace)
        fprintf(stderr,"glTexImage2D\n");
    static PFNGLTEXIMAGE2DPROC impl;
    if( !impl ){
       impl = (PFNGLTEXIMAGE2DPROC) mygetprocaddr(env,"glTexImage2D");
       if(!impl)
            return;
    }
    jbyte* _pixels_ = (pixels_ ? ((*env)->GetByteArrayElements(env,pixels_,NULL) ) : NULL);
    impl(target_, level_, internalformat_, width_, height_, border_, format_, type_, _pixels_);
    if(pixels_)
        (*env)->ReleaseByteArrayElements(env,pixels_, _pixels_,0);
}
//GLAPI void APIENTRY glDrawBuffer (GLenum buf);
JNIEXPORT void JNICALL Java_JGL_JGL_glDrawBuffer( JNIEnv* env, jclass klass,jint buf_){
    if(do_trace)
        fprintf(stderr,"glDrawBuffer\n");
    static PFNGLDRAWBUFFERPROC impl;
    if( !impl ){
       impl = (PFNGLDRAWBUFFERPROC) mygetprocaddr(env,"glDrawBuffer");
       if(!impl)
            return;
    }
    impl(buf_);
}
//GLAPI void APIENTRY glClear (GLbitfield mask);
JNIEXPORT void JNICALL Java_JGL_JGL_glClear( JNIEnv* env, jclass klass,jint mask_){
    if(do_trace)
        fprintf(stderr,"glClear\n");
    static PFNGLCLEARPROC impl;
    if( !impl ){
       impl = (PFNGLCLEARPROC) mygetprocaddr(env,"glClear");
       if(!impl)
            return;
    }
    impl(mask_);
}
//GLAPI void APIENTRY glClearColor (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
JNIEXPORT void JNICALL Java_JGL_JGL_glClearColor( JNIEnv* env, jclass klass,jfloat red_,jfloat green_,jfloat blue_,jfloat alpha_){
    if(do_trace)
        fprintf(stderr,"glClearColor\n");
    static PFNGLCLEARCOLORPROC impl;
    if( !impl ){
       impl = (PFNGLCLEARCOLORPROC) mygetprocaddr(env,"glClearColor");
       if(!impl)
            return;
    }
    impl(red_, green_, blue_, alpha_);
}
//GLAPI void APIENTRY glClearStencil (GLint s);
JNIEXPORT void JNICALL Java_JGL_JGL_glClearStencil( JNIEnv* env, jclass klass,jint s_){
    if(do_trace)
        fprintf(stderr,"glClearStencil\n");
    static PFNGLCLEARSTENCILPROC impl;
    if( !impl ){
       impl = (PFNGLCLEARSTENCILPROC) mygetprocaddr(env,"glClearStencil");
       if(!impl)
            return;
    }
    impl(s_);
}
//GLAPI void APIENTRY glClearDepth (GLdouble depth);
JNIEXPORT void JNICALL Java_JGL_JGL_glClearDepth( JNIEnv* env, jclass klass,jdouble depth_){
    if(do_trace)
        fprintf(stderr,"glClearDepth\n");
    static PFNGLCLEARDEPTHPROC impl;
    if( !impl ){
       impl = (PFNGLCLEARDEPTHPROC) mygetprocaddr(env,"glClearDepth");
       if(!impl)
            return;
    }
    impl(depth_);
}
//GLAPI void APIENTRY glStencilMask (GLuint mask);
JNIEXPORT void JNICALL Java_JGL_JGL_glStencilMask( JNIEnv* env, jclass klass,jint mask_){
    if(do_trace)
        fprintf(stderr,"glStencilMask\n");
    static PFNGLSTENCILMASKPROC impl;
    if( !impl ){
       impl = (PFNGLSTENCILMASKPROC) mygetprocaddr(env,"glStencilMask");
       if(!impl)
            return;
    }
    impl(mask_);
}
//GLAPI void APIENTRY glColorMask (GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
JNIEXPORT void JNICALL Java_JGL_JGL_glColorMask( JNIEnv* env, jclass klass,jboolean red_,jboolean green_,jboolean blue_,jboolean alpha_){
    if(do_trace)
        fprintf(stderr,"glColorMask\n");
    static PFNGLCOLORMASKPROC impl;
    if( !impl ){
       impl = (PFNGLCOLORMASKPROC) mygetprocaddr(env,"glColorMask");
       if(!impl)
            return;
    }
    impl(red_, green_, blue_, alpha_);
}
//GLAPI void APIENTRY glDepthMask (GLboolean flag);
JNIEXPORT void JNICALL Java_JGL_JGL_glDepthMask( JNIEnv* env, jclass klass,jboolean flag_){
    if(do_trace)
        fprintf(stderr,"glDepthMask\n");
    static PFNGLDEPTHMASKPROC impl;
    if( !impl ){
       impl = (PFNGLDEPTHMASKPROC) mygetprocaddr(env,"glDepthMask");
       if(!impl)
            return;
    }
    impl(flag_);
}
//GLAPI void APIENTRY glDisable (GLenum cap);
JNIEXPORT void JNICALL Java_JGL_JGL_glDisable( JNIEnv* env, jclass klass,jint cap_){
    if(do_trace)
        fprintf(stderr,"glDisable\n");
    static PFNGLDISABLEPROC impl;
    if( !impl ){
       impl = (PFNGLDISABLEPROC) mygetprocaddr(env,"glDisable");
       if(!impl)
            return;
    }
    impl(cap_);
}
//GLAPI void APIENTRY glEnable (GLenum cap);
JNIEXPORT void JNICALL Java_JGL_JGL_glEnable( JNIEnv* env, jclass klass,jint cap_){
    if(do_trace)
        fprintf(stderr,"glEnable\n");
    static PFNGLENABLEPROC impl;
    if( !impl ){
       impl = (PFNGLENABLEPROC) mygetprocaddr(env,"glEnable");
       if(!impl)
            return;
    }
    impl(cap_);
}
//GLAPI void APIENTRY glFinish (void);
JNIEXPORT void JNICALL Java_JGL_JGL_glFinish( JNIEnv* env, jclass klass){
    if(do_trace)
        fprintf(stderr,"glFinish\n");
    static PFNGLFINISHPROC impl;
    if( !impl ){
       impl = (PFNGLFINISHPROC) mygetprocaddr(env,"glFinish");
       if(!impl)
            return;
    }
    impl();
}
//GLAPI void APIENTRY glFlush (void);
JNIEXPORT void JNICALL Java_JGL_JGL_glFlush( JNIEnv* env, jclass klass){
    if(do_trace)
        fprintf(stderr,"glFlush\n");
    static PFNGLFLUSHPROC impl;
    if( !impl ){
       impl = (PFNGLFLUSHPROC) mygetprocaddr(env,"glFlush");
       if(!impl)
            return;
    }
    impl();
}
//GLAPI void APIENTRY glBlendFunc (GLenum sfactor, GLenum dfactor);
JNIEXPORT void JNICALL Java_JGL_JGL_glBlendFunc( JNIEnv* env, jclass klass,jint sfactor_,jint dfactor_){
    if(do_trace)
        fprintf(stderr,"glBlendFunc\n");
    static PFNGLBLENDFUNCPROC impl;
    if( !impl ){
       impl = (PFNGLBLENDFUNCPROC) mygetprocaddr(env,"glBlendFunc");
       if(!impl)
            return;
    }
    impl(sfactor_, dfactor_);
}
//GLAPI void APIENTRY glLogicOp (GLenum opcode);
JNIEXPORT void JNICALL Java_JGL_JGL_glLogicOp( JNIEnv* env, jclass klass,jint opcode_){
    if(do_trace)
        fprintf(stderr,"glLogicOp\n");
    static PFNGLLOGICOPPROC impl;
    if( !impl ){
       impl = (PFNGLLOGICOPPROC) mygetprocaddr(env,"glLogicOp");
       if(!impl)
            return;
    }
    impl(opcode_);
}
//GLAPI void APIENTRY glStencilFunc (GLenum func, GLint ref, GLuint mask);
JNIEXPORT void JNICALL Java_JGL_JGL_glStencilFunc( JNIEnv* env, jclass klass,jint func_,jint ref_,jint mask_){
    if(do_trace)
        fprintf(stderr,"glStencilFunc\n");
    static PFNGLSTENCILFUNCPROC impl;
    if( !impl ){
       impl = (PFNGLSTENCILFUNCPROC) mygetprocaddr(env,"glStencilFunc");
       if(!impl)
            return;
    }
    impl(func_, ref_, mask_);
}
//GLAPI void APIENTRY glStencilOp (GLenum fail, GLenum zfail, GLenum zpass);
JNIEXPORT void JNICALL Java_JGL_JGL_glStencilOp( JNIEnv* env, jclass klass,jint fail_,jint zfail_,jint zpass_){
    if(do_trace)
        fprintf(stderr,"glStencilOp\n");
    static PFNGLSTENCILOPPROC impl;
    if( !impl ){
       impl = (PFNGLSTENCILOPPROC) mygetprocaddr(env,"glStencilOp");
       if(!impl)
            return;
    }
    impl(fail_, zfail_, zpass_);
}
//GLAPI void APIENTRY glDepthFunc (GLenum func);
JNIEXPORT void JNICALL Java_JGL_JGL_glDepthFunc( JNIEnv* env, jclass klass,jint func_){
    if(do_trace)
        fprintf(stderr,"glDepthFunc\n");
    static PFNGLDEPTHFUNCPROC impl;
    if( !impl ){
       impl = (PFNGLDEPTHFUNCPROC) mygetprocaddr(env,"glDepthFunc");
       if(!impl)
            return;
    }
    impl(func_);
}
//GLAPI void APIENTRY glPixelStoref (GLenum pname, GLfloat param);
JNIEXPORT void JNICALL Java_JGL_JGL_glPixelStoref( JNIEnv* env, jclass klass,jint pname_,jfloat param_){
    if(do_trace)
        fprintf(stderr,"glPixelStoref\n");
    static PFNGLPIXELSTOREFPROC impl;
    if( !impl ){
       impl = (PFNGLPIXELSTOREFPROC) mygetprocaddr(env,"glPixelStoref");
       if(!impl)
            return;
    }
    impl(pname_, param_);
}
//GLAPI void APIENTRY glPixelStorei (GLenum pname, GLint param);
JNIEXPORT void JNICALL Java_JGL_JGL_glPixelStorei( JNIEnv* env, jclass klass,jint pname_,jint param_){
    if(do_trace)
        fprintf(stderr,"glPixelStorei\n");
    static PFNGLPIXELSTOREIPROC impl;
    if( !impl ){
       impl = (PFNGLPIXELSTOREIPROC) mygetprocaddr(env,"glPixelStorei");
       if(!impl)
            return;
    }
    impl(pname_, param_);
}
//GLAPI void APIENTRY glReadBuffer (GLenum src);
JNIEXPORT void JNICALL Java_JGL_JGL_glReadBuffer( JNIEnv* env, jclass klass,jint src_){
    if(do_trace)
        fprintf(stderr,"glReadBuffer\n");
    static PFNGLREADBUFFERPROC impl;
    if( !impl ){
       impl = (PFNGLREADBUFFERPROC) mygetprocaddr(env,"glReadBuffer");
       if(!impl)
            return;
    }
    impl(src_);
}
//GLAPI void APIENTRY glReadPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *pixels);
JNIEXPORT void JNICALL Java_JGL_JGL_glReadPixels( JNIEnv* env, jclass klass,jint x_,jint y_,jint width_,jint height_,jint format_,jint type_,jbyteArray pixels_){
    if(do_trace)
        fprintf(stderr,"glReadPixels\n");
    static PFNGLREADPIXELSPROC impl;
    if( !impl ){
       impl = (PFNGLREADPIXELSPROC) mygetprocaddr(env,"glReadPixels");
       if(!impl)
            return;
    }
    jbyte* _pixels_ = (pixels_ ? ((*env)->GetByteArrayElements(env,pixels_,NULL) ) : NULL);
    impl(x_, y_, width_, height_, format_, type_, _pixels_);
    if(pixels_)
        (*env)->ReleaseByteArrayElements(env,pixels_, _pixels_,0);
}
//GLAPI void APIENTRY glGetBooleanv (GLenum pname, GLboolean *data);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetBooleanv( JNIEnv* env, jclass klass,jint pname_,jbyteArray data_){
    if(do_trace)
        fprintf(stderr,"glGetBooleanv\n");
    static PFNGLGETBOOLEANVPROC impl;
    if( !impl ){
       impl = (PFNGLGETBOOLEANVPROC) mygetprocaddr(env,"glGetBooleanv");
       if(!impl)
            return;
    }
    jbyte* _data_ = (data_ ? ((*env)->GetByteArrayElements(env,data_,NULL) ) : NULL);
    impl(pname_, _data_);
    if(data_)
        (*env)->ReleaseByteArrayElements(env,data_, _data_,0);
}
//GLAPI void APIENTRY glGetDoublev (GLenum pname, GLdouble *data);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetDoublev( JNIEnv* env, jclass klass,jint pname_,jdoubleArray data_){
    if(do_trace)
        fprintf(stderr,"glGetDoublev\n");
    static PFNGLGETDOUBLEVPROC impl;
    if( !impl ){
       impl = (PFNGLGETDOUBLEVPROC) mygetprocaddr(env,"glGetDoublev");
       if(!impl)
            return;
    }
    jdouble* _data_ = (data_ ? ((*env)->GetDoubleArrayElements(env,data_,NULL) ) : NULL);
    impl(pname_, _data_);
    if(data_)
        (*env)->ReleaseDoubleArrayElements(env,data_, _data_,0);
}
//GLAPI GLenum APIENTRY glGetError (void);
JNIEXPORT jint JNICALL Java_JGL_JGL_glGetError( JNIEnv* env, jclass klass){
    jint rv = 0;
    if(do_trace)
        fprintf(stderr,"glGetError\n");
    static PFNGLGETERRORPROC impl;
    if( !impl ){
       impl = (PFNGLGETERRORPROC) mygetprocaddr(env,"glGetError");
       if(!impl)
            return rv;
    }
    rv = impl();
    return rv;
}
//GLAPI void APIENTRY glGetFloatv (GLenum pname, GLfloat *data);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetFloatv( JNIEnv* env, jclass klass,jint pname_,jfloatArray data_){
    if(do_trace)
        fprintf(stderr,"glGetFloatv\n");
    static PFNGLGETFLOATVPROC impl;
    if( !impl ){
       impl = (PFNGLGETFLOATVPROC) mygetprocaddr(env,"glGetFloatv");
       if(!impl)
            return;
    }
    jfloat* _data_ = (data_ ? ((*env)->GetFloatArrayElements(env,data_,NULL) ) : NULL);
    impl(pname_, _data_);
    if(data_)
        (*env)->ReleaseFloatArrayElements(env,data_, _data_,0);
}
//GLAPI void APIENTRY glGetIntegerv (GLenum pname, GLint *data);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetIntegerv( JNIEnv* env, jclass klass,jint pname_,jintArray data_){
    if(do_trace)
        fprintf(stderr,"glGetIntegerv\n");
    static PFNGLGETINTEGERVPROC impl;
    if( !impl ){
       impl = (PFNGLGETINTEGERVPROC) mygetprocaddr(env,"glGetIntegerv");
       if(!impl)
            return;
    }
    jint* _data_ = (data_ ? ((*env)->GetIntArrayElements(env,data_,NULL) ) : NULL);
    impl(pname_, _data_);
    if(data_)
        (*env)->ReleaseIntArrayElements(env,data_, _data_,0);
}
//GLAPI void APIENTRY glGetTexImage (GLenum target, GLint level, GLenum format, GLenum type, void *pixels);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetTexImage( JNIEnv* env, jclass klass,jint target_,jint level_,jint format_,jint type_,jbyteArray pixels_){
    if(do_trace)
        fprintf(stderr,"glGetTexImage\n");
    static PFNGLGETTEXIMAGEPROC impl;
    if( !impl ){
       impl = (PFNGLGETTEXIMAGEPROC) mygetprocaddr(env,"glGetTexImage");
       if(!impl)
            return;
    }
    jbyte* _pixels_ = (pixels_ ? ((*env)->GetByteArrayElements(env,pixels_,NULL) ) : NULL);
    impl(target_, level_, format_, type_, _pixels_);
    if(pixels_)
        (*env)->ReleaseByteArrayElements(env,pixels_, _pixels_,0);
}
//GLAPI void APIENTRY glGetTexParameterfv (GLenum target, GLenum pname, GLfloat *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetTexParameterfv( JNIEnv* env, jclass klass,jint target_,jint pname_,jfloatArray params_){
    if(do_trace)
        fprintf(stderr,"glGetTexParameterfv\n");
    static PFNGLGETTEXPARAMETERFVPROC impl;
    if( !impl ){
       impl = (PFNGLGETTEXPARAMETERFVPROC) mygetprocaddr(env,"glGetTexParameterfv");
       if(!impl)
            return;
    }
    jfloat* _params_ = (params_ ? ((*env)->GetFloatArrayElements(env,params_,NULL) ) : NULL);
    impl(target_, pname_, _params_);
    if(params_)
        (*env)->ReleaseFloatArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glGetTexParameteriv (GLenum target, GLenum pname, GLint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetTexParameteriv( JNIEnv* env, jclass klass,jint target_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetTexParameteriv\n");
    static PFNGLGETTEXPARAMETERIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETTEXPARAMETERIVPROC) mygetprocaddr(env,"glGetTexParameteriv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(target_, pname_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glGetTexLevelParameterfv (GLenum target, GLint level, GLenum pname, GLfloat *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetTexLevelParameterfv( JNIEnv* env, jclass klass,jint target_,jint level_,jint pname_,jfloatArray params_){
    if(do_trace)
        fprintf(stderr,"glGetTexLevelParameterfv\n");
    static PFNGLGETTEXLEVELPARAMETERFVPROC impl;
    if( !impl ){
       impl = (PFNGLGETTEXLEVELPARAMETERFVPROC) mygetprocaddr(env,"glGetTexLevelParameterfv");
       if(!impl)
            return;
    }
    jfloat* _params_ = (params_ ? ((*env)->GetFloatArrayElements(env,params_,NULL) ) : NULL);
    impl(target_, level_, pname_, _params_);
    if(params_)
        (*env)->ReleaseFloatArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glGetTexLevelParameteriv (GLenum target, GLint level, GLenum pname, GLint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetTexLevelParameteriv( JNIEnv* env, jclass klass,jint target_,jint level_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetTexLevelParameteriv\n");
    static PFNGLGETTEXLEVELPARAMETERIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETTEXLEVELPARAMETERIVPROC) mygetprocaddr(env,"glGetTexLevelParameteriv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(target_, level_, pname_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI GLboolean APIENTRY glIsEnabled (GLenum cap);
JNIEXPORT jboolean JNICALL Java_JGL_JGL_glIsEnabled( JNIEnv* env, jclass klass,jint cap_){
    jboolean rv = 0;
    if(do_trace)
        fprintf(stderr,"glIsEnabled\n");
    static PFNGLISENABLEDPROC impl;
    if( !impl ){
       impl = (PFNGLISENABLEDPROC) mygetprocaddr(env,"glIsEnabled");
       if(!impl)
            return rv;
    }
    rv = impl(cap_);
    return rv;
}
//GLAPI void APIENTRY glDepthRange (GLdouble near, GLdouble far);
JNIEXPORT void JNICALL Java_JGL_JGL_glDepthRange( JNIEnv* env, jclass klass,jdouble near_,jdouble far_){
    if(do_trace)
        fprintf(stderr,"glDepthRange\n");
    static PFNGLDEPTHRANGEPROC impl;
    if( !impl ){
       impl = (PFNGLDEPTHRANGEPROC) mygetprocaddr(env,"glDepthRange");
       if(!impl)
            return;
    }
    impl(near_, far_);
}
//GLAPI void APIENTRY glViewport (GLint x, GLint y, GLsizei width, GLsizei height);
JNIEXPORT void JNICALL Java_JGL_JGL_glViewport( JNIEnv* env, jclass klass,jint x_,jint y_,jint width_,jint height_){
    if(do_trace)
        fprintf(stderr,"glViewport\n");
    static PFNGLVIEWPORTPROC impl;
    if( !impl ){
       impl = (PFNGLVIEWPORTPROC) mygetprocaddr(env,"glViewport");
       if(!impl)
            return;
    }
    impl(x_, y_, width_, height_);
}
//GLAPI void APIENTRY glDrawArrays (GLenum mode, GLint first, GLsizei count);
JNIEXPORT void JNICALL Java_JGL_JGL_glDrawArrays( JNIEnv* env, jclass klass,jint mode_,jint first_,jint count_){
    if(do_trace)
        fprintf(stderr,"glDrawArrays\n");
    static PFNGLDRAWARRAYSPROC impl;
    if( !impl ){
       impl = (PFNGLDRAWARRAYSPROC) mygetprocaddr(env,"glDrawArrays");
       if(!impl)
            return;
    }
    impl(mode_, first_, count_);
}
//GLAPI void APIENTRY glDrawElements (GLenum mode, GLsizei count, GLenum type, const void *indices);
JNIEXPORT void JNICALL Java_JGL_JGL_glDrawElements( JNIEnv* env, jclass klass,jint mode_,jint count_,jint type_,jint indices_){
    if(do_trace)
        fprintf(stderr,"glDrawElements\n");
    static PFNGLDRAWELEMENTSPROC impl;
    if( !impl ){
       impl = (PFNGLDRAWELEMENTSPROC) mygetprocaddr(env,"glDrawElements");
       if(!impl)
            return;
    }
    impl(mode_, count_, type_, (const void *)indices_);
}
//GLAPI void APIENTRY glPolygonOffset (GLfloat factor, GLfloat units);
JNIEXPORT void JNICALL Java_JGL_JGL_glPolygonOffset( JNIEnv* env, jclass klass,jfloat factor_,jfloat units_){
    if(do_trace)
        fprintf(stderr,"glPolygonOffset\n");
    static PFNGLPOLYGONOFFSETPROC impl;
    if( !impl ){
       impl = (PFNGLPOLYGONOFFSETPROC) mygetprocaddr(env,"glPolygonOffset");
       if(!impl)
            return;
    }
    impl(factor_, units_);
}
//GLAPI void APIENTRY glCopyTexImage1D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border);
JNIEXPORT void JNICALL Java_JGL_JGL_glCopyTexImage1D( JNIEnv* env, jclass klass,jint target_,jint level_,jint internalformat_,jint x_,jint y_,jint width_,jint border_){
    if(do_trace)
        fprintf(stderr,"glCopyTexImage1D\n");
    static PFNGLCOPYTEXIMAGE1DPROC impl;
    if( !impl ){
       impl = (PFNGLCOPYTEXIMAGE1DPROC) mygetprocaddr(env,"glCopyTexImage1D");
       if(!impl)
            return;
    }
    impl(target_, level_, internalformat_, x_, y_, width_, border_);
}
//GLAPI void APIENTRY glCopyTexImage2D (GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
JNIEXPORT void JNICALL Java_JGL_JGL_glCopyTexImage2D( JNIEnv* env, jclass klass,jint target_,jint level_,jint internalformat_,jint x_,jint y_,jint width_,jint height_,jint border_){
    if(do_trace)
        fprintf(stderr,"glCopyTexImage2D\n");
    static PFNGLCOPYTEXIMAGE2DPROC impl;
    if( !impl ){
       impl = (PFNGLCOPYTEXIMAGE2DPROC) mygetprocaddr(env,"glCopyTexImage2D");
       if(!impl)
            return;
    }
    impl(target_, level_, internalformat_, x_, y_, width_, height_, border_);
}
//GLAPI void APIENTRY glCopyTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
JNIEXPORT void JNICALL Java_JGL_JGL_glCopyTexSubImage1D( JNIEnv* env, jclass klass,jint target_,jint level_,jint xoffset_,jint x_,jint y_,jint width_){
    if(do_trace)
        fprintf(stderr,"glCopyTexSubImage1D\n");
    static PFNGLCOPYTEXSUBIMAGE1DPROC impl;
    if( !impl ){
       impl = (PFNGLCOPYTEXSUBIMAGE1DPROC) mygetprocaddr(env,"glCopyTexSubImage1D");
       if(!impl)
            return;
    }
    impl(target_, level_, xoffset_, x_, y_, width_);
}
//GLAPI void APIENTRY glCopyTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
JNIEXPORT void JNICALL Java_JGL_JGL_glCopyTexSubImage2D( JNIEnv* env, jclass klass,jint target_,jint level_,jint xoffset_,jint yoffset_,jint x_,jint y_,jint width_,jint height_){
    if(do_trace)
        fprintf(stderr,"glCopyTexSubImage2D\n");
    static PFNGLCOPYTEXSUBIMAGE2DPROC impl;
    if( !impl ){
       impl = (PFNGLCOPYTEXSUBIMAGE2DPROC) mygetprocaddr(env,"glCopyTexSubImage2D");
       if(!impl)
            return;
    }
    impl(target_, level_, xoffset_, yoffset_, x_, y_, width_, height_);
}
//GLAPI void APIENTRY glTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels);
JNIEXPORT void JNICALL Java_JGL_JGL_glTexSubImage1D( JNIEnv* env, jclass klass,jint target_,jint level_,jint xoffset_,jint width_,jint format_,jint type_,jbyteArray pixels_){
    if(do_trace)
        fprintf(stderr,"glTexSubImage1D\n");
    static PFNGLTEXSUBIMAGE1DPROC impl;
    if( !impl ){
       impl = (PFNGLTEXSUBIMAGE1DPROC) mygetprocaddr(env,"glTexSubImage1D");
       if(!impl)
            return;
    }
    jbyte* _pixels_ = (pixels_ ? ((*env)->GetByteArrayElements(env,pixels_,NULL) ) : NULL);
    impl(target_, level_, xoffset_, width_, format_, type_, _pixels_);
    if(pixels_)
        (*env)->ReleaseByteArrayElements(env,pixels_, _pixels_,0);
}
//GLAPI void APIENTRY glTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
JNIEXPORT void JNICALL Java_JGL_JGL_glTexSubImage2D( JNIEnv* env, jclass klass,jint target_,jint level_,jint xoffset_,jint yoffset_,jint width_,jint height_,jint format_,jint type_,jbyteArray pixels_){
    if(do_trace)
        fprintf(stderr,"glTexSubImage2D\n");
    static PFNGLTEXSUBIMAGE2DPROC impl;
    if( !impl ){
       impl = (PFNGLTEXSUBIMAGE2DPROC) mygetprocaddr(env,"glTexSubImage2D");
       if(!impl)
            return;
    }
    jbyte* _pixels_ = (pixels_ ? ((*env)->GetByteArrayElements(env,pixels_,NULL) ) : NULL);
    impl(target_, level_, xoffset_, yoffset_, width_, height_, format_, type_, _pixels_);
    if(pixels_)
        (*env)->ReleaseByteArrayElements(env,pixels_, _pixels_,0);
}
//GLAPI void APIENTRY glBindTexture (GLenum target, GLuint texture);
JNIEXPORT void JNICALL Java_JGL_JGL_glBindTexture( JNIEnv* env, jclass klass,jint target_,jint texture_){
    if(do_trace)
        fprintf(stderr,"glBindTexture\n");
    static PFNGLBINDTEXTUREPROC impl;
    if( !impl ){
       impl = (PFNGLBINDTEXTUREPROC) mygetprocaddr(env,"glBindTexture");
       if(!impl)
            return;
    }
    impl(target_, texture_);
}
//GLAPI void APIENTRY glDeleteTextures (GLsizei n, const GLuint *textures);
JNIEXPORT void JNICALL Java_JGL_JGL_glDeleteTextures( JNIEnv* env, jclass klass,jint n_,jintArray textures_){
    if(do_trace)
        fprintf(stderr,"glDeleteTextures\n");
    static PFNGLDELETETEXTURESPROC impl;
    if( !impl ){
       impl = (PFNGLDELETETEXTURESPROC) mygetprocaddr(env,"glDeleteTextures");
       if(!impl)
            return;
    }
    jint* _textures_ = (textures_ ? ((*env)->GetIntArrayElements(env,textures_,NULL) ) : NULL);
    impl(n_, _textures_);
    if(textures_)
        (*env)->ReleaseIntArrayElements(env,textures_, _textures_,0);
}
//GLAPI void APIENTRY glGenTextures (GLsizei n, GLuint *textures);
JNIEXPORT void JNICALL Java_JGL_JGL_glGenTextures( JNIEnv* env, jclass klass,jint n_,jintArray textures_){
    if(do_trace)
        fprintf(stderr,"glGenTextures\n");
    static PFNGLGENTEXTURESPROC impl;
    if( !impl ){
       impl = (PFNGLGENTEXTURESPROC) mygetprocaddr(env,"glGenTextures");
       if(!impl)
            return;
    }
    jint* _textures_ = (textures_ ? ((*env)->GetIntArrayElements(env,textures_,NULL) ) : NULL);
    impl(n_, _textures_);
    if(textures_)
        (*env)->ReleaseIntArrayElements(env,textures_, _textures_,0);
}
//GLAPI GLboolean APIENTRY glIsTexture (GLuint texture);
JNIEXPORT jboolean JNICALL Java_JGL_JGL_glIsTexture( JNIEnv* env, jclass klass,jint texture_){
    jboolean rv = 0;
    if(do_trace)
        fprintf(stderr,"glIsTexture\n");
    static PFNGLISTEXTUREPROC impl;
    if( !impl ){
       impl = (PFNGLISTEXTUREPROC) mygetprocaddr(env,"glIsTexture");
       if(!impl)
            return rv;
    }
    rv = impl(texture_);
    return rv;
}
//GLAPI void APIENTRY glDrawRangeElements (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices);
JNIEXPORT void JNICALL Java_JGL_JGL_glDrawRangeElements( JNIEnv* env, jclass klass,jint mode_,jint start_,jint end_,jint count_,jint type_,jint indices_){
    if(do_trace)
        fprintf(stderr,"glDrawRangeElements\n");
    static PFNGLDRAWRANGEELEMENTSPROC impl;
    if( !impl ){
       impl = (PFNGLDRAWRANGEELEMENTSPROC) mygetprocaddr(env,"glDrawRangeElements");
       if(!impl)
            return;
    }
    impl(mode_, start_, end_, count_, type_, (const void *)indices_);
}
//GLAPI void APIENTRY glTexImage3D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void *pixels);
JNIEXPORT void JNICALL Java_JGL_JGL_glTexImage3D( JNIEnv* env, jclass klass,jint target_,jint level_,jint internalformat_,jint width_,jint height_,jint depth_,jint border_,jint format_,jint type_,jbyteArray pixels_){
    if(do_trace)
        fprintf(stderr,"glTexImage3D\n");
    static PFNGLTEXIMAGE3DPROC impl;
    if( !impl ){
       impl = (PFNGLTEXIMAGE3DPROC) mygetprocaddr(env,"glTexImage3D");
       if(!impl)
            return;
    }
    jbyte* _pixels_ = (pixels_ ? ((*env)->GetByteArrayElements(env,pixels_,NULL) ) : NULL);
    impl(target_, level_, internalformat_, width_, height_, depth_, border_, format_, type_, _pixels_);
    if(pixels_)
        (*env)->ReleaseByteArrayElements(env,pixels_, _pixels_,0);
}
//GLAPI void APIENTRY glTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels);
JNIEXPORT void JNICALL Java_JGL_JGL_glTexSubImage3D( JNIEnv* env, jclass klass,jint target_,jint level_,jint xoffset_,jint yoffset_,jint zoffset_,jint width_,jint height_,jint depth_,jint format_,jint type_,jbyteArray pixels_){
    if(do_trace)
        fprintf(stderr,"glTexSubImage3D\n");
    static PFNGLTEXSUBIMAGE3DPROC impl;
    if( !impl ){
       impl = (PFNGLTEXSUBIMAGE3DPROC) mygetprocaddr(env,"glTexSubImage3D");
       if(!impl)
            return;
    }
    jbyte* _pixels_ = (pixels_ ? ((*env)->GetByteArrayElements(env,pixels_,NULL) ) : NULL);
    impl(target_, level_, xoffset_, yoffset_, zoffset_, width_, height_, depth_, format_, type_, _pixels_);
    if(pixels_)
        (*env)->ReleaseByteArrayElements(env,pixels_, _pixels_,0);
}
//GLAPI void APIENTRY glCopyTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
JNIEXPORT void JNICALL Java_JGL_JGL_glCopyTexSubImage3D( JNIEnv* env, jclass klass,jint target_,jint level_,jint xoffset_,jint yoffset_,jint zoffset_,jint x_,jint y_,jint width_,jint height_){
    if(do_trace)
        fprintf(stderr,"glCopyTexSubImage3D\n");
    static PFNGLCOPYTEXSUBIMAGE3DPROC impl;
    if( !impl ){
       impl = (PFNGLCOPYTEXSUBIMAGE3DPROC) mygetprocaddr(env,"glCopyTexSubImage3D");
       if(!impl)
            return;
    }
    impl(target_, level_, xoffset_, yoffset_, zoffset_, x_, y_, width_, height_);
}
//GLAPI void APIENTRY glActiveTexture (GLenum texture);
JNIEXPORT void JNICALL Java_JGL_JGL_glActiveTexture( JNIEnv* env, jclass klass,jint texture_){
    if(do_trace)
        fprintf(stderr,"glActiveTexture\n");
    static PFNGLACTIVETEXTUREPROC impl;
    if( !impl ){
       impl = (PFNGLACTIVETEXTUREPROC) mygetprocaddr(env,"glActiveTexture");
       if(!impl)
            return;
    }
    impl(texture_);
}
//GLAPI void APIENTRY glSampleCoverage (GLfloat value, GLboolean invert);
JNIEXPORT void JNICALL Java_JGL_JGL_glSampleCoverage( JNIEnv* env, jclass klass,jfloat value_,jboolean invert_){
    if(do_trace)
        fprintf(stderr,"glSampleCoverage\n");
    static PFNGLSAMPLECOVERAGEPROC impl;
    if( !impl ){
       impl = (PFNGLSAMPLECOVERAGEPROC) mygetprocaddr(env,"glSampleCoverage");
       if(!impl)
            return;
    }
    impl(value_, invert_);
}
//GLAPI void APIENTRY glCompressedTexImage3D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void *data);
JNIEXPORT void JNICALL Java_JGL_JGL_glCompressedTexImage3D( JNIEnv* env, jclass klass,jint target_,jint level_,jint internalformat_,jint width_,jint height_,jint depth_,jint border_,jint imageSize_,jbyteArray data_){
    if(do_trace)
        fprintf(stderr,"glCompressedTexImage3D\n");
    static PFNGLCOMPRESSEDTEXIMAGE3DPROC impl;
    if( !impl ){
       impl = (PFNGLCOMPRESSEDTEXIMAGE3DPROC) mygetprocaddr(env,"glCompressedTexImage3D");
       if(!impl)
            return;
    }
    jbyte* _data_ = (data_ ? ((*env)->GetByteArrayElements(env,data_,NULL) ) : NULL);
    impl(target_, level_, internalformat_, width_, height_, depth_, border_, imageSize_, _data_);
    if(data_)
        (*env)->ReleaseByteArrayElements(env,data_, _data_,0);
}
//GLAPI void APIENTRY glCompressedTexImage2D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void *data);
JNIEXPORT void JNICALL Java_JGL_JGL_glCompressedTexImage2D( JNIEnv* env, jclass klass,jint target_,jint level_,jint internalformat_,jint width_,jint height_,jint border_,jint imageSize_,jbyteArray data_){
    if(do_trace)
        fprintf(stderr,"glCompressedTexImage2D\n");
    static PFNGLCOMPRESSEDTEXIMAGE2DPROC impl;
    if( !impl ){
       impl = (PFNGLCOMPRESSEDTEXIMAGE2DPROC) mygetprocaddr(env,"glCompressedTexImage2D");
       if(!impl)
            return;
    }
    jbyte* _data_ = (data_ ? ((*env)->GetByteArrayElements(env,data_,NULL) ) : NULL);
    impl(target_, level_, internalformat_, width_, height_, border_, imageSize_, _data_);
    if(data_)
        (*env)->ReleaseByteArrayElements(env,data_, _data_,0);
}
//GLAPI void APIENTRY glCompressedTexImage1D (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const void *data);
JNIEXPORT void JNICALL Java_JGL_JGL_glCompressedTexImage1D( JNIEnv* env, jclass klass,jint target_,jint level_,jint internalformat_,jint width_,jint border_,jint imageSize_,jbyteArray data_){
    if(do_trace)
        fprintf(stderr,"glCompressedTexImage1D\n");
    static PFNGLCOMPRESSEDTEXIMAGE1DPROC impl;
    if( !impl ){
       impl = (PFNGLCOMPRESSEDTEXIMAGE1DPROC) mygetprocaddr(env,"glCompressedTexImage1D");
       if(!impl)
            return;
    }
    jbyte* _data_ = (data_ ? ((*env)->GetByteArrayElements(env,data_,NULL) ) : NULL);
    impl(target_, level_, internalformat_, width_, border_, imageSize_, _data_);
    if(data_)
        (*env)->ReleaseByteArrayElements(env,data_, _data_,0);
}
//GLAPI void APIENTRY glCompressedTexSubImage3D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data);
JNIEXPORT void JNICALL Java_JGL_JGL_glCompressedTexSubImage3D( JNIEnv* env, jclass klass,jint target_,jint level_,jint xoffset_,jint yoffset_,jint zoffset_,jint width_,jint height_,jint depth_,jint format_,jint imageSize_,jbyteArray data_){
    if(do_trace)
        fprintf(stderr,"glCompressedTexSubImage3D\n");
    static PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC impl;
    if( !impl ){
       impl = (PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC) mygetprocaddr(env,"glCompressedTexSubImage3D");
       if(!impl)
            return;
    }
    jbyte* _data_ = (data_ ? ((*env)->GetByteArrayElements(env,data_,NULL) ) : NULL);
    impl(target_, level_, xoffset_, yoffset_, zoffset_, width_, height_, depth_, format_, imageSize_, _data_);
    if(data_)
        (*env)->ReleaseByteArrayElements(env,data_, _data_,0);
}
//GLAPI void APIENTRY glCompressedTexSubImage2D (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data);
JNIEXPORT void JNICALL Java_JGL_JGL_glCompressedTexSubImage2D( JNIEnv* env, jclass klass,jint target_,jint level_,jint xoffset_,jint yoffset_,jint width_,jint height_,jint format_,jint imageSize_,jbyteArray data_){
    if(do_trace)
        fprintf(stderr,"glCompressedTexSubImage2D\n");
    static PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC impl;
    if( !impl ){
       impl = (PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC) mygetprocaddr(env,"glCompressedTexSubImage2D");
       if(!impl)
            return;
    }
    jbyte* _data_ = (data_ ? ((*env)->GetByteArrayElements(env,data_,NULL) ) : NULL);
    impl(target_, level_, xoffset_, yoffset_, width_, height_, format_, imageSize_, _data_);
    if(data_)
        (*env)->ReleaseByteArrayElements(env,data_, _data_,0);
}
//GLAPI void APIENTRY glCompressedTexSubImage1D (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void *data);
JNIEXPORT void JNICALL Java_JGL_JGL_glCompressedTexSubImage1D( JNIEnv* env, jclass klass,jint target_,jint level_,jint xoffset_,jint width_,jint format_,jint imageSize_,jbyteArray data_){
    if(do_trace)
        fprintf(stderr,"glCompressedTexSubImage1D\n");
    static PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC impl;
    if( !impl ){
       impl = (PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC) mygetprocaddr(env,"glCompressedTexSubImage1D");
       if(!impl)
            return;
    }
    jbyte* _data_ = (data_ ? ((*env)->GetByteArrayElements(env,data_,NULL) ) : NULL);
    impl(target_, level_, xoffset_, width_, format_, imageSize_, _data_);
    if(data_)
        (*env)->ReleaseByteArrayElements(env,data_, _data_,0);
}
//GLAPI void APIENTRY glGetCompressedTexImage (GLenum target, GLint level, void *img);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetCompressedTexImage( JNIEnv* env, jclass klass,jint target_,jint level_,jbyteArray img_){
    if(do_trace)
        fprintf(stderr,"glGetCompressedTexImage\n");
    static PFNGLGETCOMPRESSEDTEXIMAGEPROC impl;
    if( !impl ){
       impl = (PFNGLGETCOMPRESSEDTEXIMAGEPROC) mygetprocaddr(env,"glGetCompressedTexImage");
       if(!impl)
            return;
    }
    jbyte* _img_ = (img_ ? ((*env)->GetByteArrayElements(env,img_,NULL) ) : NULL);
    impl(target_, level_, _img_);
    if(img_)
        (*env)->ReleaseByteArrayElements(env,img_, _img_,0);
}
//GLAPI void APIENTRY glBlendFuncSeparate (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
JNIEXPORT void JNICALL Java_JGL_JGL_glBlendFuncSeparate( JNIEnv* env, jclass klass,jint sfactorRGB_,jint dfactorRGB_,jint sfactorAlpha_,jint dfactorAlpha_){
    if(do_trace)
        fprintf(stderr,"glBlendFuncSeparate\n");
    static PFNGLBLENDFUNCSEPARATEPROC impl;
    if( !impl ){
       impl = (PFNGLBLENDFUNCSEPARATEPROC) mygetprocaddr(env,"glBlendFuncSeparate");
       if(!impl)
            return;
    }
    impl(sfactorRGB_, dfactorRGB_, sfactorAlpha_, dfactorAlpha_);
}
//GLAPI void APIENTRY glMultiDrawArrays (GLenum mode, const GLint *first, const GLsizei *count, GLsizei drawcount);
JNIEXPORT void JNICALL Java_JGL_JGL_glMultiDrawArrays( JNIEnv* env, jclass klass,jint mode_,jintArray first_,jintArray count_,jint drawcount_){
    if(do_trace)
        fprintf(stderr,"glMultiDrawArrays\n");
    static PFNGLMULTIDRAWARRAYSPROC impl;
    if( !impl ){
       impl = (PFNGLMULTIDRAWARRAYSPROC) mygetprocaddr(env,"glMultiDrawArrays");
       if(!impl)
            return;
    }
    jint* _first_ = (first_ ? ((*env)->GetIntArrayElements(env,first_,NULL) ) : NULL);
    jint* _count_ = (count_ ? ((*env)->GetIntArrayElements(env,count_,NULL) ) : NULL);
    impl(mode_, _first_, _count_, drawcount_);
    if(first_)
        (*env)->ReleaseIntArrayElements(env,first_, _first_,0);
    if(count_)
        (*env)->ReleaseIntArrayElements(env,count_, _count_,0);
}
//GLAPI void APIENTRY glPointParameterf (GLenum pname, GLfloat param);
JNIEXPORT void JNICALL Java_JGL_JGL_glPointParameterf( JNIEnv* env, jclass klass,jint pname_,jfloat param_){
    if(do_trace)
        fprintf(stderr,"glPointParameterf\n");
    static PFNGLPOINTPARAMETERFPROC impl;
    if( !impl ){
       impl = (PFNGLPOINTPARAMETERFPROC) mygetprocaddr(env,"glPointParameterf");
       if(!impl)
            return;
    }
    impl(pname_, param_);
}
//GLAPI void APIENTRY glPointParameterfv (GLenum pname, const GLfloat *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glPointParameterfv( JNIEnv* env, jclass klass,jint pname_,jfloatArray params_){
    if(do_trace)
        fprintf(stderr,"glPointParameterfv\n");
    static PFNGLPOINTPARAMETERFVPROC impl;
    if( !impl ){
       impl = (PFNGLPOINTPARAMETERFVPROC) mygetprocaddr(env,"glPointParameterfv");
       if(!impl)
            return;
    }
    jfloat* _params_ = (params_ ? ((*env)->GetFloatArrayElements(env,params_,NULL) ) : NULL);
    impl(pname_, _params_);
    if(params_)
        (*env)->ReleaseFloatArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glPointParameteri (GLenum pname, GLint param);
JNIEXPORT void JNICALL Java_JGL_JGL_glPointParameteri( JNIEnv* env, jclass klass,jint pname_,jint param_){
    if(do_trace)
        fprintf(stderr,"glPointParameteri\n");
    static PFNGLPOINTPARAMETERIPROC impl;
    if( !impl ){
       impl = (PFNGLPOINTPARAMETERIPROC) mygetprocaddr(env,"glPointParameteri");
       if(!impl)
            return;
    }
    impl(pname_, param_);
}
//GLAPI void APIENTRY glPointParameteriv (GLenum pname, const GLint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glPointParameteriv( JNIEnv* env, jclass klass,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glPointParameteriv\n");
    static PFNGLPOINTPARAMETERIVPROC impl;
    if( !impl ){
       impl = (PFNGLPOINTPARAMETERIVPROC) mygetprocaddr(env,"glPointParameteriv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(pname_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glBlendColor (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
JNIEXPORT void JNICALL Java_JGL_JGL_glBlendColor( JNIEnv* env, jclass klass,jfloat red_,jfloat green_,jfloat blue_,jfloat alpha_){
    if(do_trace)
        fprintf(stderr,"glBlendColor\n");
    static PFNGLBLENDCOLORPROC impl;
    if( !impl ){
       impl = (PFNGLBLENDCOLORPROC) mygetprocaddr(env,"glBlendColor");
       if(!impl)
            return;
    }
    impl(red_, green_, blue_, alpha_);
}
//GLAPI void APIENTRY glBlendEquation (GLenum mode);
JNIEXPORT void JNICALL Java_JGL_JGL_glBlendEquation( JNIEnv* env, jclass klass,jint mode_){
    if(do_trace)
        fprintf(stderr,"glBlendEquation\n");
    static PFNGLBLENDEQUATIONPROC impl;
    if( !impl ){
       impl = (PFNGLBLENDEQUATIONPROC) mygetprocaddr(env,"glBlendEquation");
       if(!impl)
            return;
    }
    impl(mode_);
}
//GLAPI void APIENTRY glGenQueries (GLsizei n, GLuint *ids);
JNIEXPORT void JNICALL Java_JGL_JGL_glGenQueries( JNIEnv* env, jclass klass,jint n_,jintArray ids_){
    if(do_trace)
        fprintf(stderr,"glGenQueries\n");
    static PFNGLGENQUERIESPROC impl;
    if( !impl ){
       impl = (PFNGLGENQUERIESPROC) mygetprocaddr(env,"glGenQueries");
       if(!impl)
            return;
    }
    jint* _ids_ = (ids_ ? ((*env)->GetIntArrayElements(env,ids_,NULL) ) : NULL);
    impl(n_, _ids_);
    if(ids_)
        (*env)->ReleaseIntArrayElements(env,ids_, _ids_,0);
}
//GLAPI void APIENTRY glDeleteQueries (GLsizei n, const GLuint *ids);
JNIEXPORT void JNICALL Java_JGL_JGL_glDeleteQueries( JNIEnv* env, jclass klass,jint n_,jintArray ids_){
    if(do_trace)
        fprintf(stderr,"glDeleteQueries\n");
    static PFNGLDELETEQUERIESPROC impl;
    if( !impl ){
       impl = (PFNGLDELETEQUERIESPROC) mygetprocaddr(env,"glDeleteQueries");
       if(!impl)
            return;
    }
    jint* _ids_ = (ids_ ? ((*env)->GetIntArrayElements(env,ids_,NULL) ) : NULL);
    impl(n_, _ids_);
    if(ids_)
        (*env)->ReleaseIntArrayElements(env,ids_, _ids_,0);
}
//GLAPI GLboolean APIENTRY glIsQuery (GLuint id);
JNIEXPORT jboolean JNICALL Java_JGL_JGL_glIsQuery( JNIEnv* env, jclass klass,jint id_){
    jboolean rv = 0;
    if(do_trace)
        fprintf(stderr,"glIsQuery\n");
    static PFNGLISQUERYPROC impl;
    if( !impl ){
       impl = (PFNGLISQUERYPROC) mygetprocaddr(env,"glIsQuery");
       if(!impl)
            return rv;
    }
    rv = impl(id_);
    return rv;
}
//GLAPI void APIENTRY glBeginQuery (GLenum target, GLuint id);
JNIEXPORT void JNICALL Java_JGL_JGL_glBeginQuery( JNIEnv* env, jclass klass,jint target_,jint id_){
    if(do_trace)
        fprintf(stderr,"glBeginQuery\n");
    static PFNGLBEGINQUERYPROC impl;
    if( !impl ){
       impl = (PFNGLBEGINQUERYPROC) mygetprocaddr(env,"glBeginQuery");
       if(!impl)
            return;
    }
    impl(target_, id_);
}
//GLAPI void APIENTRY glEndQuery (GLenum target);
JNIEXPORT void JNICALL Java_JGL_JGL_glEndQuery( JNIEnv* env, jclass klass,jint target_){
    if(do_trace)
        fprintf(stderr,"glEndQuery\n");
    static PFNGLENDQUERYPROC impl;
    if( !impl ){
       impl = (PFNGLENDQUERYPROC) mygetprocaddr(env,"glEndQuery");
       if(!impl)
            return;
    }
    impl(target_);
}
//GLAPI void APIENTRY glGetQueryiv (GLenum target, GLenum pname, GLint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetQueryiv( JNIEnv* env, jclass klass,jint target_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetQueryiv\n");
    static PFNGLGETQUERYIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETQUERYIVPROC) mygetprocaddr(env,"glGetQueryiv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(target_, pname_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glGetQueryObjectiv (GLuint id, GLenum pname, GLint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetQueryObjectiv( JNIEnv* env, jclass klass,jint id_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetQueryObjectiv\n");
    static PFNGLGETQUERYOBJECTIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETQUERYOBJECTIVPROC) mygetprocaddr(env,"glGetQueryObjectiv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(id_, pname_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glGetQueryObjectuiv (GLuint id, GLenum pname, GLuint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetQueryObjectuiv( JNIEnv* env, jclass klass,jint id_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetQueryObjectuiv\n");
    static PFNGLGETQUERYOBJECTUIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETQUERYOBJECTUIVPROC) mygetprocaddr(env,"glGetQueryObjectuiv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(id_, pname_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glBindBuffer (GLenum target, GLuint buffer);
JNIEXPORT void JNICALL Java_JGL_JGL_glBindBuffer( JNIEnv* env, jclass klass,jint target_,jint buffer_){
    if(do_trace)
        fprintf(stderr,"glBindBuffer\n");
    static PFNGLBINDBUFFERPROC impl;
    if( !impl ){
       impl = (PFNGLBINDBUFFERPROC) mygetprocaddr(env,"glBindBuffer");
       if(!impl)
            return;
    }
    impl(target_, buffer_);
}
//GLAPI void APIENTRY glDeleteBuffers (GLsizei n, const GLuint *buffers);
JNIEXPORT void JNICALL Java_JGL_JGL_glDeleteBuffers( JNIEnv* env, jclass klass,jint n_,jintArray buffers_){
    if(do_trace)
        fprintf(stderr,"glDeleteBuffers\n");
    static PFNGLDELETEBUFFERSPROC impl;
    if( !impl ){
       impl = (PFNGLDELETEBUFFERSPROC) mygetprocaddr(env,"glDeleteBuffers");
       if(!impl)
            return;
    }
    jint* _buffers_ = (buffers_ ? ((*env)->GetIntArrayElements(env,buffers_,NULL) ) : NULL);
    impl(n_, _buffers_);
    if(buffers_)
        (*env)->ReleaseIntArrayElements(env,buffers_, _buffers_,0);
}
//GLAPI void APIENTRY glGenBuffers (GLsizei n, GLuint *buffers);
JNIEXPORT void JNICALL Java_JGL_JGL_glGenBuffers( JNIEnv* env, jclass klass,jint n_,jintArray buffers_){
    if(do_trace)
        fprintf(stderr,"glGenBuffers\n");
    static PFNGLGENBUFFERSPROC impl;
    if( !impl ){
       impl = (PFNGLGENBUFFERSPROC) mygetprocaddr(env,"glGenBuffers");
       if(!impl)
            return;
    }
    jint* _buffers_ = (buffers_ ? ((*env)->GetIntArrayElements(env,buffers_,NULL) ) : NULL);
    impl(n_, _buffers_);
    if(buffers_)
        (*env)->ReleaseIntArrayElements(env,buffers_, _buffers_,0);
}
//GLAPI GLboolean APIENTRY glIsBuffer (GLuint buffer);
JNIEXPORT jboolean JNICALL Java_JGL_JGL_glIsBuffer( JNIEnv* env, jclass klass,jint buffer_){
    jboolean rv = 0;
    if(do_trace)
        fprintf(stderr,"glIsBuffer\n");
    static PFNGLISBUFFERPROC impl;
    if( !impl ){
       impl = (PFNGLISBUFFERPROC) mygetprocaddr(env,"glIsBuffer");
       if(!impl)
            return rv;
    }
    rv = impl(buffer_);
    return rv;
}
//GLAPI void APIENTRY glBufferData (GLenum target, GLsizeiptr size, const void *data, GLenum usage);
JNIEXPORT void JNICALL Java_JGL_JGL_glBufferData( JNIEnv* env, jclass klass,jint target_,jint size_,jbyteArray data_,jint usage_){
    if(do_trace)
        fprintf(stderr,"glBufferData\n");
    static PFNGLBUFFERDATAPROC impl;
    if( !impl ){
       impl = (PFNGLBUFFERDATAPROC) mygetprocaddr(env,"glBufferData");
       if(!impl)
            return;
    }
    jbyte* _data_ = (data_ ? ((*env)->GetByteArrayElements(env,data_,NULL) ) : NULL);
    impl(target_, size_, _data_, usage_);
    if(data_)
        (*env)->ReleaseByteArrayElements(env,data_, _data_,0);
}
//GLAPI void APIENTRY glBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, const void *data);
JNIEXPORT void JNICALL Java_JGL_JGL_glBufferSubData( JNIEnv* env, jclass klass,jint target_,jint offset_,jint size_,jbyteArray data_){
    if(do_trace)
        fprintf(stderr,"glBufferSubData\n");
    static PFNGLBUFFERSUBDATAPROC impl;
    if( !impl ){
       impl = (PFNGLBUFFERSUBDATAPROC) mygetprocaddr(env,"glBufferSubData");
       if(!impl)
            return;
    }
    jbyte* _data_ = (data_ ? ((*env)->GetByteArrayElements(env,data_,NULL) ) : NULL);
    impl(target_, offset_, size_, _data_);
    if(data_)
        (*env)->ReleaseByteArrayElements(env,data_, _data_,0);
}
//GLAPI void APIENTRY glGetBufferSubData (GLenum target, GLintptr offset, GLsizeiptr size, void *data);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetBufferSubData( JNIEnv* env, jclass klass,jint target_,jint offset_,jint size_,jbyteArray data_){
    if(do_trace)
        fprintf(stderr,"glGetBufferSubData\n");
    static PFNGLGETBUFFERSUBDATAPROC impl;
    if( !impl ){
       impl = (PFNGLGETBUFFERSUBDATAPROC) mygetprocaddr(env,"glGetBufferSubData");
       if(!impl)
            return;
    }
    jbyte* _data_ = (data_ ? ((*env)->GetByteArrayElements(env,data_,NULL) ) : NULL);
    impl(target_, offset_, size_, _data_);
    if(data_)
        (*env)->ReleaseByteArrayElements(env,data_, _data_,0);
}
//GLAPI GLboolean APIENTRY glUnmapBuffer (GLenum target);
JNIEXPORT jboolean JNICALL Java_JGL_JGL_glUnmapBuffer( JNIEnv* env, jclass klass,jint target_){
    jboolean rv = 0;
    if(do_trace)
        fprintf(stderr,"glUnmapBuffer\n");
    static PFNGLUNMAPBUFFERPROC impl;
    if( !impl ){
       impl = (PFNGLUNMAPBUFFERPROC) mygetprocaddr(env,"glUnmapBuffer");
       if(!impl)
            return rv;
    }
    rv = impl(target_);
    return rv;
}
//GLAPI void APIENTRY glGetBufferParameteriv (GLenum target, GLenum pname, GLint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetBufferParameteriv( JNIEnv* env, jclass klass,jint target_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetBufferParameteriv\n");
    static PFNGLGETBUFFERPARAMETERIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETBUFFERPARAMETERIVPROC) mygetprocaddr(env,"glGetBufferParameteriv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(target_, pname_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glBlendEquationSeparate (GLenum modeRGB, GLenum modeAlpha);
JNIEXPORT void JNICALL Java_JGL_JGL_glBlendEquationSeparate( JNIEnv* env, jclass klass,jint modeRGB_,jint modeAlpha_){
    if(do_trace)
        fprintf(stderr,"glBlendEquationSeparate\n");
    static PFNGLBLENDEQUATIONSEPARATEPROC impl;
    if( !impl ){
       impl = (PFNGLBLENDEQUATIONSEPARATEPROC) mygetprocaddr(env,"glBlendEquationSeparate");
       if(!impl)
            return;
    }
    impl(modeRGB_, modeAlpha_);
}
//GLAPI void APIENTRY glDrawBuffers (GLsizei n, const GLenum *bufs);
JNIEXPORT void JNICALL Java_JGL_JGL_glDrawBuffers( JNIEnv* env, jclass klass,jint n_,jintArray bufs_){
    if(do_trace)
        fprintf(stderr,"glDrawBuffers\n");
    static PFNGLDRAWBUFFERSPROC impl;
    if( !impl ){
       impl = (PFNGLDRAWBUFFERSPROC) mygetprocaddr(env,"glDrawBuffers");
       if(!impl)
            return;
    }
    jint* _bufs_ = (bufs_ ? ((*env)->GetIntArrayElements(env,bufs_,NULL) ) : NULL);
    impl(n_, _bufs_);
    if(bufs_)
        (*env)->ReleaseIntArrayElements(env,bufs_, _bufs_,0);
}
//GLAPI void APIENTRY glStencilOpSeparate (GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
JNIEXPORT void JNICALL Java_JGL_JGL_glStencilOpSeparate( JNIEnv* env, jclass klass,jint face_,jint sfail_,jint dpfail_,jint dppass_){
    if(do_trace)
        fprintf(stderr,"glStencilOpSeparate\n");
    static PFNGLSTENCILOPSEPARATEPROC impl;
    if( !impl ){
       impl = (PFNGLSTENCILOPSEPARATEPROC) mygetprocaddr(env,"glStencilOpSeparate");
       if(!impl)
            return;
    }
    impl(face_, sfail_, dpfail_, dppass_);
}
//GLAPI void APIENTRY glStencilFuncSeparate (GLenum face, GLenum func, GLint ref, GLuint mask);
JNIEXPORT void JNICALL Java_JGL_JGL_glStencilFuncSeparate( JNIEnv* env, jclass klass,jint face_,jint func_,jint ref_,jint mask_){
    if(do_trace)
        fprintf(stderr,"glStencilFuncSeparate\n");
    static PFNGLSTENCILFUNCSEPARATEPROC impl;
    if( !impl ){
       impl = (PFNGLSTENCILFUNCSEPARATEPROC) mygetprocaddr(env,"glStencilFuncSeparate");
       if(!impl)
            return;
    }
    impl(face_, func_, ref_, mask_);
}
//GLAPI void APIENTRY glStencilMaskSeparate (GLenum face, GLuint mask);
JNIEXPORT void JNICALL Java_JGL_JGL_glStencilMaskSeparate( JNIEnv* env, jclass klass,jint face_,jint mask_){
    if(do_trace)
        fprintf(stderr,"glStencilMaskSeparate\n");
    static PFNGLSTENCILMASKSEPARATEPROC impl;
    if( !impl ){
       impl = (PFNGLSTENCILMASKSEPARATEPROC) mygetprocaddr(env,"glStencilMaskSeparate");
       if(!impl)
            return;
    }
    impl(face_, mask_);
}
//GLAPI void APIENTRY glAttachShader (GLuint program, GLuint shader);
JNIEXPORT void JNICALL Java_JGL_JGL_glAttachShader( JNIEnv* env, jclass klass,jint program_,jint shader_){
    if(do_trace)
        fprintf(stderr,"glAttachShader\n");
    static PFNGLATTACHSHADERPROC impl;
    if( !impl ){
       impl = (PFNGLATTACHSHADERPROC) mygetprocaddr(env,"glAttachShader");
       if(!impl)
            return;
    }
    impl(program_, shader_);
}
//GLAPI void APIENTRY glBindAttribLocation (GLuint program, GLuint index, const GLchar *name);
JNIEXPORT void JNICALL Java_JGL_JGL_glBindAttribLocation( JNIEnv* env, jclass klass,jint program_,jint index_,jbyteArray name_){
    if(do_trace)
        fprintf(stderr,"glBindAttribLocation\n");
    static PFNGLBINDATTRIBLOCATIONPROC impl;
    if( !impl ){
       impl = (PFNGLBINDATTRIBLOCATIONPROC) mygetprocaddr(env,"glBindAttribLocation");
       if(!impl)
            return;
    }
    jbyte* _name_ = (name_ ? ((*env)->GetByteArrayElements(env,name_,NULL) ) : NULL);
    impl(program_, index_, _name_);
    if(name_)
        (*env)->ReleaseByteArrayElements(env,name_, _name_,0);
}
//GLAPI void APIENTRY glCompileShader (GLuint shader);
JNIEXPORT void JNICALL Java_JGL_JGL_glCompileShader( JNIEnv* env, jclass klass,jint shader_){
    if(do_trace)
        fprintf(stderr,"glCompileShader\n");
    static PFNGLCOMPILESHADERPROC impl;
    if( !impl ){
       impl = (PFNGLCOMPILESHADERPROC) mygetprocaddr(env,"glCompileShader");
       if(!impl)
            return;
    }
    impl(shader_);
}
//GLAPI GLuint APIENTRY glCreateProgram (void);
JNIEXPORT jint JNICALL Java_JGL_JGL_glCreateProgram( JNIEnv* env, jclass klass){
    jint rv = 0;
    if(do_trace)
        fprintf(stderr,"glCreateProgram\n");
    static PFNGLCREATEPROGRAMPROC impl;
    if( !impl ){
       impl = (PFNGLCREATEPROGRAMPROC) mygetprocaddr(env,"glCreateProgram");
       if(!impl)
            return rv;
    }
    rv = impl();
    return rv;
}
//GLAPI GLuint APIENTRY glCreateShader (GLenum type);
JNIEXPORT jint JNICALL Java_JGL_JGL_glCreateShader( JNIEnv* env, jclass klass,jint type_){
    jint rv = 0;
    if(do_trace)
        fprintf(stderr,"glCreateShader\n");
    static PFNGLCREATESHADERPROC impl;
    if( !impl ){
       impl = (PFNGLCREATESHADERPROC) mygetprocaddr(env,"glCreateShader");
       if(!impl)
            return rv;
    }
    rv = impl(type_);
    return rv;
}
//GLAPI void APIENTRY glDeleteProgram (GLuint program);
JNIEXPORT void JNICALL Java_JGL_JGL_glDeleteProgram( JNIEnv* env, jclass klass,jint program_){
    if(do_trace)
        fprintf(stderr,"glDeleteProgram\n");
    static PFNGLDELETEPROGRAMPROC impl;
    if( !impl ){
       impl = (PFNGLDELETEPROGRAMPROC) mygetprocaddr(env,"glDeleteProgram");
       if(!impl)
            return;
    }
    impl(program_);
}
//GLAPI void APIENTRY glDeleteShader (GLuint shader);
JNIEXPORT void JNICALL Java_JGL_JGL_glDeleteShader( JNIEnv* env, jclass klass,jint shader_){
    if(do_trace)
        fprintf(stderr,"glDeleteShader\n");
    static PFNGLDELETESHADERPROC impl;
    if( !impl ){
       impl = (PFNGLDELETESHADERPROC) mygetprocaddr(env,"glDeleteShader");
       if(!impl)
            return;
    }
    impl(shader_);
}
//GLAPI void APIENTRY glDetachShader (GLuint program, GLuint shader);
JNIEXPORT void JNICALL Java_JGL_JGL_glDetachShader( JNIEnv* env, jclass klass,jint program_,jint shader_){
    if(do_trace)
        fprintf(stderr,"glDetachShader\n");
    static PFNGLDETACHSHADERPROC impl;
    if( !impl ){
       impl = (PFNGLDETACHSHADERPROC) mygetprocaddr(env,"glDetachShader");
       if(!impl)
            return;
    }
    impl(program_, shader_);
}
//GLAPI void APIENTRY glDisableVertexAttribArray (GLuint index);
JNIEXPORT void JNICALL Java_JGL_JGL_glDisableVertexAttribArray( JNIEnv* env, jclass klass,jint index_){
    if(do_trace)
        fprintf(stderr,"glDisableVertexAttribArray\n");
    static PFNGLDISABLEVERTEXATTRIBARRAYPROC impl;
    if( !impl ){
       impl = (PFNGLDISABLEVERTEXATTRIBARRAYPROC) mygetprocaddr(env,"glDisableVertexAttribArray");
       if(!impl)
            return;
    }
    impl(index_);
}
//GLAPI void APIENTRY glEnableVertexAttribArray (GLuint index);
JNIEXPORT void JNICALL Java_JGL_JGL_glEnableVertexAttribArray( JNIEnv* env, jclass klass,jint index_){
    if(do_trace)
        fprintf(stderr,"glEnableVertexAttribArray\n");
    static PFNGLENABLEVERTEXATTRIBARRAYPROC impl;
    if( !impl ){
       impl = (PFNGLENABLEVERTEXATTRIBARRAYPROC) mygetprocaddr(env,"glEnableVertexAttribArray");
       if(!impl)
            return;
    }
    impl(index_);
}
//GLAPI void APIENTRY glGetActiveAttrib (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetActiveAttrib( JNIEnv* env, jclass klass,jint program_,jint index_,jint bufSize_,jintArray length_,jintArray size_,jintArray type_,jbyteArray name_){
    if(do_trace)
        fprintf(stderr,"glGetActiveAttrib\n");
    static PFNGLGETACTIVEATTRIBPROC impl;
    if( !impl ){
       impl = (PFNGLGETACTIVEATTRIBPROC) mygetprocaddr(env,"glGetActiveAttrib");
       if(!impl)
            return;
    }
    jint* _length_ = (length_ ? ((*env)->GetIntArrayElements(env,length_,NULL) ) : NULL);
    jint* _size_ = (size_ ? ((*env)->GetIntArrayElements(env,size_,NULL) ) : NULL);
    jint* _type_ = (type_ ? ((*env)->GetIntArrayElements(env,type_,NULL) ) : NULL);
    jbyte* _name_ = (name_ ? ((*env)->GetByteArrayElements(env,name_,NULL) ) : NULL);
    impl(program_, index_, bufSize_, _length_, _size_, _type_, _name_);
    if(length_)
        (*env)->ReleaseIntArrayElements(env,length_, _length_,0);
    if(size_)
        (*env)->ReleaseIntArrayElements(env,size_, _size_,0);
    if(type_)
        (*env)->ReleaseIntArrayElements(env,type_, _type_,0);
    if(name_)
        (*env)->ReleaseByteArrayElements(env,name_, _name_,0);
}
//GLAPI void APIENTRY glGetActiveUniform (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLint *size, GLenum *type, GLchar *name);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetActiveUniform( JNIEnv* env, jclass klass,jint program_,jint index_,jint bufSize_,jintArray length_,jintArray size_,jintArray type_,jbyteArray name_){
    if(do_trace)
        fprintf(stderr,"glGetActiveUniform\n");
    static PFNGLGETACTIVEUNIFORMPROC impl;
    if( !impl ){
       impl = (PFNGLGETACTIVEUNIFORMPROC) mygetprocaddr(env,"glGetActiveUniform");
       if(!impl)
            return;
    }
    jint* _length_ = (length_ ? ((*env)->GetIntArrayElements(env,length_,NULL) ) : NULL);
    jint* _size_ = (size_ ? ((*env)->GetIntArrayElements(env,size_,NULL) ) : NULL);
    jint* _type_ = (type_ ? ((*env)->GetIntArrayElements(env,type_,NULL) ) : NULL);
    jbyte* _name_ = (name_ ? ((*env)->GetByteArrayElements(env,name_,NULL) ) : NULL);
    impl(program_, index_, bufSize_, _length_, _size_, _type_, _name_);
    if(length_)
        (*env)->ReleaseIntArrayElements(env,length_, _length_,0);
    if(size_)
        (*env)->ReleaseIntArrayElements(env,size_, _size_,0);
    if(type_)
        (*env)->ReleaseIntArrayElements(env,type_, _type_,0);
    if(name_)
        (*env)->ReleaseByteArrayElements(env,name_, _name_,0);
}
//GLAPI void APIENTRY glGetAttachedShaders (GLuint program, GLsizei maxCount, GLsizei *count, GLuint *shaders);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetAttachedShaders( JNIEnv* env, jclass klass,jint program_,jint maxCount_,jintArray count_,jintArray shaders_){
    if(do_trace)
        fprintf(stderr,"glGetAttachedShaders\n");
    static PFNGLGETATTACHEDSHADERSPROC impl;
    if( !impl ){
       impl = (PFNGLGETATTACHEDSHADERSPROC) mygetprocaddr(env,"glGetAttachedShaders");
       if(!impl)
            return;
    }
    jint* _count_ = (count_ ? ((*env)->GetIntArrayElements(env,count_,NULL) ) : NULL);
    jint* _shaders_ = (shaders_ ? ((*env)->GetIntArrayElements(env,shaders_,NULL) ) : NULL);
    impl(program_, maxCount_, _count_, _shaders_);
    if(count_)
        (*env)->ReleaseIntArrayElements(env,count_, _count_,0);
    if(shaders_)
        (*env)->ReleaseIntArrayElements(env,shaders_, _shaders_,0);
}
//GLAPI GLint APIENTRY glGetAttribLocation (GLuint program, const GLchar *name);
JNIEXPORT jint JNICALL Java_JGL_JGL_glGetAttribLocation( JNIEnv* env, jclass klass,jint program_,jbyteArray name_){
    jint rv = 0;
    if(do_trace)
        fprintf(stderr,"glGetAttribLocation\n");
    static PFNGLGETATTRIBLOCATIONPROC impl;
    if( !impl ){
       impl = (PFNGLGETATTRIBLOCATIONPROC) mygetprocaddr(env,"glGetAttribLocation");
       if(!impl)
            return rv;
    }
    jbyte* _name_ = (name_ ? ((*env)->GetByteArrayElements(env,name_,NULL) ) : NULL);
    rv = impl(program_, _name_);
    if(name_)
        (*env)->ReleaseByteArrayElements(env,name_, _name_,0);
    return rv;
}
//GLAPI void APIENTRY glGetProgramiv (GLuint program, GLenum pname, GLint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetProgramiv( JNIEnv* env, jclass klass,jint program_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetProgramiv\n");
    static PFNGLGETPROGRAMIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETPROGRAMIVPROC) mygetprocaddr(env,"glGetProgramiv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(program_, pname_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glGetProgramInfoLog (GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetProgramInfoLog( JNIEnv* env, jclass klass,jint program_,jint bufSize_,jintArray length_,jbyteArray infoLog_){
    if(do_trace)
        fprintf(stderr,"glGetProgramInfoLog\n");
    static PFNGLGETPROGRAMINFOLOGPROC impl;
    if( !impl ){
       impl = (PFNGLGETPROGRAMINFOLOGPROC) mygetprocaddr(env,"glGetProgramInfoLog");
       if(!impl)
            return;
    }
    jint* _length_ = (length_ ? ((*env)->GetIntArrayElements(env,length_,NULL) ) : NULL);
    jbyte* _infoLog_ = (infoLog_ ? ((*env)->GetByteArrayElements(env,infoLog_,NULL) ) : NULL);
    impl(program_, bufSize_, _length_, _infoLog_);
    if(length_)
        (*env)->ReleaseIntArrayElements(env,length_, _length_,0);
    if(infoLog_)
        (*env)->ReleaseByteArrayElements(env,infoLog_, _infoLog_,0);
}
//GLAPI void APIENTRY glGetShaderiv (GLuint shader, GLenum pname, GLint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetShaderiv( JNIEnv* env, jclass klass,jint shader_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetShaderiv\n");
    static PFNGLGETSHADERIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETSHADERIVPROC) mygetprocaddr(env,"glGetShaderiv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(shader_, pname_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glGetShaderInfoLog (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetShaderInfoLog( JNIEnv* env, jclass klass,jint shader_,jint bufSize_,jintArray length_,jbyteArray infoLog_){
    if(do_trace)
        fprintf(stderr,"glGetShaderInfoLog\n");
    static PFNGLGETSHADERINFOLOGPROC impl;
    if( !impl ){
       impl = (PFNGLGETSHADERINFOLOGPROC) mygetprocaddr(env,"glGetShaderInfoLog");
       if(!impl)
            return;
    }
    jint* _length_ = (length_ ? ((*env)->GetIntArrayElements(env,length_,NULL) ) : NULL);
    jbyte* _infoLog_ = (infoLog_ ? ((*env)->GetByteArrayElements(env,infoLog_,NULL) ) : NULL);
    impl(shader_, bufSize_, _length_, _infoLog_);
    if(length_)
        (*env)->ReleaseIntArrayElements(env,length_, _length_,0);
    if(infoLog_)
        (*env)->ReleaseByteArrayElements(env,infoLog_, _infoLog_,0);
}
//GLAPI void APIENTRY glGetShaderSource (GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *source);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetShaderSource( JNIEnv* env, jclass klass,jint shader_,jint bufSize_,jintArray length_,jbyteArray source_){
    if(do_trace)
        fprintf(stderr,"glGetShaderSource\n");
    static PFNGLGETSHADERSOURCEPROC impl;
    if( !impl ){
       impl = (PFNGLGETSHADERSOURCEPROC) mygetprocaddr(env,"glGetShaderSource");
       if(!impl)
            return;
    }
    jint* _length_ = (length_ ? ((*env)->GetIntArrayElements(env,length_,NULL) ) : NULL);
    jbyte* _source_ = (source_ ? ((*env)->GetByteArrayElements(env,source_,NULL) ) : NULL);
    impl(shader_, bufSize_, _length_, _source_);
    if(length_)
        (*env)->ReleaseIntArrayElements(env,length_, _length_,0);
    if(source_)
        (*env)->ReleaseByteArrayElements(env,source_, _source_,0);
}
//GLAPI GLint APIENTRY glGetUniformLocation (GLuint program, const GLchar *name);
JNIEXPORT jint JNICALL Java_JGL_JGL_glGetUniformLocation( JNIEnv* env, jclass klass,jint program_,jbyteArray name_){
    jint rv = 0;
    if(do_trace)
        fprintf(stderr,"glGetUniformLocation\n");
    static PFNGLGETUNIFORMLOCATIONPROC impl;
    if( !impl ){
       impl = (PFNGLGETUNIFORMLOCATIONPROC) mygetprocaddr(env,"glGetUniformLocation");
       if(!impl)
            return rv;
    }
    jbyte* _name_ = (name_ ? ((*env)->GetByteArrayElements(env,name_,NULL) ) : NULL);
    rv = impl(program_, _name_);
    if(name_)
        (*env)->ReleaseByteArrayElements(env,name_, _name_,0);
    return rv;
}
//GLAPI void APIENTRY glGetUniformfv (GLuint program, GLint location, GLfloat *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetUniformfv( JNIEnv* env, jclass klass,jint program_,jint location_,jfloatArray params_){
    if(do_trace)
        fprintf(stderr,"glGetUniformfv\n");
    static PFNGLGETUNIFORMFVPROC impl;
    if( !impl ){
       impl = (PFNGLGETUNIFORMFVPROC) mygetprocaddr(env,"glGetUniformfv");
       if(!impl)
            return;
    }
    jfloat* _params_ = (params_ ? ((*env)->GetFloatArrayElements(env,params_,NULL) ) : NULL);
    impl(program_, location_, _params_);
    if(params_)
        (*env)->ReleaseFloatArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glGetUniformiv (GLuint program, GLint location, GLint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetUniformiv( JNIEnv* env, jclass klass,jint program_,jint location_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetUniformiv\n");
    static PFNGLGETUNIFORMIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETUNIFORMIVPROC) mygetprocaddr(env,"glGetUniformiv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(program_, location_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glGetVertexAttribdv (GLuint index, GLenum pname, GLdouble *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetVertexAttribdv( JNIEnv* env, jclass klass,jint index_,jint pname_,jdoubleArray params_){
    if(do_trace)
        fprintf(stderr,"glGetVertexAttribdv\n");
    static PFNGLGETVERTEXATTRIBDVPROC impl;
    if( !impl ){
       impl = (PFNGLGETVERTEXATTRIBDVPROC) mygetprocaddr(env,"glGetVertexAttribdv");
       if(!impl)
            return;
    }
    jdouble* _params_ = (params_ ? ((*env)->GetDoubleArrayElements(env,params_,NULL) ) : NULL);
    impl(index_, pname_, _params_);
    if(params_)
        (*env)->ReleaseDoubleArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glGetVertexAttribfv (GLuint index, GLenum pname, GLfloat *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetVertexAttribfv( JNIEnv* env, jclass klass,jint index_,jint pname_,jfloatArray params_){
    if(do_trace)
        fprintf(stderr,"glGetVertexAttribfv\n");
    static PFNGLGETVERTEXATTRIBFVPROC impl;
    if( !impl ){
       impl = (PFNGLGETVERTEXATTRIBFVPROC) mygetprocaddr(env,"glGetVertexAttribfv");
       if(!impl)
            return;
    }
    jfloat* _params_ = (params_ ? ((*env)->GetFloatArrayElements(env,params_,NULL) ) : NULL);
    impl(index_, pname_, _params_);
    if(params_)
        (*env)->ReleaseFloatArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glGetVertexAttribiv (GLuint index, GLenum pname, GLint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetVertexAttribiv( JNIEnv* env, jclass klass,jint index_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetVertexAttribiv\n");
    static PFNGLGETVERTEXATTRIBIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETVERTEXATTRIBIVPROC) mygetprocaddr(env,"glGetVertexAttribiv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(index_, pname_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI GLboolean APIENTRY glIsProgram (GLuint program);
JNIEXPORT jboolean JNICALL Java_JGL_JGL_glIsProgram( JNIEnv* env, jclass klass,jint program_){
    jboolean rv = 0;
    if(do_trace)
        fprintf(stderr,"glIsProgram\n");
    static PFNGLISPROGRAMPROC impl;
    if( !impl ){
       impl = (PFNGLISPROGRAMPROC) mygetprocaddr(env,"glIsProgram");
       if(!impl)
            return rv;
    }
    rv = impl(program_);
    return rv;
}
//GLAPI GLboolean APIENTRY glIsShader (GLuint shader);
JNIEXPORT jboolean JNICALL Java_JGL_JGL_glIsShader( JNIEnv* env, jclass klass,jint shader_){
    jboolean rv = 0;
    if(do_trace)
        fprintf(stderr,"glIsShader\n");
    static PFNGLISSHADERPROC impl;
    if( !impl ){
       impl = (PFNGLISSHADERPROC) mygetprocaddr(env,"glIsShader");
       if(!impl)
            return rv;
    }
    rv = impl(shader_);
    return rv;
}
//GLAPI void APIENTRY glLinkProgram (GLuint program);
JNIEXPORT void JNICALL Java_JGL_JGL_glLinkProgram( JNIEnv* env, jclass klass,jint program_){
    if(do_trace)
        fprintf(stderr,"glLinkProgram\n");
    static PFNGLLINKPROGRAMPROC impl;
    if( !impl ){
       impl = (PFNGLLINKPROGRAMPROC) mygetprocaddr(env,"glLinkProgram");
       if(!impl)
            return;
    }
    impl(program_);
}
//GLAPI void APIENTRY glUseProgram (GLuint program);
JNIEXPORT void JNICALL Java_JGL_JGL_glUseProgram( JNIEnv* env, jclass klass,jint program_){
    if(do_trace)
        fprintf(stderr,"glUseProgram\n");
    static PFNGLUSEPROGRAMPROC impl;
    if( !impl ){
       impl = (PFNGLUSEPROGRAMPROC) mygetprocaddr(env,"glUseProgram");
       if(!impl)
            return;
    }
    impl(program_);
}
//GLAPI void APIENTRY glUniform1f (GLint location, GLfloat v0);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniform1f( JNIEnv* env, jclass klass,jint location_,jfloat v0_){
    if(do_trace)
        fprintf(stderr,"glUniform1f\n");
    static PFNGLUNIFORM1FPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORM1FPROC) mygetprocaddr(env,"glUniform1f");
       if(!impl)
            return;
    }
    impl(location_, v0_);
}
//GLAPI void APIENTRY glUniform2f (GLint location, GLfloat v0, GLfloat v1);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniform2f( JNIEnv* env, jclass klass,jint location_,jfloat v0_,jfloat v1_){
    if(do_trace)
        fprintf(stderr,"glUniform2f\n");
    static PFNGLUNIFORM2FPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORM2FPROC) mygetprocaddr(env,"glUniform2f");
       if(!impl)
            return;
    }
    impl(location_, v0_, v1_);
}
//GLAPI void APIENTRY glUniform3f (GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniform3f( JNIEnv* env, jclass klass,jint location_,jfloat v0_,jfloat v1_,jfloat v2_){
    if(do_trace)
        fprintf(stderr,"glUniform3f\n");
    static PFNGLUNIFORM3FPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORM3FPROC) mygetprocaddr(env,"glUniform3f");
       if(!impl)
            return;
    }
    impl(location_, v0_, v1_, v2_);
}
//GLAPI void APIENTRY glUniform4f (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniform4f( JNIEnv* env, jclass klass,jint location_,jfloat v0_,jfloat v1_,jfloat v2_,jfloat v3_){
    if(do_trace)
        fprintf(stderr,"glUniform4f\n");
    static PFNGLUNIFORM4FPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORM4FPROC) mygetprocaddr(env,"glUniform4f");
       if(!impl)
            return;
    }
    impl(location_, v0_, v1_, v2_, v3_);
}
//GLAPI void APIENTRY glUniform1i (GLint location, GLint v0);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniform1i( JNIEnv* env, jclass klass,jint location_,jint v0_){
    if(do_trace)
        fprintf(stderr,"glUniform1i\n");
    static PFNGLUNIFORM1IPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORM1IPROC) mygetprocaddr(env,"glUniform1i");
       if(!impl)
            return;
    }
    impl(location_, v0_);
}
//GLAPI void APIENTRY glUniform2i (GLint location, GLint v0, GLint v1);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniform2i( JNIEnv* env, jclass klass,jint location_,jint v0_,jint v1_){
    if(do_trace)
        fprintf(stderr,"glUniform2i\n");
    static PFNGLUNIFORM2IPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORM2IPROC) mygetprocaddr(env,"glUniform2i");
       if(!impl)
            return;
    }
    impl(location_, v0_, v1_);
}
//GLAPI void APIENTRY glUniform3i (GLint location, GLint v0, GLint v1, GLint v2);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniform3i( JNIEnv* env, jclass klass,jint location_,jint v0_,jint v1_,jint v2_){
    if(do_trace)
        fprintf(stderr,"glUniform3i\n");
    static PFNGLUNIFORM3IPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORM3IPROC) mygetprocaddr(env,"glUniform3i");
       if(!impl)
            return;
    }
    impl(location_, v0_, v1_, v2_);
}
//GLAPI void APIENTRY glUniform4i (GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniform4i( JNIEnv* env, jclass klass,jint location_,jint v0_,jint v1_,jint v2_,jint v3_){
    if(do_trace)
        fprintf(stderr,"glUniform4i\n");
    static PFNGLUNIFORM4IPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORM4IPROC) mygetprocaddr(env,"glUniform4i");
       if(!impl)
            return;
    }
    impl(location_, v0_, v1_, v2_, v3_);
}
//GLAPI void APIENTRY glUniform1fv (GLint location, GLsizei count, const GLfloat *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniform1fv( JNIEnv* env, jclass klass,jint location_,jint count_,jfloatArray value_){
    if(do_trace)
        fprintf(stderr,"glUniform1fv\n");
    static PFNGLUNIFORM1FVPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORM1FVPROC) mygetprocaddr(env,"glUniform1fv");
       if(!impl)
            return;
    }
    jfloat* _value_ = (value_ ? ((*env)->GetFloatArrayElements(env,value_,NULL) ) : NULL);
    impl(location_, count_, _value_);
    if(value_)
        (*env)->ReleaseFloatArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glUniform2fv (GLint location, GLsizei count, const GLfloat *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniform2fv( JNIEnv* env, jclass klass,jint location_,jint count_,jfloatArray value_){
    if(do_trace)
        fprintf(stderr,"glUniform2fv\n");
    static PFNGLUNIFORM2FVPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORM2FVPROC) mygetprocaddr(env,"glUniform2fv");
       if(!impl)
            return;
    }
    jfloat* _value_ = (value_ ? ((*env)->GetFloatArrayElements(env,value_,NULL) ) : NULL);
    impl(location_, count_, _value_);
    if(value_)
        (*env)->ReleaseFloatArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glUniform3fv (GLint location, GLsizei count, const GLfloat *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniform3fv( JNIEnv* env, jclass klass,jint location_,jint count_,jfloatArray value_){
    if(do_trace)
        fprintf(stderr,"glUniform3fv\n");
    static PFNGLUNIFORM3FVPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORM3FVPROC) mygetprocaddr(env,"glUniform3fv");
       if(!impl)
            return;
    }
    jfloat* _value_ = (value_ ? ((*env)->GetFloatArrayElements(env,value_,NULL) ) : NULL);
    impl(location_, count_, _value_);
    if(value_)
        (*env)->ReleaseFloatArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glUniform4fv (GLint location, GLsizei count, const GLfloat *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniform4fv( JNIEnv* env, jclass klass,jint location_,jint count_,jfloatArray value_){
    if(do_trace)
        fprintf(stderr,"glUniform4fv\n");
    static PFNGLUNIFORM4FVPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORM4FVPROC) mygetprocaddr(env,"glUniform4fv");
       if(!impl)
            return;
    }
    jfloat* _value_ = (value_ ? ((*env)->GetFloatArrayElements(env,value_,NULL) ) : NULL);
    impl(location_, count_, _value_);
    if(value_)
        (*env)->ReleaseFloatArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glUniform1iv (GLint location, GLsizei count, const GLint *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniform1iv( JNIEnv* env, jclass klass,jint location_,jint count_,jintArray value_){
    if(do_trace)
        fprintf(stderr,"glUniform1iv\n");
    static PFNGLUNIFORM1IVPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORM1IVPROC) mygetprocaddr(env,"glUniform1iv");
       if(!impl)
            return;
    }
    jint* _value_ = (value_ ? ((*env)->GetIntArrayElements(env,value_,NULL) ) : NULL);
    impl(location_, count_, _value_);
    if(value_)
        (*env)->ReleaseIntArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glUniform2iv (GLint location, GLsizei count, const GLint *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniform2iv( JNIEnv* env, jclass klass,jint location_,jint count_,jintArray value_){
    if(do_trace)
        fprintf(stderr,"glUniform2iv\n");
    static PFNGLUNIFORM2IVPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORM2IVPROC) mygetprocaddr(env,"glUniform2iv");
       if(!impl)
            return;
    }
    jint* _value_ = (value_ ? ((*env)->GetIntArrayElements(env,value_,NULL) ) : NULL);
    impl(location_, count_, _value_);
    if(value_)
        (*env)->ReleaseIntArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glUniform3iv (GLint location, GLsizei count, const GLint *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniform3iv( JNIEnv* env, jclass klass,jint location_,jint count_,jintArray value_){
    if(do_trace)
        fprintf(stderr,"glUniform3iv\n");
    static PFNGLUNIFORM3IVPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORM3IVPROC) mygetprocaddr(env,"glUniform3iv");
       if(!impl)
            return;
    }
    jint* _value_ = (value_ ? ((*env)->GetIntArrayElements(env,value_,NULL) ) : NULL);
    impl(location_, count_, _value_);
    if(value_)
        (*env)->ReleaseIntArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glUniform4iv (GLint location, GLsizei count, const GLint *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniform4iv( JNIEnv* env, jclass klass,jint location_,jint count_,jintArray value_){
    if(do_trace)
        fprintf(stderr,"glUniform4iv\n");
    static PFNGLUNIFORM4IVPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORM4IVPROC) mygetprocaddr(env,"glUniform4iv");
       if(!impl)
            return;
    }
    jint* _value_ = (value_ ? ((*env)->GetIntArrayElements(env,value_,NULL) ) : NULL);
    impl(location_, count_, _value_);
    if(value_)
        (*env)->ReleaseIntArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glUniformMatrix2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniformMatrix2fv( JNIEnv* env, jclass klass,jint location_,jint count_,jboolean transpose_,jfloatArray value_){
    if(do_trace)
        fprintf(stderr,"glUniformMatrix2fv\n");
    static PFNGLUNIFORMMATRIX2FVPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORMMATRIX2FVPROC) mygetprocaddr(env,"glUniformMatrix2fv");
       if(!impl)
            return;
    }
    jfloat* _value_ = (value_ ? ((*env)->GetFloatArrayElements(env,value_,NULL) ) : NULL);
    impl(location_, count_, transpose_, _value_);
    if(value_)
        (*env)->ReleaseFloatArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glUniformMatrix3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniformMatrix3fv( JNIEnv* env, jclass klass,jint location_,jint count_,jboolean transpose_,jfloatArray value_){
    if(do_trace)
        fprintf(stderr,"glUniformMatrix3fv\n");
    static PFNGLUNIFORMMATRIX3FVPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORMMATRIX3FVPROC) mygetprocaddr(env,"glUniformMatrix3fv");
       if(!impl)
            return;
    }
    jfloat* _value_ = (value_ ? ((*env)->GetFloatArrayElements(env,value_,NULL) ) : NULL);
    impl(location_, count_, transpose_, _value_);
    if(value_)
        (*env)->ReleaseFloatArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glUniformMatrix4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniformMatrix4fv( JNIEnv* env, jclass klass,jint location_,jint count_,jboolean transpose_,jfloatArray value_){
    if(do_trace)
        fprintf(stderr,"glUniformMatrix4fv\n");
    static PFNGLUNIFORMMATRIX4FVPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORMMATRIX4FVPROC) mygetprocaddr(env,"glUniformMatrix4fv");
       if(!impl)
            return;
    }
    jfloat* _value_ = (value_ ? ((*env)->GetFloatArrayElements(env,value_,NULL) ) : NULL);
    impl(location_, count_, transpose_, _value_);
    if(value_)
        (*env)->ReleaseFloatArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glValidateProgram (GLuint program);
JNIEXPORT void JNICALL Java_JGL_JGL_glValidateProgram( JNIEnv* env, jclass klass,jint program_){
    if(do_trace)
        fprintf(stderr,"glValidateProgram\n");
    static PFNGLVALIDATEPROGRAMPROC impl;
    if( !impl ){
       impl = (PFNGLVALIDATEPROGRAMPROC) mygetprocaddr(env,"glValidateProgram");
       if(!impl)
            return;
    }
    impl(program_);
}
//GLAPI void APIENTRY glVertexAttrib1d (GLuint index, GLdouble x);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttrib1d( JNIEnv* env, jclass klass,jint index_,jdouble x_){
    if(do_trace)
        fprintf(stderr,"glVertexAttrib1d\n");
    static PFNGLVERTEXATTRIB1DPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIB1DPROC) mygetprocaddr(env,"glVertexAttrib1d");
       if(!impl)
            return;
    }
    impl(index_, x_);
}
//GLAPI void APIENTRY glVertexAttrib1dv (GLuint index, const GLdouble *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttrib1dv( JNIEnv* env, jclass klass,jint index_,jdoubleArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttrib1dv\n");
    static PFNGLVERTEXATTRIB1DVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIB1DVPROC) mygetprocaddr(env,"glVertexAttrib1dv");
       if(!impl)
            return;
    }
    jdouble* _v_ = (v_ ? ((*env)->GetDoubleArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseDoubleArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttrib1f (GLuint index, GLfloat x);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttrib1f( JNIEnv* env, jclass klass,jint index_,jfloat x_){
    if(do_trace)
        fprintf(stderr,"glVertexAttrib1f\n");
    static PFNGLVERTEXATTRIB1FPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIB1FPROC) mygetprocaddr(env,"glVertexAttrib1f");
       if(!impl)
            return;
    }
    impl(index_, x_);
}
//GLAPI void APIENTRY glVertexAttrib1fv (GLuint index, const GLfloat *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttrib1fv( JNIEnv* env, jclass klass,jint index_,jfloatArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttrib1fv\n");
    static PFNGLVERTEXATTRIB1FVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIB1FVPROC) mygetprocaddr(env,"glVertexAttrib1fv");
       if(!impl)
            return;
    }
    jfloat* _v_ = (v_ ? ((*env)->GetFloatArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseFloatArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttrib1s (GLuint index, GLshort x);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttrib1s( JNIEnv* env, jclass klass,jint index_,jshort x_){
    if(do_trace)
        fprintf(stderr,"glVertexAttrib1s\n");
    static PFNGLVERTEXATTRIB1SPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIB1SPROC) mygetprocaddr(env,"glVertexAttrib1s");
       if(!impl)
            return;
    }
    impl(index_, x_);
}
//GLAPI void APIENTRY glVertexAttrib1sv (GLuint index, const GLshort *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttrib1sv( JNIEnv* env, jclass klass,jint index_,jshortArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttrib1sv\n");
    static PFNGLVERTEXATTRIB1SVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIB1SVPROC) mygetprocaddr(env,"glVertexAttrib1sv");
       if(!impl)
            return;
    }
    jshort* _v_ = (v_ ? ((*env)->GetShortArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseShortArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttrib2d (GLuint index, GLdouble x, GLdouble y);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttrib2d( JNIEnv* env, jclass klass,jint index_,jdouble x_,jdouble y_){
    if(do_trace)
        fprintf(stderr,"glVertexAttrib2d\n");
    static PFNGLVERTEXATTRIB2DPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIB2DPROC) mygetprocaddr(env,"glVertexAttrib2d");
       if(!impl)
            return;
    }
    impl(index_, x_, y_);
}
//GLAPI void APIENTRY glVertexAttrib2dv (GLuint index, const GLdouble *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttrib2dv( JNIEnv* env, jclass klass,jint index_,jdoubleArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttrib2dv\n");
    static PFNGLVERTEXATTRIB2DVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIB2DVPROC) mygetprocaddr(env,"glVertexAttrib2dv");
       if(!impl)
            return;
    }
    jdouble* _v_ = (v_ ? ((*env)->GetDoubleArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseDoubleArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttrib2f (GLuint index, GLfloat x, GLfloat y);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttrib2f( JNIEnv* env, jclass klass,jint index_,jfloat x_,jfloat y_){
    if(do_trace)
        fprintf(stderr,"glVertexAttrib2f\n");
    static PFNGLVERTEXATTRIB2FPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIB2FPROC) mygetprocaddr(env,"glVertexAttrib2f");
       if(!impl)
            return;
    }
    impl(index_, x_, y_);
}
//GLAPI void APIENTRY glVertexAttrib2fv (GLuint index, const GLfloat *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttrib2fv( JNIEnv* env, jclass klass,jint index_,jfloatArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttrib2fv\n");
    static PFNGLVERTEXATTRIB2FVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIB2FVPROC) mygetprocaddr(env,"glVertexAttrib2fv");
       if(!impl)
            return;
    }
    jfloat* _v_ = (v_ ? ((*env)->GetFloatArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseFloatArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttrib2s (GLuint index, GLshort x, GLshort y);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttrib2s( JNIEnv* env, jclass klass,jint index_,jshort x_,jshort y_){
    if(do_trace)
        fprintf(stderr,"glVertexAttrib2s\n");
    static PFNGLVERTEXATTRIB2SPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIB2SPROC) mygetprocaddr(env,"glVertexAttrib2s");
       if(!impl)
            return;
    }
    impl(index_, x_, y_);
}
//GLAPI void APIENTRY glVertexAttrib2sv (GLuint index, const GLshort *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttrib2sv( JNIEnv* env, jclass klass,jint index_,jshortArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttrib2sv\n");
    static PFNGLVERTEXATTRIB2SVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIB2SVPROC) mygetprocaddr(env,"glVertexAttrib2sv");
       if(!impl)
            return;
    }
    jshort* _v_ = (v_ ? ((*env)->GetShortArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseShortArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttrib3d (GLuint index, GLdouble x, GLdouble y, GLdouble z);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttrib3d( JNIEnv* env, jclass klass,jint index_,jdouble x_,jdouble y_,jdouble z_){
    if(do_trace)
        fprintf(stderr,"glVertexAttrib3d\n");
    static PFNGLVERTEXATTRIB3DPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIB3DPROC) mygetprocaddr(env,"glVertexAttrib3d");
       if(!impl)
            return;
    }
    impl(index_, x_, y_, z_);
}
//GLAPI void APIENTRY glVertexAttrib3dv (GLuint index, const GLdouble *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttrib3dv( JNIEnv* env, jclass klass,jint index_,jdoubleArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttrib3dv\n");
    static PFNGLVERTEXATTRIB3DVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIB3DVPROC) mygetprocaddr(env,"glVertexAttrib3dv");
       if(!impl)
            return;
    }
    jdouble* _v_ = (v_ ? ((*env)->GetDoubleArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseDoubleArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttrib3f (GLuint index, GLfloat x, GLfloat y, GLfloat z);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttrib3f( JNIEnv* env, jclass klass,jint index_,jfloat x_,jfloat y_,jfloat z_){
    if(do_trace)
        fprintf(stderr,"glVertexAttrib3f\n");
    static PFNGLVERTEXATTRIB3FPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIB3FPROC) mygetprocaddr(env,"glVertexAttrib3f");
       if(!impl)
            return;
    }
    impl(index_, x_, y_, z_);
}
//GLAPI void APIENTRY glVertexAttrib3fv (GLuint index, const GLfloat *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttrib3fv( JNIEnv* env, jclass klass,jint index_,jfloatArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttrib3fv\n");
    static PFNGLVERTEXATTRIB3FVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIB3FVPROC) mygetprocaddr(env,"glVertexAttrib3fv");
       if(!impl)
            return;
    }
    jfloat* _v_ = (v_ ? ((*env)->GetFloatArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseFloatArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttrib3s (GLuint index, GLshort x, GLshort y, GLshort z);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttrib3s( JNIEnv* env, jclass klass,jint index_,jshort x_,jshort y_,jshort z_){
    if(do_trace)
        fprintf(stderr,"glVertexAttrib3s\n");
    static PFNGLVERTEXATTRIB3SPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIB3SPROC) mygetprocaddr(env,"glVertexAttrib3s");
       if(!impl)
            return;
    }
    impl(index_, x_, y_, z_);
}
//GLAPI void APIENTRY glVertexAttrib3sv (GLuint index, const GLshort *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttrib3sv( JNIEnv* env, jclass klass,jint index_,jshortArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttrib3sv\n");
    static PFNGLVERTEXATTRIB3SVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIB3SVPROC) mygetprocaddr(env,"glVertexAttrib3sv");
       if(!impl)
            return;
    }
    jshort* _v_ = (v_ ? ((*env)->GetShortArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseShortArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttrib4Nbv (GLuint index, const GLbyte *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttrib4Nbv( JNIEnv* env, jclass klass,jint index_,jbyteArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttrib4Nbv\n");
    static PFNGLVERTEXATTRIB4NBVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIB4NBVPROC) mygetprocaddr(env,"glVertexAttrib4Nbv");
       if(!impl)
            return;
    }
    jbyte* _v_ = (v_ ? ((*env)->GetByteArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseByteArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttrib4Niv (GLuint index, const GLint *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttrib4Niv( JNIEnv* env, jclass klass,jint index_,jintArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttrib4Niv\n");
    static PFNGLVERTEXATTRIB4NIVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIB4NIVPROC) mygetprocaddr(env,"glVertexAttrib4Niv");
       if(!impl)
            return;
    }
    jint* _v_ = (v_ ? ((*env)->GetIntArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseIntArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttrib4Nsv (GLuint index, const GLshort *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttrib4Nsv( JNIEnv* env, jclass klass,jint index_,jshortArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttrib4Nsv\n");
    static PFNGLVERTEXATTRIB4NSVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIB4NSVPROC) mygetprocaddr(env,"glVertexAttrib4Nsv");
       if(!impl)
            return;
    }
    jshort* _v_ = (v_ ? ((*env)->GetShortArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseShortArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttrib4Nub (GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttrib4Nub( JNIEnv* env, jclass klass,jint index_,jbyte x_,jbyte y_,jbyte z_,jbyte w_){
    if(do_trace)
        fprintf(stderr,"glVertexAttrib4Nub\n");
    static PFNGLVERTEXATTRIB4NUBPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIB4NUBPROC) mygetprocaddr(env,"glVertexAttrib4Nub");
       if(!impl)
            return;
    }
    impl(index_, x_, y_, z_, w_);
}
//GLAPI void APIENTRY glVertexAttrib4Nubv (GLuint index, const GLubyte *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttrib4Nubv( JNIEnv* env, jclass klass,jint index_,jbyteArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttrib4Nubv\n");
    static PFNGLVERTEXATTRIB4NUBVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIB4NUBVPROC) mygetprocaddr(env,"glVertexAttrib4Nubv");
       if(!impl)
            return;
    }
    jbyte* _v_ = (v_ ? ((*env)->GetByteArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseByteArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttrib4Nuiv (GLuint index, const GLuint *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttrib4Nuiv( JNIEnv* env, jclass klass,jint index_,jintArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttrib4Nuiv\n");
    static PFNGLVERTEXATTRIB4NUIVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIB4NUIVPROC) mygetprocaddr(env,"glVertexAttrib4Nuiv");
       if(!impl)
            return;
    }
    jint* _v_ = (v_ ? ((*env)->GetIntArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseIntArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttrib4Nusv (GLuint index, const GLushort *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttrib4Nusv( JNIEnv* env, jclass klass,jint index_,jshortArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttrib4Nusv\n");
    static PFNGLVERTEXATTRIB4NUSVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIB4NUSVPROC) mygetprocaddr(env,"glVertexAttrib4Nusv");
       if(!impl)
            return;
    }
    jshort* _v_ = (v_ ? ((*env)->GetShortArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseShortArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttrib4bv (GLuint index, const GLbyte *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttrib4bv( JNIEnv* env, jclass klass,jint index_,jbyteArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttrib4bv\n");
    static PFNGLVERTEXATTRIB4BVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIB4BVPROC) mygetprocaddr(env,"glVertexAttrib4bv");
       if(!impl)
            return;
    }
    jbyte* _v_ = (v_ ? ((*env)->GetByteArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseByteArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttrib4d (GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttrib4d( JNIEnv* env, jclass klass,jint index_,jdouble x_,jdouble y_,jdouble z_,jdouble w_){
    if(do_trace)
        fprintf(stderr,"glVertexAttrib4d\n");
    static PFNGLVERTEXATTRIB4DPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIB4DPROC) mygetprocaddr(env,"glVertexAttrib4d");
       if(!impl)
            return;
    }
    impl(index_, x_, y_, z_, w_);
}
//GLAPI void APIENTRY glVertexAttrib4dv (GLuint index, const GLdouble *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttrib4dv( JNIEnv* env, jclass klass,jint index_,jdoubleArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttrib4dv\n");
    static PFNGLVERTEXATTRIB4DVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIB4DVPROC) mygetprocaddr(env,"glVertexAttrib4dv");
       if(!impl)
            return;
    }
    jdouble* _v_ = (v_ ? ((*env)->GetDoubleArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseDoubleArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttrib4f (GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttrib4f( JNIEnv* env, jclass klass,jint index_,jfloat x_,jfloat y_,jfloat z_,jfloat w_){
    if(do_trace)
        fprintf(stderr,"glVertexAttrib4f\n");
    static PFNGLVERTEXATTRIB4FPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIB4FPROC) mygetprocaddr(env,"glVertexAttrib4f");
       if(!impl)
            return;
    }
    impl(index_, x_, y_, z_, w_);
}
//GLAPI void APIENTRY glVertexAttrib4fv (GLuint index, const GLfloat *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttrib4fv( JNIEnv* env, jclass klass,jint index_,jfloatArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttrib4fv\n");
    static PFNGLVERTEXATTRIB4FVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIB4FVPROC) mygetprocaddr(env,"glVertexAttrib4fv");
       if(!impl)
            return;
    }
    jfloat* _v_ = (v_ ? ((*env)->GetFloatArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseFloatArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttrib4iv (GLuint index, const GLint *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttrib4iv( JNIEnv* env, jclass klass,jint index_,jintArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttrib4iv\n");
    static PFNGLVERTEXATTRIB4IVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIB4IVPROC) mygetprocaddr(env,"glVertexAttrib4iv");
       if(!impl)
            return;
    }
    jint* _v_ = (v_ ? ((*env)->GetIntArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseIntArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttrib4s (GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttrib4s( JNIEnv* env, jclass klass,jint index_,jshort x_,jshort y_,jshort z_,jshort w_){
    if(do_trace)
        fprintf(stderr,"glVertexAttrib4s\n");
    static PFNGLVERTEXATTRIB4SPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIB4SPROC) mygetprocaddr(env,"glVertexAttrib4s");
       if(!impl)
            return;
    }
    impl(index_, x_, y_, z_, w_);
}
//GLAPI void APIENTRY glVertexAttrib4sv (GLuint index, const GLshort *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttrib4sv( JNIEnv* env, jclass klass,jint index_,jshortArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttrib4sv\n");
    static PFNGLVERTEXATTRIB4SVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIB4SVPROC) mygetprocaddr(env,"glVertexAttrib4sv");
       if(!impl)
            return;
    }
    jshort* _v_ = (v_ ? ((*env)->GetShortArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseShortArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttrib4ubv (GLuint index, const GLubyte *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttrib4ubv( JNIEnv* env, jclass klass,jint index_,jbyteArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttrib4ubv\n");
    static PFNGLVERTEXATTRIB4UBVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIB4UBVPROC) mygetprocaddr(env,"glVertexAttrib4ubv");
       if(!impl)
            return;
    }
    jbyte* _v_ = (v_ ? ((*env)->GetByteArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseByteArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttrib4uiv (GLuint index, const GLuint *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttrib4uiv( JNIEnv* env, jclass klass,jint index_,jintArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttrib4uiv\n");
    static PFNGLVERTEXATTRIB4UIVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIB4UIVPROC) mygetprocaddr(env,"glVertexAttrib4uiv");
       if(!impl)
            return;
    }
    jint* _v_ = (v_ ? ((*env)->GetIntArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseIntArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttrib4usv (GLuint index, const GLushort *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttrib4usv( JNIEnv* env, jclass klass,jint index_,jshortArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttrib4usv\n");
    static PFNGLVERTEXATTRIB4USVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIB4USVPROC) mygetprocaddr(env,"glVertexAttrib4usv");
       if(!impl)
            return;
    }
    jshort* _v_ = (v_ ? ((*env)->GetShortArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseShortArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttribPointer (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribPointer( JNIEnv* env, jclass klass,jint index_,jint size_,jint type_,jboolean normalized_,jint stride_,jint pointer_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribPointer\n");
    static PFNGLVERTEXATTRIBPOINTERPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBPOINTERPROC) mygetprocaddr(env,"glVertexAttribPointer");
       if(!impl)
            return;
    }
    impl(index_, size_, type_, (GLboolean)normalized_, stride_, (const void *)pointer_);
}
//GLAPI void APIENTRY glUniformMatrix2x3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniformMatrix2x3fv( JNIEnv* env, jclass klass,jint location_,jint count_,jboolean transpose_,jfloatArray value_){
    if(do_trace)
        fprintf(stderr,"glUniformMatrix2x3fv\n");
    static PFNGLUNIFORMMATRIX2X3FVPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORMMATRIX2X3FVPROC) mygetprocaddr(env,"glUniformMatrix2x3fv");
       if(!impl)
            return;
    }
    jfloat* _value_ = (value_ ? ((*env)->GetFloatArrayElements(env,value_,NULL) ) : NULL);
    impl(location_, count_, transpose_, _value_);
    if(value_)
        (*env)->ReleaseFloatArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glUniformMatrix3x2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniformMatrix3x2fv( JNIEnv* env, jclass klass,jint location_,jint count_,jboolean transpose_,jfloatArray value_){
    if(do_trace)
        fprintf(stderr,"glUniformMatrix3x2fv\n");
    static PFNGLUNIFORMMATRIX3X2FVPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORMMATRIX3X2FVPROC) mygetprocaddr(env,"glUniformMatrix3x2fv");
       if(!impl)
            return;
    }
    jfloat* _value_ = (value_ ? ((*env)->GetFloatArrayElements(env,value_,NULL) ) : NULL);
    impl(location_, count_, transpose_, _value_);
    if(value_)
        (*env)->ReleaseFloatArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glUniformMatrix2x4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniformMatrix2x4fv( JNIEnv* env, jclass klass,jint location_,jint count_,jboolean transpose_,jfloatArray value_){
    if(do_trace)
        fprintf(stderr,"glUniformMatrix2x4fv\n");
    static PFNGLUNIFORMMATRIX2X4FVPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORMMATRIX2X4FVPROC) mygetprocaddr(env,"glUniformMatrix2x4fv");
       if(!impl)
            return;
    }
    jfloat* _value_ = (value_ ? ((*env)->GetFloatArrayElements(env,value_,NULL) ) : NULL);
    impl(location_, count_, transpose_, _value_);
    if(value_)
        (*env)->ReleaseFloatArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glUniformMatrix4x2fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniformMatrix4x2fv( JNIEnv* env, jclass klass,jint location_,jint count_,jboolean transpose_,jfloatArray value_){
    if(do_trace)
        fprintf(stderr,"glUniformMatrix4x2fv\n");
    static PFNGLUNIFORMMATRIX4X2FVPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORMMATRIX4X2FVPROC) mygetprocaddr(env,"glUniformMatrix4x2fv");
       if(!impl)
            return;
    }
    jfloat* _value_ = (value_ ? ((*env)->GetFloatArrayElements(env,value_,NULL) ) : NULL);
    impl(location_, count_, transpose_, _value_);
    if(value_)
        (*env)->ReleaseFloatArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glUniformMatrix3x4fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniformMatrix3x4fv( JNIEnv* env, jclass klass,jint location_,jint count_,jboolean transpose_,jfloatArray value_){
    if(do_trace)
        fprintf(stderr,"glUniformMatrix3x4fv\n");
    static PFNGLUNIFORMMATRIX3X4FVPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORMMATRIX3X4FVPROC) mygetprocaddr(env,"glUniformMatrix3x4fv");
       if(!impl)
            return;
    }
    jfloat* _value_ = (value_ ? ((*env)->GetFloatArrayElements(env,value_,NULL) ) : NULL);
    impl(location_, count_, transpose_, _value_);
    if(value_)
        (*env)->ReleaseFloatArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glUniformMatrix4x3fv (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniformMatrix4x3fv( JNIEnv* env, jclass klass,jint location_,jint count_,jboolean transpose_,jfloatArray value_){
    if(do_trace)
        fprintf(stderr,"glUniformMatrix4x3fv\n");
    static PFNGLUNIFORMMATRIX4X3FVPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORMMATRIX4X3FVPROC) mygetprocaddr(env,"glUniformMatrix4x3fv");
       if(!impl)
            return;
    }
    jfloat* _value_ = (value_ ? ((*env)->GetFloatArrayElements(env,value_,NULL) ) : NULL);
    impl(location_, count_, transpose_, _value_);
    if(value_)
        (*env)->ReleaseFloatArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glColorMaski (GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a);
JNIEXPORT void JNICALL Java_JGL_JGL_glColorMaski( JNIEnv* env, jclass klass,jint index_,jboolean r_,jboolean g_,jboolean b_,jboolean a_){
    if(do_trace)
        fprintf(stderr,"glColorMaski\n");
    static PFNGLCOLORMASKIPROC impl;
    if( !impl ){
       impl = (PFNGLCOLORMASKIPROC) mygetprocaddr(env,"glColorMaski");
       if(!impl)
            return;
    }
    impl(index_, r_, g_, b_, a_);
}
//GLAPI void APIENTRY glGetBooleani_v (GLenum target, GLuint index, GLboolean *data);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetBooleani_v( JNIEnv* env, jclass klass,jint target_,jint index_,jbyteArray data_){
    if(do_trace)
        fprintf(stderr,"glGetBooleani_v\n");
    static PFNGLGETBOOLEANI_VPROC impl;
    if( !impl ){
       impl = (PFNGLGETBOOLEANI_VPROC) mygetprocaddr(env,"glGetBooleani_v");
       if(!impl)
            return;
    }
    jbyte* _data_ = (data_ ? ((*env)->GetByteArrayElements(env,data_,NULL) ) : NULL);
    impl(target_, index_, _data_);
    if(data_)
        (*env)->ReleaseByteArrayElements(env,data_, _data_,0);
}
//GLAPI void APIENTRY glGetIntegeri_v (GLenum target, GLuint index, GLint *data);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetIntegeri_v( JNIEnv* env, jclass klass,jint target_,jint index_,jintArray data_){
    if(do_trace)
        fprintf(stderr,"glGetIntegeri_v\n");
    static PFNGLGETINTEGERI_VPROC impl;
    if( !impl ){
       impl = (PFNGLGETINTEGERI_VPROC) mygetprocaddr(env,"glGetIntegeri_v");
       if(!impl)
            return;
    }
    jint* _data_ = (data_ ? ((*env)->GetIntArrayElements(env,data_,NULL) ) : NULL);
    impl(target_, index_, _data_);
    if(data_)
        (*env)->ReleaseIntArrayElements(env,data_, _data_,0);
}
//GLAPI void APIENTRY glEnablei (GLenum target, GLuint index);
JNIEXPORT void JNICALL Java_JGL_JGL_glEnablei( JNIEnv* env, jclass klass,jint target_,jint index_){
    if(do_trace)
        fprintf(stderr,"glEnablei\n");
    static PFNGLENABLEIPROC impl;
    if( !impl ){
       impl = (PFNGLENABLEIPROC) mygetprocaddr(env,"glEnablei");
       if(!impl)
            return;
    }
    impl(target_, index_);
}
//GLAPI void APIENTRY glDisablei (GLenum target, GLuint index);
JNIEXPORT void JNICALL Java_JGL_JGL_glDisablei( JNIEnv* env, jclass klass,jint target_,jint index_){
    if(do_trace)
        fprintf(stderr,"glDisablei\n");
    static PFNGLDISABLEIPROC impl;
    if( !impl ){
       impl = (PFNGLDISABLEIPROC) mygetprocaddr(env,"glDisablei");
       if(!impl)
            return;
    }
    impl(target_, index_);
}
//GLAPI GLboolean APIENTRY glIsEnabledi (GLenum target, GLuint index);
JNIEXPORT jboolean JNICALL Java_JGL_JGL_glIsEnabledi( JNIEnv* env, jclass klass,jint target_,jint index_){
    jboolean rv = 0;
    if(do_trace)
        fprintf(stderr,"glIsEnabledi\n");
    static PFNGLISENABLEDIPROC impl;
    if( !impl ){
       impl = (PFNGLISENABLEDIPROC) mygetprocaddr(env,"glIsEnabledi");
       if(!impl)
            return rv;
    }
    rv = impl(target_, index_);
    return rv;
}
//GLAPI void APIENTRY glBeginTransformFeedback (GLenum primitiveMode);
JNIEXPORT void JNICALL Java_JGL_JGL_glBeginTransformFeedback( JNIEnv* env, jclass klass,jint primitiveMode_){
    if(do_trace)
        fprintf(stderr,"glBeginTransformFeedback\n");
    static PFNGLBEGINTRANSFORMFEEDBACKPROC impl;
    if( !impl ){
       impl = (PFNGLBEGINTRANSFORMFEEDBACKPROC) mygetprocaddr(env,"glBeginTransformFeedback");
       if(!impl)
            return;
    }
    impl(primitiveMode_);
}
//GLAPI void APIENTRY glEndTransformFeedback (void);
JNIEXPORT void JNICALL Java_JGL_JGL_glEndTransformFeedback( JNIEnv* env, jclass klass){
    if(do_trace)
        fprintf(stderr,"glEndTransformFeedback\n");
    static PFNGLENDTRANSFORMFEEDBACKPROC impl;
    if( !impl ){
       impl = (PFNGLENDTRANSFORMFEEDBACKPROC) mygetprocaddr(env,"glEndTransformFeedback");
       if(!impl)
            return;
    }
    impl();
}
//GLAPI void APIENTRY glBindBufferRange (GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);
JNIEXPORT void JNICALL Java_JGL_JGL_glBindBufferRange( JNIEnv* env, jclass klass,jint target_,jint index_,jint buffer_,jint offset_,jint size_){
    if(do_trace)
        fprintf(stderr,"glBindBufferRange\n");
    static PFNGLBINDBUFFERRANGEPROC impl;
    if( !impl ){
       impl = (PFNGLBINDBUFFERRANGEPROC) mygetprocaddr(env,"glBindBufferRange");
       if(!impl)
            return;
    }
    impl(target_, index_, buffer_, offset_, size_);
}
//GLAPI void APIENTRY glBindBufferBase (GLenum target, GLuint index, GLuint buffer);
JNIEXPORT void JNICALL Java_JGL_JGL_glBindBufferBase( JNIEnv* env, jclass klass,jint target_,jint index_,jint buffer_){
    if(do_trace)
        fprintf(stderr,"glBindBufferBase\n");
    static PFNGLBINDBUFFERBASEPROC impl;
    if( !impl ){
       impl = (PFNGLBINDBUFFERBASEPROC) mygetprocaddr(env,"glBindBufferBase");
       if(!impl)
            return;
    }
    impl(target_, index_, buffer_);
}
//GLAPI void APIENTRY glGetTransformFeedbackVarying (GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetTransformFeedbackVarying( JNIEnv* env, jclass klass,jint program_,jint index_,jint bufSize_,jintArray length_,jintArray size_,jintArray type_,jbyteArray name_){
    if(do_trace)
        fprintf(stderr,"glGetTransformFeedbackVarying\n");
    static PFNGLGETTRANSFORMFEEDBACKVARYINGPROC impl;
    if( !impl ){
       impl = (PFNGLGETTRANSFORMFEEDBACKVARYINGPROC) mygetprocaddr(env,"glGetTransformFeedbackVarying");
       if(!impl)
            return;
    }
    jint* _length_ = (length_ ? ((*env)->GetIntArrayElements(env,length_,NULL) ) : NULL);
    jint* _size_ = (size_ ? ((*env)->GetIntArrayElements(env,size_,NULL) ) : NULL);
    jint* _type_ = (type_ ? ((*env)->GetIntArrayElements(env,type_,NULL) ) : NULL);
    jbyte* _name_ = (name_ ? ((*env)->GetByteArrayElements(env,name_,NULL) ) : NULL);
    impl(program_, index_, bufSize_, _length_, _size_, _type_, _name_);
    if(length_)
        (*env)->ReleaseIntArrayElements(env,length_, _length_,0);
    if(size_)
        (*env)->ReleaseIntArrayElements(env,size_, _size_,0);
    if(type_)
        (*env)->ReleaseIntArrayElements(env,type_, _type_,0);
    if(name_)
        (*env)->ReleaseByteArrayElements(env,name_, _name_,0);
}
//GLAPI void APIENTRY glClampColor (GLenum target, GLenum clamp);
JNIEXPORT void JNICALL Java_JGL_JGL_glClampColor( JNIEnv* env, jclass klass,jint target_,jint clamp_){
    if(do_trace)
        fprintf(stderr,"glClampColor\n");
    static PFNGLCLAMPCOLORPROC impl;
    if( !impl ){
       impl = (PFNGLCLAMPCOLORPROC) mygetprocaddr(env,"glClampColor");
       if(!impl)
            return;
    }
    impl(target_, clamp_);
}
//GLAPI void APIENTRY glBeginConditionalRender (GLuint id, GLenum mode);
JNIEXPORT void JNICALL Java_JGL_JGL_glBeginConditionalRender( JNIEnv* env, jclass klass,jint id_,jint mode_){
    if(do_trace)
        fprintf(stderr,"glBeginConditionalRender\n");
    static PFNGLBEGINCONDITIONALRENDERPROC impl;
    if( !impl ){
       impl = (PFNGLBEGINCONDITIONALRENDERPROC) mygetprocaddr(env,"glBeginConditionalRender");
       if(!impl)
            return;
    }
    impl(id_, mode_);
}
//GLAPI void APIENTRY glEndConditionalRender (void);
JNIEXPORT void JNICALL Java_JGL_JGL_glEndConditionalRender( JNIEnv* env, jclass klass){
    if(do_trace)
        fprintf(stderr,"glEndConditionalRender\n");
    static PFNGLENDCONDITIONALRENDERPROC impl;
    if( !impl ){
       impl = (PFNGLENDCONDITIONALRENDERPROC) mygetprocaddr(env,"glEndConditionalRender");
       if(!impl)
            return;
    }
    impl();
}
//GLAPI void APIENTRY glVertexAttribIPointer (GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribIPointer( JNIEnv* env, jclass klass,jint index_,jint size_,jint type_,jint stride_,jbyteArray pointer_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribIPointer\n");
    static PFNGLVERTEXATTRIBIPOINTERPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBIPOINTERPROC) mygetprocaddr(env,"glVertexAttribIPointer");
       if(!impl)
            return;
    }
    jbyte* _pointer_ = (pointer_ ? ((*env)->GetByteArrayElements(env,pointer_,NULL) ) : NULL);
    impl(index_, size_, type_, stride_, _pointer_);
    if(pointer_)
        (*env)->ReleaseByteArrayElements(env,pointer_, _pointer_,0);
}
//GLAPI void APIENTRY glGetVertexAttribIiv (GLuint index, GLenum pname, GLint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetVertexAttribIiv( JNIEnv* env, jclass klass,jint index_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetVertexAttribIiv\n");
    static PFNGLGETVERTEXATTRIBIIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETVERTEXATTRIBIIVPROC) mygetprocaddr(env,"glGetVertexAttribIiv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(index_, pname_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glGetVertexAttribIuiv (GLuint index, GLenum pname, GLuint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetVertexAttribIuiv( JNIEnv* env, jclass klass,jint index_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetVertexAttribIuiv\n");
    static PFNGLGETVERTEXATTRIBIUIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETVERTEXATTRIBIUIVPROC) mygetprocaddr(env,"glGetVertexAttribIuiv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(index_, pname_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glVertexAttribI1i (GLuint index, GLint x);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribI1i( JNIEnv* env, jclass klass,jint index_,jint x_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribI1i\n");
    static PFNGLVERTEXATTRIBI1IPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBI1IPROC) mygetprocaddr(env,"glVertexAttribI1i");
       if(!impl)
            return;
    }
    impl(index_, x_);
}
//GLAPI void APIENTRY glVertexAttribI2i (GLuint index, GLint x, GLint y);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribI2i( JNIEnv* env, jclass klass,jint index_,jint x_,jint y_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribI2i\n");
    static PFNGLVERTEXATTRIBI2IPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBI2IPROC) mygetprocaddr(env,"glVertexAttribI2i");
       if(!impl)
            return;
    }
    impl(index_, x_, y_);
}
//GLAPI void APIENTRY glVertexAttribI3i (GLuint index, GLint x, GLint y, GLint z);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribI3i( JNIEnv* env, jclass klass,jint index_,jint x_,jint y_,jint z_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribI3i\n");
    static PFNGLVERTEXATTRIBI3IPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBI3IPROC) mygetprocaddr(env,"glVertexAttribI3i");
       if(!impl)
            return;
    }
    impl(index_, x_, y_, z_);
}
//GLAPI void APIENTRY glVertexAttribI4i (GLuint index, GLint x, GLint y, GLint z, GLint w);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribI4i( JNIEnv* env, jclass klass,jint index_,jint x_,jint y_,jint z_,jint w_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribI4i\n");
    static PFNGLVERTEXATTRIBI4IPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBI4IPROC) mygetprocaddr(env,"glVertexAttribI4i");
       if(!impl)
            return;
    }
    impl(index_, x_, y_, z_, w_);
}
//GLAPI void APIENTRY glVertexAttribI1ui (GLuint index, GLuint x);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribI1ui( JNIEnv* env, jclass klass,jint index_,jint x_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribI1ui\n");
    static PFNGLVERTEXATTRIBI1UIPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBI1UIPROC) mygetprocaddr(env,"glVertexAttribI1ui");
       if(!impl)
            return;
    }
    impl(index_, x_);
}
//GLAPI void APIENTRY glVertexAttribI2ui (GLuint index, GLuint x, GLuint y);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribI2ui( JNIEnv* env, jclass klass,jint index_,jint x_,jint y_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribI2ui\n");
    static PFNGLVERTEXATTRIBI2UIPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBI2UIPROC) mygetprocaddr(env,"glVertexAttribI2ui");
       if(!impl)
            return;
    }
    impl(index_, x_, y_);
}
//GLAPI void APIENTRY glVertexAttribI3ui (GLuint index, GLuint x, GLuint y, GLuint z);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribI3ui( JNIEnv* env, jclass klass,jint index_,jint x_,jint y_,jint z_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribI3ui\n");
    static PFNGLVERTEXATTRIBI3UIPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBI3UIPROC) mygetprocaddr(env,"glVertexAttribI3ui");
       if(!impl)
            return;
    }
    impl(index_, x_, y_, z_);
}
//GLAPI void APIENTRY glVertexAttribI4ui (GLuint index, GLuint x, GLuint y, GLuint z, GLuint w);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribI4ui( JNIEnv* env, jclass klass,jint index_,jint x_,jint y_,jint z_,jint w_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribI4ui\n");
    static PFNGLVERTEXATTRIBI4UIPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBI4UIPROC) mygetprocaddr(env,"glVertexAttribI4ui");
       if(!impl)
            return;
    }
    impl(index_, x_, y_, z_, w_);
}
//GLAPI void APIENTRY glVertexAttribI1iv (GLuint index, const GLint *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribI1iv( JNIEnv* env, jclass klass,jint index_,jintArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribI1iv\n");
    static PFNGLVERTEXATTRIBI1IVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBI1IVPROC) mygetprocaddr(env,"glVertexAttribI1iv");
       if(!impl)
            return;
    }
    jint* _v_ = (v_ ? ((*env)->GetIntArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseIntArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttribI2iv (GLuint index, const GLint *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribI2iv( JNIEnv* env, jclass klass,jint index_,jintArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribI2iv\n");
    static PFNGLVERTEXATTRIBI2IVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBI2IVPROC) mygetprocaddr(env,"glVertexAttribI2iv");
       if(!impl)
            return;
    }
    jint* _v_ = (v_ ? ((*env)->GetIntArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseIntArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttribI3iv (GLuint index, const GLint *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribI3iv( JNIEnv* env, jclass klass,jint index_,jintArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribI3iv\n");
    static PFNGLVERTEXATTRIBI3IVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBI3IVPROC) mygetprocaddr(env,"glVertexAttribI3iv");
       if(!impl)
            return;
    }
    jint* _v_ = (v_ ? ((*env)->GetIntArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseIntArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttribI4iv (GLuint index, const GLint *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribI4iv( JNIEnv* env, jclass klass,jint index_,jintArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribI4iv\n");
    static PFNGLVERTEXATTRIBI4IVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBI4IVPROC) mygetprocaddr(env,"glVertexAttribI4iv");
       if(!impl)
            return;
    }
    jint* _v_ = (v_ ? ((*env)->GetIntArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseIntArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttribI1uiv (GLuint index, const GLuint *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribI1uiv( JNIEnv* env, jclass klass,jint index_,jintArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribI1uiv\n");
    static PFNGLVERTEXATTRIBI1UIVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBI1UIVPROC) mygetprocaddr(env,"glVertexAttribI1uiv");
       if(!impl)
            return;
    }
    jint* _v_ = (v_ ? ((*env)->GetIntArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseIntArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttribI2uiv (GLuint index, const GLuint *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribI2uiv( JNIEnv* env, jclass klass,jint index_,jintArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribI2uiv\n");
    static PFNGLVERTEXATTRIBI2UIVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBI2UIVPROC) mygetprocaddr(env,"glVertexAttribI2uiv");
       if(!impl)
            return;
    }
    jint* _v_ = (v_ ? ((*env)->GetIntArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseIntArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttribI3uiv (GLuint index, const GLuint *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribI3uiv( JNIEnv* env, jclass klass,jint index_,jintArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribI3uiv\n");
    static PFNGLVERTEXATTRIBI3UIVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBI3UIVPROC) mygetprocaddr(env,"glVertexAttribI3uiv");
       if(!impl)
            return;
    }
    jint* _v_ = (v_ ? ((*env)->GetIntArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseIntArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttribI4uiv (GLuint index, const GLuint *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribI4uiv( JNIEnv* env, jclass klass,jint index_,jintArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribI4uiv\n");
    static PFNGLVERTEXATTRIBI4UIVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBI4UIVPROC) mygetprocaddr(env,"glVertexAttribI4uiv");
       if(!impl)
            return;
    }
    jint* _v_ = (v_ ? ((*env)->GetIntArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseIntArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttribI4bv (GLuint index, const GLbyte *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribI4bv( JNIEnv* env, jclass klass,jint index_,jbyteArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribI4bv\n");
    static PFNGLVERTEXATTRIBI4BVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBI4BVPROC) mygetprocaddr(env,"glVertexAttribI4bv");
       if(!impl)
            return;
    }
    jbyte* _v_ = (v_ ? ((*env)->GetByteArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseByteArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttribI4sv (GLuint index, const GLshort *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribI4sv( JNIEnv* env, jclass klass,jint index_,jshortArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribI4sv\n");
    static PFNGLVERTEXATTRIBI4SVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBI4SVPROC) mygetprocaddr(env,"glVertexAttribI4sv");
       if(!impl)
            return;
    }
    jshort* _v_ = (v_ ? ((*env)->GetShortArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseShortArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttribI4ubv (GLuint index, const GLubyte *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribI4ubv( JNIEnv* env, jclass klass,jint index_,jbyteArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribI4ubv\n");
    static PFNGLVERTEXATTRIBI4UBVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBI4UBVPROC) mygetprocaddr(env,"glVertexAttribI4ubv");
       if(!impl)
            return;
    }
    jbyte* _v_ = (v_ ? ((*env)->GetByteArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseByteArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttribI4usv (GLuint index, const GLushort *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribI4usv( JNIEnv* env, jclass klass,jint index_,jshortArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribI4usv\n");
    static PFNGLVERTEXATTRIBI4USVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBI4USVPROC) mygetprocaddr(env,"glVertexAttribI4usv");
       if(!impl)
            return;
    }
    jshort* _v_ = (v_ ? ((*env)->GetShortArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseShortArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glGetUniformuiv (GLuint program, GLint location, GLuint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetUniformuiv( JNIEnv* env, jclass klass,jint program_,jint location_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetUniformuiv\n");
    static PFNGLGETUNIFORMUIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETUNIFORMUIVPROC) mygetprocaddr(env,"glGetUniformuiv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(program_, location_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glBindFragDataLocation (GLuint program, GLuint color, const GLchar *name);
JNIEXPORT void JNICALL Java_JGL_JGL_glBindFragDataLocation( JNIEnv* env, jclass klass,jint program_,jint color_,jbyteArray name_){
    if(do_trace)
        fprintf(stderr,"glBindFragDataLocation\n");
    static PFNGLBINDFRAGDATALOCATIONPROC impl;
    if( !impl ){
       impl = (PFNGLBINDFRAGDATALOCATIONPROC) mygetprocaddr(env,"glBindFragDataLocation");
       if(!impl)
            return;
    }
    jbyte* _name_ = (name_ ? ((*env)->GetByteArrayElements(env,name_,NULL) ) : NULL);
    impl(program_, color_, _name_);
    if(name_)
        (*env)->ReleaseByteArrayElements(env,name_, _name_,0);
}
//GLAPI GLint APIENTRY glGetFragDataLocation (GLuint program, const GLchar *name);
JNIEXPORT jint JNICALL Java_JGL_JGL_glGetFragDataLocation( JNIEnv* env, jclass klass,jint program_,jbyteArray name_){
    jint rv = 0;
    if(do_trace)
        fprintf(stderr,"glGetFragDataLocation\n");
    static PFNGLGETFRAGDATALOCATIONPROC impl;
    if( !impl ){
       impl = (PFNGLGETFRAGDATALOCATIONPROC) mygetprocaddr(env,"glGetFragDataLocation");
       if(!impl)
            return rv;
    }
    jbyte* _name_ = (name_ ? ((*env)->GetByteArrayElements(env,name_,NULL) ) : NULL);
    rv = impl(program_, _name_);
    if(name_)
        (*env)->ReleaseByteArrayElements(env,name_, _name_,0);
    return rv;
}
//GLAPI void APIENTRY glUniform1ui (GLint location, GLuint v0);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniform1ui( JNIEnv* env, jclass klass,jint location_,jint v0_){
    if(do_trace)
        fprintf(stderr,"glUniform1ui\n");
    static PFNGLUNIFORM1UIPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORM1UIPROC) mygetprocaddr(env,"glUniform1ui");
       if(!impl)
            return;
    }
    impl(location_, v0_);
}
//GLAPI void APIENTRY glUniform2ui (GLint location, GLuint v0, GLuint v1);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniform2ui( JNIEnv* env, jclass klass,jint location_,jint v0_,jint v1_){
    if(do_trace)
        fprintf(stderr,"glUniform2ui\n");
    static PFNGLUNIFORM2UIPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORM2UIPROC) mygetprocaddr(env,"glUniform2ui");
       if(!impl)
            return;
    }
    impl(location_, v0_, v1_);
}
//GLAPI void APIENTRY glUniform3ui (GLint location, GLuint v0, GLuint v1, GLuint v2);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniform3ui( JNIEnv* env, jclass klass,jint location_,jint v0_,jint v1_,jint v2_){
    if(do_trace)
        fprintf(stderr,"glUniform3ui\n");
    static PFNGLUNIFORM3UIPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORM3UIPROC) mygetprocaddr(env,"glUniform3ui");
       if(!impl)
            return;
    }
    impl(location_, v0_, v1_, v2_);
}
//GLAPI void APIENTRY glUniform4ui (GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniform4ui( JNIEnv* env, jclass klass,jint location_,jint v0_,jint v1_,jint v2_,jint v3_){
    if(do_trace)
        fprintf(stderr,"glUniform4ui\n");
    static PFNGLUNIFORM4UIPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORM4UIPROC) mygetprocaddr(env,"glUniform4ui");
       if(!impl)
            return;
    }
    impl(location_, v0_, v1_, v2_, v3_);
}
//GLAPI void APIENTRY glUniform1uiv (GLint location, GLsizei count, const GLuint *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniform1uiv( JNIEnv* env, jclass klass,jint location_,jint count_,jintArray value_){
    if(do_trace)
        fprintf(stderr,"glUniform1uiv\n");
    static PFNGLUNIFORM1UIVPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORM1UIVPROC) mygetprocaddr(env,"glUniform1uiv");
       if(!impl)
            return;
    }
    jint* _value_ = (value_ ? ((*env)->GetIntArrayElements(env,value_,NULL) ) : NULL);
    impl(location_, count_, _value_);
    if(value_)
        (*env)->ReleaseIntArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glUniform2uiv (GLint location, GLsizei count, const GLuint *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniform2uiv( JNIEnv* env, jclass klass,jint location_,jint count_,jintArray value_){
    if(do_trace)
        fprintf(stderr,"glUniform2uiv\n");
    static PFNGLUNIFORM2UIVPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORM2UIVPROC) mygetprocaddr(env,"glUniform2uiv");
       if(!impl)
            return;
    }
    jint* _value_ = (value_ ? ((*env)->GetIntArrayElements(env,value_,NULL) ) : NULL);
    impl(location_, count_, _value_);
    if(value_)
        (*env)->ReleaseIntArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glUniform3uiv (GLint location, GLsizei count, const GLuint *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniform3uiv( JNIEnv* env, jclass klass,jint location_,jint count_,jintArray value_){
    if(do_trace)
        fprintf(stderr,"glUniform3uiv\n");
    static PFNGLUNIFORM3UIVPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORM3UIVPROC) mygetprocaddr(env,"glUniform3uiv");
       if(!impl)
            return;
    }
    jint* _value_ = (value_ ? ((*env)->GetIntArrayElements(env,value_,NULL) ) : NULL);
    impl(location_, count_, _value_);
    if(value_)
        (*env)->ReleaseIntArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glUniform4uiv (GLint location, GLsizei count, const GLuint *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniform4uiv( JNIEnv* env, jclass klass,jint location_,jint count_,jintArray value_){
    if(do_trace)
        fprintf(stderr,"glUniform4uiv\n");
    static PFNGLUNIFORM4UIVPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORM4UIVPROC) mygetprocaddr(env,"glUniform4uiv");
       if(!impl)
            return;
    }
    jint* _value_ = (value_ ? ((*env)->GetIntArrayElements(env,value_,NULL) ) : NULL);
    impl(location_, count_, _value_);
    if(value_)
        (*env)->ReleaseIntArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glTexParameterIiv (GLenum target, GLenum pname, const GLint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glTexParameterIiv( JNIEnv* env, jclass klass,jint target_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glTexParameterIiv\n");
    static PFNGLTEXPARAMETERIIVPROC impl;
    if( !impl ){
       impl = (PFNGLTEXPARAMETERIIVPROC) mygetprocaddr(env,"glTexParameterIiv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(target_, pname_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glTexParameterIuiv (GLenum target, GLenum pname, const GLuint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glTexParameterIuiv( JNIEnv* env, jclass klass,jint target_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glTexParameterIuiv\n");
    static PFNGLTEXPARAMETERIUIVPROC impl;
    if( !impl ){
       impl = (PFNGLTEXPARAMETERIUIVPROC) mygetprocaddr(env,"glTexParameterIuiv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(target_, pname_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glGetTexParameterIiv (GLenum target, GLenum pname, GLint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetTexParameterIiv( JNIEnv* env, jclass klass,jint target_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetTexParameterIiv\n");
    static PFNGLGETTEXPARAMETERIIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETTEXPARAMETERIIVPROC) mygetprocaddr(env,"glGetTexParameterIiv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(target_, pname_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glGetTexParameterIuiv (GLenum target, GLenum pname, GLuint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetTexParameterIuiv( JNIEnv* env, jclass klass,jint target_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetTexParameterIuiv\n");
    static PFNGLGETTEXPARAMETERIUIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETTEXPARAMETERIUIVPROC) mygetprocaddr(env,"glGetTexParameterIuiv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(target_, pname_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glClearBufferiv (GLenum buffer, GLint drawbuffer, const GLint *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glClearBufferiv( JNIEnv* env, jclass klass,jint buffer_,jint drawbuffer_,jintArray value_){
    if(do_trace)
        fprintf(stderr,"glClearBufferiv\n");
    static PFNGLCLEARBUFFERIVPROC impl;
    if( !impl ){
       impl = (PFNGLCLEARBUFFERIVPROC) mygetprocaddr(env,"glClearBufferiv");
       if(!impl)
            return;
    }
    jint* _value_ = (value_ ? ((*env)->GetIntArrayElements(env,value_,NULL) ) : NULL);
    impl(buffer_, drawbuffer_, _value_);
    if(value_)
        (*env)->ReleaseIntArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glClearBufferuiv (GLenum buffer, GLint drawbuffer, const GLuint *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glClearBufferuiv( JNIEnv* env, jclass klass,jint buffer_,jint drawbuffer_,jintArray value_){
    if(do_trace)
        fprintf(stderr,"glClearBufferuiv\n");
    static PFNGLCLEARBUFFERUIVPROC impl;
    if( !impl ){
       impl = (PFNGLCLEARBUFFERUIVPROC) mygetprocaddr(env,"glClearBufferuiv");
       if(!impl)
            return;
    }
    jint* _value_ = (value_ ? ((*env)->GetIntArrayElements(env,value_,NULL) ) : NULL);
    impl(buffer_, drawbuffer_, _value_);
    if(value_)
        (*env)->ReleaseIntArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glClearBufferfv (GLenum buffer, GLint drawbuffer, const GLfloat *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glClearBufferfv( JNIEnv* env, jclass klass,jint buffer_,jint drawbuffer_,jfloatArray value_){
    if(do_trace)
        fprintf(stderr,"glClearBufferfv\n");
    static PFNGLCLEARBUFFERFVPROC impl;
    if( !impl ){
       impl = (PFNGLCLEARBUFFERFVPROC) mygetprocaddr(env,"glClearBufferfv");
       if(!impl)
            return;
    }
    jfloat* _value_ = (value_ ? ((*env)->GetFloatArrayElements(env,value_,NULL) ) : NULL);
    impl(buffer_, drawbuffer_, _value_);
    if(value_)
        (*env)->ReleaseFloatArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glClearBufferfi (GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil);
JNIEXPORT void JNICALL Java_JGL_JGL_glClearBufferfi( JNIEnv* env, jclass klass,jint buffer_,jint drawbuffer_,jfloat depth_,jint stencil_){
    if(do_trace)
        fprintf(stderr,"glClearBufferfi\n");
    static PFNGLCLEARBUFFERFIPROC impl;
    if( !impl ){
       impl = (PFNGLCLEARBUFFERFIPROC) mygetprocaddr(env,"glClearBufferfi");
       if(!impl)
            return;
    }
    impl(buffer_, drawbuffer_, depth_, stencil_);
}
//GLAPI GLboolean APIENTRY glIsRenderbuffer (GLuint renderbuffer);
JNIEXPORT jboolean JNICALL Java_JGL_JGL_glIsRenderbuffer( JNIEnv* env, jclass klass,jint renderbuffer_){
    jboolean rv = 0;
    if(do_trace)
        fprintf(stderr,"glIsRenderbuffer\n");
    static PFNGLISRENDERBUFFERPROC impl;
    if( !impl ){
       impl = (PFNGLISRENDERBUFFERPROC) mygetprocaddr(env,"glIsRenderbuffer");
       if(!impl)
            return rv;
    }
    rv = impl(renderbuffer_);
    return rv;
}
//GLAPI void APIENTRY glBindRenderbuffer (GLenum target, GLuint renderbuffer);
JNIEXPORT void JNICALL Java_JGL_JGL_glBindRenderbuffer( JNIEnv* env, jclass klass,jint target_,jint renderbuffer_){
    if(do_trace)
        fprintf(stderr,"glBindRenderbuffer\n");
    static PFNGLBINDRENDERBUFFERPROC impl;
    if( !impl ){
       impl = (PFNGLBINDRENDERBUFFERPROC) mygetprocaddr(env,"glBindRenderbuffer");
       if(!impl)
            return;
    }
    impl(target_, renderbuffer_);
}
//GLAPI void APIENTRY glDeleteRenderbuffers (GLsizei n, const GLuint *renderbuffers);
JNIEXPORT void JNICALL Java_JGL_JGL_glDeleteRenderbuffers( JNIEnv* env, jclass klass,jint n_,jintArray renderbuffers_){
    if(do_trace)
        fprintf(stderr,"glDeleteRenderbuffers\n");
    static PFNGLDELETERENDERBUFFERSPROC impl;
    if( !impl ){
       impl = (PFNGLDELETERENDERBUFFERSPROC) mygetprocaddr(env,"glDeleteRenderbuffers");
       if(!impl)
            return;
    }
    jint* _renderbuffers_ = (renderbuffers_ ? ((*env)->GetIntArrayElements(env,renderbuffers_,NULL) ) : NULL);
    impl(n_, _renderbuffers_);
    if(renderbuffers_)
        (*env)->ReleaseIntArrayElements(env,renderbuffers_, _renderbuffers_,0);
}
//GLAPI void APIENTRY glGenRenderbuffers (GLsizei n, GLuint *renderbuffers);
JNIEXPORT void JNICALL Java_JGL_JGL_glGenRenderbuffers( JNIEnv* env, jclass klass,jint n_,jintArray renderbuffers_){
    if(do_trace)
        fprintf(stderr,"glGenRenderbuffers\n");
    static PFNGLGENRENDERBUFFERSPROC impl;
    if( !impl ){
       impl = (PFNGLGENRENDERBUFFERSPROC) mygetprocaddr(env,"glGenRenderbuffers");
       if(!impl)
            return;
    }
    jint* _renderbuffers_ = (renderbuffers_ ? ((*env)->GetIntArrayElements(env,renderbuffers_,NULL) ) : NULL);
    impl(n_, _renderbuffers_);
    if(renderbuffers_)
        (*env)->ReleaseIntArrayElements(env,renderbuffers_, _renderbuffers_,0);
}
//GLAPI void APIENTRY glRenderbufferStorage (GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
JNIEXPORT void JNICALL Java_JGL_JGL_glRenderbufferStorage( JNIEnv* env, jclass klass,jint target_,jint internalformat_,jint width_,jint height_){
    if(do_trace)
        fprintf(stderr,"glRenderbufferStorage\n");
    static PFNGLRENDERBUFFERSTORAGEPROC impl;
    if( !impl ){
       impl = (PFNGLRENDERBUFFERSTORAGEPROC) mygetprocaddr(env,"glRenderbufferStorage");
       if(!impl)
            return;
    }
    impl(target_, internalformat_, width_, height_);
}
//GLAPI void APIENTRY glGetRenderbufferParameteriv (GLenum target, GLenum pname, GLint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetRenderbufferParameteriv( JNIEnv* env, jclass klass,jint target_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetRenderbufferParameteriv\n");
    static PFNGLGETRENDERBUFFERPARAMETERIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETRENDERBUFFERPARAMETERIVPROC) mygetprocaddr(env,"glGetRenderbufferParameteriv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(target_, pname_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI GLboolean APIENTRY glIsFramebuffer (GLuint framebuffer);
JNIEXPORT jboolean JNICALL Java_JGL_JGL_glIsFramebuffer( JNIEnv* env, jclass klass,jint framebuffer_){
    jboolean rv = 0;
    if(do_trace)
        fprintf(stderr,"glIsFramebuffer\n");
    static PFNGLISFRAMEBUFFERPROC impl;
    if( !impl ){
       impl = (PFNGLISFRAMEBUFFERPROC) mygetprocaddr(env,"glIsFramebuffer");
       if(!impl)
            return rv;
    }
    rv = impl(framebuffer_);
    return rv;
}
//GLAPI void APIENTRY glBindFramebuffer (GLenum target, GLuint framebuffer);
JNIEXPORT void JNICALL Java_JGL_JGL_glBindFramebuffer( JNIEnv* env, jclass klass,jint target_,jint framebuffer_){
    if(do_trace)
        fprintf(stderr,"glBindFramebuffer\n");
    static PFNGLBINDFRAMEBUFFERPROC impl;
    if( !impl ){
       impl = (PFNGLBINDFRAMEBUFFERPROC) mygetprocaddr(env,"glBindFramebuffer");
       if(!impl)
            return;
    }
    impl(target_, framebuffer_);
}
//GLAPI void APIENTRY glDeleteFramebuffers (GLsizei n, const GLuint *framebuffers);
JNIEXPORT void JNICALL Java_JGL_JGL_glDeleteFramebuffers( JNIEnv* env, jclass klass,jint n_,jintArray framebuffers_){
    if(do_trace)
        fprintf(stderr,"glDeleteFramebuffers\n");
    static PFNGLDELETEFRAMEBUFFERSPROC impl;
    if( !impl ){
       impl = (PFNGLDELETEFRAMEBUFFERSPROC) mygetprocaddr(env,"glDeleteFramebuffers");
       if(!impl)
            return;
    }
    jint* _framebuffers_ = (framebuffers_ ? ((*env)->GetIntArrayElements(env,framebuffers_,NULL) ) : NULL);
    impl(n_, _framebuffers_);
    if(framebuffers_)
        (*env)->ReleaseIntArrayElements(env,framebuffers_, _framebuffers_,0);
}
//GLAPI void APIENTRY glGenFramebuffers (GLsizei n, GLuint *framebuffers);
JNIEXPORT void JNICALL Java_JGL_JGL_glGenFramebuffers( JNIEnv* env, jclass klass,jint n_,jintArray framebuffers_){
    if(do_trace)
        fprintf(stderr,"glGenFramebuffers\n");
    static PFNGLGENFRAMEBUFFERSPROC impl;
    if( !impl ){
       impl = (PFNGLGENFRAMEBUFFERSPROC) mygetprocaddr(env,"glGenFramebuffers");
       if(!impl)
            return;
    }
    jint* _framebuffers_ = (framebuffers_ ? ((*env)->GetIntArrayElements(env,framebuffers_,NULL) ) : NULL);
    impl(n_, _framebuffers_);
    if(framebuffers_)
        (*env)->ReleaseIntArrayElements(env,framebuffers_, _framebuffers_,0);
}
//GLAPI GLenum APIENTRY glCheckFramebufferStatus (GLenum target);
JNIEXPORT jint JNICALL Java_JGL_JGL_glCheckFramebufferStatus( JNIEnv* env, jclass klass,jint target_){
    jint rv = 0;
    if(do_trace)
        fprintf(stderr,"glCheckFramebufferStatus\n");
    static PFNGLCHECKFRAMEBUFFERSTATUSPROC impl;
    if( !impl ){
       impl = (PFNGLCHECKFRAMEBUFFERSTATUSPROC) mygetprocaddr(env,"glCheckFramebufferStatus");
       if(!impl)
            return rv;
    }
    rv = impl(target_);
    return rv;
}
//GLAPI void APIENTRY glFramebufferTexture1D (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
JNIEXPORT void JNICALL Java_JGL_JGL_glFramebufferTexture1D( JNIEnv* env, jclass klass,jint target_,jint attachment_,jint textarget_,jint texture_,jint level_){
    if(do_trace)
        fprintf(stderr,"glFramebufferTexture1D\n");
    static PFNGLFRAMEBUFFERTEXTURE1DPROC impl;
    if( !impl ){
       impl = (PFNGLFRAMEBUFFERTEXTURE1DPROC) mygetprocaddr(env,"glFramebufferTexture1D");
       if(!impl)
            return;
    }
    impl(target_, attachment_, textarget_, texture_, level_);
}
//GLAPI void APIENTRY glFramebufferTexture2D (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
JNIEXPORT void JNICALL Java_JGL_JGL_glFramebufferTexture2D( JNIEnv* env, jclass klass,jint target_,jint attachment_,jint textarget_,jint texture_,jint level_){
    if(do_trace)
        fprintf(stderr,"glFramebufferTexture2D\n");
    static PFNGLFRAMEBUFFERTEXTURE2DPROC impl;
    if( !impl ){
       impl = (PFNGLFRAMEBUFFERTEXTURE2DPROC) mygetprocaddr(env,"glFramebufferTexture2D");
       if(!impl)
            return;
    }
    impl(target_, attachment_, textarget_, texture_, level_);
}
//GLAPI void APIENTRY glFramebufferTexture3D (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset);
JNIEXPORT void JNICALL Java_JGL_JGL_glFramebufferTexture3D( JNIEnv* env, jclass klass,jint target_,jint attachment_,jint textarget_,jint texture_,jint level_,jint zoffset_){
    if(do_trace)
        fprintf(stderr,"glFramebufferTexture3D\n");
    static PFNGLFRAMEBUFFERTEXTURE3DPROC impl;
    if( !impl ){
       impl = (PFNGLFRAMEBUFFERTEXTURE3DPROC) mygetprocaddr(env,"glFramebufferTexture3D");
       if(!impl)
            return;
    }
    impl(target_, attachment_, textarget_, texture_, level_, zoffset_);
}
//GLAPI void APIENTRY glFramebufferRenderbuffer (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
JNIEXPORT void JNICALL Java_JGL_JGL_glFramebufferRenderbuffer( JNIEnv* env, jclass klass,jint target_,jint attachment_,jint renderbuffertarget_,jint renderbuffer_){
    if(do_trace)
        fprintf(stderr,"glFramebufferRenderbuffer\n");
    static PFNGLFRAMEBUFFERRENDERBUFFERPROC impl;
    if( !impl ){
       impl = (PFNGLFRAMEBUFFERRENDERBUFFERPROC) mygetprocaddr(env,"glFramebufferRenderbuffer");
       if(!impl)
            return;
    }
    impl(target_, attachment_, renderbuffertarget_, renderbuffer_);
}
//GLAPI void APIENTRY glGetFramebufferAttachmentParameteriv (GLenum target, GLenum attachment, GLenum pname, GLint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetFramebufferAttachmentParameteriv( JNIEnv* env, jclass klass,jint target_,jint attachment_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetFramebufferAttachmentParameteriv\n");
    static PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC) mygetprocaddr(env,"glGetFramebufferAttachmentParameteriv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(target_, attachment_, pname_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glGenerateMipmap (GLenum target);
JNIEXPORT void JNICALL Java_JGL_JGL_glGenerateMipmap( JNIEnv* env, jclass klass,jint target_){
    if(do_trace)
        fprintf(stderr,"glGenerateMipmap\n");
    static PFNGLGENERATEMIPMAPPROC impl;
    if( !impl ){
       impl = (PFNGLGENERATEMIPMAPPROC) mygetprocaddr(env,"glGenerateMipmap");
       if(!impl)
            return;
    }
    impl(target_);
}
//GLAPI void APIENTRY glBlitFramebuffer (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
JNIEXPORT void JNICALL Java_JGL_JGL_glBlitFramebuffer( JNIEnv* env, jclass klass,jint srcX0_,jint srcY0_,jint srcX1_,jint srcY1_,jint dstX0_,jint dstY0_,jint dstX1_,jint dstY1_,jint mask_,jint filter_){
    if(do_trace)
        fprintf(stderr,"glBlitFramebuffer\n");
    static PFNGLBLITFRAMEBUFFERPROC impl;
    if( !impl ){
       impl = (PFNGLBLITFRAMEBUFFERPROC) mygetprocaddr(env,"glBlitFramebuffer");
       if(!impl)
            return;
    }
    impl(srcX0_, srcY0_, srcX1_, srcY1_, dstX0_, dstY0_, dstX1_, dstY1_, mask_, filter_);
}
//GLAPI void APIENTRY glRenderbufferStorageMultisample (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
JNIEXPORT void JNICALL Java_JGL_JGL_glRenderbufferStorageMultisample( JNIEnv* env, jclass klass,jint target_,jint samples_,jint internalformat_,jint width_,jint height_){
    if(do_trace)
        fprintf(stderr,"glRenderbufferStorageMultisample\n");
    static PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC impl;
    if( !impl ){
       impl = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC) mygetprocaddr(env,"glRenderbufferStorageMultisample");
       if(!impl)
            return;
    }
    impl(target_, samples_, internalformat_, width_, height_);
}
//GLAPI void APIENTRY glFramebufferTextureLayer (GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);
JNIEXPORT void JNICALL Java_JGL_JGL_glFramebufferTextureLayer( JNIEnv* env, jclass klass,jint target_,jint attachment_,jint texture_,jint level_,jint layer_){
    if(do_trace)
        fprintf(stderr,"glFramebufferTextureLayer\n");
    static PFNGLFRAMEBUFFERTEXTURELAYERPROC impl;
    if( !impl ){
       impl = (PFNGLFRAMEBUFFERTEXTURELAYERPROC) mygetprocaddr(env,"glFramebufferTextureLayer");
       if(!impl)
            return;
    }
    impl(target_, attachment_, texture_, level_, layer_);
}
//GLAPI void APIENTRY glFlushMappedBufferRange (GLenum target, GLintptr offset, GLsizeiptr length);
JNIEXPORT void JNICALL Java_JGL_JGL_glFlushMappedBufferRange( JNIEnv* env, jclass klass,jint target_,jint offset_,jint length_){
    if(do_trace)
        fprintf(stderr,"glFlushMappedBufferRange\n");
    static PFNGLFLUSHMAPPEDBUFFERRANGEPROC impl;
    if( !impl ){
       impl = (PFNGLFLUSHMAPPEDBUFFERRANGEPROC) mygetprocaddr(env,"glFlushMappedBufferRange");
       if(!impl)
            return;
    }
    impl(target_, offset_, length_);
}
//GLAPI void APIENTRY glBindVertexArray (GLuint array);
JNIEXPORT void JNICALL Java_JGL_JGL_glBindVertexArray( JNIEnv* env, jclass klass,jint array_){
    if(do_trace)
        fprintf(stderr,"glBindVertexArray\n");
    static PFNGLBINDVERTEXARRAYPROC impl;
    if( !impl ){
       impl = (PFNGLBINDVERTEXARRAYPROC) mygetprocaddr(env,"glBindVertexArray");
       if(!impl)
            return;
    }
    impl(array_);
}
//GLAPI void APIENTRY glDeleteVertexArrays (GLsizei n, const GLuint *arrays);
JNIEXPORT void JNICALL Java_JGL_JGL_glDeleteVertexArrays( JNIEnv* env, jclass klass,jint n_,jintArray arrays_){
    if(do_trace)
        fprintf(stderr,"glDeleteVertexArrays\n");
    static PFNGLDELETEVERTEXARRAYSPROC impl;
    if( !impl ){
       impl = (PFNGLDELETEVERTEXARRAYSPROC) mygetprocaddr(env,"glDeleteVertexArrays");
       if(!impl)
            return;
    }
    jint* _arrays_ = (arrays_ ? ((*env)->GetIntArrayElements(env,arrays_,NULL) ) : NULL);
    impl(n_, _arrays_);
    if(arrays_)
        (*env)->ReleaseIntArrayElements(env,arrays_, _arrays_,0);
}
//GLAPI void APIENTRY glGenVertexArrays (GLsizei n, GLuint *arrays);
JNIEXPORT void JNICALL Java_JGL_JGL_glGenVertexArrays( JNIEnv* env, jclass klass,jint n_,jintArray arrays_){
    if(do_trace)
        fprintf(stderr,"glGenVertexArrays\n");
    static PFNGLGENVERTEXARRAYSPROC impl;
    if( !impl ){
       impl = (PFNGLGENVERTEXARRAYSPROC) mygetprocaddr(env,"glGenVertexArrays");
       if(!impl)
            return;
    }
    jint* _arrays_ = (arrays_ ? ((*env)->GetIntArrayElements(env,arrays_,NULL) ) : NULL);
    impl(n_, _arrays_);
    if(arrays_)
        (*env)->ReleaseIntArrayElements(env,arrays_, _arrays_,0);
}
//GLAPI GLboolean APIENTRY glIsVertexArray (GLuint array);
JNIEXPORT jboolean JNICALL Java_JGL_JGL_glIsVertexArray( JNIEnv* env, jclass klass,jint array_){
    jboolean rv = 0;
    if(do_trace)
        fprintf(stderr,"glIsVertexArray\n");
    static PFNGLISVERTEXARRAYPROC impl;
    if( !impl ){
       impl = (PFNGLISVERTEXARRAYPROC) mygetprocaddr(env,"glIsVertexArray");
       if(!impl)
            return rv;
    }
    rv = impl(array_);
    return rv;
}
//GLAPI void APIENTRY glDrawArraysInstanced (GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
JNIEXPORT void JNICALL Java_JGL_JGL_glDrawArraysInstanced( JNIEnv* env, jclass klass,jint mode_,jint first_,jint count_,jint instancecount_){
    if(do_trace)
        fprintf(stderr,"glDrawArraysInstanced\n");
    static PFNGLDRAWARRAYSINSTANCEDPROC impl;
    if( !impl ){
       impl = (PFNGLDRAWARRAYSINSTANCEDPROC) mygetprocaddr(env,"glDrawArraysInstanced");
       if(!impl)
            return;
    }
    impl(mode_, first_, count_, instancecount_);
}
//GLAPI void APIENTRY glDrawElementsInstanced (GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount);
JNIEXPORT void JNICALL Java_JGL_JGL_glDrawElementsInstanced( JNIEnv* env, jclass klass,jint mode_,jint count_,jint type_,jint indices_,jint instancecount_){
    if(do_trace)
        fprintf(stderr,"glDrawElementsInstanced\n");
    static PFNGLDRAWELEMENTSINSTANCEDPROC impl;
    if( !impl ){
       impl = (PFNGLDRAWELEMENTSINSTANCEDPROC) mygetprocaddr(env,"glDrawElementsInstanced");
       if(!impl)
            return;
    }
    impl(mode_, count_, type_, (const void *)indices_, instancecount_);
}
//GLAPI void APIENTRY glTexBuffer (GLenum target, GLenum internalformat, GLuint buffer);
JNIEXPORT void JNICALL Java_JGL_JGL_glTexBuffer( JNIEnv* env, jclass klass,jint target_,jint internalformat_,jint buffer_){
    if(do_trace)
        fprintf(stderr,"glTexBuffer\n");
    static PFNGLTEXBUFFERPROC impl;
    if( !impl ){
       impl = (PFNGLTEXBUFFERPROC) mygetprocaddr(env,"glTexBuffer");
       if(!impl)
            return;
    }
    impl(target_, internalformat_, buffer_);
}
//GLAPI void APIENTRY glPrimitiveRestartIndex (GLuint index);
JNIEXPORT void JNICALL Java_JGL_JGL_glPrimitiveRestartIndex( JNIEnv* env, jclass klass,jint index_){
    if(do_trace)
        fprintf(stderr,"glPrimitiveRestartIndex\n");
    static PFNGLPRIMITIVERESTARTINDEXPROC impl;
    if( !impl ){
       impl = (PFNGLPRIMITIVERESTARTINDEXPROC) mygetprocaddr(env,"glPrimitiveRestartIndex");
       if(!impl)
            return;
    }
    impl(index_);
}
//GLAPI void APIENTRY glCopyBufferSubData (GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
JNIEXPORT void JNICALL Java_JGL_JGL_glCopyBufferSubData( JNIEnv* env, jclass klass,jint readTarget_,jint writeTarget_,jint readOffset_,jint writeOffset_,jint size_){
    if(do_trace)
        fprintf(stderr,"glCopyBufferSubData\n");
    static PFNGLCOPYBUFFERSUBDATAPROC impl;
    if( !impl ){
       impl = (PFNGLCOPYBUFFERSUBDATAPROC) mygetprocaddr(env,"glCopyBufferSubData");
       if(!impl)
            return;
    }
    impl(readTarget_, writeTarget_, readOffset_, writeOffset_, size_);
}
//GLAPI void APIENTRY glGetActiveUniformsiv (GLuint program, GLsizei uniformCount, const GLuint *uniformIndices, GLenum pname, GLint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetActiveUniformsiv( JNIEnv* env, jclass klass,jint program_,jint uniformCount_,jintArray uniformIndices_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetActiveUniformsiv\n");
    static PFNGLGETACTIVEUNIFORMSIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETACTIVEUNIFORMSIVPROC) mygetprocaddr(env,"glGetActiveUniformsiv");
       if(!impl)
            return;
    }
    jint* _uniformIndices_ = (uniformIndices_ ? ((*env)->GetIntArrayElements(env,uniformIndices_,NULL) ) : NULL);
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(program_, uniformCount_, _uniformIndices_, pname_, _params_);
    if(uniformIndices_)
        (*env)->ReleaseIntArrayElements(env,uniformIndices_, _uniformIndices_,0);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glGetActiveUniformName (GLuint program, GLuint uniformIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformName);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetActiveUniformName( JNIEnv* env, jclass klass,jint program_,jint uniformIndex_,jint bufSize_,jintArray length_,jbyteArray uniformName_){
    if(do_trace)
        fprintf(stderr,"glGetActiveUniformName\n");
    static PFNGLGETACTIVEUNIFORMNAMEPROC impl;
    if( !impl ){
       impl = (PFNGLGETACTIVEUNIFORMNAMEPROC) mygetprocaddr(env,"glGetActiveUniformName");
       if(!impl)
            return;
    }
    jint* _length_ = (length_ ? ((*env)->GetIntArrayElements(env,length_,NULL) ) : NULL);
    jbyte* _uniformName_ = (uniformName_ ? ((*env)->GetByteArrayElements(env,uniformName_,NULL) ) : NULL);
    impl(program_, uniformIndex_, bufSize_, _length_, _uniformName_);
    if(length_)
        (*env)->ReleaseIntArrayElements(env,length_, _length_,0);
    if(uniformName_)
        (*env)->ReleaseByteArrayElements(env,uniformName_, _uniformName_,0);
}
//GLAPI GLuint APIENTRY glGetUniformBlockIndex (GLuint program, const GLchar *uniformBlockName);
JNIEXPORT jint JNICALL Java_JGL_JGL_glGetUniformBlockIndex( JNIEnv* env, jclass klass,jint program_,jbyteArray uniformBlockName_){
    jint rv = 0;
    if(do_trace)
        fprintf(stderr,"glGetUniformBlockIndex\n");
    static PFNGLGETUNIFORMBLOCKINDEXPROC impl;
    if( !impl ){
       impl = (PFNGLGETUNIFORMBLOCKINDEXPROC) mygetprocaddr(env,"glGetUniformBlockIndex");
       if(!impl)
            return rv;
    }
    jbyte* _uniformBlockName_ = (uniformBlockName_ ? ((*env)->GetByteArrayElements(env,uniformBlockName_,NULL) ) : NULL);
    rv = impl(program_, _uniformBlockName_);
    if(uniformBlockName_)
        (*env)->ReleaseByteArrayElements(env,uniformBlockName_, _uniformBlockName_,0);
    return rv;
}
//GLAPI void APIENTRY glGetActiveUniformBlockiv (GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetActiveUniformBlockiv( JNIEnv* env, jclass klass,jint program_,jint uniformBlockIndex_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetActiveUniformBlockiv\n");
    static PFNGLGETACTIVEUNIFORMBLOCKIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETACTIVEUNIFORMBLOCKIVPROC) mygetprocaddr(env,"glGetActiveUniformBlockiv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(program_, uniformBlockIndex_, pname_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glGetActiveUniformBlockName (GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei *length, GLchar *uniformBlockName);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetActiveUniformBlockName( JNIEnv* env, jclass klass,jint program_,jint uniformBlockIndex_,jint bufSize_,jintArray length_,jbyteArray uniformBlockName_){
    if(do_trace)
        fprintf(stderr,"glGetActiveUniformBlockName\n");
    static PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC impl;
    if( !impl ){
       impl = (PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC) mygetprocaddr(env,"glGetActiveUniformBlockName");
       if(!impl)
            return;
    }
    jint* _length_ = (length_ ? ((*env)->GetIntArrayElements(env,length_,NULL) ) : NULL);
    jbyte* _uniformBlockName_ = (uniformBlockName_ ? ((*env)->GetByteArrayElements(env,uniformBlockName_,NULL) ) : NULL);
    impl(program_, uniformBlockIndex_, bufSize_, _length_, _uniformBlockName_);
    if(length_)
        (*env)->ReleaseIntArrayElements(env,length_, _length_,0);
    if(uniformBlockName_)
        (*env)->ReleaseByteArrayElements(env,uniformBlockName_, _uniformBlockName_,0);
}
//GLAPI void APIENTRY glUniformBlockBinding (GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniformBlockBinding( JNIEnv* env, jclass klass,jint program_,jint uniformBlockIndex_,jint uniformBlockBinding_){
    if(do_trace)
        fprintf(stderr,"glUniformBlockBinding\n");
    static PFNGLUNIFORMBLOCKBINDINGPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORMBLOCKBINDINGPROC) mygetprocaddr(env,"glUniformBlockBinding");
       if(!impl)
            return;
    }
    impl(program_, uniformBlockIndex_, uniformBlockBinding_);
}
//GLAPI void APIENTRY glDrawElementsBaseVertex (GLenum mode, GLsizei count, GLenum type, const void *indices, GLint basevertex);
JNIEXPORT void JNICALL Java_JGL_JGL_glDrawElementsBaseVertex( JNIEnv* env, jclass klass,jint mode_,jint count_,jint type_,jint indices_,jint basevertex_){
    if(do_trace)
        fprintf(stderr,"glDrawElementsBaseVertex\n");
    static PFNGLDRAWELEMENTSBASEVERTEXPROC impl;
    if( !impl ){
       impl = (PFNGLDRAWELEMENTSBASEVERTEXPROC) mygetprocaddr(env,"glDrawElementsBaseVertex");
       if(!impl)
            return;
    }
    impl(mode_, count_, type_, (const void *)indices_, basevertex_);
}
//GLAPI void APIENTRY glDrawRangeElementsBaseVertex (GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices, GLint basevertex);
JNIEXPORT void JNICALL Java_JGL_JGL_glDrawRangeElementsBaseVertex( JNIEnv* env, jclass klass,jint mode_,jint start_,jint end_,jint count_,jint type_,jint indices_,jint basevertex_){
    if(do_trace)
        fprintf(stderr,"glDrawRangeElementsBaseVertex\n");
    static PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC impl;
    if( !impl ){
       impl = (PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC) mygetprocaddr(env,"glDrawRangeElementsBaseVertex");
       if(!impl)
            return;
    }
    impl(mode_, start_, end_, count_, type_, (const void *)indices_, basevertex_);
}
//GLAPI void APIENTRY glDrawElementsInstancedBaseVertex (GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLint basevertex);
JNIEXPORT void JNICALL Java_JGL_JGL_glDrawElementsInstancedBaseVertex( JNIEnv* env, jclass klass,jint mode_,jint count_,jint type_,jint indices_,jint instancecount_,jint basevertex_){
    if(do_trace)
        fprintf(stderr,"glDrawElementsInstancedBaseVertex\n");
    static PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC impl;
    if( !impl ){
       impl = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC) mygetprocaddr(env,"glDrawElementsInstancedBaseVertex");
       if(!impl)
            return;
    }
    impl(mode_, count_, type_, (const void *)indices_, instancecount_, basevertex_);
}
//GLAPI void APIENTRY glProvokingVertex (GLenum mode);
JNIEXPORT void JNICALL Java_JGL_JGL_glProvokingVertex( JNIEnv* env, jclass klass,jint mode_){
    if(do_trace)
        fprintf(stderr,"glProvokingVertex\n");
    static PFNGLPROVOKINGVERTEXPROC impl;
    if( !impl ){
       impl = (PFNGLPROVOKINGVERTEXPROC) mygetprocaddr(env,"glProvokingVertex");
       if(!impl)
            return;
    }
    impl(mode_);
}
//GLAPI GLsync APIENTRY glFenceSync (GLenum condition, GLbitfield flags);
JNIEXPORT jlong JNICALL Java_JGL_JGL_glFenceSync( JNIEnv* env, jclass klass,jint condition_,jint flags_){
    jlong rv = 0;
    if(do_trace)
        fprintf(stderr,"glFenceSync\n");
    static PFNGLFENCESYNCPROC impl;
    if( !impl ){
       impl = (PFNGLFENCESYNCPROC) mygetprocaddr(env,"glFenceSync");
       if(!impl)
            return rv;
    }
    rv = impl(condition_, flags_);
    return rv;
}
//GLAPI GLboolean APIENTRY glIsSync (GLsync sync);
JNIEXPORT jboolean JNICALL Java_JGL_JGL_glIsSync( JNIEnv* env, jclass klass,jlong sync_){
    jboolean rv = 0;
    if(do_trace)
        fprintf(stderr,"glIsSync\n");
    static PFNGLISSYNCPROC impl;
    if( !impl ){
       impl = (PFNGLISSYNCPROC) mygetprocaddr(env,"glIsSync");
       if(!impl)
            return rv;
    }
    rv = impl((GLsync)sync_);
    return rv;
}
//GLAPI void APIENTRY glDeleteSync (GLsync sync);
JNIEXPORT void JNICALL Java_JGL_JGL_glDeleteSync( JNIEnv* env, jclass klass,jlong sync_){
    if(do_trace)
        fprintf(stderr,"glDeleteSync\n");
    static PFNGLDELETESYNCPROC impl;
    if( !impl ){
       impl = (PFNGLDELETESYNCPROC) mygetprocaddr(env,"glDeleteSync");
       if(!impl)
            return;
    }
    impl((GLsync)sync_);
}
//GLAPI GLenum APIENTRY glClientWaitSync (GLsync sync, GLbitfield flags, GLuint64 timeout);
JNIEXPORT jint JNICALL Java_JGL_JGL_glClientWaitSync( JNIEnv* env, jclass klass,jlong sync_,jint flags_,jlong timeout_){
    jint rv = 0;
    if(do_trace)
        fprintf(stderr,"glClientWaitSync\n");
    static PFNGLCLIENTWAITSYNCPROC impl;
    if( !impl ){
       impl = (PFNGLCLIENTWAITSYNCPROC) mygetprocaddr(env,"glClientWaitSync");
       if(!impl)
            return rv;
    }
    rv = impl((GLsync)sync_, flags_, timeout_);
    return rv;
}
//GLAPI void APIENTRY glWaitSync (GLsync sync, GLbitfield flags, GLuint64 timeout);
JNIEXPORT void JNICALL Java_JGL_JGL_glWaitSync( JNIEnv* env, jclass klass,jlong sync_,jint flags_,jlong timeout_){
    if(do_trace)
        fprintf(stderr,"glWaitSync\n");
    static PFNGLWAITSYNCPROC impl;
    if( !impl ){
       impl = (PFNGLWAITSYNCPROC) mygetprocaddr(env,"glWaitSync");
       if(!impl)
            return;
    }
    impl((GLsync)sync_, flags_, timeout_);
}
//GLAPI void APIENTRY glGetInteger64v (GLenum pname, GLint64 *data);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetInteger64v( JNIEnv* env, jclass klass,jint pname_,jlongArray data_){
    if(do_trace)
        fprintf(stderr,"glGetInteger64v\n");
    static PFNGLGETINTEGER64VPROC impl;
    if( !impl ){
       impl = (PFNGLGETINTEGER64VPROC) mygetprocaddr(env,"glGetInteger64v");
       if(!impl)
            return;
    }
    jlong* _data_ = (data_ ? ((*env)->GetLongArrayElements(env,data_,NULL) ) : NULL);
    impl(pname_, _data_);
    if(data_)
        (*env)->ReleaseLongArrayElements(env,data_, _data_,0);
}
//GLAPI void APIENTRY glGetSynciv (GLsync sync, GLenum pname, GLsizei bufSize, GLsizei *length, GLint *values);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetSynciv( JNIEnv* env, jclass klass,jlong sync_,jint pname_,jint bufSize_,jintArray length_,jintArray values_){
    if(do_trace)
        fprintf(stderr,"glGetSynciv\n");
    static PFNGLGETSYNCIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETSYNCIVPROC) mygetprocaddr(env,"glGetSynciv");
       if(!impl)
            return;
    }
    jint* _length_ = (length_ ? ((*env)->GetIntArrayElements(env,length_,NULL) ) : NULL);
    jint* _values_ = (values_ ? ((*env)->GetIntArrayElements(env,values_,NULL) ) : NULL);
    impl((GLsync)sync_, pname_, bufSize_, _length_, _values_);
    if(length_)
        (*env)->ReleaseIntArrayElements(env,length_, _length_,0);
    if(values_)
        (*env)->ReleaseIntArrayElements(env,values_, _values_,0);
}
//GLAPI void APIENTRY glGetInteger64i_v (GLenum target, GLuint index, GLint64 *data);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetInteger64i_v( JNIEnv* env, jclass klass,jint target_,jint index_,jlongArray data_){
    if(do_trace)
        fprintf(stderr,"glGetInteger64i_v\n");
    static PFNGLGETINTEGER64I_VPROC impl;
    if( !impl ){
       impl = (PFNGLGETINTEGER64I_VPROC) mygetprocaddr(env,"glGetInteger64i_v");
       if(!impl)
            return;
    }
    jlong* _data_ = (data_ ? ((*env)->GetLongArrayElements(env,data_,NULL) ) : NULL);
    impl(target_, index_, _data_);
    if(data_)
        (*env)->ReleaseLongArrayElements(env,data_, _data_,0);
}
//GLAPI void APIENTRY glGetBufferParameteri64v (GLenum target, GLenum pname, GLint64 *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetBufferParameteri64v( JNIEnv* env, jclass klass,jint target_,jint pname_,jlongArray params_){
    if(do_trace)
        fprintf(stderr,"glGetBufferParameteri64v\n");
    static PFNGLGETBUFFERPARAMETERI64VPROC impl;
    if( !impl ){
       impl = (PFNGLGETBUFFERPARAMETERI64VPROC) mygetprocaddr(env,"glGetBufferParameteri64v");
       if(!impl)
            return;
    }
    jlong* _params_ = (params_ ? ((*env)->GetLongArrayElements(env,params_,NULL) ) : NULL);
    impl(target_, pname_, _params_);
    if(params_)
        (*env)->ReleaseLongArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glFramebufferTexture (GLenum target, GLenum attachment, GLuint texture, GLint level);
JNIEXPORT void JNICALL Java_JGL_JGL_glFramebufferTexture( JNIEnv* env, jclass klass,jint target_,jint attachment_,jint texture_,jint level_){
    if(do_trace)
        fprintf(stderr,"glFramebufferTexture\n");
    static PFNGLFRAMEBUFFERTEXTUREPROC impl;
    if( !impl ){
       impl = (PFNGLFRAMEBUFFERTEXTUREPROC) mygetprocaddr(env,"glFramebufferTexture");
       if(!impl)
            return;
    }
    impl(target_, attachment_, texture_, level_);
}
//GLAPI void APIENTRY glTexImage2DMultisample (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
JNIEXPORT void JNICALL Java_JGL_JGL_glTexImage2DMultisample( JNIEnv* env, jclass klass,jint target_,jint samples_,jint internalformat_,jint width_,jint height_,jboolean fixedsamplelocations_){
    if(do_trace)
        fprintf(stderr,"glTexImage2DMultisample\n");
    static PFNGLTEXIMAGE2DMULTISAMPLEPROC impl;
    if( !impl ){
       impl = (PFNGLTEXIMAGE2DMULTISAMPLEPROC) mygetprocaddr(env,"glTexImage2DMultisample");
       if(!impl)
            return;
    }
    impl(target_, samples_, internalformat_, width_, height_, fixedsamplelocations_);
}
//GLAPI void APIENTRY glTexImage3DMultisample (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
JNIEXPORT void JNICALL Java_JGL_JGL_glTexImage3DMultisample( JNIEnv* env, jclass klass,jint target_,jint samples_,jint internalformat_,jint width_,jint height_,jint depth_,jboolean fixedsamplelocations_){
    if(do_trace)
        fprintf(stderr,"glTexImage3DMultisample\n");
    static PFNGLTEXIMAGE3DMULTISAMPLEPROC impl;
    if( !impl ){
       impl = (PFNGLTEXIMAGE3DMULTISAMPLEPROC) mygetprocaddr(env,"glTexImage3DMultisample");
       if(!impl)
            return;
    }
    impl(target_, samples_, internalformat_, width_, height_, depth_, fixedsamplelocations_);
}
//GLAPI void APIENTRY glGetMultisamplefv (GLenum pname, GLuint index, GLfloat *val);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetMultisamplefv( JNIEnv* env, jclass klass,jint pname_,jint index_,jfloatArray val_){
    if(do_trace)
        fprintf(stderr,"glGetMultisamplefv\n");
    static PFNGLGETMULTISAMPLEFVPROC impl;
    if( !impl ){
       impl = (PFNGLGETMULTISAMPLEFVPROC) mygetprocaddr(env,"glGetMultisamplefv");
       if(!impl)
            return;
    }
    jfloat* _val_ = (val_ ? ((*env)->GetFloatArrayElements(env,val_,NULL) ) : NULL);
    impl(pname_, index_, _val_);
    if(val_)
        (*env)->ReleaseFloatArrayElements(env,val_, _val_,0);
}
//GLAPI void APIENTRY glSampleMaski (GLuint maskNumber, GLbitfield mask);
JNIEXPORT void JNICALL Java_JGL_JGL_glSampleMaski( JNIEnv* env, jclass klass,jint maskNumber_,jint mask_){
    if(do_trace)
        fprintf(stderr,"glSampleMaski\n");
    static PFNGLSAMPLEMASKIPROC impl;
    if( !impl ){
       impl = (PFNGLSAMPLEMASKIPROC) mygetprocaddr(env,"glSampleMaski");
       if(!impl)
            return;
    }
    impl(maskNumber_, mask_);
}
//GLAPI void APIENTRY glBindFragDataLocationIndexed (GLuint program, GLuint colorNumber, GLuint index, const GLchar *name);
JNIEXPORT void JNICALL Java_JGL_JGL_glBindFragDataLocationIndexed( JNIEnv* env, jclass klass,jint program_,jint colorNumber_,jint index_,jbyteArray name_){
    if(do_trace)
        fprintf(stderr,"glBindFragDataLocationIndexed\n");
    static PFNGLBINDFRAGDATALOCATIONINDEXEDPROC impl;
    if( !impl ){
       impl = (PFNGLBINDFRAGDATALOCATIONINDEXEDPROC) mygetprocaddr(env,"glBindFragDataLocationIndexed");
       if(!impl)
            return;
    }
    jbyte* _name_ = (name_ ? ((*env)->GetByteArrayElements(env,name_,NULL) ) : NULL);
    impl(program_, colorNumber_, index_, _name_);
    if(name_)
        (*env)->ReleaseByteArrayElements(env,name_, _name_,0);
}
//GLAPI GLint APIENTRY glGetFragDataIndex (GLuint program, const GLchar *name);
JNIEXPORT jint JNICALL Java_JGL_JGL_glGetFragDataIndex( JNIEnv* env, jclass klass,jint program_,jbyteArray name_){
    jint rv = 0;
    if(do_trace)
        fprintf(stderr,"glGetFragDataIndex\n");
    static PFNGLGETFRAGDATAINDEXPROC impl;
    if( !impl ){
       impl = (PFNGLGETFRAGDATAINDEXPROC) mygetprocaddr(env,"glGetFragDataIndex");
       if(!impl)
            return rv;
    }
    jbyte* _name_ = (name_ ? ((*env)->GetByteArrayElements(env,name_,NULL) ) : NULL);
    rv = impl(program_, _name_);
    if(name_)
        (*env)->ReleaseByteArrayElements(env,name_, _name_,0);
    return rv;
}
//GLAPI void APIENTRY glGenSamplers (GLsizei count, GLuint *samplers);
JNIEXPORT void JNICALL Java_JGL_JGL_glGenSamplers( JNIEnv* env, jclass klass,jint count_,jintArray samplers_){
    if(do_trace)
        fprintf(stderr,"glGenSamplers\n");
    static PFNGLGENSAMPLERSPROC impl;
    if( !impl ){
       impl = (PFNGLGENSAMPLERSPROC) mygetprocaddr(env,"glGenSamplers");
       if(!impl)
            return;
    }
    jint* _samplers_ = (samplers_ ? ((*env)->GetIntArrayElements(env,samplers_,NULL) ) : NULL);
    impl(count_, _samplers_);
    if(samplers_)
        (*env)->ReleaseIntArrayElements(env,samplers_, _samplers_,0);
}
//GLAPI void APIENTRY glDeleteSamplers (GLsizei count, const GLuint *samplers);
JNIEXPORT void JNICALL Java_JGL_JGL_glDeleteSamplers( JNIEnv* env, jclass klass,jint count_,jintArray samplers_){
    if(do_trace)
        fprintf(stderr,"glDeleteSamplers\n");
    static PFNGLDELETESAMPLERSPROC impl;
    if( !impl ){
       impl = (PFNGLDELETESAMPLERSPROC) mygetprocaddr(env,"glDeleteSamplers");
       if(!impl)
            return;
    }
    jint* _samplers_ = (samplers_ ? ((*env)->GetIntArrayElements(env,samplers_,NULL) ) : NULL);
    impl(count_, _samplers_);
    if(samplers_)
        (*env)->ReleaseIntArrayElements(env,samplers_, _samplers_,0);
}
//GLAPI GLboolean APIENTRY glIsSampler (GLuint sampler);
JNIEXPORT jboolean JNICALL Java_JGL_JGL_glIsSampler( JNIEnv* env, jclass klass,jint sampler_){
    jboolean rv = 0;
    if(do_trace)
        fprintf(stderr,"glIsSampler\n");
    static PFNGLISSAMPLERPROC impl;
    if( !impl ){
       impl = (PFNGLISSAMPLERPROC) mygetprocaddr(env,"glIsSampler");
       if(!impl)
            return rv;
    }
    rv = impl(sampler_);
    return rv;
}
//GLAPI void APIENTRY glBindSampler (GLuint unit, GLuint sampler);
JNIEXPORT void JNICALL Java_JGL_JGL_glBindSampler( JNIEnv* env, jclass klass,jint unit_,jint sampler_){
    if(do_trace)
        fprintf(stderr,"glBindSampler\n");
    static PFNGLBINDSAMPLERPROC impl;
    if( !impl ){
       impl = (PFNGLBINDSAMPLERPROC) mygetprocaddr(env,"glBindSampler");
       if(!impl)
            return;
    }
    impl(unit_, sampler_);
}
//GLAPI void APIENTRY glSamplerParameteri (GLuint sampler, GLenum pname, GLint param);
JNIEXPORT void JNICALL Java_JGL_JGL_glSamplerParameteri( JNIEnv* env, jclass klass,jint sampler_,jint pname_,jint param_){
    if(do_trace)
        fprintf(stderr,"glSamplerParameteri\n");
    static PFNGLSAMPLERPARAMETERIPROC impl;
    if( !impl ){
       impl = (PFNGLSAMPLERPARAMETERIPROC) mygetprocaddr(env,"glSamplerParameteri");
       if(!impl)
            return;
    }
    impl(sampler_, pname_, param_);
}
//GLAPI void APIENTRY glSamplerParameteriv (GLuint sampler, GLenum pname, const GLint *param);
JNIEXPORT void JNICALL Java_JGL_JGL_glSamplerParameteriv( JNIEnv* env, jclass klass,jint sampler_,jint pname_,jintArray param_){
    if(do_trace)
        fprintf(stderr,"glSamplerParameteriv\n");
    static PFNGLSAMPLERPARAMETERIVPROC impl;
    if( !impl ){
       impl = (PFNGLSAMPLERPARAMETERIVPROC) mygetprocaddr(env,"glSamplerParameteriv");
       if(!impl)
            return;
    }
    jint* _param_ = (param_ ? ((*env)->GetIntArrayElements(env,param_,NULL) ) : NULL);
    impl(sampler_, pname_, _param_);
    if(param_)
        (*env)->ReleaseIntArrayElements(env,param_, _param_,0);
}
//GLAPI void APIENTRY glSamplerParameterf (GLuint sampler, GLenum pname, GLfloat param);
JNIEXPORT void JNICALL Java_JGL_JGL_glSamplerParameterf( JNIEnv* env, jclass klass,jint sampler_,jint pname_,jfloat param_){
    if(do_trace)
        fprintf(stderr,"glSamplerParameterf\n");
    static PFNGLSAMPLERPARAMETERFPROC impl;
    if( !impl ){
       impl = (PFNGLSAMPLERPARAMETERFPROC) mygetprocaddr(env,"glSamplerParameterf");
       if(!impl)
            return;
    }
    impl(sampler_, pname_, param_);
}
//GLAPI void APIENTRY glSamplerParameterfv (GLuint sampler, GLenum pname, const GLfloat *param);
JNIEXPORT void JNICALL Java_JGL_JGL_glSamplerParameterfv( JNIEnv* env, jclass klass,jint sampler_,jint pname_,jfloatArray param_){
    if(do_trace)
        fprintf(stderr,"glSamplerParameterfv\n");
    static PFNGLSAMPLERPARAMETERFVPROC impl;
    if( !impl ){
       impl = (PFNGLSAMPLERPARAMETERFVPROC) mygetprocaddr(env,"glSamplerParameterfv");
       if(!impl)
            return;
    }
    jfloat* _param_ = (param_ ? ((*env)->GetFloatArrayElements(env,param_,NULL) ) : NULL);
    impl(sampler_, pname_, _param_);
    if(param_)
        (*env)->ReleaseFloatArrayElements(env,param_, _param_,0);
}
//GLAPI void APIENTRY glSamplerParameterIiv (GLuint sampler, GLenum pname, const GLint *param);
JNIEXPORT void JNICALL Java_JGL_JGL_glSamplerParameterIiv( JNIEnv* env, jclass klass,jint sampler_,jint pname_,jintArray param_){
    if(do_trace)
        fprintf(stderr,"glSamplerParameterIiv\n");
    static PFNGLSAMPLERPARAMETERIIVPROC impl;
    if( !impl ){
       impl = (PFNGLSAMPLERPARAMETERIIVPROC) mygetprocaddr(env,"glSamplerParameterIiv");
       if(!impl)
            return;
    }
    jint* _param_ = (param_ ? ((*env)->GetIntArrayElements(env,param_,NULL) ) : NULL);
    impl(sampler_, pname_, _param_);
    if(param_)
        (*env)->ReleaseIntArrayElements(env,param_, _param_,0);
}
//GLAPI void APIENTRY glSamplerParameterIuiv (GLuint sampler, GLenum pname, const GLuint *param);
JNIEXPORT void JNICALL Java_JGL_JGL_glSamplerParameterIuiv( JNIEnv* env, jclass klass,jint sampler_,jint pname_,jintArray param_){
    if(do_trace)
        fprintf(stderr,"glSamplerParameterIuiv\n");
    static PFNGLSAMPLERPARAMETERIUIVPROC impl;
    if( !impl ){
       impl = (PFNGLSAMPLERPARAMETERIUIVPROC) mygetprocaddr(env,"glSamplerParameterIuiv");
       if(!impl)
            return;
    }
    jint* _param_ = (param_ ? ((*env)->GetIntArrayElements(env,param_,NULL) ) : NULL);
    impl(sampler_, pname_, _param_);
    if(param_)
        (*env)->ReleaseIntArrayElements(env,param_, _param_,0);
}
//GLAPI void APIENTRY glGetSamplerParameteriv (GLuint sampler, GLenum pname, GLint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetSamplerParameteriv( JNIEnv* env, jclass klass,jint sampler_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetSamplerParameteriv\n");
    static PFNGLGETSAMPLERPARAMETERIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETSAMPLERPARAMETERIVPROC) mygetprocaddr(env,"glGetSamplerParameteriv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(sampler_, pname_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glGetSamplerParameterIiv (GLuint sampler, GLenum pname, GLint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetSamplerParameterIiv( JNIEnv* env, jclass klass,jint sampler_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetSamplerParameterIiv\n");
    static PFNGLGETSAMPLERPARAMETERIIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETSAMPLERPARAMETERIIVPROC) mygetprocaddr(env,"glGetSamplerParameterIiv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(sampler_, pname_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glGetSamplerParameterfv (GLuint sampler, GLenum pname, GLfloat *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetSamplerParameterfv( JNIEnv* env, jclass klass,jint sampler_,jint pname_,jfloatArray params_){
    if(do_trace)
        fprintf(stderr,"glGetSamplerParameterfv\n");
    static PFNGLGETSAMPLERPARAMETERFVPROC impl;
    if( !impl ){
       impl = (PFNGLGETSAMPLERPARAMETERFVPROC) mygetprocaddr(env,"glGetSamplerParameterfv");
       if(!impl)
            return;
    }
    jfloat* _params_ = (params_ ? ((*env)->GetFloatArrayElements(env,params_,NULL) ) : NULL);
    impl(sampler_, pname_, _params_);
    if(params_)
        (*env)->ReleaseFloatArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glGetSamplerParameterIuiv (GLuint sampler, GLenum pname, GLuint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetSamplerParameterIuiv( JNIEnv* env, jclass klass,jint sampler_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetSamplerParameterIuiv\n");
    static PFNGLGETSAMPLERPARAMETERIUIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETSAMPLERPARAMETERIUIVPROC) mygetprocaddr(env,"glGetSamplerParameterIuiv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(sampler_, pname_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glQueryCounter (GLuint id, GLenum target);
JNIEXPORT void JNICALL Java_JGL_JGL_glQueryCounter( JNIEnv* env, jclass klass,jint id_,jint target_){
    if(do_trace)
        fprintf(stderr,"glQueryCounter\n");
    static PFNGLQUERYCOUNTERPROC impl;
    if( !impl ){
       impl = (PFNGLQUERYCOUNTERPROC) mygetprocaddr(env,"glQueryCounter");
       if(!impl)
            return;
    }
    impl(id_, target_);
}
//GLAPI void APIENTRY glGetQueryObjecti64v (GLuint id, GLenum pname, GLint64 *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetQueryObjecti64v( JNIEnv* env, jclass klass,jint id_,jint pname_,jlongArray params_){
    if(do_trace)
        fprintf(stderr,"glGetQueryObjecti64v\n");
    static PFNGLGETQUERYOBJECTI64VPROC impl;
    if( !impl ){
       impl = (PFNGLGETQUERYOBJECTI64VPROC) mygetprocaddr(env,"glGetQueryObjecti64v");
       if(!impl)
            return;
    }
    jlong* _params_ = (params_ ? ((*env)->GetLongArrayElements(env,params_,NULL) ) : NULL);
    impl(id_, pname_, _params_);
    if(params_)
        (*env)->ReleaseLongArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glGetQueryObjectui64v (GLuint id, GLenum pname, GLuint64 *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetQueryObjectui64v( JNIEnv* env, jclass klass,jint id_,jint pname_,jlongArray params_){
    if(do_trace)
        fprintf(stderr,"glGetQueryObjectui64v\n");
    static PFNGLGETQUERYOBJECTUI64VPROC impl;
    if( !impl ){
       impl = (PFNGLGETQUERYOBJECTUI64VPROC) mygetprocaddr(env,"glGetQueryObjectui64v");
       if(!impl)
            return;
    }
    jlong* _params_ = (params_ ? ((*env)->GetLongArrayElements(env,params_,NULL) ) : NULL);
    impl(id_, pname_, _params_);
    if(params_)
        (*env)->ReleaseLongArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glVertexAttribDivisor (GLuint index, GLuint divisor);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribDivisor( JNIEnv* env, jclass klass,jint index_,jint divisor_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribDivisor\n");
    static PFNGLVERTEXATTRIBDIVISORPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBDIVISORPROC) mygetprocaddr(env,"glVertexAttribDivisor");
       if(!impl)
            return;
    }
    impl(index_, divisor_);
}
//GLAPI void APIENTRY glVertexAttribP1ui (GLuint index, GLenum type, GLboolean normalized, GLuint value);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribP1ui( JNIEnv* env, jclass klass,jint index_,jint type_,jboolean normalized_,jint value_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribP1ui\n");
    static PFNGLVERTEXATTRIBP1UIPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBP1UIPROC) mygetprocaddr(env,"glVertexAttribP1ui");
       if(!impl)
            return;
    }
    impl(index_, type_, normalized_, value_);
}
//GLAPI void APIENTRY glVertexAttribP1uiv (GLuint index, GLenum type, GLboolean normalized, const GLuint *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribP1uiv( JNIEnv* env, jclass klass,jint index_,jint type_,jboolean normalized_,jintArray value_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribP1uiv\n");
    static PFNGLVERTEXATTRIBP1UIVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBP1UIVPROC) mygetprocaddr(env,"glVertexAttribP1uiv");
       if(!impl)
            return;
    }
    jint* _value_ = (value_ ? ((*env)->GetIntArrayElements(env,value_,NULL) ) : NULL);
    impl(index_, type_, normalized_, _value_);
    if(value_)
        (*env)->ReleaseIntArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glVertexAttribP2ui (GLuint index, GLenum type, GLboolean normalized, GLuint value);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribP2ui( JNIEnv* env, jclass klass,jint index_,jint type_,jboolean normalized_,jint value_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribP2ui\n");
    static PFNGLVERTEXATTRIBP2UIPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBP2UIPROC) mygetprocaddr(env,"glVertexAttribP2ui");
       if(!impl)
            return;
    }
    impl(index_, type_, normalized_, value_);
}
//GLAPI void APIENTRY glVertexAttribP2uiv (GLuint index, GLenum type, GLboolean normalized, const GLuint *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribP2uiv( JNIEnv* env, jclass klass,jint index_,jint type_,jboolean normalized_,jintArray value_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribP2uiv\n");
    static PFNGLVERTEXATTRIBP2UIVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBP2UIVPROC) mygetprocaddr(env,"glVertexAttribP2uiv");
       if(!impl)
            return;
    }
    jint* _value_ = (value_ ? ((*env)->GetIntArrayElements(env,value_,NULL) ) : NULL);
    impl(index_, type_, normalized_, _value_);
    if(value_)
        (*env)->ReleaseIntArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glVertexAttribP3ui (GLuint index, GLenum type, GLboolean normalized, GLuint value);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribP3ui( JNIEnv* env, jclass klass,jint index_,jint type_,jboolean normalized_,jint value_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribP3ui\n");
    static PFNGLVERTEXATTRIBP3UIPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBP3UIPROC) mygetprocaddr(env,"glVertexAttribP3ui");
       if(!impl)
            return;
    }
    impl(index_, type_, normalized_, value_);
}
//GLAPI void APIENTRY glVertexAttribP3uiv (GLuint index, GLenum type, GLboolean normalized, const GLuint *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribP3uiv( JNIEnv* env, jclass klass,jint index_,jint type_,jboolean normalized_,jintArray value_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribP3uiv\n");
    static PFNGLVERTEXATTRIBP3UIVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBP3UIVPROC) mygetprocaddr(env,"glVertexAttribP3uiv");
       if(!impl)
            return;
    }
    jint* _value_ = (value_ ? ((*env)->GetIntArrayElements(env,value_,NULL) ) : NULL);
    impl(index_, type_, normalized_, _value_);
    if(value_)
        (*env)->ReleaseIntArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glVertexAttribP4ui (GLuint index, GLenum type, GLboolean normalized, GLuint value);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribP4ui( JNIEnv* env, jclass klass,jint index_,jint type_,jboolean normalized_,jint value_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribP4ui\n");
    static PFNGLVERTEXATTRIBP4UIPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBP4UIPROC) mygetprocaddr(env,"glVertexAttribP4ui");
       if(!impl)
            return;
    }
    impl(index_, type_, normalized_, value_);
}
//GLAPI void APIENTRY glVertexAttribP4uiv (GLuint index, GLenum type, GLboolean normalized, const GLuint *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribP4uiv( JNIEnv* env, jclass klass,jint index_,jint type_,jboolean normalized_,jintArray value_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribP4uiv\n");
    static PFNGLVERTEXATTRIBP4UIVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBP4UIVPROC) mygetprocaddr(env,"glVertexAttribP4uiv");
       if(!impl)
            return;
    }
    jint* _value_ = (value_ ? ((*env)->GetIntArrayElements(env,value_,NULL) ) : NULL);
    impl(index_, type_, normalized_, _value_);
    if(value_)
        (*env)->ReleaseIntArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glMinSampleShading (GLfloat value);
JNIEXPORT void JNICALL Java_JGL_JGL_glMinSampleShading( JNIEnv* env, jclass klass,jfloat value_){
    if(do_trace)
        fprintf(stderr,"glMinSampleShading\n");
    static PFNGLMINSAMPLESHADINGPROC impl;
    if( !impl ){
       impl = (PFNGLMINSAMPLESHADINGPROC) mygetprocaddr(env,"glMinSampleShading");
       if(!impl)
            return;
    }
    impl(value_);
}
//GLAPI void APIENTRY glBlendEquationi (GLuint buf, GLenum mode);
JNIEXPORT void JNICALL Java_JGL_JGL_glBlendEquationi( JNIEnv* env, jclass klass,jint buf_,jint mode_){
    if(do_trace)
        fprintf(stderr,"glBlendEquationi\n");
    static PFNGLBLENDEQUATIONIPROC impl;
    if( !impl ){
       impl = (PFNGLBLENDEQUATIONIPROC) mygetprocaddr(env,"glBlendEquationi");
       if(!impl)
            return;
    }
    impl(buf_, mode_);
}
//GLAPI void APIENTRY glBlendEquationSeparatei (GLuint buf, GLenum modeRGB, GLenum modeAlpha);
JNIEXPORT void JNICALL Java_JGL_JGL_glBlendEquationSeparatei( JNIEnv* env, jclass klass,jint buf_,jint modeRGB_,jint modeAlpha_){
    if(do_trace)
        fprintf(stderr,"glBlendEquationSeparatei\n");
    static PFNGLBLENDEQUATIONSEPARATEIPROC impl;
    if( !impl ){
       impl = (PFNGLBLENDEQUATIONSEPARATEIPROC) mygetprocaddr(env,"glBlendEquationSeparatei");
       if(!impl)
            return;
    }
    impl(buf_, modeRGB_, modeAlpha_);
}
//GLAPI void APIENTRY glBlendFunci (GLuint buf, GLenum src, GLenum dst);
JNIEXPORT void JNICALL Java_JGL_JGL_glBlendFunci( JNIEnv* env, jclass klass,jint buf_,jint src_,jint dst_){
    if(do_trace)
        fprintf(stderr,"glBlendFunci\n");
    static PFNGLBLENDFUNCIPROC impl;
    if( !impl ){
       impl = (PFNGLBLENDFUNCIPROC) mygetprocaddr(env,"glBlendFunci");
       if(!impl)
            return;
    }
    impl(buf_, src_, dst_);
}
//GLAPI void APIENTRY glBlendFuncSeparatei (GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
JNIEXPORT void JNICALL Java_JGL_JGL_glBlendFuncSeparatei( JNIEnv* env, jclass klass,jint buf_,jint srcRGB_,jint dstRGB_,jint srcAlpha_,jint dstAlpha_){
    if(do_trace)
        fprintf(stderr,"glBlendFuncSeparatei\n");
    static PFNGLBLENDFUNCSEPARATEIPROC impl;
    if( !impl ){
       impl = (PFNGLBLENDFUNCSEPARATEIPROC) mygetprocaddr(env,"glBlendFuncSeparatei");
       if(!impl)
            return;
    }
    impl(buf_, srcRGB_, dstRGB_, srcAlpha_, dstAlpha_);
}
//GLAPI void APIENTRY glDrawArraysIndirect (GLenum mode, const void *indirect);
JNIEXPORT void JNICALL Java_JGL_JGL_glDrawArraysIndirect( JNIEnv* env, jclass klass,jint mode_,jint indirect_){
    if(do_trace)
        fprintf(stderr,"glDrawArraysIndirect\n");
    static PFNGLDRAWARRAYSINDIRECTPROC impl;
    if( !impl ){
       impl = (PFNGLDRAWARRAYSINDIRECTPROC) mygetprocaddr(env,"glDrawArraysIndirect");
       if(!impl)
            return;
    }
    impl(mode_, (const void *)indirect_);
}
//GLAPI void APIENTRY glDrawElementsIndirect (GLenum mode, GLenum type, const void *indirect);
JNIEXPORT void JNICALL Java_JGL_JGL_glDrawElementsIndirect( JNIEnv* env, jclass klass,jint mode_,jint type_,jint indirect_){
    if(do_trace)
        fprintf(stderr,"glDrawElementsIndirect\n");
    static PFNGLDRAWELEMENTSINDIRECTPROC impl;
    if( !impl ){
       impl = (PFNGLDRAWELEMENTSINDIRECTPROC) mygetprocaddr(env,"glDrawElementsIndirect");
       if(!impl)
            return;
    }
    impl(mode_, type_, (const void *)indirect_);
}
//GLAPI void APIENTRY glUniform1d (GLint location, GLdouble x);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniform1d( JNIEnv* env, jclass klass,jint location_,jdouble x_){
    if(do_trace)
        fprintf(stderr,"glUniform1d\n");
    static PFNGLUNIFORM1DPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORM1DPROC) mygetprocaddr(env,"glUniform1d");
       if(!impl)
            return;
    }
    impl(location_, x_);
}
//GLAPI void APIENTRY glUniform2d (GLint location, GLdouble x, GLdouble y);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniform2d( JNIEnv* env, jclass klass,jint location_,jdouble x_,jdouble y_){
    if(do_trace)
        fprintf(stderr,"glUniform2d\n");
    static PFNGLUNIFORM2DPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORM2DPROC) mygetprocaddr(env,"glUniform2d");
       if(!impl)
            return;
    }
    impl(location_, x_, y_);
}
//GLAPI void APIENTRY glUniform3d (GLint location, GLdouble x, GLdouble y, GLdouble z);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniform3d( JNIEnv* env, jclass klass,jint location_,jdouble x_,jdouble y_,jdouble z_){
    if(do_trace)
        fprintf(stderr,"glUniform3d\n");
    static PFNGLUNIFORM3DPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORM3DPROC) mygetprocaddr(env,"glUniform3d");
       if(!impl)
            return;
    }
    impl(location_, x_, y_, z_);
}
//GLAPI void APIENTRY glUniform4d (GLint location, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniform4d( JNIEnv* env, jclass klass,jint location_,jdouble x_,jdouble y_,jdouble z_,jdouble w_){
    if(do_trace)
        fprintf(stderr,"glUniform4d\n");
    static PFNGLUNIFORM4DPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORM4DPROC) mygetprocaddr(env,"glUniform4d");
       if(!impl)
            return;
    }
    impl(location_, x_, y_, z_, w_);
}
//GLAPI void APIENTRY glUniform1dv (GLint location, GLsizei count, const GLdouble *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniform1dv( JNIEnv* env, jclass klass,jint location_,jint count_,jdoubleArray value_){
    if(do_trace)
        fprintf(stderr,"glUniform1dv\n");
    static PFNGLUNIFORM1DVPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORM1DVPROC) mygetprocaddr(env,"glUniform1dv");
       if(!impl)
            return;
    }
    jdouble* _value_ = (value_ ? ((*env)->GetDoubleArrayElements(env,value_,NULL) ) : NULL);
    impl(location_, count_, _value_);
    if(value_)
        (*env)->ReleaseDoubleArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glUniform2dv (GLint location, GLsizei count, const GLdouble *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniform2dv( JNIEnv* env, jclass klass,jint location_,jint count_,jdoubleArray value_){
    if(do_trace)
        fprintf(stderr,"glUniform2dv\n");
    static PFNGLUNIFORM2DVPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORM2DVPROC) mygetprocaddr(env,"glUniform2dv");
       if(!impl)
            return;
    }
    jdouble* _value_ = (value_ ? ((*env)->GetDoubleArrayElements(env,value_,NULL) ) : NULL);
    impl(location_, count_, _value_);
    if(value_)
        (*env)->ReleaseDoubleArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glUniform3dv (GLint location, GLsizei count, const GLdouble *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniform3dv( JNIEnv* env, jclass klass,jint location_,jint count_,jdoubleArray value_){
    if(do_trace)
        fprintf(stderr,"glUniform3dv\n");
    static PFNGLUNIFORM3DVPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORM3DVPROC) mygetprocaddr(env,"glUniform3dv");
       if(!impl)
            return;
    }
    jdouble* _value_ = (value_ ? ((*env)->GetDoubleArrayElements(env,value_,NULL) ) : NULL);
    impl(location_, count_, _value_);
    if(value_)
        (*env)->ReleaseDoubleArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glUniform4dv (GLint location, GLsizei count, const GLdouble *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniform4dv( JNIEnv* env, jclass klass,jint location_,jint count_,jdoubleArray value_){
    if(do_trace)
        fprintf(stderr,"glUniform4dv\n");
    static PFNGLUNIFORM4DVPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORM4DVPROC) mygetprocaddr(env,"glUniform4dv");
       if(!impl)
            return;
    }
    jdouble* _value_ = (value_ ? ((*env)->GetDoubleArrayElements(env,value_,NULL) ) : NULL);
    impl(location_, count_, _value_);
    if(value_)
        (*env)->ReleaseDoubleArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glUniformMatrix2dv (GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniformMatrix2dv( JNIEnv* env, jclass klass,jint location_,jint count_,jboolean transpose_,jdoubleArray value_){
    if(do_trace)
        fprintf(stderr,"glUniformMatrix2dv\n");
    static PFNGLUNIFORMMATRIX2DVPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORMMATRIX2DVPROC) mygetprocaddr(env,"glUniformMatrix2dv");
       if(!impl)
            return;
    }
    jdouble* _value_ = (value_ ? ((*env)->GetDoubleArrayElements(env,value_,NULL) ) : NULL);
    impl(location_, count_, transpose_, _value_);
    if(value_)
        (*env)->ReleaseDoubleArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glUniformMatrix3dv (GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniformMatrix3dv( JNIEnv* env, jclass klass,jint location_,jint count_,jboolean transpose_,jdoubleArray value_){
    if(do_trace)
        fprintf(stderr,"glUniformMatrix3dv\n");
    static PFNGLUNIFORMMATRIX3DVPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORMMATRIX3DVPROC) mygetprocaddr(env,"glUniformMatrix3dv");
       if(!impl)
            return;
    }
    jdouble* _value_ = (value_ ? ((*env)->GetDoubleArrayElements(env,value_,NULL) ) : NULL);
    impl(location_, count_, transpose_, _value_);
    if(value_)
        (*env)->ReleaseDoubleArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glUniformMatrix4dv (GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniformMatrix4dv( JNIEnv* env, jclass klass,jint location_,jint count_,jboolean transpose_,jdoubleArray value_){
    if(do_trace)
        fprintf(stderr,"glUniformMatrix4dv\n");
    static PFNGLUNIFORMMATRIX4DVPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORMMATRIX4DVPROC) mygetprocaddr(env,"glUniformMatrix4dv");
       if(!impl)
            return;
    }
    jdouble* _value_ = (value_ ? ((*env)->GetDoubleArrayElements(env,value_,NULL) ) : NULL);
    impl(location_, count_, transpose_, _value_);
    if(value_)
        (*env)->ReleaseDoubleArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glUniformMatrix2x3dv (GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniformMatrix2x3dv( JNIEnv* env, jclass klass,jint location_,jint count_,jboolean transpose_,jdoubleArray value_){
    if(do_trace)
        fprintf(stderr,"glUniformMatrix2x3dv\n");
    static PFNGLUNIFORMMATRIX2X3DVPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORMMATRIX2X3DVPROC) mygetprocaddr(env,"glUniformMatrix2x3dv");
       if(!impl)
            return;
    }
    jdouble* _value_ = (value_ ? ((*env)->GetDoubleArrayElements(env,value_,NULL) ) : NULL);
    impl(location_, count_, transpose_, _value_);
    if(value_)
        (*env)->ReleaseDoubleArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glUniformMatrix2x4dv (GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniformMatrix2x4dv( JNIEnv* env, jclass klass,jint location_,jint count_,jboolean transpose_,jdoubleArray value_){
    if(do_trace)
        fprintf(stderr,"glUniformMatrix2x4dv\n");
    static PFNGLUNIFORMMATRIX2X4DVPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORMMATRIX2X4DVPROC) mygetprocaddr(env,"glUniformMatrix2x4dv");
       if(!impl)
            return;
    }
    jdouble* _value_ = (value_ ? ((*env)->GetDoubleArrayElements(env,value_,NULL) ) : NULL);
    impl(location_, count_, transpose_, _value_);
    if(value_)
        (*env)->ReleaseDoubleArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glUniformMatrix3x2dv (GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniformMatrix3x2dv( JNIEnv* env, jclass klass,jint location_,jint count_,jboolean transpose_,jdoubleArray value_){
    if(do_trace)
        fprintf(stderr,"glUniformMatrix3x2dv\n");
    static PFNGLUNIFORMMATRIX3X2DVPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORMMATRIX3X2DVPROC) mygetprocaddr(env,"glUniformMatrix3x2dv");
       if(!impl)
            return;
    }
    jdouble* _value_ = (value_ ? ((*env)->GetDoubleArrayElements(env,value_,NULL) ) : NULL);
    impl(location_, count_, transpose_, _value_);
    if(value_)
        (*env)->ReleaseDoubleArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glUniformMatrix3x4dv (GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniformMatrix3x4dv( JNIEnv* env, jclass klass,jint location_,jint count_,jboolean transpose_,jdoubleArray value_){
    if(do_trace)
        fprintf(stderr,"glUniformMatrix3x4dv\n");
    static PFNGLUNIFORMMATRIX3X4DVPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORMMATRIX3X4DVPROC) mygetprocaddr(env,"glUniformMatrix3x4dv");
       if(!impl)
            return;
    }
    jdouble* _value_ = (value_ ? ((*env)->GetDoubleArrayElements(env,value_,NULL) ) : NULL);
    impl(location_, count_, transpose_, _value_);
    if(value_)
        (*env)->ReleaseDoubleArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glUniformMatrix4x2dv (GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniformMatrix4x2dv( JNIEnv* env, jclass klass,jint location_,jint count_,jboolean transpose_,jdoubleArray value_){
    if(do_trace)
        fprintf(stderr,"glUniformMatrix4x2dv\n");
    static PFNGLUNIFORMMATRIX4X2DVPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORMMATRIX4X2DVPROC) mygetprocaddr(env,"glUniformMatrix4x2dv");
       if(!impl)
            return;
    }
    jdouble* _value_ = (value_ ? ((*env)->GetDoubleArrayElements(env,value_,NULL) ) : NULL);
    impl(location_, count_, transpose_, _value_);
    if(value_)
        (*env)->ReleaseDoubleArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glUniformMatrix4x3dv (GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniformMatrix4x3dv( JNIEnv* env, jclass klass,jint location_,jint count_,jboolean transpose_,jdoubleArray value_){
    if(do_trace)
        fprintf(stderr,"glUniformMatrix4x3dv\n");
    static PFNGLUNIFORMMATRIX4X3DVPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORMMATRIX4X3DVPROC) mygetprocaddr(env,"glUniformMatrix4x3dv");
       if(!impl)
            return;
    }
    jdouble* _value_ = (value_ ? ((*env)->GetDoubleArrayElements(env,value_,NULL) ) : NULL);
    impl(location_, count_, transpose_, _value_);
    if(value_)
        (*env)->ReleaseDoubleArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glGetUniformdv (GLuint program, GLint location, GLdouble *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetUniformdv( JNIEnv* env, jclass klass,jint program_,jint location_,jdoubleArray params_){
    if(do_trace)
        fprintf(stderr,"glGetUniformdv\n");
    static PFNGLGETUNIFORMDVPROC impl;
    if( !impl ){
       impl = (PFNGLGETUNIFORMDVPROC) mygetprocaddr(env,"glGetUniformdv");
       if(!impl)
            return;
    }
    jdouble* _params_ = (params_ ? ((*env)->GetDoubleArrayElements(env,params_,NULL) ) : NULL);
    impl(program_, location_, _params_);
    if(params_)
        (*env)->ReleaseDoubleArrayElements(env,params_, _params_,0);
}
//GLAPI GLint APIENTRY glGetSubroutineUniformLocation (GLuint program, GLenum shadertype, const GLchar *name);
JNIEXPORT jint JNICALL Java_JGL_JGL_glGetSubroutineUniformLocation( JNIEnv* env, jclass klass,jint program_,jint shadertype_,jbyteArray name_){
    jint rv = 0;
    if(do_trace)
        fprintf(stderr,"glGetSubroutineUniformLocation\n");
    static PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC impl;
    if( !impl ){
       impl = (PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC) mygetprocaddr(env,"glGetSubroutineUniformLocation");
       if(!impl)
            return rv;
    }
    jbyte* _name_ = (name_ ? ((*env)->GetByteArrayElements(env,name_,NULL) ) : NULL);
    rv = impl(program_, shadertype_, _name_);
    if(name_)
        (*env)->ReleaseByteArrayElements(env,name_, _name_,0);
    return rv;
}
//GLAPI GLuint APIENTRY glGetSubroutineIndex (GLuint program, GLenum shadertype, const GLchar *name);
JNIEXPORT jint JNICALL Java_JGL_JGL_glGetSubroutineIndex( JNIEnv* env, jclass klass,jint program_,jint shadertype_,jbyteArray name_){
    jint rv = 0;
    if(do_trace)
        fprintf(stderr,"glGetSubroutineIndex\n");
    static PFNGLGETSUBROUTINEINDEXPROC impl;
    if( !impl ){
       impl = (PFNGLGETSUBROUTINEINDEXPROC) mygetprocaddr(env,"glGetSubroutineIndex");
       if(!impl)
            return rv;
    }
    jbyte* _name_ = (name_ ? ((*env)->GetByteArrayElements(env,name_,NULL) ) : NULL);
    rv = impl(program_, shadertype_, _name_);
    if(name_)
        (*env)->ReleaseByteArrayElements(env,name_, _name_,0);
    return rv;
}
//GLAPI void APIENTRY glGetActiveSubroutineUniformiv (GLuint program, GLenum shadertype, GLuint index, GLenum pname, GLint *values);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetActiveSubroutineUniformiv( JNIEnv* env, jclass klass,jint program_,jint shadertype_,jint index_,jint pname_,jintArray values_){
    if(do_trace)
        fprintf(stderr,"glGetActiveSubroutineUniformiv\n");
    static PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC) mygetprocaddr(env,"glGetActiveSubroutineUniformiv");
       if(!impl)
            return;
    }
    jint* _values_ = (values_ ? ((*env)->GetIntArrayElements(env,values_,NULL) ) : NULL);
    impl(program_, shadertype_, index_, pname_, _values_);
    if(values_)
        (*env)->ReleaseIntArrayElements(env,values_, _values_,0);
}
//GLAPI void APIENTRY glGetActiveSubroutineUniformName (GLuint program, GLenum shadertype, GLuint index, GLsizei bufsize, GLsizei *length, GLchar *name);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetActiveSubroutineUniformName( JNIEnv* env, jclass klass,jint program_,jint shadertype_,jint index_,jint bufsize_,jintArray length_,jbyteArray name_){
    if(do_trace)
        fprintf(stderr,"glGetActiveSubroutineUniformName\n");
    static PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC impl;
    if( !impl ){
       impl = (PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC) mygetprocaddr(env,"glGetActiveSubroutineUniformName");
       if(!impl)
            return;
    }
    jint* _length_ = (length_ ? ((*env)->GetIntArrayElements(env,length_,NULL) ) : NULL);
    jbyte* _name_ = (name_ ? ((*env)->GetByteArrayElements(env,name_,NULL) ) : NULL);
    impl(program_, shadertype_, index_, bufsize_, _length_, _name_);
    if(length_)
        (*env)->ReleaseIntArrayElements(env,length_, _length_,0);
    if(name_)
        (*env)->ReleaseByteArrayElements(env,name_, _name_,0);
}
//GLAPI void APIENTRY glGetActiveSubroutineName (GLuint program, GLenum shadertype, GLuint index, GLsizei bufsize, GLsizei *length, GLchar *name);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetActiveSubroutineName( JNIEnv* env, jclass klass,jint program_,jint shadertype_,jint index_,jint bufsize_,jintArray length_,jbyteArray name_){
    if(do_trace)
        fprintf(stderr,"glGetActiveSubroutineName\n");
    static PFNGLGETACTIVESUBROUTINENAMEPROC impl;
    if( !impl ){
       impl = (PFNGLGETACTIVESUBROUTINENAMEPROC) mygetprocaddr(env,"glGetActiveSubroutineName");
       if(!impl)
            return;
    }
    jint* _length_ = (length_ ? ((*env)->GetIntArrayElements(env,length_,NULL) ) : NULL);
    jbyte* _name_ = (name_ ? ((*env)->GetByteArrayElements(env,name_,NULL) ) : NULL);
    impl(program_, shadertype_, index_, bufsize_, _length_, _name_);
    if(length_)
        (*env)->ReleaseIntArrayElements(env,length_, _length_,0);
    if(name_)
        (*env)->ReleaseByteArrayElements(env,name_, _name_,0);
}
//GLAPI void APIENTRY glUniformSubroutinesuiv (GLenum shadertype, GLsizei count, const GLuint *indices);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniformSubroutinesuiv( JNIEnv* env, jclass klass,jint shadertype_,jint count_,jintArray indices_){
    if(do_trace)
        fprintf(stderr,"glUniformSubroutinesuiv\n");
    static PFNGLUNIFORMSUBROUTINESUIVPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORMSUBROUTINESUIVPROC) mygetprocaddr(env,"glUniformSubroutinesuiv");
       if(!impl)
            return;
    }
    jint* _indices_ = (indices_ ? ((*env)->GetIntArrayElements(env,indices_,NULL) ) : NULL);
    impl(shadertype_, count_, _indices_);
    if(indices_)
        (*env)->ReleaseIntArrayElements(env,indices_, _indices_,0);
}
//GLAPI void APIENTRY glGetUniformSubroutineuiv (GLenum shadertype, GLint location, GLuint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetUniformSubroutineuiv( JNIEnv* env, jclass klass,jint shadertype_,jint location_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetUniformSubroutineuiv\n");
    static PFNGLGETUNIFORMSUBROUTINEUIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETUNIFORMSUBROUTINEUIVPROC) mygetprocaddr(env,"glGetUniformSubroutineuiv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(shadertype_, location_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glGetProgramStageiv (GLuint program, GLenum shadertype, GLenum pname, GLint *values);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetProgramStageiv( JNIEnv* env, jclass klass,jint program_,jint shadertype_,jint pname_,jintArray values_){
    if(do_trace)
        fprintf(stderr,"glGetProgramStageiv\n");
    static PFNGLGETPROGRAMSTAGEIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETPROGRAMSTAGEIVPROC) mygetprocaddr(env,"glGetProgramStageiv");
       if(!impl)
            return;
    }
    jint* _values_ = (values_ ? ((*env)->GetIntArrayElements(env,values_,NULL) ) : NULL);
    impl(program_, shadertype_, pname_, _values_);
    if(values_)
        (*env)->ReleaseIntArrayElements(env,values_, _values_,0);
}
//GLAPI void APIENTRY glPatchParameteri (GLenum pname, GLint value);
JNIEXPORT void JNICALL Java_JGL_JGL_glPatchParameteri( JNIEnv* env, jclass klass,jint pname_,jint value_){
    if(do_trace)
        fprintf(stderr,"glPatchParameteri\n");
    static PFNGLPATCHPARAMETERIPROC impl;
    if( !impl ){
       impl = (PFNGLPATCHPARAMETERIPROC) mygetprocaddr(env,"glPatchParameteri");
       if(!impl)
            return;
    }
    impl(pname_, value_);
}
//GLAPI void APIENTRY glPatchParameterfv (GLenum pname, const GLfloat *values);
JNIEXPORT void JNICALL Java_JGL_JGL_glPatchParameterfv( JNIEnv* env, jclass klass,jint pname_,jfloatArray values_){
    if(do_trace)
        fprintf(stderr,"glPatchParameterfv\n");
    static PFNGLPATCHPARAMETERFVPROC impl;
    if( !impl ){
       impl = (PFNGLPATCHPARAMETERFVPROC) mygetprocaddr(env,"glPatchParameterfv");
       if(!impl)
            return;
    }
    jfloat* _values_ = (values_ ? ((*env)->GetFloatArrayElements(env,values_,NULL) ) : NULL);
    impl(pname_, _values_);
    if(values_)
        (*env)->ReleaseFloatArrayElements(env,values_, _values_,0);
}
//GLAPI void APIENTRY glBindTransformFeedback (GLenum target, GLuint id);
JNIEXPORT void JNICALL Java_JGL_JGL_glBindTransformFeedback( JNIEnv* env, jclass klass,jint target_,jint id_){
    if(do_trace)
        fprintf(stderr,"glBindTransformFeedback\n");
    static PFNGLBINDTRANSFORMFEEDBACKPROC impl;
    if( !impl ){
       impl = (PFNGLBINDTRANSFORMFEEDBACKPROC) mygetprocaddr(env,"glBindTransformFeedback");
       if(!impl)
            return;
    }
    impl(target_, id_);
}
//GLAPI void APIENTRY glDeleteTransformFeedbacks (GLsizei n, const GLuint *ids);
JNIEXPORT void JNICALL Java_JGL_JGL_glDeleteTransformFeedbacks( JNIEnv* env, jclass klass,jint n_,jintArray ids_){
    if(do_trace)
        fprintf(stderr,"glDeleteTransformFeedbacks\n");
    static PFNGLDELETETRANSFORMFEEDBACKSPROC impl;
    if( !impl ){
       impl = (PFNGLDELETETRANSFORMFEEDBACKSPROC) mygetprocaddr(env,"glDeleteTransformFeedbacks");
       if(!impl)
            return;
    }
    jint* _ids_ = (ids_ ? ((*env)->GetIntArrayElements(env,ids_,NULL) ) : NULL);
    impl(n_, _ids_);
    if(ids_)
        (*env)->ReleaseIntArrayElements(env,ids_, _ids_,0);
}
//GLAPI void APIENTRY glGenTransformFeedbacks (GLsizei n, GLuint *ids);
JNIEXPORT void JNICALL Java_JGL_JGL_glGenTransformFeedbacks( JNIEnv* env, jclass klass,jint n_,jintArray ids_){
    if(do_trace)
        fprintf(stderr,"glGenTransformFeedbacks\n");
    static PFNGLGENTRANSFORMFEEDBACKSPROC impl;
    if( !impl ){
       impl = (PFNGLGENTRANSFORMFEEDBACKSPROC) mygetprocaddr(env,"glGenTransformFeedbacks");
       if(!impl)
            return;
    }
    jint* _ids_ = (ids_ ? ((*env)->GetIntArrayElements(env,ids_,NULL) ) : NULL);
    impl(n_, _ids_);
    if(ids_)
        (*env)->ReleaseIntArrayElements(env,ids_, _ids_,0);
}
//GLAPI GLboolean APIENTRY glIsTransformFeedback (GLuint id);
JNIEXPORT jboolean JNICALL Java_JGL_JGL_glIsTransformFeedback( JNIEnv* env, jclass klass,jint id_){
    jboolean rv = 0;
    if(do_trace)
        fprintf(stderr,"glIsTransformFeedback\n");
    static PFNGLISTRANSFORMFEEDBACKPROC impl;
    if( !impl ){
       impl = (PFNGLISTRANSFORMFEEDBACKPROC) mygetprocaddr(env,"glIsTransformFeedback");
       if(!impl)
            return rv;
    }
    rv = impl(id_);
    return rv;
}
//GLAPI void APIENTRY glPauseTransformFeedback (void);
JNIEXPORT void JNICALL Java_JGL_JGL_glPauseTransformFeedback( JNIEnv* env, jclass klass){
    if(do_trace)
        fprintf(stderr,"glPauseTransformFeedback\n");
    static PFNGLPAUSETRANSFORMFEEDBACKPROC impl;
    if( !impl ){
       impl = (PFNGLPAUSETRANSFORMFEEDBACKPROC) mygetprocaddr(env,"glPauseTransformFeedback");
       if(!impl)
            return;
    }
    impl();
}
//GLAPI void APIENTRY glResumeTransformFeedback (void);
JNIEXPORT void JNICALL Java_JGL_JGL_glResumeTransformFeedback( JNIEnv* env, jclass klass){
    if(do_trace)
        fprintf(stderr,"glResumeTransformFeedback\n");
    static PFNGLRESUMETRANSFORMFEEDBACKPROC impl;
    if( !impl ){
       impl = (PFNGLRESUMETRANSFORMFEEDBACKPROC) mygetprocaddr(env,"glResumeTransformFeedback");
       if(!impl)
            return;
    }
    impl();
}
//GLAPI void APIENTRY glDrawTransformFeedback (GLenum mode, GLuint id);
JNIEXPORT void JNICALL Java_JGL_JGL_glDrawTransformFeedback( JNIEnv* env, jclass klass,jint mode_,jint id_){
    if(do_trace)
        fprintf(stderr,"glDrawTransformFeedback\n");
    static PFNGLDRAWTRANSFORMFEEDBACKPROC impl;
    if( !impl ){
       impl = (PFNGLDRAWTRANSFORMFEEDBACKPROC) mygetprocaddr(env,"glDrawTransformFeedback");
       if(!impl)
            return;
    }
    impl(mode_, id_);
}
//GLAPI void APIENTRY glDrawTransformFeedbackStream (GLenum mode, GLuint id, GLuint stream);
JNIEXPORT void JNICALL Java_JGL_JGL_glDrawTransformFeedbackStream( JNIEnv* env, jclass klass,jint mode_,jint id_,jint stream_){
    if(do_trace)
        fprintf(stderr,"glDrawTransformFeedbackStream\n");
    static PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC impl;
    if( !impl ){
       impl = (PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC) mygetprocaddr(env,"glDrawTransformFeedbackStream");
       if(!impl)
            return;
    }
    impl(mode_, id_, stream_);
}
//GLAPI void APIENTRY glBeginQueryIndexed (GLenum target, GLuint index, GLuint id);
JNIEXPORT void JNICALL Java_JGL_JGL_glBeginQueryIndexed( JNIEnv* env, jclass klass,jint target_,jint index_,jint id_){
    if(do_trace)
        fprintf(stderr,"glBeginQueryIndexed\n");
    static PFNGLBEGINQUERYINDEXEDPROC impl;
    if( !impl ){
       impl = (PFNGLBEGINQUERYINDEXEDPROC) mygetprocaddr(env,"glBeginQueryIndexed");
       if(!impl)
            return;
    }
    impl(target_, index_, id_);
}
//GLAPI void APIENTRY glEndQueryIndexed (GLenum target, GLuint index);
JNIEXPORT void JNICALL Java_JGL_JGL_glEndQueryIndexed( JNIEnv* env, jclass klass,jint target_,jint index_){
    if(do_trace)
        fprintf(stderr,"glEndQueryIndexed\n");
    static PFNGLENDQUERYINDEXEDPROC impl;
    if( !impl ){
       impl = (PFNGLENDQUERYINDEXEDPROC) mygetprocaddr(env,"glEndQueryIndexed");
       if(!impl)
            return;
    }
    impl(target_, index_);
}
//GLAPI void APIENTRY glGetQueryIndexediv (GLenum target, GLuint index, GLenum pname, GLint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetQueryIndexediv( JNIEnv* env, jclass klass,jint target_,jint index_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetQueryIndexediv\n");
    static PFNGLGETQUERYINDEXEDIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETQUERYINDEXEDIVPROC) mygetprocaddr(env,"glGetQueryIndexediv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(target_, index_, pname_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glReleaseShaderCompiler (void);
JNIEXPORT void JNICALL Java_JGL_JGL_glReleaseShaderCompiler( JNIEnv* env, jclass klass){
    if(do_trace)
        fprintf(stderr,"glReleaseShaderCompiler\n");
    static PFNGLRELEASESHADERCOMPILERPROC impl;
    if( !impl ){
       impl = (PFNGLRELEASESHADERCOMPILERPROC) mygetprocaddr(env,"glReleaseShaderCompiler");
       if(!impl)
            return;
    }
    impl();
}
//GLAPI void APIENTRY glShaderBinary (GLsizei count, const GLuint *shaders, GLenum binaryformat, const void *binary, GLsizei length);
JNIEXPORT void JNICALL Java_JGL_JGL_glShaderBinary( JNIEnv* env, jclass klass,jint count_,jintArray shaders_,jint binaryformat_,jbyteArray binary_,jint length_){
    if(do_trace)
        fprintf(stderr,"glShaderBinary\n");
    static PFNGLSHADERBINARYPROC impl;
    if( !impl ){
       impl = (PFNGLSHADERBINARYPROC) mygetprocaddr(env,"glShaderBinary");
       if(!impl)
            return;
    }
    jint* _shaders_ = (shaders_ ? ((*env)->GetIntArrayElements(env,shaders_,NULL) ) : NULL);
    jbyte* _binary_ = (binary_ ? ((*env)->GetByteArrayElements(env,binary_,NULL) ) : NULL);
    impl(count_, _shaders_, binaryformat_, _binary_, length_);
    if(shaders_)
        (*env)->ReleaseIntArrayElements(env,shaders_, _shaders_,0);
    if(binary_)
        (*env)->ReleaseByteArrayElements(env,binary_, _binary_,0);
}
//GLAPI void APIENTRY glGetShaderPrecisionFormat (GLenum shadertype, GLenum precisiontype, GLint *range, GLint *precision);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetShaderPrecisionFormat( JNIEnv* env, jclass klass,jint shadertype_,jint precisiontype_,jintArray range_,jintArray precision_){
    if(do_trace)
        fprintf(stderr,"glGetShaderPrecisionFormat\n");
    static PFNGLGETSHADERPRECISIONFORMATPROC impl;
    if( !impl ){
       impl = (PFNGLGETSHADERPRECISIONFORMATPROC) mygetprocaddr(env,"glGetShaderPrecisionFormat");
       if(!impl)
            return;
    }
    jint* _range_ = (range_ ? ((*env)->GetIntArrayElements(env,range_,NULL) ) : NULL);
    jint* _precision_ = (precision_ ? ((*env)->GetIntArrayElements(env,precision_,NULL) ) : NULL);
    impl(shadertype_, precisiontype_, _range_, _precision_);
    if(range_)
        (*env)->ReleaseIntArrayElements(env,range_, _range_,0);
    if(precision_)
        (*env)->ReleaseIntArrayElements(env,precision_, _precision_,0);
}
//GLAPI void APIENTRY glDepthRangef (GLfloat n, GLfloat f);
JNIEXPORT void JNICALL Java_JGL_JGL_glDepthRangef( JNIEnv* env, jclass klass,jfloat n_,jfloat f_){
    if(do_trace)
        fprintf(stderr,"glDepthRangef\n");
    static PFNGLDEPTHRANGEFPROC impl;
    if( !impl ){
       impl = (PFNGLDEPTHRANGEFPROC) mygetprocaddr(env,"glDepthRangef");
       if(!impl)
            return;
    }
    impl(n_, f_);
}
//GLAPI void APIENTRY glClearDepthf (GLfloat d);
JNIEXPORT void JNICALL Java_JGL_JGL_glClearDepthf( JNIEnv* env, jclass klass,jfloat d_){
    if(do_trace)
        fprintf(stderr,"glClearDepthf\n");
    static PFNGLCLEARDEPTHFPROC impl;
    if( !impl ){
       impl = (PFNGLCLEARDEPTHFPROC) mygetprocaddr(env,"glClearDepthf");
       if(!impl)
            return;
    }
    impl(d_);
}
//GLAPI void APIENTRY glGetProgramBinary (GLuint program, GLsizei bufSize, GLsizei *length, GLenum *binaryFormat, void *binary);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetProgramBinary( JNIEnv* env, jclass klass,jint program_,jint bufSize_,jintArray length_,jintArray binaryFormat_,jbyteArray binary_){
    if(do_trace)
        fprintf(stderr,"glGetProgramBinary\n");
    static PFNGLGETPROGRAMBINARYPROC impl;
    if( !impl ){
       impl = (PFNGLGETPROGRAMBINARYPROC) mygetprocaddr(env,"glGetProgramBinary");
       if(!impl)
            return;
    }
    jint* _length_ = (length_ ? ((*env)->GetIntArrayElements(env,length_,NULL) ) : NULL);
    jint* _binaryFormat_ = (binaryFormat_ ? ((*env)->GetIntArrayElements(env,binaryFormat_,NULL) ) : NULL);
    jbyte* _binary_ = (binary_ ? ((*env)->GetByteArrayElements(env,binary_,NULL) ) : NULL);
    impl(program_, bufSize_, _length_, _binaryFormat_, _binary_);
    if(length_)
        (*env)->ReleaseIntArrayElements(env,length_, _length_,0);
    if(binaryFormat_)
        (*env)->ReleaseIntArrayElements(env,binaryFormat_, _binaryFormat_,0);
    if(binary_)
        (*env)->ReleaseByteArrayElements(env,binary_, _binary_,0);
}
//GLAPI void APIENTRY glProgramBinary (GLuint program, GLenum binaryFormat, const void *binary, GLsizei length);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramBinary( JNIEnv* env, jclass klass,jint program_,jint binaryFormat_,jbyteArray binary_,jint length_){
    if(do_trace)
        fprintf(stderr,"glProgramBinary\n");
    static PFNGLPROGRAMBINARYPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMBINARYPROC) mygetprocaddr(env,"glProgramBinary");
       if(!impl)
            return;
    }
    jbyte* _binary_ = (binary_ ? ((*env)->GetByteArrayElements(env,binary_,NULL) ) : NULL);
    impl(program_, binaryFormat_, _binary_, length_);
    if(binary_)
        (*env)->ReleaseByteArrayElements(env,binary_, _binary_,0);
}
//GLAPI void APIENTRY glProgramParameteri (GLuint program, GLenum pname, GLint value);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramParameteri( JNIEnv* env, jclass klass,jint program_,jint pname_,jint value_){
    if(do_trace)
        fprintf(stderr,"glProgramParameteri\n");
    static PFNGLPROGRAMPARAMETERIPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMPARAMETERIPROC) mygetprocaddr(env,"glProgramParameteri");
       if(!impl)
            return;
    }
    impl(program_, pname_, value_);
}
//GLAPI void APIENTRY glUseProgramStages (GLuint pipeline, GLbitfield stages, GLuint program);
JNIEXPORT void JNICALL Java_JGL_JGL_glUseProgramStages( JNIEnv* env, jclass klass,jint pipeline_,jint stages_,jint program_){
    if(do_trace)
        fprintf(stderr,"glUseProgramStages\n");
    static PFNGLUSEPROGRAMSTAGESPROC impl;
    if( !impl ){
       impl = (PFNGLUSEPROGRAMSTAGESPROC) mygetprocaddr(env,"glUseProgramStages");
       if(!impl)
            return;
    }
    impl(pipeline_, stages_, program_);
}
//GLAPI void APIENTRY glActiveShaderProgram (GLuint pipeline, GLuint program);
JNIEXPORT void JNICALL Java_JGL_JGL_glActiveShaderProgram( JNIEnv* env, jclass klass,jint pipeline_,jint program_){
    if(do_trace)
        fprintf(stderr,"glActiveShaderProgram\n");
    static PFNGLACTIVESHADERPROGRAMPROC impl;
    if( !impl ){
       impl = (PFNGLACTIVESHADERPROGRAMPROC) mygetprocaddr(env,"glActiveShaderProgram");
       if(!impl)
            return;
    }
    impl(pipeline_, program_);
}
//GLAPI void APIENTRY glBindProgramPipeline (GLuint pipeline);
JNIEXPORT void JNICALL Java_JGL_JGL_glBindProgramPipeline( JNIEnv* env, jclass klass,jint pipeline_){
    if(do_trace)
        fprintf(stderr,"glBindProgramPipeline\n");
    static PFNGLBINDPROGRAMPIPELINEPROC impl;
    if( !impl ){
       impl = (PFNGLBINDPROGRAMPIPELINEPROC) mygetprocaddr(env,"glBindProgramPipeline");
       if(!impl)
            return;
    }
    impl(pipeline_);
}
//GLAPI void APIENTRY glDeleteProgramPipelines (GLsizei n, const GLuint *pipelines);
JNIEXPORT void JNICALL Java_JGL_JGL_glDeleteProgramPipelines( JNIEnv* env, jclass klass,jint n_,jintArray pipelines_){
    if(do_trace)
        fprintf(stderr,"glDeleteProgramPipelines\n");
    static PFNGLDELETEPROGRAMPIPELINESPROC impl;
    if( !impl ){
       impl = (PFNGLDELETEPROGRAMPIPELINESPROC) mygetprocaddr(env,"glDeleteProgramPipelines");
       if(!impl)
            return;
    }
    jint* _pipelines_ = (pipelines_ ? ((*env)->GetIntArrayElements(env,pipelines_,NULL) ) : NULL);
    impl(n_, _pipelines_);
    if(pipelines_)
        (*env)->ReleaseIntArrayElements(env,pipelines_, _pipelines_,0);
}
//GLAPI void APIENTRY glGenProgramPipelines (GLsizei n, GLuint *pipelines);
JNIEXPORT void JNICALL Java_JGL_JGL_glGenProgramPipelines( JNIEnv* env, jclass klass,jint n_,jintArray pipelines_){
    if(do_trace)
        fprintf(stderr,"glGenProgramPipelines\n");
    static PFNGLGENPROGRAMPIPELINESPROC impl;
    if( !impl ){
       impl = (PFNGLGENPROGRAMPIPELINESPROC) mygetprocaddr(env,"glGenProgramPipelines");
       if(!impl)
            return;
    }
    jint* _pipelines_ = (pipelines_ ? ((*env)->GetIntArrayElements(env,pipelines_,NULL) ) : NULL);
    impl(n_, _pipelines_);
    if(pipelines_)
        (*env)->ReleaseIntArrayElements(env,pipelines_, _pipelines_,0);
}
//GLAPI GLboolean APIENTRY glIsProgramPipeline (GLuint pipeline);
JNIEXPORT jboolean JNICALL Java_JGL_JGL_glIsProgramPipeline( JNIEnv* env, jclass klass,jint pipeline_){
    jboolean rv = 0;
    if(do_trace)
        fprintf(stderr,"glIsProgramPipeline\n");
    static PFNGLISPROGRAMPIPELINEPROC impl;
    if( !impl ){
       impl = (PFNGLISPROGRAMPIPELINEPROC) mygetprocaddr(env,"glIsProgramPipeline");
       if(!impl)
            return rv;
    }
    rv = impl(pipeline_);
    return rv;
}
//GLAPI void APIENTRY glGetProgramPipelineiv (GLuint pipeline, GLenum pname, GLint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetProgramPipelineiv( JNIEnv* env, jclass klass,jint pipeline_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetProgramPipelineiv\n");
    static PFNGLGETPROGRAMPIPELINEIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETPROGRAMPIPELINEIVPROC) mygetprocaddr(env,"glGetProgramPipelineiv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(pipeline_, pname_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glProgramUniform1i (GLuint program, GLint location, GLint v0);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniform1i( JNIEnv* env, jclass klass,jint program_,jint location_,jint v0_){
    if(do_trace)
        fprintf(stderr,"glProgramUniform1i\n");
    static PFNGLPROGRAMUNIFORM1IPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORM1IPROC) mygetprocaddr(env,"glProgramUniform1i");
       if(!impl)
            return;
    }
    impl(program_, location_, v0_);
}
//GLAPI void APIENTRY glProgramUniform1iv (GLuint program, GLint location, GLsizei count, const GLint *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniform1iv( JNIEnv* env, jclass klass,jint program_,jint location_,jint count_,jintArray value_){
    if(do_trace)
        fprintf(stderr,"glProgramUniform1iv\n");
    static PFNGLPROGRAMUNIFORM1IVPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORM1IVPROC) mygetprocaddr(env,"glProgramUniform1iv");
       if(!impl)
            return;
    }
    jint* _value_ = (value_ ? ((*env)->GetIntArrayElements(env,value_,NULL) ) : NULL);
    impl(program_, location_, count_, _value_);
    if(value_)
        (*env)->ReleaseIntArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glProgramUniform1f (GLuint program, GLint location, GLfloat v0);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniform1f( JNIEnv* env, jclass klass,jint program_,jint location_,jfloat v0_){
    if(do_trace)
        fprintf(stderr,"glProgramUniform1f\n");
    static PFNGLPROGRAMUNIFORM1FPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORM1FPROC) mygetprocaddr(env,"glProgramUniform1f");
       if(!impl)
            return;
    }
    impl(program_, location_, v0_);
}
//GLAPI void APIENTRY glProgramUniform1fv (GLuint program, GLint location, GLsizei count, const GLfloat *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniform1fv( JNIEnv* env, jclass klass,jint program_,jint location_,jint count_,jfloatArray value_){
    if(do_trace)
        fprintf(stderr,"glProgramUniform1fv\n");
    static PFNGLPROGRAMUNIFORM1FVPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORM1FVPROC) mygetprocaddr(env,"glProgramUniform1fv");
       if(!impl)
            return;
    }
    jfloat* _value_ = (value_ ? ((*env)->GetFloatArrayElements(env,value_,NULL) ) : NULL);
    impl(program_, location_, count_, _value_);
    if(value_)
        (*env)->ReleaseFloatArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glProgramUniform1d (GLuint program, GLint location, GLdouble v0);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniform1d( JNIEnv* env, jclass klass,jint program_,jint location_,jdouble v0_){
    if(do_trace)
        fprintf(stderr,"glProgramUniform1d\n");
    static PFNGLPROGRAMUNIFORM1DPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORM1DPROC) mygetprocaddr(env,"glProgramUniform1d");
       if(!impl)
            return;
    }
    impl(program_, location_, v0_);
}
//GLAPI void APIENTRY glProgramUniform1dv (GLuint program, GLint location, GLsizei count, const GLdouble *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniform1dv( JNIEnv* env, jclass klass,jint program_,jint location_,jint count_,jdoubleArray value_){
    if(do_trace)
        fprintf(stderr,"glProgramUniform1dv\n");
    static PFNGLPROGRAMUNIFORM1DVPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORM1DVPROC) mygetprocaddr(env,"glProgramUniform1dv");
       if(!impl)
            return;
    }
    jdouble* _value_ = (value_ ? ((*env)->GetDoubleArrayElements(env,value_,NULL) ) : NULL);
    impl(program_, location_, count_, _value_);
    if(value_)
        (*env)->ReleaseDoubleArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glProgramUniform1ui (GLuint program, GLint location, GLuint v0);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniform1ui( JNIEnv* env, jclass klass,jint program_,jint location_,jint v0_){
    if(do_trace)
        fprintf(stderr,"glProgramUniform1ui\n");
    static PFNGLPROGRAMUNIFORM1UIPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORM1UIPROC) mygetprocaddr(env,"glProgramUniform1ui");
       if(!impl)
            return;
    }
    impl(program_, location_, v0_);
}
//GLAPI void APIENTRY glProgramUniform1uiv (GLuint program, GLint location, GLsizei count, const GLuint *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniform1uiv( JNIEnv* env, jclass klass,jint program_,jint location_,jint count_,jintArray value_){
    if(do_trace)
        fprintf(stderr,"glProgramUniform1uiv\n");
    static PFNGLPROGRAMUNIFORM1UIVPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORM1UIVPROC) mygetprocaddr(env,"glProgramUniform1uiv");
       if(!impl)
            return;
    }
    jint* _value_ = (value_ ? ((*env)->GetIntArrayElements(env,value_,NULL) ) : NULL);
    impl(program_, location_, count_, _value_);
    if(value_)
        (*env)->ReleaseIntArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glProgramUniform2i (GLuint program, GLint location, GLint v0, GLint v1);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniform2i( JNIEnv* env, jclass klass,jint program_,jint location_,jint v0_,jint v1_){
    if(do_trace)
        fprintf(stderr,"glProgramUniform2i\n");
    static PFNGLPROGRAMUNIFORM2IPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORM2IPROC) mygetprocaddr(env,"glProgramUniform2i");
       if(!impl)
            return;
    }
    impl(program_, location_, v0_, v1_);
}
//GLAPI void APIENTRY glProgramUniform2iv (GLuint program, GLint location, GLsizei count, const GLint *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniform2iv( JNIEnv* env, jclass klass,jint program_,jint location_,jint count_,jintArray value_){
    if(do_trace)
        fprintf(stderr,"glProgramUniform2iv\n");
    static PFNGLPROGRAMUNIFORM2IVPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORM2IVPROC) mygetprocaddr(env,"glProgramUniform2iv");
       if(!impl)
            return;
    }
    jint* _value_ = (value_ ? ((*env)->GetIntArrayElements(env,value_,NULL) ) : NULL);
    impl(program_, location_, count_, _value_);
    if(value_)
        (*env)->ReleaseIntArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glProgramUniform2f (GLuint program, GLint location, GLfloat v0, GLfloat v1);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniform2f( JNIEnv* env, jclass klass,jint program_,jint location_,jfloat v0_,jfloat v1_){
    if(do_trace)
        fprintf(stderr,"glProgramUniform2f\n");
    static PFNGLPROGRAMUNIFORM2FPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORM2FPROC) mygetprocaddr(env,"glProgramUniform2f");
       if(!impl)
            return;
    }
    impl(program_, location_, v0_, v1_);
}
//GLAPI void APIENTRY glProgramUniform2fv (GLuint program, GLint location, GLsizei count, const GLfloat *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniform2fv( JNIEnv* env, jclass klass,jint program_,jint location_,jint count_,jfloatArray value_){
    if(do_trace)
        fprintf(stderr,"glProgramUniform2fv\n");
    static PFNGLPROGRAMUNIFORM2FVPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORM2FVPROC) mygetprocaddr(env,"glProgramUniform2fv");
       if(!impl)
            return;
    }
    jfloat* _value_ = (value_ ? ((*env)->GetFloatArrayElements(env,value_,NULL) ) : NULL);
    impl(program_, location_, count_, _value_);
    if(value_)
        (*env)->ReleaseFloatArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glProgramUniform2d (GLuint program, GLint location, GLdouble v0, GLdouble v1);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniform2d( JNIEnv* env, jclass klass,jint program_,jint location_,jdouble v0_,jdouble v1_){
    if(do_trace)
        fprintf(stderr,"glProgramUniform2d\n");
    static PFNGLPROGRAMUNIFORM2DPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORM2DPROC) mygetprocaddr(env,"glProgramUniform2d");
       if(!impl)
            return;
    }
    impl(program_, location_, v0_, v1_);
}
//GLAPI void APIENTRY glProgramUniform2dv (GLuint program, GLint location, GLsizei count, const GLdouble *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniform2dv( JNIEnv* env, jclass klass,jint program_,jint location_,jint count_,jdoubleArray value_){
    if(do_trace)
        fprintf(stderr,"glProgramUniform2dv\n");
    static PFNGLPROGRAMUNIFORM2DVPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORM2DVPROC) mygetprocaddr(env,"glProgramUniform2dv");
       if(!impl)
            return;
    }
    jdouble* _value_ = (value_ ? ((*env)->GetDoubleArrayElements(env,value_,NULL) ) : NULL);
    impl(program_, location_, count_, _value_);
    if(value_)
        (*env)->ReleaseDoubleArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glProgramUniform2ui (GLuint program, GLint location, GLuint v0, GLuint v1);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniform2ui( JNIEnv* env, jclass klass,jint program_,jint location_,jint v0_,jint v1_){
    if(do_trace)
        fprintf(stderr,"glProgramUniform2ui\n");
    static PFNGLPROGRAMUNIFORM2UIPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORM2UIPROC) mygetprocaddr(env,"glProgramUniform2ui");
       if(!impl)
            return;
    }
    impl(program_, location_, v0_, v1_);
}
//GLAPI void APIENTRY glProgramUniform2uiv (GLuint program, GLint location, GLsizei count, const GLuint *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniform2uiv( JNIEnv* env, jclass klass,jint program_,jint location_,jint count_,jintArray value_){
    if(do_trace)
        fprintf(stderr,"glProgramUniform2uiv\n");
    static PFNGLPROGRAMUNIFORM2UIVPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORM2UIVPROC) mygetprocaddr(env,"glProgramUniform2uiv");
       if(!impl)
            return;
    }
    jint* _value_ = (value_ ? ((*env)->GetIntArrayElements(env,value_,NULL) ) : NULL);
    impl(program_, location_, count_, _value_);
    if(value_)
        (*env)->ReleaseIntArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glProgramUniform3i (GLuint program, GLint location, GLint v0, GLint v1, GLint v2);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniform3i( JNIEnv* env, jclass klass,jint program_,jint location_,jint v0_,jint v1_,jint v2_){
    if(do_trace)
        fprintf(stderr,"glProgramUniform3i\n");
    static PFNGLPROGRAMUNIFORM3IPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORM3IPROC) mygetprocaddr(env,"glProgramUniform3i");
       if(!impl)
            return;
    }
    impl(program_, location_, v0_, v1_, v2_);
}
//GLAPI void APIENTRY glProgramUniform3iv (GLuint program, GLint location, GLsizei count, const GLint *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniform3iv( JNIEnv* env, jclass klass,jint program_,jint location_,jint count_,jintArray value_){
    if(do_trace)
        fprintf(stderr,"glProgramUniform3iv\n");
    static PFNGLPROGRAMUNIFORM3IVPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORM3IVPROC) mygetprocaddr(env,"glProgramUniform3iv");
       if(!impl)
            return;
    }
    jint* _value_ = (value_ ? ((*env)->GetIntArrayElements(env,value_,NULL) ) : NULL);
    impl(program_, location_, count_, _value_);
    if(value_)
        (*env)->ReleaseIntArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glProgramUniform3f (GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniform3f( JNIEnv* env, jclass klass,jint program_,jint location_,jfloat v0_,jfloat v1_,jfloat v2_){
    if(do_trace)
        fprintf(stderr,"glProgramUniform3f\n");
    static PFNGLPROGRAMUNIFORM3FPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORM3FPROC) mygetprocaddr(env,"glProgramUniform3f");
       if(!impl)
            return;
    }
    impl(program_, location_, v0_, v1_, v2_);
}
//GLAPI void APIENTRY glProgramUniform3fv (GLuint program, GLint location, GLsizei count, const GLfloat *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniform3fv( JNIEnv* env, jclass klass,jint program_,jint location_,jint count_,jfloatArray value_){
    if(do_trace)
        fprintf(stderr,"glProgramUniform3fv\n");
    static PFNGLPROGRAMUNIFORM3FVPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORM3FVPROC) mygetprocaddr(env,"glProgramUniform3fv");
       if(!impl)
            return;
    }
    jfloat* _value_ = (value_ ? ((*env)->GetFloatArrayElements(env,value_,NULL) ) : NULL);
    impl(program_, location_, count_, _value_);
    if(value_)
        (*env)->ReleaseFloatArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glProgramUniform3d (GLuint program, GLint location, GLdouble v0, GLdouble v1, GLdouble v2);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniform3d( JNIEnv* env, jclass klass,jint program_,jint location_,jdouble v0_,jdouble v1_,jdouble v2_){
    if(do_trace)
        fprintf(stderr,"glProgramUniform3d\n");
    static PFNGLPROGRAMUNIFORM3DPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORM3DPROC) mygetprocaddr(env,"glProgramUniform3d");
       if(!impl)
            return;
    }
    impl(program_, location_, v0_, v1_, v2_);
}
//GLAPI void APIENTRY glProgramUniform3dv (GLuint program, GLint location, GLsizei count, const GLdouble *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniform3dv( JNIEnv* env, jclass klass,jint program_,jint location_,jint count_,jdoubleArray value_){
    if(do_trace)
        fprintf(stderr,"glProgramUniform3dv\n");
    static PFNGLPROGRAMUNIFORM3DVPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORM3DVPROC) mygetprocaddr(env,"glProgramUniform3dv");
       if(!impl)
            return;
    }
    jdouble* _value_ = (value_ ? ((*env)->GetDoubleArrayElements(env,value_,NULL) ) : NULL);
    impl(program_, location_, count_, _value_);
    if(value_)
        (*env)->ReleaseDoubleArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glProgramUniform3ui (GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniform3ui( JNIEnv* env, jclass klass,jint program_,jint location_,jint v0_,jint v1_,jint v2_){
    if(do_trace)
        fprintf(stderr,"glProgramUniform3ui\n");
    static PFNGLPROGRAMUNIFORM3UIPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORM3UIPROC) mygetprocaddr(env,"glProgramUniform3ui");
       if(!impl)
            return;
    }
    impl(program_, location_, v0_, v1_, v2_);
}
//GLAPI void APIENTRY glProgramUniform3uiv (GLuint program, GLint location, GLsizei count, const GLuint *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniform3uiv( JNIEnv* env, jclass klass,jint program_,jint location_,jint count_,jintArray value_){
    if(do_trace)
        fprintf(stderr,"glProgramUniform3uiv\n");
    static PFNGLPROGRAMUNIFORM3UIVPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORM3UIVPROC) mygetprocaddr(env,"glProgramUniform3uiv");
       if(!impl)
            return;
    }
    jint* _value_ = (value_ ? ((*env)->GetIntArrayElements(env,value_,NULL) ) : NULL);
    impl(program_, location_, count_, _value_);
    if(value_)
        (*env)->ReleaseIntArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glProgramUniform4i (GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniform4i( JNIEnv* env, jclass klass,jint program_,jint location_,jint v0_,jint v1_,jint v2_,jint v3_){
    if(do_trace)
        fprintf(stderr,"glProgramUniform4i\n");
    static PFNGLPROGRAMUNIFORM4IPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORM4IPROC) mygetprocaddr(env,"glProgramUniform4i");
       if(!impl)
            return;
    }
    impl(program_, location_, v0_, v1_, v2_, v3_);
}
//GLAPI void APIENTRY glProgramUniform4iv (GLuint program, GLint location, GLsizei count, const GLint *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniform4iv( JNIEnv* env, jclass klass,jint program_,jint location_,jint count_,jintArray value_){
    if(do_trace)
        fprintf(stderr,"glProgramUniform4iv\n");
    static PFNGLPROGRAMUNIFORM4IVPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORM4IVPROC) mygetprocaddr(env,"glProgramUniform4iv");
       if(!impl)
            return;
    }
    jint* _value_ = (value_ ? ((*env)->GetIntArrayElements(env,value_,NULL) ) : NULL);
    impl(program_, location_, count_, _value_);
    if(value_)
        (*env)->ReleaseIntArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glProgramUniform4f (GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniform4f( JNIEnv* env, jclass klass,jint program_,jint location_,jfloat v0_,jfloat v1_,jfloat v2_,jfloat v3_){
    if(do_trace)
        fprintf(stderr,"glProgramUniform4f\n");
    static PFNGLPROGRAMUNIFORM4FPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORM4FPROC) mygetprocaddr(env,"glProgramUniform4f");
       if(!impl)
            return;
    }
    impl(program_, location_, v0_, v1_, v2_, v3_);
}
//GLAPI void APIENTRY glProgramUniform4fv (GLuint program, GLint location, GLsizei count, const GLfloat *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniform4fv( JNIEnv* env, jclass klass,jint program_,jint location_,jint count_,jfloatArray value_){
    if(do_trace)
        fprintf(stderr,"glProgramUniform4fv\n");
    static PFNGLPROGRAMUNIFORM4FVPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORM4FVPROC) mygetprocaddr(env,"glProgramUniform4fv");
       if(!impl)
            return;
    }
    jfloat* _value_ = (value_ ? ((*env)->GetFloatArrayElements(env,value_,NULL) ) : NULL);
    impl(program_, location_, count_, _value_);
    if(value_)
        (*env)->ReleaseFloatArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glProgramUniform4d (GLuint program, GLint location, GLdouble v0, GLdouble v1, GLdouble v2, GLdouble v3);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniform4d( JNIEnv* env, jclass klass,jint program_,jint location_,jdouble v0_,jdouble v1_,jdouble v2_,jdouble v3_){
    if(do_trace)
        fprintf(stderr,"glProgramUniform4d\n");
    static PFNGLPROGRAMUNIFORM4DPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORM4DPROC) mygetprocaddr(env,"glProgramUniform4d");
       if(!impl)
            return;
    }
    impl(program_, location_, v0_, v1_, v2_, v3_);
}
//GLAPI void APIENTRY glProgramUniform4dv (GLuint program, GLint location, GLsizei count, const GLdouble *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniform4dv( JNIEnv* env, jclass klass,jint program_,jint location_,jint count_,jdoubleArray value_){
    if(do_trace)
        fprintf(stderr,"glProgramUniform4dv\n");
    static PFNGLPROGRAMUNIFORM4DVPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORM4DVPROC) mygetprocaddr(env,"glProgramUniform4dv");
       if(!impl)
            return;
    }
    jdouble* _value_ = (value_ ? ((*env)->GetDoubleArrayElements(env,value_,NULL) ) : NULL);
    impl(program_, location_, count_, _value_);
    if(value_)
        (*env)->ReleaseDoubleArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glProgramUniform4ui (GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniform4ui( JNIEnv* env, jclass klass,jint program_,jint location_,jint v0_,jint v1_,jint v2_,jint v3_){
    if(do_trace)
        fprintf(stderr,"glProgramUniform4ui\n");
    static PFNGLPROGRAMUNIFORM4UIPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORM4UIPROC) mygetprocaddr(env,"glProgramUniform4ui");
       if(!impl)
            return;
    }
    impl(program_, location_, v0_, v1_, v2_, v3_);
}
//GLAPI void APIENTRY glProgramUniform4uiv (GLuint program, GLint location, GLsizei count, const GLuint *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniform4uiv( JNIEnv* env, jclass klass,jint program_,jint location_,jint count_,jintArray value_){
    if(do_trace)
        fprintf(stderr,"glProgramUniform4uiv\n");
    static PFNGLPROGRAMUNIFORM4UIVPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORM4UIVPROC) mygetprocaddr(env,"glProgramUniform4uiv");
       if(!impl)
            return;
    }
    jint* _value_ = (value_ ? ((*env)->GetIntArrayElements(env,value_,NULL) ) : NULL);
    impl(program_, location_, count_, _value_);
    if(value_)
        (*env)->ReleaseIntArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glProgramUniformMatrix2fv (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniformMatrix2fv( JNIEnv* env, jclass klass,jint program_,jint location_,jint count_,jboolean transpose_,jfloatArray value_){
    if(do_trace)
        fprintf(stderr,"glProgramUniformMatrix2fv\n");
    static PFNGLPROGRAMUNIFORMMATRIX2FVPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORMMATRIX2FVPROC) mygetprocaddr(env,"glProgramUniformMatrix2fv");
       if(!impl)
            return;
    }
    jfloat* _value_ = (value_ ? ((*env)->GetFloatArrayElements(env,value_,NULL) ) : NULL);
    impl(program_, location_, count_, transpose_, _value_);
    if(value_)
        (*env)->ReleaseFloatArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glProgramUniformMatrix3fv (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniformMatrix3fv( JNIEnv* env, jclass klass,jint program_,jint location_,jint count_,jboolean transpose_,jfloatArray value_){
    if(do_trace)
        fprintf(stderr,"glProgramUniformMatrix3fv\n");
    static PFNGLPROGRAMUNIFORMMATRIX3FVPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORMMATRIX3FVPROC) mygetprocaddr(env,"glProgramUniformMatrix3fv");
       if(!impl)
            return;
    }
    jfloat* _value_ = (value_ ? ((*env)->GetFloatArrayElements(env,value_,NULL) ) : NULL);
    impl(program_, location_, count_, transpose_, _value_);
    if(value_)
        (*env)->ReleaseFloatArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glProgramUniformMatrix4fv (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniformMatrix4fv( JNIEnv* env, jclass klass,jint program_,jint location_,jint count_,jboolean transpose_,jfloatArray value_){
    if(do_trace)
        fprintf(stderr,"glProgramUniformMatrix4fv\n");
    static PFNGLPROGRAMUNIFORMMATRIX4FVPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORMMATRIX4FVPROC) mygetprocaddr(env,"glProgramUniformMatrix4fv");
       if(!impl)
            return;
    }
    jfloat* _value_ = (value_ ? ((*env)->GetFloatArrayElements(env,value_,NULL) ) : NULL);
    impl(program_, location_, count_, transpose_, _value_);
    if(value_)
        (*env)->ReleaseFloatArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glProgramUniformMatrix2dv (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniformMatrix2dv( JNIEnv* env, jclass klass,jint program_,jint location_,jint count_,jboolean transpose_,jdoubleArray value_){
    if(do_trace)
        fprintf(stderr,"glProgramUniformMatrix2dv\n");
    static PFNGLPROGRAMUNIFORMMATRIX2DVPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORMMATRIX2DVPROC) mygetprocaddr(env,"glProgramUniformMatrix2dv");
       if(!impl)
            return;
    }
    jdouble* _value_ = (value_ ? ((*env)->GetDoubleArrayElements(env,value_,NULL) ) : NULL);
    impl(program_, location_, count_, transpose_, _value_);
    if(value_)
        (*env)->ReleaseDoubleArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glProgramUniformMatrix3dv (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniformMatrix3dv( JNIEnv* env, jclass klass,jint program_,jint location_,jint count_,jboolean transpose_,jdoubleArray value_){
    if(do_trace)
        fprintf(stderr,"glProgramUniformMatrix3dv\n");
    static PFNGLPROGRAMUNIFORMMATRIX3DVPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORMMATRIX3DVPROC) mygetprocaddr(env,"glProgramUniformMatrix3dv");
       if(!impl)
            return;
    }
    jdouble* _value_ = (value_ ? ((*env)->GetDoubleArrayElements(env,value_,NULL) ) : NULL);
    impl(program_, location_, count_, transpose_, _value_);
    if(value_)
        (*env)->ReleaseDoubleArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glProgramUniformMatrix4dv (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniformMatrix4dv( JNIEnv* env, jclass klass,jint program_,jint location_,jint count_,jboolean transpose_,jdoubleArray value_){
    if(do_trace)
        fprintf(stderr,"glProgramUniformMatrix4dv\n");
    static PFNGLPROGRAMUNIFORMMATRIX4DVPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORMMATRIX4DVPROC) mygetprocaddr(env,"glProgramUniformMatrix4dv");
       if(!impl)
            return;
    }
    jdouble* _value_ = (value_ ? ((*env)->GetDoubleArrayElements(env,value_,NULL) ) : NULL);
    impl(program_, location_, count_, transpose_, _value_);
    if(value_)
        (*env)->ReleaseDoubleArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glProgramUniformMatrix2x3fv (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniformMatrix2x3fv( JNIEnv* env, jclass klass,jint program_,jint location_,jint count_,jboolean transpose_,jfloatArray value_){
    if(do_trace)
        fprintf(stderr,"glProgramUniformMatrix2x3fv\n");
    static PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC) mygetprocaddr(env,"glProgramUniformMatrix2x3fv");
       if(!impl)
            return;
    }
    jfloat* _value_ = (value_ ? ((*env)->GetFloatArrayElements(env,value_,NULL) ) : NULL);
    impl(program_, location_, count_, transpose_, _value_);
    if(value_)
        (*env)->ReleaseFloatArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glProgramUniformMatrix3x2fv (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniformMatrix3x2fv( JNIEnv* env, jclass klass,jint program_,jint location_,jint count_,jboolean transpose_,jfloatArray value_){
    if(do_trace)
        fprintf(stderr,"glProgramUniformMatrix3x2fv\n");
    static PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC) mygetprocaddr(env,"glProgramUniformMatrix3x2fv");
       if(!impl)
            return;
    }
    jfloat* _value_ = (value_ ? ((*env)->GetFloatArrayElements(env,value_,NULL) ) : NULL);
    impl(program_, location_, count_, transpose_, _value_);
    if(value_)
        (*env)->ReleaseFloatArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glProgramUniformMatrix2x4fv (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniformMatrix2x4fv( JNIEnv* env, jclass klass,jint program_,jint location_,jint count_,jboolean transpose_,jfloatArray value_){
    if(do_trace)
        fprintf(stderr,"glProgramUniformMatrix2x4fv\n");
    static PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC) mygetprocaddr(env,"glProgramUniformMatrix2x4fv");
       if(!impl)
            return;
    }
    jfloat* _value_ = (value_ ? ((*env)->GetFloatArrayElements(env,value_,NULL) ) : NULL);
    impl(program_, location_, count_, transpose_, _value_);
    if(value_)
        (*env)->ReleaseFloatArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glProgramUniformMatrix4x2fv (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniformMatrix4x2fv( JNIEnv* env, jclass klass,jint program_,jint location_,jint count_,jboolean transpose_,jfloatArray value_){
    if(do_trace)
        fprintf(stderr,"glProgramUniformMatrix4x2fv\n");
    static PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC) mygetprocaddr(env,"glProgramUniformMatrix4x2fv");
       if(!impl)
            return;
    }
    jfloat* _value_ = (value_ ? ((*env)->GetFloatArrayElements(env,value_,NULL) ) : NULL);
    impl(program_, location_, count_, transpose_, _value_);
    if(value_)
        (*env)->ReleaseFloatArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glProgramUniformMatrix3x4fv (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniformMatrix3x4fv( JNIEnv* env, jclass klass,jint program_,jint location_,jint count_,jboolean transpose_,jfloatArray value_){
    if(do_trace)
        fprintf(stderr,"glProgramUniformMatrix3x4fv\n");
    static PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC) mygetprocaddr(env,"glProgramUniformMatrix3x4fv");
       if(!impl)
            return;
    }
    jfloat* _value_ = (value_ ? ((*env)->GetFloatArrayElements(env,value_,NULL) ) : NULL);
    impl(program_, location_, count_, transpose_, _value_);
    if(value_)
        (*env)->ReleaseFloatArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glProgramUniformMatrix4x3fv (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniformMatrix4x3fv( JNIEnv* env, jclass klass,jint program_,jint location_,jint count_,jboolean transpose_,jfloatArray value_){
    if(do_trace)
        fprintf(stderr,"glProgramUniformMatrix4x3fv\n");
    static PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC) mygetprocaddr(env,"glProgramUniformMatrix4x3fv");
       if(!impl)
            return;
    }
    jfloat* _value_ = (value_ ? ((*env)->GetFloatArrayElements(env,value_,NULL) ) : NULL);
    impl(program_, location_, count_, transpose_, _value_);
    if(value_)
        (*env)->ReleaseFloatArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glProgramUniformMatrix2x3dv (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniformMatrix2x3dv( JNIEnv* env, jclass klass,jint program_,jint location_,jint count_,jboolean transpose_,jdoubleArray value_){
    if(do_trace)
        fprintf(stderr,"glProgramUniformMatrix2x3dv\n");
    static PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORMMATRIX2X3DVPROC) mygetprocaddr(env,"glProgramUniformMatrix2x3dv");
       if(!impl)
            return;
    }
    jdouble* _value_ = (value_ ? ((*env)->GetDoubleArrayElements(env,value_,NULL) ) : NULL);
    impl(program_, location_, count_, transpose_, _value_);
    if(value_)
        (*env)->ReleaseDoubleArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glProgramUniformMatrix3x2dv (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniformMatrix3x2dv( JNIEnv* env, jclass klass,jint program_,jint location_,jint count_,jboolean transpose_,jdoubleArray value_){
    if(do_trace)
        fprintf(stderr,"glProgramUniformMatrix3x2dv\n");
    static PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORMMATRIX3X2DVPROC) mygetprocaddr(env,"glProgramUniformMatrix3x2dv");
       if(!impl)
            return;
    }
    jdouble* _value_ = (value_ ? ((*env)->GetDoubleArrayElements(env,value_,NULL) ) : NULL);
    impl(program_, location_, count_, transpose_, _value_);
    if(value_)
        (*env)->ReleaseDoubleArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glProgramUniformMatrix2x4dv (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniformMatrix2x4dv( JNIEnv* env, jclass klass,jint program_,jint location_,jint count_,jboolean transpose_,jdoubleArray value_){
    if(do_trace)
        fprintf(stderr,"glProgramUniformMatrix2x4dv\n");
    static PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORMMATRIX2X4DVPROC) mygetprocaddr(env,"glProgramUniformMatrix2x4dv");
       if(!impl)
            return;
    }
    jdouble* _value_ = (value_ ? ((*env)->GetDoubleArrayElements(env,value_,NULL) ) : NULL);
    impl(program_, location_, count_, transpose_, _value_);
    if(value_)
        (*env)->ReleaseDoubleArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glProgramUniformMatrix4x2dv (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniformMatrix4x2dv( JNIEnv* env, jclass klass,jint program_,jint location_,jint count_,jboolean transpose_,jdoubleArray value_){
    if(do_trace)
        fprintf(stderr,"glProgramUniformMatrix4x2dv\n");
    static PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORMMATRIX4X2DVPROC) mygetprocaddr(env,"glProgramUniformMatrix4x2dv");
       if(!impl)
            return;
    }
    jdouble* _value_ = (value_ ? ((*env)->GetDoubleArrayElements(env,value_,NULL) ) : NULL);
    impl(program_, location_, count_, transpose_, _value_);
    if(value_)
        (*env)->ReleaseDoubleArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glProgramUniformMatrix3x4dv (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniformMatrix3x4dv( JNIEnv* env, jclass klass,jint program_,jint location_,jint count_,jboolean transpose_,jdoubleArray value_){
    if(do_trace)
        fprintf(stderr,"glProgramUniformMatrix3x4dv\n");
    static PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORMMATRIX3X4DVPROC) mygetprocaddr(env,"glProgramUniformMatrix3x4dv");
       if(!impl)
            return;
    }
    jdouble* _value_ = (value_ ? ((*env)->GetDoubleArrayElements(env,value_,NULL) ) : NULL);
    impl(program_, location_, count_, transpose_, _value_);
    if(value_)
        (*env)->ReleaseDoubleArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glProgramUniformMatrix4x3dv (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLdouble *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniformMatrix4x3dv( JNIEnv* env, jclass klass,jint program_,jint location_,jint count_,jboolean transpose_,jdoubleArray value_){
    if(do_trace)
        fprintf(stderr,"glProgramUniformMatrix4x3dv\n");
    static PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORMMATRIX4X3DVPROC) mygetprocaddr(env,"glProgramUniformMatrix4x3dv");
       if(!impl)
            return;
    }
    jdouble* _value_ = (value_ ? ((*env)->GetDoubleArrayElements(env,value_,NULL) ) : NULL);
    impl(program_, location_, count_, transpose_, _value_);
    if(value_)
        (*env)->ReleaseDoubleArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glValidateProgramPipeline (GLuint pipeline);
JNIEXPORT void JNICALL Java_JGL_JGL_glValidateProgramPipeline( JNIEnv* env, jclass klass,jint pipeline_){
    if(do_trace)
        fprintf(stderr,"glValidateProgramPipeline\n");
    static PFNGLVALIDATEPROGRAMPIPELINEPROC impl;
    if( !impl ){
       impl = (PFNGLVALIDATEPROGRAMPIPELINEPROC) mygetprocaddr(env,"glValidateProgramPipeline");
       if(!impl)
            return;
    }
    impl(pipeline_);
}
//GLAPI void APIENTRY glGetProgramPipelineInfoLog (GLuint pipeline, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetProgramPipelineInfoLog( JNIEnv* env, jclass klass,jint pipeline_,jint bufSize_,jintArray length_,jbyteArray infoLog_){
    if(do_trace)
        fprintf(stderr,"glGetProgramPipelineInfoLog\n");
    static PFNGLGETPROGRAMPIPELINEINFOLOGPROC impl;
    if( !impl ){
       impl = (PFNGLGETPROGRAMPIPELINEINFOLOGPROC) mygetprocaddr(env,"glGetProgramPipelineInfoLog");
       if(!impl)
            return;
    }
    jint* _length_ = (length_ ? ((*env)->GetIntArrayElements(env,length_,NULL) ) : NULL);
    jbyte* _infoLog_ = (infoLog_ ? ((*env)->GetByteArrayElements(env,infoLog_,NULL) ) : NULL);
    impl(pipeline_, bufSize_, _length_, _infoLog_);
    if(length_)
        (*env)->ReleaseIntArrayElements(env,length_, _length_,0);
    if(infoLog_)
        (*env)->ReleaseByteArrayElements(env,infoLog_, _infoLog_,0);
}
//GLAPI void APIENTRY glVertexAttribL1d (GLuint index, GLdouble x);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribL1d( JNIEnv* env, jclass klass,jint index_,jdouble x_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribL1d\n");
    static PFNGLVERTEXATTRIBL1DPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBL1DPROC) mygetprocaddr(env,"glVertexAttribL1d");
       if(!impl)
            return;
    }
    impl(index_, x_);
}
//GLAPI void APIENTRY glVertexAttribL2d (GLuint index, GLdouble x, GLdouble y);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribL2d( JNIEnv* env, jclass klass,jint index_,jdouble x_,jdouble y_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribL2d\n");
    static PFNGLVERTEXATTRIBL2DPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBL2DPROC) mygetprocaddr(env,"glVertexAttribL2d");
       if(!impl)
            return;
    }
    impl(index_, x_, y_);
}
//GLAPI void APIENTRY glVertexAttribL3d (GLuint index, GLdouble x, GLdouble y, GLdouble z);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribL3d( JNIEnv* env, jclass klass,jint index_,jdouble x_,jdouble y_,jdouble z_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribL3d\n");
    static PFNGLVERTEXATTRIBL3DPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBL3DPROC) mygetprocaddr(env,"glVertexAttribL3d");
       if(!impl)
            return;
    }
    impl(index_, x_, y_, z_);
}
//GLAPI void APIENTRY glVertexAttribL4d (GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribL4d( JNIEnv* env, jclass klass,jint index_,jdouble x_,jdouble y_,jdouble z_,jdouble w_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribL4d\n");
    static PFNGLVERTEXATTRIBL4DPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBL4DPROC) mygetprocaddr(env,"glVertexAttribL4d");
       if(!impl)
            return;
    }
    impl(index_, x_, y_, z_, w_);
}
//GLAPI void APIENTRY glVertexAttribL1dv (GLuint index, const GLdouble *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribL1dv( JNIEnv* env, jclass klass,jint index_,jdoubleArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribL1dv\n");
    static PFNGLVERTEXATTRIBL1DVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBL1DVPROC) mygetprocaddr(env,"glVertexAttribL1dv");
       if(!impl)
            return;
    }
    jdouble* _v_ = (v_ ? ((*env)->GetDoubleArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseDoubleArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttribL2dv (GLuint index, const GLdouble *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribL2dv( JNIEnv* env, jclass klass,jint index_,jdoubleArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribL2dv\n");
    static PFNGLVERTEXATTRIBL2DVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBL2DVPROC) mygetprocaddr(env,"glVertexAttribL2dv");
       if(!impl)
            return;
    }
    jdouble* _v_ = (v_ ? ((*env)->GetDoubleArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseDoubleArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttribL3dv (GLuint index, const GLdouble *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribL3dv( JNIEnv* env, jclass klass,jint index_,jdoubleArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribL3dv\n");
    static PFNGLVERTEXATTRIBL3DVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBL3DVPROC) mygetprocaddr(env,"glVertexAttribL3dv");
       if(!impl)
            return;
    }
    jdouble* _v_ = (v_ ? ((*env)->GetDoubleArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseDoubleArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttribL4dv (GLuint index, const GLdouble *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribL4dv( JNIEnv* env, jclass klass,jint index_,jdoubleArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribL4dv\n");
    static PFNGLVERTEXATTRIBL4DVPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBL4DVPROC) mygetprocaddr(env,"glVertexAttribL4dv");
       if(!impl)
            return;
    }
    jdouble* _v_ = (v_ ? ((*env)->GetDoubleArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseDoubleArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glVertexAttribLPointer (GLuint index, GLint size, GLenum type, GLsizei stride, const void *pointer);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribLPointer( JNIEnv* env, jclass klass,jint index_,jint size_,jint type_,jint stride_,jint pointer_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribLPointer\n");
    static PFNGLVERTEXATTRIBLPOINTERPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBLPOINTERPROC) mygetprocaddr(env,"glVertexAttribLPointer");
       if(!impl)
            return;
    }
    impl(index_, size_, type_, stride_, (const void *)pointer_);
}
//GLAPI void APIENTRY glGetVertexAttribLdv (GLuint index, GLenum pname, GLdouble *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetVertexAttribLdv( JNIEnv* env, jclass klass,jint index_,jint pname_,jdoubleArray params_){
    if(do_trace)
        fprintf(stderr,"glGetVertexAttribLdv\n");
    static PFNGLGETVERTEXATTRIBLDVPROC impl;
    if( !impl ){
       impl = (PFNGLGETVERTEXATTRIBLDVPROC) mygetprocaddr(env,"glGetVertexAttribLdv");
       if(!impl)
            return;
    }
    jdouble* _params_ = (params_ ? ((*env)->GetDoubleArrayElements(env,params_,NULL) ) : NULL);
    impl(index_, pname_, _params_);
    if(params_)
        (*env)->ReleaseDoubleArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glViewportArrayv (GLuint first, GLsizei count, const GLfloat *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glViewportArrayv( JNIEnv* env, jclass klass,jint first_,jint count_,jfloatArray v_){
    if(do_trace)
        fprintf(stderr,"glViewportArrayv\n");
    static PFNGLVIEWPORTARRAYVPROC impl;
    if( !impl ){
       impl = (PFNGLVIEWPORTARRAYVPROC) mygetprocaddr(env,"glViewportArrayv");
       if(!impl)
            return;
    }
    jfloat* _v_ = (v_ ? ((*env)->GetFloatArrayElements(env,v_,NULL) ) : NULL);
    impl(first_, count_, _v_);
    if(v_)
        (*env)->ReleaseFloatArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glViewportIndexedf (GLuint index, GLfloat x, GLfloat y, GLfloat w, GLfloat h);
JNIEXPORT void JNICALL Java_JGL_JGL_glViewportIndexedf( JNIEnv* env, jclass klass,jint index_,jfloat x_,jfloat y_,jfloat w_,jfloat h_){
    if(do_trace)
        fprintf(stderr,"glViewportIndexedf\n");
    static PFNGLVIEWPORTINDEXEDFPROC impl;
    if( !impl ){
       impl = (PFNGLVIEWPORTINDEXEDFPROC) mygetprocaddr(env,"glViewportIndexedf");
       if(!impl)
            return;
    }
    impl(index_, x_, y_, w_, h_);
}
//GLAPI void APIENTRY glViewportIndexedfv (GLuint index, const GLfloat *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glViewportIndexedfv( JNIEnv* env, jclass klass,jint index_,jfloatArray v_){
    if(do_trace)
        fprintf(stderr,"glViewportIndexedfv\n");
    static PFNGLVIEWPORTINDEXEDFVPROC impl;
    if( !impl ){
       impl = (PFNGLVIEWPORTINDEXEDFVPROC) mygetprocaddr(env,"glViewportIndexedfv");
       if(!impl)
            return;
    }
    jfloat* _v_ = (v_ ? ((*env)->GetFloatArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseFloatArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glScissorArrayv (GLuint first, GLsizei count, const GLint *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glScissorArrayv( JNIEnv* env, jclass klass,jint first_,jint count_,jintArray v_){
    if(do_trace)
        fprintf(stderr,"glScissorArrayv\n");
    static PFNGLSCISSORARRAYVPROC impl;
    if( !impl ){
       impl = (PFNGLSCISSORARRAYVPROC) mygetprocaddr(env,"glScissorArrayv");
       if(!impl)
            return;
    }
    jint* _v_ = (v_ ? ((*env)->GetIntArrayElements(env,v_,NULL) ) : NULL);
    impl(first_, count_, _v_);
    if(v_)
        (*env)->ReleaseIntArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glScissorIndexed (GLuint index, GLint left, GLint bottom, GLsizei width, GLsizei height);
JNIEXPORT void JNICALL Java_JGL_JGL_glScissorIndexed( JNIEnv* env, jclass klass,jint index_,jint left_,jint bottom_,jint width_,jint height_){
    if(do_trace)
        fprintf(stderr,"glScissorIndexed\n");
    static PFNGLSCISSORINDEXEDPROC impl;
    if( !impl ){
       impl = (PFNGLSCISSORINDEXEDPROC) mygetprocaddr(env,"glScissorIndexed");
       if(!impl)
            return;
    }
    impl(index_, left_, bottom_, width_, height_);
}
//GLAPI void APIENTRY glScissorIndexedv (GLuint index, const GLint *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glScissorIndexedv( JNIEnv* env, jclass klass,jint index_,jintArray v_){
    if(do_trace)
        fprintf(stderr,"glScissorIndexedv\n");
    static PFNGLSCISSORINDEXEDVPROC impl;
    if( !impl ){
       impl = (PFNGLSCISSORINDEXEDVPROC) mygetprocaddr(env,"glScissorIndexedv");
       if(!impl)
            return;
    }
    jint* _v_ = (v_ ? ((*env)->GetIntArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseIntArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glDepthRangeArrayv (GLuint first, GLsizei count, const GLdouble *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glDepthRangeArrayv( JNIEnv* env, jclass klass,jint first_,jint count_,jdoubleArray v_){
    if(do_trace)
        fprintf(stderr,"glDepthRangeArrayv\n");
    static PFNGLDEPTHRANGEARRAYVPROC impl;
    if( !impl ){
       impl = (PFNGLDEPTHRANGEARRAYVPROC) mygetprocaddr(env,"glDepthRangeArrayv");
       if(!impl)
            return;
    }
    jdouble* _v_ = (v_ ? ((*env)->GetDoubleArrayElements(env,v_,NULL) ) : NULL);
    impl(first_, count_, _v_);
    if(v_)
        (*env)->ReleaseDoubleArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glDepthRangeIndexed (GLuint index, GLdouble n, GLdouble f);
JNIEXPORT void JNICALL Java_JGL_JGL_glDepthRangeIndexed( JNIEnv* env, jclass klass,jint index_,jdouble n_,jdouble f_){
    if(do_trace)
        fprintf(stderr,"glDepthRangeIndexed\n");
    static PFNGLDEPTHRANGEINDEXEDPROC impl;
    if( !impl ){
       impl = (PFNGLDEPTHRANGEINDEXEDPROC) mygetprocaddr(env,"glDepthRangeIndexed");
       if(!impl)
            return;
    }
    impl(index_, n_, f_);
}
//GLAPI void APIENTRY glGetFloati_v (GLenum target, GLuint index, GLfloat *data);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetFloati_v( JNIEnv* env, jclass klass,jint target_,jint index_,jfloatArray data_){
    if(do_trace)
        fprintf(stderr,"glGetFloati_v\n");
    static PFNGLGETFLOATI_VPROC impl;
    if( !impl ){
       impl = (PFNGLGETFLOATI_VPROC) mygetprocaddr(env,"glGetFloati_v");
       if(!impl)
            return;
    }
    jfloat* _data_ = (data_ ? ((*env)->GetFloatArrayElements(env,data_,NULL) ) : NULL);
    impl(target_, index_, _data_);
    if(data_)
        (*env)->ReleaseFloatArrayElements(env,data_, _data_,0);
}
//GLAPI void APIENTRY glGetDoublei_v (GLenum target, GLuint index, GLdouble *data);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetDoublei_v( JNIEnv* env, jclass klass,jint target_,jint index_,jdoubleArray data_){
    if(do_trace)
        fprintf(stderr,"glGetDoublei_v\n");
    static PFNGLGETDOUBLEI_VPROC impl;
    if( !impl ){
       impl = (PFNGLGETDOUBLEI_VPROC) mygetprocaddr(env,"glGetDoublei_v");
       if(!impl)
            return;
    }
    jdouble* _data_ = (data_ ? ((*env)->GetDoubleArrayElements(env,data_,NULL) ) : NULL);
    impl(target_, index_, _data_);
    if(data_)
        (*env)->ReleaseDoubleArrayElements(env,data_, _data_,0);
}
//GLAPI void APIENTRY glDrawArraysInstancedBaseInstance (GLenum mode, GLint first, GLsizei count, GLsizei instancecount, GLuint baseinstance);
JNIEXPORT void JNICALL Java_JGL_JGL_glDrawArraysInstancedBaseInstance( JNIEnv* env, jclass klass,jint mode_,jint first_,jint count_,jint instancecount_,jint baseinstance_){
    if(do_trace)
        fprintf(stderr,"glDrawArraysInstancedBaseInstance\n");
    static PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC impl;
    if( !impl ){
       impl = (PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEPROC) mygetprocaddr(env,"glDrawArraysInstancedBaseInstance");
       if(!impl)
            return;
    }
    impl(mode_, first_, count_, instancecount_, baseinstance_);
}
//GLAPI void APIENTRY glDrawElementsInstancedBaseInstance (GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLuint baseinstance);
JNIEXPORT void JNICALL Java_JGL_JGL_glDrawElementsInstancedBaseInstance( JNIEnv* env, jclass klass,jint mode_,jint count_,jint type_,jint indices_,jint instancecount_,jint baseinstance_){
    if(do_trace)
        fprintf(stderr,"glDrawElementsInstancedBaseInstance\n");
    static PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC impl;
    if( !impl ){
       impl = (PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEPROC) mygetprocaddr(env,"glDrawElementsInstancedBaseInstance");
       if(!impl)
            return;
    }
    impl(mode_, count_, type_, (const void *)indices_, instancecount_, baseinstance_);
}
//GLAPI void APIENTRY glDrawElementsInstancedBaseVertexBaseInstance (GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLint basevertex, GLuint baseinstance);
JNIEXPORT void JNICALL Java_JGL_JGL_glDrawElementsInstancedBaseVertexBaseInstance( JNIEnv* env, jclass klass,jint mode_,jint count_,jint type_,jint indices_,jint instancecount_,jint basevertex_,jint baseinstance_){
    if(do_trace)
        fprintf(stderr,"glDrawElementsInstancedBaseVertexBaseInstance\n");
    static PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC impl;
    if( !impl ){
       impl = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEPROC) mygetprocaddr(env,"glDrawElementsInstancedBaseVertexBaseInstance");
       if(!impl)
            return;
    }
    impl(mode_, count_, type_, (const void *)indices_, instancecount_, basevertex_, baseinstance_);
}
//GLAPI void APIENTRY glGetInternalformativ (GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetInternalformativ( JNIEnv* env, jclass klass,jint target_,jint internalformat_,jint pname_,jint bufSize_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetInternalformativ\n");
    static PFNGLGETINTERNALFORMATIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETINTERNALFORMATIVPROC) mygetprocaddr(env,"glGetInternalformativ");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(target_, internalformat_, pname_, bufSize_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glGetActiveAtomicCounterBufferiv (GLuint program, GLuint bufferIndex, GLenum pname, GLint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetActiveAtomicCounterBufferiv( JNIEnv* env, jclass klass,jint program_,jint bufferIndex_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetActiveAtomicCounterBufferiv\n");
    static PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETACTIVEATOMICCOUNTERBUFFERIVPROC) mygetprocaddr(env,"glGetActiveAtomicCounterBufferiv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(program_, bufferIndex_, pname_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glBindImageTexture (GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format);
JNIEXPORT void JNICALL Java_JGL_JGL_glBindImageTexture( JNIEnv* env, jclass klass,jint unit_,jint texture_,jint level_,jboolean layered_,jint layer_,jint access_,jint format_){
    if(do_trace)
        fprintf(stderr,"glBindImageTexture\n");
    static PFNGLBINDIMAGETEXTUREPROC impl;
    if( !impl ){
       impl = (PFNGLBINDIMAGETEXTUREPROC) mygetprocaddr(env,"glBindImageTexture");
       if(!impl)
            return;
    }
    impl(unit_, texture_, level_, layered_, layer_, access_, format_);
}
//GLAPI void APIENTRY glMemoryBarrier (GLbitfield barriers);
JNIEXPORT void JNICALL Java_JGL_JGL_glMemoryBarrier( JNIEnv* env, jclass klass,jint barriers_){
    if(do_trace)
        fprintf(stderr,"glMemoryBarrier\n");
    static PFNGLMEMORYBARRIERPROC impl;
    if( !impl ){
       impl = (PFNGLMEMORYBARRIERPROC) mygetprocaddr(env,"glMemoryBarrier");
       if(!impl)
            return;
    }
    impl(barriers_);
}
//GLAPI void APIENTRY glTexStorage1D (GLenum target, GLsizei levels, GLenum internalformat, GLsizei width);
JNIEXPORT void JNICALL Java_JGL_JGL_glTexStorage1D( JNIEnv* env, jclass klass,jint target_,jint levels_,jint internalformat_,jint width_){
    if(do_trace)
        fprintf(stderr,"glTexStorage1D\n");
    static PFNGLTEXSTORAGE1DPROC impl;
    if( !impl ){
       impl = (PFNGLTEXSTORAGE1DPROC) mygetprocaddr(env,"glTexStorage1D");
       if(!impl)
            return;
    }
    impl(target_, levels_, internalformat_, width_);
}
//GLAPI void APIENTRY glTexStorage2D (GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
JNIEXPORT void JNICALL Java_JGL_JGL_glTexStorage2D( JNIEnv* env, jclass klass,jint target_,jint levels_,jint internalformat_,jint width_,jint height_){
    if(do_trace)
        fprintf(stderr,"glTexStorage2D\n");
    static PFNGLTEXSTORAGE2DPROC impl;
    if( !impl ){
       impl = (PFNGLTEXSTORAGE2DPROC) mygetprocaddr(env,"glTexStorage2D");
       if(!impl)
            return;
    }
    impl(target_, levels_, internalformat_, width_, height_);
}
//GLAPI void APIENTRY glTexStorage3D (GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);
JNIEXPORT void JNICALL Java_JGL_JGL_glTexStorage3D( JNIEnv* env, jclass klass,jint target_,jint levels_,jint internalformat_,jint width_,jint height_,jint depth_){
    if(do_trace)
        fprintf(stderr,"glTexStorage3D\n");
    static PFNGLTEXSTORAGE3DPROC impl;
    if( !impl ){
       impl = (PFNGLTEXSTORAGE3DPROC) mygetprocaddr(env,"glTexStorage3D");
       if(!impl)
            return;
    }
    impl(target_, levels_, internalformat_, width_, height_, depth_);
}
//GLAPI void APIENTRY glDrawTransformFeedbackInstanced (GLenum mode, GLuint id, GLsizei instancecount);
JNIEXPORT void JNICALL Java_JGL_JGL_glDrawTransformFeedbackInstanced( JNIEnv* env, jclass klass,jint mode_,jint id_,jint instancecount_){
    if(do_trace)
        fprintf(stderr,"glDrawTransformFeedbackInstanced\n");
    static PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC impl;
    if( !impl ){
       impl = (PFNGLDRAWTRANSFORMFEEDBACKINSTANCEDPROC) mygetprocaddr(env,"glDrawTransformFeedbackInstanced");
       if(!impl)
            return;
    }
    impl(mode_, id_, instancecount_);
}
//GLAPI void APIENTRY glDrawTransformFeedbackStreamInstanced (GLenum mode, GLuint id, GLuint stream, GLsizei instancecount);
JNIEXPORT void JNICALL Java_JGL_JGL_glDrawTransformFeedbackStreamInstanced( JNIEnv* env, jclass klass,jint mode_,jint id_,jint stream_,jint instancecount_){
    if(do_trace)
        fprintf(stderr,"glDrawTransformFeedbackStreamInstanced\n");
    static PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC impl;
    if( !impl ){
       impl = (PFNGLDRAWTRANSFORMFEEDBACKSTREAMINSTANCEDPROC) mygetprocaddr(env,"glDrawTransformFeedbackStreamInstanced");
       if(!impl)
            return;
    }
    impl(mode_, id_, stream_, instancecount_);
}
//GLAPI void APIENTRY glClearBufferData (GLenum target, GLenum internalformat, GLenum format, GLenum type, const void *data);
JNIEXPORT void JNICALL Java_JGL_JGL_glClearBufferData( JNIEnv* env, jclass klass,jint target_,jint internalformat_,jint format_,jint type_,jbyteArray data_){
    if(do_trace)
        fprintf(stderr,"glClearBufferData\n");
    static PFNGLCLEARBUFFERDATAPROC impl;
    if( !impl ){
       impl = (PFNGLCLEARBUFFERDATAPROC) mygetprocaddr(env,"glClearBufferData");
       if(!impl)
            return;
    }
    jbyte* _data_ = (data_ ? ((*env)->GetByteArrayElements(env,data_,NULL) ) : NULL);
    impl(target_, internalformat_, format_, type_, _data_);
    if(data_)
        (*env)->ReleaseByteArrayElements(env,data_, _data_,0);
}
//GLAPI void APIENTRY glClearBufferSubData (GLenum target, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void *data);
JNIEXPORT void JNICALL Java_JGL_JGL_glClearBufferSubData( JNIEnv* env, jclass klass,jint target_,jint internalformat_,jint offset_,jint size_,jint format_,jint type_,jbyteArray data_){
    if(do_trace)
        fprintf(stderr,"glClearBufferSubData\n");
    static PFNGLCLEARBUFFERSUBDATAPROC impl;
    if( !impl ){
       impl = (PFNGLCLEARBUFFERSUBDATAPROC) mygetprocaddr(env,"glClearBufferSubData");
       if(!impl)
            return;
    }
    jbyte* _data_ = (data_ ? ((*env)->GetByteArrayElements(env,data_,NULL) ) : NULL);
    impl(target_, internalformat_, offset_, size_, format_, type_, _data_);
    if(data_)
        (*env)->ReleaseByteArrayElements(env,data_, _data_,0);
}
//GLAPI void APIENTRY glDispatchCompute (GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z);
JNIEXPORT void JNICALL Java_JGL_JGL_glDispatchCompute( JNIEnv* env, jclass klass,jint num_groups_x_,jint num_groups_y_,jint num_groups_z_){
    if(do_trace)
        fprintf(stderr,"glDispatchCompute\n");
    static PFNGLDISPATCHCOMPUTEPROC impl;
    if( !impl ){
       impl = (PFNGLDISPATCHCOMPUTEPROC) mygetprocaddr(env,"glDispatchCompute");
       if(!impl)
            return;
    }
    impl(num_groups_x_, num_groups_y_, num_groups_z_);
}
//GLAPI void APIENTRY glDispatchComputeIndirect (GLintptr indirect);
JNIEXPORT void JNICALL Java_JGL_JGL_glDispatchComputeIndirect( JNIEnv* env, jclass klass,jint indirect_){
    if(do_trace)
        fprintf(stderr,"glDispatchComputeIndirect\n");
    static PFNGLDISPATCHCOMPUTEINDIRECTPROC impl;
    if( !impl ){
       impl = (PFNGLDISPATCHCOMPUTEINDIRECTPROC) mygetprocaddr(env,"glDispatchComputeIndirect");
       if(!impl)
            return;
    }
    impl(indirect_);
}
//GLAPI void APIENTRY glCopyImageSubData (GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth);
JNIEXPORT void JNICALL Java_JGL_JGL_glCopyImageSubData( JNIEnv* env, jclass klass,jint srcName_,jint srcTarget_,jint srcLevel_,jint srcX_,jint srcY_,jint srcZ_,jint dstName_,jint dstTarget_,jint dstLevel_,jint dstX_,jint dstY_,jint dstZ_,jint srcWidth_,jint srcHeight_,jint srcDepth_){
    if(do_trace)
        fprintf(stderr,"glCopyImageSubData\n");
    static PFNGLCOPYIMAGESUBDATAPROC impl;
    if( !impl ){
       impl = (PFNGLCOPYIMAGESUBDATAPROC) mygetprocaddr(env,"glCopyImageSubData");
       if(!impl)
            return;
    }
    impl(srcName_, srcTarget_, srcLevel_, srcX_, srcY_, srcZ_, dstName_, dstTarget_, dstLevel_, dstX_, dstY_, dstZ_, srcWidth_, srcHeight_, srcDepth_);
}
//GLAPI void APIENTRY glFramebufferParameteri (GLenum target, GLenum pname, GLint param);
JNIEXPORT void JNICALL Java_JGL_JGL_glFramebufferParameteri( JNIEnv* env, jclass klass,jint target_,jint pname_,jint param_){
    if(do_trace)
        fprintf(stderr,"glFramebufferParameteri\n");
    static PFNGLFRAMEBUFFERPARAMETERIPROC impl;
    if( !impl ){
       impl = (PFNGLFRAMEBUFFERPARAMETERIPROC) mygetprocaddr(env,"glFramebufferParameteri");
       if(!impl)
            return;
    }
    impl(target_, pname_, param_);
}
//GLAPI void APIENTRY glGetFramebufferParameteriv (GLenum target, GLenum pname, GLint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetFramebufferParameteriv( JNIEnv* env, jclass klass,jint target_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetFramebufferParameteriv\n");
    static PFNGLGETFRAMEBUFFERPARAMETERIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETFRAMEBUFFERPARAMETERIVPROC) mygetprocaddr(env,"glGetFramebufferParameteriv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(target_, pname_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glGetInternalformati64v (GLenum target, GLenum internalformat, GLenum pname, GLsizei bufSize, GLint64 *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetInternalformati64v( JNIEnv* env, jclass klass,jint target_,jint internalformat_,jint pname_,jint bufSize_,jlongArray params_){
    if(do_trace)
        fprintf(stderr,"glGetInternalformati64v\n");
    static PFNGLGETINTERNALFORMATI64VPROC impl;
    if( !impl ){
       impl = (PFNGLGETINTERNALFORMATI64VPROC) mygetprocaddr(env,"glGetInternalformati64v");
       if(!impl)
            return;
    }
    jlong* _params_ = (params_ ? ((*env)->GetLongArrayElements(env,params_,NULL) ) : NULL);
    impl(target_, internalformat_, pname_, bufSize_, _params_);
    if(params_)
        (*env)->ReleaseLongArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glInvalidateTexSubImage (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth);
JNIEXPORT void JNICALL Java_JGL_JGL_glInvalidateTexSubImage( JNIEnv* env, jclass klass,jint texture_,jint level_,jint xoffset_,jint yoffset_,jint zoffset_,jint width_,jint height_,jint depth_){
    if(do_trace)
        fprintf(stderr,"glInvalidateTexSubImage\n");
    static PFNGLINVALIDATETEXSUBIMAGEPROC impl;
    if( !impl ){
       impl = (PFNGLINVALIDATETEXSUBIMAGEPROC) mygetprocaddr(env,"glInvalidateTexSubImage");
       if(!impl)
            return;
    }
    impl(texture_, level_, xoffset_, yoffset_, zoffset_, width_, height_, depth_);
}
//GLAPI void APIENTRY glInvalidateTexImage (GLuint texture, GLint level);
JNIEXPORT void JNICALL Java_JGL_JGL_glInvalidateTexImage( JNIEnv* env, jclass klass,jint texture_,jint level_){
    if(do_trace)
        fprintf(stderr,"glInvalidateTexImage\n");
    static PFNGLINVALIDATETEXIMAGEPROC impl;
    if( !impl ){
       impl = (PFNGLINVALIDATETEXIMAGEPROC) mygetprocaddr(env,"glInvalidateTexImage");
       if(!impl)
            return;
    }
    impl(texture_, level_);
}
//GLAPI void APIENTRY glInvalidateBufferSubData (GLuint buffer, GLintptr offset, GLsizeiptr length);
JNIEXPORT void JNICALL Java_JGL_JGL_glInvalidateBufferSubData( JNIEnv* env, jclass klass,jint buffer_,jint offset_,jint length_){
    if(do_trace)
        fprintf(stderr,"glInvalidateBufferSubData\n");
    static PFNGLINVALIDATEBUFFERSUBDATAPROC impl;
    if( !impl ){
       impl = (PFNGLINVALIDATEBUFFERSUBDATAPROC) mygetprocaddr(env,"glInvalidateBufferSubData");
       if(!impl)
            return;
    }
    impl(buffer_, offset_, length_);
}
//GLAPI void APIENTRY glInvalidateBufferData (GLuint buffer);
JNIEXPORT void JNICALL Java_JGL_JGL_glInvalidateBufferData( JNIEnv* env, jclass klass,jint buffer_){
    if(do_trace)
        fprintf(stderr,"glInvalidateBufferData\n");
    static PFNGLINVALIDATEBUFFERDATAPROC impl;
    if( !impl ){
       impl = (PFNGLINVALIDATEBUFFERDATAPROC) mygetprocaddr(env,"glInvalidateBufferData");
       if(!impl)
            return;
    }
    impl(buffer_);
}
//GLAPI void APIENTRY glInvalidateFramebuffer (GLenum target, GLsizei numAttachments, const GLenum *attachments);
JNIEXPORT void JNICALL Java_JGL_JGL_glInvalidateFramebuffer( JNIEnv* env, jclass klass,jint target_,jint numAttachments_,jintArray attachments_){
    if(do_trace)
        fprintf(stderr,"glInvalidateFramebuffer\n");
    static PFNGLINVALIDATEFRAMEBUFFERPROC impl;
    if( !impl ){
       impl = (PFNGLINVALIDATEFRAMEBUFFERPROC) mygetprocaddr(env,"glInvalidateFramebuffer");
       if(!impl)
            return;
    }
    jint* _attachments_ = (attachments_ ? ((*env)->GetIntArrayElements(env,attachments_,NULL) ) : NULL);
    impl(target_, numAttachments_, _attachments_);
    if(attachments_)
        (*env)->ReleaseIntArrayElements(env,attachments_, _attachments_,0);
}
//GLAPI void APIENTRY glInvalidateSubFramebuffer (GLenum target, GLsizei numAttachments, const GLenum *attachments, GLint x, GLint y, GLsizei width, GLsizei height);
JNIEXPORT void JNICALL Java_JGL_JGL_glInvalidateSubFramebuffer( JNIEnv* env, jclass klass,jint target_,jint numAttachments_,jintArray attachments_,jint x_,jint y_,jint width_,jint height_){
    if(do_trace)
        fprintf(stderr,"glInvalidateSubFramebuffer\n");
    static PFNGLINVALIDATESUBFRAMEBUFFERPROC impl;
    if( !impl ){
       impl = (PFNGLINVALIDATESUBFRAMEBUFFERPROC) mygetprocaddr(env,"glInvalidateSubFramebuffer");
       if(!impl)
            return;
    }
    jint* _attachments_ = (attachments_ ? ((*env)->GetIntArrayElements(env,attachments_,NULL) ) : NULL);
    impl(target_, numAttachments_, _attachments_, x_, y_, width_, height_);
    if(attachments_)
        (*env)->ReleaseIntArrayElements(env,attachments_, _attachments_,0);
}
//GLAPI void APIENTRY glGetProgramInterfaceiv (GLuint program, GLenum programInterface, GLenum pname, GLint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetProgramInterfaceiv( JNIEnv* env, jclass klass,jint program_,jint programInterface_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetProgramInterfaceiv\n");
    static PFNGLGETPROGRAMINTERFACEIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETPROGRAMINTERFACEIVPROC) mygetprocaddr(env,"glGetProgramInterfaceiv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(program_, programInterface_, pname_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI GLuint APIENTRY glGetProgramResourceIndex (GLuint program, GLenum programInterface, const GLchar *name);
JNIEXPORT jint JNICALL Java_JGL_JGL_glGetProgramResourceIndex( JNIEnv* env, jclass klass,jint program_,jint programInterface_,jbyteArray name_){
    jint rv = 0;
    if(do_trace)
        fprintf(stderr,"glGetProgramResourceIndex\n");
    static PFNGLGETPROGRAMRESOURCEINDEXPROC impl;
    if( !impl ){
       impl = (PFNGLGETPROGRAMRESOURCEINDEXPROC) mygetprocaddr(env,"glGetProgramResourceIndex");
       if(!impl)
            return rv;
    }
    jbyte* _name_ = (name_ ? ((*env)->GetByteArrayElements(env,name_,NULL) ) : NULL);
    rv = impl(program_, programInterface_, _name_);
    if(name_)
        (*env)->ReleaseByteArrayElements(env,name_, _name_,0);
    return rv;
}
//GLAPI void APIENTRY glGetProgramResourceName (GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei *length, GLchar *name);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetProgramResourceName( JNIEnv* env, jclass klass,jint program_,jint programInterface_,jint index_,jint bufSize_,jintArray length_,jbyteArray name_){
    if(do_trace)
        fprintf(stderr,"glGetProgramResourceName\n");
    static PFNGLGETPROGRAMRESOURCENAMEPROC impl;
    if( !impl ){
       impl = (PFNGLGETPROGRAMRESOURCENAMEPROC) mygetprocaddr(env,"glGetProgramResourceName");
       if(!impl)
            return;
    }
    jint* _length_ = (length_ ? ((*env)->GetIntArrayElements(env,length_,NULL) ) : NULL);
    jbyte* _name_ = (name_ ? ((*env)->GetByteArrayElements(env,name_,NULL) ) : NULL);
    impl(program_, programInterface_, index_, bufSize_, _length_, _name_);
    if(length_)
        (*env)->ReleaseIntArrayElements(env,length_, _length_,0);
    if(name_)
        (*env)->ReleaseByteArrayElements(env,name_, _name_,0);
}
//GLAPI void APIENTRY glGetProgramResourceiv (GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum *props, GLsizei bufSize, GLsizei *length, GLint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetProgramResourceiv( JNIEnv* env, jclass klass,jint program_,jint programInterface_,jint index_,jint propCount_,jintArray props_,jint bufSize_,jintArray length_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetProgramResourceiv\n");
    static PFNGLGETPROGRAMRESOURCEIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETPROGRAMRESOURCEIVPROC) mygetprocaddr(env,"glGetProgramResourceiv");
       if(!impl)
            return;
    }
    jint* _props_ = (props_ ? ((*env)->GetIntArrayElements(env,props_,NULL) ) : NULL);
    jint* _length_ = (length_ ? ((*env)->GetIntArrayElements(env,length_,NULL) ) : NULL);
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(program_, programInterface_, index_, propCount_, _props_, bufSize_, _length_, _params_);
    if(props_)
        (*env)->ReleaseIntArrayElements(env,props_, _props_,0);
    if(length_)
        (*env)->ReleaseIntArrayElements(env,length_, _length_,0);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI GLint APIENTRY glGetProgramResourceLocation (GLuint program, GLenum programInterface, const GLchar *name);
JNIEXPORT jint JNICALL Java_JGL_JGL_glGetProgramResourceLocation( JNIEnv* env, jclass klass,jint program_,jint programInterface_,jbyteArray name_){
    jint rv = 0;
    if(do_trace)
        fprintf(stderr,"glGetProgramResourceLocation\n");
    static PFNGLGETPROGRAMRESOURCELOCATIONPROC impl;
    if( !impl ){
       impl = (PFNGLGETPROGRAMRESOURCELOCATIONPROC) mygetprocaddr(env,"glGetProgramResourceLocation");
       if(!impl)
            return rv;
    }
    jbyte* _name_ = (name_ ? ((*env)->GetByteArrayElements(env,name_,NULL) ) : NULL);
    rv = impl(program_, programInterface_, _name_);
    if(name_)
        (*env)->ReleaseByteArrayElements(env,name_, _name_,0);
    return rv;
}
//GLAPI GLint APIENTRY glGetProgramResourceLocationIndex (GLuint program, GLenum programInterface, const GLchar *name);
JNIEXPORT jint JNICALL Java_JGL_JGL_glGetProgramResourceLocationIndex( JNIEnv* env, jclass klass,jint program_,jint programInterface_,jbyteArray name_){
    jint rv = 0;
    if(do_trace)
        fprintf(stderr,"glGetProgramResourceLocationIndex\n");
    static PFNGLGETPROGRAMRESOURCELOCATIONINDEXPROC impl;
    if( !impl ){
       impl = (PFNGLGETPROGRAMRESOURCELOCATIONINDEXPROC) mygetprocaddr(env,"glGetProgramResourceLocationIndex");
       if(!impl)
            return rv;
    }
    jbyte* _name_ = (name_ ? ((*env)->GetByteArrayElements(env,name_,NULL) ) : NULL);
    rv = impl(program_, programInterface_, _name_);
    if(name_)
        (*env)->ReleaseByteArrayElements(env,name_, _name_,0);
    return rv;
}
//GLAPI void APIENTRY glShaderStorageBlockBinding (GLuint program, GLuint storageBlockIndex, GLuint storageBlockBinding);
JNIEXPORT void JNICALL Java_JGL_JGL_glShaderStorageBlockBinding( JNIEnv* env, jclass klass,jint program_,jint storageBlockIndex_,jint storageBlockBinding_){
    if(do_trace)
        fprintf(stderr,"glShaderStorageBlockBinding\n");
    static PFNGLSHADERSTORAGEBLOCKBINDINGPROC impl;
    if( !impl ){
       impl = (PFNGLSHADERSTORAGEBLOCKBINDINGPROC) mygetprocaddr(env,"glShaderStorageBlockBinding");
       if(!impl)
            return;
    }
    impl(program_, storageBlockIndex_, storageBlockBinding_);
}
//GLAPI void APIENTRY glTexBufferRange (GLenum target, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size);
JNIEXPORT void JNICALL Java_JGL_JGL_glTexBufferRange( JNIEnv* env, jclass klass,jint target_,jint internalformat_,jint buffer_,jint offset_,jint size_){
    if(do_trace)
        fprintf(stderr,"glTexBufferRange\n");
    static PFNGLTEXBUFFERRANGEPROC impl;
    if( !impl ){
       impl = (PFNGLTEXBUFFERRANGEPROC) mygetprocaddr(env,"glTexBufferRange");
       if(!impl)
            return;
    }
    impl(target_, internalformat_, buffer_, offset_, size_);
}
//GLAPI void APIENTRY glTexStorage2DMultisample (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
JNIEXPORT void JNICALL Java_JGL_JGL_glTexStorage2DMultisample( JNIEnv* env, jclass klass,jint target_,jint samples_,jint internalformat_,jint width_,jint height_,jboolean fixedsamplelocations_){
    if(do_trace)
        fprintf(stderr,"glTexStorage2DMultisample\n");
    static PFNGLTEXSTORAGE2DMULTISAMPLEPROC impl;
    if( !impl ){
       impl = (PFNGLTEXSTORAGE2DMULTISAMPLEPROC) mygetprocaddr(env,"glTexStorage2DMultisample");
       if(!impl)
            return;
    }
    impl(target_, samples_, internalformat_, width_, height_, fixedsamplelocations_);
}
//GLAPI void APIENTRY glTexStorage3DMultisample (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
JNIEXPORT void JNICALL Java_JGL_JGL_glTexStorage3DMultisample( JNIEnv* env, jclass klass,jint target_,jint samples_,jint internalformat_,jint width_,jint height_,jint depth_,jboolean fixedsamplelocations_){
    if(do_trace)
        fprintf(stderr,"glTexStorage3DMultisample\n");
    static PFNGLTEXSTORAGE3DMULTISAMPLEPROC impl;
    if( !impl ){
       impl = (PFNGLTEXSTORAGE3DMULTISAMPLEPROC) mygetprocaddr(env,"glTexStorage3DMultisample");
       if(!impl)
            return;
    }
    impl(target_, samples_, internalformat_, width_, height_, depth_, fixedsamplelocations_);
}
//GLAPI void APIENTRY glTextureView (GLuint texture, GLenum target, GLuint origtexture, GLenum internalformat, GLuint minlevel, GLuint numlevels, GLuint minlayer, GLuint numlayers);
JNIEXPORT void JNICALL Java_JGL_JGL_glTextureView( JNIEnv* env, jclass klass,jint texture_,jint target_,jint origtexture_,jint internalformat_,jint minlevel_,jint numlevels_,jint minlayer_,jint numlayers_){
    if(do_trace)
        fprintf(stderr,"glTextureView\n");
    static PFNGLTEXTUREVIEWPROC impl;
    if( !impl ){
       impl = (PFNGLTEXTUREVIEWPROC) mygetprocaddr(env,"glTextureView");
       if(!impl)
            return;
    }
    impl(texture_, target_, origtexture_, internalformat_, minlevel_, numlevels_, minlayer_, numlayers_);
}
//GLAPI void APIENTRY glBindVertexBuffer (GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);
JNIEXPORT void JNICALL Java_JGL_JGL_glBindVertexBuffer( JNIEnv* env, jclass klass,jint bindingindex_,jint buffer_,jint offset_,jint stride_){
    if(do_trace)
        fprintf(stderr,"glBindVertexBuffer\n");
    static PFNGLBINDVERTEXBUFFERPROC impl;
    if( !impl ){
       impl = (PFNGLBINDVERTEXBUFFERPROC) mygetprocaddr(env,"glBindVertexBuffer");
       if(!impl)
            return;
    }
    impl(bindingindex_, buffer_, offset_, stride_);
}
//GLAPI void APIENTRY glVertexAttribFormat (GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribFormat( JNIEnv* env, jclass klass,jint attribindex_,jint size_,jint type_,jboolean normalized_,jint relativeoffset_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribFormat\n");
    static PFNGLVERTEXATTRIBFORMATPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBFORMATPROC) mygetprocaddr(env,"glVertexAttribFormat");
       if(!impl)
            return;
    }
    impl(attribindex_, size_, type_, normalized_, relativeoffset_);
}
//GLAPI void APIENTRY glVertexAttribIFormat (GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribIFormat( JNIEnv* env, jclass klass,jint attribindex_,jint size_,jint type_,jint relativeoffset_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribIFormat\n");
    static PFNGLVERTEXATTRIBIFORMATPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBIFORMATPROC) mygetprocaddr(env,"glVertexAttribIFormat");
       if(!impl)
            return;
    }
    impl(attribindex_, size_, type_, relativeoffset_);
}
//GLAPI void APIENTRY glVertexAttribLFormat (GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribLFormat( JNIEnv* env, jclass klass,jint attribindex_,jint size_,jint type_,jint relativeoffset_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribLFormat\n");
    static PFNGLVERTEXATTRIBLFORMATPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBLFORMATPROC) mygetprocaddr(env,"glVertexAttribLFormat");
       if(!impl)
            return;
    }
    impl(attribindex_, size_, type_, relativeoffset_);
}
//GLAPI void APIENTRY glVertexAttribBinding (GLuint attribindex, GLuint bindingindex);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribBinding( JNIEnv* env, jclass klass,jint attribindex_,jint bindingindex_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribBinding\n");
    static PFNGLVERTEXATTRIBBINDINGPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBBINDINGPROC) mygetprocaddr(env,"glVertexAttribBinding");
       if(!impl)
            return;
    }
    impl(attribindex_, bindingindex_);
}
//GLAPI void APIENTRY glVertexBindingDivisor (GLuint bindingindex, GLuint divisor);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexBindingDivisor( JNIEnv* env, jclass klass,jint bindingindex_,jint divisor_){
    if(do_trace)
        fprintf(stderr,"glVertexBindingDivisor\n");
    static PFNGLVERTEXBINDINGDIVISORPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXBINDINGDIVISORPROC) mygetprocaddr(env,"glVertexBindingDivisor");
       if(!impl)
            return;
    }
    impl(bindingindex_, divisor_);
}
//GLAPI void APIENTRY glDebugMessageControl (GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled);
JNIEXPORT void JNICALL Java_JGL_JGL_glDebugMessageControl( JNIEnv* env, jclass klass,jint source_,jint type_,jint severity_,jint count_,jintArray ids_,jboolean enabled_){
    if(do_trace)
        fprintf(stderr,"glDebugMessageControl\n");
    static PFNGLDEBUGMESSAGECONTROLPROC impl;
    if( !impl ){
       impl = (PFNGLDEBUGMESSAGECONTROLPROC) mygetprocaddr(env,"glDebugMessageControl");
       if(!impl)
            return;
    }
    jint* _ids_ = (ids_ ? ((*env)->GetIntArrayElements(env,ids_,NULL) ) : NULL);
    impl(source_, type_, severity_, count_, _ids_, enabled_);
    if(ids_)
        (*env)->ReleaseIntArrayElements(env,ids_, _ids_,0);
}
//GLAPI void APIENTRY glDebugMessageInsert (GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *buf);
JNIEXPORT void JNICALL Java_JGL_JGL_glDebugMessageInsert( JNIEnv* env, jclass klass,jint source_,jint type_,jint id_,jint severity_,jint length_,jbyteArray buf_){
    if(do_trace)
        fprintf(stderr,"glDebugMessageInsert\n");
    static PFNGLDEBUGMESSAGEINSERTPROC impl;
    if( !impl ){
       impl = (PFNGLDEBUGMESSAGEINSERTPROC) mygetprocaddr(env,"glDebugMessageInsert");
       if(!impl)
            return;
    }
    jbyte* _buf_ = (buf_ ? ((*env)->GetByteArrayElements(env,buf_,NULL) ) : NULL);
    impl(source_, type_, id_, severity_, length_, _buf_);
    if(buf_)
        (*env)->ReleaseByteArrayElements(env,buf_, _buf_,0);
}
//GLAPI GLuint APIENTRY glGetDebugMessageLog (GLuint count, GLsizei bufSize, GLenum *sources, GLenum *types, GLuint *ids, GLenum *severities, GLsizei *lengths, GLchar *messageLog);
JNIEXPORT jint JNICALL Java_JGL_JGL_glGetDebugMessageLog( JNIEnv* env, jclass klass,jint count_,jint bufSize_,jintArray sources_,jintArray types_,jintArray ids_,jintArray severities_,jintArray lengths_,jbyteArray messageLog_){
    jint rv = 0;
    if(do_trace)
        fprintf(stderr,"glGetDebugMessageLog\n");
    static PFNGLGETDEBUGMESSAGELOGPROC impl;
    if( !impl ){
       impl = (PFNGLGETDEBUGMESSAGELOGPROC) mygetprocaddr(env,"glGetDebugMessageLog");
       if(!impl)
            return rv;
    }
    jint* _sources_ = (sources_ ? ((*env)->GetIntArrayElements(env,sources_,NULL) ) : NULL);
    jint* _types_ = (types_ ? ((*env)->GetIntArrayElements(env,types_,NULL) ) : NULL);
    jint* _ids_ = (ids_ ? ((*env)->GetIntArrayElements(env,ids_,NULL) ) : NULL);
    jint* _severities_ = (severities_ ? ((*env)->GetIntArrayElements(env,severities_,NULL) ) : NULL);
    jint* _lengths_ = (lengths_ ? ((*env)->GetIntArrayElements(env,lengths_,NULL) ) : NULL);
    jbyte* _messageLog_ = (messageLog_ ? ((*env)->GetByteArrayElements(env,messageLog_,NULL) ) : NULL);
    rv = impl(count_, bufSize_, _sources_, _types_, _ids_, _severities_, _lengths_, _messageLog_);
    if(sources_)
        (*env)->ReleaseIntArrayElements(env,sources_, _sources_,0);
    if(types_)
        (*env)->ReleaseIntArrayElements(env,types_, _types_,0);
    if(ids_)
        (*env)->ReleaseIntArrayElements(env,ids_, _ids_,0);
    if(severities_)
        (*env)->ReleaseIntArrayElements(env,severities_, _severities_,0);
    if(lengths_)
        (*env)->ReleaseIntArrayElements(env,lengths_, _lengths_,0);
    if(messageLog_)
        (*env)->ReleaseByteArrayElements(env,messageLog_, _messageLog_,0);
    return rv;
}
//GLAPI void APIENTRY glPushDebugGroup (GLenum source, GLuint id, GLsizei length, const GLchar *message);
JNIEXPORT void JNICALL Java_JGL_JGL_glPushDebugGroup( JNIEnv* env, jclass klass,jint source_,jint id_,jint length_,jbyteArray message_){
    if(do_trace)
        fprintf(stderr,"glPushDebugGroup\n");
    static PFNGLPUSHDEBUGGROUPPROC impl;
    if( !impl ){
       impl = (PFNGLPUSHDEBUGGROUPPROC) mygetprocaddr(env,"glPushDebugGroup");
       if(!impl)
            return;
    }
    jbyte* _message_ = (message_ ? ((*env)->GetByteArrayElements(env,message_,NULL) ) : NULL);
    impl(source_, id_, length_, _message_);
    if(message_)
        (*env)->ReleaseByteArrayElements(env,message_, _message_,0);
}
//GLAPI void APIENTRY glPopDebugGroup (void);
JNIEXPORT void JNICALL Java_JGL_JGL_glPopDebugGroup( JNIEnv* env, jclass klass){
    if(do_trace)
        fprintf(stderr,"glPopDebugGroup\n");
    static PFNGLPOPDEBUGGROUPPROC impl;
    if( !impl ){
       impl = (PFNGLPOPDEBUGGROUPPROC) mygetprocaddr(env,"glPopDebugGroup");
       if(!impl)
            return;
    }
    impl();
}
//GLAPI void APIENTRY glObjectLabel (GLenum identifier, GLuint name, GLsizei length, const GLchar *label);
JNIEXPORT void JNICALL Java_JGL_JGL_glObjectLabel( JNIEnv* env, jclass klass,jint identifier_,jint name_,jint length_,jbyteArray label_){
    if(do_trace)
        fprintf(stderr,"glObjectLabel\n");
    static PFNGLOBJECTLABELPROC impl;
    if( !impl ){
       impl = (PFNGLOBJECTLABELPROC) mygetprocaddr(env,"glObjectLabel");
       if(!impl)
            return;
    }
    jbyte* _label_ = (label_ ? ((*env)->GetByteArrayElements(env,label_,NULL) ) : NULL);
    impl(identifier_, name_, length_, _label_);
    if(label_)
        (*env)->ReleaseByteArrayElements(env,label_, _label_,0);
}
//GLAPI void APIENTRY glGetObjectLabel (GLenum identifier, GLuint name, GLsizei bufSize, GLsizei *length, GLchar *label);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetObjectLabel( JNIEnv* env, jclass klass,jint identifier_,jint name_,jint bufSize_,jintArray length_,jbyteArray label_){
    if(do_trace)
        fprintf(stderr,"glGetObjectLabel\n");
    static PFNGLGETOBJECTLABELPROC impl;
    if( !impl ){
       impl = (PFNGLGETOBJECTLABELPROC) mygetprocaddr(env,"glGetObjectLabel");
       if(!impl)
            return;
    }
    jint* _length_ = (length_ ? ((*env)->GetIntArrayElements(env,length_,NULL) ) : NULL);
    jbyte* _label_ = (label_ ? ((*env)->GetByteArrayElements(env,label_,NULL) ) : NULL);
    impl(identifier_, name_, bufSize_, _length_, _label_);
    if(length_)
        (*env)->ReleaseIntArrayElements(env,length_, _length_,0);
    if(label_)
        (*env)->ReleaseByteArrayElements(env,label_, _label_,0);
}
//GLAPI void APIENTRY glObjectPtrLabel (const void *ptr, GLsizei length, const GLchar *label);
JNIEXPORT void JNICALL Java_JGL_JGL_glObjectPtrLabel( JNIEnv* env, jclass klass,jbyteArray ptr_,jint length_,jbyteArray label_){
    if(do_trace)
        fprintf(stderr,"glObjectPtrLabel\n");
    static PFNGLOBJECTPTRLABELPROC impl;
    if( !impl ){
       impl = (PFNGLOBJECTPTRLABELPROC) mygetprocaddr(env,"glObjectPtrLabel");
       if(!impl)
            return;
    }
    jbyte* _ptr_ = (ptr_ ? ((*env)->GetByteArrayElements(env,ptr_,NULL) ) : NULL);
    jbyte* _label_ = (label_ ? ((*env)->GetByteArrayElements(env,label_,NULL) ) : NULL);
    impl(_ptr_, length_, _label_);
    if(ptr_)
        (*env)->ReleaseByteArrayElements(env,ptr_, _ptr_,0);
    if(label_)
        (*env)->ReleaseByteArrayElements(env,label_, _label_,0);
}
//GLAPI void APIENTRY glGetObjectPtrLabel (const void *ptr, GLsizei bufSize, GLsizei *length, GLchar *label);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetObjectPtrLabel( JNIEnv* env, jclass klass,jbyteArray ptr_,jint bufSize_,jintArray length_,jbyteArray label_){
    if(do_trace)
        fprintf(stderr,"glGetObjectPtrLabel\n");
    static PFNGLGETOBJECTPTRLABELPROC impl;
    if( !impl ){
       impl = (PFNGLGETOBJECTPTRLABELPROC) mygetprocaddr(env,"glGetObjectPtrLabel");
       if(!impl)
            return;
    }
    jbyte* _ptr_ = (ptr_ ? ((*env)->GetByteArrayElements(env,ptr_,NULL) ) : NULL);
    jint* _length_ = (length_ ? ((*env)->GetIntArrayElements(env,length_,NULL) ) : NULL);
    jbyte* _label_ = (label_ ? ((*env)->GetByteArrayElements(env,label_,NULL) ) : NULL);
    impl(_ptr_, bufSize_, _length_, _label_);
    if(ptr_)
        (*env)->ReleaseByteArrayElements(env,ptr_, _ptr_,0);
    if(length_)
        (*env)->ReleaseIntArrayElements(env,length_, _length_,0);
    if(label_)
        (*env)->ReleaseByteArrayElements(env,label_, _label_,0);
}
//GLAPI void APIENTRY glBufferStorage (GLenum target, GLsizeiptr size, const void *data, GLbitfield flags);
JNIEXPORT void JNICALL Java_JGL_JGL_glBufferStorage( JNIEnv* env, jclass klass,jint target_,jint size_,jbyteArray data_,jint flags_){
    if(do_trace)
        fprintf(stderr,"glBufferStorage\n");
    static PFNGLBUFFERSTORAGEPROC impl;
    if( !impl ){
       impl = (PFNGLBUFFERSTORAGEPROC) mygetprocaddr(env,"glBufferStorage");
       if(!impl)
            return;
    }
    jbyte* _data_ = (data_ ? ((*env)->GetByteArrayElements(env,data_,NULL) ) : NULL);
    impl(target_, size_, _data_, flags_);
    if(data_)
        (*env)->ReleaseByteArrayElements(env,data_, _data_,0);
}
//GLAPI void APIENTRY glClearTexImage (GLuint texture, GLint level, GLenum format, GLenum type, const void *data);
JNIEXPORT void JNICALL Java_JGL_JGL_glClearTexImage( JNIEnv* env, jclass klass,jint texture_,jint level_,jint format_,jint type_,jbyteArray data_){
    if(do_trace)
        fprintf(stderr,"glClearTexImage\n");
    static PFNGLCLEARTEXIMAGEPROC impl;
    if( !impl ){
       impl = (PFNGLCLEARTEXIMAGEPROC) mygetprocaddr(env,"glClearTexImage");
       if(!impl)
            return;
    }
    jbyte* _data_ = (data_ ? ((*env)->GetByteArrayElements(env,data_,NULL) ) : NULL);
    impl(texture_, level_, format_, type_, _data_);
    if(data_)
        (*env)->ReleaseByteArrayElements(env,data_, _data_,0);
}
//GLAPI void APIENTRY glClearTexSubImage (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *data);
JNIEXPORT void JNICALL Java_JGL_JGL_glClearTexSubImage( JNIEnv* env, jclass klass,jint texture_,jint level_,jint xoffset_,jint yoffset_,jint zoffset_,jint width_,jint height_,jint depth_,jint format_,jint type_,jbyteArray data_){
    if(do_trace)
        fprintf(stderr,"glClearTexSubImage\n");
    static PFNGLCLEARTEXSUBIMAGEPROC impl;
    if( !impl ){
       impl = (PFNGLCLEARTEXSUBIMAGEPROC) mygetprocaddr(env,"glClearTexSubImage");
       if(!impl)
            return;
    }
    jbyte* _data_ = (data_ ? ((*env)->GetByteArrayElements(env,data_,NULL) ) : NULL);
    impl(texture_, level_, xoffset_, yoffset_, zoffset_, width_, height_, depth_, format_, type_, _data_);
    if(data_)
        (*env)->ReleaseByteArrayElements(env,data_, _data_,0);
}
//GLAPI void APIENTRY glBindBuffersBase (GLenum target, GLuint first, GLsizei count, const GLuint *buffers);
JNIEXPORT void JNICALL Java_JGL_JGL_glBindBuffersBase( JNIEnv* env, jclass klass,jint target_,jint first_,jint count_,jintArray buffers_){
    if(do_trace)
        fprintf(stderr,"glBindBuffersBase\n");
    static PFNGLBINDBUFFERSBASEPROC impl;
    if( !impl ){
       impl = (PFNGLBINDBUFFERSBASEPROC) mygetprocaddr(env,"glBindBuffersBase");
       if(!impl)
            return;
    }
    jint* _buffers_ = (buffers_ ? ((*env)->GetIntArrayElements(env,buffers_,NULL) ) : NULL);
    impl(target_, first_, count_, _buffers_);
    if(buffers_)
        (*env)->ReleaseIntArrayElements(env,buffers_, _buffers_,0);
}
//GLAPI void APIENTRY glBindTextures (GLuint first, GLsizei count, const GLuint *textures);
JNIEXPORT void JNICALL Java_JGL_JGL_glBindTextures( JNIEnv* env, jclass klass,jint first_,jint count_,jintArray textures_){
    if(do_trace)
        fprintf(stderr,"glBindTextures\n");
    static PFNGLBINDTEXTURESPROC impl;
    if( !impl ){
       impl = (PFNGLBINDTEXTURESPROC) mygetprocaddr(env,"glBindTextures");
       if(!impl)
            return;
    }
    jint* _textures_ = (textures_ ? ((*env)->GetIntArrayElements(env,textures_,NULL) ) : NULL);
    impl(first_, count_, _textures_);
    if(textures_)
        (*env)->ReleaseIntArrayElements(env,textures_, _textures_,0);
}
//GLAPI void APIENTRY glBindSamplers (GLuint first, GLsizei count, const GLuint *samplers);
JNIEXPORT void JNICALL Java_JGL_JGL_glBindSamplers( JNIEnv* env, jclass klass,jint first_,jint count_,jintArray samplers_){
    if(do_trace)
        fprintf(stderr,"glBindSamplers\n");
    static PFNGLBINDSAMPLERSPROC impl;
    if( !impl ){
       impl = (PFNGLBINDSAMPLERSPROC) mygetprocaddr(env,"glBindSamplers");
       if(!impl)
            return;
    }
    jint* _samplers_ = (samplers_ ? ((*env)->GetIntArrayElements(env,samplers_,NULL) ) : NULL);
    impl(first_, count_, _samplers_);
    if(samplers_)
        (*env)->ReleaseIntArrayElements(env,samplers_, _samplers_,0);
}
//GLAPI void APIENTRY glBindImageTextures (GLuint first, GLsizei count, const GLuint *textures);
JNIEXPORT void JNICALL Java_JGL_JGL_glBindImageTextures( JNIEnv* env, jclass klass,jint first_,jint count_,jintArray textures_){
    if(do_trace)
        fprintf(stderr,"glBindImageTextures\n");
    static PFNGLBINDIMAGETEXTURESPROC impl;
    if( !impl ){
       impl = (PFNGLBINDIMAGETEXTURESPROC) mygetprocaddr(env,"glBindImageTextures");
       if(!impl)
            return;
    }
    jint* _textures_ = (textures_ ? ((*env)->GetIntArrayElements(env,textures_,NULL) ) : NULL);
    impl(first_, count_, _textures_);
    if(textures_)
        (*env)->ReleaseIntArrayElements(env,textures_, _textures_,0);
}
//GLAPI void APIENTRY glClipControl (GLenum origin, GLenum depth);
JNIEXPORT void JNICALL Java_JGL_JGL_glClipControl( JNIEnv* env, jclass klass,jint origin_,jint depth_){
    if(do_trace)
        fprintf(stderr,"glClipControl\n");
    static PFNGLCLIPCONTROLPROC impl;
    if( !impl ){
       impl = (PFNGLCLIPCONTROLPROC) mygetprocaddr(env,"glClipControl");
       if(!impl)
            return;
    }
    impl(origin_, depth_);
}
//GLAPI void APIENTRY glCreateTransformFeedbacks (GLsizei n, GLuint *ids);
JNIEXPORT void JNICALL Java_JGL_JGL_glCreateTransformFeedbacks( JNIEnv* env, jclass klass,jint n_,jintArray ids_){
    if(do_trace)
        fprintf(stderr,"glCreateTransformFeedbacks\n");
    static PFNGLCREATETRANSFORMFEEDBACKSPROC impl;
    if( !impl ){
       impl = (PFNGLCREATETRANSFORMFEEDBACKSPROC) mygetprocaddr(env,"glCreateTransformFeedbacks");
       if(!impl)
            return;
    }
    jint* _ids_ = (ids_ ? ((*env)->GetIntArrayElements(env,ids_,NULL) ) : NULL);
    impl(n_, _ids_);
    if(ids_)
        (*env)->ReleaseIntArrayElements(env,ids_, _ids_,0);
}
//GLAPI void APIENTRY glTransformFeedbackBufferBase (GLuint xfb, GLuint index, GLuint buffer);
JNIEXPORT void JNICALL Java_JGL_JGL_glTransformFeedbackBufferBase( JNIEnv* env, jclass klass,jint xfb_,jint index_,jint buffer_){
    if(do_trace)
        fprintf(stderr,"glTransformFeedbackBufferBase\n");
    static PFNGLTRANSFORMFEEDBACKBUFFERBASEPROC impl;
    if( !impl ){
       impl = (PFNGLTRANSFORMFEEDBACKBUFFERBASEPROC) mygetprocaddr(env,"glTransformFeedbackBufferBase");
       if(!impl)
            return;
    }
    impl(xfb_, index_, buffer_);
}
//GLAPI void APIENTRY glTransformFeedbackBufferRange (GLuint xfb, GLuint index, GLuint buffer, GLintptr offset, GLsizei size);
JNIEXPORT void JNICALL Java_JGL_JGL_glTransformFeedbackBufferRange( JNIEnv* env, jclass klass,jint xfb_,jint index_,jint buffer_,jint offset_,jint size_){
    if(do_trace)
        fprintf(stderr,"glTransformFeedbackBufferRange\n");
    static PFNGLTRANSFORMFEEDBACKBUFFERRANGEPROC impl;
    if( !impl ){
       impl = (PFNGLTRANSFORMFEEDBACKBUFFERRANGEPROC) mygetprocaddr(env,"glTransformFeedbackBufferRange");
       if(!impl)
            return;
    }
    impl(xfb_, index_, buffer_, offset_, size_);
}
//GLAPI void APIENTRY glGetTransformFeedbackiv (GLuint xfb, GLenum pname, GLint *param);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetTransformFeedbackiv( JNIEnv* env, jclass klass,jint xfb_,jint pname_,jintArray param_){
    if(do_trace)
        fprintf(stderr,"glGetTransformFeedbackiv\n");
    static PFNGLGETTRANSFORMFEEDBACKIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETTRANSFORMFEEDBACKIVPROC) mygetprocaddr(env,"glGetTransformFeedbackiv");
       if(!impl)
            return;
    }
    jint* _param_ = (param_ ? ((*env)->GetIntArrayElements(env,param_,NULL) ) : NULL);
    impl(xfb_, pname_, _param_);
    if(param_)
        (*env)->ReleaseIntArrayElements(env,param_, _param_,0);
}
//GLAPI void APIENTRY glGetTransformFeedbacki_v (GLuint xfb, GLenum pname, GLuint index, GLint *param);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetTransformFeedbacki_v( JNIEnv* env, jclass klass,jint xfb_,jint pname_,jint index_,jintArray param_){
    if(do_trace)
        fprintf(stderr,"glGetTransformFeedbacki_v\n");
    static PFNGLGETTRANSFORMFEEDBACKI_VPROC impl;
    if( !impl ){
       impl = (PFNGLGETTRANSFORMFEEDBACKI_VPROC) mygetprocaddr(env,"glGetTransformFeedbacki_v");
       if(!impl)
            return;
    }
    jint* _param_ = (param_ ? ((*env)->GetIntArrayElements(env,param_,NULL) ) : NULL);
    impl(xfb_, pname_, index_, _param_);
    if(param_)
        (*env)->ReleaseIntArrayElements(env,param_, _param_,0);
}
//GLAPI void APIENTRY glGetTransformFeedbacki64_v (GLuint xfb, GLenum pname, GLuint index, GLint64 *param);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetTransformFeedbacki64_v( JNIEnv* env, jclass klass,jint xfb_,jint pname_,jint index_,jlongArray param_){
    if(do_trace)
        fprintf(stderr,"glGetTransformFeedbacki64_v\n");
    static PFNGLGETTRANSFORMFEEDBACKI64_VPROC impl;
    if( !impl ){
       impl = (PFNGLGETTRANSFORMFEEDBACKI64_VPROC) mygetprocaddr(env,"glGetTransformFeedbacki64_v");
       if(!impl)
            return;
    }
    jlong* _param_ = (param_ ? ((*env)->GetLongArrayElements(env,param_,NULL) ) : NULL);
    impl(xfb_, pname_, index_, _param_);
    if(param_)
        (*env)->ReleaseLongArrayElements(env,param_, _param_,0);
}
//GLAPI void APIENTRY glCreateBuffers (GLsizei n, GLuint *buffers);
JNIEXPORT void JNICALL Java_JGL_JGL_glCreateBuffers( JNIEnv* env, jclass klass,jint n_,jintArray buffers_){
    if(do_trace)
        fprintf(stderr,"glCreateBuffers\n");
    static PFNGLCREATEBUFFERSPROC impl;
    if( !impl ){
       impl = (PFNGLCREATEBUFFERSPROC) mygetprocaddr(env,"glCreateBuffers");
       if(!impl)
            return;
    }
    jint* _buffers_ = (buffers_ ? ((*env)->GetIntArrayElements(env,buffers_,NULL) ) : NULL);
    impl(n_, _buffers_);
    if(buffers_)
        (*env)->ReleaseIntArrayElements(env,buffers_, _buffers_,0);
}
//GLAPI void APIENTRY glNamedBufferStorage (GLuint buffer, GLsizei size, const void *data, GLbitfield flags);
JNIEXPORT void JNICALL Java_JGL_JGL_glNamedBufferStorage( JNIEnv* env, jclass klass,jint buffer_,jint size_,jbyteArray data_,jint flags_){
    if(do_trace)
        fprintf(stderr,"glNamedBufferStorage\n");
    static PFNGLNAMEDBUFFERSTORAGEPROC impl;
    if( !impl ){
       impl = (PFNGLNAMEDBUFFERSTORAGEPROC) mygetprocaddr(env,"glNamedBufferStorage");
       if(!impl)
            return;
    }
    jbyte* _data_ = (data_ ? ((*env)->GetByteArrayElements(env,data_,NULL) ) : NULL);
    impl(buffer_, size_, _data_, flags_);
    if(data_)
        (*env)->ReleaseByteArrayElements(env,data_, _data_,0);
}
//GLAPI void APIENTRY glNamedBufferData (GLuint buffer, GLsizei size, const void *data, GLenum usage);
JNIEXPORT void JNICALL Java_JGL_JGL_glNamedBufferData( JNIEnv* env, jclass klass,jint buffer_,jint size_,jbyteArray data_,jint usage_){
    if(do_trace)
        fprintf(stderr,"glNamedBufferData\n");
    static PFNGLNAMEDBUFFERDATAPROC impl;
    if( !impl ){
       impl = (PFNGLNAMEDBUFFERDATAPROC) mygetprocaddr(env,"glNamedBufferData");
       if(!impl)
            return;
    }
    jbyte* _data_ = (data_ ? ((*env)->GetByteArrayElements(env,data_,NULL) ) : NULL);
    impl(buffer_, size_, _data_, usage_);
    if(data_)
        (*env)->ReleaseByteArrayElements(env,data_, _data_,0);
}
//GLAPI void APIENTRY glNamedBufferSubData (GLuint buffer, GLintptr offset, GLsizei size, const void *data);
JNIEXPORT void JNICALL Java_JGL_JGL_glNamedBufferSubData( JNIEnv* env, jclass klass,jint buffer_,jint offset_,jint size_,jbyteArray data_){
    if(do_trace)
        fprintf(stderr,"glNamedBufferSubData\n");
    static PFNGLNAMEDBUFFERSUBDATAPROC impl;
    if( !impl ){
       impl = (PFNGLNAMEDBUFFERSUBDATAPROC) mygetprocaddr(env,"glNamedBufferSubData");
       if(!impl)
            return;
    }
    jbyte* _data_ = (data_ ? ((*env)->GetByteArrayElements(env,data_,NULL) ) : NULL);
    impl(buffer_, offset_, size_, _data_);
    if(data_)
        (*env)->ReleaseByteArrayElements(env,data_, _data_,0);
}
//GLAPI void APIENTRY glCopyNamedBufferSubData (GLuint readBuffer, GLuint writeBuffer, GLintptr readOffset, GLintptr writeOffset, GLsizei size);
JNIEXPORT void JNICALL Java_JGL_JGL_glCopyNamedBufferSubData( JNIEnv* env, jclass klass,jint readBuffer_,jint writeBuffer_,jint readOffset_,jint writeOffset_,jint size_){
    if(do_trace)
        fprintf(stderr,"glCopyNamedBufferSubData\n");
    static PFNGLCOPYNAMEDBUFFERSUBDATAPROC impl;
    if( !impl ){
       impl = (PFNGLCOPYNAMEDBUFFERSUBDATAPROC) mygetprocaddr(env,"glCopyNamedBufferSubData");
       if(!impl)
            return;
    }
    impl(readBuffer_, writeBuffer_, readOffset_, writeOffset_, size_);
}
//GLAPI void APIENTRY glClearNamedBufferData (GLuint buffer, GLenum internalformat, GLenum format, GLenum type, const void *data);
JNIEXPORT void JNICALL Java_JGL_JGL_glClearNamedBufferData( JNIEnv* env, jclass klass,jint buffer_,jint internalformat_,jint format_,jint type_,jbyteArray data_){
    if(do_trace)
        fprintf(stderr,"glClearNamedBufferData\n");
    static PFNGLCLEARNAMEDBUFFERDATAPROC impl;
    if( !impl ){
       impl = (PFNGLCLEARNAMEDBUFFERDATAPROC) mygetprocaddr(env,"glClearNamedBufferData");
       if(!impl)
            return;
    }
    jbyte* _data_ = (data_ ? ((*env)->GetByteArrayElements(env,data_,NULL) ) : NULL);
    impl(buffer_, internalformat_, format_, type_, _data_);
    if(data_)
        (*env)->ReleaseByteArrayElements(env,data_, _data_,0);
}
//GLAPI void APIENTRY glClearNamedBufferSubData (GLuint buffer, GLenum internalformat, GLintptr offset, GLsizei size, GLenum format, GLenum type, const void *data);
JNIEXPORT void JNICALL Java_JGL_JGL_glClearNamedBufferSubData( JNIEnv* env, jclass klass,jint buffer_,jint internalformat_,jint offset_,jint size_,jint format_,jint type_,jbyteArray data_){
    if(do_trace)
        fprintf(stderr,"glClearNamedBufferSubData\n");
    static PFNGLCLEARNAMEDBUFFERSUBDATAPROC impl;
    if( !impl ){
       impl = (PFNGLCLEARNAMEDBUFFERSUBDATAPROC) mygetprocaddr(env,"glClearNamedBufferSubData");
       if(!impl)
            return;
    }
    jbyte* _data_ = (data_ ? ((*env)->GetByteArrayElements(env,data_,NULL) ) : NULL);
    impl(buffer_, internalformat_, offset_, size_, format_, type_, _data_);
    if(data_)
        (*env)->ReleaseByteArrayElements(env,data_, _data_,0);
}
//GLAPI GLboolean APIENTRY glUnmapNamedBuffer (GLuint buffer);
JNIEXPORT jboolean JNICALL Java_JGL_JGL_glUnmapNamedBuffer( JNIEnv* env, jclass klass,jint buffer_){
    jboolean rv = 0;
    if(do_trace)
        fprintf(stderr,"glUnmapNamedBuffer\n");
    static PFNGLUNMAPNAMEDBUFFERPROC impl;
    if( !impl ){
       impl = (PFNGLUNMAPNAMEDBUFFERPROC) mygetprocaddr(env,"glUnmapNamedBuffer");
       if(!impl)
            return rv;
    }
    rv = impl(buffer_);
    return rv;
}
//GLAPI void APIENTRY glFlushMappedNamedBufferRange (GLuint buffer, GLintptr offset, GLsizei length);
JNIEXPORT void JNICALL Java_JGL_JGL_glFlushMappedNamedBufferRange( JNIEnv* env, jclass klass,jint buffer_,jint offset_,jint length_){
    if(do_trace)
        fprintf(stderr,"glFlushMappedNamedBufferRange\n");
    static PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEPROC impl;
    if( !impl ){
       impl = (PFNGLFLUSHMAPPEDNAMEDBUFFERRANGEPROC) mygetprocaddr(env,"glFlushMappedNamedBufferRange");
       if(!impl)
            return;
    }
    impl(buffer_, offset_, length_);
}
//GLAPI void APIENTRY glGetNamedBufferParameteriv (GLuint buffer, GLenum pname, GLint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetNamedBufferParameteriv( JNIEnv* env, jclass klass,jint buffer_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetNamedBufferParameteriv\n");
    static PFNGLGETNAMEDBUFFERPARAMETERIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETNAMEDBUFFERPARAMETERIVPROC) mygetprocaddr(env,"glGetNamedBufferParameteriv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(buffer_, pname_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glGetNamedBufferParameteri64v (GLuint buffer, GLenum pname, GLint64 *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetNamedBufferParameteri64v( JNIEnv* env, jclass klass,jint buffer_,jint pname_,jlongArray params_){
    if(do_trace)
        fprintf(stderr,"glGetNamedBufferParameteri64v\n");
    static PFNGLGETNAMEDBUFFERPARAMETERI64VPROC impl;
    if( !impl ){
       impl = (PFNGLGETNAMEDBUFFERPARAMETERI64VPROC) mygetprocaddr(env,"glGetNamedBufferParameteri64v");
       if(!impl)
            return;
    }
    jlong* _params_ = (params_ ? ((*env)->GetLongArrayElements(env,params_,NULL) ) : NULL);
    impl(buffer_, pname_, _params_);
    if(params_)
        (*env)->ReleaseLongArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glGetNamedBufferSubData (GLuint buffer, GLintptr offset, GLsizei size, void *data);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetNamedBufferSubData( JNIEnv* env, jclass klass,jint buffer_,jint offset_,jint size_,jbyteArray data_){
    if(do_trace)
        fprintf(stderr,"glGetNamedBufferSubData\n");
    static PFNGLGETNAMEDBUFFERSUBDATAPROC impl;
    if( !impl ){
       impl = (PFNGLGETNAMEDBUFFERSUBDATAPROC) mygetprocaddr(env,"glGetNamedBufferSubData");
       if(!impl)
            return;
    }
    jbyte* _data_ = (data_ ? ((*env)->GetByteArrayElements(env,data_,NULL) ) : NULL);
    impl(buffer_, offset_, size_, _data_);
    if(data_)
        (*env)->ReleaseByteArrayElements(env,data_, _data_,0);
}
//GLAPI void APIENTRY glCreateFramebuffers (GLsizei n, GLuint *framebuffers);
JNIEXPORT void JNICALL Java_JGL_JGL_glCreateFramebuffers( JNIEnv* env, jclass klass,jint n_,jintArray framebuffers_){
    if(do_trace)
        fprintf(stderr,"glCreateFramebuffers\n");
    static PFNGLCREATEFRAMEBUFFERSPROC impl;
    if( !impl ){
       impl = (PFNGLCREATEFRAMEBUFFERSPROC) mygetprocaddr(env,"glCreateFramebuffers");
       if(!impl)
            return;
    }
    jint* _framebuffers_ = (framebuffers_ ? ((*env)->GetIntArrayElements(env,framebuffers_,NULL) ) : NULL);
    impl(n_, _framebuffers_);
    if(framebuffers_)
        (*env)->ReleaseIntArrayElements(env,framebuffers_, _framebuffers_,0);
}
//GLAPI void APIENTRY glNamedFramebufferRenderbuffer (GLuint framebuffer, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
JNIEXPORT void JNICALL Java_JGL_JGL_glNamedFramebufferRenderbuffer( JNIEnv* env, jclass klass,jint framebuffer_,jint attachment_,jint renderbuffertarget_,jint renderbuffer_){
    if(do_trace)
        fprintf(stderr,"glNamedFramebufferRenderbuffer\n");
    static PFNGLNAMEDFRAMEBUFFERRENDERBUFFERPROC impl;
    if( !impl ){
       impl = (PFNGLNAMEDFRAMEBUFFERRENDERBUFFERPROC) mygetprocaddr(env,"glNamedFramebufferRenderbuffer");
       if(!impl)
            return;
    }
    impl(framebuffer_, attachment_, renderbuffertarget_, renderbuffer_);
}
//GLAPI void APIENTRY glNamedFramebufferParameteri (GLuint framebuffer, GLenum pname, GLint param);
JNIEXPORT void JNICALL Java_JGL_JGL_glNamedFramebufferParameteri( JNIEnv* env, jclass klass,jint framebuffer_,jint pname_,jint param_){
    if(do_trace)
        fprintf(stderr,"glNamedFramebufferParameteri\n");
    static PFNGLNAMEDFRAMEBUFFERPARAMETERIPROC impl;
    if( !impl ){
       impl = (PFNGLNAMEDFRAMEBUFFERPARAMETERIPROC) mygetprocaddr(env,"glNamedFramebufferParameteri");
       if(!impl)
            return;
    }
    impl(framebuffer_, pname_, param_);
}
//GLAPI void APIENTRY glNamedFramebufferTexture (GLuint framebuffer, GLenum attachment, GLuint texture, GLint level);
JNIEXPORT void JNICALL Java_JGL_JGL_glNamedFramebufferTexture( JNIEnv* env, jclass klass,jint framebuffer_,jint attachment_,jint texture_,jint level_){
    if(do_trace)
        fprintf(stderr,"glNamedFramebufferTexture\n");
    static PFNGLNAMEDFRAMEBUFFERTEXTUREPROC impl;
    if( !impl ){
       impl = (PFNGLNAMEDFRAMEBUFFERTEXTUREPROC) mygetprocaddr(env,"glNamedFramebufferTexture");
       if(!impl)
            return;
    }
    impl(framebuffer_, attachment_, texture_, level_);
}
//GLAPI void APIENTRY glNamedFramebufferTextureLayer (GLuint framebuffer, GLenum attachment, GLuint texture, GLint level, GLint layer);
JNIEXPORT void JNICALL Java_JGL_JGL_glNamedFramebufferTextureLayer( JNIEnv* env, jclass klass,jint framebuffer_,jint attachment_,jint texture_,jint level_,jint layer_){
    if(do_trace)
        fprintf(stderr,"glNamedFramebufferTextureLayer\n");
    static PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC impl;
    if( !impl ){
       impl = (PFNGLNAMEDFRAMEBUFFERTEXTURELAYERPROC) mygetprocaddr(env,"glNamedFramebufferTextureLayer");
       if(!impl)
            return;
    }
    impl(framebuffer_, attachment_, texture_, level_, layer_);
}
//GLAPI void APIENTRY glNamedFramebufferDrawBuffer (GLuint framebuffer, GLenum buf);
JNIEXPORT void JNICALL Java_JGL_JGL_glNamedFramebufferDrawBuffer( JNIEnv* env, jclass klass,jint framebuffer_,jint buf_){
    if(do_trace)
        fprintf(stderr,"glNamedFramebufferDrawBuffer\n");
    static PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC impl;
    if( !impl ){
       impl = (PFNGLNAMEDFRAMEBUFFERDRAWBUFFERPROC) mygetprocaddr(env,"glNamedFramebufferDrawBuffer");
       if(!impl)
            return;
    }
    impl(framebuffer_, buf_);
}
//GLAPI void APIENTRY glNamedFramebufferDrawBuffers (GLuint framebuffer, GLsizei n, const GLenum *bufs);
JNIEXPORT void JNICALL Java_JGL_JGL_glNamedFramebufferDrawBuffers( JNIEnv* env, jclass klass,jint framebuffer_,jint n_,jintArray bufs_){
    if(do_trace)
        fprintf(stderr,"glNamedFramebufferDrawBuffers\n");
    static PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC impl;
    if( !impl ){
       impl = (PFNGLNAMEDFRAMEBUFFERDRAWBUFFERSPROC) mygetprocaddr(env,"glNamedFramebufferDrawBuffers");
       if(!impl)
            return;
    }
    jint* _bufs_ = (bufs_ ? ((*env)->GetIntArrayElements(env,bufs_,NULL) ) : NULL);
    impl(framebuffer_, n_, _bufs_);
    if(bufs_)
        (*env)->ReleaseIntArrayElements(env,bufs_, _bufs_,0);
}
//GLAPI void APIENTRY glNamedFramebufferReadBuffer (GLuint framebuffer, GLenum src);
JNIEXPORT void JNICALL Java_JGL_JGL_glNamedFramebufferReadBuffer( JNIEnv* env, jclass klass,jint framebuffer_,jint src_){
    if(do_trace)
        fprintf(stderr,"glNamedFramebufferReadBuffer\n");
    static PFNGLNAMEDFRAMEBUFFERREADBUFFERPROC impl;
    if( !impl ){
       impl = (PFNGLNAMEDFRAMEBUFFERREADBUFFERPROC) mygetprocaddr(env,"glNamedFramebufferReadBuffer");
       if(!impl)
            return;
    }
    impl(framebuffer_, src_);
}
//GLAPI void APIENTRY glInvalidateNamedFramebufferData (GLuint framebuffer, GLsizei numAttachments, const GLenum *attachments);
JNIEXPORT void JNICALL Java_JGL_JGL_glInvalidateNamedFramebufferData( JNIEnv* env, jclass klass,jint framebuffer_,jint numAttachments_,jintArray attachments_){
    if(do_trace)
        fprintf(stderr,"glInvalidateNamedFramebufferData\n");
    static PFNGLINVALIDATENAMEDFRAMEBUFFERDATAPROC impl;
    if( !impl ){
       impl = (PFNGLINVALIDATENAMEDFRAMEBUFFERDATAPROC) mygetprocaddr(env,"glInvalidateNamedFramebufferData");
       if(!impl)
            return;
    }
    jint* _attachments_ = (attachments_ ? ((*env)->GetIntArrayElements(env,attachments_,NULL) ) : NULL);
    impl(framebuffer_, numAttachments_, _attachments_);
    if(attachments_)
        (*env)->ReleaseIntArrayElements(env,attachments_, _attachments_,0);
}
//GLAPI void APIENTRY glInvalidateNamedFramebufferSubData (GLuint framebuffer, GLsizei numAttachments, const GLenum *attachments, GLint x, GLint y, GLsizei width, GLsizei height);
JNIEXPORT void JNICALL Java_JGL_JGL_glInvalidateNamedFramebufferSubData( JNIEnv* env, jclass klass,jint framebuffer_,jint numAttachments_,jintArray attachments_,jint x_,jint y_,jint width_,jint height_){
    if(do_trace)
        fprintf(stderr,"glInvalidateNamedFramebufferSubData\n");
    static PFNGLINVALIDATENAMEDFRAMEBUFFERSUBDATAPROC impl;
    if( !impl ){
       impl = (PFNGLINVALIDATENAMEDFRAMEBUFFERSUBDATAPROC) mygetprocaddr(env,"glInvalidateNamedFramebufferSubData");
       if(!impl)
            return;
    }
    jint* _attachments_ = (attachments_ ? ((*env)->GetIntArrayElements(env,attachments_,NULL) ) : NULL);
    impl(framebuffer_, numAttachments_, _attachments_, x_, y_, width_, height_);
    if(attachments_)
        (*env)->ReleaseIntArrayElements(env,attachments_, _attachments_,0);
}
//GLAPI void APIENTRY glClearNamedFramebufferiv (GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLint *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glClearNamedFramebufferiv( JNIEnv* env, jclass klass,jint framebuffer_,jint buffer_,jint drawbuffer_,jintArray value_){
    if(do_trace)
        fprintf(stderr,"glClearNamedFramebufferiv\n");
    static PFNGLCLEARNAMEDFRAMEBUFFERIVPROC impl;
    if( !impl ){
       impl = (PFNGLCLEARNAMEDFRAMEBUFFERIVPROC) mygetprocaddr(env,"glClearNamedFramebufferiv");
       if(!impl)
            return;
    }
    jint* _value_ = (value_ ? ((*env)->GetIntArrayElements(env,value_,NULL) ) : NULL);
    impl(framebuffer_, buffer_, drawbuffer_, _value_);
    if(value_)
        (*env)->ReleaseIntArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glClearNamedFramebufferuiv (GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLuint *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glClearNamedFramebufferuiv( JNIEnv* env, jclass klass,jint framebuffer_,jint buffer_,jint drawbuffer_,jintArray value_){
    if(do_trace)
        fprintf(stderr,"glClearNamedFramebufferuiv\n");
    static PFNGLCLEARNAMEDFRAMEBUFFERUIVPROC impl;
    if( !impl ){
       impl = (PFNGLCLEARNAMEDFRAMEBUFFERUIVPROC) mygetprocaddr(env,"glClearNamedFramebufferuiv");
       if(!impl)
            return;
    }
    jint* _value_ = (value_ ? ((*env)->GetIntArrayElements(env,value_,NULL) ) : NULL);
    impl(framebuffer_, buffer_, drawbuffer_, _value_);
    if(value_)
        (*env)->ReleaseIntArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glClearNamedFramebufferfv (GLuint framebuffer, GLenum buffer, GLint drawbuffer, const GLfloat *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glClearNamedFramebufferfv( JNIEnv* env, jclass klass,jint framebuffer_,jint buffer_,jint drawbuffer_,jfloatArray value_){
    if(do_trace)
        fprintf(stderr,"glClearNamedFramebufferfv\n");
    static PFNGLCLEARNAMEDFRAMEBUFFERFVPROC impl;
    if( !impl ){
       impl = (PFNGLCLEARNAMEDFRAMEBUFFERFVPROC) mygetprocaddr(env,"glClearNamedFramebufferfv");
       if(!impl)
            return;
    }
    jfloat* _value_ = (value_ ? ((*env)->GetFloatArrayElements(env,value_,NULL) ) : NULL);
    impl(framebuffer_, buffer_, drawbuffer_, _value_);
    if(value_)
        (*env)->ReleaseFloatArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glClearNamedFramebufferfi (GLuint framebuffer, GLenum buffer, const GLfloat depth, GLint stencil);
JNIEXPORT void JNICALL Java_JGL_JGL_glClearNamedFramebufferfi( JNIEnv* env, jclass klass,jint framebuffer_,jint buffer_,jfloat depth_,jint stencil_){
    if(do_trace)
        fprintf(stderr,"glClearNamedFramebufferfi\n");
    static PFNGLCLEARNAMEDFRAMEBUFFERFIPROC impl;
    if( !impl ){
       impl = (PFNGLCLEARNAMEDFRAMEBUFFERFIPROC) mygetprocaddr(env,"glClearNamedFramebufferfi");
       if(!impl)
            return;
    }
    impl(framebuffer_, buffer_, depth_, stencil_);
}
//GLAPI void APIENTRY glBlitNamedFramebuffer (GLuint readFramebuffer, GLuint drawFramebuffer, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
JNIEXPORT void JNICALL Java_JGL_JGL_glBlitNamedFramebuffer( JNIEnv* env, jclass klass,jint readFramebuffer_,jint drawFramebuffer_,jint srcX0_,jint srcY0_,jint srcX1_,jint srcY1_,jint dstX0_,jint dstY0_,jint dstX1_,jint dstY1_,jint mask_,jint filter_){
    if(do_trace)
        fprintf(stderr,"glBlitNamedFramebuffer\n");
    static PFNGLBLITNAMEDFRAMEBUFFERPROC impl;
    if( !impl ){
       impl = (PFNGLBLITNAMEDFRAMEBUFFERPROC) mygetprocaddr(env,"glBlitNamedFramebuffer");
       if(!impl)
            return;
    }
    impl(readFramebuffer_, drawFramebuffer_, srcX0_, srcY0_, srcX1_, srcY1_, dstX0_, dstY0_, dstX1_, dstY1_, mask_, filter_);
}
//GLAPI GLenum APIENTRY glCheckNamedFramebufferStatus (GLuint framebuffer, GLenum target);
JNIEXPORT jint JNICALL Java_JGL_JGL_glCheckNamedFramebufferStatus( JNIEnv* env, jclass klass,jint framebuffer_,jint target_){
    jint rv = 0;
    if(do_trace)
        fprintf(stderr,"glCheckNamedFramebufferStatus\n");
    static PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC impl;
    if( !impl ){
       impl = (PFNGLCHECKNAMEDFRAMEBUFFERSTATUSPROC) mygetprocaddr(env,"glCheckNamedFramebufferStatus");
       if(!impl)
            return rv;
    }
    rv = impl(framebuffer_, target_);
    return rv;
}
//GLAPI void APIENTRY glGetNamedFramebufferParameteriv (GLuint framebuffer, GLenum pname, GLint *param);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetNamedFramebufferParameteriv( JNIEnv* env, jclass klass,jint framebuffer_,jint pname_,jintArray param_){
    if(do_trace)
        fprintf(stderr,"glGetNamedFramebufferParameteriv\n");
    static PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETNAMEDFRAMEBUFFERPARAMETERIVPROC) mygetprocaddr(env,"glGetNamedFramebufferParameteriv");
       if(!impl)
            return;
    }
    jint* _param_ = (param_ ? ((*env)->GetIntArrayElements(env,param_,NULL) ) : NULL);
    impl(framebuffer_, pname_, _param_);
    if(param_)
        (*env)->ReleaseIntArrayElements(env,param_, _param_,0);
}
//GLAPI void APIENTRY glGetNamedFramebufferAttachmentParameteriv (GLuint framebuffer, GLenum attachment, GLenum pname, GLint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetNamedFramebufferAttachmentParameteriv( JNIEnv* env, jclass klass,jint framebuffer_,jint attachment_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetNamedFramebufferAttachmentParameteriv\n");
    static PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVPROC) mygetprocaddr(env,"glGetNamedFramebufferAttachmentParameteriv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(framebuffer_, attachment_, pname_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glCreateRenderbuffers (GLsizei n, GLuint *renderbuffers);
JNIEXPORT void JNICALL Java_JGL_JGL_glCreateRenderbuffers( JNIEnv* env, jclass klass,jint n_,jintArray renderbuffers_){
    if(do_trace)
        fprintf(stderr,"glCreateRenderbuffers\n");
    static PFNGLCREATERENDERBUFFERSPROC impl;
    if( !impl ){
       impl = (PFNGLCREATERENDERBUFFERSPROC) mygetprocaddr(env,"glCreateRenderbuffers");
       if(!impl)
            return;
    }
    jint* _renderbuffers_ = (renderbuffers_ ? ((*env)->GetIntArrayElements(env,renderbuffers_,NULL) ) : NULL);
    impl(n_, _renderbuffers_);
    if(renderbuffers_)
        (*env)->ReleaseIntArrayElements(env,renderbuffers_, _renderbuffers_,0);
}
//GLAPI void APIENTRY glNamedRenderbufferStorage (GLuint renderbuffer, GLenum internalformat, GLsizei width, GLsizei height);
JNIEXPORT void JNICALL Java_JGL_JGL_glNamedRenderbufferStorage( JNIEnv* env, jclass klass,jint renderbuffer_,jint internalformat_,jint width_,jint height_){
    if(do_trace)
        fprintf(stderr,"glNamedRenderbufferStorage\n");
    static PFNGLNAMEDRENDERBUFFERSTORAGEPROC impl;
    if( !impl ){
       impl = (PFNGLNAMEDRENDERBUFFERSTORAGEPROC) mygetprocaddr(env,"glNamedRenderbufferStorage");
       if(!impl)
            return;
    }
    impl(renderbuffer_, internalformat_, width_, height_);
}
//GLAPI void APIENTRY glNamedRenderbufferStorageMultisample (GLuint renderbuffer, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
JNIEXPORT void JNICALL Java_JGL_JGL_glNamedRenderbufferStorageMultisample( JNIEnv* env, jclass klass,jint renderbuffer_,jint samples_,jint internalformat_,jint width_,jint height_){
    if(do_trace)
        fprintf(stderr,"glNamedRenderbufferStorageMultisample\n");
    static PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEPROC impl;
    if( !impl ){
       impl = (PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEPROC) mygetprocaddr(env,"glNamedRenderbufferStorageMultisample");
       if(!impl)
            return;
    }
    impl(renderbuffer_, samples_, internalformat_, width_, height_);
}
//GLAPI void APIENTRY glGetNamedRenderbufferParameteriv (GLuint renderbuffer, GLenum pname, GLint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetNamedRenderbufferParameteriv( JNIEnv* env, jclass klass,jint renderbuffer_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetNamedRenderbufferParameteriv\n");
    static PFNGLGETNAMEDRENDERBUFFERPARAMETERIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETNAMEDRENDERBUFFERPARAMETERIVPROC) mygetprocaddr(env,"glGetNamedRenderbufferParameteriv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(renderbuffer_, pname_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glCreateTextures (GLenum target, GLsizei n, GLuint *textures);
JNIEXPORT void JNICALL Java_JGL_JGL_glCreateTextures( JNIEnv* env, jclass klass,jint target_,jint n_,jintArray textures_){
    if(do_trace)
        fprintf(stderr,"glCreateTextures\n");
    static PFNGLCREATETEXTURESPROC impl;
    if( !impl ){
       impl = (PFNGLCREATETEXTURESPROC) mygetprocaddr(env,"glCreateTextures");
       if(!impl)
            return;
    }
    jint* _textures_ = (textures_ ? ((*env)->GetIntArrayElements(env,textures_,NULL) ) : NULL);
    impl(target_, n_, _textures_);
    if(textures_)
        (*env)->ReleaseIntArrayElements(env,textures_, _textures_,0);
}
//GLAPI void APIENTRY glTextureBuffer (GLuint texture, GLenum internalformat, GLuint buffer);
JNIEXPORT void JNICALL Java_JGL_JGL_glTextureBuffer( JNIEnv* env, jclass klass,jint texture_,jint internalformat_,jint buffer_){
    if(do_trace)
        fprintf(stderr,"glTextureBuffer\n");
    static PFNGLTEXTUREBUFFERPROC impl;
    if( !impl ){
       impl = (PFNGLTEXTUREBUFFERPROC) mygetprocaddr(env,"glTextureBuffer");
       if(!impl)
            return;
    }
    impl(texture_, internalformat_, buffer_);
}
//GLAPI void APIENTRY glTextureBufferRange (GLuint texture, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizei size);
JNIEXPORT void JNICALL Java_JGL_JGL_glTextureBufferRange( JNIEnv* env, jclass klass,jint texture_,jint internalformat_,jint buffer_,jint offset_,jint size_){
    if(do_trace)
        fprintf(stderr,"glTextureBufferRange\n");
    static PFNGLTEXTUREBUFFERRANGEPROC impl;
    if( !impl ){
       impl = (PFNGLTEXTUREBUFFERRANGEPROC) mygetprocaddr(env,"glTextureBufferRange");
       if(!impl)
            return;
    }
    impl(texture_, internalformat_, buffer_, offset_, size_);
}
//GLAPI void APIENTRY glTextureStorage1D (GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width);
JNIEXPORT void JNICALL Java_JGL_JGL_glTextureStorage1D( JNIEnv* env, jclass klass,jint texture_,jint levels_,jint internalformat_,jint width_){
    if(do_trace)
        fprintf(stderr,"glTextureStorage1D\n");
    static PFNGLTEXTURESTORAGE1DPROC impl;
    if( !impl ){
       impl = (PFNGLTEXTURESTORAGE1DPROC) mygetprocaddr(env,"glTextureStorage1D");
       if(!impl)
            return;
    }
    impl(texture_, levels_, internalformat_, width_);
}
//GLAPI void APIENTRY glTextureStorage2D (GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
JNIEXPORT void JNICALL Java_JGL_JGL_glTextureStorage2D( JNIEnv* env, jclass klass,jint texture_,jint levels_,jint internalformat_,jint width_,jint height_){
    if(do_trace)
        fprintf(stderr,"glTextureStorage2D\n");
    static PFNGLTEXTURESTORAGE2DPROC impl;
    if( !impl ){
       impl = (PFNGLTEXTURESTORAGE2DPROC) mygetprocaddr(env,"glTextureStorage2D");
       if(!impl)
            return;
    }
    impl(texture_, levels_, internalformat_, width_, height_);
}
//GLAPI void APIENTRY glTextureStorage3D (GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);
JNIEXPORT void JNICALL Java_JGL_JGL_glTextureStorage3D( JNIEnv* env, jclass klass,jint texture_,jint levels_,jint internalformat_,jint width_,jint height_,jint depth_){
    if(do_trace)
        fprintf(stderr,"glTextureStorage3D\n");
    static PFNGLTEXTURESTORAGE3DPROC impl;
    if( !impl ){
       impl = (PFNGLTEXTURESTORAGE3DPROC) mygetprocaddr(env,"glTextureStorage3D");
       if(!impl)
            return;
    }
    impl(texture_, levels_, internalformat_, width_, height_, depth_);
}
//GLAPI void APIENTRY glTextureStorage2DMultisample (GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
JNIEXPORT void JNICALL Java_JGL_JGL_glTextureStorage2DMultisample( JNIEnv* env, jclass klass,jint texture_,jint samples_,jint internalformat_,jint width_,jint height_,jboolean fixedsamplelocations_){
    if(do_trace)
        fprintf(stderr,"glTextureStorage2DMultisample\n");
    static PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC impl;
    if( !impl ){
       impl = (PFNGLTEXTURESTORAGE2DMULTISAMPLEPROC) mygetprocaddr(env,"glTextureStorage2DMultisample");
       if(!impl)
            return;
    }
    impl(texture_, samples_, internalformat_, width_, height_, fixedsamplelocations_);
}
//GLAPI void APIENTRY glTextureStorage3DMultisample (GLuint texture, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
JNIEXPORT void JNICALL Java_JGL_JGL_glTextureStorage3DMultisample( JNIEnv* env, jclass klass,jint texture_,jint samples_,jint internalformat_,jint width_,jint height_,jint depth_,jboolean fixedsamplelocations_){
    if(do_trace)
        fprintf(stderr,"glTextureStorage3DMultisample\n");
    static PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC impl;
    if( !impl ){
       impl = (PFNGLTEXTURESTORAGE3DMULTISAMPLEPROC) mygetprocaddr(env,"glTextureStorage3DMultisample");
       if(!impl)
            return;
    }
    impl(texture_, samples_, internalformat_, width_, height_, depth_, fixedsamplelocations_);
}
//GLAPI void APIENTRY glTextureSubImage1D (GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void *pixels);
JNIEXPORT void JNICALL Java_JGL_JGL_glTextureSubImage1D( JNIEnv* env, jclass klass,jint texture_,jint level_,jint xoffset_,jint width_,jint format_,jint type_,jbyteArray pixels_){
    if(do_trace)
        fprintf(stderr,"glTextureSubImage1D\n");
    static PFNGLTEXTURESUBIMAGE1DPROC impl;
    if( !impl ){
       impl = (PFNGLTEXTURESUBIMAGE1DPROC) mygetprocaddr(env,"glTextureSubImage1D");
       if(!impl)
            return;
    }
    jbyte* _pixels_ = (pixels_ ? ((*env)->GetByteArrayElements(env,pixels_,NULL) ) : NULL);
    impl(texture_, level_, xoffset_, width_, format_, type_, _pixels_);
    if(pixels_)
        (*env)->ReleaseByteArrayElements(env,pixels_, _pixels_,0);
}
//GLAPI void APIENTRY glTextureSubImage2D (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
JNIEXPORT void JNICALL Java_JGL_JGL_glTextureSubImage2D( JNIEnv* env, jclass klass,jint texture_,jint level_,jint xoffset_,jint yoffset_,jint width_,jint height_,jint format_,jint type_,jbyteArray pixels_){
    if(do_trace)
        fprintf(stderr,"glTextureSubImage2D\n");
    static PFNGLTEXTURESUBIMAGE2DPROC impl;
    if( !impl ){
       impl = (PFNGLTEXTURESUBIMAGE2DPROC) mygetprocaddr(env,"glTextureSubImage2D");
       if(!impl)
            return;
    }
    jbyte* _pixels_ = (pixels_ ? ((*env)->GetByteArrayElements(env,pixels_,NULL) ) : NULL);
    impl(texture_, level_, xoffset_, yoffset_, width_, height_, format_, type_, _pixels_);
    if(pixels_)
        (*env)->ReleaseByteArrayElements(env,pixels_, _pixels_,0);
}
//GLAPI void APIENTRY glTextureSubImage3D (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void *pixels);
JNIEXPORT void JNICALL Java_JGL_JGL_glTextureSubImage3D( JNIEnv* env, jclass klass,jint texture_,jint level_,jint xoffset_,jint yoffset_,jint zoffset_,jint width_,jint height_,jint depth_,jint format_,jint type_,jbyteArray pixels_){
    if(do_trace)
        fprintf(stderr,"glTextureSubImage3D\n");
    static PFNGLTEXTURESUBIMAGE3DPROC impl;
    if( !impl ){
       impl = (PFNGLTEXTURESUBIMAGE3DPROC) mygetprocaddr(env,"glTextureSubImage3D");
       if(!impl)
            return;
    }
    jbyte* _pixels_ = (pixels_ ? ((*env)->GetByteArrayElements(env,pixels_,NULL) ) : NULL);
    impl(texture_, level_, xoffset_, yoffset_, zoffset_, width_, height_, depth_, format_, type_, _pixels_);
    if(pixels_)
        (*env)->ReleaseByteArrayElements(env,pixels_, _pixels_,0);
}
//GLAPI void APIENTRY glCompressedTextureSubImage1D (GLuint texture, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void *data);
JNIEXPORT void JNICALL Java_JGL_JGL_glCompressedTextureSubImage1D( JNIEnv* env, jclass klass,jint texture_,jint level_,jint xoffset_,jint width_,jint format_,jint imageSize_,jbyteArray data_){
    if(do_trace)
        fprintf(stderr,"glCompressedTextureSubImage1D\n");
    static PFNGLCOMPRESSEDTEXTURESUBIMAGE1DPROC impl;
    if( !impl ){
       impl = (PFNGLCOMPRESSEDTEXTURESUBIMAGE1DPROC) mygetprocaddr(env,"glCompressedTextureSubImage1D");
       if(!impl)
            return;
    }
    jbyte* _data_ = (data_ ? ((*env)->GetByteArrayElements(env,data_,NULL) ) : NULL);
    impl(texture_, level_, xoffset_, width_, format_, imageSize_, _data_);
    if(data_)
        (*env)->ReleaseByteArrayElements(env,data_, _data_,0);
}
//GLAPI void APIENTRY glCompressedTextureSubImage2D (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void *data);
JNIEXPORT void JNICALL Java_JGL_JGL_glCompressedTextureSubImage2D( JNIEnv* env, jclass klass,jint texture_,jint level_,jint xoffset_,jint yoffset_,jint width_,jint height_,jint format_,jint imageSize_,jbyteArray data_){
    if(do_trace)
        fprintf(stderr,"glCompressedTextureSubImage2D\n");
    static PFNGLCOMPRESSEDTEXTURESUBIMAGE2DPROC impl;
    if( !impl ){
       impl = (PFNGLCOMPRESSEDTEXTURESUBIMAGE2DPROC) mygetprocaddr(env,"glCompressedTextureSubImage2D");
       if(!impl)
            return;
    }
    jbyte* _data_ = (data_ ? ((*env)->GetByteArrayElements(env,data_,NULL) ) : NULL);
    impl(texture_, level_, xoffset_, yoffset_, width_, height_, format_, imageSize_, _data_);
    if(data_)
        (*env)->ReleaseByteArrayElements(env,data_, _data_,0);
}
//GLAPI void APIENTRY glCompressedTextureSubImage3D (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void *data);
JNIEXPORT void JNICALL Java_JGL_JGL_glCompressedTextureSubImage3D( JNIEnv* env, jclass klass,jint texture_,jint level_,jint xoffset_,jint yoffset_,jint zoffset_,jint width_,jint height_,jint depth_,jint format_,jint imageSize_,jbyteArray data_){
    if(do_trace)
        fprintf(stderr,"glCompressedTextureSubImage3D\n");
    static PFNGLCOMPRESSEDTEXTURESUBIMAGE3DPROC impl;
    if( !impl ){
       impl = (PFNGLCOMPRESSEDTEXTURESUBIMAGE3DPROC) mygetprocaddr(env,"glCompressedTextureSubImage3D");
       if(!impl)
            return;
    }
    jbyte* _data_ = (data_ ? ((*env)->GetByteArrayElements(env,data_,NULL) ) : NULL);
    impl(texture_, level_, xoffset_, yoffset_, zoffset_, width_, height_, depth_, format_, imageSize_, _data_);
    if(data_)
        (*env)->ReleaseByteArrayElements(env,data_, _data_,0);
}
//GLAPI void APIENTRY glCopyTextureSubImage1D (GLuint texture, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
JNIEXPORT void JNICALL Java_JGL_JGL_glCopyTextureSubImage1D( JNIEnv* env, jclass klass,jint texture_,jint level_,jint xoffset_,jint x_,jint y_,jint width_){
    if(do_trace)
        fprintf(stderr,"glCopyTextureSubImage1D\n");
    static PFNGLCOPYTEXTURESUBIMAGE1DPROC impl;
    if( !impl ){
       impl = (PFNGLCOPYTEXTURESUBIMAGE1DPROC) mygetprocaddr(env,"glCopyTextureSubImage1D");
       if(!impl)
            return;
    }
    impl(texture_, level_, xoffset_, x_, y_, width_);
}
//GLAPI void APIENTRY glCopyTextureSubImage2D (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
JNIEXPORT void JNICALL Java_JGL_JGL_glCopyTextureSubImage2D( JNIEnv* env, jclass klass,jint texture_,jint level_,jint xoffset_,jint yoffset_,jint x_,jint y_,jint width_,jint height_){
    if(do_trace)
        fprintf(stderr,"glCopyTextureSubImage2D\n");
    static PFNGLCOPYTEXTURESUBIMAGE2DPROC impl;
    if( !impl ){
       impl = (PFNGLCOPYTEXTURESUBIMAGE2DPROC) mygetprocaddr(env,"glCopyTextureSubImage2D");
       if(!impl)
            return;
    }
    impl(texture_, level_, xoffset_, yoffset_, x_, y_, width_, height_);
}
//GLAPI void APIENTRY glCopyTextureSubImage3D (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
JNIEXPORT void JNICALL Java_JGL_JGL_glCopyTextureSubImage3D( JNIEnv* env, jclass klass,jint texture_,jint level_,jint xoffset_,jint yoffset_,jint zoffset_,jint x_,jint y_,jint width_,jint height_){
    if(do_trace)
        fprintf(stderr,"glCopyTextureSubImage3D\n");
    static PFNGLCOPYTEXTURESUBIMAGE3DPROC impl;
    if( !impl ){
       impl = (PFNGLCOPYTEXTURESUBIMAGE3DPROC) mygetprocaddr(env,"glCopyTextureSubImage3D");
       if(!impl)
            return;
    }
    impl(texture_, level_, xoffset_, yoffset_, zoffset_, x_, y_, width_, height_);
}
//GLAPI void APIENTRY glTextureParameterf (GLuint texture, GLenum pname, GLfloat param);
JNIEXPORT void JNICALL Java_JGL_JGL_glTextureParameterf( JNIEnv* env, jclass klass,jint texture_,jint pname_,jfloat param_){
    if(do_trace)
        fprintf(stderr,"glTextureParameterf\n");
    static PFNGLTEXTUREPARAMETERFPROC impl;
    if( !impl ){
       impl = (PFNGLTEXTUREPARAMETERFPROC) mygetprocaddr(env,"glTextureParameterf");
       if(!impl)
            return;
    }
    impl(texture_, pname_, param_);
}
//GLAPI void APIENTRY glTextureParameterfv (GLuint texture, GLenum pname, const GLfloat *param);
JNIEXPORT void JNICALL Java_JGL_JGL_glTextureParameterfv( JNIEnv* env, jclass klass,jint texture_,jint pname_,jfloatArray param_){
    if(do_trace)
        fprintf(stderr,"glTextureParameterfv\n");
    static PFNGLTEXTUREPARAMETERFVPROC impl;
    if( !impl ){
       impl = (PFNGLTEXTUREPARAMETERFVPROC) mygetprocaddr(env,"glTextureParameterfv");
       if(!impl)
            return;
    }
    jfloat* _param_ = (param_ ? ((*env)->GetFloatArrayElements(env,param_,NULL) ) : NULL);
    impl(texture_, pname_, _param_);
    if(param_)
        (*env)->ReleaseFloatArrayElements(env,param_, _param_,0);
}
//GLAPI void APIENTRY glTextureParameteri (GLuint texture, GLenum pname, GLint param);
JNIEXPORT void JNICALL Java_JGL_JGL_glTextureParameteri( JNIEnv* env, jclass klass,jint texture_,jint pname_,jint param_){
    if(do_trace)
        fprintf(stderr,"glTextureParameteri\n");
    static PFNGLTEXTUREPARAMETERIPROC impl;
    if( !impl ){
       impl = (PFNGLTEXTUREPARAMETERIPROC) mygetprocaddr(env,"glTextureParameteri");
       if(!impl)
            return;
    }
    impl(texture_, pname_, param_);
}
//GLAPI void APIENTRY glTextureParameterIiv (GLuint texture, GLenum pname, const GLint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glTextureParameterIiv( JNIEnv* env, jclass klass,jint texture_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glTextureParameterIiv\n");
    static PFNGLTEXTUREPARAMETERIIVPROC impl;
    if( !impl ){
       impl = (PFNGLTEXTUREPARAMETERIIVPROC) mygetprocaddr(env,"glTextureParameterIiv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(texture_, pname_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glTextureParameterIuiv (GLuint texture, GLenum pname, const GLuint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glTextureParameterIuiv( JNIEnv* env, jclass klass,jint texture_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glTextureParameterIuiv\n");
    static PFNGLTEXTUREPARAMETERIUIVPROC impl;
    if( !impl ){
       impl = (PFNGLTEXTUREPARAMETERIUIVPROC) mygetprocaddr(env,"glTextureParameterIuiv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(texture_, pname_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glTextureParameteriv (GLuint texture, GLenum pname, const GLint *param);
JNIEXPORT void JNICALL Java_JGL_JGL_glTextureParameteriv( JNIEnv* env, jclass klass,jint texture_,jint pname_,jintArray param_){
    if(do_trace)
        fprintf(stderr,"glTextureParameteriv\n");
    static PFNGLTEXTUREPARAMETERIVPROC impl;
    if( !impl ){
       impl = (PFNGLTEXTUREPARAMETERIVPROC) mygetprocaddr(env,"glTextureParameteriv");
       if(!impl)
            return;
    }
    jint* _param_ = (param_ ? ((*env)->GetIntArrayElements(env,param_,NULL) ) : NULL);
    impl(texture_, pname_, _param_);
    if(param_)
        (*env)->ReleaseIntArrayElements(env,param_, _param_,0);
}
//GLAPI void APIENTRY glGenerateTextureMipmap (GLuint texture);
JNIEXPORT void JNICALL Java_JGL_JGL_glGenerateTextureMipmap( JNIEnv* env, jclass klass,jint texture_){
    if(do_trace)
        fprintf(stderr,"glGenerateTextureMipmap\n");
    static PFNGLGENERATETEXTUREMIPMAPPROC impl;
    if( !impl ){
       impl = (PFNGLGENERATETEXTUREMIPMAPPROC) mygetprocaddr(env,"glGenerateTextureMipmap");
       if(!impl)
            return;
    }
    impl(texture_);
}
//GLAPI void APIENTRY glBindTextureUnit (GLuint unit, GLuint texture);
JNIEXPORT void JNICALL Java_JGL_JGL_glBindTextureUnit( JNIEnv* env, jclass klass,jint unit_,jint texture_){
    if(do_trace)
        fprintf(stderr,"glBindTextureUnit\n");
    static PFNGLBINDTEXTUREUNITPROC impl;
    if( !impl ){
       impl = (PFNGLBINDTEXTUREUNITPROC) mygetprocaddr(env,"glBindTextureUnit");
       if(!impl)
            return;
    }
    impl(unit_, texture_);
}
//GLAPI void APIENTRY glGetTextureImage (GLuint texture, GLint level, GLenum format, GLenum type, GLsizei bufSize, void *pixels);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetTextureImage( JNIEnv* env, jclass klass,jint texture_,jint level_,jint format_,jint type_,jint bufSize_,jbyteArray pixels_){
    if(do_trace)
        fprintf(stderr,"glGetTextureImage\n");
    static PFNGLGETTEXTUREIMAGEPROC impl;
    if( !impl ){
       impl = (PFNGLGETTEXTUREIMAGEPROC) mygetprocaddr(env,"glGetTextureImage");
       if(!impl)
            return;
    }
    jbyte* _pixels_ = (pixels_ ? ((*env)->GetByteArrayElements(env,pixels_,NULL) ) : NULL);
    impl(texture_, level_, format_, type_, bufSize_, _pixels_);
    if(pixels_)
        (*env)->ReleaseByteArrayElements(env,pixels_, _pixels_,0);
}
//GLAPI void APIENTRY glGetCompressedTextureImage (GLuint texture, GLint level, GLsizei bufSize, void *pixels);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetCompressedTextureImage( JNIEnv* env, jclass klass,jint texture_,jint level_,jint bufSize_,jbyteArray pixels_){
    if(do_trace)
        fprintf(stderr,"glGetCompressedTextureImage\n");
    static PFNGLGETCOMPRESSEDTEXTUREIMAGEPROC impl;
    if( !impl ){
       impl = (PFNGLGETCOMPRESSEDTEXTUREIMAGEPROC) mygetprocaddr(env,"glGetCompressedTextureImage");
       if(!impl)
            return;
    }
    jbyte* _pixels_ = (pixels_ ? ((*env)->GetByteArrayElements(env,pixels_,NULL) ) : NULL);
    impl(texture_, level_, bufSize_, _pixels_);
    if(pixels_)
        (*env)->ReleaseByteArrayElements(env,pixels_, _pixels_,0);
}
//GLAPI void APIENTRY glGetTextureLevelParameterfv (GLuint texture, GLint level, GLenum pname, GLfloat *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetTextureLevelParameterfv( JNIEnv* env, jclass klass,jint texture_,jint level_,jint pname_,jfloatArray params_){
    if(do_trace)
        fprintf(stderr,"glGetTextureLevelParameterfv\n");
    static PFNGLGETTEXTURELEVELPARAMETERFVPROC impl;
    if( !impl ){
       impl = (PFNGLGETTEXTURELEVELPARAMETERFVPROC) mygetprocaddr(env,"glGetTextureLevelParameterfv");
       if(!impl)
            return;
    }
    jfloat* _params_ = (params_ ? ((*env)->GetFloatArrayElements(env,params_,NULL) ) : NULL);
    impl(texture_, level_, pname_, _params_);
    if(params_)
        (*env)->ReleaseFloatArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glGetTextureLevelParameteriv (GLuint texture, GLint level, GLenum pname, GLint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetTextureLevelParameteriv( JNIEnv* env, jclass klass,jint texture_,jint level_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetTextureLevelParameteriv\n");
    static PFNGLGETTEXTURELEVELPARAMETERIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETTEXTURELEVELPARAMETERIVPROC) mygetprocaddr(env,"glGetTextureLevelParameteriv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(texture_, level_, pname_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glGetTextureParameterfv (GLuint texture, GLenum pname, GLfloat *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetTextureParameterfv( JNIEnv* env, jclass klass,jint texture_,jint pname_,jfloatArray params_){
    if(do_trace)
        fprintf(stderr,"glGetTextureParameterfv\n");
    static PFNGLGETTEXTUREPARAMETERFVPROC impl;
    if( !impl ){
       impl = (PFNGLGETTEXTUREPARAMETERFVPROC) mygetprocaddr(env,"glGetTextureParameterfv");
       if(!impl)
            return;
    }
    jfloat* _params_ = (params_ ? ((*env)->GetFloatArrayElements(env,params_,NULL) ) : NULL);
    impl(texture_, pname_, _params_);
    if(params_)
        (*env)->ReleaseFloatArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glGetTextureParameterIiv (GLuint texture, GLenum pname, GLint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetTextureParameterIiv( JNIEnv* env, jclass klass,jint texture_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetTextureParameterIiv\n");
    static PFNGLGETTEXTUREPARAMETERIIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETTEXTUREPARAMETERIIVPROC) mygetprocaddr(env,"glGetTextureParameterIiv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(texture_, pname_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glGetTextureParameterIuiv (GLuint texture, GLenum pname, GLuint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetTextureParameterIuiv( JNIEnv* env, jclass klass,jint texture_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetTextureParameterIuiv\n");
    static PFNGLGETTEXTUREPARAMETERIUIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETTEXTUREPARAMETERIUIVPROC) mygetprocaddr(env,"glGetTextureParameterIuiv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(texture_, pname_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glGetTextureParameteriv (GLuint texture, GLenum pname, GLint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetTextureParameteriv( JNIEnv* env, jclass klass,jint texture_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetTextureParameteriv\n");
    static PFNGLGETTEXTUREPARAMETERIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETTEXTUREPARAMETERIVPROC) mygetprocaddr(env,"glGetTextureParameteriv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(texture_, pname_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glCreateVertexArrays (GLsizei n, GLuint *arrays);
JNIEXPORT void JNICALL Java_JGL_JGL_glCreateVertexArrays( JNIEnv* env, jclass klass,jint n_,jintArray arrays_){
    if(do_trace)
        fprintf(stderr,"glCreateVertexArrays\n");
    static PFNGLCREATEVERTEXARRAYSPROC impl;
    if( !impl ){
       impl = (PFNGLCREATEVERTEXARRAYSPROC) mygetprocaddr(env,"glCreateVertexArrays");
       if(!impl)
            return;
    }
    jint* _arrays_ = (arrays_ ? ((*env)->GetIntArrayElements(env,arrays_,NULL) ) : NULL);
    impl(n_, _arrays_);
    if(arrays_)
        (*env)->ReleaseIntArrayElements(env,arrays_, _arrays_,0);
}
//GLAPI void APIENTRY glDisableVertexArrayAttrib (GLuint vaobj, GLuint index);
JNIEXPORT void JNICALL Java_JGL_JGL_glDisableVertexArrayAttrib( JNIEnv* env, jclass klass,jint vaobj_,jint index_){
    if(do_trace)
        fprintf(stderr,"glDisableVertexArrayAttrib\n");
    static PFNGLDISABLEVERTEXARRAYATTRIBPROC impl;
    if( !impl ){
       impl = (PFNGLDISABLEVERTEXARRAYATTRIBPROC) mygetprocaddr(env,"glDisableVertexArrayAttrib");
       if(!impl)
            return;
    }
    impl(vaobj_, index_);
}
//GLAPI void APIENTRY glEnableVertexArrayAttrib (GLuint vaobj, GLuint index);
JNIEXPORT void JNICALL Java_JGL_JGL_glEnableVertexArrayAttrib( JNIEnv* env, jclass klass,jint vaobj_,jint index_){
    if(do_trace)
        fprintf(stderr,"glEnableVertexArrayAttrib\n");
    static PFNGLENABLEVERTEXARRAYATTRIBPROC impl;
    if( !impl ){
       impl = (PFNGLENABLEVERTEXARRAYATTRIBPROC) mygetprocaddr(env,"glEnableVertexArrayAttrib");
       if(!impl)
            return;
    }
    impl(vaobj_, index_);
}
//GLAPI void APIENTRY glVertexArrayElementBuffer (GLuint vaobj, GLuint buffer);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexArrayElementBuffer( JNIEnv* env, jclass klass,jint vaobj_,jint buffer_){
    if(do_trace)
        fprintf(stderr,"glVertexArrayElementBuffer\n");
    static PFNGLVERTEXARRAYELEMENTBUFFERPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXARRAYELEMENTBUFFERPROC) mygetprocaddr(env,"glVertexArrayElementBuffer");
       if(!impl)
            return;
    }
    impl(vaobj_, buffer_);
}
//GLAPI void APIENTRY glVertexArrayVertexBuffer (GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexArrayVertexBuffer( JNIEnv* env, jclass klass,jint vaobj_,jint bindingindex_,jint buffer_,jint offset_,jint stride_){
    if(do_trace)
        fprintf(stderr,"glVertexArrayVertexBuffer\n");
    static PFNGLVERTEXARRAYVERTEXBUFFERPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXARRAYVERTEXBUFFERPROC) mygetprocaddr(env,"glVertexArrayVertexBuffer");
       if(!impl)
            return;
    }
    impl(vaobj_, bindingindex_, buffer_, offset_, stride_);
}
//GLAPI void APIENTRY glVertexArrayAttribBinding (GLuint vaobj, GLuint attribindex, GLuint bindingindex);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexArrayAttribBinding( JNIEnv* env, jclass klass,jint vaobj_,jint attribindex_,jint bindingindex_){
    if(do_trace)
        fprintf(stderr,"glVertexArrayAttribBinding\n");
    static PFNGLVERTEXARRAYATTRIBBINDINGPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXARRAYATTRIBBINDINGPROC) mygetprocaddr(env,"glVertexArrayAttribBinding");
       if(!impl)
            return;
    }
    impl(vaobj_, attribindex_, bindingindex_);
}
//GLAPI void APIENTRY glVertexArrayAttribFormat (GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexArrayAttribFormat( JNIEnv* env, jclass klass,jint vaobj_,jint attribindex_,jint size_,jint type_,jboolean normalized_,jint relativeoffset_){
    if(do_trace)
        fprintf(stderr,"glVertexArrayAttribFormat\n");
    static PFNGLVERTEXARRAYATTRIBFORMATPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXARRAYATTRIBFORMATPROC) mygetprocaddr(env,"glVertexArrayAttribFormat");
       if(!impl)
            return;
    }
    impl(vaobj_, attribindex_, size_, type_, normalized_, relativeoffset_);
}
//GLAPI void APIENTRY glVertexArrayAttribIFormat (GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexArrayAttribIFormat( JNIEnv* env, jclass klass,jint vaobj_,jint attribindex_,jint size_,jint type_,jint relativeoffset_){
    if(do_trace)
        fprintf(stderr,"glVertexArrayAttribIFormat\n");
    static PFNGLVERTEXARRAYATTRIBIFORMATPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXARRAYATTRIBIFORMATPROC) mygetprocaddr(env,"glVertexArrayAttribIFormat");
       if(!impl)
            return;
    }
    impl(vaobj_, attribindex_, size_, type_, relativeoffset_);
}
//GLAPI void APIENTRY glVertexArrayAttribLFormat (GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexArrayAttribLFormat( JNIEnv* env, jclass klass,jint vaobj_,jint attribindex_,jint size_,jint type_,jint relativeoffset_){
    if(do_trace)
        fprintf(stderr,"glVertexArrayAttribLFormat\n");
    static PFNGLVERTEXARRAYATTRIBLFORMATPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXARRAYATTRIBLFORMATPROC) mygetprocaddr(env,"glVertexArrayAttribLFormat");
       if(!impl)
            return;
    }
    impl(vaobj_, attribindex_, size_, type_, relativeoffset_);
}
//GLAPI void APIENTRY glVertexArrayBindingDivisor (GLuint vaobj, GLuint bindingindex, GLuint divisor);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexArrayBindingDivisor( JNIEnv* env, jclass klass,jint vaobj_,jint bindingindex_,jint divisor_){
    if(do_trace)
        fprintf(stderr,"glVertexArrayBindingDivisor\n");
    static PFNGLVERTEXARRAYBINDINGDIVISORPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXARRAYBINDINGDIVISORPROC) mygetprocaddr(env,"glVertexArrayBindingDivisor");
       if(!impl)
            return;
    }
    impl(vaobj_, bindingindex_, divisor_);
}
//GLAPI void APIENTRY glGetVertexArrayiv (GLuint vaobj, GLenum pname, GLint *param);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetVertexArrayiv( JNIEnv* env, jclass klass,jint vaobj_,jint pname_,jintArray param_){
    if(do_trace)
        fprintf(stderr,"glGetVertexArrayiv\n");
    static PFNGLGETVERTEXARRAYIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETVERTEXARRAYIVPROC) mygetprocaddr(env,"glGetVertexArrayiv");
       if(!impl)
            return;
    }
    jint* _param_ = (param_ ? ((*env)->GetIntArrayElements(env,param_,NULL) ) : NULL);
    impl(vaobj_, pname_, _param_);
    if(param_)
        (*env)->ReleaseIntArrayElements(env,param_, _param_,0);
}
//GLAPI void APIENTRY glGetVertexArrayIndexediv (GLuint vaobj, GLuint index, GLenum pname, GLint *param);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetVertexArrayIndexediv( JNIEnv* env, jclass klass,jint vaobj_,jint index_,jint pname_,jintArray param_){
    if(do_trace)
        fprintf(stderr,"glGetVertexArrayIndexediv\n");
    static PFNGLGETVERTEXARRAYINDEXEDIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETVERTEXARRAYINDEXEDIVPROC) mygetprocaddr(env,"glGetVertexArrayIndexediv");
       if(!impl)
            return;
    }
    jint* _param_ = (param_ ? ((*env)->GetIntArrayElements(env,param_,NULL) ) : NULL);
    impl(vaobj_, index_, pname_, _param_);
    if(param_)
        (*env)->ReleaseIntArrayElements(env,param_, _param_,0);
}
//GLAPI void APIENTRY glGetVertexArrayIndexed64iv (GLuint vaobj, GLuint index, GLenum pname, GLint64 *param);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetVertexArrayIndexed64iv( JNIEnv* env, jclass klass,jint vaobj_,jint index_,jint pname_,jlongArray param_){
    if(do_trace)
        fprintf(stderr,"glGetVertexArrayIndexed64iv\n");
    static PFNGLGETVERTEXARRAYINDEXED64IVPROC impl;
    if( !impl ){
       impl = (PFNGLGETVERTEXARRAYINDEXED64IVPROC) mygetprocaddr(env,"glGetVertexArrayIndexed64iv");
       if(!impl)
            return;
    }
    jlong* _param_ = (param_ ? ((*env)->GetLongArrayElements(env,param_,NULL) ) : NULL);
    impl(vaobj_, index_, pname_, _param_);
    if(param_)
        (*env)->ReleaseLongArrayElements(env,param_, _param_,0);
}
//GLAPI void APIENTRY glCreateSamplers (GLsizei n, GLuint *samplers);
JNIEXPORT void JNICALL Java_JGL_JGL_glCreateSamplers( JNIEnv* env, jclass klass,jint n_,jintArray samplers_){
    if(do_trace)
        fprintf(stderr,"glCreateSamplers\n");
    static PFNGLCREATESAMPLERSPROC impl;
    if( !impl ){
       impl = (PFNGLCREATESAMPLERSPROC) mygetprocaddr(env,"glCreateSamplers");
       if(!impl)
            return;
    }
    jint* _samplers_ = (samplers_ ? ((*env)->GetIntArrayElements(env,samplers_,NULL) ) : NULL);
    impl(n_, _samplers_);
    if(samplers_)
        (*env)->ReleaseIntArrayElements(env,samplers_, _samplers_,0);
}
//GLAPI void APIENTRY glCreateProgramPipelines (GLsizei n, GLuint *pipelines);
JNIEXPORT void JNICALL Java_JGL_JGL_glCreateProgramPipelines( JNIEnv* env, jclass klass,jint n_,jintArray pipelines_){
    if(do_trace)
        fprintf(stderr,"glCreateProgramPipelines\n");
    static PFNGLCREATEPROGRAMPIPELINESPROC impl;
    if( !impl ){
       impl = (PFNGLCREATEPROGRAMPIPELINESPROC) mygetprocaddr(env,"glCreateProgramPipelines");
       if(!impl)
            return;
    }
    jint* _pipelines_ = (pipelines_ ? ((*env)->GetIntArrayElements(env,pipelines_,NULL) ) : NULL);
    impl(n_, _pipelines_);
    if(pipelines_)
        (*env)->ReleaseIntArrayElements(env,pipelines_, _pipelines_,0);
}
//GLAPI void APIENTRY glCreateQueries (GLenum target, GLsizei n, GLuint *ids);
JNIEXPORT void JNICALL Java_JGL_JGL_glCreateQueries( JNIEnv* env, jclass klass,jint target_,jint n_,jintArray ids_){
    if(do_trace)
        fprintf(stderr,"glCreateQueries\n");
    static PFNGLCREATEQUERIESPROC impl;
    if( !impl ){
       impl = (PFNGLCREATEQUERIESPROC) mygetprocaddr(env,"glCreateQueries");
       if(!impl)
            return;
    }
    jint* _ids_ = (ids_ ? ((*env)->GetIntArrayElements(env,ids_,NULL) ) : NULL);
    impl(target_, n_, _ids_);
    if(ids_)
        (*env)->ReleaseIntArrayElements(env,ids_, _ids_,0);
}
//GLAPI void APIENTRY glMemoryBarrierByRegion (GLbitfield barriers);
JNIEXPORT void JNICALL Java_JGL_JGL_glMemoryBarrierByRegion( JNIEnv* env, jclass klass,jint barriers_){
    if(do_trace)
        fprintf(stderr,"glMemoryBarrierByRegion\n");
    static PFNGLMEMORYBARRIERBYREGIONPROC impl;
    if( !impl ){
       impl = (PFNGLMEMORYBARRIERBYREGIONPROC) mygetprocaddr(env,"glMemoryBarrierByRegion");
       if(!impl)
            return;
    }
    impl(barriers_);
}
//GLAPI void APIENTRY glGetTextureSubImage (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLsizei bufSize, void *pixels);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetTextureSubImage( JNIEnv* env, jclass klass,jint texture_,jint level_,jint xoffset_,jint yoffset_,jint zoffset_,jint width_,jint height_,jint depth_,jint format_,jint type_,jint bufSize_,jbyteArray pixels_){
    if(do_trace)
        fprintf(stderr,"glGetTextureSubImage\n");
    static PFNGLGETTEXTURESUBIMAGEPROC impl;
    if( !impl ){
       impl = (PFNGLGETTEXTURESUBIMAGEPROC) mygetprocaddr(env,"glGetTextureSubImage");
       if(!impl)
            return;
    }
    jbyte* _pixels_ = (pixels_ ? ((*env)->GetByteArrayElements(env,pixels_,NULL) ) : NULL);
    impl(texture_, level_, xoffset_, yoffset_, zoffset_, width_, height_, depth_, format_, type_, bufSize_, _pixels_);
    if(pixels_)
        (*env)->ReleaseByteArrayElements(env,pixels_, _pixels_,0);
}
//GLAPI void APIENTRY glGetCompressedTextureSubImage (GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLsizei bufSize, void *pixels);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetCompressedTextureSubImage( JNIEnv* env, jclass klass,jint texture_,jint level_,jint xoffset_,jint yoffset_,jint zoffset_,jint width_,jint height_,jint depth_,jint bufSize_,jbyteArray pixels_){
    if(do_trace)
        fprintf(stderr,"glGetCompressedTextureSubImage\n");
    static PFNGLGETCOMPRESSEDTEXTURESUBIMAGEPROC impl;
    if( !impl ){
       impl = (PFNGLGETCOMPRESSEDTEXTURESUBIMAGEPROC) mygetprocaddr(env,"glGetCompressedTextureSubImage");
       if(!impl)
            return;
    }
    jbyte* _pixels_ = (pixels_ ? ((*env)->GetByteArrayElements(env,pixels_,NULL) ) : NULL);
    impl(texture_, level_, xoffset_, yoffset_, zoffset_, width_, height_, depth_, bufSize_, _pixels_);
    if(pixels_)
        (*env)->ReleaseByteArrayElements(env,pixels_, _pixels_,0);
}
//GLAPI GLenum APIENTRY glGetGraphicsResetStatus (void);
JNIEXPORT jint JNICALL Java_JGL_JGL_glGetGraphicsResetStatus( JNIEnv* env, jclass klass){
    jint rv = 0;
    if(do_trace)
        fprintf(stderr,"glGetGraphicsResetStatus\n");
    static PFNGLGETGRAPHICSRESETSTATUSPROC impl;
    if( !impl ){
       impl = (PFNGLGETGRAPHICSRESETSTATUSPROC) mygetprocaddr(env,"glGetGraphicsResetStatus");
       if(!impl)
            return rv;
    }
    rv = impl();
    return rv;
}
//GLAPI void APIENTRY glGetnCompressedTexImage (GLenum target, GLint lod, GLsizei bufSize, void *pixels);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetnCompressedTexImage( JNIEnv* env, jclass klass,jint target_,jint lod_,jint bufSize_,jbyteArray pixels_){
    if(do_trace)
        fprintf(stderr,"glGetnCompressedTexImage\n");
    static PFNGLGETNCOMPRESSEDTEXIMAGEPROC impl;
    if( !impl ){
       impl = (PFNGLGETNCOMPRESSEDTEXIMAGEPROC) mygetprocaddr(env,"glGetnCompressedTexImage");
       if(!impl)
            return;
    }
    jbyte* _pixels_ = (pixels_ ? ((*env)->GetByteArrayElements(env,pixels_,NULL) ) : NULL);
    impl(target_, lod_, bufSize_, _pixels_);
    if(pixels_)
        (*env)->ReleaseByteArrayElements(env,pixels_, _pixels_,0);
}
//GLAPI void APIENTRY glGetnTexImage (GLenum target, GLint level, GLenum format, GLenum type, GLsizei bufSize, void *pixels);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetnTexImage( JNIEnv* env, jclass klass,jint target_,jint level_,jint format_,jint type_,jint bufSize_,jbyteArray pixels_){
    if(do_trace)
        fprintf(stderr,"glGetnTexImage\n");
    static PFNGLGETNTEXIMAGEPROC impl;
    if( !impl ){
       impl = (PFNGLGETNTEXIMAGEPROC) mygetprocaddr(env,"glGetnTexImage");
       if(!impl)
            return;
    }
    jbyte* _pixels_ = (pixels_ ? ((*env)->GetByteArrayElements(env,pixels_,NULL) ) : NULL);
    impl(target_, level_, format_, type_, bufSize_, _pixels_);
    if(pixels_)
        (*env)->ReleaseByteArrayElements(env,pixels_, _pixels_,0);
}
//GLAPI void APIENTRY glGetnUniformdv (GLuint program, GLint location, GLsizei bufSize, GLdouble *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetnUniformdv( JNIEnv* env, jclass klass,jint program_,jint location_,jint bufSize_,jdoubleArray params_){
    if(do_trace)
        fprintf(stderr,"glGetnUniformdv\n");
    static PFNGLGETNUNIFORMDVPROC impl;
    if( !impl ){
       impl = (PFNGLGETNUNIFORMDVPROC) mygetprocaddr(env,"glGetnUniformdv");
       if(!impl)
            return;
    }
    jdouble* _params_ = (params_ ? ((*env)->GetDoubleArrayElements(env,params_,NULL) ) : NULL);
    impl(program_, location_, bufSize_, _params_);
    if(params_)
        (*env)->ReleaseDoubleArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glGetnUniformfv (GLuint program, GLint location, GLsizei bufSize, GLfloat *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetnUniformfv( JNIEnv* env, jclass klass,jint program_,jint location_,jint bufSize_,jfloatArray params_){
    if(do_trace)
        fprintf(stderr,"glGetnUniformfv\n");
    static PFNGLGETNUNIFORMFVPROC impl;
    if( !impl ){
       impl = (PFNGLGETNUNIFORMFVPROC) mygetprocaddr(env,"glGetnUniformfv");
       if(!impl)
            return;
    }
    jfloat* _params_ = (params_ ? ((*env)->GetFloatArrayElements(env,params_,NULL) ) : NULL);
    impl(program_, location_, bufSize_, _params_);
    if(params_)
        (*env)->ReleaseFloatArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glGetnUniformiv (GLuint program, GLint location, GLsizei bufSize, GLint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetnUniformiv( JNIEnv* env, jclass klass,jint program_,jint location_,jint bufSize_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetnUniformiv\n");
    static PFNGLGETNUNIFORMIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETNUNIFORMIVPROC) mygetprocaddr(env,"glGetnUniformiv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(program_, location_, bufSize_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glGetnUniformuiv (GLuint program, GLint location, GLsizei bufSize, GLuint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetnUniformuiv( JNIEnv* env, jclass klass,jint program_,jint location_,jint bufSize_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetnUniformuiv\n");
    static PFNGLGETNUNIFORMUIVPROC impl;
    if( !impl ){
       impl = (PFNGLGETNUNIFORMUIVPROC) mygetprocaddr(env,"glGetnUniformuiv");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(program_, location_, bufSize_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glReadnPixels (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLsizei bufSize, void *data);
JNIEXPORT void JNICALL Java_JGL_JGL_glReadnPixels( JNIEnv* env, jclass klass,jint x_,jint y_,jint width_,jint height_,jint format_,jint type_,jint bufSize_,jbyteArray data_){
    if(do_trace)
        fprintf(stderr,"glReadnPixels\n");
    static PFNGLREADNPIXELSPROC impl;
    if( !impl ){
       impl = (PFNGLREADNPIXELSPROC) mygetprocaddr(env,"glReadnPixels");
       if(!impl)
            return;
    }
    jbyte* _data_ = (data_ ? ((*env)->GetByteArrayElements(env,data_,NULL) ) : NULL);
    impl(x_, y_, width_, height_, format_, type_, bufSize_, _data_);
    if(data_)
        (*env)->ReleaseByteArrayElements(env,data_, _data_,0);
}
//GLAPI void APIENTRY glTextureBarrier (void);
JNIEXPORT void JNICALL Java_JGL_JGL_glTextureBarrier( JNIEnv* env, jclass klass){
    if(do_trace)
        fprintf(stderr,"glTextureBarrier\n");
    static PFNGLTEXTUREBARRIERPROC impl;
    if( !impl ){
       impl = (PFNGLTEXTUREBARRIERPROC) mygetprocaddr(env,"glTextureBarrier");
       if(!impl)
            return;
    }
    impl();
}
//GLAPI GLuint64 APIENTRY glGetTextureHandleARB (GLuint texture);
JNIEXPORT jlong JNICALL Java_JGL_JGL_glGetTextureHandleARB( JNIEnv* env, jclass klass,jint texture_){
    jlong rv = 0;
    if(do_trace)
        fprintf(stderr,"glGetTextureHandleARB\n");
    static PFNGLGETTEXTUREHANDLEARBPROC impl;
    if( !impl ){
       impl = (PFNGLGETTEXTUREHANDLEARBPROC) mygetprocaddr(env,"glGetTextureHandleARB");
       if(!impl)
            return rv;
    }
    rv = impl(texture_);
    return rv;
}
//GLAPI GLuint64 APIENTRY glGetTextureSamplerHandleARB (GLuint texture, GLuint sampler);
JNIEXPORT jlong JNICALL Java_JGL_JGL_glGetTextureSamplerHandleARB( JNIEnv* env, jclass klass,jint texture_,jint sampler_){
    jlong rv = 0;
    if(do_trace)
        fprintf(stderr,"glGetTextureSamplerHandleARB\n");
    static PFNGLGETTEXTURESAMPLERHANDLEARBPROC impl;
    if( !impl ){
       impl = (PFNGLGETTEXTURESAMPLERHANDLEARBPROC) mygetprocaddr(env,"glGetTextureSamplerHandleARB");
       if(!impl)
            return rv;
    }
    rv = impl(texture_, sampler_);
    return rv;
}
//GLAPI void APIENTRY glMakeTextureHandleResidentARB (GLuint64 handle);
JNIEXPORT void JNICALL Java_JGL_JGL_glMakeTextureHandleResidentARB( JNIEnv* env, jclass klass,jlong handle_){
    if(do_trace)
        fprintf(stderr,"glMakeTextureHandleResidentARB\n");
    static PFNGLMAKETEXTUREHANDLERESIDENTARBPROC impl;
    if( !impl ){
       impl = (PFNGLMAKETEXTUREHANDLERESIDENTARBPROC) mygetprocaddr(env,"glMakeTextureHandleResidentARB");
       if(!impl)
            return;
    }
    impl(handle_);
}
//GLAPI void APIENTRY glMakeTextureHandleNonResidentARB (GLuint64 handle);
JNIEXPORT void JNICALL Java_JGL_JGL_glMakeTextureHandleNonResidentARB( JNIEnv* env, jclass klass,jlong handle_){
    if(do_trace)
        fprintf(stderr,"glMakeTextureHandleNonResidentARB\n");
    static PFNGLMAKETEXTUREHANDLENONRESIDENTARBPROC impl;
    if( !impl ){
       impl = (PFNGLMAKETEXTUREHANDLENONRESIDENTARBPROC) mygetprocaddr(env,"glMakeTextureHandleNonResidentARB");
       if(!impl)
            return;
    }
    impl(handle_);
}
//GLAPI GLuint64 APIENTRY glGetImageHandleARB (GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum format);
JNIEXPORT jlong JNICALL Java_JGL_JGL_glGetImageHandleARB( JNIEnv* env, jclass klass,jint texture_,jint level_,jboolean layered_,jint layer_,jint format_){
    jlong rv = 0;
    if(do_trace)
        fprintf(stderr,"glGetImageHandleARB\n");
    static PFNGLGETIMAGEHANDLEARBPROC impl;
    if( !impl ){
       impl = (PFNGLGETIMAGEHANDLEARBPROC) mygetprocaddr(env,"glGetImageHandleARB");
       if(!impl)
            return rv;
    }
    rv = impl(texture_, level_, layered_, layer_, format_);
    return rv;
}
//GLAPI void APIENTRY glMakeImageHandleResidentARB (GLuint64 handle, GLenum access);
JNIEXPORT void JNICALL Java_JGL_JGL_glMakeImageHandleResidentARB( JNIEnv* env, jclass klass,jlong handle_,jint access_){
    if(do_trace)
        fprintf(stderr,"glMakeImageHandleResidentARB\n");
    static PFNGLMAKEIMAGEHANDLERESIDENTARBPROC impl;
    if( !impl ){
       impl = (PFNGLMAKEIMAGEHANDLERESIDENTARBPROC) mygetprocaddr(env,"glMakeImageHandleResidentARB");
       if(!impl)
            return;
    }
    impl(handle_, access_);
}
//GLAPI void APIENTRY glMakeImageHandleNonResidentARB (GLuint64 handle);
JNIEXPORT void JNICALL Java_JGL_JGL_glMakeImageHandleNonResidentARB( JNIEnv* env, jclass klass,jlong handle_){
    if(do_trace)
        fprintf(stderr,"glMakeImageHandleNonResidentARB\n");
    static PFNGLMAKEIMAGEHANDLENONRESIDENTARBPROC impl;
    if( !impl ){
       impl = (PFNGLMAKEIMAGEHANDLENONRESIDENTARBPROC) mygetprocaddr(env,"glMakeImageHandleNonResidentARB");
       if(!impl)
            return;
    }
    impl(handle_);
}
//GLAPI void APIENTRY glUniformHandleui64ARB (GLint location, GLuint64 value);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniformHandleui64ARB( JNIEnv* env, jclass klass,jint location_,jlong value_){
    if(do_trace)
        fprintf(stderr,"glUniformHandleui64ARB\n");
    static PFNGLUNIFORMHANDLEUI64ARBPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORMHANDLEUI64ARBPROC) mygetprocaddr(env,"glUniformHandleui64ARB");
       if(!impl)
            return;
    }
    impl(location_, value_);
}
//GLAPI void APIENTRY glUniformHandleui64vARB (GLint location, GLsizei count, const GLuint64 *value);
JNIEXPORT void JNICALL Java_JGL_JGL_glUniformHandleui64vARB( JNIEnv* env, jclass klass,jint location_,jint count_,jlongArray value_){
    if(do_trace)
        fprintf(stderr,"glUniformHandleui64vARB\n");
    static PFNGLUNIFORMHANDLEUI64VARBPROC impl;
    if( !impl ){
       impl = (PFNGLUNIFORMHANDLEUI64VARBPROC) mygetprocaddr(env,"glUniformHandleui64vARB");
       if(!impl)
            return;
    }
    jlong* _value_ = (value_ ? ((*env)->GetLongArrayElements(env,value_,NULL) ) : NULL);
    impl(location_, count_, _value_);
    if(value_)
        (*env)->ReleaseLongArrayElements(env,value_, _value_,0);
}
//GLAPI void APIENTRY glProgramUniformHandleui64ARB (GLuint program, GLint location, GLuint64 value);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniformHandleui64ARB( JNIEnv* env, jclass klass,jint program_,jint location_,jlong value_){
    if(do_trace)
        fprintf(stderr,"glProgramUniformHandleui64ARB\n");
    static PFNGLPROGRAMUNIFORMHANDLEUI64ARBPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORMHANDLEUI64ARBPROC) mygetprocaddr(env,"glProgramUniformHandleui64ARB");
       if(!impl)
            return;
    }
    impl(program_, location_, value_);
}
//GLAPI void APIENTRY glProgramUniformHandleui64vARB (GLuint program, GLint location, GLsizei count, const GLuint64 *values);
JNIEXPORT void JNICALL Java_JGL_JGL_glProgramUniformHandleui64vARB( JNIEnv* env, jclass klass,jint program_,jint location_,jint count_,jlongArray values_){
    if(do_trace)
        fprintf(stderr,"glProgramUniformHandleui64vARB\n");
    static PFNGLPROGRAMUNIFORMHANDLEUI64VARBPROC impl;
    if( !impl ){
       impl = (PFNGLPROGRAMUNIFORMHANDLEUI64VARBPROC) mygetprocaddr(env,"glProgramUniformHandleui64vARB");
       if(!impl)
            return;
    }
    jlong* _values_ = (values_ ? ((*env)->GetLongArrayElements(env,values_,NULL) ) : NULL);
    impl(program_, location_, count_, _values_);
    if(values_)
        (*env)->ReleaseLongArrayElements(env,values_, _values_,0);
}
//GLAPI GLboolean APIENTRY glIsTextureHandleResidentARB (GLuint64 handle);
JNIEXPORT jboolean JNICALL Java_JGL_JGL_glIsTextureHandleResidentARB( JNIEnv* env, jclass klass,jlong handle_){
    jboolean rv = 0;
    if(do_trace)
        fprintf(stderr,"glIsTextureHandleResidentARB\n");
    static PFNGLISTEXTUREHANDLERESIDENTARBPROC impl;
    if( !impl ){
       impl = (PFNGLISTEXTUREHANDLERESIDENTARBPROC) mygetprocaddr(env,"glIsTextureHandleResidentARB");
       if(!impl)
            return rv;
    }
    rv = impl(handle_);
    return rv;
}
//GLAPI GLboolean APIENTRY glIsImageHandleResidentARB (GLuint64 handle);
JNIEXPORT jboolean JNICALL Java_JGL_JGL_glIsImageHandleResidentARB( JNIEnv* env, jclass klass,jlong handle_){
    jboolean rv = 0;
    if(do_trace)
        fprintf(stderr,"glIsImageHandleResidentARB\n");
    static PFNGLISIMAGEHANDLERESIDENTARBPROC impl;
    if( !impl ){
       impl = (PFNGLISIMAGEHANDLERESIDENTARBPROC) mygetprocaddr(env,"glIsImageHandleResidentARB");
       if(!impl)
            return rv;
    }
    rv = impl(handle_);
    return rv;
}
//GLAPI void APIENTRY glVertexAttribL1ui64ARB (GLuint index, GLuint64EXT x);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribL1ui64ARB( JNIEnv* env, jclass klass,jint index_,jlong x_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribL1ui64ARB\n");
    static PFNGLVERTEXATTRIBL1UI64ARBPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBL1UI64ARBPROC) mygetprocaddr(env,"glVertexAttribL1ui64ARB");
       if(!impl)
            return;
    }
    impl(index_, x_);
}
//GLAPI void APIENTRY glVertexAttribL1ui64vARB (GLuint index, const GLuint64EXT *v);
JNIEXPORT void JNICALL Java_JGL_JGL_glVertexAttribL1ui64vARB( JNIEnv* env, jclass klass,jint index_,jlongArray v_){
    if(do_trace)
        fprintf(stderr,"glVertexAttribL1ui64vARB\n");
    static PFNGLVERTEXATTRIBL1UI64VARBPROC impl;
    if( !impl ){
       impl = (PFNGLVERTEXATTRIBL1UI64VARBPROC) mygetprocaddr(env,"glVertexAttribL1ui64vARB");
       if(!impl)
            return;
    }
    jlong* _v_ = (v_ ? ((*env)->GetLongArrayElements(env,v_,NULL) ) : NULL);
    impl(index_, _v_);
    if(v_)
        (*env)->ReleaseLongArrayElements(env,v_, _v_,0);
}
//GLAPI void APIENTRY glGetVertexAttribLui64vARB (GLuint index, GLenum pname, GLuint64EXT *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetVertexAttribLui64vARB( JNIEnv* env, jclass klass,jint index_,jint pname_,jlongArray params_){
    if(do_trace)
        fprintf(stderr,"glGetVertexAttribLui64vARB\n");
    static PFNGLGETVERTEXATTRIBLUI64VARBPROC impl;
    if( !impl ){
       impl = (PFNGLGETVERTEXATTRIBLUI64VARBPROC) mygetprocaddr(env,"glGetVertexAttribLui64vARB");
       if(!impl)
            return;
    }
    jlong* _params_ = (params_ ? ((*env)->GetLongArrayElements(env,params_,NULL) ) : NULL);
    impl(index_, pname_, _params_);
    if(params_)
        (*env)->ReleaseLongArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glDispatchComputeGroupSizeARB (GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z, GLuint group_size_x, GLuint group_size_y, GLuint group_size_z);
JNIEXPORT void JNICALL Java_JGL_JGL_glDispatchComputeGroupSizeARB( JNIEnv* env, jclass klass,jint num_groups_x_,jint num_groups_y_,jint num_groups_z_,jint group_size_x_,jint group_size_y_,jint group_size_z_){
    if(do_trace)
        fprintf(stderr,"glDispatchComputeGroupSizeARB\n");
    static PFNGLDISPATCHCOMPUTEGROUPSIZEARBPROC impl;
    if( !impl ){
       impl = (PFNGLDISPATCHCOMPUTEGROUPSIZEARBPROC) mygetprocaddr(env,"glDispatchComputeGroupSizeARB");
       if(!impl)
            return;
    }
    impl(num_groups_x_, num_groups_y_, num_groups_z_, group_size_x_, group_size_y_, group_size_z_);
}
//GLAPI void APIENTRY glDebugMessageControlARB (GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled);
JNIEXPORT void JNICALL Java_JGL_JGL_glDebugMessageControlARB( JNIEnv* env, jclass klass,jint source_,jint type_,jint severity_,jint count_,jintArray ids_,jboolean enabled_){
    if(do_trace)
        fprintf(stderr,"glDebugMessageControlARB\n");
    static PFNGLDEBUGMESSAGECONTROLARBPROC impl;
    if( !impl ){
       impl = (PFNGLDEBUGMESSAGECONTROLARBPROC) mygetprocaddr(env,"glDebugMessageControlARB");
       if(!impl)
            return;
    }
    jint* _ids_ = (ids_ ? ((*env)->GetIntArrayElements(env,ids_,NULL) ) : NULL);
    impl(source_, type_, severity_, count_, _ids_, enabled_);
    if(ids_)
        (*env)->ReleaseIntArrayElements(env,ids_, _ids_,0);
}
//GLAPI void APIENTRY glDebugMessageInsertARB (GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *buf);
JNIEXPORT void JNICALL Java_JGL_JGL_glDebugMessageInsertARB( JNIEnv* env, jclass klass,jint source_,jint type_,jint id_,jint severity_,jint length_,jbyteArray buf_){
    if(do_trace)
        fprintf(stderr,"glDebugMessageInsertARB\n");
    static PFNGLDEBUGMESSAGEINSERTARBPROC impl;
    if( !impl ){
       impl = (PFNGLDEBUGMESSAGEINSERTARBPROC) mygetprocaddr(env,"glDebugMessageInsertARB");
       if(!impl)
            return;
    }
    jbyte* _buf_ = (buf_ ? ((*env)->GetByteArrayElements(env,buf_,NULL) ) : NULL);
    impl(source_, type_, id_, severity_, length_, _buf_);
    if(buf_)
        (*env)->ReleaseByteArrayElements(env,buf_, _buf_,0);
}
//GLAPI GLuint APIENTRY glGetDebugMessageLogARB (GLuint count, GLsizei bufSize, GLenum *sources, GLenum *types, GLuint *ids, GLenum *severities, GLsizei *lengths, GLchar *messageLog);
JNIEXPORT jint JNICALL Java_JGL_JGL_glGetDebugMessageLogARB( JNIEnv* env, jclass klass,jint count_,jint bufSize_,jintArray sources_,jintArray types_,jintArray ids_,jintArray severities_,jintArray lengths_,jbyteArray messageLog_){
    jint rv = 0;
    if(do_trace)
        fprintf(stderr,"glGetDebugMessageLogARB\n");
    static PFNGLGETDEBUGMESSAGELOGARBPROC impl;
    if( !impl ){
       impl = (PFNGLGETDEBUGMESSAGELOGARBPROC) mygetprocaddr(env,"glGetDebugMessageLogARB");
       if(!impl)
            return rv;
    }
    jint* _sources_ = (sources_ ? ((*env)->GetIntArrayElements(env,sources_,NULL) ) : NULL);
    jint* _types_ = (types_ ? ((*env)->GetIntArrayElements(env,types_,NULL) ) : NULL);
    jint* _ids_ = (ids_ ? ((*env)->GetIntArrayElements(env,ids_,NULL) ) : NULL);
    jint* _severities_ = (severities_ ? ((*env)->GetIntArrayElements(env,severities_,NULL) ) : NULL);
    jint* _lengths_ = (lengths_ ? ((*env)->GetIntArrayElements(env,lengths_,NULL) ) : NULL);
    jbyte* _messageLog_ = (messageLog_ ? ((*env)->GetByteArrayElements(env,messageLog_,NULL) ) : NULL);
    rv = impl(count_, bufSize_, _sources_, _types_, _ids_, _severities_, _lengths_, _messageLog_);
    if(sources_)
        (*env)->ReleaseIntArrayElements(env,sources_, _sources_,0);
    if(types_)
        (*env)->ReleaseIntArrayElements(env,types_, _types_,0);
    if(ids_)
        (*env)->ReleaseIntArrayElements(env,ids_, _ids_,0);
    if(severities_)
        (*env)->ReleaseIntArrayElements(env,severities_, _severities_,0);
    if(lengths_)
        (*env)->ReleaseIntArrayElements(env,lengths_, _lengths_,0);
    if(messageLog_)
        (*env)->ReleaseByteArrayElements(env,messageLog_, _messageLog_,0);
    return rv;
}
//GLAPI void APIENTRY glBlendEquationiARB (GLuint buf, GLenum mode);
JNIEXPORT void JNICALL Java_JGL_JGL_glBlendEquationiARB( JNIEnv* env, jclass klass,jint buf_,jint mode_){
    if(do_trace)
        fprintf(stderr,"glBlendEquationiARB\n");
    static PFNGLBLENDEQUATIONIARBPROC impl;
    if( !impl ){
       impl = (PFNGLBLENDEQUATIONIARBPROC) mygetprocaddr(env,"glBlendEquationiARB");
       if(!impl)
            return;
    }
    impl(buf_, mode_);
}
//GLAPI void APIENTRY glBlendEquationSeparateiARB (GLuint buf, GLenum modeRGB, GLenum modeAlpha);
JNIEXPORT void JNICALL Java_JGL_JGL_glBlendEquationSeparateiARB( JNIEnv* env, jclass klass,jint buf_,jint modeRGB_,jint modeAlpha_){
    if(do_trace)
        fprintf(stderr,"glBlendEquationSeparateiARB\n");
    static PFNGLBLENDEQUATIONSEPARATEIARBPROC impl;
    if( !impl ){
       impl = (PFNGLBLENDEQUATIONSEPARATEIARBPROC) mygetprocaddr(env,"glBlendEquationSeparateiARB");
       if(!impl)
            return;
    }
    impl(buf_, modeRGB_, modeAlpha_);
}
//GLAPI void APIENTRY glBlendFunciARB (GLuint buf, GLenum src, GLenum dst);
JNIEXPORT void JNICALL Java_JGL_JGL_glBlendFunciARB( JNIEnv* env, jclass klass,jint buf_,jint src_,jint dst_){
    if(do_trace)
        fprintf(stderr,"glBlendFunciARB\n");
    static PFNGLBLENDFUNCIARBPROC impl;
    if( !impl ){
       impl = (PFNGLBLENDFUNCIARBPROC) mygetprocaddr(env,"glBlendFunciARB");
       if(!impl)
            return;
    }
    impl(buf_, src_, dst_);
}
//GLAPI void APIENTRY glBlendFuncSeparateiARB (GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
JNIEXPORT void JNICALL Java_JGL_JGL_glBlendFuncSeparateiARB( JNIEnv* env, jclass klass,jint buf_,jint srcRGB_,jint dstRGB_,jint srcAlpha_,jint dstAlpha_){
    if(do_trace)
        fprintf(stderr,"glBlendFuncSeparateiARB\n");
    static PFNGLBLENDFUNCSEPARATEIARBPROC impl;
    if( !impl ){
       impl = (PFNGLBLENDFUNCSEPARATEIARBPROC) mygetprocaddr(env,"glBlendFuncSeparateiARB");
       if(!impl)
            return;
    }
    impl(buf_, srcRGB_, dstRGB_, srcAlpha_, dstAlpha_);
}
//GLAPI void APIENTRY glMultiDrawArraysIndirectCountARB (GLenum mode, GLintptr indirect, GLintptr drawcount, GLsizei maxdrawcount, GLsizei stride);
JNIEXPORT void JNICALL Java_JGL_JGL_glMultiDrawArraysIndirectCountARB( JNIEnv* env, jclass klass,jint mode_,jint indirect_,jint drawcount_,jint maxdrawcount_,jint stride_){
    if(do_trace)
        fprintf(stderr,"glMultiDrawArraysIndirectCountARB\n");
    static PFNGLMULTIDRAWARRAYSINDIRECTCOUNTARBPROC impl;
    if( !impl ){
       impl = (PFNGLMULTIDRAWARRAYSINDIRECTCOUNTARBPROC) mygetprocaddr(env,"glMultiDrawArraysIndirectCountARB");
       if(!impl)
            return;
    }
    impl(mode_, indirect_, drawcount_, maxdrawcount_, stride_);
}
//GLAPI void APIENTRY glMultiDrawElementsIndirectCountARB (GLenum mode, GLenum type, GLintptr indirect, GLintptr drawcount, GLsizei maxdrawcount, GLsizei stride);
JNIEXPORT void JNICALL Java_JGL_JGL_glMultiDrawElementsIndirectCountARB( JNIEnv* env, jclass klass,jint mode_,jint type_,jint indirect_,jint drawcount_,jint maxdrawcount_,jint stride_){
    if(do_trace)
        fprintf(stderr,"glMultiDrawElementsIndirectCountARB\n");
    static PFNGLMULTIDRAWELEMENTSINDIRECTCOUNTARBPROC impl;
    if( !impl ){
       impl = (PFNGLMULTIDRAWELEMENTSINDIRECTCOUNTARBPROC) mygetprocaddr(env,"glMultiDrawElementsIndirectCountARB");
       if(!impl)
            return;
    }
    impl(mode_, type_, indirect_, drawcount_, maxdrawcount_, stride_);
}
//GLAPI GLenum APIENTRY glGetGraphicsResetStatusARB (void);
JNIEXPORT jint JNICALL Java_JGL_JGL_glGetGraphicsResetStatusARB( JNIEnv* env, jclass klass){
    jint rv = 0;
    if(do_trace)
        fprintf(stderr,"glGetGraphicsResetStatusARB\n");
    static PFNGLGETGRAPHICSRESETSTATUSARBPROC impl;
    if( !impl ){
       impl = (PFNGLGETGRAPHICSRESETSTATUSARBPROC) mygetprocaddr(env,"glGetGraphicsResetStatusARB");
       if(!impl)
            return rv;
    }
    rv = impl();
    return rv;
}
//GLAPI void APIENTRY glGetnTexImageARB (GLenum target, GLint level, GLenum format, GLenum type, GLsizei bufSize, void *img);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetnTexImageARB( JNIEnv* env, jclass klass,jint target_,jint level_,jint format_,jint type_,jint bufSize_,jbyteArray img_){
    if(do_trace)
        fprintf(stderr,"glGetnTexImageARB\n");
    static PFNGLGETNTEXIMAGEARBPROC impl;
    if( !impl ){
       impl = (PFNGLGETNTEXIMAGEARBPROC) mygetprocaddr(env,"glGetnTexImageARB");
       if(!impl)
            return;
    }
    jbyte* _img_ = (img_ ? ((*env)->GetByteArrayElements(env,img_,NULL) ) : NULL);
    impl(target_, level_, format_, type_, bufSize_, _img_);
    if(img_)
        (*env)->ReleaseByteArrayElements(env,img_, _img_,0);
}
//GLAPI void APIENTRY glReadnPixelsARB (GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLsizei bufSize, void *data);
JNIEXPORT void JNICALL Java_JGL_JGL_glReadnPixelsARB( JNIEnv* env, jclass klass,jint x_,jint y_,jint width_,jint height_,jint format_,jint type_,jint bufSize_,jbyteArray data_){
    if(do_trace)
        fprintf(stderr,"glReadnPixelsARB\n");
    static PFNGLREADNPIXELSARBPROC impl;
    if( !impl ){
       impl = (PFNGLREADNPIXELSARBPROC) mygetprocaddr(env,"glReadnPixelsARB");
       if(!impl)
            return;
    }
    jbyte* _data_ = (data_ ? ((*env)->GetByteArrayElements(env,data_,NULL) ) : NULL);
    impl(x_, y_, width_, height_, format_, type_, bufSize_, _data_);
    if(data_)
        (*env)->ReleaseByteArrayElements(env,data_, _data_,0);
}
//GLAPI void APIENTRY glGetnCompressedTexImageARB (GLenum target, GLint lod, GLsizei bufSize, void *img);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetnCompressedTexImageARB( JNIEnv* env, jclass klass,jint target_,jint lod_,jint bufSize_,jbyteArray img_){
    if(do_trace)
        fprintf(stderr,"glGetnCompressedTexImageARB\n");
    static PFNGLGETNCOMPRESSEDTEXIMAGEARBPROC impl;
    if( !impl ){
       impl = (PFNGLGETNCOMPRESSEDTEXIMAGEARBPROC) mygetprocaddr(env,"glGetnCompressedTexImageARB");
       if(!impl)
            return;
    }
    jbyte* _img_ = (img_ ? ((*env)->GetByteArrayElements(env,img_,NULL) ) : NULL);
    impl(target_, lod_, bufSize_, _img_);
    if(img_)
        (*env)->ReleaseByteArrayElements(env,img_, _img_,0);
}
//GLAPI void APIENTRY glGetnUniformfvARB (GLuint program, GLint location, GLsizei bufSize, GLfloat *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetnUniformfvARB( JNIEnv* env, jclass klass,jint program_,jint location_,jint bufSize_,jfloatArray params_){
    if(do_trace)
        fprintf(stderr,"glGetnUniformfvARB\n");
    static PFNGLGETNUNIFORMFVARBPROC impl;
    if( !impl ){
       impl = (PFNGLGETNUNIFORMFVARBPROC) mygetprocaddr(env,"glGetnUniformfvARB");
       if(!impl)
            return;
    }
    jfloat* _params_ = (params_ ? ((*env)->GetFloatArrayElements(env,params_,NULL) ) : NULL);
    impl(program_, location_, bufSize_, _params_);
    if(params_)
        (*env)->ReleaseFloatArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glGetnUniformivARB (GLuint program, GLint location, GLsizei bufSize, GLint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetnUniformivARB( JNIEnv* env, jclass klass,jint program_,jint location_,jint bufSize_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetnUniformivARB\n");
    static PFNGLGETNUNIFORMIVARBPROC impl;
    if( !impl ){
       impl = (PFNGLGETNUNIFORMIVARBPROC) mygetprocaddr(env,"glGetnUniformivARB");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(program_, location_, bufSize_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glGetnUniformuivARB (GLuint program, GLint location, GLsizei bufSize, GLuint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetnUniformuivARB( JNIEnv* env, jclass klass,jint program_,jint location_,jint bufSize_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetnUniformuivARB\n");
    static PFNGLGETNUNIFORMUIVARBPROC impl;
    if( !impl ){
       impl = (PFNGLGETNUNIFORMUIVARBPROC) mygetprocaddr(env,"glGetnUniformuivARB");
       if(!impl)
            return;
    }
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(program_, location_, bufSize_, _params_);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glGetnUniformdvARB (GLuint program, GLint location, GLsizei bufSize, GLdouble *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetnUniformdvARB( JNIEnv* env, jclass klass,jint program_,jint location_,jint bufSize_,jdoubleArray params_){
    if(do_trace)
        fprintf(stderr,"glGetnUniformdvARB\n");
    static PFNGLGETNUNIFORMDVARBPROC impl;
    if( !impl ){
       impl = (PFNGLGETNUNIFORMDVARBPROC) mygetprocaddr(env,"glGetnUniformdvARB");
       if(!impl)
            return;
    }
    jdouble* _params_ = (params_ ? ((*env)->GetDoubleArrayElements(env,params_,NULL) ) : NULL);
    impl(program_, location_, bufSize_, _params_);
    if(params_)
        (*env)->ReleaseDoubleArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glMinSampleShadingARB (GLfloat value);
JNIEXPORT void JNICALL Java_JGL_JGL_glMinSampleShadingARB( JNIEnv* env, jclass klass,jfloat value_){
    if(do_trace)
        fprintf(stderr,"glMinSampleShadingARB\n");
    static PFNGLMINSAMPLESHADINGARBPROC impl;
    if( !impl ){
       impl = (PFNGLMINSAMPLESHADINGARBPROC) mygetprocaddr(env,"glMinSampleShadingARB");
       if(!impl)
            return;
    }
    impl(value_);
}
//GLAPI void APIENTRY glNamedStringARB (GLenum type, GLint namelen, const GLchar *name, GLint stringlen, const GLchar *string);
JNIEXPORT void JNICALL Java_JGL_JGL_glNamedStringARB( JNIEnv* env, jclass klass,jint type_,jint namelen_,jbyteArray name_,jint stringlen_,jbyteArray string_){
    if(do_trace)
        fprintf(stderr,"glNamedStringARB\n");
    static PFNGLNAMEDSTRINGARBPROC impl;
    if( !impl ){
       impl = (PFNGLNAMEDSTRINGARBPROC) mygetprocaddr(env,"glNamedStringARB");
       if(!impl)
            return;
    }
    jbyte* _name_ = (name_ ? ((*env)->GetByteArrayElements(env,name_,NULL) ) : NULL);
    jbyte* _string_ = (string_ ? ((*env)->GetByteArrayElements(env,string_,NULL) ) : NULL);
    impl(type_, namelen_, _name_, stringlen_, _string_);
    if(name_)
        (*env)->ReleaseByteArrayElements(env,name_, _name_,0);
    if(string_)
        (*env)->ReleaseByteArrayElements(env,string_, _string_,0);
}
//GLAPI void APIENTRY glDeleteNamedStringARB (GLint namelen, const GLchar *name);
JNIEXPORT void JNICALL Java_JGL_JGL_glDeleteNamedStringARB( JNIEnv* env, jclass klass,jint namelen_,jbyteArray name_){
    if(do_trace)
        fprintf(stderr,"glDeleteNamedStringARB\n");
    static PFNGLDELETENAMEDSTRINGARBPROC impl;
    if( !impl ){
       impl = (PFNGLDELETENAMEDSTRINGARBPROC) mygetprocaddr(env,"glDeleteNamedStringARB");
       if(!impl)
            return;
    }
    jbyte* _name_ = (name_ ? ((*env)->GetByteArrayElements(env,name_,NULL) ) : NULL);
    impl(namelen_, _name_);
    if(name_)
        (*env)->ReleaseByteArrayElements(env,name_, _name_,0);
}
//GLAPI GLboolean APIENTRY glIsNamedStringARB (GLint namelen, const GLchar *name);
JNIEXPORT jboolean JNICALL Java_JGL_JGL_glIsNamedStringARB( JNIEnv* env, jclass klass,jint namelen_,jbyteArray name_){
    jboolean rv = 0;
    if(do_trace)
        fprintf(stderr,"glIsNamedStringARB\n");
    static PFNGLISNAMEDSTRINGARBPROC impl;
    if( !impl ){
       impl = (PFNGLISNAMEDSTRINGARBPROC) mygetprocaddr(env,"glIsNamedStringARB");
       if(!impl)
            return rv;
    }
    jbyte* _name_ = (name_ ? ((*env)->GetByteArrayElements(env,name_,NULL) ) : NULL);
    rv = impl(namelen_, _name_);
    if(name_)
        (*env)->ReleaseByteArrayElements(env,name_, _name_,0);
    return rv;
}
//GLAPI void APIENTRY glGetNamedStringARB (GLint namelen, const GLchar *name, GLsizei bufSize, GLint *stringlen, GLchar *string);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetNamedStringARB( JNIEnv* env, jclass klass,jint namelen_,jbyteArray name_,jint bufSize_,jintArray stringlen_,jbyteArray string_){
    if(do_trace)
        fprintf(stderr,"glGetNamedStringARB\n");
    static PFNGLGETNAMEDSTRINGARBPROC impl;
    if( !impl ){
       impl = (PFNGLGETNAMEDSTRINGARBPROC) mygetprocaddr(env,"glGetNamedStringARB");
       if(!impl)
            return;
    }
    jbyte* _name_ = (name_ ? ((*env)->GetByteArrayElements(env,name_,NULL) ) : NULL);
    jint* _stringlen_ = (stringlen_ ? ((*env)->GetIntArrayElements(env,stringlen_,NULL) ) : NULL);
    jbyte* _string_ = (string_ ? ((*env)->GetByteArrayElements(env,string_,NULL) ) : NULL);
    impl(namelen_, _name_, bufSize_, _stringlen_, _string_);
    if(name_)
        (*env)->ReleaseByteArrayElements(env,name_, _name_,0);
    if(stringlen_)
        (*env)->ReleaseIntArrayElements(env,stringlen_, _stringlen_,0);
    if(string_)
        (*env)->ReleaseByteArrayElements(env,string_, _string_,0);
}
//GLAPI void APIENTRY glGetNamedStringivARB (GLint namelen, const GLchar *name, GLenum pname, GLint *params);
JNIEXPORT void JNICALL Java_JGL_JGL_glGetNamedStringivARB( JNIEnv* env, jclass klass,jint namelen_,jbyteArray name_,jint pname_,jintArray params_){
    if(do_trace)
        fprintf(stderr,"glGetNamedStringivARB\n");
    static PFNGLGETNAMEDSTRINGIVARBPROC impl;
    if( !impl ){
       impl = (PFNGLGETNAMEDSTRINGIVARBPROC) mygetprocaddr(env,"glGetNamedStringivARB");
       if(!impl)
            return;
    }
    jbyte* _name_ = (name_ ? ((*env)->GetByteArrayElements(env,name_,NULL) ) : NULL);
    jint* _params_ = (params_ ? ((*env)->GetIntArrayElements(env,params_,NULL) ) : NULL);
    impl(namelen_, _name_, pname_, _params_);
    if(name_)
        (*env)->ReleaseByteArrayElements(env,name_, _name_,0);
    if(params_)
        (*env)->ReleaseIntArrayElements(env,params_, _params_,0);
}
//GLAPI void APIENTRY glBufferPageCommitmentARB (GLenum target, GLintptr offset, GLsizei size, GLboolean commit);
JNIEXPORT void JNICALL Java_JGL_JGL_glBufferPageCommitmentARB( JNIEnv* env, jclass klass,jint target_,jint offset_,jint size_,jboolean commit_){
    if(do_trace)
        fprintf(stderr,"glBufferPageCommitmentARB\n");
    static PFNGLBUFFERPAGECOMMITMENTARBPROC impl;
    if( !impl ){
       impl = (PFNGLBUFFERPAGECOMMITMENTARBPROC) mygetprocaddr(env,"glBufferPageCommitmentARB");
       if(!impl)
            return;
    }
    impl(target_, offset_, size_, commit_);
}
//GLAPI void APIENTRY glNamedBufferPageCommitmentEXT (GLuint buffer, GLintptr offset, GLsizei size, GLboolean commit);
JNIEXPORT void JNICALL Java_JGL_JGL_glNamedBufferPageCommitmentEXT( JNIEnv* env, jclass klass,jint buffer_,jint offset_,jint size_,jboolean commit_){
    if(do_trace)
        fprintf(stderr,"glNamedBufferPageCommitmentEXT\n");
    static PFNGLNAMEDBUFFERPAGECOMMITMENTEXTPROC impl;
    if( !impl ){
       impl = (PFNGLNAMEDBUFFERPAGECOMMITMENTEXTPROC) mygetprocaddr(env,"glNamedBufferPageCommitmentEXT");
       if(!impl)
            return;
    }
    impl(buffer_, offset_, size_, commit_);
}
//GLAPI void APIENTRY glNamedBufferPageCommitmentARB (GLuint buffer, GLintptr offset, GLsizei size, GLboolean commit);
JNIEXPORT void JNICALL Java_JGL_JGL_glNamedBufferPageCommitmentARB( JNIEnv* env, jclass klass,jint buffer_,jint offset_,jint size_,jboolean commit_){
    if(do_trace)
        fprintf(stderr,"glNamedBufferPageCommitmentARB\n");
    static PFNGLNAMEDBUFFERPAGECOMMITMENTARBPROC impl;
    if( !impl ){
       impl = (PFNGLNAMEDBUFFERPAGECOMMITMENTARBPROC) mygetprocaddr(env,"glNamedBufferPageCommitmentARB");
       if(!impl)
            return;
    }
    impl(buffer_, offset_, size_, commit_);
}
//GLAPI void APIENTRY glTexPageCommitmentARB (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLboolean resident);
JNIEXPORT void JNICALL Java_JGL_JGL_glTexPageCommitmentARB( JNIEnv* env, jclass klass,jint target_,jint level_,jint xoffset_,jint yoffset_,jint zoffset_,jint width_,jint height_,jint depth_,jboolean resident_){
    if(do_trace)
        fprintf(stderr,"glTexPageCommitmentARB\n");
    static PFNGLTEXPAGECOMMITMENTARBPROC impl;
    if( !impl ){
       impl = (PFNGLTEXPAGECOMMITMENTARBPROC) mygetprocaddr(env,"glTexPageCommitmentARB");
       if(!impl)
            return;
    }
    impl(target_, level_, xoffset_, yoffset_, zoffset_, width_, height_, depth_, resident_);
}
