
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


    JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1HasClipboardText(JNIEnv* env, jclass klass){
    SDL_bool rv;
    rv=SDL_HasClipboardText();
    return rv;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1ClearError(JNIEnv* env, jclass klass){
    SDL_ClearError();
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1Error(JNIEnv* env, jclass klass,jint code){
    int rv;
    rv=SDL_Error(code);
    return rv;
}
JNIEXPORT jbyteArray JNICALL Java_JSDL_JSDL_SDL_1GetPixelFormatName(JNIEnv* env, jclass klass,jint format){
    const char* rv;
    rv=SDL_GetPixelFormatName(format);
    int slen = (int)strlen(rv);
    jbyteArray rv_ = (*env)->NewByteArray(env,slen);
    return rv_;}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1PixelFormatEnumToMasks(JNIEnv* env, jclass klass,jint format,jintArray bpp,jintArray Rmask,jintArray Gmask,jintArray Bmask,jintArray Amask){
    SDL_bool rv;
    jint* _bpp;
    _bpp = (*env)->GetIntArrayElements(env,bpp,NULL);
    jint* _Rmask;
    _Rmask = (*env)->GetIntArrayElements(env,Rmask,NULL);
    jint* _Gmask;
    _Gmask = (*env)->GetIntArrayElements(env,Gmask,NULL);
    jint* _Bmask;
    _Bmask = (*env)->GetIntArrayElements(env,Bmask,NULL);
    jint* _Amask;
    _Amask = (*env)->GetIntArrayElements(env,Amask,NULL);
    rv=SDL_PixelFormatEnumToMasks(format,_bpp,_Rmask,_Gmask,_Bmask,_Amask);
    (*env)->ReleaseIntArrayElements(env,bpp, _bpp,0);
    (*env)->ReleaseIntArrayElements(env,Rmask, _Rmask,0);
    (*env)->ReleaseIntArrayElements(env,Gmask, _Gmask,0);
    (*env)->ReleaseIntArrayElements(env,Bmask, _Bmask,0);
    (*env)->ReleaseIntArrayElements(env,Amask, _Amask,0);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1MasksToPixelFormatEnum(JNIEnv* env, jclass klass,jint bpp,jint Rmask,jint Gmask,jint Bmask,jint Amask){
    Uint32 rv;
    rv=SDL_MasksToPixelFormatEnum(bpp,Rmask,Gmask,Bmask,Amask);
    return rv;
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1AllocFormat(JNIEnv* env, jclass klass,jint pixel_format){
    SDL_PixelFormat* rv;
    rv=SDL_AllocFormat(pixel_format);
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1FreeFormat(JNIEnv* env, jclass klass,jlong format){
    SDL_PixelFormat* _format;
    memcpy(& _format, &format, sizeof(_format) );
    SDL_FreeFormat(_format);
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1AllocPalette(JNIEnv* env, jclass klass,jint ncolors){
    SDL_Palette* rv;
    rv=SDL_AllocPalette(ncolors);
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1SetPixelFormatPalette(JNIEnv* env, jclass klass,jlong format,jlong palette){
    int rv;
    SDL_PixelFormat* _format;
    memcpy(& _format, &format, sizeof(_format) );
    SDL_Palette* _palette;
    memcpy(& _palette, &palette, sizeof(_palette) );
    rv=SDL_SetPixelFormatPalette(_format,_palette);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1SetPaletteColors(JNIEnv* env, jclass klass,jlong palette,jlong colors,jint firstcolor,jint ncolors){
    int rv;
    SDL_Palette* _palette;
    memcpy(& _palette, &palette, sizeof(_palette) );
     SDL_Color* _colors;
    memcpy(& _colors, &colors, sizeof(_colors) );
    rv=SDL_SetPaletteColors(_palette,_colors,firstcolor,ncolors);
    return rv;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1FreePalette(JNIEnv* env, jclass klass,jlong palette){
    SDL_Palette* _palette;
    memcpy(& _palette, &palette, sizeof(_palette) );
    SDL_FreePalette(_palette);
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1MapRGB(JNIEnv* env, jclass klass,jlong format,jbyte r,jbyte g,jbyte b){
    Uint32 rv;
     SDL_PixelFormat* _format;
    memcpy(& _format, &format, sizeof(_format) );
    rv=SDL_MapRGB(_format,r,g,b);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1MapRGBA(JNIEnv* env, jclass klass,jlong format,jbyte r,jbyte g,jbyte b,jbyte a){
    Uint32 rv;
     SDL_PixelFormat* _format;
    memcpy(& _format, &format, sizeof(_format) );
    rv=SDL_MapRGBA(_format,r,g,b,a);
    return rv;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1GetRGB(JNIEnv* env, jclass klass,jint pixel,jlong format,jbyteArray r,jbyteArray g,jbyteArray b){
     SDL_PixelFormat* _format;
    memcpy(& _format, &format, sizeof(_format) );
    jbyte* _r;
    _r = (*env)->GetByteArrayElements(env,r,NULL);
    jbyte* _g;
    _g = (*env)->GetByteArrayElements(env,g,NULL);
    jbyte* _b;
    _b = (*env)->GetByteArrayElements(env,b,NULL);
    SDL_GetRGB(pixel,_format,_r,_g,_b);
    (*env)->ReleaseByteArrayElements(env,r, _r,0);
    (*env)->ReleaseByteArrayElements(env,g, _g,0);
    (*env)->ReleaseByteArrayElements(env,b, _b,0);
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1GetRGBA(JNIEnv* env, jclass klass,jint pixel,jlong format,jbyteArray r,jbyteArray g,jbyteArray b,jbyteArray a){
     SDL_PixelFormat* _format;
    memcpy(& _format, &format, sizeof(_format) );
    jbyte* _r;
    _r = (*env)->GetByteArrayElements(env,r,NULL);
    jbyte* _g;
    _g = (*env)->GetByteArrayElements(env,g,NULL);
    jbyte* _b;
    _b = (*env)->GetByteArrayElements(env,b,NULL);
    jbyte* _a;
    _a = (*env)->GetByteArrayElements(env,a,NULL);
    SDL_GetRGBA(pixel,_format,_r,_g,_b,_a);
    (*env)->ReleaseByteArrayElements(env,r, _r,0);
    (*env)->ReleaseByteArrayElements(env,g, _g,0);
    (*env)->ReleaseByteArrayElements(env,b, _b,0);
    (*env)->ReleaseByteArrayElements(env,a, _a,0);
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1CalculateGammaRamp(JNIEnv* env, jclass klass,jfloat gamma,jshortArray ramp){
    jshort* _ramp;
    _ramp = (*env)->GetShortArrayElements(env,ramp,NULL);
    SDL_CalculateGammaRamp(gamma,_ramp);
    (*env)->ReleaseShortArrayElements(env,ramp, _ramp,0);
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1CreateRGBSurface(JNIEnv* env, jclass klass,jint flags,jint width,jint height,jint depth,jint Rmask,jint Gmask,jint Bmask,jint Amask){
    SDL_Surface* rv;
    rv=SDL_CreateRGBSurface(flags,width,height,depth,Rmask,Gmask,Bmask,Amask);
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1CreateRGBSurfaceFrom(JNIEnv* env, jclass klass,jlong pixels,jint width,jint height,jint depth,jint pitch,jint Rmask,jint Gmask,jint Bmask,jint Amask){
    SDL_Surface* rv;
    void* _pixels;
    memcpy(& _pixels, &pixels, sizeof(_pixels) );
    rv=SDL_CreateRGBSurfaceFrom(_pixels,width,height,depth,pitch,Rmask,Gmask,Bmask,Amask);
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1FreeSurface(JNIEnv* env, jclass klass,jlong surface){
    SDL_Surface* _surface;
    memcpy(& _surface, &surface, sizeof(_surface) );
    SDL_FreeSurface(_surface);
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1SetSurfacePalette(JNIEnv* env, jclass klass,jlong surface,jlong palette){
    int rv;
    SDL_Surface* _surface;
    memcpy(& _surface, &surface, sizeof(_surface) );
    SDL_Palette* _palette;
    memcpy(& _palette, &palette, sizeof(_palette) );
    rv=SDL_SetSurfacePalette(_surface,_palette);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1LockSurface(JNIEnv* env, jclass klass,jlong surface){
    int rv;
    SDL_Surface* _surface;
    memcpy(& _surface, &surface, sizeof(_surface) );
    rv=SDL_LockSurface(_surface);
    return rv;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1UnlockSurface(JNIEnv* env, jclass klass,jlong surface){
    SDL_Surface* _surface;
    memcpy(& _surface, &surface, sizeof(_surface) );
    SDL_UnlockSurface(_surface);
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1LoadBMP_1RW(JNIEnv* env, jclass klass,jlong src,jint freesrc){
    SDL_Surface* rv;
    SDL_RWops* _src;
    memcpy(& _src, &src, sizeof(_src) );
    rv=SDL_LoadBMP_RW(_src,freesrc);
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1SaveBMP_1RW(JNIEnv* env, jclass klass,jlong surface,jlong dst,jint freedst){
    int rv;
    SDL_Surface* _surface;
    memcpy(& _surface, &surface, sizeof(_surface) );
    SDL_RWops* _dst;
    memcpy(& _dst, &dst, sizeof(_dst) );
    rv=SDL_SaveBMP_RW(_surface,_dst,freedst);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1SetSurfaceRLE(JNIEnv* env, jclass klass,jlong surface,jint flag){
    int rv;
    SDL_Surface* _surface;
    memcpy(& _surface, &surface, sizeof(_surface) );
    rv=SDL_SetSurfaceRLE(_surface,flag);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1SetColorKey(JNIEnv* env, jclass klass,jlong surface,jint flag,jint key){
    int rv;
    SDL_Surface* _surface;
    memcpy(& _surface, &surface, sizeof(_surface) );
    rv=SDL_SetColorKey(_surface,flag,key);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetColorKey(JNIEnv* env, jclass klass,jlong surface,jintArray key){
    int rv;
    SDL_Surface* _surface;
    memcpy(& _surface, &surface, sizeof(_surface) );
    jint* _key;
    _key = (*env)->GetIntArrayElements(env,key,NULL);
    rv=SDL_GetColorKey(_surface,_key);
    (*env)->ReleaseIntArrayElements(env,key, _key,0);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1SetSurfaceColorMod(JNIEnv* env, jclass klass,jlong surface,jbyte r,jbyte g,jbyte b){
    int rv;
    SDL_Surface* _surface;
    memcpy(& _surface, &surface, sizeof(_surface) );
    rv=SDL_SetSurfaceColorMod(_surface,r,g,b);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetSurfaceColorMod(JNIEnv* env, jclass klass,jlong surface,jbyteArray r,jbyteArray g,jbyteArray b){
    int rv;
    SDL_Surface* _surface;
    memcpy(& _surface, &surface, sizeof(_surface) );
    jbyte* _r;
    _r = (*env)->GetByteArrayElements(env,r,NULL);
    jbyte* _g;
    _g = (*env)->GetByteArrayElements(env,g,NULL);
    jbyte* _b;
    _b = (*env)->GetByteArrayElements(env,b,NULL);
    rv=SDL_GetSurfaceColorMod(_surface,_r,_g,_b);
    (*env)->ReleaseByteArrayElements(env,r, _r,0);
    (*env)->ReleaseByteArrayElements(env,g, _g,0);
    (*env)->ReleaseByteArrayElements(env,b, _b,0);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1SetSurfaceAlphaMod(JNIEnv* env, jclass klass,jlong surface,jbyte alpha){
    int rv;
    SDL_Surface* _surface;
    memcpy(& _surface, &surface, sizeof(_surface) );
    rv=SDL_SetSurfaceAlphaMod(_surface,alpha);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetSurfaceAlphaMod(JNIEnv* env, jclass klass,jlong surface,jbyteArray alpha){
    int rv;
    SDL_Surface* _surface;
    memcpy(& _surface, &surface, sizeof(_surface) );
    jbyte* _alpha;
    _alpha = (*env)->GetByteArrayElements(env,alpha,NULL);
    rv=SDL_GetSurfaceAlphaMod(_surface,_alpha);
    (*env)->ReleaseByteArrayElements(env,alpha, _alpha,0);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1SetSurfaceBlendMode(JNIEnv* env, jclass klass,jlong surface,jint blendMode){
    int rv;
    SDL_Surface* _surface;
    memcpy(& _surface, &surface, sizeof(_surface) );
    rv=SDL_SetSurfaceBlendMode(_surface,blendMode);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetSurfaceBlendMode(JNIEnv* env, jclass klass,jlong surface,jlong blendMode){
    int rv;
    SDL_Surface* _surface;
    memcpy(& _surface, &surface, sizeof(_surface) );
    SDL_BlendMode* _blendMode;
    memcpy(& _blendMode, &blendMode, sizeof(_blendMode) );
    rv=SDL_GetSurfaceBlendMode(_surface,_blendMode);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1SetClipRect(JNIEnv* env, jclass klass,jlong surface,jlong rect){
    SDL_bool rv;
    SDL_Surface* _surface;
    memcpy(& _surface, &surface, sizeof(_surface) );
     SDL_Rect* _rect;
    memcpy(& _rect, &rect, sizeof(_rect) );
    rv=SDL_SetClipRect(_surface,_rect);
    return rv;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1GetClipRect(JNIEnv* env, jclass klass,jlong surface,jlong rect){
    SDL_Surface* _surface;
    memcpy(& _surface, &surface, sizeof(_surface) );
    SDL_Rect* _rect;
    memcpy(& _rect, &rect, sizeof(_rect) );
    SDL_GetClipRect(_surface,_rect);
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1ConvertSurface(JNIEnv* env, jclass klass,jlong src,jlong fmt,jint flags){
    SDL_Surface* rv;
    SDL_Surface* _src;
    memcpy(& _src, &src, sizeof(_src) );
     SDL_PixelFormat* _fmt;
    memcpy(& _fmt, &fmt, sizeof(_fmt) );
    rv=SDL_ConvertSurface(_src,_fmt,flags);
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1ConvertSurfaceFormat(JNIEnv* env, jclass klass,jlong src,jint pixel_format,jint flags){
    SDL_Surface* rv;
    SDL_Surface* _src;
    memcpy(& _src, &src, sizeof(_src) );
    rv=SDL_ConvertSurfaceFormat(_src,pixel_format,flags);
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1ConvertPixels(JNIEnv* env, jclass klass,jint width,jint height,jint src_format,jlong src,jint src_pitch,jint dst_format,jlong dst,jint dst_pitch){
    int rv;
     void* _src;
    memcpy(& _src, &src, sizeof(_src) );
    void* _dst;
    memcpy(& _dst, &dst, sizeof(_dst) );
    rv=SDL_ConvertPixels(width,height,src_format,_src,src_pitch,dst_format,_dst,dst_pitch);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1FillRect(JNIEnv* env, jclass klass,jlong dst,jlong rect,jint color){
    int rv;
    SDL_Surface* _dst;
    memcpy(& _dst, &dst, sizeof(_dst) );
     SDL_Rect* _rect;
    memcpy(& _rect, &rect, sizeof(_rect) );
    rv=SDL_FillRect(_dst,_rect,color);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1FillRects(JNIEnv* env, jclass klass,jlong dst,jlong rects,jint count,jint color){
    int rv;
    SDL_Surface* _dst;
    memcpy(& _dst, &dst, sizeof(_dst) );
     SDL_Rect* _rects;
    memcpy(& _rects, &rects, sizeof(_rects) );
    rv=SDL_FillRects(_dst,_rects,count,color);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1UpperBlit(JNIEnv* env, jclass klass,jlong src,jlong srcrect,jlong dst,jlong dstrect){
    int rv;
    SDL_Surface* _src;
    memcpy(& _src, &src, sizeof(_src) );
     SDL_Rect* _srcrect;
    memcpy(& _srcrect, &srcrect, sizeof(_srcrect) );
    SDL_Surface* _dst;
    memcpy(& _dst, &dst, sizeof(_dst) );
    SDL_Rect* _dstrect;
    memcpy(& _dstrect, &dstrect, sizeof(_dstrect) );
    rv=SDL_UpperBlit(_src,_srcrect,_dst,_dstrect);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1LowerBlit(JNIEnv* env, jclass klass,jlong src,jlong srcrect,jlong dst,jlong dstrect){
    int rv;
    SDL_Surface* _src;
    memcpy(& _src, &src, sizeof(_src) );
    SDL_Rect* _srcrect;
    memcpy(& _srcrect, &srcrect, sizeof(_srcrect) );
    SDL_Surface* _dst;
    memcpy(& _dst, &dst, sizeof(_dst) );
    SDL_Rect* _dstrect;
    memcpy(& _dstrect, &dstrect, sizeof(_dstrect) );
    rv=SDL_LowerBlit(_src,_srcrect,_dst,_dstrect);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1SoftStretch(JNIEnv* env, jclass klass,jlong src,jlong srcrect,jlong dst,jlong dstrect){
    int rv;
    SDL_Surface* _src;
    memcpy(& _src, &src, sizeof(_src) );
     SDL_Rect* _srcrect;
    memcpy(& _srcrect, &srcrect, sizeof(_srcrect) );
    SDL_Surface* _dst;
    memcpy(& _dst, &dst, sizeof(_dst) );
     SDL_Rect* _dstrect;
    memcpy(& _dstrect, &dstrect, sizeof(_dstrect) );
    rv=SDL_SoftStretch(_src,_srcrect,_dst,_dstrect);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1UpperBlitScaled(JNIEnv* env, jclass klass,jlong src,jlong srcrect,jlong dst,jlong dstrect){
    int rv;
    SDL_Surface* _src;
    memcpy(& _src, &src, sizeof(_src) );
     SDL_Rect* _srcrect;
    memcpy(& _srcrect, &srcrect, sizeof(_srcrect) );
    SDL_Surface* _dst;
    memcpy(& _dst, &dst, sizeof(_dst) );
    SDL_Rect* _dstrect;
    memcpy(& _dstrect, &dstrect, sizeof(_dstrect) );
    rv=SDL_UpperBlitScaled(_src,_srcrect,_dst,_dstrect);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1LowerBlitScaled(JNIEnv* env, jclass klass,jlong src,jlong srcrect,jlong dst,jlong dstrect){
    int rv;
    SDL_Surface* _src;
    memcpy(& _src, &src, sizeof(_src) );
    SDL_Rect* _srcrect;
    memcpy(& _srcrect, &srcrect, sizeof(_srcrect) );
    SDL_Surface* _dst;
    memcpy(& _dst, &dst, sizeof(_dst) );
    SDL_Rect* _dstrect;
    memcpy(& _dstrect, &dstrect, sizeof(_dstrect) );
    rv=SDL_LowerBlitScaled(_src,_srcrect,_dst,_dstrect);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetNumVideoDrivers(JNIEnv* env, jclass klass){
    int rv;
    rv=SDL_GetNumVideoDrivers();
    return rv;
}
JNIEXPORT jbyteArray JNICALL Java_JSDL_JSDL_SDL_1GetVideoDriver(JNIEnv* env, jclass klass,jint index){
    const char* rv;
    rv=SDL_GetVideoDriver(index);
    int slen = (int)strlen(rv);
    jbyteArray rv_ = (*env)->NewByteArray(env,slen);
    return rv_;}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1VideoInit(JNIEnv* env, jclass klass,jbyteArray driver_name){
    int rv;
    jbyte* _driver_name;
    _driver_name = (*env)->GetByteArrayElements(env,driver_name,NULL);
    rv=SDL_VideoInit(_driver_name);
    (*env)->ReleaseByteArrayElements(env,driver_name, _driver_name,0);
    return rv;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1VideoQuit(JNIEnv* env, jclass klass){
    SDL_VideoQuit();
}
JNIEXPORT jbyteArray JNICALL Java_JSDL_JSDL_SDL_1GetCurrentVideoDriver(JNIEnv* env, jclass klass){
    const char* rv;
    rv=SDL_GetCurrentVideoDriver();
    int slen = (int)strlen(rv);
    jbyteArray rv_ = (*env)->NewByteArray(env,slen);
    return rv_;}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetNumVideoDisplays(JNIEnv* env, jclass klass){
    int rv;
    rv=SDL_GetNumVideoDisplays();
    return rv;
}
JNIEXPORT jbyteArray JNICALL Java_JSDL_JSDL_SDL_1GetDisplayName(JNIEnv* env, jclass klass,jint displayIndex){
    const char* rv;
    rv=SDL_GetDisplayName(displayIndex);
    int slen = (int)strlen(rv);
    jbyteArray rv_ = (*env)->NewByteArray(env,slen);
    return rv_;}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetDisplayBounds(JNIEnv* env, jclass klass,jint displayIndex,jlong rect){
    int rv;
    SDL_Rect* _rect;
    memcpy(& _rect, &rect, sizeof(_rect) );
    rv=SDL_GetDisplayBounds(displayIndex,_rect);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetNumDisplayModes(JNIEnv* env, jclass klass,jint displayIndex){
    int rv;
    rv=SDL_GetNumDisplayModes(displayIndex);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetDisplayMode(JNIEnv* env, jclass klass,jint displayIndex,jint modeIndex,jlong mode){
    int rv;
    SDL_DisplayMode* _mode;
    memcpy(& _mode, &mode, sizeof(_mode) );
    rv=SDL_GetDisplayMode(displayIndex,modeIndex,_mode);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetDesktopDisplayMode(JNIEnv* env, jclass klass,jint displayIndex,jlong mode){
    int rv;
    SDL_DisplayMode* _mode;
    memcpy(& _mode, &mode, sizeof(_mode) );
    rv=SDL_GetDesktopDisplayMode(displayIndex,_mode);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetCurrentDisplayMode(JNIEnv* env, jclass klass,jint displayIndex,jlong mode){
    int rv;
    SDL_DisplayMode* _mode;
    memcpy(& _mode, &mode, sizeof(_mode) );
    rv=SDL_GetCurrentDisplayMode(displayIndex,_mode);
    return rv;
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1GetClosestDisplayMode(JNIEnv* env, jclass klass,jint displayIndex,jlong mode,jlong closest){
    SDL_DisplayMode* rv;
     SDL_DisplayMode* _mode;
    memcpy(& _mode, &mode, sizeof(_mode) );
    SDL_DisplayMode* _closest;
    memcpy(& _closest, &closest, sizeof(_closest) );
    rv=SDL_GetClosestDisplayMode(displayIndex,_mode,_closest);
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetWindowDisplayIndex(JNIEnv* env, jclass klass,jlong window){
    int rv;
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    rv=SDL_GetWindowDisplayIndex(_window);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1SetWindowDisplayMode(JNIEnv* env, jclass klass,jlong window,jlong mode){
    int rv;
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
     SDL_DisplayMode* _mode;
    memcpy(& _mode, &mode, sizeof(_mode) );
    rv=SDL_SetWindowDisplayMode(_window,_mode);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetWindowDisplayMode(JNIEnv* env, jclass klass,jlong window,jlong mode){
    int rv;
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    SDL_DisplayMode* _mode;
    memcpy(& _mode, &mode, sizeof(_mode) );
    rv=SDL_GetWindowDisplayMode(_window,_mode);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetWindowPixelFormat(JNIEnv* env, jclass klass,jlong window){
    Uint32 rv;
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    rv=SDL_GetWindowPixelFormat(_window);
    return rv;
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1CreateWindow(JNIEnv* env, jclass klass,jbyteArray title,jint x,jint y,jint w,jint h,jint flags){
    SDL_Window* rv;
    jbyte* _title;
    _title = (*env)->GetByteArrayElements(env,title,NULL);
    rv=SDL_CreateWindow(_title,x,y,w,h,flags);
    (*env)->ReleaseByteArrayElements(env,title, _title,0);
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1CreateWindowFrom(JNIEnv* env, jclass klass,jlong data){
    SDL_Window* rv;
     void* _data;
    memcpy(& _data, &data, sizeof(_data) );
    rv=SDL_CreateWindowFrom(_data);
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetWindowID(JNIEnv* env, jclass klass,jlong window){
    Uint32 rv;
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    rv=SDL_GetWindowID(_window);
    return rv;
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1GetWindowFromID(JNIEnv* env, jclass klass,jint id){
    SDL_Window* rv;
    rv=SDL_GetWindowFromID(id);
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetWindowFlags(JNIEnv* env, jclass klass,jlong window){
    Uint32 rv;
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    rv=SDL_GetWindowFlags(_window);
    return rv;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1SetWindowTitle(JNIEnv* env, jclass klass,jlong window,jbyteArray title){
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    jbyte* _title;
    _title = (*env)->GetByteArrayElements(env,title,NULL);
    SDL_SetWindowTitle(_window,_title);
    (*env)->ReleaseByteArrayElements(env,title, _title,0);
}
JNIEXPORT jbyteArray JNICALL Java_JSDL_JSDL_SDL_1GetWindowTitle(JNIEnv* env, jclass klass,jlong window){
    const char* rv;
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    rv=SDL_GetWindowTitle(_window);
    int slen = (int)strlen(rv);
    jbyteArray rv_ = (*env)->NewByteArray(env,slen);
    return rv_;}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1SetWindowIcon(JNIEnv* env, jclass klass,jlong window,jlong icon){
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    SDL_Surface* _icon;
    memcpy(& _icon, &icon, sizeof(_icon) );
    SDL_SetWindowIcon(_window,_icon);
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1SetWindowData(JNIEnv* env, jclass klass,jlong window,jbyteArray name,jlong userdata){
    void* rv;
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    jbyte* _name;
    _name = (*env)->GetByteArrayElements(env,name,NULL);
    void* _userdata;
    memcpy(& _userdata, &userdata, sizeof(_userdata) );
    rv=SDL_SetWindowData(_window,_name,_userdata);
    (*env)->ReleaseByteArrayElements(env,name, _name,0);
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1GetWindowData(JNIEnv* env, jclass klass,jlong window,jbyteArray name){
    void* rv;
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    jbyte* _name;
    _name = (*env)->GetByteArrayElements(env,name,NULL);
    rv=SDL_GetWindowData(_window,_name);
    (*env)->ReleaseByteArrayElements(env,name, _name,0);
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1SetWindowPosition(JNIEnv* env, jclass klass,jlong window,jint x,jint y){
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    SDL_SetWindowPosition(_window,x,y);
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1GetWindowPosition(JNIEnv* env, jclass klass,jlong window,jintArray x,jintArray y){
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    jint* _x;
    _x = (*env)->GetIntArrayElements(env,x,NULL);
    jint* _y;
    _y = (*env)->GetIntArrayElements(env,y,NULL);
    SDL_GetWindowPosition(_window,_x,_y);
    (*env)->ReleaseIntArrayElements(env,x, _x,0);
    (*env)->ReleaseIntArrayElements(env,y, _y,0);
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1SetWindowSize(JNIEnv* env, jclass klass,jlong window,jint w,jint h){
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    SDL_SetWindowSize(_window,w,h);
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1GetWindowSize(JNIEnv* env, jclass klass,jlong window,jintArray w,jintArray h){
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    jint* _w;
    _w = (*env)->GetIntArrayElements(env,w,NULL);
    jint* _h;
    _h = (*env)->GetIntArrayElements(env,h,NULL);
    SDL_GetWindowSize(_window,_w,_h);
    (*env)->ReleaseIntArrayElements(env,w, _w,0);
    (*env)->ReleaseIntArrayElements(env,h, _h,0);
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1SetWindowMinimumSize(JNIEnv* env, jclass klass,jlong window,jint min_w,jint min_h){
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    SDL_SetWindowMinimumSize(_window,min_w,min_h);
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1GetWindowMinimumSize(JNIEnv* env, jclass klass,jlong window,jintArray w,jintArray h){
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    jint* _w;
    _w = (*env)->GetIntArrayElements(env,w,NULL);
    jint* _h;
    _h = (*env)->GetIntArrayElements(env,h,NULL);
    SDL_GetWindowMinimumSize(_window,_w,_h);
    (*env)->ReleaseIntArrayElements(env,w, _w,0);
    (*env)->ReleaseIntArrayElements(env,h, _h,0);
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1SetWindowMaximumSize(JNIEnv* env, jclass klass,jlong window,jint max_w,jint max_h){
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    SDL_SetWindowMaximumSize(_window,max_w,max_h);
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1GetWindowMaximumSize(JNIEnv* env, jclass klass,jlong window,jintArray w,jintArray h){
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    jint* _w;
    _w = (*env)->GetIntArrayElements(env,w,NULL);
    jint* _h;
    _h = (*env)->GetIntArrayElements(env,h,NULL);
    SDL_GetWindowMaximumSize(_window,_w,_h);
    (*env)->ReleaseIntArrayElements(env,w, _w,0);
    (*env)->ReleaseIntArrayElements(env,h, _h,0);
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1SetWindowBordered(JNIEnv* env, jclass klass,jlong window,jint bordered){
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    SDL_SetWindowBordered(_window,bordered);
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1ShowWindow(JNIEnv* env, jclass klass,jlong window){
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    SDL_ShowWindow(_window);
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1HideWindow(JNIEnv* env, jclass klass,jlong window){
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    SDL_HideWindow(_window);
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1RaiseWindow(JNIEnv* env, jclass klass,jlong window){
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    SDL_RaiseWindow(_window);
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1MaximizeWindow(JNIEnv* env, jclass klass,jlong window){
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    SDL_MaximizeWindow(_window);
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1MinimizeWindow(JNIEnv* env, jclass klass,jlong window){
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    SDL_MinimizeWindow(_window);
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1RestoreWindow(JNIEnv* env, jclass klass,jlong window){
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    SDL_RestoreWindow(_window);
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1SetWindowFullscreen(JNIEnv* env, jclass klass,jlong window,jint flags){
    int rv;
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    rv=SDL_SetWindowFullscreen(_window,flags);
    return rv;
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1GetWindowSurface(JNIEnv* env, jclass klass,jlong window){
    SDL_Surface* rv;
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    rv=SDL_GetWindowSurface(_window);
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1UpdateWindowSurface(JNIEnv* env, jclass klass,jlong window){
    int rv;
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    rv=SDL_UpdateWindowSurface(_window);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1UpdateWindowSurfaceRects(JNIEnv* env, jclass klass,jlong window,jlong rects,jint numrects){
    int rv;
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
     SDL_Rect* _rects;
    memcpy(& _rects, &rects, sizeof(_rects) );
    rv=SDL_UpdateWindowSurfaceRects(_window,_rects,numrects);
    return rv;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1SetWindowGrab(JNIEnv* env, jclass klass,jlong window,jint grabbed){
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    SDL_SetWindowGrab(_window,grabbed);
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetWindowGrab(JNIEnv* env, jclass klass,jlong window){
    SDL_bool rv;
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    rv=SDL_GetWindowGrab(_window);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1SetWindowBrightness(JNIEnv* env, jclass klass,jlong window,jfloat brightness){
    int rv;
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    rv=SDL_SetWindowBrightness(_window,brightness);
    return rv;
}
JNIEXPORT jfloat JNICALL Java_JSDL_JSDL_SDL_1GetWindowBrightness(JNIEnv* env, jclass klass,jlong window){
    float rv;
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    rv=SDL_GetWindowBrightness(_window);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1SetWindowGammaRamp(JNIEnv* env, jclass klass,jlong window,jshortArray red,jshortArray green,jshortArray blue){
    int rv;
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    jshort* _red;
    _red = (*env)->GetShortArrayElements(env,red,NULL);
    jshort* _green;
    _green = (*env)->GetShortArrayElements(env,green,NULL);
    jshort* _blue;
    _blue = (*env)->GetShortArrayElements(env,blue,NULL);
    rv=SDL_SetWindowGammaRamp(_window,_red,_green,_blue);
    (*env)->ReleaseShortArrayElements(env,red, _red,0);
    (*env)->ReleaseShortArrayElements(env,green, _green,0);
    (*env)->ReleaseShortArrayElements(env,blue, _blue,0);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetWindowGammaRamp(JNIEnv* env, jclass klass,jlong window,jshortArray red,jshortArray green,jshortArray blue){
    int rv;
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    jshort* _red;
    _red = (*env)->GetShortArrayElements(env,red,NULL);
    jshort* _green;
    _green = (*env)->GetShortArrayElements(env,green,NULL);
    jshort* _blue;
    _blue = (*env)->GetShortArrayElements(env,blue,NULL);
    rv=SDL_GetWindowGammaRamp(_window,_red,_green,_blue);
    (*env)->ReleaseShortArrayElements(env,red, _red,0);
    (*env)->ReleaseShortArrayElements(env,green, _green,0);
    (*env)->ReleaseShortArrayElements(env,blue, _blue,0);
    return rv;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1DestroyWindow(JNIEnv* env, jclass klass,jlong window){
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    SDL_DestroyWindow(_window);
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1IsScreenSaverEnabled(JNIEnv* env, jclass klass){
    SDL_bool rv;
    rv=SDL_IsScreenSaverEnabled();
    return rv;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1EnableScreenSaver(JNIEnv* env, jclass klass){
    SDL_EnableScreenSaver();
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1DisableScreenSaver(JNIEnv* env, jclass klass){
    SDL_DisableScreenSaver();
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GL_1LoadLibrary(JNIEnv* env, jclass klass,jbyteArray path){
    int rv;
    jbyte* _path;
    _path = (*env)->GetByteArrayElements(env,path,NULL);
    rv=SDL_GL_LoadLibrary(_path);
    (*env)->ReleaseByteArrayElements(env,path, _path,0);
    return rv;
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1GL_1GetProcAddress(JNIEnv* env, jclass klass,jbyteArray proc){
    void* rv;
    jbyte* _proc;
    _proc = (*env)->GetByteArrayElements(env,proc,NULL);
    rv=SDL_GL_GetProcAddress(_proc);
    (*env)->ReleaseByteArrayElements(env,proc, _proc,0);
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1GL_1UnloadLibrary(JNIEnv* env, jclass klass){
    SDL_GL_UnloadLibrary();
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GL_1ExtensionSupported(JNIEnv* env, jclass klass,jbyteArray extension){
    SDL_bool rv;
    jbyte* _extension;
    _extension = (*env)->GetByteArrayElements(env,extension,NULL);
    rv=SDL_GL_ExtensionSupported(_extension);
    (*env)->ReleaseByteArrayElements(env,extension, _extension,0);
    return rv;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1GL_1ResetAttributes(JNIEnv* env, jclass klass){
    SDL_GL_ResetAttributes();
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GL_1SetAttribute(JNIEnv* env, jclass klass,jint attr,jint value){
    int rv;
    rv=SDL_GL_SetAttribute(attr,value);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GL_1GetAttribute(JNIEnv* env, jclass klass,jint attr,jintArray value){
    int rv;
    jint* _value;
    _value = (*env)->GetIntArrayElements(env,value,NULL);
    rv=SDL_GL_GetAttribute(attr,_value);
    (*env)->ReleaseIntArrayElements(env,value, _value,0);
    return rv;
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1GL_1CreateContext(JNIEnv* env, jclass klass,jlong window){
    SDL_GLContext rv;
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    rv=SDL_GL_CreateContext(_window);
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GL_1MakeCurrent(JNIEnv* env, jclass klass,jlong window,jlong context){
    int rv;
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    SDL_GLContext _context;
    memcpy(& _context, &context, sizeof(_context) );
    rv=SDL_GL_MakeCurrent(_window,_context);
    return rv;
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1GL_1GetCurrentWindow(JNIEnv* env, jclass klass){
    SDL_Window* rv;
    rv=SDL_GL_GetCurrentWindow();
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1GL_1GetCurrentContext(JNIEnv* env, jclass klass){
    SDL_GLContext rv;
    rv=SDL_GL_GetCurrentContext();
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1GL_1GetDrawableSize(JNIEnv* env, jclass klass,jlong window,jintArray w,jintArray h){
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    jint* _w;
    _w = (*env)->GetIntArrayElements(env,w,NULL);
    jint* _h;
    _h = (*env)->GetIntArrayElements(env,h,NULL);
    SDL_GL_GetDrawableSize(_window,_w,_h);
    (*env)->ReleaseIntArrayElements(env,w, _w,0);
    (*env)->ReleaseIntArrayElements(env,h, _h,0);
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GL_1SetSwapInterval(JNIEnv* env, jclass klass,jint interval){
    int rv;
    rv=SDL_GL_SetSwapInterval(interval);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GL_1GetSwapInterval(JNIEnv* env, jclass klass){
    int rv;
    rv=SDL_GL_GetSwapInterval();
    return rv;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1GL_1SwapWindow(JNIEnv* env, jclass klass,jlong window){
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    SDL_GL_SwapWindow(_window);
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1GL_1DeleteContext(JNIEnv* env, jclass klass,jlong context){
    SDL_GLContext _context;
    memcpy(& _context, &context, sizeof(_context) );
    SDL_GL_DeleteContext(_context);
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1GetKeyboardFocus(JNIEnv* env, jclass klass){
    SDL_Window* rv;
    rv=SDL_GetKeyboardFocus();
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetModState(JNIEnv* env, jclass klass){
    SDL_Keymod rv;
    rv=SDL_GetModState();
    return (jint) rv;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1SetModState(JNIEnv* env, jclass klass,jint modstate){
    SDL_SetModState(modstate);
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetKeyFromScancode(JNIEnv* env, jclass klass,jint scancode){
    SDL_Keycode rv;
    rv=SDL_GetKeyFromScancode(scancode);
    return (jint) rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetScancodeFromKey(JNIEnv* env, jclass klass,jint key){
    SDL_Scancode rv;
    rv=SDL_GetScancodeFromKey(key);
    return (jint) rv;
}
JNIEXPORT jbyteArray JNICALL Java_JSDL_JSDL_SDL_1GetScancodeName(JNIEnv* env, jclass klass,jint scancode){
    const char* rv;
    rv=SDL_GetScancodeName(scancode);
    int slen = (int)strlen(rv);
    jbyteArray rv_ = (*env)->NewByteArray(env,slen);
    return rv_;}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetScancodeFromName(JNIEnv* env, jclass klass,jbyteArray name){
    SDL_Scancode rv;
    jbyte* _name;
    _name = (*env)->GetByteArrayElements(env,name,NULL);
    rv=SDL_GetScancodeFromName(_name);
    (*env)->ReleaseByteArrayElements(env,name, _name,0);
    return (jint) rv;
}
JNIEXPORT jbyteArray JNICALL Java_JSDL_JSDL_SDL_1GetKeyName(JNIEnv* env, jclass klass,jint key){
    const char* rv;
    rv=SDL_GetKeyName(key);
    int slen = (int)strlen(rv);
    jbyteArray rv_ = (*env)->NewByteArray(env,slen);
    return rv_;}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetKeyFromName(JNIEnv* env, jclass klass,jbyteArray name){
    SDL_Keycode rv;
    jbyte* _name;
    _name = (*env)->GetByteArrayElements(env,name,NULL);
    rv=SDL_GetKeyFromName(_name);
    (*env)->ReleaseByteArrayElements(env,name, _name,0);
    return (jint) rv;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1StartTextInput(JNIEnv* env, jclass klass){
    SDL_StartTextInput();
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1IsTextInputActive(JNIEnv* env, jclass klass){
    SDL_bool rv;
    rv=SDL_IsTextInputActive();
    return rv;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1StopTextInput(JNIEnv* env, jclass klass){
    SDL_StopTextInput();
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1SetTextInputRect(JNIEnv* env, jclass klass,jlong rect){
    SDL_Rect* _rect;
    memcpy(& _rect, &rect, sizeof(_rect) );
    SDL_SetTextInputRect(_rect);
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1HasScreenKeyboardSupport(JNIEnv* env, jclass klass){
    SDL_bool rv;
    rv=SDL_HasScreenKeyboardSupport();
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1IsScreenKeyboardShown(JNIEnv* env, jclass klass,jlong window){
    SDL_bool rv;
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    rv=SDL_IsScreenKeyboardShown(_window);
    return rv;
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1GetMouseFocus(JNIEnv* env, jclass klass){
    SDL_Window* rv;
    rv=SDL_GetMouseFocus();
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetMouseState(JNIEnv* env, jclass klass,jintArray x,jintArray y){
    Uint32 rv;
    jint* _x;
    _x = (*env)->GetIntArrayElements(env,x,NULL);
    jint* _y;
    _y = (*env)->GetIntArrayElements(env,y,NULL);
    rv=SDL_GetMouseState(_x,_y);
    (*env)->ReleaseIntArrayElements(env,x, _x,0);
    (*env)->ReleaseIntArrayElements(env,y, _y,0);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetRelativeMouseState(JNIEnv* env, jclass klass,jintArray x,jintArray y){
    Uint32 rv;
    jint* _x;
    _x = (*env)->GetIntArrayElements(env,x,NULL);
    jint* _y;
    _y = (*env)->GetIntArrayElements(env,y,NULL);
    rv=SDL_GetRelativeMouseState(_x,_y);
    (*env)->ReleaseIntArrayElements(env,x, _x,0);
    (*env)->ReleaseIntArrayElements(env,y, _y,0);
    return rv;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1WarpMouseInWindow(JNIEnv* env, jclass klass,jlong window,jint x,jint y){
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    SDL_WarpMouseInWindow(_window,x,y);
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1SetRelativeMouseMode(JNIEnv* env, jclass klass,jint enabled){
    int rv;
    rv=SDL_SetRelativeMouseMode(enabled);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetRelativeMouseMode(JNIEnv* env, jclass klass){
    SDL_bool rv;
    rv=SDL_GetRelativeMouseMode();
    return rv;
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1CreateCursor(JNIEnv* env, jclass klass,jbyteArray data,jbyteArray mask,jint w,jint h,jint hot_x,jint hot_y){
    SDL_Cursor* rv;
    jbyte* _data;
    _data = (*env)->GetByteArrayElements(env,data,NULL);
    jbyte* _mask;
    _mask = (*env)->GetByteArrayElements(env,mask,NULL);
    rv=SDL_CreateCursor(_data,_mask,w,h,hot_x,hot_y);
    (*env)->ReleaseByteArrayElements(env,data, _data,0);
    (*env)->ReleaseByteArrayElements(env,mask, _mask,0);
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1CreateColorCursor(JNIEnv* env, jclass klass,jlong surface,jint hot_x,jint hot_y){
    SDL_Cursor* rv;
    SDL_Surface* _surface;
    memcpy(& _surface, &surface, sizeof(_surface) );
    rv=SDL_CreateColorCursor(_surface,hot_x,hot_y);
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1CreateSystemCursor(JNIEnv* env, jclass klass,jint id){
    SDL_Cursor* rv;
    rv=SDL_CreateSystemCursor(id);
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1SetCursor(JNIEnv* env, jclass klass,jlong cursor){
    SDL_Cursor* _cursor;
    memcpy(& _cursor, &cursor, sizeof(_cursor) );
    SDL_SetCursor(_cursor);
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1GetCursor(JNIEnv* env, jclass klass){
    SDL_Cursor* rv;
    rv=SDL_GetCursor();
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1GetDefaultCursor(JNIEnv* env, jclass klass){
    SDL_Cursor* rv;
    rv=SDL_GetDefaultCursor();
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1FreeCursor(JNIEnv* env, jclass klass,jlong cursor){
    SDL_Cursor* _cursor;
    memcpy(& _cursor, &cursor, sizeof(_cursor) );
    SDL_FreeCursor(_cursor);
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1ShowCursor(JNIEnv* env, jclass klass,jint toggle){
    int rv;
    rv=SDL_ShowCursor(toggle);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1NumJoysticks(JNIEnv* env, jclass klass){
    int rv;
    rv=SDL_NumJoysticks();
    return rv;
}
JNIEXPORT jbyteArray JNICALL Java_JSDL_JSDL_SDL_1JoystickNameForIndex(JNIEnv* env, jclass klass,jint device_index){
    const char* rv;
    rv=SDL_JoystickNameForIndex(device_index);
    int slen = (int)strlen(rv);
    jbyteArray rv_ = (*env)->NewByteArray(env,slen);
    return rv_;}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1JoystickOpen(JNIEnv* env, jclass klass,jint device_index){
    SDL_Joystick* rv;
    rv=SDL_JoystickOpen(device_index);
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT jbyteArray JNICALL Java_JSDL_JSDL_SDL_1JoystickName(JNIEnv* env, jclass klass,jlong joystick){
    const char* rv;
    SDL_Joystick* _joystick;
    memcpy(& _joystick, &joystick, sizeof(_joystick) );
    rv=SDL_JoystickName(_joystick);
    int slen = (int)strlen(rv);
    jbyteArray rv_ = (*env)->NewByteArray(env,slen);
    return rv_;}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1JoystickGetAttached(JNIEnv* env, jclass klass,jlong joystick){
    SDL_bool rv;
    SDL_Joystick* _joystick;
    memcpy(& _joystick, &joystick, sizeof(_joystick) );
    rv=SDL_JoystickGetAttached(_joystick);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1JoystickInstanceID(JNIEnv* env, jclass klass,jlong joystick){
    SDL_JoystickID rv;
    SDL_Joystick* _joystick;
    memcpy(& _joystick, &joystick, sizeof(_joystick) );
    rv=SDL_JoystickInstanceID(_joystick);
    return (jint) rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1JoystickNumAxes(JNIEnv* env, jclass klass,jlong joystick){
    int rv;
    SDL_Joystick* _joystick;
    memcpy(& _joystick, &joystick, sizeof(_joystick) );
    rv=SDL_JoystickNumAxes(_joystick);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1JoystickNumBalls(JNIEnv* env, jclass klass,jlong joystick){
    int rv;
    SDL_Joystick* _joystick;
    memcpy(& _joystick, &joystick, sizeof(_joystick) );
    rv=SDL_JoystickNumBalls(_joystick);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1JoystickNumHats(JNIEnv* env, jclass klass,jlong joystick){
    int rv;
    SDL_Joystick* _joystick;
    memcpy(& _joystick, &joystick, sizeof(_joystick) );
    rv=SDL_JoystickNumHats(_joystick);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1JoystickNumButtons(JNIEnv* env, jclass klass,jlong joystick){
    int rv;
    SDL_Joystick* _joystick;
    memcpy(& _joystick, &joystick, sizeof(_joystick) );
    rv=SDL_JoystickNumButtons(_joystick);
    return rv;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1JoystickUpdate(JNIEnv* env, jclass klass){
    SDL_JoystickUpdate();
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1JoystickEventState(JNIEnv* env, jclass klass,jint state){
    int rv;
    rv=SDL_JoystickEventState(state);
    return rv;
}
JNIEXPORT jshort JNICALL Java_JSDL_JSDL_SDL_1JoystickGetAxis(JNIEnv* env, jclass klass,jlong joystick,jint axis){
    Sint16 rv;
    SDL_Joystick* _joystick;
    memcpy(& _joystick, &joystick, sizeof(_joystick) );
    rv=SDL_JoystickGetAxis(_joystick,axis);
    return rv;
}
JNIEXPORT jbyte JNICALL Java_JSDL_JSDL_SDL_1JoystickGetHat(JNIEnv* env, jclass klass,jlong joystick,jint hat){
    Uint8 rv;
    SDL_Joystick* _joystick;
    memcpy(& _joystick, &joystick, sizeof(_joystick) );
    rv=SDL_JoystickGetHat(_joystick,hat);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1JoystickGetBall(JNIEnv* env, jclass klass,jlong joystick,jint ball,jintArray dx,jintArray dy){
    int rv;
    SDL_Joystick* _joystick;
    memcpy(& _joystick, &joystick, sizeof(_joystick) );
    jint* _dx;
    _dx = (*env)->GetIntArrayElements(env,dx,NULL);
    jint* _dy;
    _dy = (*env)->GetIntArrayElements(env,dy,NULL);
    rv=SDL_JoystickGetBall(_joystick,ball,_dx,_dy);
    (*env)->ReleaseIntArrayElements(env,dx, _dx,0);
    (*env)->ReleaseIntArrayElements(env,dy, _dy,0);
    return rv;
}
JNIEXPORT jbyte JNICALL Java_JSDL_JSDL_SDL_1JoystickGetButton(JNIEnv* env, jclass klass,jlong joystick,jint button){
    Uint8 rv;
    SDL_Joystick* _joystick;
    memcpy(& _joystick, &joystick, sizeof(_joystick) );
    rv=SDL_JoystickGetButton(_joystick,button);
    return rv;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1JoystickClose(JNIEnv* env, jclass klass,jlong joystick){
    SDL_Joystick* _joystick;
    memcpy(& _joystick, &joystick, sizeof(_joystick) );
    SDL_JoystickClose(_joystick);
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GameControllerAddMappingsFromRW(JNIEnv* env, jclass klass,jlong rw,jint freerw){
    int rv;
    SDL_RWops* _rw;
    memcpy(& _rw, &rw, sizeof(_rw) );
    rv=SDL_GameControllerAddMappingsFromRW(_rw,freerw);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GameControllerAddMapping(JNIEnv* env, jclass klass,jbyteArray mappingString){
    int rv;
    jbyte* _mappingString;
    _mappingString = (*env)->GetByteArrayElements(env,mappingString,NULL);
    rv=SDL_GameControllerAddMapping(_mappingString);
    (*env)->ReleaseByteArrayElements(env,mappingString, _mappingString,0);
    return rv;
}
JNIEXPORT jbyteArray JNICALL Java_JSDL_JSDL_SDL_1GameControllerMapping(JNIEnv* env, jclass klass,jlong gamecontroller){
    char* rv;
    SDL_GameController* _gamecontroller;
    memcpy(& _gamecontroller, &gamecontroller, sizeof(_gamecontroller) );
    rv=SDL_GameControllerMapping(_gamecontroller);
    int slen = (int)strlen(rv);
    jbyteArray rv_ = (*env)->NewByteArray(env,slen);
    return rv_;}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1IsGameController(JNIEnv* env, jclass klass,jint joystick_index){
    SDL_bool rv;
    rv=SDL_IsGameController(joystick_index);
    return rv;
}
JNIEXPORT jbyteArray JNICALL Java_JSDL_JSDL_SDL_1GameControllerNameForIndex(JNIEnv* env, jclass klass,jint joystick_index){
    const char* rv;
    rv=SDL_GameControllerNameForIndex(joystick_index);
    int slen = (int)strlen(rv);
    jbyteArray rv_ = (*env)->NewByteArray(env,slen);
    return rv_;}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1GameControllerOpen(JNIEnv* env, jclass klass,jint joystick_index){
    SDL_GameController* rv;
    rv=SDL_GameControllerOpen(joystick_index);
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT jbyteArray JNICALL Java_JSDL_JSDL_SDL_1GameControllerName(JNIEnv* env, jclass klass,jlong gamecontroller){
    const char* rv;
    SDL_GameController* _gamecontroller;
    memcpy(& _gamecontroller, &gamecontroller, sizeof(_gamecontroller) );
    rv=SDL_GameControllerName(_gamecontroller);
    int slen = (int)strlen(rv);
    jbyteArray rv_ = (*env)->NewByteArray(env,slen);
    return rv_;}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GameControllerGetAttached(JNIEnv* env, jclass klass,jlong gamecontroller){
    SDL_bool rv;
    SDL_GameController* _gamecontroller;
    memcpy(& _gamecontroller, &gamecontroller, sizeof(_gamecontroller) );
    rv=SDL_GameControllerGetAttached(_gamecontroller);
    return rv;
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1GameControllerGetJoystick(JNIEnv* env, jclass klass,jlong gamecontroller){
    SDL_Joystick* rv;
    SDL_GameController* _gamecontroller;
    memcpy(& _gamecontroller, &gamecontroller, sizeof(_gamecontroller) );
    rv=SDL_GameControllerGetJoystick(_gamecontroller);
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GameControllerEventState(JNIEnv* env, jclass klass,jint state){
    int rv;
    rv=SDL_GameControllerEventState(state);
    return rv;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1GameControllerUpdate(JNIEnv* env, jclass klass){
    SDL_GameControllerUpdate();
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GameControllerGetAxisFromString(JNIEnv* env, jclass klass,jbyteArray pchString){
    SDL_GameControllerAxis rv;
    jbyte* _pchString;
    _pchString = (*env)->GetByteArrayElements(env,pchString,NULL);
    rv=SDL_GameControllerGetAxisFromString(_pchString);
    (*env)->ReleaseByteArrayElements(env,pchString, _pchString,0);
    return (jint) rv;
}
JNIEXPORT jbyteArray JNICALL Java_JSDL_JSDL_SDL_1GameControllerGetStringForAxis(JNIEnv* env, jclass klass,jint axis){
    const char* rv;
    rv=SDL_GameControllerGetStringForAxis(axis);
    int slen = (int)strlen(rv);
    jbyteArray rv_ = (*env)->NewByteArray(env,slen);
    return rv_;}
JNIEXPORT jshort JNICALL Java_JSDL_JSDL_SDL_1GameControllerGetAxis(JNIEnv* env, jclass klass,jlong gamecontroller,jint axis){
    Sint16 rv;
    SDL_GameController* _gamecontroller;
    memcpy(& _gamecontroller, &gamecontroller, sizeof(_gamecontroller) );
    rv=SDL_GameControllerGetAxis(_gamecontroller,axis);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GameControllerGetButtonFromString(JNIEnv* env, jclass klass,jbyteArray pchString){
    SDL_GameControllerButton rv;
    jbyte* _pchString;
    _pchString = (*env)->GetByteArrayElements(env,pchString,NULL);
    rv=SDL_GameControllerGetButtonFromString(_pchString);
    (*env)->ReleaseByteArrayElements(env,pchString, _pchString,0);
    return (jint) rv;
}
JNIEXPORT jbyteArray JNICALL Java_JSDL_JSDL_SDL_1GameControllerGetStringForButton(JNIEnv* env, jclass klass,jint button){
    const char* rv;
    rv=SDL_GameControllerGetStringForButton(button);
    int slen = (int)strlen(rv);
    jbyteArray rv_ = (*env)->NewByteArray(env,slen);
    return rv_;}
JNIEXPORT jbyte JNICALL Java_JSDL_JSDL_SDL_1GameControllerGetButton(JNIEnv* env, jclass klass,jlong gamecontroller,jint button){
    Uint8 rv;
    SDL_GameController* _gamecontroller;
    memcpy(& _gamecontroller, &gamecontroller, sizeof(_gamecontroller) );
    rv=SDL_GameControllerGetButton(_gamecontroller,button);
    return rv;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1GameControllerClose(JNIEnv* env, jclass klass,jlong gamecontroller){
    SDL_GameController* _gamecontroller;
    memcpy(& _gamecontroller, &gamecontroller, sizeof(_gamecontroller) );
    SDL_GameControllerClose(_gamecontroller);
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetNumTouchDevices(JNIEnv* env, jclass klass){
    int rv;
    rv=SDL_GetNumTouchDevices();
    return rv;
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1GetTouchDevice(JNIEnv* env, jclass klass,jint index){
    SDL_TouchID rv;
    rv=SDL_GetTouchDevice(index);
    return (long)rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetNumTouchFingers(JNIEnv* env, jclass klass,jlong touchID){
    int rv;
    rv=SDL_GetNumTouchFingers(touchID);
    return rv;
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1GetTouchFinger(JNIEnv* env, jclass klass,jlong touchID,jint index){
    SDL_Finger* rv;
    rv=SDL_GetTouchFinger(touchID,index);
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1RecordGesture(JNIEnv* env, jclass klass,jlong touchId){
    int rv;
    rv=SDL_RecordGesture(touchId);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1SaveAllDollarTemplates(JNIEnv* env, jclass klass,jlong dst){
    int rv;
    SDL_RWops* _dst;
    memcpy(& _dst, &dst, sizeof(_dst) );
    rv=SDL_SaveAllDollarTemplates(_dst);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1SaveDollarTemplate(JNIEnv* env, jclass klass,jlong gestureId,jlong dst){
    int rv;
    SDL_RWops* _dst;
    memcpy(& _dst, &dst, sizeof(_dst) );
    rv=SDL_SaveDollarTemplate(gestureId,_dst);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1LoadDollarTemplates(JNIEnv* env, jclass klass,jlong touchId,jlong src){
    int rv;
    SDL_RWops* _src;
    memcpy(& _src, &src, sizeof(_src) );
    rv=SDL_LoadDollarTemplates(touchId,_src);
    return rv;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1PumpEvents(JNIEnv* env, jclass klass){
    SDL_PumpEvents();
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1PeepEvents(JNIEnv* env, jclass klass,jlong events,jint numevents,jint action,jint minType,jint maxType){
    int rv;
    SDL_Event* _events;
    memcpy(& _events, &events, sizeof(_events) );
    rv=SDL_PeepEvents(_events,numevents,action,minType,maxType);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1HasEvent(JNIEnv* env, jclass klass,jint type){
    SDL_bool rv;
    rv=SDL_HasEvent(type);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1HasEvents(JNIEnv* env, jclass klass,jint minType,jint maxType){
    SDL_bool rv;
    rv=SDL_HasEvents(minType,maxType);
    return rv;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1FlushEvent(JNIEnv* env, jclass klass,jint type){
    SDL_FlushEvent(type);
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1FlushEvents(JNIEnv* env, jclass klass,jint minType,jint maxType){
    SDL_FlushEvents(minType,maxType);
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1WaitEventTimeout(JNIEnv* env, jclass klass,jlong event,jint timeout){
    int rv;
    SDL_Event* _event;
    memcpy(& _event, &event, sizeof(_event) );
    rv=SDL_WaitEventTimeout(_event,timeout);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1PushEvent(JNIEnv* env, jclass klass,jlong event){
    int rv;
    SDL_Event* _event;
    memcpy(& _event, &event, sizeof(_event) );
    rv=SDL_PushEvent(_event);
    return rv;
}
JNIEXPORT jbyte JNICALL Java_JSDL_JSDL_SDL_1EventState(JNIEnv* env, jclass klass,jint type,jint state){
    Uint8 rv;
    rv=SDL_EventState(type,state);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1RegisterEvents(JNIEnv* env, jclass klass,jint numevents){
    Uint32 rv;
    rv=SDL_RegisterEvents(numevents);
    return rv;
}
JNIEXPORT jbyteArray JNICALL Java_JSDL_JSDL_SDL_1GetBasePath(JNIEnv* env, jclass klass){
    char* rv;
    rv=SDL_GetBasePath();
    int slen = (int)strlen(rv);
    jbyteArray rv_ = (*env)->NewByteArray(env,slen);
    return rv_;}
JNIEXPORT jbyteArray JNICALL Java_JSDL_JSDL_SDL_1GetPrefPath(JNIEnv* env, jclass klass,jbyteArray org,jbyteArray app){
    char* rv;
    jbyte* _org;
    _org = (*env)->GetByteArrayElements(env,org,NULL);
    jbyte* _app;
    _app = (*env)->GetByteArrayElements(env,app,NULL);
    rv=SDL_GetPrefPath(_org,_app);
    (*env)->ReleaseByteArrayElements(env,org, _org,0);
    (*env)->ReleaseByteArrayElements(env,app, _app,0);
    int slen = (int)strlen(rv);
    jbyteArray rv_ = (*env)->NewByteArray(env,slen);
    return rv_;}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1NumHaptics(JNIEnv* env, jclass klass){
    int rv;
    rv=SDL_NumHaptics();
    return rv;
}
JNIEXPORT jbyteArray JNICALL Java_JSDL_JSDL_SDL_1HapticName(JNIEnv* env, jclass klass,jint device_index){
    const char* rv;
    rv=SDL_HapticName(device_index);
    int slen = (int)strlen(rv);
    jbyteArray rv_ = (*env)->NewByteArray(env,slen);
    return rv_;}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1HapticOpen(JNIEnv* env, jclass klass,jint device_index){
    SDL_Haptic* rv;
    rv=SDL_HapticOpen(device_index);
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1HapticOpened(JNIEnv* env, jclass klass,jint device_index){
    int rv;
    rv=SDL_HapticOpened(device_index);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1HapticIndex(JNIEnv* env, jclass klass,jlong haptic){
    int rv;
    SDL_Haptic* _haptic;
    memcpy(& _haptic, &haptic, sizeof(_haptic) );
    rv=SDL_HapticIndex(_haptic);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1MouseIsHaptic(JNIEnv* env, jclass klass){
    int rv;
    rv=SDL_MouseIsHaptic();
    return rv;
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1HapticOpenFromMouse(JNIEnv* env, jclass klass){
    SDL_Haptic* rv;
    rv=SDL_HapticOpenFromMouse();
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1JoystickIsHaptic(JNIEnv* env, jclass klass,jlong joystick){
    int rv;
    SDL_Joystick* _joystick;
    memcpy(& _joystick, &joystick, sizeof(_joystick) );
    rv=SDL_JoystickIsHaptic(_joystick);
    return rv;
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1HapticOpenFromJoystick(JNIEnv* env, jclass klass,jlong joystick){
    SDL_Haptic* rv;
    SDL_Joystick* _joystick;
    memcpy(& _joystick, &joystick, sizeof(_joystick) );
    rv=SDL_HapticOpenFromJoystick(_joystick);
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1HapticClose(JNIEnv* env, jclass klass,jlong haptic){
    SDL_Haptic* _haptic;
    memcpy(& _haptic, &haptic, sizeof(_haptic) );
    SDL_HapticClose(_haptic);
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1HapticNumEffects(JNIEnv* env, jclass klass,jlong haptic){
    int rv;
    SDL_Haptic* _haptic;
    memcpy(& _haptic, &haptic, sizeof(_haptic) );
    rv=SDL_HapticNumEffects(_haptic);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1HapticNumEffectsPlaying(JNIEnv* env, jclass klass,jlong haptic){
    int rv;
    SDL_Haptic* _haptic;
    memcpy(& _haptic, &haptic, sizeof(_haptic) );
    rv=SDL_HapticNumEffectsPlaying(_haptic);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1HapticQuery(JNIEnv* env, jclass klass,jlong haptic){
    unsigned int rv;
    SDL_Haptic* _haptic;
    memcpy(& _haptic, &haptic, sizeof(_haptic) );
    rv=SDL_HapticQuery(_haptic);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1HapticNumAxes(JNIEnv* env, jclass klass,jlong haptic){
    int rv;
    SDL_Haptic* _haptic;
    memcpy(& _haptic, &haptic, sizeof(_haptic) );
    rv=SDL_HapticNumAxes(_haptic);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1HapticEffectSupported(JNIEnv* env, jclass klass,jlong haptic,jlong effect){
    int rv;
    SDL_Haptic* _haptic;
    memcpy(& _haptic, &haptic, sizeof(_haptic) );
    SDL_HapticEffect* _effect;
    memcpy(& _effect, &effect, sizeof(_effect) );
    rv=SDL_HapticEffectSupported(_haptic,_effect);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1HapticNewEffect(JNIEnv* env, jclass klass,jlong haptic,jlong effect){
    int rv;
    SDL_Haptic* _haptic;
    memcpy(& _haptic, &haptic, sizeof(_haptic) );
    SDL_HapticEffect* _effect;
    memcpy(& _effect, &effect, sizeof(_effect) );
    rv=SDL_HapticNewEffect(_haptic,_effect);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1HapticUpdateEffect(JNIEnv* env, jclass klass,jlong haptic,jint effect,jlong data){
    int rv;
    SDL_Haptic* _haptic;
    memcpy(& _haptic, &haptic, sizeof(_haptic) );
    SDL_HapticEffect* _data;
    memcpy(& _data, &data, sizeof(_data) );
    rv=SDL_HapticUpdateEffect(_haptic,effect,_data);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1HapticRunEffect(JNIEnv* env, jclass klass,jlong haptic,jint effect,jint iterations){
    int rv;
    SDL_Haptic* _haptic;
    memcpy(& _haptic, &haptic, sizeof(_haptic) );
    rv=SDL_HapticRunEffect(_haptic,effect,iterations);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1HapticStopEffect(JNIEnv* env, jclass klass,jlong haptic,jint effect){
    int rv;
    SDL_Haptic* _haptic;
    memcpy(& _haptic, &haptic, sizeof(_haptic) );
    rv=SDL_HapticStopEffect(_haptic,effect);
    return rv;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1HapticDestroyEffect(JNIEnv* env, jclass klass,jlong haptic,jint effect){
    SDL_Haptic* _haptic;
    memcpy(& _haptic, &haptic, sizeof(_haptic) );
    SDL_HapticDestroyEffect(_haptic,effect);
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1HapticGetEffectStatus(JNIEnv* env, jclass klass,jlong haptic,jint effect){
    int rv;
    SDL_Haptic* _haptic;
    memcpy(& _haptic, &haptic, sizeof(_haptic) );
    rv=SDL_HapticGetEffectStatus(_haptic,effect);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1HapticSetGain(JNIEnv* env, jclass klass,jlong haptic,jint gain){
    int rv;
    SDL_Haptic* _haptic;
    memcpy(& _haptic, &haptic, sizeof(_haptic) );
    rv=SDL_HapticSetGain(_haptic,gain);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1HapticSetAutocenter(JNIEnv* env, jclass klass,jlong haptic,jint autocenter){
    int rv;
    SDL_Haptic* _haptic;
    memcpy(& _haptic, &haptic, sizeof(_haptic) );
    rv=SDL_HapticSetAutocenter(_haptic,autocenter);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1HapticPause(JNIEnv* env, jclass klass,jlong haptic){
    int rv;
    SDL_Haptic* _haptic;
    memcpy(& _haptic, &haptic, sizeof(_haptic) );
    rv=SDL_HapticPause(_haptic);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1HapticUnpause(JNIEnv* env, jclass klass,jlong haptic){
    int rv;
    SDL_Haptic* _haptic;
    memcpy(& _haptic, &haptic, sizeof(_haptic) );
    rv=SDL_HapticUnpause(_haptic);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1HapticStopAll(JNIEnv* env, jclass klass,jlong haptic){
    int rv;
    SDL_Haptic* _haptic;
    memcpy(& _haptic, &haptic, sizeof(_haptic) );
    rv=SDL_HapticStopAll(_haptic);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1HapticRumbleSupported(JNIEnv* env, jclass klass,jlong haptic){
    int rv;
    SDL_Haptic* _haptic;
    memcpy(& _haptic, &haptic, sizeof(_haptic) );
    rv=SDL_HapticRumbleSupported(_haptic);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1HapticRumbleInit(JNIEnv* env, jclass klass,jlong haptic){
    int rv;
    SDL_Haptic* _haptic;
    memcpy(& _haptic, &haptic, sizeof(_haptic) );
    rv=SDL_HapticRumbleInit(_haptic);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1HapticRumblePlay(JNIEnv* env, jclass klass,jlong haptic,jfloat strength,jint length){
    int rv;
    SDL_Haptic* _haptic;
    memcpy(& _haptic, &haptic, sizeof(_haptic) );
    rv=SDL_HapticRumblePlay(_haptic,strength,length);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1HapticRumbleStop(JNIEnv* env, jclass klass,jlong haptic){
    int rv;
    SDL_Haptic* _haptic;
    memcpy(& _haptic, &haptic, sizeof(_haptic) );
    rv=SDL_HapticRumbleStop(_haptic);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1SetHintWithPriority(JNIEnv* env, jclass klass,jbyteArray name,jbyteArray value,jint priority){
    SDL_bool rv;
    jbyte* _name;
    _name = (*env)->GetByteArrayElements(env,name,NULL);
    jbyte* _value;
    _value = (*env)->GetByteArrayElements(env,value,NULL);
    rv=SDL_SetHintWithPriority(_name,_value,priority);
    (*env)->ReleaseByteArrayElements(env,name, _name,0);
    (*env)->ReleaseByteArrayElements(env,value, _value,0);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1SetHint(JNIEnv* env, jclass klass,jbyteArray name,jbyteArray value){
    SDL_bool rv;
    jbyte* _name;
    _name = (*env)->GetByteArrayElements(env,name,NULL);
    jbyte* _value;
    _value = (*env)->GetByteArrayElements(env,value,NULL);
    rv=SDL_SetHint(_name,_value);
    (*env)->ReleaseByteArrayElements(env,name, _name,0);
    (*env)->ReleaseByteArrayElements(env,value, _value,0);
    return rv;
}
JNIEXPORT jbyteArray JNICALL Java_JSDL_JSDL_SDL_1GetHint(JNIEnv* env, jclass klass,jbyteArray name){
    const char* rv;
    jbyte* _name;
    _name = (*env)->GetByteArrayElements(env,name,NULL);
    rv=SDL_GetHint(_name);
    (*env)->ReleaseByteArrayElements(env,name, _name,0);
    int slen = (int)strlen(rv);
    jbyteArray rv_ = (*env)->NewByteArray(env,slen);
    return rv_;}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1ClearHints(JNIEnv* env, jclass klass){
    SDL_ClearHints();
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1LoadObject(JNIEnv* env, jclass klass,jbyteArray sofile){
    void* rv;
    jbyte* _sofile;
    _sofile = (*env)->GetByteArrayElements(env,sofile,NULL);
    rv=SDL_LoadObject(_sofile);
    (*env)->ReleaseByteArrayElements(env,sofile, _sofile,0);
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1LoadFunction(JNIEnv* env, jclass klass,jlong handle,jbyteArray name){
    void* rv;
    void* _handle;
    memcpy(& _handle, &handle, sizeof(_handle) );
    jbyte* _name;
    _name = (*env)->GetByteArrayElements(env,name,NULL);
    rv=SDL_LoadFunction(_handle,_name);
    (*env)->ReleaseByteArrayElements(env,name, _name,0);
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1UnloadObject(JNIEnv* env, jclass klass,jlong handle){
    void* _handle;
    memcpy(& _handle, &handle, sizeof(_handle) );
    SDL_UnloadObject(_handle);
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1LogSetAllPriority(JNIEnv* env, jclass klass,jint priority){
    SDL_LogSetAllPriority(priority);
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1LogSetPriority(JNIEnv* env, jclass klass,jint category,jint priority){
    SDL_LogSetPriority(category,priority);
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1LogGetPriority(JNIEnv* env, jclass klass,jint category){
    SDL_LogPriority rv;
    rv=SDL_LogGetPriority(category);
    return (jint) rv;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1LogResetPriorities(JNIEnv* env, jclass klass){
    SDL_LogResetPriorities();
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1ShowMessageBox(JNIEnv* env, jclass klass,jlong messageboxdata,jintArray buttonid){
    int rv;
     SDL_MessageBoxData* _messageboxdata;
    memcpy(& _messageboxdata, &messageboxdata, sizeof(_messageboxdata) );
    jint* _buttonid;
    _buttonid = (*env)->GetIntArrayElements(env,buttonid,NULL);
    rv=SDL_ShowMessageBox(_messageboxdata,_buttonid);
    (*env)->ReleaseIntArrayElements(env,buttonid, _buttonid,0);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1ShowSimpleMessageBox(JNIEnv* env, jclass klass,jint flags,jbyteArray title,jbyteArray message,jlong window){
    int rv;
    jbyte* _title;
    _title = (*env)->GetByteArrayElements(env,title,NULL);
    jbyte* _message;
    _message = (*env)->GetByteArrayElements(env,message,NULL);
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    rv=SDL_ShowSimpleMessageBox(flags,_title,_message,_window);
    (*env)->ReleaseByteArrayElements(env,title, _title,0);
    (*env)->ReleaseByteArrayElements(env,message, _message,0);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetPowerInfo(JNIEnv* env, jclass klass,jintArray secs,jintArray pct){
    SDL_PowerState rv;
    jint* _secs;
    _secs = (*env)->GetIntArrayElements(env,secs,NULL);
    jint* _pct;
    _pct = (*env)->GetIntArrayElements(env,pct,NULL);
    rv=SDL_GetPowerInfo(_secs,_pct);
    (*env)->ReleaseIntArrayElements(env,secs, _secs,0);
    (*env)->ReleaseIntArrayElements(env,pct, _pct,0);
    return (jint) rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetNumRenderDrivers(JNIEnv* env, jclass klass){
    int rv;
    rv=SDL_GetNumRenderDrivers();
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetRenderDriverInfo(JNIEnv* env, jclass klass,jint index,jlong info){
    int rv;
    SDL_RendererInfo* _info;
    memcpy(& _info, &info, sizeof(_info) );
    rv=SDL_GetRenderDriverInfo(index,_info);
    return rv;
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1CreateRenderer(JNIEnv* env, jclass klass,jlong window,jint index,jint flags){
    SDL_Renderer* rv;
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    rv=SDL_CreateRenderer(_window,index,flags);
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1CreateSoftwareRenderer(JNIEnv* env, jclass klass,jlong surface){
    SDL_Renderer* rv;
    SDL_Surface* _surface;
    memcpy(& _surface, &surface, sizeof(_surface) );
    rv=SDL_CreateSoftwareRenderer(_surface);
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1GetRenderer(JNIEnv* env, jclass klass,jlong window){
    SDL_Renderer* rv;
    SDL_Window* _window;
    memcpy(& _window, &window, sizeof(_window) );
    rv=SDL_GetRenderer(_window);
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetRendererInfo(JNIEnv* env, jclass klass,jlong renderer,jlong info){
    int rv;
    SDL_Renderer* _renderer;
    memcpy(& _renderer, &renderer, sizeof(_renderer) );
    SDL_RendererInfo* _info;
    memcpy(& _info, &info, sizeof(_info) );
    rv=SDL_GetRendererInfo(_renderer,_info);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetRendererOutputSize(JNIEnv* env, jclass klass,jlong renderer,jintArray w,jintArray h){
    int rv;
    SDL_Renderer* _renderer;
    memcpy(& _renderer, &renderer, sizeof(_renderer) );
    jint* _w;
    _w = (*env)->GetIntArrayElements(env,w,NULL);
    jint* _h;
    _h = (*env)->GetIntArrayElements(env,h,NULL);
    rv=SDL_GetRendererOutputSize(_renderer,_w,_h);
    (*env)->ReleaseIntArrayElements(env,w, _w,0);
    (*env)->ReleaseIntArrayElements(env,h, _h,0);
    return rv;
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1CreateTexture(JNIEnv* env, jclass klass,jlong renderer,jint format,jint access,jint w,jint h){
    SDL_Texture* rv;
    SDL_Renderer* _renderer;
    memcpy(& _renderer, &renderer, sizeof(_renderer) );
    rv=SDL_CreateTexture(_renderer,format,access,w,h);
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1CreateTextureFromSurface(JNIEnv* env, jclass klass,jlong renderer,jlong surface){
    SDL_Texture* rv;
    SDL_Renderer* _renderer;
    memcpy(& _renderer, &renderer, sizeof(_renderer) );
    SDL_Surface* _surface;
    memcpy(& _surface, &surface, sizeof(_surface) );
    rv=SDL_CreateTextureFromSurface(_renderer,_surface);
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1QueryTexture(JNIEnv* env, jclass klass,jlong texture,jintArray format,jintArray access,jintArray w,jintArray h){
    int rv;
    SDL_Texture* _texture;
    memcpy(& _texture, &texture, sizeof(_texture) );
    jint* _format;
    _format = (*env)->GetIntArrayElements(env,format,NULL);
    jint* _access;
    _access = (*env)->GetIntArrayElements(env,access,NULL);
    jint* _w;
    _w = (*env)->GetIntArrayElements(env,w,NULL);
    jint* _h;
    _h = (*env)->GetIntArrayElements(env,h,NULL);
    rv=SDL_QueryTexture(_texture,_format,_access,_w,_h);
    (*env)->ReleaseIntArrayElements(env,format, _format,0);
    (*env)->ReleaseIntArrayElements(env,access, _access,0);
    (*env)->ReleaseIntArrayElements(env,w, _w,0);
    (*env)->ReleaseIntArrayElements(env,h, _h,0);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1SetTextureColorMod(JNIEnv* env, jclass klass,jlong texture,jbyte r,jbyte g,jbyte b){
    int rv;
    SDL_Texture* _texture;
    memcpy(& _texture, &texture, sizeof(_texture) );
    rv=SDL_SetTextureColorMod(_texture,r,g,b);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetTextureColorMod(JNIEnv* env, jclass klass,jlong texture,jbyteArray r,jbyteArray g,jbyteArray b){
    int rv;
    SDL_Texture* _texture;
    memcpy(& _texture, &texture, sizeof(_texture) );
    jbyte* _r;
    _r = (*env)->GetByteArrayElements(env,r,NULL);
    jbyte* _g;
    _g = (*env)->GetByteArrayElements(env,g,NULL);
    jbyte* _b;
    _b = (*env)->GetByteArrayElements(env,b,NULL);
    rv=SDL_GetTextureColorMod(_texture,_r,_g,_b);
    (*env)->ReleaseByteArrayElements(env,r, _r,0);
    (*env)->ReleaseByteArrayElements(env,g, _g,0);
    (*env)->ReleaseByteArrayElements(env,b, _b,0);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1SetTextureAlphaMod(JNIEnv* env, jclass klass,jlong texture,jbyte alpha){
    int rv;
    SDL_Texture* _texture;
    memcpy(& _texture, &texture, sizeof(_texture) );
    rv=SDL_SetTextureAlphaMod(_texture,alpha);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetTextureAlphaMod(JNIEnv* env, jclass klass,jlong texture,jbyteArray alpha){
    int rv;
    SDL_Texture* _texture;
    memcpy(& _texture, &texture, sizeof(_texture) );
    jbyte* _alpha;
    _alpha = (*env)->GetByteArrayElements(env,alpha,NULL);
    rv=SDL_GetTextureAlphaMod(_texture,_alpha);
    (*env)->ReleaseByteArrayElements(env,alpha, _alpha,0);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1SetTextureBlendMode(JNIEnv* env, jclass klass,jlong texture,jint blendMode){
    int rv;
    SDL_Texture* _texture;
    memcpy(& _texture, &texture, sizeof(_texture) );
    rv=SDL_SetTextureBlendMode(_texture,blendMode);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetTextureBlendMode(JNIEnv* env, jclass klass,jlong texture,jlong blendMode){
    int rv;
    SDL_Texture* _texture;
    memcpy(& _texture, &texture, sizeof(_texture) );
    SDL_BlendMode* _blendMode;
    memcpy(& _blendMode, &blendMode, sizeof(_blendMode) );
    rv=SDL_GetTextureBlendMode(_texture,_blendMode);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1UpdateTexture(JNIEnv* env, jclass klass,jlong texture,jlong rect,jlong pixels,jint pitch){
    int rv;
    SDL_Texture* _texture;
    memcpy(& _texture, &texture, sizeof(_texture) );
     SDL_Rect* _rect;
    memcpy(& _rect, &rect, sizeof(_rect) );
     void* _pixels;
    memcpy(& _pixels, &pixels, sizeof(_pixels) );
    rv=SDL_UpdateTexture(_texture,_rect,_pixels,pitch);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1UpdateYUVTexture(JNIEnv* env, jclass klass,jlong texture,jlong rect,jbyteArray Yplane,jint Ypitch,jbyteArray Uplane,jint Upitch,jbyteArray Vplane,jint Vpitch){
    int rv;
    SDL_Texture* _texture;
    memcpy(& _texture, &texture, sizeof(_texture) );
     SDL_Rect* _rect;
    memcpy(& _rect, &rect, sizeof(_rect) );
    jbyte* _Yplane;
    _Yplane = (*env)->GetByteArrayElements(env,Yplane,NULL);
    jbyte* _Uplane;
    _Uplane = (*env)->GetByteArrayElements(env,Uplane,NULL);
    jbyte* _Vplane;
    _Vplane = (*env)->GetByteArrayElements(env,Vplane,NULL);
    rv=SDL_UpdateYUVTexture(_texture,_rect,_Yplane,Ypitch,_Uplane,Upitch,_Vplane,Vpitch);
    (*env)->ReleaseByteArrayElements(env,Yplane, _Yplane,0);
    (*env)->ReleaseByteArrayElements(env,Uplane, _Uplane,0);
    (*env)->ReleaseByteArrayElements(env,Vplane, _Vplane,0);
    return rv;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1UnlockTexture(JNIEnv* env, jclass klass,jlong texture){
    SDL_Texture* _texture;
    memcpy(& _texture, &texture, sizeof(_texture) );
    SDL_UnlockTexture(_texture);
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1RenderTargetSupported(JNIEnv* env, jclass klass,jlong renderer){
    SDL_bool rv;
    SDL_Renderer* _renderer;
    memcpy(& _renderer, &renderer, sizeof(_renderer) );
    rv=SDL_RenderTargetSupported(_renderer);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1SetRenderTarget(JNIEnv* env, jclass klass,jlong renderer,jlong texture){
    int rv;
    SDL_Renderer* _renderer;
    memcpy(& _renderer, &renderer, sizeof(_renderer) );
    SDL_Texture* _texture;
    memcpy(& _texture, &texture, sizeof(_texture) );
    rv=SDL_SetRenderTarget(_renderer,_texture);
    return rv;
}
JNIEXPORT jlong JNICALL Java_JSDL_JSDL_SDL_1GetRenderTarget(JNIEnv* env, jclass klass,jlong renderer){
    SDL_Texture* rv;
    SDL_Renderer* _renderer;
    memcpy(& _renderer, &renderer, sizeof(_renderer) );
    rv=SDL_GetRenderTarget(_renderer);
    jlong rv_;
    memcpy(&rv_,&rv,sizeof(rv_));
    return rv_;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1RenderSetLogicalSize(JNIEnv* env, jclass klass,jlong renderer,jint w,jint h){
    int rv;
    SDL_Renderer* _renderer;
    memcpy(& _renderer, &renderer, sizeof(_renderer) );
    rv=SDL_RenderSetLogicalSize(_renderer,w,h);
    return rv;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1RenderGetLogicalSize(JNIEnv* env, jclass klass,jlong renderer,jintArray w,jintArray h){
    SDL_Renderer* _renderer;
    memcpy(& _renderer, &renderer, sizeof(_renderer) );
    jint* _w;
    _w = (*env)->GetIntArrayElements(env,w,NULL);
    jint* _h;
    _h = (*env)->GetIntArrayElements(env,h,NULL);
    SDL_RenderGetLogicalSize(_renderer,_w,_h);
    (*env)->ReleaseIntArrayElements(env,w, _w,0);
    (*env)->ReleaseIntArrayElements(env,h, _h,0);
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1RenderSetViewport(JNIEnv* env, jclass klass,jlong renderer,jlong rect){
    int rv;
    SDL_Renderer* _renderer;
    memcpy(& _renderer, &renderer, sizeof(_renderer) );
     SDL_Rect* _rect;
    memcpy(& _rect, &rect, sizeof(_rect) );
    rv=SDL_RenderSetViewport(_renderer,_rect);
    return rv;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1RenderGetViewport(JNIEnv* env, jclass klass,jlong renderer,jlong rect){
    SDL_Renderer* _renderer;
    memcpy(& _renderer, &renderer, sizeof(_renderer) );
    SDL_Rect* _rect;
    memcpy(& _rect, &rect, sizeof(_rect) );
    SDL_RenderGetViewport(_renderer,_rect);
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1RenderSetClipRect(JNIEnv* env, jclass klass,jlong renderer,jlong rect){
    int rv;
    SDL_Renderer* _renderer;
    memcpy(& _renderer, &renderer, sizeof(_renderer) );
     SDL_Rect* _rect;
    memcpy(& _rect, &rect, sizeof(_rect) );
    rv=SDL_RenderSetClipRect(_renderer,_rect);
    return rv;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1RenderGetClipRect(JNIEnv* env, jclass klass,jlong renderer,jlong rect){
    SDL_Renderer* _renderer;
    memcpy(& _renderer, &renderer, sizeof(_renderer) );
    SDL_Rect* _rect;
    memcpy(& _rect, &rect, sizeof(_rect) );
    SDL_RenderGetClipRect(_renderer,_rect);
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1RenderSetScale(JNIEnv* env, jclass klass,jlong renderer,jfloat scaleX,jfloat scaleY){
    int rv;
    SDL_Renderer* _renderer;
    memcpy(& _renderer, &renderer, sizeof(_renderer) );
    rv=SDL_RenderSetScale(_renderer,scaleX,scaleY);
    return rv;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1RenderGetScale(JNIEnv* env, jclass klass,jlong renderer,jfloatArray scaleX,jfloatArray scaleY){
    SDL_Renderer* _renderer;
    memcpy(& _renderer, &renderer, sizeof(_renderer) );
    jfloat* _scaleX;
    _scaleX = (*env)->GetFloatArrayElements(env,scaleX,NULL);
    jfloat* _scaleY;
    _scaleY = (*env)->GetFloatArrayElements(env,scaleY,NULL);
    SDL_RenderGetScale(_renderer,_scaleX,_scaleY);
    (*env)->ReleaseFloatArrayElements(env,scaleX, _scaleX,0);
    (*env)->ReleaseFloatArrayElements(env,scaleY, _scaleY,0);
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1SetRenderDrawColor(JNIEnv* env, jclass klass,jlong renderer,jbyte r,jbyte g,jbyte b,jbyte a){
    int rv;
    SDL_Renderer* _renderer;
    memcpy(& _renderer, &renderer, sizeof(_renderer) );
    rv=SDL_SetRenderDrawColor(_renderer,r,g,b,a);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetRenderDrawColor(JNIEnv* env, jclass klass,jlong renderer,jbyteArray r,jbyteArray g,jbyteArray b,jbyteArray a){
    int rv;
    SDL_Renderer* _renderer;
    memcpy(& _renderer, &renderer, sizeof(_renderer) );
    jbyte* _r;
    _r = (*env)->GetByteArrayElements(env,r,NULL);
    jbyte* _g;
    _g = (*env)->GetByteArrayElements(env,g,NULL);
    jbyte* _b;
    _b = (*env)->GetByteArrayElements(env,b,NULL);
    jbyte* _a;
    _a = (*env)->GetByteArrayElements(env,a,NULL);
    rv=SDL_GetRenderDrawColor(_renderer,_r,_g,_b,_a);
    (*env)->ReleaseByteArrayElements(env,r, _r,0);
    (*env)->ReleaseByteArrayElements(env,g, _g,0);
    (*env)->ReleaseByteArrayElements(env,b, _b,0);
    (*env)->ReleaseByteArrayElements(env,a, _a,0);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1SetRenderDrawBlendMode(JNIEnv* env, jclass klass,jlong renderer,jint blendMode){
    int rv;
    SDL_Renderer* _renderer;
    memcpy(& _renderer, &renderer, sizeof(_renderer) );
    rv=SDL_SetRenderDrawBlendMode(_renderer,blendMode);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GetRenderDrawBlendMode(JNIEnv* env, jclass klass,jlong renderer,jlong blendMode){
    int rv;
    SDL_Renderer* _renderer;
    memcpy(& _renderer, &renderer, sizeof(_renderer) );
    SDL_BlendMode* _blendMode;
    memcpy(& _blendMode, &blendMode, sizeof(_blendMode) );
    rv=SDL_GetRenderDrawBlendMode(_renderer,_blendMode);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1RenderClear(JNIEnv* env, jclass klass,jlong renderer){
    int rv;
    SDL_Renderer* _renderer;
    memcpy(& _renderer, &renderer, sizeof(_renderer) );
    rv=SDL_RenderClear(_renderer);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1RenderDrawPoint(JNIEnv* env, jclass klass,jlong renderer,jint x,jint y){
    int rv;
    SDL_Renderer* _renderer;
    memcpy(& _renderer, &renderer, sizeof(_renderer) );
    rv=SDL_RenderDrawPoint(_renderer,x,y);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1RenderDrawPoints(JNIEnv* env, jclass klass,jlong renderer,jlong points,jint count){
    int rv;
    SDL_Renderer* _renderer;
    memcpy(& _renderer, &renderer, sizeof(_renderer) );
     SDL_Point* _points;
    memcpy(& _points, &points, sizeof(_points) );
    rv=SDL_RenderDrawPoints(_renderer,_points,count);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1RenderDrawLine(JNIEnv* env, jclass klass,jlong renderer,jint x1,jint y1,jint x2,jint y2){
    int rv;
    SDL_Renderer* _renderer;
    memcpy(& _renderer, &renderer, sizeof(_renderer) );
    rv=SDL_RenderDrawLine(_renderer,x1,y1,x2,y2);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1RenderDrawLines(JNIEnv* env, jclass klass,jlong renderer,jlong points,jint count){
    int rv;
    SDL_Renderer* _renderer;
    memcpy(& _renderer, &renderer, sizeof(_renderer) );
     SDL_Point* _points;
    memcpy(& _points, &points, sizeof(_points) );
    rv=SDL_RenderDrawLines(_renderer,_points,count);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1RenderDrawRect(JNIEnv* env, jclass klass,jlong renderer,jlong rect){
    int rv;
    SDL_Renderer* _renderer;
    memcpy(& _renderer, &renderer, sizeof(_renderer) );
     SDL_Rect* _rect;
    memcpy(& _rect, &rect, sizeof(_rect) );
    rv=SDL_RenderDrawRect(_renderer,_rect);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1RenderDrawRects(JNIEnv* env, jclass klass,jlong renderer,jlong rects,jint count){
    int rv;
    SDL_Renderer* _renderer;
    memcpy(& _renderer, &renderer, sizeof(_renderer) );
     SDL_Rect* _rects;
    memcpy(& _rects, &rects, sizeof(_rects) );
    rv=SDL_RenderDrawRects(_renderer,_rects,count);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1RenderFillRect(JNIEnv* env, jclass klass,jlong renderer,jlong rect){
    int rv;
    SDL_Renderer* _renderer;
    memcpy(& _renderer, &renderer, sizeof(_renderer) );
     SDL_Rect* _rect;
    memcpy(& _rect, &rect, sizeof(_rect) );
    rv=SDL_RenderFillRect(_renderer,_rect);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1RenderFillRects(JNIEnv* env, jclass klass,jlong renderer,jlong rects,jint count){
    int rv;
    SDL_Renderer* _renderer;
    memcpy(& _renderer, &renderer, sizeof(_renderer) );
     SDL_Rect* _rects;
    memcpy(& _rects, &rects, sizeof(_rects) );
    rv=SDL_RenderFillRects(_renderer,_rects,count);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1RenderCopy(JNIEnv* env, jclass klass,jlong renderer,jlong texture,jlong srcrect,jlong dstrect){
    int rv;
    SDL_Renderer* _renderer;
    memcpy(& _renderer, &renderer, sizeof(_renderer) );
    SDL_Texture* _texture;
    memcpy(& _texture, &texture, sizeof(_texture) );
     SDL_Rect* _srcrect;
    memcpy(& _srcrect, &srcrect, sizeof(_srcrect) );
     SDL_Rect* _dstrect;
    memcpy(& _dstrect, &dstrect, sizeof(_dstrect) );
    rv=SDL_RenderCopy(_renderer,_texture,_srcrect,_dstrect);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1RenderCopyEx(JNIEnv* env, jclass klass,jlong renderer,jlong texture,jlong srcrect,jlong dstrect,jdouble angle,jlong center,jint flip){
    int rv;
    SDL_Renderer* _renderer;
    memcpy(& _renderer, &renderer, sizeof(_renderer) );
    SDL_Texture* _texture;
    memcpy(& _texture, &texture, sizeof(_texture) );
     SDL_Rect* _srcrect;
    memcpy(& _srcrect, &srcrect, sizeof(_srcrect) );
     SDL_Rect* _dstrect;
    memcpy(& _dstrect, &dstrect, sizeof(_dstrect) );
     SDL_Point* _center;
    memcpy(& _center, &center, sizeof(_center) );
    rv=SDL_RenderCopyEx(_renderer,_texture,_srcrect,_dstrect,angle,_center,flip);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1RenderReadPixels(JNIEnv* env, jclass klass,jlong renderer,jlong rect,jint format,jlong pixels,jint pitch){
    int rv;
    SDL_Renderer* _renderer;
    memcpy(& _renderer, &renderer, sizeof(_renderer) );
     SDL_Rect* _rect;
    memcpy(& _rect, &rect, sizeof(_rect) );
    void* _pixels;
    memcpy(& _pixels, &pixels, sizeof(_pixels) );
    rv=SDL_RenderReadPixels(_renderer,_rect,format,_pixels,pitch);
    return rv;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1RenderPresent(JNIEnv* env, jclass klass,jlong renderer){
    SDL_Renderer* _renderer;
    memcpy(& _renderer, &renderer, sizeof(_renderer) );
    SDL_RenderPresent(_renderer);
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1DestroyTexture(JNIEnv* env, jclass klass,jlong texture){
    SDL_Texture* _texture;
    memcpy(& _texture, &texture, sizeof(_texture) );
    SDL_DestroyTexture(_texture);
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1DestroyRenderer(JNIEnv* env, jclass klass,jlong renderer){
    SDL_Renderer* _renderer;
    memcpy(& _renderer, &renderer, sizeof(_renderer) );
    SDL_DestroyRenderer(_renderer);
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GL_1BindTexture(JNIEnv* env, jclass klass,jlong texture,jfloatArray texw,jfloatArray texh){
    int rv;
    SDL_Texture* _texture;
    memcpy(& _texture, &texture, sizeof(_texture) );
    jfloat* _texw;
    _texw = (*env)->GetFloatArrayElements(env,texw,NULL);
    jfloat* _texh;
    _texh = (*env)->GetFloatArrayElements(env,texh,NULL);
    rv=SDL_GL_BindTexture(_texture,_texw,_texh);
    (*env)->ReleaseFloatArrayElements(env,texw, _texw,0);
    (*env)->ReleaseFloatArrayElements(env,texh, _texh,0);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1GL_1UnbindTexture(JNIEnv* env, jclass klass,jlong texture){
    int rv;
    SDL_Texture* _texture;
    memcpy(& _texture, &texture, sizeof(_texture) );
    rv=SDL_GL_UnbindTexture(_texture);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1Init(JNIEnv* env, jclass klass,jint flags){
    int rv;
    rv=SDL_Init(flags);
    return rv;
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1InitSubSystem(JNIEnv* env, jclass klass,jint flags){
    int rv;
    rv=SDL_InitSubSystem(flags);
    return rv;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1QuitSubSystem(JNIEnv* env, jclass klass,jint flags){
    SDL_QuitSubSystem(flags);
}
JNIEXPORT jint JNICALL Java_JSDL_JSDL_SDL_1WasInit(JNIEnv* env, jclass klass,jint flags){
    Uint32 rv;
    rv=SDL_WasInit(flags);
    return rv;
}
JNIEXPORT void JNICALL Java_JSDL_JSDL_SDL_1Quit(JNIEnv* env, jclass klass){
    SDL_Quit();
}
