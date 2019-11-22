
#this file contains code from the Wine library. See the copyright/license notice below.
#The license also pertains to this file.


# From Wine's winuser.rh file:
"""/*
 * Copyright (C) the Wine project
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */
"""

#from Wine's wingdi.h file:
"""
/*
 * Copyright (C) the Wine project
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

"""

#not from ms

import os
import sys
from ctypes import *
import ctypes
import tempfile
import threading
import queue
import time
import OpenGL
import OpenGL.GL as GL

wmdict={}


_recording_video = False
_last_frame_time=0
_wintype=None       #1=GL, 2=DX
_worker=None

class _Worker(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
        self.tempdir = tempfile.TemporaryDirectory()
        self.currfile=None
        self.Q = queue.Queue()
        
    def run(self):
        print("R")
        while 1:
            buff = self.Q.get()
            if type(buff) == str:
                #indicates we're starting a new video
                if self.currfile:
                    #failed to close the old file first!
                    self.done()
                self.currfile = buff
            elif type(buff) == None:
                #indicates we're done with this video
                self.done()
                self.currfile=None
            else:
                #it's a frame
                if not self.currfile:
                    assert 0
                #encode it!
                print("enc")
            
    def done(self):
        pass
    def put(self,item):
        self.Q.put(item)
        

def ToggleVideo(fname):
    if _recording_video:
        EndVideo()
    else:
        BeginVideo(fname)
        
def BeginVideo(fname):
    global _recording_video,_worker
    _recording_video=True
    if not _worker:
        _worker = _Worker()
        _worker.start()
        
    _worker.put(fname)
    print("Recording")
    
def EndVideo():
    _worker.put(None)
    _recording_video=False
    print("Stop recording")
    
def SwapBuffers(win_or_hdc):
    global _last_frame_time, _recording_video
    if _recording_video:
        now = time.time()
        
        #67 msec per frame = 15 fps
        if now - _last_frame_time >= 0.067:
            if _wintype == 1: 
                #GL window
                buf = ctypes.create_string_buffer(_winwidth*_winheight*4)
                GL.glReadPixels(0,0,_winwidth, _winheight, 
                    GL.GL_RGBA, GL.GL_UNSIGNED_BYTE, byref(buf))
                _worker.put(buf)
            else:
                #DX not yet implemented
                assert 0
                
            _last_frame_time = now
            
    _SwapBuffers(win_or_hdc)
    
    
if sys.platform.find("win32") != -1:
    from ctypes.wintypes import *
    NULL=None
    TRUE=1
    FALSE=0
    S_OK=0
    D3D_OK=0

    WNDPROC = WINFUNCTYPE( LONG, HWND, UINT, WPARAM, LPARAM)
    HCURSOR=HANDLE


    class WNDCLASS(Structure):
        _fields_ = [ 
            ("style",UINT),
            ("lpfnWndProc",WNDPROC),
            ("cbClsExtra",INT),
            ("cbWndExtra",INT),
            ("hInstance",HANDLE),
            ("hIcon",HICON),
            ("hCursor",HCURSOR),
            ("hbrBackground",HBRUSH),
            ("lpszMenuName",LPCSTR),
            ("lpszClassName",LPCSTR)
        ]



    DefWindowProc = WINFUNCTYPE( LONG, HWND, UINT, 
        WPARAM, LPARAM )( ("DefWindowProcA",windll.user32 ) )

    RegisterClass = WINFUNCTYPE( ATOM, c_void_p )( 
        ("RegisterClassA",windll.user32) 
        )

    #user32 only ms
    ValidateRect = WINFUNCTYPE( BOOL, HWND, POINTER(RECT) )( ("ValidateRect", windll.user32) )

    #doc only ms
    CreateWindowEx = WINFUNCTYPE( HWND, 
        DWORD, #extra style
        DWORD, #class name
        LPCSTR, #win name
        DWORD, #style
        INT, #x
        INT, #y
        INT, #w
        INT, #h
        HWND, #parent
        HMENU, #menu
        HINSTANCE, #instance
        LPVOID #lparam
    )( 
            ("CreateWindowExA",windll.user32) 
        )

    GetModuleHandle = WINFUNCTYPE(HMODULE,LPCSTR)( ("GetModuleHandleA",windll.kernel32) )

    WaitMessage = WINFUNCTYPE(BOOL)( ("WaitMessage",windll.user32) )

    ShowWindow = WINFUNCTYPE(BOOL,HWND,INT)( ("ShowWindow",windll.user32) )

    GetMessage = WINFUNCTYPE(BOOL,POINTER(MSG),HWND,UINT,UINT)( ("GetMessageA",windll.user32) )

    #doc only ms
    PeekMessage = WINFUNCTYPE(BOOL,
        POINTER(MSG),
        HWND,
        UINT,UINT, #filter min/max
        UINT    #remove message?
    )( ("PeekMessageA",windll.user32) )
    TranslateMessage = WINFUNCTYPE(BOOL,POINTER(MSG))( ("TranslateMessage",windll.user32) )
    DispatchMessage = WINFUNCTYPE(LONG,POINTER(MSG))(("DispatchMessageA",windll.user32))
    LoadCursor = WINFUNCTYPE( HCURSOR, HINSTANCE, LPCSTR)(("LoadCursorA",windll.user32))
    
    _GetClientRect = WINFUNCTYPE(BOOL,HWND,c_void_p)(("GetClientRect",windll.user32))

    def GetClientRect(hwnd,lprect):
        return _GetClientRect(hwnd,lprect)

    def _winproc_(win,msg,wp,lp):
        if msg == WM_DESTROY or msg == WM_QUIT or msg == WM_CLOSE:
            sys.exit(0)
        return DefWindowProc(win,msg,wp,lp)
        
        
    _winproc = WNDPROC(_winproc_)
    _win=None
    _cl=None
    def CreateWindow(title, x, y, width,height):
        global _win,_cl
        if _cl == None:
            cl = WNDCLASS()
            cl.style = CS_OWNDC
            cl.lpfnWndProc = _winproc
            cl.cbClsExtra=0
            cl.cbWndExtra=0
            cl.hInstance=GetModuleHandle(NULL)
            cl.hIcon=None
            cl.hCursor=LoadCursor(None, c_char_p(32512) )  #None #32512 #wine says this is IDC_ARROW
            cl.hbrBackground=None
            cl.lpszMenuName=None
            cl.lpszClassName = c_char_p(b"fooclass")
            cls = RegisterClass(byref(cl))
            if not cls:
                raise WinError()
            _cl=cls
        else:
            cls=_cl
            
        win = CreateWindowEx(
            0, #0x100 | 0x200, 
            cls, 
            title.encode(),
            WS_OVERLAPPEDWINDOW | WS_VISIBLE ,
            x,y,width,height,
            NULL, NULL, GetModuleHandle(NULL), NULL )
        if not win:
            print("Could not create window")
            raise WinError()
        ShowWindow(win,1)
        _win=win
        return win

    def PollEvent():
        if not _win:
            return None
        m=MSG()
        b = PeekMessage(byref(m), _win,0,0, 1 )
        if b:
            #TranslateMessage(byref(m))
            if m.message == WM_PAINT:
                #GetMessage(byref(m),_win,0,0)
                ValidateRect(_win,NULL)
                return None
            else:
                return m



    #definition of pixelformatdescriptor from wine's wingdi.h file
    class PIXELFORMATDESCRIPTOR(Structure):
        _fields_ = [
            ("nSize",WORD),
            ("nVersion",WORD),
            ("dwFlags",DWORD),
            ("iPixelType",BYTE),
            ("cColorBits",BYTE),
            ("cRedBits",BYTE),
            ("cRedShift",BYTE),
            ("cGreenBits",BYTE),
            ("cGreenShift",BYTE),
            ("cBlueBits",BYTE),
            ("cBlueShift",BYTE),
            ("cAlphaBits",BYTE),
            ("cAlphaShift",BYTE),
            ("cAccumBits",BYTE),
            ("cAccumRedBits",BYTE),
            ("cAccumGreenBits",BYTE),
            ("cAccumBlueBits",BYTE),
            ("cAccumAlphaBits",BYTE),
            ("cDepthBits",BYTE),
            ("cStencilBits",BYTE),
            ("cAuxBuffers",BYTE),
            ("iLayerType",BYTE),
            ("bReserved",BYTE),
            ("dwLayerMask",DWORD),
            ("dwVisibleMask",DWORD),
            ("dwDamageMask",DWORD)
        ]
        
    #from wine wingdi.h
    HGLRC=HANDLE
    ChoosePixelFormat = WINFUNCTYPE( INT, HDC, POINTER(PIXELFORMATDESCRIPTOR) )( ("ChoosePixelFormat",windll.gdi32) )
    SetPixelFormat = WINFUNCTYPE( BOOL, HDC, INT, POINTER(PIXELFORMATDESCRIPTOR))(("SetPixelFormat",windll.gdi32))
    wglCreateContext = WINFUNCTYPE( HGLRC, HDC )(("wglCreateContext",windll.opengl32))
    wglMakeCurrent = WINFUNCTYPE( BOOL, HDC, HGLRC )(("wglMakeCurrent",windll.opengl32))
    
    #from wine winuser.h
    GetDC = WINFUNCTYPE( HDC, HWND )( ("GetDC",windll.user32) )
    
    #from wine headers
    PFD_DRAW_TO_WINDOW=4
    PFD_SUPPORT_OPENGL = 0x20
    PFD_DOUBLEBUFFER = 1
    PFD_TYPE_RGBA=0
    PFD_MAIN_PLANE=0
    
    def CreateGLWindow(title, x,y,w,h):
        global _wintype,_winwidth,_winheight
        _wintype = 1
        _winwidth=w
        _winheight=h
        win1 = CreateWindow(title,x,y,w,h)
        
        pd = PIXELFORMATDESCRIPTOR()
        memset(byref(pd),0,sizeof(pd))
        pd.nSize = sizeof(PIXELFORMATDESCRIPTOR)
        pd.nVersion=1
        pd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER
        pd.iPixelType = PFD_TYPE_RGBA
        pd.cColorBits = 32
        #pd.cRedBits = 0
        #pd.cGreenBits = 0
        #pd.cBlueBits = 0
        #pd.cAlphaBits = 0
        #pd.cAlphaShift=0
        #pd.cAccumBits=0
        #pd.cAccumRedBits=0
        #pd.cAccumGreenBits=0
        #pd.cAccumBlueBits=0
        #pd.cAccumAlphaBits=0
        pd.cDepthBits=24
        pd.cStencilBits=8
        #pd.cAuxBuffers=0
        pd.iLayerType = PFD_MAIN_PLANE
        #pd.bReserved=0
        #pd.dwLayerMask=0
        #pd.dwVisibleMask=0
        #pd.dwDamageMask=0

        dc1 = GetDC(win1)
        pfi1 = ChoosePixelFormat( dc1, byref(pd) )
        if not SetPixelFormat(dc1, pfi1, byref(pd) ):
            raise WinError()
        
        rc1 = wglCreateContext(dc1)
        if not rc1:
            raise WinError()
        if not wglMakeCurrent( dc1, rc1 ):
            raise WinError()

        #from pyopengl
        WGL_CONTEXT_DEBUG_BIT_ARB = 1
        WGL_CONTEXT_FLAGS_ARB = 0x2094
        WGL_CONTEXT_MAJOR_VERSION_ARB = 0x2091
        WGL_CONTEXT_MINOR_VERSION_ARB = 0x2092
        WGL_CONTEXT_CORE_PROFILE_BIT_ARB = 0x1
        WGL_CONTEXT_PROFILE_MASK_ARB = 0x9126
        WGL_CONTEXT_LAYER_PLANE_ARB = 0x2093
        wglGetProcAddress = WINFUNCTYPE( c_void_p, LPCSTR )( ("wglGetProcAddress",windll.opengl32) )
        if not wglGetProcAddress:
            raise RuntimeError("No wglGetProcAddress")
        caa = wglGetProcAddress("wglCreateContextAttribsARB".encode())
        if not caa:
            raise RuntimeError("No wglCreateContextAttribsARB (No support for GL 3.2+)")

        wglCreateContextAttribsARB = WINFUNCTYPE( HANDLE, HDC, HANDLE, POINTER(c_int) )(caa)
        if not wglCreateContextAttribsARB:
            assert 0

        #from wine, but kernel32 = ms
        GetLastError = WINFUNCTYPE( DWORD )( ("GetLastError",windll.kernel32) )

        #from wine, but kernel32= ms
        FormatMessage = WINFUNCTYPE( DWORD, 
            DWORD, LPCVOID, 
            DWORD, DWORD, 
            LPSTR, DWORD,
            c_void_p )( ("FormatMessageA",windll.kernel32))
        
        rc2=None
        for major,minor in [ (4,4),(4,3),(4,2),(4,1),(4,0),(3,3),(3,2) ]:
            if rc2:
                break
            for dbg in [1,0]:
                if rc2:
                    break
                for core in [1,0]:
                    if rc2:
                        break
                    tmp = [
                        WGL_CONTEXT_MAJOR_VERSION_ARB, major, 
                        WGL_CONTEXT_MINOR_VERSION_ARB, minor,
                        WGL_CONTEXT_LAYER_PLANE_ARB,0
                    ]
                    if dbg:
                        tmp += [WGL_CONTEXT_FLAGS_ARB , WGL_CONTEXT_DEBUG_BIT_ARB]
                    if core:
                        tmp += [WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB]

                    tmp += [0,0]
                    
                    tmpA = (c_int  * len(tmp))(*tmp)
                    
                    rc2 = wglCreateContextAttribsARB(dc1, NULL, tmpA)
                    if rc2:
                        contextdesc = "%d.%d" % (major,minor)
                        if dbg:
                            contextdesc += " debug"
                        else:
                            contextdesc += " nodebug"
                        if core:
                            contextdesc += " core"
                        else:
                            contextdesc += " nocore"
                        break
                    else:
                        #http://stackoverflow.com/questions/21074447/undocumented-error-during-opengl-context-creation
                        er=GetLastError()
                        severity = er & 0xf0000000
                        facility = er& 0x0fff0000
                        code = er & 0xffff
                        memblock = (c_char * 2048)()
                        nchars = FormatMessage( 0x1000 | 0x200,
                            NULL,
                            er&0xffff, #code,
                            0,
                            memblock,
                            1024,NULL)
                        #print("nchars=",nchars)
                        if nchars == 0:
                            print("format error!!!!")
                            code = GetLastError()&0xffff
                            FormatMessage(0x1000|0x200,NULL,code,0,memblock,1024,NULL)
                            
                        tmps=""
                        for x in memblock:
                            if not x[0]:
                                break
                            tmps += chr(x[0])
                        #print("GLE error for",major,minor,dbg,core,":",hex(code),tmps)
                        
                        

        if not rc2:
            raise RuntimeError("Cannot make context")
            
 
        wglMakeCurrent(dc1,NULL)
        #should delete old glrc here
        
        if not wglMakeCurrent(dc1,rc2):
            raise WinError()

        return win1,dc1,contextdesc
    
    _SwapBuffers = WINFUNCTYPE( BOOL, HDC )( ("SwapBuffers",windll.gdi32) )
    

else:
    import sdl2
    
    _sdl_init=False
    _win=None
    def CreateWindow(title, x, y, width,height):
        global _sdl_init,_win
        if not _sdl_init:
            _sdl_init=True
            sdl2.SDL_Init(sdl2.SDL_INIT_VIDEO)
        win = sdl2.SDL_CreateWindow( title.encode(), 
            x,y,width,height,sdl2.SDL_WINDOW_OPENGL)
        if not win:
            print("Could not create window")
            raise RuntimeError()
        _win=win
        return win

    class MSG(ctypes.Structure):
        _fields_=[  ("wParam",ctypes.c_ulong),
                    ("lParam",ctypes.c_ulong),
                    ("message",ctypes.c_uint)
                ]
        pass
        
    def PollEvent():
        if not _win:
            return None
        ev=sdl2.SDL_Event()
        if not sdl2.SDL_PollEvent(byref(ev)):
            return None
        m=MSG()
        if ev.type == sdl2.SDL_QUIT:
            sys.exit(0)
        elif ev.type == sdl2.SDL_KEYDOWN:
            m.message = WM_KEYDOWN
            m.wParam = ev.key.keysym.sym
            if m.wParam >= 97 and m.wParam <= 122:
                m.wParam -= 32
        elif ev.type == sdl2.SDL_KEYUP:
            m.message = WM_KEYUP
            m.wParam = ev.key.keysym.sym
            if m.wParam >= 97 and m.wParam <= 122:
                m.wParam -= 32
        elif ev.type == sdl2.SDL_MOUSEBUTTONDOWN:
            if ev.button == sdl2.SDL_BUTTON_LEFT:
                m.message = WM_LBUTTONDOWN
            elif ev.button == sdl2.SDL_BUTTON_MIDDLE:
                m.message = WM_MBUTTONDOWN
            elif ev.button == sdl2.SDL_BUTTON_RIGHT:
                m.message  = WM_RBUTTONDOWN
            m.lParam = (ev.button.x & 0xffff) | ( (ev.button.y & 0xffff) << 16 )
        elif ev.type == sdl2.SDL_MOUSEBUTTONUP:
            if ev.button == sdl2.SDL_BUTTON_LEFT:
                m.message = WM_LBUTTONUP
            elif ev.button == sdl2.SDL_BUTTON_MIDDLE:
                m.message = WM_MBUTTONUP
            elif ev.button == sdl2.SDL_BUTTON_RIGHT:
                m.message  = WM_RBUTTONUP
            m.lParam = (ev.button.x & 0xffff) | ( (ev.button.y & 0xffff) << 16 )
        elif ev.type == sdl2.SDL_MOUSEMOTION:
            m.lParam = (ev.motion.x & 0xffff) | ( (ev.motion.y & 0xffff) << 16 )
            m.message = WM_MOUSEMOVE
        else:
            m.message = 0xdeadbeef
            
        return m

    def CreateGLWindow(title, x,y,w,h):
        global _wintype,_winwidth,_winheight
        _wintype = 1
        _winwidth=w
        _winheight=h
        
        win1 = CreateWindow(title,x,y,w,h)
        sdl2.SDL_GL_SetAttribute(sdl2.SDL_GL_CONTEXT_PROFILE_MASK, sdl2.SDL_GL_CONTEXT_PROFILE_CORE)
        sdl2.SDL_GL_SetAttribute(sdl2.SDL_GL_DEPTH_SIZE, 24)
        sdl2.SDL_GL_SetAttribute(sdl2.SDL_GL_STENCIL_SIZE, 8)

        rc2=None
        for major,minor in [ (4,4),(4,3),(4,2),(4,1),(4,0),(3,3),(3,2) ]:
            if rc2:
                break
            sdl2.SDL_GL_SetAttribute(sdl2.SDL_GL_CONTEXT_MAJOR_VERSION,major)
            sdl2.SDL_GL_SetAttribute(sdl2.SDL_GL_CONTEXT_MINOR_VERSION,minor)
            for dbg in [1,0]:
                if rc2:
                    break
                for core in [1,0]:
                    if rc2:
                        break
                    if dbg:
                        sdl2.SDL_GL_SetAttribute(sdl2.SDL_GL_CONTEXT_FLAGS,sdl2.SDL_GL_CONTEXT_DEBUG_FLAG)
                    else:
                        sdl2.SDL_GL_SetAttribute(sdl2.SDL_GL_CONTEXT_FLAGS,0)
                        
                    if core:
                        sdl2.SDL_GL_SetAttribute(sdl2.SDL_GL_CONTEXT_PROFILE_MASK, sdl2.SDL_GL_CONTEXT_PROFILE_CORE)
                    else:
                        sdl2.SDL_GL_SetAttribute(sdl2.SDL_GL_CONTEXT_PROFILE_MASK, 0)
                        
                    rc2 = sdl2.SDL_GL_CreateContext(win1)
                    if rc2:
                        contextdesc = "%d.%d" % (major,minor)
                        if dbg:
                            contextdesc += " debug"
                        else:
                            contextdesc += " nodebug"
                        if core:
                            contextdesc += " core"
                        else:
                            contextdesc += " nocore"
                        break
                            
        if not rc2:
            raise RuntimeError("Cannot make context")
            
        return win1,win1,contextdesc
    
    def _SwapBuffers(win):
        sdl2.SDL_GL_SwapWindow(win)
        
    def DispatchMessage(msgptr):
        pass
        
    

#from Wine's winuser.rh header:

# WM_SHOWWINDOW lParam codes */
WM_NULL = 0x0000
WM_CREATE = 0x0001
WM_DESTROY = 0x0002
WM_MOVE = 0x0003
WM_SIZEWAIT = 0x0004 # DDK / Win16 */
WM_SIZE = 0x0005
WM_ACTIVATE = 0x0006
# WM_ACTIVATE wParam values */
WM_SETFOCUS = 0x0007
WM_KILLFOCUS = 0x0008
WM_SETVISIBLE = 0x0009 # DDK / Win16 */
WM_ENABLE = 0x000a
WM_SETREDRAW = 0x000b
WM_SETTEXT = 0x000c
WM_GETTEXT = 0x000d
WM_GETTEXTLENGTH = 0x000e
WM_PAINT = 0x000f
WM_CLOSE = 0x0010
WM_QUERYENDSESSION = 0x0011
WM_QUIT = 0x0012
WM_QUERYOPEN = 0x0013
WM_ERASEBKGND = 0x0014
WM_SYSCOLORCHANGE = 0x0015
WM_ENDSESSION = 0x0016
WM_SYSTEMERROR = 0x0017 # DDK / Win16 */
WM_SHOWWINDOW = 0x0018
WM_CTLCOLOR = 0x0019 # Added from windowsx.h */
WM_WININICHANGE = 0x001a
WM_SETTINGCHANGE = WM_WININICHANGE
WM_DEVMODECHANGE = 0x001b
WM_ACTIVATEAPP = 0x001c
WM_FONTCHANGE = 0x001d
WM_TIMECHANGE = 0x001e
WM_CANCELMODE = 0x001f
WM_SETCURSOR = 0x0020
WM_MOUSEACTIVATE = 0x0021
WM_CHILDACTIVATE = 0x0022
WM_QUEUESYNC = 0x0023
WM_GETMINMAXINFO = 0x0024
WM_PAINTICON = 0x0026
WM_ICONERASEBKGND = 0x0027
WM_NEXTDLGCTL = 0x0028
WM_ALTTABACTIVE = 0x0029 # DDK / Win16 */
WM_SPOOLERSTATUS = 0x002a
WM_DRAWITEM = 0x002b
WM_MEASUREITEM = 0x002c
WM_DELETEITEM = 0x002d
WM_VKEYTOITEM = 0x002e
WM_CHARTOITEM = 0x002f
WM_SETFONT = 0x0030
WM_GETFONT = 0x0031
WM_SETHOTKEY = 0x0032
WM_GETHOTKEY = 0x0033
WM_FILESYSCHANGE = 0x0034 # DDK / Win16 */
WM_ISACTIVEICON = 0x0035 # DDK / Win16 */
WM_QUERYPARKICON = 0x0036 # Undocumented */
WM_QUERYDRAGICON = 0x0037
WM_QUERYSAVESTATE = 0x0038 # Undocumented */
WM_COMPAREITEM = 0x0039
WM_TESTING = 0x003a # DDK / Win16 */
WM_GETOBJECT = 0x003d
WM_ACTIVATESHELLWINDOW = 0x003e # FIXME: Wine-only */
WM_COMPACTING = 0x0041
WM_COMMNOTIFY = 0x0044
WM_WINDOWPOSCHANGING = 0x0046
WM_WINDOWPOSCHANGED = 0x0047
WM_POWER = 0x0048
# For WM_POWER */
WM_COPYDATA = 0x004a
WM_CANCELJOURNAL = 0x004b
WM_KEYF1 = 0x004d # DDK / Win16 */
WM_NOTIFY = 0x004e
WM_INPUTLANGCHANGEREQUEST = 0x0050
WM_INPUTLANGCHANGE = 0x0051
WM_TCARD = 0x0052
WM_HELP = 0x0053
WM_USERCHANGED = 0x0054
WM_NOTIFYFORMAT = 0x0055
# WM_NOTIFYFORMAT commands and return values */
WM_CONTEXTMENU = 0x007b
WM_STYLECHANGING = 0x007c
WM_STYLECHANGED = 0x007d
WM_DISPLAYCHANGE = 0x007e
WM_GETICON = 0x007f
WM_SETICON = 0x0080
WM_NCCREATE = 0x0081
WM_NCDESTROY = 0x0082
WM_NCCALCSIZE = 0x0083
WM_NCHITTEST = 0x0084
WM_NCPAINT = 0x0085
WM_NCACTIVATE = 0x0086
WM_GETDLGCODE = 0x0087
WM_SYNCPAINT = 0x0088
WM_SYNCTASK = 0x0089 # DDK / Win16 */
WM_NCMOUSEMOVE = 0x00a0
WM_NCLBUTTONDOWN = 0x00a1
WM_NCLBUTTONUP = 0x00a2
WM_NCLBUTTONDBLCLK = 0x00a3
WM_NCRBUTTONDOWN = 0x00a4
WM_NCRBUTTONUP = 0x00a5
WM_NCRBUTTONDBLCLK = 0x00a6
WM_NCMBUTTONDOWN = 0x00a7
WM_NCMBUTTONUP = 0x00a8
WM_NCMBUTTONDBLCLK = 0x00a9
WM_NCXBUTTONDOWN = 0x00ab
WM_NCXBUTTONUP = 0x00ac
WM_NCXBUTTONDBLCLK = 0x00ad
WM_INPUT_DEVICE_CHANGE = 0x00fe
WM_INPUT = 0x00ff
WM_KEYFIRST = 0x0100
WM_KEYDOWN = WM_KEYFIRST
WM_KEYUP = 0x0101
WM_CHAR = 0x0102
WM_DEADCHAR = 0x0103
WM_SYSKEYDOWN = 0x0104
WM_SYSKEYUP = 0x0105
WM_SYSCHAR = 0x0106
WM_SYSDEADCHAR = 0x0107
WM_UNICHAR = 0x0109
WM_KEYLAST = WM_UNICHAR
WM_IME_STARTCOMPOSITION = 0x010d
WM_IME_ENDCOMPOSITION = 0x010e
WM_IME_COMPOSITION = 0x010f
WM_IME_KEYLAST = 0x010f
WM_INITDIALOG = 0x0110
WM_COMMAND = 0x0111
WM_SYSCOMMAND = 0x0112
WM_TIMER = 0x0113
WM_HSCROLL = 0x0114
WM_VSCROLL = 0x0115
WM_INITMENU = 0x0116
WM_INITMENUPOPUP = 0x0117
WM_GESTURE = 0x0119
WM_GESTURENOTIFY = 0x011A
WM_MENUSELECT = 0x011F
WM_MENUCHAR = 0x0120
WM_ENTERIDLE = 0x0121
WM_MENURBUTTONUP = 0x0122
WM_MENUDRAG = 0x0123
WM_MENUGETOBJECT = 0x0124
WM_UNINITMENUPOPUP = 0x0125
WM_MENUCOMMAND = 0x0126
WM_CHANGEUISTATE = 0x0127
WM_UPDATEUISTATE = 0x0128
WM_QUERYUISTATE = 0x0129
# UI flags for WM_*UISTATE */
WM_LBTRACKPOINT = 0x0131 # DDK / Win16 */
WM_CTLCOLORMSGBOX = 0x0132
WM_CTLCOLOREDIT = 0x0133
WM_CTLCOLORLISTBOX = 0x0134
WM_CTLCOLORBTN = 0x0135
WM_CTLCOLORDLG = 0x0136
WM_CTLCOLORSCROLLBAR = 0x0137
WM_CTLCOLORSTATIC = 0x0138
WM_MOUSEFIRST = 0x0200
WM_MOUSEMOVE = WM_MOUSEFIRST
WM_LBUTTONDOWN = 0x0201
WM_LBUTTONUP = 0x0202
WM_LBUTTONDBLCLK = 0x0203
WM_RBUTTONDOWN = 0x0204
WM_RBUTTONUP = 0x0205
WM_RBUTTONDBLCLK = 0x0206
WM_MBUTTONDOWN = 0x0207
WM_MBUTTONUP = 0x0208
WM_MBUTTONDBLCLK = 0x0209
WM_MOUSEWHEEL = 0x020A
WM_XBUTTONDOWN = 0x020B
WM_XBUTTONUP = 0x020C
WM_XBUTTONDBLCLK = 0x020D
WM_MOUSEHWHEEL = 0x020E
WM_MOUSELAST = WM_MOUSEHWHEEL
WM_PARENTNOTIFY = 0x0210
WM_ENTERMENULOOP = 0x0211
WM_EXITMENULOOP = 0x0212
WM_NEXTMENU = 0x0213
WM_SIZING = 0x0214
WM_CAPTURECHANGED = 0x0215
WM_MOVING = 0x0216
WM_POWERBROADCAST = 0x0218
WM_DEVICECHANGE = 0x0219
WM_MDICREATE = 0x0220
WM_MDIDESTROY = 0x0221
WM_MDIACTIVATE = 0x0222
WM_MDIRESTORE = 0x0223
WM_MDINEXT = 0x0224
WM_MDIMAXIMIZE = 0x0225
WM_MDITILE = 0x0226
WM_MDICASCADE = 0x0227
WM_MDIICONARRANGE = 0x0228
WM_MDIGETACTIVE = 0x0229
WM_MDISETMENU = 0x0230
WM_MDIREFRESHMENU = 0x0234
WM_DROPOBJECT = 0x022A # DDK / Win16 */
WM_QUERYDROPOBJECT = 0x022B # DDK / Win16 */
WM_BEGINDRAG = 0x022C # DDK / Win16 */
WM_DRAGLOOP = 0x022D # DDK / Win16 */
WM_DRAGSELECT = 0x022E # DDK / Win16 */
WM_DRAGMOVE = 0x022F # DDK / Win16 */
WM_ENTERSIZEMOVE = 0x0231
WM_EXITSIZEMOVE = 0x0232
WM_DROPFILES = 0x0233
WM_TOUCH = 0x0240
WM_IME_SETCONTEXT = 0x0281
WM_IME_NOTIFY = 0x0282
WM_IME_CONTROL = 0x0283
WM_IME_COMPOSITIONFULL = 0x0284
WM_IME_SELECT = 0x0285
WM_IME_CHAR = 0x0286
WM_IME_REQUEST = 0x0288
WM_IME_KEYDOWN = 0x0290
WM_IME_KEYUP = 0x0291
WM_NCMOUSEHOVER = 0x02A0
WM_MOUSEHOVER = 0x02A1
WM_MOUSELEAVE = 0x02A3
WM_NCMOUSELEAVE = 0x02A2
WM_WTSSESSION_CHANGE = 0x02B1
WM_TABLET_FIRST = 0x02c0
WM_TABLET_LAST = 0x02df
WM_CUT = 0x0300
WM_COPY = 0x0301
WM_PASTE = 0x0302
WM_CLEAR = 0x0303
WM_UNDO = 0x0304
WM_RENDERFORMAT = 0x0305
WM_RENDERALLFORMATS = 0x0306
WM_DESTROYCLIPBOARD = 0x0307
WM_DRAWCLIPBOARD = 0x0308
WM_PAINTCLIPBOARD = 0x0309
WM_VSCROLLCLIPBOARD = 0x030A
WM_SIZECLIPBOARD = 0x030B
WM_ASKCBFORMATNAME = 0x030C
WM_CHANGECBCHAIN = 0x030D
WM_HSCROLLCLIPBOARD = 0x030E
WM_QUERYNEWPALETTE = 0x030F
WM_PALETTEISCHANGING = 0x0310
WM_PALETTECHANGED = 0x0311
WM_HOTKEY = 0x0312
WM_PRINT = 0x0317
WM_PRINTCLIENT = 0x0318
WM_APPCOMMAND = 0x0319
WM_THEMECHANGED = 0x031A
WM_CLIPBOARDUPDATE = 0x031D
WM_DWMCOMPOSITIONCHANGED = 0x031E
WM_DWMNCRENDERINGCHANGED = 0x031F
WM_DWMCOLORIZATIONCOLORCHANGED = 0x0320
WM_DWMWINDOWMAXIMIZEDCHANGE = 0x0321
WM_DWMSENDICONICTHUMBNAIL = 0x0323
WM_DWMSENDICONICLIVEPREVIEWBITMAP = 0x0326
WM_GETTITLEBARINFOEX = 0x033F
WM_HANDHELDFIRST = 0x0358
WM_HANDHELDLAST = 0x035F
WM_AFXFIRST = 0x0360
WM_AFXLAST = 0x037F
WM_PENWINFIRST = 0x0380
WM_PENWINLAST = 0x038F
WM_USER = 0x0400
WM_APP = 0x8000
# wParam for WM_SIZING message */
# WM_NCHITTEST return codes */
# WM_MOUSEACTIVATE return values */
# WM_GETICON/WM_SETICON params values */
# WM_SIZE message wParam values */
# WM_NCCALCSIZE return flags */
#** WM_SYSCOMMAND parameters ***/
HELP_WM_HELP = 0x000c



#** ShowWindow() codes ***/
SW_HIDE = 0
SW_SHOWNORMAL = 1
SW_NORMAL = SW_SHOWNORMAL
SW_SHOWMINIMIZED = 2
SW_SHOWMAXIMIZED = 3
SW_MAXIMIZE = SW_SHOWMAXIMIZED
SW_SHOWNOACTIVATE = 4
SW_SHOW = 5
SW_MINIMIZE = 6
SW_SHOWMINNOACTIVE = 7
SW_SHOWNA = 8
SW_RESTORE = 9
SW_SHOWDEFAULT = 10
SW_FORCEMINIMIZE = 11
SW_MAX = 11
SW_NORMALNA = 0xCC # Undocumented. Flag in MinMaximize */

# Obsolete ShowWindow() codes for compatibility */
HIDE_WINDOW = SW_HIDE
SHOW_OPENWINDOW = SW_SHOWNORMAL
SHOW_ICONWINDOW = SW_SHOWMINIMIZED
SHOW_FULLSCREEN = SW_SHOWMAXIMIZED
SHOW_OPENNOACTIVATE = SW_SHOWNOACTIVATE

# WM_SHOWWINDOW lParam codes */
SW_PARENTCLOSING = 1
SW_OTHERZOOM = 2
SW_PARENTOPENING = 3
SW_OTHERUNZOOM = 4


#** Virtual key codes ***/
VK_LBUTTON = 0x01
VK_RBUTTON = 0x02
VK_CANCEL = 0x03
VK_MBUTTON = 0x04
VK_XBUTTON1 = 0x05
VK_XBUTTON2 = 0x06
#                             0x07  Undefined */
VK_BACK = 0x08
VK_TAB = 0x09
#                             0x0A-0x0B  Undefined */
VK_CLEAR = 0x0C
VK_RETURN = 0x0D
#                             0x0E-0x0F  Undefined */
VK_SHIFT = 0x10
VK_CONTROL = 0x11
VK_MENU = 0x12
VK_PAUSE = 0x13
VK_CAPITAL = 0x14

VK_KANA = 0x15
VK_HANGEUL = VK_KANA
VK_HANGUL = VK_KANA
VK_JUNJA = 0x17
VK_FINAL = 0x18
VK_HANJA = 0x19
VK_KANJI = VK_HANJA

#                             0x1A       Undefined */
VK_ESCAPE = 0x1B

VK_CONVERT = 0x1C
VK_NONCONVERT = 0x1D
VK_ACCEPT = 0x1E
VK_MODECHANGE = 0x1F

VK_SPACE = 0x20
VK_PRIOR = 0x21
VK_NEXT = 0x22
VK_END = 0x23
VK_HOME = 0x24
VK_LEFT = 0x25
VK_UP = 0x26
VK_RIGHT = 0x27
VK_DOWN = 0x28
VK_SELECT = 0x29
VK_PRINT = 0x2A # OEM specific in Windows 3.1 SDK */
VK_EXECUTE = 0x2B
VK_SNAPSHOT = 0x2C
VK_INSERT = 0x2D
VK_DELETE = 0x2E
VK_HELP = 0x2F
# VK_0 - VK-9                 0x30-0x39  Use ASCII instead */
#                             0x3A-0x40  Undefined */
# VK_A - VK_Z                 0x41-0x5A  Use ASCII instead */
VK_LWIN = 0x5B
VK_RWIN = 0x5C
VK_APPS = 0x5D
#                             0x5E Unassigned */
VK_SLEEP = 0x5F
VK_NUMPAD0 = 0x60
VK_NUMPAD1 = 0x61
VK_NUMPAD2 = 0x62
VK_NUMPAD3 = 0x63
VK_NUMPAD4 = 0x64
VK_NUMPAD5 = 0x65
VK_NUMPAD6 = 0x66
VK_NUMPAD7 = 0x67
VK_NUMPAD8 = 0x68
VK_NUMPAD9 = 0x69
VK_MULTIPLY = 0x6A
VK_ADD = 0x6B
VK_SEPARATOR = 0x6C
VK_SUBTRACT = 0x6D
VK_DECIMAL = 0x6E
VK_DIVIDE = 0x6F
VK_F1 = 0x70
VK_F2 = 0x71
VK_F3 = 0x72
VK_F4 = 0x73
VK_F5 = 0x74
VK_F6 = 0x75
VK_F7 = 0x76
VK_F8 = 0x77
VK_F9 = 0x78
VK_F10 = 0x79
VK_F11 = 0x7A
VK_F12 = 0x7B
VK_F13 = 0x7C
VK_F14 = 0x7D
VK_F15 = 0x7E
VK_F16 = 0x7F
VK_F17 = 0x80
VK_F18 = 0x81
VK_F19 = 0x82
VK_F20 = 0x83
VK_F21 = 0x84
VK_F22 = 0x85
VK_F23 = 0x86
VK_F24 = 0x87
#                             0x88-0x8F  Unassigned */
VK_NUMLOCK = 0x90
VK_SCROLL = 0x91
VK_OEM_NEC_EQUAL = 0x92
VK_OEM_FJ_JISHO = 0x92
VK_OEM_FJ_MASSHOU = 0x93
VK_OEM_FJ_TOUROKU = 0x94
VK_OEM_FJ_LOYA = 0x95
VK_OEM_FJ_ROYA = 0x96
#                             0x97-0x9F  Unassigned */
#
# * differencing between right and left shift/control/alt key.
# * Used only by GetAsyncKeyState() and GetKeyState().
# */
VK_LSHIFT = 0xA0
VK_RSHIFT = 0xA1
VK_LCONTROL = 0xA2
VK_RCONTROL = 0xA3
VK_LMENU = 0xA4
VK_RMENU = 0xA5

VK_BROWSER_BACK = 0xA6
VK_BROWSER_FORWARD = 0xA7
VK_BROWSER_REFRESH = 0xA8
VK_BROWSER_STOP = 0xA9
VK_BROWSER_SEARCH = 0xAA
VK_BROWSER_FAVORITES = 0xAB
VK_BROWSER_HOME = 0xAC
VK_VOLUME_MUTE = 0xAD
VK_VOLUME_DOWN = 0xAE
VK_VOLUME_UP = 0xAF
VK_MEDIA_NEXT_TRACK = 0xB0
VK_MEDIA_PREV_TRACK = 0xB1
VK_MEDIA_STOP = 0xB2
VK_MEDIA_PLAY_PAUSE = 0xB3
VK_LAUNCH_MAIL = 0xB4
VK_LAUNCH_MEDIA_SELECT = 0xB5
VK_LAUNCH_APP1 = 0xB6
VK_LAUNCH_APP2 = 0xB7

#                             0xB8-0xB9  Unassigned */
VK_OEM_1 = 0xBA
VK_OEM_PLUS = 0xBB
VK_OEM_COMMA = 0xBC
VK_OEM_MINUS = 0xBD
VK_OEM_PERIOD = 0xBE
VK_OEM_2 = 0xBF
VK_OEM_3 = 0xC0
#                             0xC1-0xDA  Unassigned */
VK_OEM_4 = 0xDB
VK_OEM_5 = 0xDC
VK_OEM_6 = 0xDD
VK_OEM_7 = 0xDE
VK_OEM_8 = 0xDF
#                             0xE0       OEM specific */
VK_OEM_AX = 0xE1  # "AX" key on Japanese AX keyboard */
VK_OEM_102 = 0xE2  # "<>" or "\|" on RT 102-key keyboard */
VK_ICO_HELP = 0xE3  # Help key on ICO */
VK_ICO_00 = 0xE4  # 00 key on ICO */
VK_PROCESSKEY = 0xE5
VK_ICO_CLEAR = 0xE6

VK_PACKET = 0xE7
#                             0xE8       Unassigned */

VK_OEM_RESET = 0xE9
VK_OEM_JUMP = 0xEA
VK_OEM_PA1 = 0xEB
VK_OEM_PA2 = 0xEC
VK_OEM_PA3 = 0xED
VK_OEM_WSCTRL = 0xEE
VK_OEM_CUSEL = 0xEF
VK_OEM_ATTN = 0xF0
VK_OEM_FINISH = 0xF1
VK_OEM_COPY = 0xF2
VK_OEM_AUTO = 0xF3
VK_OEM_ENLW = 0xF4
VK_OEM_BACKTAB = 0xF5
VK_ATTN = 0xF6
VK_CRSEL = 0xF7
VK_EXSEL = 0xF8
VK_EREOF = 0xF9
VK_PLAY = 0xFA
VK_ZOOM = 0xFB
VK_NONAME = 0xFC
VK_PA1 = 0xFD
VK_OEM_CLEAR = 0xFE
#                             0xFF       Unassigned */


#** Messages ***/
#ifndef NOWINMESSAGES
WM_NULL = 0x0000
WM_CREATE = 0x0001
WM_DESTROY = 0x0002
WM_MOVE = 0x0003
WM_SIZEWAIT = 0x0004 # DDK / Win16 */
WM_SIZE = 0x0005
WM_ACTIVATE = 0x0006

# WM_ACTIVATE wParam values */
WA_INACTIVE = 0
WA_ACTIVE = 1
WA_CLICKACTIVE = 2

WM_SETFOCUS = 0x0007
WM_KILLFOCUS = 0x0008
WM_SETVISIBLE = 0x0009 # DDK / Win16 */
WM_ENABLE = 0x000a
WM_SETREDRAW = 0x000b
WM_SETTEXT = 0x000c
WM_GETTEXT = 0x000d
WM_GETTEXTLENGTH = 0x000e
WM_PAINT = 0x000f
WM_CLOSE = 0x0010
WM_QUERYENDSESSION = 0x0011
WM_QUIT = 0x0012
WM_QUERYOPEN = 0x0013
WM_ERASEBKGND = 0x0014
WM_SYSCOLORCHANGE = 0x0015
WM_ENDSESSION = 0x0016
WM_SYSTEMERROR = 0x0017 # DDK / Win16 */
WM_SHOWWINDOW = 0x0018
WM_CTLCOLOR = 0x0019 # Added from windowsx.h */
WM_WININICHANGE = 0x001a
WM_SETTINGCHANGE = WM_WININICHANGE
WM_DEVMODECHANGE = 0x001b
WM_ACTIVATEAPP = 0x001c
WM_FONTCHANGE = 0x001d
WM_TIMECHANGE = 0x001e
WM_CANCELMODE = 0x001f
WM_SETCURSOR = 0x0020
WM_MOUSEACTIVATE = 0x0021
WM_CHILDACTIVATE = 0x0022
WM_QUEUESYNC = 0x0023
WM_GETMINMAXINFO = 0x0024

WM_PAINTICON = 0x0026
WM_ICONERASEBKGND = 0x0027
WM_NEXTDLGCTL = 0x0028
WM_ALTTABACTIVE = 0x0029 # DDK / Win16 */
WM_SPOOLERSTATUS = 0x002a
WM_DRAWITEM = 0x002b
WM_MEASUREITEM = 0x002c
WM_DELETEITEM = 0x002d
WM_VKEYTOITEM = 0x002e
WM_CHARTOITEM = 0x002f
WM_SETFONT = 0x0030
WM_GETFONT = 0x0031
WM_SETHOTKEY = 0x0032
WM_GETHOTKEY = 0x0033
WM_FILESYSCHANGE = 0x0034 # DDK / Win16 */
WM_ISACTIVEICON = 0x0035 # DDK / Win16 */
WM_QUERYPARKICON = 0x0036 # Undocumented */
WM_QUERYDRAGICON = 0x0037
WM_QUERYSAVESTATE = 0x0038 # Undocumented */
WM_COMPAREITEM = 0x0039
WM_TESTING = 0x003a # DDK / Win16 */

WM_GETOBJECT = 0x003d

WM_ACTIVATESHELLWINDOW = 0x003e # FIXME: Wine-only */

WM_COMPACTING = 0x0041

WM_COMMNOTIFY = 0x0044
WM_WINDOWPOSCHANGING = 0x0046
WM_WINDOWPOSCHANGED = 0x0047

WM_POWER = 0x0048

# For WM_POWER */
PWR_OK = 1
PWR_FAIL = (-1)
PWR_SUSPENDREQUEST = 1
PWR_SUSPENDRESUME = 2
PWR_CRITICALRESUME = 3

# Win32 4.0 messages */
WM_COPYDATA = 0x004a
WM_CANCELJOURNAL = 0x004b
WM_KEYF1 = 0x004d # DDK / Win16 */
WM_NOTIFY = 0x004e
WM_INPUTLANGCHANGEREQUEST = 0x0050
WM_INPUTLANGCHANGE = 0x0051
WM_TCARD = 0x0052
WM_HELP = 0x0053
WM_USERCHANGED = 0x0054
WM_NOTIFYFORMAT = 0x0055

# WM_NOTIFYFORMAT commands and return values */
NFR_ANSI = 1
NFR_UNICODE = 2
NF_QUERY = 3
NF_REQUERY = 4

WM_CONTEXTMENU = 0x007b
WM_STYLECHANGING = 0x007c
WM_STYLECHANGED = 0x007d
WM_DISPLAYCHANGE = 0x007e
WM_GETICON = 0x007f
WM_SETICON = 0x0080

# Non-client system messages */
WM_NCCREATE = 0x0081
WM_NCDESTROY = 0x0082
WM_NCCALCSIZE = 0x0083
WM_NCHITTEST = 0x0084
WM_NCPAINT = 0x0085
WM_NCACTIVATE = 0x0086

WM_GETDLGCODE = 0x0087
WM_SYNCPAINT = 0x0088
WM_SYNCTASK = 0x0089 # DDK / Win16 */

# Non-client mouse messages */
WM_NCMOUSEMOVE = 0x00a0
WM_NCLBUTTONDOWN = 0x00a1
WM_NCLBUTTONUP = 0x00a2
WM_NCLBUTTONDBLCLK = 0x00a3
WM_NCRBUTTONDOWN = 0x00a4
WM_NCRBUTTONUP = 0x00a5
WM_NCRBUTTONDBLCLK = 0x00a6
WM_NCMBUTTONDOWN = 0x00a7
WM_NCMBUTTONUP = 0x00a8
WM_NCMBUTTONDBLCLK = 0x00a9

WM_NCXBUTTONDOWN = 0x00ab
WM_NCXBUTTONUP = 0x00ac
WM_NCXBUTTONDBLCLK = 0x00ad

# Raw input */
WM_INPUT_DEVICE_CHANGE = 0x00fe
WM_INPUT = 0x00ff

# Keyboard messages */
WM_KEYFIRST = 0x0100
WM_KEYDOWN = WM_KEYFIRST
WM_KEYUP = 0x0101
WM_CHAR = 0x0102
WM_DEADCHAR = 0x0103
WM_SYSKEYDOWN = 0x0104
WM_SYSKEYUP = 0x0105
WM_SYSCHAR = 0x0106
WM_SYSDEADCHAR = 0x0107
WM_UNICHAR = 0x0109
WM_KEYLAST = WM_UNICHAR

UNICODE_NOCHAR = 0xffff

# Win32 4.0 messages for IME */
WM_IME_STARTCOMPOSITION = 0x010d
WM_IME_ENDCOMPOSITION = 0x010e
WM_IME_COMPOSITION = 0x010f
WM_IME_KEYLAST = 0x010f

WM_INITDIALOG = 0x0110
WM_COMMAND = 0x0111
WM_SYSCOMMAND = 0x0112
WM_TIMER = 0x0113

# Scroll messages */
WM_HSCROLL = 0x0114
WM_VSCROLL = 0x0115

# Menu messages */
WM_INITMENU = 0x0116
WM_INITMENUPOPUP = 0x0117
WM_GESTURE = 0x0119
WM_GESTURENOTIFY = 0x011A

WM_MENUSELECT = 0x011F
WM_MENUCHAR = 0x0120
WM_ENTERIDLE = 0x0121

WM_MENURBUTTONUP = 0x0122
WM_MENUDRAG = 0x0123
WM_MENUGETOBJECT = 0x0124
WM_UNINITMENUPOPUP = 0x0125
WM_MENUCOMMAND = 0x0126

WM_CHANGEUISTATE = 0x0127
WM_UPDATEUISTATE = 0x0128
WM_QUERYUISTATE = 0x0129

# UI flags for WM_*UISTATE */
# for low-order word of wparam */
UIS_SET = 1
UIS_CLEAR = 2
UIS_INITIALIZE = 3
# for hi-order word of wparam */
UISF_HIDEFOCUS = 0x1
UISF_HIDEACCEL = 0x2
UISF_ACTIVE = 0x4

WM_LBTRACKPOINT = 0x0131 # DDK / Win16 */

# Win32 CTLCOLOR messages */
WM_CTLCOLORMSGBOX = 0x0132
WM_CTLCOLOREDIT = 0x0133
WM_CTLCOLORLISTBOX = 0x0134
WM_CTLCOLORBTN = 0x0135
WM_CTLCOLORDLG = 0x0136
WM_CTLCOLORSCROLLBAR = 0x0137
WM_CTLCOLORSTATIC = 0x0138

MN_GETHMENU = 0x01E1

# Mouse messages */
WM_MOUSEFIRST = 0x0200
WM_MOUSEMOVE = WM_MOUSEFIRST
WM_LBUTTONDOWN = 0x0201
WM_LBUTTONUP = 0x0202
WM_LBUTTONDBLCLK = 0x0203
WM_RBUTTONDOWN = 0x0204
WM_RBUTTONUP = 0x0205
WM_RBUTTONDBLCLK = 0x0206
WM_MBUTTONDOWN = 0x0207
WM_MBUTTONUP = 0x0208
WM_MBUTTONDBLCLK = 0x0209
WM_MOUSEWHEEL = 0x020A
WM_XBUTTONDOWN = 0x020B
WM_XBUTTONUP = 0x020C
WM_XBUTTONDBLCLK = 0x020D
WM_MOUSEHWHEEL = 0x020E
WM_MOUSELAST = WM_MOUSEHWHEEL

# Macros for the mouse messages */
WHEEL_DELTA = 120
#define GET_WHEEL_DELTA_WPARAM(wParam) ((short)HIWORD(wParam))
#WHEEL_PAGESCROLL = (UINT_MAX)

#define GET_KEYSTATE_WPARAM(wParam)     (LOWORD(wParam))
#define GET_NCHITTEST_WPARAM(wParam)    ((short)LOWORD(wParam))
#define GET_XBUTTON_WPARAM(wParam)      (HIWORD(wParam))
XBUTTON1 = 0x0001
XBUTTON2 = 0x0002

WM_PARENTNOTIFY = 0x0210
WM_ENTERMENULOOP = 0x0211
WM_EXITMENULOOP = 0x0212
WM_NEXTMENU = 0x0213

# Win32 4.0 messages */
WM_SIZING = 0x0214
WM_CAPTURECHANGED = 0x0215
WM_MOVING = 0x0216
WM_POWERBROADCAST = 0x0218
WM_DEVICECHANGE = 0x0219
# MDI messages */
WM_MDICREATE = 0x0220
WM_MDIDESTROY = 0x0221
WM_MDIACTIVATE = 0x0222
WM_MDIRESTORE = 0x0223
WM_MDINEXT = 0x0224
WM_MDIMAXIMIZE = 0x0225
WM_MDITILE = 0x0226
WM_MDICASCADE = 0x0227
WM_MDIICONARRANGE = 0x0228
WM_MDIGETACTIVE = 0x0229
WM_MDISETMENU = 0x0230
WM_MDIREFRESHMENU = 0x0234

# D&D messages */
WM_DROPOBJECT = 0x022A # DDK / Win16 */
WM_QUERYDROPOBJECT = 0x022B # DDK / Win16 */
WM_BEGINDRAG = 0x022C # DDK / Win16 */
WM_DRAGLOOP = 0x022D # DDK / Win16 */
WM_DRAGSELECT = 0x022E # DDK / Win16 */
WM_DRAGMOVE = 0x022F # DDK / Win16 */

WM_ENTERSIZEMOVE = 0x0231
WM_EXITSIZEMOVE = 0x0232
WM_DROPFILES = 0x0233

WM_TOUCH = 0x0240

# Win32 4.0 messages for IME */
WM_IME_SETCONTEXT = 0x0281
WM_IME_NOTIFY = 0x0282
WM_IME_CONTROL = 0x0283
WM_IME_COMPOSITIONFULL = 0x0284
WM_IME_SELECT = 0x0285
WM_IME_CHAR = 0x0286
# Win32 5.0 messages for IME */
WM_IME_REQUEST = 0x0288

# Win32 4.0 messages for IME */
WM_IME_KEYDOWN = 0x0290
WM_IME_KEYUP = 0x0291

WM_NCMOUSEHOVER = 0x02A0
WM_MOUSEHOVER = 0x02A1
WM_MOUSELEAVE = 0x02A3
WM_NCMOUSELEAVE = 0x02A2

WM_WTSSESSION_CHANGE = 0x02B1

WM_TABLET_FIRST = 0x02c0
WM_TABLET_LAST = 0x02df

# Clipboard command messages */
WM_CUT = 0x0300
WM_COPY = 0x0301
WM_PASTE = 0x0302
WM_CLEAR = 0x0303
WM_UNDO = 0x0304

# Clipboard owner messages */
WM_RENDERFORMAT = 0x0305
WM_RENDERALLFORMATS = 0x0306
WM_DESTROYCLIPBOARD = 0x0307

# Clipboard viewer messages */
WM_DRAWCLIPBOARD = 0x0308
WM_PAINTCLIPBOARD = 0x0309
WM_VSCROLLCLIPBOARD = 0x030A
WM_SIZECLIPBOARD = 0x030B
WM_ASKCBFORMATNAME = 0x030C
WM_CHANGECBCHAIN = 0x030D
WM_HSCROLLCLIPBOARD = 0x030E

WM_QUERYNEWPALETTE = 0x030F
WM_PALETTEISCHANGING = 0x0310
WM_PALETTECHANGED = 0x0311
WM_HOTKEY = 0x0312

WM_PRINT = 0x0317
WM_PRINTCLIENT = 0x0318
WM_APPCOMMAND = 0x0319
WM_THEMECHANGED = 0x031A
WM_CLIPBOARDUPDATE = 0x031D

WM_DWMCOMPOSITIONCHANGED = 0x031E
WM_DWMNCRENDERINGCHANGED = 0x031F
WM_DWMCOLORIZATIONCOLORCHANGED = 0x0320
WM_DWMWINDOWMAXIMIZEDCHANGE = 0x0321
WM_DWMSENDICONICTHUMBNAIL = 0x0323
WM_DWMSENDICONICLIVEPREVIEWBITMAP = 0x0326

WM_GETTITLEBARINFOEX = 0x033F

WM_HANDHELDFIRST = 0x0358
WM_HANDHELDLAST = 0x035F

WM_AFXFIRST = 0x0360
WM_AFXLAST = 0x037F

WM_PENWINFIRST = 0x0380
WM_PENWINLAST = 0x038F

WM_USER = 0x0400

WM_APP = 0x8000


# wParam for WM_SIZING message */
WMSZ_LEFT = 1
WMSZ_RIGHT = 2
WMSZ_TOP = 3
WMSZ_TOPLEFT = 4
WMSZ_TOPRIGHT = 5
WMSZ_BOTTOM = 6
WMSZ_BOTTOMLEFT = 7
WMSZ_BOTTOMRIGHT = 8

# WM_NCHITTEST return codes */
HTERROR = (-2)
HTTRANSPARENT = (-1)
HTNOWHERE = 0
HTCLIENT = 1
HTCAPTION = 2
HTSYSMENU = 3
HTSIZE = 4
HTGROWBOX = HTSIZE
HTMENU = 5
HTHSCROLL = 6
HTVSCROLL = 7
HTMINBUTTON = 8
HTREDUCE = HTMINBUTTON
HTMAXBUTTON = 9
HTZOOM = HTMAXBUTTON
HTLEFT = 10
HTSIZEFIRST = HTLEFT
HTRIGHT = 11
HTTOP = 12
HTTOPLEFT = 13
HTTOPRIGHT = 14
HTBOTTOM = 15
HTBOTTOMLEFT = 16
HTBOTTOMRIGHT = 17
HTSIZELAST = HTBOTTOMRIGHT
HTBORDER = 18
HTOBJECT = 19
HTCLOSE = 20
HTHELP = 21

# SendMessageTimeout flags */
SMTO_NORMAL = 0x0000
SMTO_BLOCK = 0x0001
SMTO_ABORTIFHUNG = 0x0002
SMTO_NOTIMEOUTIFNOTHUNG = 0x0008
SMTO_ERRORONEXIT = 0x0020

# WM_MOUSEACTIVATE return values */
MA_ACTIVATE = 1
MA_ACTIVATEANDEAT = 2
MA_NOACTIVATE = 3
MA_NOACTIVATEANDEAT = 4

# WM_GETICON/WM_SETICON params values */
ICON_SMALL = 0
ICON_BIG = 1
ICON_SMALL2 = 2

# WM_SIZE message wParam values */
SIZE_RESTORED = 0
SIZE_MINIMIZED = 1
SIZE_MAXIMIZED = 2
SIZE_MAXSHOW = 3
SIZE_MAXHIDE = 4
SIZENORMAL = SIZE_RESTORED
SIZEICONIC = SIZE_MINIMIZED
SIZEFULLSCREEN = SIZE_MAXIMIZED
SIZEZOOMSHOW = SIZE_MAXSHOW
SIZEZOOMHIDE = SIZE_MAXHIDE

# WM_NCCALCSIZE return flags */
WVR_ALIGNTOP = 0x0010
WVR_ALIGNLEFT = 0x0020
WVR_ALIGNBOTTOM = 0x0040
WVR_ALIGNRIGHT = 0x0080
WVR_HREDRAW = 0x0100
WVR_VREDRAW = 0x0200
WVR_REDRAW = (WVR_HREDRAW | WVR_VREDRAW)
WVR_VALIDRECTS = 0x0400

# Key status flags for mouse events */
#ifndef NOKEYSTATES
MK_LBUTTON = 0x0001
MK_RBUTTON = 0x0002
MK_SHIFT = 0x0004
MK_CONTROL = 0x0008
MK_MBUTTON = 0x0010
MK_XBUTTON1 = 0x0020
MK_XBUTTON2 = 0x0040
#endif # NOKEYSTATES */

#ifndef NOTRACKMOUSEEVENT
TME_HOVER = 0x00000001
TME_LEAVE = 0x00000002
TME_NONCLIENT = 0x00000010
TME_QUERY = 0x40000000
TME_CANCEL = 0x80000000
HOVER_DEFAULT = 0xFFFFFFFF
#endif # NOTRACKMOUSEEVENT */

#endif # NOWINMESSAGES */


#** Window Styles ***/
#ifndef NOWINSTYLES
WS_OVERLAPPED = 0x00000000
WS_POPUP = 0x80000000
WS_CHILD = 0x40000000
WS_MINIMIZE = 0x20000000
WS_VISIBLE = 0x10000000
WS_DISABLED = 0x08000000
WS_CLIPSIBLINGS = 0x04000000
WS_CLIPCHILDREN = 0x02000000
WS_MAXIMIZE = 0x01000000
WS_BORDER = 0x00800000
WS_DLGFRAME = 0x00400000
WS_VSCROLL = 0x00200000
WS_HSCROLL = 0x00100000
WS_SYSMENU = 0x00080000
WS_THICKFRAME = 0x00040000
WS_GROUP = 0x00020000
WS_TABSTOP = 0x00010000
WS_MINIMIZEBOX = 0x00020000
WS_MAXIMIZEBOX = 0x00010000
WS_CAPTION = (WS_BORDER | WS_DLGFRAME)
WS_TILED = WS_OVERLAPPED
WS_ICONIC = WS_MINIMIZE
WS_SIZEBOX = WS_THICKFRAME
WS_OVERLAPPEDWINDOW = (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME| WS_MINIMIZEBOX | WS_MAXIMIZEBOX)
WS_POPUPWINDOW = (WS_POPUP | WS_BORDER | WS_SYSMENU)
WS_CHILDWINDOW = WS_CHILD
WS_TILEDWINDOW = WS_OVERLAPPEDWINDOW
#endif # NOWINSTYLES */


#** Window extended styles ***/
#ifndef NOWINSTYLES
WS_EX_DLGMODALFRAME = 0x00000001
WS_EX_DRAGDETECT = 0x00000002 # Undocumented */
WS_EX_NOPARENTNOTIFY = 0x00000004
WS_EX_TOPMOST = 0x00000008
WS_EX_ACCEPTFILES = 0x00000010
WS_EX_TRANSPARENT = 0x00000020
WS_EX_MDICHILD = 0x00000040
WS_EX_TOOLWINDOW = 0x00000080
WS_EX_WINDOWEDGE = 0x00000100
WS_EX_CLIENTEDGE = 0x00000200
WS_EX_CONTEXTHELP = 0x00000400
WS_EX_RIGHT = 0x00001000
WS_EX_LEFT = 0x00000000
WS_EX_RTLREADING = 0x00002000
WS_EX_LTRREADING = 0x00000000
WS_EX_LEFTSCROLLBAR = 0x00004000
WS_EX_RIGHTSCROLLBAR = 0x00000000
WS_EX_CONTROLPARENT = 0x00010000
WS_EX_STATICEDGE = 0x00020000
WS_EX_APPWINDOW = 0x00040000
WS_EX_LAYERED = 0x00080000
WS_EX_NOINHERITLAYOUT = 0x00100000
WS_EX_LAYOUTRTL = 0x00400000
WS_EX_COMPOSITED = 0x02000000
WS_EX_NOACTIVATE = 0x08000000

WS_EX_OVERLAPPEDWINDOW = (WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE)
WS_EX_PALETTEWINDOW = (WS_EX_WINDOWEDGE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST)
#endif # NOWINSTYLES */


#** Class styles ***/
#ifndef NOWINSTYLES
CS_VREDRAW = 0x00000001
CS_HREDRAW = 0x00000002
CS_KEYCVTWINDOW = 0x00000004 # DDK / Win16 */
CS_DBLCLKS = 0x00000008
CS_OWNDC = 0x00000020
CS_CLASSDC = 0x00000040
CS_PARENTDC = 0x00000080
CS_NOKEYCVT = 0x00000100 # DDK / Win16 */
CS_NOCLOSE = 0x00000200
CS_SAVEBITS = 0x00000800
CS_BYTEALIGNCLIENT = 0x00001000
CS_BYTEALIGNWINDOW = 0x00002000
CS_GLOBALCLASS = 0x00004000
CS_IME = 0x00010000
CS_DROPSHADOW = 0x00020000
#endif # NOWINSTYLES */


#** Predefined Clipboard Formats ***/
#ifndef NOCLIPBOARD
CF_TEXT = 1
CF_BITMAP = 2
CF_METAFILEPICT = 3
CF_SYLK = 4
CF_DIF = 5
CF_TIFF = 6
CF_OEMTEXT = 7
CF_DIB = 8
CF_PALETTE = 9
CF_PENDATA = 10
CF_RIFF = 11
CF_WAVE = 12
CF_UNICODETEXT = 13
CF_ENHMETAFILE = 14
CF_HDROP = 15
CF_LOCALE = 16
CF_DIBV5 = 17
CF_MAX = 18

CF_OWNERDISPLAY = 0x0080
CF_DSPTEXT = 0x0081
CF_DSPBITMAP = 0x0082
CF_DSPMETAFILEPICT = 0x0083
CF_DSPENHMETAFILE = 0x008E

# "Private" formats don't get GlobalFree()'d */
CF_PRIVATEFIRST = 0x0200
CF_PRIVATELAST = 0x02FF

# "GDIOBJ" formats do get DeleteObject()'d */
CF_GDIOBJFIRST = 0x0300
CF_GDIOBJLAST = 0x03FF
#endif # NOCLIPBOARD */


#** Menu flags ***/
#ifndef NOMENUS
MF_INSERT = 0x00000000
MF_CHANGE = 0x00000080
MF_APPEND = 0x00000100
MF_DELETE = 0x00000200
MF_REMOVE = 0x00001000
MF_END = 0x00000080

MF_ENABLED = 0x00000000
MF_GRAYED = 0x00000001
MF_DISABLED = 0x00000002
MF_STRING = 0x00000000
MF_BITMAP = 0x00000004
MF_UNCHECKED = 0x00000000
MF_CHECKED = 0x00000008
MF_POPUP = 0x00000010
MF_MENUBARBREAK = 0x00000020
MF_MENUBREAK = 0x00000040
MF_UNHILITE = 0x00000000
MF_HILITE = 0x00000080
MF_OWNERDRAW = 0x00000100
MF_USECHECKBITMAPS = 0x00000200
MF_BYCOMMAND = 0x00000000
MF_BYPOSITION = 0x00000400
MF_SEPARATOR = 0x00000800
MF_DEFAULT = 0x00001000
MF_SYSMENU = 0x00002000
MF_HELP = 0x00004000
MF_RIGHTJUSTIFY = 0x00004000
MF_MOUSESELECT = 0x00008000

# Flags for extended menu item types */
MFT_STRING = MF_STRING
MFT_BITMAP = MF_BITMAP
MFT_MENUBARBREAK = MF_MENUBARBREAK
MFT_MENUBREAK = MF_MENUBREAK
MFT_OWNERDRAW = MF_OWNERDRAW
MFT_RADIOCHECK = 0x00000200
MFT_SEPARATOR = MF_SEPARATOR
MFT_RIGHTORDER = 0x00002000
MFT_RIGHTJUSTIFY = MF_RIGHTJUSTIFY

# Flags for extended menu item states */
MFS_GRAYED = 0x00000003
MFS_DISABLED = MFS_GRAYED
MFS_CHECKED = MF_CHECKED
MFS_HILITE = MF_HILITE
MFS_ENABLED = MF_ENABLED
MFS_UNCHECKED = MF_UNCHECKED
MFS_UNHILITE = MF_UNHILITE
MFS_DEFAULT = MF_DEFAULT

# DDK / Win16 defines */
MFS_MASK = 0x0000108B
MFS_HOTTRACKDRAWN = 0x10000000
MFS_CACHEDBMP = 0x20000000
MFS_BOTTOMGAPDROP = 0x40000000
MFS_TOPGAPDROP = 0x80000000
MFS_GAPDROP = (MFS_BOTTOMGAPDROP | MFS_TOPGAPDROP)
#endif # NOMENUS */


#** WM_SYSCOMMAND parameters ***/
#ifndef NOSYSCOMMANDS
# At least HP-UX defines it in /usr/include/sys/signal.h */
# ifdef SC_SIZE
#  undef SC_SIZE
# endif
SC_SIZE = 0xf000
SC_MOVE = 0xf010
SC_MINIMIZE = 0xf020
SC_MAXIMIZE = 0xf030
SC_NEXTWINDOW = 0xf040
SC_PREVWINDOW = 0xf050
SC_CLOSE = 0xf060
SC_VSCROLL = 0xf070
SC_HSCROLL = 0xf080
SC_MOUSEMENU = 0xf090
SC_KEYMENU = 0xf100
SC_ARRANGE = 0xf110
SC_RESTORE = 0xf120
SC_TASKLIST = 0xf130
SC_SCREENSAVE = 0xf140
SC_HOTKEY = 0xf150

# Win32 4.0 */
SC_DEFAULT = 0xf160
SC_MONITORPOWER = 0xf170
SC_CONTEXTHELP = 0xf180
SC_SEPARATOR = 0xf00f

#define GET_SC_WPARAM(wParam)  ((int)wParam & 0xfff0)
SCF_ISSECURE = 0x0001

# Obsolete names */
SC_ICON = SC_MINIMIZE
SC_ZOOM = SC_MAXIMIZE
#endif # NOSYSCOMMANDS */


#** OEM Resource Ordinal Numbers ***/
#ifdef OEMRESOURCE
OBM_RDRVERT = 32559
OBM_RDRHORZ = 32660
OBM_RDR2DIM = 32661
OBM_TRTYPE = 32732 # FIXME: Wine-only */
OBM_LFARROWI = 32734
OBM_RGARROWI = 32735
OBM_DNARROWI = 32736
OBM_UPARROWI = 32737
OBM_COMBO = 32738
OBM_MNARROW = 32739
OBM_LFARROWD = 32740
OBM_RGARROWD = 32741
OBM_DNARROWD = 32742
OBM_UPARROWD = 32743
OBM_RESTORED = 32744
OBM_ZOOMD = 32745
OBM_REDUCED = 32746
OBM_RESTORE = 32747
OBM_ZOOM = 32748
OBM_REDUCE = 32749
OBM_LFARROW = 32750
OBM_RGARROW = 32751
OBM_DNARROW = 32752
OBM_UPARROW = 32753
OBM_CLOSE = 32754
OBM_OLD_RESTORE = 32755
OBM_OLD_ZOOM = 32756
OBM_OLD_REDUCE = 32757
OBM_BTNCORNERS = 32758
OBM_CHECKBOXES = 32759
OBM_CHECK = 32760
OBM_BTSIZE = 32761
OBM_OLD_LFARROW = 32762
OBM_OLD_RGARROW = 32763
OBM_OLD_DNARROW = 32764
OBM_OLD_UPARROW = 32765
OBM_SIZE = 32766
OBM_OLD_CLOSE = 32767

OCR_NORMAL = 32512
OCR_IBEAM = 32513
OCR_WAIT = 32514
OCR_CROSS = 32515
OCR_UP = 32516
OCR_SIZE = 32640
OCR_ICON = 32641
OCR_SIZENWSE = 32642
OCR_SIZENESW = 32643
OCR_SIZEWE = 32644
OCR_SIZENS = 32645
OCR_SIZEALL = 32646
OCR_ICOCUR = 32647
OCR_NO = 32648
OCR_HAND = 32649
OCR_APPSTARTING = 32650
OCR_HELP = 32651 # DDK / Win16 */
OCR_RDRVERT = 32652 # DDK / Win16 */
OCR_RDRHORZ = 32653 # DDK / Win16 */
OCR_DRAGOBJECT = OCR_RDRHORZ # FIXME: Wine-only */
OCR_RDR2DIM = 32654 # DDK / Win16 */
OCR_RDRNORTH = 32655 # DDK / Win16 */
OCR_RDRSOUTH = 32656 # DDK / Win16 */
OCR_RDRWEST = 32657 # DDK / Win16 */
OCR_RDREAST = 32658 # DDK / Win16 */
OCR_RDRNORTHWEST = 32659 # DDK / Win16 */
OCR_RDRNORTHEAST = 32660 # DDK / Win16 */
OCR_RDRSOUTHWEST = 32661 # DDK / Win16 */
OCR_RDRSOUTHEAST = 32662 # DDK / Win16 */

OIC_SAMPLE = 32512
OIC_HAND = 32513
OIC_ERROR = OIC_HAND
OIC_QUES = 32514
OIC_BANG = 32515
OIC_WARNING = OIC_BANG
OIC_NOTE = 32516
OIC_INFORMATION = OIC_NOTE
OIC_WINLOGO = 32517
OIC_SHIELD = 32518
#endif # OEMRESOURCE */


#** Predefined resources ***/
#ifndef NOICONS
#IDI_APPLICATION = MAKEINTRESOURCE(32512)
#IDI_HAND = MAKEINTRESOURCE(32513)
#IDI_QUESTION = MAKEINTRESOURCE(32514)
#IDI_EXCLAMATION = MAKEINTRESOURCE(32515)
#IDI_ASTERISK = MAKEINTRESOURCE(32516)
#IDI_WINLOGO = MAKEINTRESOURCE(32517)
#IDI_SHIELD = MAKEINTRESOURCE(32518)

#IDI_WARNING = IDI_EXCLAMATION
#IDI_ERROR = IDI_HAND
#IDI_INFORMATION = IDI_ASTERISK
#endif # NOICONS */


#** Standard dialog button IDs ***/
IDOK = 1
IDCANCEL = 2
IDABORT = 3
IDRETRY = 4
IDIGNORE = 5
IDYES = 6
IDNO = 7
IDCLOSE = 8
IDHELP = 9
IDTRYAGAIN = 10
IDCONTINUE = 11
#ifndef IDTIMEOUT
IDTIMEOUT = 32000
#endif


#** Edit control styles ***/
#ifndef NOWINSTYLES
ES_LEFT = 0x00000000
ES_CENTER = 0x00000001
ES_RIGHT = 0x00000002
ES_MULTILINE = 0x00000004
ES_UPPERCASE = 0x00000008
ES_LOWERCASE = 0x00000010
ES_PASSWORD = 0x00000020
ES_AUTOVSCROLL = 0x00000040
ES_AUTOHSCROLL = 0x00000080
ES_NOHIDESEL = 0x00000100
ES_COMBO = 0x00000200 # Undocumented. Parent is a combobox */
ES_OEMCONVERT = 0x00000400
ES_READONLY = 0x00000800
ES_WANTRETURN = 0x00001000
ES_NUMBER = 0x00002000
#endif # NOWINSTYLES */


#** Edit control messages ***/
#ifndef NOWINMESSAGES
EM_GETSEL = 0x00b0
EM_SETSEL = 0x00b1
EM_GETRECT = 0x00b2
EM_SETRECT = 0x00b3
EM_SETRECTNP = 0x00b4
EM_SCROLL = 0x00b5
EM_LINESCROLL = 0x00b6
EM_SCROLLCARET = 0x00b7
EM_GETMODIFY = 0x00b8
EM_SETMODIFY = 0x00b9
EM_GETLINECOUNT = 0x00ba
EM_LINEINDEX = 0x00bb
EM_SETHANDLE = 0x00bc
EM_GETHANDLE = 0x00bd
EM_GETTHUMB = 0x00be
# Unassigned 0x00bf and 0x00c0 */
EM_LINELENGTH = 0x00c1
EM_REPLACESEL = 0x00c2
EM_SETFONT = 0x00c3 # DDK / Win16 */
EM_GETLINE = 0x00c4
EM_LIMITTEXT = 0x00c5
EM_SETLIMITTEXT = EM_LIMITTEXT
EM_CANUNDO = 0x00c6
EM_UNDO = 0x00c7
EM_FMTLINES = 0x00c8
EM_LINEFROMCHAR = 0x00c9
EM_SETWORDBREAK = 0x00ca # DDK / Win16 */
EM_SETTABSTOPS = 0x00cb
EM_SETPASSWORDCHAR = 0x00cc
EM_EMPTYUNDOBUFFER = 0x00cd
EM_GETFIRSTVISIBLELINE = 0x00ce
EM_SETREADONLY = 0x00cf
EM_SETWORDBREAKPROC = 0x00d0
EM_GETWORDBREAKPROC = 0x00d1
EM_GETPASSWORDCHAR = 0x00d2
EM_SETMARGINS = 0x00d3
EM_GETMARGINS = 0x00d4
EM_GETLIMITTEXT = 0x00d5
EM_POSFROMCHAR = 0x00d6
EM_CHARFROMPOS = 0x00d7
EM_SETIMESTATUS = 0x00d8
EM_GETIMESTATUS = 0x00d9
#endif # NOWINMESSAGES */


#** Button control styles ***/
BS_PUSHBUTTON = 0x00000000
BS_DEFPUSHBUTTON = 0x00000001
BS_CHECKBOX = 0x00000002
BS_AUTOCHECKBOX = 0x00000003
BS_RADIOBUTTON = 0x00000004
BS_3STATE = 0x00000005
BS_AUTO3STATE = 0x00000006
BS_GROUPBOX = 0x00000007
BS_USERBUTTON = 0x00000008
BS_AUTORADIOBUTTON = 0x00000009
BS_PUSHBOX = 0x0000000A
BS_OWNERDRAW = 0x0000000B
BS_TYPEMASK = 0x0000000F
BS_LEFTTEXT = 0x00000020
BS_RIGHTBUTTON = BS_LEFTTEXT

BS_TEXT = 0x00000000
BS_ICON = 0x00000040
BS_BITMAP = 0x00000080
BS_LEFT = 0x00000100
BS_RIGHT = 0x00000200
BS_CENTER = 0x00000300
BS_TOP = 0x00000400
BS_BOTTOM = 0x00000800
BS_VCENTER = 0x00000C00
BS_PUSHLIKE = 0x00001000
BS_MULTILINE = 0x00002000
BS_NOTIFY = 0x00004000
BS_FLAT = 0x00008000


#** Button notification codes ***/
BN_CLICKED = 0
BN_PAINT = 1
BN_HILITE = 2
BN_PUSHED = BN_HILITE
BN_UNHILITE = 3
BN_UNPUSHED = BN_UNHILITE
BN_DISABLE = 4
BN_DOUBLECLICKED = 5
BN_DBLCLK = BN_DOUBLECLICKED
BN_SETFOCUS = 6
BN_KILLFOCUS = 7


#** Win32 button control messages ***/
BM_GETCHECK = 0x00f0
BM_SETCHECK = 0x00f1
BM_GETSTATE = 0x00f2
BM_SETSTATE = 0x00f3
BM_SETSTYLE = 0x00f4
BM_CLICK = 0x00f5
BM_GETIMAGE = 0x00f6
BM_SETIMAGE = 0x00f7
BM_SETDONTCLICK = 0x00f8

# Button states */
BST_UNCHECKED = 0x0000
BST_CHECKED = 0x0001
BST_INDETERMINATE = 0x0002
BST_PUSHED = 0x0004
BST_FOCUS = 0x0008

#** Static Control Styles ***/
SS_LEFT = 0x00000000
SS_CENTER = 0x00000001
SS_RIGHT = 0x00000002
SS_ICON = 0x00000003
SS_BLACKRECT = 0x00000004
SS_GRAYRECT = 0x00000005
SS_WHITERECT = 0x00000006
SS_BLACKFRAME = 0x00000007
SS_GRAYFRAME = 0x00000008
SS_WHITEFRAME = 0x00000009
SS_USERITEM = 0x0000000A
SS_SIMPLE = 0x0000000B
SS_LEFTNOWORDWRAP = 0x0000000C
SS_OWNERDRAW = 0x0000000D
SS_BITMAP = 0x0000000E
SS_ENHMETAFILE = 0x0000000F
SS_ETCHEDHORZ = 0x00000010
SS_ETCHEDVERT = 0x00000011
SS_ETCHEDFRAME = 0x00000012
SS_TYPEMASK = 0x0000001F

SS_REALSIZECONTROL = 0x00000040
SS_NOPREFIX = 0x00000080
SS_NOTIFY = 0x00000100
SS_CENTERIMAGE = 0x00000200
SS_RIGHTJUST = 0x00000400
SS_REALSIZEIMAGE = 0x00000800
SS_SUNKEN = 0x00001000
SS_EDITCONTROL = 0x00002000
SS_ENDELLIPSIS = 0x00004000
SS_PATHELLIPSIS = 0x00008000
SS_WORDELLIPSIS = 0x0000C000
SS_ELLIPSISMASK = SS_WORDELLIPSIS


#** Dialog styles ***/
DS_ABSALIGN = 0x00000001
DS_SYSMODAL = 0x00000002
DS_3DLOOK = 0x00000004 # win95 */
DS_FIXEDSYS = 0x00000008 # win95 */
DS_NOFAILCREATE = 0x00000010 # win95 */
DS_LOCALEDIT = 0x00000020
DS_SETFONT = 0x00000040
DS_MODALFRAME = 0x00000080
DS_NOIDLEMSG = 0x00000100
DS_SETFOREGROUND = 0x00000200 # win95 */
DS_CONTROL = 0x00000400 # win95 */
DS_CENTER = 0x00000800 # win95 */
DS_CENTERMOUSE = 0x00001000 # win95 */
DS_CONTEXTHELP = 0x00002000 # win95 */
DS_USEPIXELS = 0x00008000
DS_SHELLFONT = (DS_SETFONT | DS_FIXEDSYS)


#** Listbox styles ***/
#ifndef NOWINSTYLES
LBS_NOTIFY = 0x00000001
LBS_SORT = 0x00000002
LBS_NOREDRAW = 0x00000004
LBS_MULTIPLESEL = 0x00000008
LBS_OWNERDRAWFIXED = 0x00000010
LBS_OWNERDRAWVARIABLE = 0x00000020
LBS_HASSTRINGS = 0x00000040
LBS_USETABSTOPS = 0x00000080
LBS_NOINTEGRALHEIGHT = 0x00000100
LBS_MULTICOLUMN = 0x00000200
LBS_WANTKEYBOARDINPUT = 0x00000400
LBS_EXTENDEDSEL = 0x00000800
LBS_DISABLENOSCROLL = 0x00001000
LBS_NODATA = 0x00002000
LBS_NOSEL = 0x00004000
LBS_COMBOBOX = 0x00008000
LBS_STANDARD = (LBS_NOTIFY | LBS_SORT | WS_VSCROLL | WS_BORDER)
#endif # NOWINSTYLES */

#** Combo box styles ***/
#ifndef NOWINSTYLES
CBS_SIMPLE = 0x00000001
CBS_DROPDOWN = 0x00000002
CBS_DROPDOWNLIST = 0x00000003
CBS_OWNERDRAWFIXED = 0x00000010
CBS_OWNERDRAWVARIABLE = 0x00000020
CBS_AUTOHSCROLL = 0x00000040
CBS_OEMCONVERT = 0x00000080
CBS_SORT = 0x00000100
CBS_HASSTRINGS = 0x00000200
CBS_NOINTEGRALHEIGHT = 0x00000400
CBS_DISABLENOSCROLL = 0x00000800

CBS_UPPERCASE = 0x00002000
CBS_LOWERCASE = 0x00004000
#endif # NOWINSTYLES */


#** Scrollbar styles ***/
#ifndef NOWINSTYLES
SBS_HORZ = 0x00000000
SBS_VERT = 0x00000001
SBS_TOPALIGN = 0x00000002
SBS_LEFTALIGN = 0x00000002
SBS_BOTTOMALIGN = 0x00000004
SBS_RIGHTALIGN = 0x00000004
SBS_SIZEBOXTOPLEFTALIGN = 0x00000002
SBS_SIZEBOXBOTTOMRIGHTALIGN = 0x00000004
SBS_SIZEBOX = 0x00000008
SBS_SIZEGRIP = 0x00000010
#endif # NOWINSTYLES */

#** WinHelp commands ***/
HELP_CONTEXT = 0x00000001
HELP_QUIT = 0x00000002
HELP_INDEX = 0x00000003
HELP_CONTENTS = HELP_INDEX
HELP_HELPONHELP = 0x00000004
HELP_SETINDEX = 0x00000005
HELP_SETCONTENTS = HELP_SETINDEX
HELP_CONTEXTPOPUP = 0x00000008
HELP_FORCEFILE = 0x00000009
HELP_KEY = 0x00000101
HELP_COMMAND = 0x00000102
HELP_PARTIALKEY = 0x00000105
HELP_MULTIKEY = 0x00000201
HELP_SETWINPOS = 0x00000203

HELP_CONTEXTMENU = 0x000a
HELP_FINDER = 0x000b
HELP_WM_HELP = 0x000c
HELP_SETPOPUP_POS = 0x000d
HELP_TCARD_DATA = 0x0010
HELP_TCARD_OTHER_CALLER = 0x0011
HELP_TCARD = 0x8000

IDH_NO_HELP = 28440
IDH_MISSING_CONTEXT = 28441
IDH_GENERIC_HELP_BUTTON = 28442
IDH_OK = 28443
IDH_CANCEL = 28444
IDH_HELP = 28445



















wmdict = {
#** Messages ***/
#ifndef NOWINMESSAGES
0x0000 : "WM_NULL",
0x0001 : "WM_CREATE",
0x0002 : "WM_DESTROY",
0x0003 : "WM_MOVE",
0x0004 : "WM_SIZEWAIT", # DDK / Win16 */
0x0005 : "WM_SIZE",
0x0006 : "WM_ACTIVATE",

# WM_ACTIVATE wParam values */
0 : "WA_INACTIVE",
1 : "WA_ACTIVE",
2 : "WA_CLICKACTIVE",

0x0007 : "WM_SETFOCUS",
0x0008 : "WM_KILLFOCUS",
0x0009 : "WM_SETVISIBLE", # DDK / Win16 */
0x000a : "WM_ENABLE",
0x000b : "WM_SETREDRAW",
0x000c : "WM_SETTEXT",
0x000d : "WM_GETTEXT",
0x000e : "WM_GETTEXTLENGTH",
0x000f : "WM_PAINT",
0x0010 : "WM_CLOSE",
0x0011 : "WM_QUERYENDSESSION",
0x0012 : "WM_QUIT",
0x0013 : "WM_QUERYOPEN",
0x0014 : "WM_ERASEBKGND",
0x0015 : "WM_SYSCOLORCHANGE",
0x0016 : "WM_ENDSESSION",
0x0017 : "WM_SYSTEMERROR", # DDK / Win16 */
0x0018 : "WM_SHOWWINDOW",
0x0019 : "WM_CTLCOLOR", # Added from windowsx.h */
0x001a : "WM_WININICHANGE",
WM_WININICHANGE : "WM_SETTINGCHANGE",
0x001b : "WM_DEVMODECHANGE",
0x001c : "WM_ACTIVATEAPP",
0x001d : "WM_FONTCHANGE",
0x001e : "WM_TIMECHANGE",
0x001f : "WM_CANCELMODE",
0x0020 : "WM_SETCURSOR",
0x0021 : "WM_MOUSEACTIVATE",
0x0022 : "WM_CHILDACTIVATE",
0x0023 : "WM_QUEUESYNC",
0x0024 : "WM_GETMINMAXINFO",

0x0026 : "WM_PAINTICON",
0x0027 : "WM_ICONERASEBKGND",
0x0028 : "WM_NEXTDLGCTL",
0x0029 : "WM_ALTTABACTIVE", # DDK / Win16 */
0x002a : "WM_SPOOLERSTATUS",
0x002b : "WM_DRAWITEM",
0x002c : "WM_MEASUREITEM",
0x002d : "WM_DELETEITEM",
0x002e : "WM_VKEYTOITEM",
0x002f : "WM_CHARTOITEM",
0x0030 : "WM_SETFONT",
0x0031 : "WM_GETFONT",
0x0032 : "WM_SETHOTKEY",
0x0033 : "WM_GETHOTKEY",
0x0034 : "WM_FILESYSCHANGE", # DDK / Win16 */
0x0035 : "WM_ISACTIVEICON", # DDK / Win16 */
0x0036 : "WM_QUERYPARKICON", # Undocumented */
0x0037 : "WM_QUERYDRAGICON",
0x0038 : "WM_QUERYSAVESTATE", # Undocumented */
0x0039 : "WM_COMPAREITEM",
0x003a : "WM_TESTING", # DDK / Win16 */

0x003d : "WM_GETOBJECT",

0x003e : "WM_ACTIVATESHELLWINDOW", # FIXME: Wine-only */

0x0041 : "WM_COMPACTING",

0x0044 : "WM_COMMNOTIFY",
0x0046 : "WM_WINDOWPOSCHANGING",
0x0047 : "WM_WINDOWPOSCHANGED",

0x0048 : "WM_POWER",

# For WM_POWER */
1 : "PWR_OK",
1 : "PWR_SUSPENDREQUEST",
2 : "PWR_SUSPENDRESUME",
3 : "PWR_CRITICALRESUME",

# Win32 4.0 messages */
0x004a : "WM_COPYDATA",
0x004b : "WM_CANCELJOURNAL",
0x004d : "WM_KEYF1", # DDK / Win16 */
0x004e : "WM_NOTIFY",
0x0050 : "WM_INPUTLANGCHANGEREQUEST",
0x0051 : "WM_INPUTLANGCHANGE",
0x0052 : "WM_TCARD",
0x0053 : "WM_HELP",
0x0054 : "WM_USERCHANGED",
0x0055 : "WM_NOTIFYFORMAT",

# WM_NOTIFYFORMAT commands and return values */
1 : "NFR_ANSI",
2 : "NFR_UNICODE",
3 : "NF_QUERY",
4 : "NF_REQUERY",

0x007b : "WM_CONTEXTMENU",
0x007c : "WM_STYLECHANGING",
0x007d : "WM_STYLECHANGED",
0x007e : "WM_DISPLAYCHANGE",
0x007f : "WM_GETICON",
0x0080 : "WM_SETICON",

# Non-client system messages */
0x0081 : "WM_NCCREATE",
0x0082 : "WM_NCDESTROY",
0x0083 : "WM_NCCALCSIZE",
0x0084 : "WM_NCHITTEST",
0x0085 : "WM_NCPAINT",
0x0086 : "WM_NCACTIVATE",

0x0087 : "WM_GETDLGCODE",
0x0088 : "WM_SYNCPAINT",
0x0089 : "WM_SYNCTASK", # DDK / Win16 */

# Non-client mouse messages */
0x00a0 : "WM_NCMOUSEMOVE",
0x00a1 : "WM_NCLBUTTONDOWN",
0x00a2 : "WM_NCLBUTTONUP",
0x00a3 : "WM_NCLBUTTONDBLCLK",
0x00a4 : "WM_NCRBUTTONDOWN",
0x00a5 : "WM_NCRBUTTONUP",
0x00a6 : "WM_NCRBUTTONDBLCLK",
0x00a7 : "WM_NCMBUTTONDOWN",
0x00a8 : "WM_NCMBUTTONUP",
0x00a9 : "WM_NCMBUTTONDBLCLK",

0x00ab : "WM_NCXBUTTONDOWN",
0x00ac : "WM_NCXBUTTONUP",
0x00ad : "WM_NCXBUTTONDBLCLK",

# Raw input */
0x00fe : "WM_INPUT_DEVICE_CHANGE",
0x00ff : "WM_INPUT",

# Keyboard messages */
0x0100 : "WM_KEYFIRST",
WM_KEYFIRST : "WM_KEYDOWN",
0x0101 : "WM_KEYUP",
0x0102 : "WM_CHAR",
0x0103 : "WM_DEADCHAR",
0x0104 : "WM_SYSKEYDOWN",
0x0105 : "WM_SYSKEYUP",
0x0106 : "WM_SYSCHAR",
0x0107 : "WM_SYSDEADCHAR",
0x0109 : "WM_UNICHAR",
WM_UNICHAR : "WM_KEYLAST",

0xffff : "UNICODE_NOCHAR",

# Win32 4.0 messages for IME */
0x010d : "WM_IME_STARTCOMPOSITION",
0x010e : "WM_IME_ENDCOMPOSITION",
0x010f : "WM_IME_COMPOSITION",
0x010f : "WM_IME_KEYLAST",

0x0110 : "WM_INITDIALOG",
0x0111 : "WM_COMMAND",
0x0112 : "WM_SYSCOMMAND",
0x0113 : "WM_TIMER",

# Scroll messages */
0x0114 : "WM_HSCROLL",
0x0115 : "WM_VSCROLL",

# Menu messages */
0x0116 : "WM_INITMENU",
0x0117 : "WM_INITMENUPOPUP",
0x0119 : "WM_GESTURE",
0x011A : "WM_GESTURENOTIFY",

0x011F : "WM_MENUSELECT",
0x0120 : "WM_MENUCHAR",
0x0121 : "WM_ENTERIDLE",

0x0122 : "WM_MENURBUTTONUP",
0x0123 : "WM_MENUDRAG",
0x0124 : "WM_MENUGETOBJECT",
0x0125 : "WM_UNINITMENUPOPUP",
0x0126 : "WM_MENUCOMMAND",

0x0127 : "WM_CHANGEUISTATE",
0x0128 : "WM_UPDATEUISTATE",
0x0129 : "WM_QUERYUISTATE",

# UI flags for WM_*UISTATE */
# for low-order word of wparam */
1 : "UIS_SET",
2 : "UIS_CLEAR",
3 : "UIS_INITIALIZE",
# for hi-order word of wparam */
0x1 : "UISF_HIDEFOCUS",
0x2 : "UISF_HIDEACCEL",
0x4 : "UISF_ACTIVE",

0x0131 : "WM_LBTRACKPOINT", # DDK / Win16 */

# Win32 CTLCOLOR messages */
0x0132 : "WM_CTLCOLORMSGBOX",
0x0133 : "WM_CTLCOLOREDIT",
0x0134 : "WM_CTLCOLORLISTBOX",
0x0135 : "WM_CTLCOLORBTN",
0x0136 : "WM_CTLCOLORDLG",
0x0137 : "WM_CTLCOLORSCROLLBAR",
0x0138 : "WM_CTLCOLORSTATIC",

0x01E1 : "MN_GETHMENU",

# Mouse messages */
0x0200 : "WM_MOUSEFIRST",
WM_MOUSEFIRST : "WM_MOUSEMOVE",
0x0201 : "WM_LBUTTONDOWN",
0x0202 : "WM_LBUTTONUP",
0x0203 : "WM_LBUTTONDBLCLK",
0x0204 : "WM_RBUTTONDOWN",
0x0205 : "WM_RBUTTONUP",
0x0206 : "WM_RBUTTONDBLCLK",
0x0207 : "WM_MBUTTONDOWN",
0x0208 : "WM_MBUTTONUP",
0x0209 : "WM_MBUTTONDBLCLK",
0x020A : "WM_MOUSEWHEEL",
0x020B : "WM_XBUTTONDOWN",
0x020C : "WM_XBUTTONUP",
0x020D : "WM_XBUTTONDBLCLK",
0x020E : "WM_MOUSEHWHEEL",
WM_MOUSEHWHEEL : "WM_MOUSELAST",

# Macros for the mouse messages */
120 : "WHEEL_DELTA",
#define GET_WHEEL_DELTA_WPARAM(wParam) ((short)HIWORD(wParam))
#WHEEL_PAGESCROLL = (UINT_MAX)

#define GET_KEYSTATE_WPARAM(wParam)     (LOWORD(wParam))
#define GET_NCHITTEST_WPARAM(wParam)    ((short)LOWORD(wParam))
#define GET_XBUTTON_WPARAM(wParam)      (HIWORD(wParam))
0x0001 : "XBUTTON1",
0x0002 : "XBUTTON2",

0x0210 : "WM_PARENTNOTIFY",
0x0211 : "WM_ENTERMENULOOP",
0x0212 : "WM_EXITMENULOOP",
0x0213 : "WM_NEXTMENU",

# Win32 4.0 messages */
0x0214 : "WM_SIZING",
0x0215 : "WM_CAPTURECHANGED",
0x0216 : "WM_MOVING",
0x0218 : "WM_POWERBROADCAST",
0x0219 : "WM_DEVICECHANGE",
# MDI messages */
0x0220 : "WM_MDICREATE",
0x0221 : "WM_MDIDESTROY",
0x0222 : "WM_MDIACTIVATE",
0x0223 : "WM_MDIRESTORE",
0x0224 : "WM_MDINEXT",
0x0225 : "WM_MDIMAXIMIZE",
0x0226 : "WM_MDITILE",
0x0227 : "WM_MDICASCADE",
0x0228 : "WM_MDIICONARRANGE",
0x0229 : "WM_MDIGETACTIVE",
0x0230 : "WM_MDISETMENU",
0x0234 : "WM_MDIREFRESHMENU",

# D&D messages */
0x022A : "WM_DROPOBJECT", # DDK / Win16 */
0x022B : "WM_QUERYDROPOBJECT", # DDK / Win16 */
0x022C : "WM_BEGINDRAG", # DDK / Win16 */
0x022D : "WM_DRAGLOOP", # DDK / Win16 */
0x022E : "WM_DRAGSELECT", # DDK / Win16 */
0x022F : "WM_DRAGMOVE", # DDK / Win16 */

0x0231 : "WM_ENTERSIZEMOVE",
0x0232 : "WM_EXITSIZEMOVE",
0x0233 : "WM_DROPFILES",

0x0240 : "WM_TOUCH",

# Win32 4.0 messages for IME */
0x0281 : "WM_IME_SETCONTEXT",
0x0282 : "WM_IME_NOTIFY",
0x0283 : "WM_IME_CONTROL",
0x0284 : "WM_IME_COMPOSITIONFULL",
0x0285 : "WM_IME_SELECT",
0x0286 : "WM_IME_CHAR",
# Win32 5.0 messages for IME */
0x0288 : "WM_IME_REQUEST",

# Win32 4.0 messages for IME */
0x0290 : "WM_IME_KEYDOWN",
0x0291 : "WM_IME_KEYUP",

0x02A0 : "WM_NCMOUSEHOVER",
0x02A1 : "WM_MOUSEHOVER",
0x02A3 : "WM_MOUSELEAVE",
0x02A2 : "WM_NCMOUSELEAVE",

0x02B1 : "WM_WTSSESSION_CHANGE",

0x02c0 : "WM_TABLET_FIRST",
0x02df : "WM_TABLET_LAST",

# Clipboard command messages */
0x0300 : "WM_CUT",
0x0301 : "WM_COPY",
0x0302 : "WM_PASTE",
0x0303 : "WM_CLEAR",
0x0304 : "WM_UNDO",

# Clipboard owner messages */
0x0305 : "WM_RENDERFORMAT",
0x0306 : "WM_RENDERALLFORMATS",
0x0307 : "WM_DESTROYCLIPBOARD",

# Clipboard viewer messages */
0x0308 : "WM_DRAWCLIPBOARD",
0x0309 : "WM_PAINTCLIPBOARD",
0x030A : "WM_VSCROLLCLIPBOARD",
0x030B : "WM_SIZECLIPBOARD",
0x030C : "WM_ASKCBFORMATNAME",
0x030D : "WM_CHANGECBCHAIN",
0x030E : "WM_HSCROLLCLIPBOARD",

0x030F : "WM_QUERYNEWPALETTE",
0x0310 : "WM_PALETTEISCHANGING",
0x0311 : "WM_PALETTECHANGED",
0x0312 : "WM_HOTKEY",

0x0317 : "WM_PRINT",
0x0318 : "WM_PRINTCLIENT",
0x0319 : "WM_APPCOMMAND",
0x031A : "WM_THEMECHANGED",
0x031D : "WM_CLIPBOARDUPDATE",

0x031E : "WM_DWMCOMPOSITIONCHANGED",
0x031F : "WM_DWMNCRENDERINGCHANGED",
0x0320 : "WM_DWMCOLORIZATIONCOLORCHANGED",
0x0321 : "WM_DWMWINDOWMAXIMIZEDCHANGE",
0x0323 : "WM_DWMSENDICONICTHUMBNAIL",
0x0326 : "WM_DWMSENDICONICLIVEPREVIEWBITMAP",

0x033F : "WM_GETTITLEBARINFOEX",

0x0358 : "WM_HANDHELDFIRST",
0x035F : "WM_HANDHELDLAST",

0x0360 : "WM_AFXFIRST",
0x037F : "WM_AFXLAST",

0x0380 : "WM_PENWINFIRST",
0x038F : "WM_PENWINLAST",

0x0400 : "WM_USER",

0x8000 : "WM_APP"
}
