#!/usr/bin/env python3

#scan the SDL headers and make suitable Java libraries for them

#http://www3.ntu.edu.sg/home/ehchua/programming/java/JavaNativeInterface.html

import os
import re
import subprocess
import sys
import shutil


def tprint(*args):
	#print("--->"," ".join([str(q) for q in args]))
    pass  
      
      
      
      
      
      
      

def eventparse(data,outfp):

    data = re.sub(r"(?s)/\*.*?\*/","",data)
    
    rex = re.compile(r"(?ms)typedef struct (\w+)\s*\{(.*?)^}\s*\w+\s*;")
    i=0
    snames=[]
    while 1:
        m = rex.search(data,i)
        if not m:
            break 
        i=m.end()
        
        sname = m.group(1).strip()
        if sname in ["SDL_TouchFingerEvent","SDL_MultiGestureEvent","SDL_DollarGestureEvent",
            "SDL_UserEvent","SDL_SysWMEvent","SDL_TextEditingEvent","SDL_TextInputEvent","SDL_DropEvent"]:
            continue 

        snames.append(sname)
        
        sitems = m.group(2)
        
        sitems = sitems.split(";")
        
        outfp.write("        public static class "+sname+" {\n")
        
        items=[]
        offset=0
        for item in sitems:
            item=item.strip()
            if len(item) == 0:
                continue
            tmp = item.split()
            if len(tmp) != 2:
                print(item)
                assert 0
            ty,nam = tmp
            ty=ty.strip()
            nam=nam.strip()
            if ty in ["Uint32","Sint32","SDL_JoystickID"]:
                sz=4
            elif ty in ["Sint16","Uint16"]:
                sz=2
            elif ty in ["Uint8","char"]:
                sz=1
            elif ty == "SDL_Keysym":
                sz=14
            else:
                print(sname)
                print(item)
                print(ty)
                assert 0
                
            items.append( (nam,sz,offset,ty) )
            offset += sz
            
            if ty != "SDL_Keysym":
                outfp.write("            public int "+nam+";\n")
            else:
                outfp.write("            public static class SDL_Keysym {\n")
                outfp.write("                public int scancode;\n")
                outfp.write("                public int sym;\n")
                outfp.write("                public int mod;\n")
                outfp.write("            }\n")
                outfp.write("            public SDL_Keysym keysym = new SDL_Keysym();\n")
                
        
        outfp.write("            public "+sname+"(byte[] b){\n")
        outfp.write("                int b1,b2,b3,b4;\n")
        for nam,sz,offset,ty in items:
            #assuming little endian
            if sz == 1:
                outfp.write("                this."+nam+" = b["+str(offset)+"]; this."+nam+" &= 0xff;\n")
            elif sz == 2:
                outfp.write("                b1 = b["+str(offset)+"];\n")
                outfp.write("                b2 = b["+str(offset+1)+"];\n")
                outfp.write("                b1 &= 0xff; b2 &= 0xff;\n")
                outfp.write("                this."+nam+" = b1 | (b2<<8);\n")
                if ty == "Uint16":
                    pass
                elif ty == "Sint16":
                    outfp.write("                if( 0 != (b2 & 0x80) ) this."+nam+" |= 0xffff0000;\n")
                else:
                    assert 0
            elif sz == 4:
                outfp.write("                b1 = b["+str(offset)+"];\n")
                outfp.write("                b2 = b["+str(offset+1)+"];\n")
                outfp.write("                b3 = b["+str(offset+2)+"];\n")
                outfp.write("                b4 = b["+str(offset+3)+"];\n")
                outfp.write("                b1 &= 0xff; b2 &= 0xff; b3 &= 0xff; b4 &= 0xff;\n")
                outfp.write("                this."+nam+" = b1 | (b2<<8) | (b3<<16) | (b4<<24);\n")
            elif ty == "SDL_Keysym":
                #int scancode, int sym, int16 mod, int unused
                outfp.write("                b1 = b["+str(offset)+"];\n")
                outfp.write("                b2 = b["+str(offset+1)+"];\n")
                outfp.write("                b3 = b["+str(offset+2)+"];\n")
                outfp.write("                b4 = b["+str(offset+3)+"];\n")
                outfp.write("                b1 &= 0xff; b2 &= 0xff; b3 &= 0xff; b4 &= 0xff;\n")
                outfp.write("                this.keysym.scancode = b1 | (b2<<8) | (b3<<16) | (b4<<24);\n")
                outfp.write("                b1 = b["+str(offset+4)+"];\n")
                outfp.write("                b2 = b["+str(offset+5)+"];\n")
                outfp.write("                b3 = b["+str(offset+6)+"];\n")
                outfp.write("                b4 = b["+str(offset+7)+"];\n")
                outfp.write("                b1 &= 0xff; b2 &= 0xff; b3 &= 0xff; b4 &= 0xff;\n")
                outfp.write("                this.keysym.sym = b1 | (b2<<8) | (b3<<16) | (b4<<24);\n")
                outfp.write("                b1 = b["+str(offset+8)+"];\n")
                outfp.write("                b2 = b["+str(offset+9)+"];\n")
                outfp.write("                b1 &= 0xff; b2 &= 0xff;\n")
                outfp.write("                this.keysym.mod = b1 | (b2<<8);\n")
                
            else:
                assert 0
                
        outfp.write("            }\n")
        
        outfp.write("        }\n")
        
        
    outfp.write("""
        public static class SDL_Event {\
            public int type;                       /**< Event type, shared with all events */
            public SDL_CommonEvent common;         /**< Common event data */
            public SDL_WindowEvent window;         /**< Window event data */
            public SDL_KeyboardEvent key;          /**< Keyboard event data */
            //public SDL_TextEditingEvent edit;      /**< Text editing event data */
            //public SDL_TextInputEvent text;        /**< Text input event data */
            public SDL_MouseMotionEvent motion;    /**< Mouse motion event data */
            public SDL_MouseButtonEvent button;    /**< Mouse button event data */
            public SDL_MouseWheelEvent wheel;      /**< Mouse wheel event data */
            public SDL_JoyAxisEvent jaxis;         /**< Joystick axis event data */
            public SDL_JoyBallEvent jball;         /**< Joystick ball event data */
            public SDL_JoyHatEvent jhat;           /**< Joystick hat event data */
            public SDL_JoyButtonEvent jbutton;     /**< Joystick button event data */
            public SDL_JoyDeviceEvent jdevice;     /**< Joystick device change event data */
            public SDL_ControllerAxisEvent caxis;      /**< Game Controller axis event data */
            public SDL_ControllerButtonEvent cbutton;  /**< Game Controller button event data */
            public SDL_ControllerDeviceEvent cdevice;  /**< Game Controller device event data */
            public SDL_QuitEvent quit;             /**< Quit request event data */
            //SDL_UserEvent user;             /**< Custom event data */
            //SDL_SysWMEvent syswm;           /**< System dependent window event data */
            //SDL_TouchFingerEvent tfinger;   /**< Touch finger event data */
            //SDL_MultiGestureEvent mgesture; /**< Gesture event data */
            //SDL_DollarGestureEvent dgesture; /**< Gesture event data */
            //public SDL_DropEvent drop;             /**< Drag and drop event data */
            
            public void fromBytes(byte[] b){
                type = b[0] | (b[1]<<8) | (b[2]<<16) | (b[3]<<24);
                switch(type){
                    case SDL_WINDOWEVENT:
                        window = new SDL_WindowEvent(b);
                        break;
                    case SDL_KEYDOWN:
                    case SDL_KEYUP:
                    //case SDL_TEXTEDITING:
                    //case SDL_TEXTINPUT:
                        key = new SDL_KeyboardEvent(b);
                        break;
                    case SDL_MOUSEMOTION:
                        motion = new SDL_MouseMotionEvent(b);
                        break;
                    case SDL_MOUSEBUTTONDOWN:
                    case SDL_MOUSEBUTTONUP:
                        button = new SDL_MouseButtonEvent(b);
                        break;
                    case SDL_MOUSEWHEEL:
                        wheel = new SDL_MouseWheelEvent(b);
                        break;
                    case SDL_JOYAXISMOTION:
                        jaxis = new SDL_JoyAxisEvent(b);
                        break;
                    case SDL_JOYBALLMOTION:
                        jball = new SDL_JoyBallEvent(b);
                        break;
                    case SDL_JOYHATMOTION:
                        jhat = new SDL_JoyHatEvent(b);
                        break;
                    case SDL_JOYBUTTONDOWN:
                    case SDL_JOYBUTTONUP:
                        jbutton = new SDL_JoyButtonEvent(b);
                        break;
                    case SDL_JOYDEVICEADDED:
                    case SDL_JOYDEVICEREMOVED:
                        jdevice = new SDL_JoyDeviceEvent(b);
                    case SDL_CONTROLLERAXISMOTION:
                        caxis = new SDL_ControllerAxisEvent(b);
                        break;
                    case SDL_CONTROLLERBUTTONDOWN:
                    case SDL_CONTROLLERBUTTONUP:
                        cbutton = new SDL_ControllerButtonEvent(b);
                        break;
                    case SDL_CONTROLLERDEVICEADDED:
                    case SDL_CONTROLLERDEVICEREMOVED:
                    case SDL_CONTROLLERDEVICEREMAPPED:
                        cdevice = new SDL_ControllerDeviceEvent(b);
                        break;
                    case SDL_QUIT:
                        quit = new SDL_QuitEvent(b);
                        break;
                }
            }
        }
    """)
    
      
      
      
      
      
      
      
      
      
        
def main():
    done=set()

    
    #ignore a few headers...
    #FIXME: Figure out how to parse these...
    done.add("SDL_platform.h")
    done.add("SDL_config.h")
    done.add("SDL_stdinc.h")
    done.add("SDL_main.h")
    done.add("begin_code.h")
    done.add("close_code.h")
    done.add("SDL_assert.h")
    done.add("SDL_atomic.h")
    done.add("SDL_endian.h")
    done.add("SDL_mutex.h")
    done.add("SDL_thread.h")
    done.add("SDL_rwops.h")
    done.add("SDL_audio.h")     #use Java audio system instead
    done.add("SDL_cpuinfo.h")
    #done.add("SDL_pixels.h")
    done.add("SDL_rect.h")
    #done.add("SDL_surface.h")
    done.add("SDL_system.h")
    done.add("SDL_timer.h")
    done.add("SDL_version.h")
    done.add("SDL_opengl.h")
    done.add("SDL_opengles.h")
    done.add("SDL_opengles2.h")


    #(funcname, return type, params, original prototype)
    functions=[]

    #struct name -> list of (type,name) pairs
    structs={ 
        "SDL_Palette":None,
        "SDL_RWops":None,
        "SDL_BlendMode":None,
        "SDL_Rect":None,
        "SDL_Event":None,
        "SDL_HapticEffect":None,
        "SDL_Point":None
        
    }
    
    

    fblacklist=["SDL_SetError","SDL_GetClipboardText","SDL_SetClipboardText","SDL_GetError",
        "SDL_Log","SDL_LogVerbose","SDL_LogDebug","SDL_LogInfo","SDL_LogWarn","SDL_LogError",
        "SDL_LogCritical","SDL_LogMessage","SDL_GameControllerGetBindForAxis",
        "SDL_GameControllerGetBindForButton","SDL_SetEventFilter","SDL_GetEventFilter",
        "SDL_AddEventWatch","SDL_DelEventWatch","SDL_FilterEvents",
        "SDL_AddHintCallback","SDL_DelHintCallback","SDL_LogMessageV","SDL_LogGetOutputFunction",
        "SDL_LogSetOutputFunction",
        "SDL_GetKeyboardState",      #this one requires its own implementation
        "SDL_PollEvent",
        "SDL_WaitEvent",
        "SDL_CreateWindowAndRenderer","SDL_LockTexture",
        "SDL_JoystickGetGUID",
        "SDL_JoystickGetDeviceGUID",
        "SDL_JoystickGetGUIDString",
        "SDL_JoystickGetGUIDFromString",
        "SDL_GameControllerMappingForGUID"
        

    ]

    #enum types
    enumtypes=set()
    
    jfp=open("JSDL.java","w")
    jfp.write("""
    package JSDL;
    import java.io.*;
    
    //This file contains data from SDL 2.0.3. The copyright on SDL is:
    /*""")
    
    jfp.write(open(os.path.join("..","SDL2-2.0.3","COPYING.txt")).read())
    jfp.write(""" */
    public class JSDL {
""")
    
    eventparse(open(os.path.join("..","SDL2-2.0.3","include","SDL_events.h")).read(),jfp)


    
    jfp.write("""
   
		static{
            JGLSDLUtils.JGLSDLUtils.extractAndLoad(JSDL.class,"SDL2");
            JGLSDLUtils.JGLSDLUtils.extractAndLoad(JSDL.class,"JSDL");
        }
     
        //public static native void SDL_Init(int what);
        public static long SDL_CreateWindow(String title, int x, int y, int w, int h, int flags){
            byte[] b1 = title.getBytes();
            byte[] b2 = new byte[b1.length+1];
            for(int i=0;i<b1.length;++i)
                b2[i]=b1[i];
            return my_SDL_CreateWindow(b2,x,y,w,h,flags);
        }
            
        private static native long my_SDL_CreateWindow(byte[] title, int x, int y, int w, int h, int flags);
        
        public static int SDL_PollEvent(SDL_Event ev){
            byte[] tmp = new byte[512];
            int rv = my_SDLPollEvent(tmp);
            if( rv != 0 )
                ev.fromBytes(tmp);
            return rv;
            
        }
        private static native int my_SDLPollEvent(byte[] b);
        
        public static int SDL_WaitEvent(SDL_Event ev){
            byte[] tmp = new byte[56];
            int rv = my_SDLWaitEvent(tmp);
            if( rv != 0 )
                ev.fromBytes(tmp);
            return rv;
        }
        private static native int my_SDLWaitEvent(byte[] b);

        public static int SDL_WaitEventTimeout(SDL_Event ev,int timeout){
            byte[] tmp = new byte[56];
            int rv = my_SDLWaitEventTimeout(tmp,timeout);
            if( rv != 0 )
                ev.fromBytes(tmp);
            return rv;
        }
        private static native int my_SDLWaitEventTimeout(byte[] b,int timeout);

        
        //public static native int SDL_GL_SetAttribute(int attr, int value);
        //public static native long SDL_GL_CreateContext(long win);
        //public static native void SDL_GL_SwapWindow(long win);
        //public static native int SDL_GL_MakeCurrent(long win, long ctx);
        //public static native int SDL_GL_GetAttribute(int attr, int[] value);
        
        
        
        
        
        public static int SDL_BUTTON(int y){ 
            return (1<<((y)-1));
        }
        public static int SDL_SCANCODE_TO_KEYCODE(int y){
            return y | SDLK_SCANCODE_MASK;
        }
        public static int SDL_WINDOWPOS_UNDEFINED_DISPLAY(int y){
            return y | SDL_WINDOWPOS_UNDEFINED_MASK;
        }
        public static int SDL_WINDOWPOS_CENTERED_DISPLAY(int y){
            return y | SDL_WINDOWPOS_CENTERED_MASK;
        }
        
        //dummy
        public static final int SDL_Color = 0;
        public static final int SDL_UpperBlit = 0;
        public static final int SDL_UpperBlitScaled = 0;
        
    """)


    cfp = open("JSDL.c","w")
    cfp.write("""
    #include "JSDL_JSDL.h"
    #include <SDL.h>
    #include <stdlib.h>
    #include <string.h>

        
    JNIEXPORT void JNICALL zzzJava_JSDL_JSDL_SDL_1Init(JNIEnv* env, jclass klass, jint what){
        SDL_Init(what);
    }

    JNIEXPORT jlong JNICALL Java_JSDL_JSDL_my_1SDL_1CreateWindow(JNIEnv* env, jclass klass,
        jbyteArray title, jint x, jint y, jint w, jint h, jint flags){
        
        jbyte* p = (*env)->GetByteArrayElements(env,title,NULL);
        
        SDL_Window* win = SDL_CreateWindow((char*)p, x,y,w,h,flags);
        jlong L;
        memcpy(&L,&win,sizeof(win));
        
        (*env)->ReleaseByteArrayElements(env,title,p,0);

        return L;
    }

    JNIEXPORT jint JNICALL Java_JSDL_JSDL_my_1SDLPollEvent(JNIEnv* env, jclass klass, jbyteArray arr){
        jint rv;
        SDL_Event ev;
        rv = SDL_PollEvent(&ev);
        jsize asize = (*env)->GetArrayLength(env,arr);
        if( asize > sizeof(ev) )
            asize = sizeof(ev);
        (*env)->SetByteArrayRegion(env,arr,0,asize,(jbyte*)&ev);
        return rv;
    }
    JNIEXPORT jint JNICALL Java_JSDL_JSDL_my_1SDLWaitEvent(JNIEnv* env, jclass klass, jbyteArray arr){
        jint rv;
        SDL_Event ev;
        rv = SDL_WaitEvent(&ev);
        jsize asize = (*env)->GetArrayLength(env,arr);
        if( asize > sizeof(ev) )
            asize = sizeof(ev);
        (*env)->SetByteArrayRegion(env,arr,0,asize,(jbyte*)&ev);
        return rv;
    }
    JNIEXPORT jint JNICALL Java_JSDL_JSDL_my_1SDLWaitEventTimeout(JNIEnv* env, jclass klass, jbyteArray arr, jint timeout){
        jint rv;
        SDL_Event ev;
        rv = SDL_WaitEventTimeout(&ev,timeout);
        jsize asize = (*env)->GetArrayLength(env,arr);
        if( asize > sizeof(ev) )
            asize = sizeof(ev);
        (*env)->SetByteArrayRegion(env,arr,0,asize,(jbyte*)&ev);
        return rv;
    }

    JNIEXPORT jint JNICALL zzzJava_JSDL_JSDL_SDL_1GL_1SetAttribute(JNIEnv* env, jclass klass, jint attr, jint val){
        return SDL_GL_SetAttribute(attr,val);
    }

    JNIEXPORT jlong JNICALL zzzJava_JSDL_JSDL_SDL_1GL_1CreateContext(JNIEnv* env, jclass klass, jlong win){
        SDL_Window* winp;
        memcpy(&winp,&win,sizeof(winp));
        SDL_GLContext ctx = SDL_GL_CreateContext(winp);
        jlong L;
        memcpy(&L,&ctx,sizeof(ctx));
        return L;
    }

    JNIEXPORT void JNICALL zzzJava_JSDL_JSDL_SDL_1GL_1SwapWindow(JNIEnv* env, jclass klass, jlong win){
        SDL_Window* winp;
        memcpy(&winp,&win,sizeof(winp));
        SDL_GL_SwapWindow(winp);
        return;
    }

    JNIEXPORT jint JNICALL zzzJava_JSDL_JSDL_SDL_1GL_1MakeCurrent(JNIEnv* env, jclass klass, jlong win, jlong ctx){
        SDL_Window* winp;
        memcpy(&winp,&win,sizeof(winp));
        SDL_GLContext ctxp;
        memcpy(&ctxp,&ctx,sizeof(ctxp));
        jint rv = SDL_GL_MakeCurrent(winp,ctxp);
        return rv;
    }


    JNIEXPORT jint JNICALL zzzJava_JSDL_JSDL_SDL_1GL_1GetAttribute(JNIEnv* env, jclass klass, jint key, jintArray val){
        int v;
        jint rv = SDL_GL_GetAttribute(key,&v);
        jint jv = v;
        (*env)->SetIntArrayRegion(env,val,0,1,&jv);    
        return rv;
    }


    """)

    def sdltype_to_java_type(s):
        s=s.replace("const","")
        
        s=s.strip()
        
        if s in ["int","float","double","void"]:
            return s
            
        M={ 
            "const char*" : "byte[]",
            "char*":"byte[]",
            "SDL_bool":"int",
            "Uint32":"int",
            "SDL_Keycode":"int",
            "SDL_JoystickGUID":"byte[]",
            "SDL_JoystickID":"int",
            "Sint16":"short",
            "Uint8":"byte",
            "SDL_TouchID":"long",
            "unsigned int":"int",
            "SDL_GestureID":"long"
        }

        if s in M:
            return M[s]
            
        #pointer-to-struct
        if s == "SDL_GLContext" or (s.endswith("*") and (s[:-1] in structs or s[:-1]=="void")):
            return "long"
        
        if s in enumtypes:
            return "int"
            
        if s == "int*":
            return "int[]"
            
        if s == "Uint16*":
            return "short[]"
            
        if s == "Uint8*":
            return "byte[]"
            
        if s == "Uint32*":
            return "int[]"
            
        if s == "float*":
            return "float[]"
            
        print("!!sdltype to java type:",s)
        print(sorted(structs.keys()))
        assert 0

    def sdltype_to_jni_type(s):
        t = sdltype_to_java_type(s)
        if t == "void":
            return t
        if t in ["int","float","double","long","short","byte"]:
            return "j"+t
        if t == "String":
            return "jstring"
        if t == "byte[]":
            return "jbyteArray"
        if t == "int[]":
            return "jintArray"
        if t == "float[]":
            return "jfloatArray"
        if t == "short[]":
            return "jshortArray"
            
        print("!!sdltype to    ***JNI***  type:",s,t)
        assert 0

        
        
    
    
    
    fblacklist=set(fblacklist)

    def process(fname):
        if fname in done:
            return
        done.add(fname)
        
        tprint("----------process ",fname,"------------")
            
        fp=open(os.path.join("..","SDL2-2.0.3","include",fname))
        data = fp.read()
        
        #eliminate comments
        data = re.sub(r"(?s)/\*(.*?)\*/","",data)
        
        #eliminate line continuation
        data = data.replace("\\\n"," ")

        #eliminate multiple consecutive newlines
        data = re.sub(r"\n+","\n",data)
        
        def charsubber(m):
            x=eval(m.group(0))
            return str(ord(x))
            
        #eliminate character constants
        data = re.sub(r"'(\\.|[^'])'",charsubber,data)
        
        data = data.split("\n")
        while len(data) > 0:
            line = data.pop(0)
        
            line=line.replace(" *","* ")
            line=line.strip()

            
            if line=="":
                tprint("empty line")
                continue

            if line.startswith("#ifndef"):
                tprint("#ifndef",line)
                continue
            if line.startswith("#ifdef"):
                tprint("#ifdef",line)
                continue
            if line.startswith("#endif"):
                tprint("#endif",line)
                continue
                
            if line=="}":
                tprint("}")
                continue

            m=re.match(r"#define \w+\(",line)
            if m:
                tprint("#define of complex thing:",line)
                continue
                
            m=re.match(r"#define \w+$",line)
            if m:
                tprint("empty define",line)
                continue

            m=re.match(r"#define (\w+) (.*)",line)
            if m:
                tprint("useful define",line)
                if line.find("SDL_DEFINE_PIXELFORMAT") != -1:
                    tprint("skipping this define")
                    continue
                    
                k=m.group(1).strip()
                v=m.group(2).strip()
                if v == "4294967295U":
                    v="-1"
                elif v == "((Uint32)-1)":
                    v="-1"
                if v.startswith('"'):
                    jfp.write("    public static final String "+k+" = "+v+";\n")
                else:
                    jfp.write("    public static final int "+k+" = "+v+";\n")
                continue
                
            m=re.match(r'#include "([^"]+)"',line)
            if m:
                tprint("include:",line)
                process(m.group(1))
                tprint("resuming",fname)
                continue
            
            if line.startswith('extern "C"'):
                tprint("extern C")
                continue
                
            m = re.match("struct \w+;",line)
            if m:
                tprint("useless struct decl",line)
                continue
                
            m = re.match(r"extern DECLSPEC (.*)SDLCALL\b",line)
            if m:
                tmp=line.strip()
                while line.find(";") == -1:
                    line = data.pop(0)
                    tmp += " "+line.strip()
                    
                line=tmp.replace("\n"," ")
                tprint("function decl",line)
                    
                m = re.match(r"extern DECLSPEC (.*)SDLCALL (\w+)\s*\((.*)\);",line)
                if not m:
                    print("func decl doesn't match what we think")
                    print("---->"+line+"<-----")
                    assert 0
                    
                rettype = m.group(1).strip()
                rettype = rettype.replace(" *","* ").strip()
                funcname = m.group(2).strip()
                
                if funcname in fblacklist:
                    continue
                    
                if m.group(3) == "" or m.group(3) == "void":
                    params=[]
                else:
                    params = m.group(3).split(",")
                tprint("\t","rettype=",rettype,"funcname=",funcname)
                
                javarettype = sdltype_to_java_type(rettype)
                jfp.write("    public static native "+javarettype+" "+funcname+"(")
                
                jnirettype = sdltype_to_jni_type(rettype)
                cfp.write("JNIEXPORT "+jnirettype+" "+
                    "JNICALL "+
                    "Java_JSDL_JSDL_"+funcname.replace("_","_1")+"(")
                cfp.write("JNIEnv* env, jclass klass")
                if len(params) > 0:
                    cfp.write(",")
                    
                flag=False
                jtylist=[]
                ctylist=[]
                namlist=[]
                tylist=[]
                for param in params:
                    param=param.strip()
                    param=param.replace(" *","* ")
                    i=param.rfind(" ")
                    if i == -1:
                        print("Parameter doesn't have spaces")
                        print(funcname)
                        print(param)
                        assert 0
                    ty = param[:i].strip()
                    tylist.append(ty)
                    jty = sdltype_to_java_type(ty)
                    jtylist.append(jty)
                    cty = sdltype_to_jni_type(ty)
                    ctylist.append(cty)
                    nam = param[i+1:].strip()
                    namlist.append(nam)
                    if flag:
                        jfp.write(",")
                        cfp.write(",")
                    flag=True
                    tprint("\t","param:",ty,nam)
                    jfp.write( jty+" "+nam )
                    cfp.write( cty+" "+nam )
                
                cfp.write("){\n")
                jfp.write(");\n")
                
                
                if jnirettype != "void":
                    cfp.write("    "+rettype+" rv;\n")
                    
                cparams=[]
                needarrayrelease=[]
                for i in range(len(params)):
                    nam = namlist[i]
                    if jtylist[i].find("[]") != -1:
                        #we need to dereference the array
                        tmp=ctylist[i].replace("Array","")
                        ptmp = tmp[1:]
                        ptmp = ptmp[0].upper()+ptmp[1:]
                        cfp.write("    "+tmp+"* _"+nam+";\n")
                        cfp.write("    _"+nam+" = (*env)->Get"+ptmp+"ArrayElements(env,"+nam+",NULL);\n")
                        cparams.append("_"+nam)
                        needarrayrelease.append( (nam,ptmp ) )
                    elif tylist[i].endswith("*") or tylist[i] == "SDL_GLContext":
                        if jtylist[i] != "long" :
                            print(jtylist[i])
                            assert 0
                        #its a pointer
                        cfp.write("    "+tylist[i].replace("const","")+" _"+nam+";\n")
                        cfp.write("    memcpy(& _"+nam+", &"+nam+", sizeof(_"+nam+") );\n")
                        cparams.append("_"+nam)
                    else:
                        #use parameter as-is
                        cparams.append(namlist[i])
                        
                cfp.write("    ")
                if jnirettype != "void":
                    cfp.write("rv=")
                cfp.write(funcname+"(")
                cfp.write(",".join(cparams))
                cfp.write(");\n")
                
                for nam,ptmp in needarrayrelease:
                    cfp.write("    (*env)->Release"+ptmp+"ArrayElements(env,"+nam+", _"+nam+",0);\n")
                    
                if jnirettype != "void":
                    if rettype == "const char*" or rettype == "char*":
                        cfp.write("    int slen = (int)strlen(rv);\n")
                        cfp.write("    jbyteArray rv_ = (*env)->NewByteArray(env,slen);\n")
                        cfp.write("    return rv_;")
                    elif rettype in ["unsigned int","int","float","double","SDL_bool","Uint32","Sint16","Uint8"]:
                        cfp.write("    return rv;\n")
                    elif (jnirettype == "jlong" and rettype.endswith("*")) or rettype == "SDL_GLContext":
                        cfp.write("    jlong rv_;\n")
                        cfp.write("    memcpy(&rv_,&rv,sizeof(rv_));\n")
                        cfp.write("    return rv_;\n")
                    elif (rettype in enumtypes or rettype in ["SDL_Keycode","SDL_JoystickID"]) and jnirettype == "jint":
                        cfp.write("    return (jint) rv;\n")
                    elif rettype == "SDL_JoystickGUID" and jnirettype == "jbyteArray":
                        cfp.write("    jbyteArray rv_ = (*env)->NewByteArray(env,16);\n")
                        cfp.write("    (*env)->SetByteArrayRegion(env,rv_,0,16,rv);\n")
                        cfp.write("    return rv_;\n")
                    elif rettype == "SDL_TouchID":
                        cfp.write("    return (long)rv;\n")
                    else:
                        print(funcname)
                        print(rettype,jnirettype)
                        print(enumtypes)
                        assert 0
                    
                cfp.write("}\n")
                
                continue
                    

                
            if line.startswith("typedef enum") or line.startswith("enum"):
                tprint("typedef enum")
                if line.find("{") == -1:
                    line=data.pop(0)
                    assert line.startswith("{")
                tmp=""
                while len(data) > 0:
                    line = data.pop(0)
                    tprint(line)
                    line=line.strip()
                    if line.startswith("}"):
                        name=line[1:].strip()
                        assert name.endswith(";")
                        name=name[:-1]
                        if len(name) > 0:
                            ename = name
                            enumtypes.add(ename)
                        else:
                            ename=None
                        tprint("end typedef enum for",ename )
                        break
                    else:
                        tmp += line
                        
                if tmp.find("SDL_DEFINE_PIXELFORMAT") != -1:
                    print("Skipping enum")
                    continue
                    
                base="0"
                counter=0
                for e in tmp.split(","):
                    e=e.strip()
                    ei = e.find("=")
                    if ei ==  -1:
                        jfp.write("    public static final int "+e+" = ("+base+") + "+str(counter)+";\n")
                        counter += 1
                    else:
                        left = e[:ei].strip()
                        right = e[ei+1:].strip()
                        jfp.write("    public static final int "+left+" = ("+right+");\n")
                        base=right
                        counter=1
                continue
            
            if line.startswith("typedef struct"):
                tprint("typedef of a struct:",line)
                
                if line.endswith(";"):
                    tprint("Dummy struct typedef:",line)
                    line=line.split()
                    sname = line[3]
                    assert sname.endswith(";")
                    sname=sname[:-1]
                    if sname not in structs:
                        structs[sname]=None
                else:
                    if line.find("{") == -1:
                        line=data.pop(0).strip()
                        if line != "{":
                            print(line)
                            assert 0
                    sfields=[]
                    while 1:
                        line = data.pop(0).strip()
                        tprint("struct field:",line)
                        if len(line) == 0:
                            pass
                        elif line.startswith("union"):
                            if line.find("{") == -1:
                                line += data.pop(0)
                            while line.count("{") != line.count("}"):
                                line += " "+data.pop(0)
                            print("union:",line)
                            #FIXME
                        elif line[0] == '}':
                            sname = line[1:].strip()
                            assert sname[-1] == ';'
                            sname=sname[:-1]
                            break
                        else:
                            line=line.replace(" *","* ")
                            i = line.rfind(" ")
                            assert i != -1
                            typ = line[:i].strip()
                            nam = line[i+1:].strip()
                            assert nam.endswith(";")
                            nam=nam[:-1]
                            sfields.append( (typ,nam) )
                    tprint(sname)
                    tprint(sfields)
                    structs[sname]=sfields
                continue 
                

            if line.startswith("typedef union "):
                tprint("Ignoring union",line)
                while not line.startswith("}"):
                    line = data.pop(0)
                continue 

            if line.startswith("typedef int (SDLCALL* "):
                while line.find(";") == -1:
                    line += data.pop(0)
                tprint("Callback typedef",line)
                continue

            m=re.match(r"typedef \w+ \(\*SDL_",line)
            if m:
                while line.find(";") == -1:
                    line += data.pop(0)
                tprint("Callback typedef",line)
                continue

                
            if line.startswith("typedef"):
                line=line.strip()
                m = re.match(r"typedef \S+ \w+;",line)
                if not m:
                    print("unrecognized typedef:",line)
                    assert 0
                continue
              
              
                
                
            print("Unrecognized line:")
            print(line)
            print("Next few lines:")
            print(data[:5])
            assert 0
            
    process("SDL.h")
    #process("SDL_joystick.h")

    jfp.write("}\n")

    jfp.close()
    cfp.close()


if __name__=="__main__":
    main()
