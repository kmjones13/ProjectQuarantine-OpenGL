import os
import sys
import time
from ctypes import *
import array
import random

#GL imports
import OpenGL
OpenGL.ERROR_LOGGING=True
OpenGL.ERROR_ON_COPY=True
OpenGL.ERROR_CHECKING=True
OpenGL.STORE_POINTERS=True
OpenGL.FORWARD_COMPATIBLE_ONLY=True
from OpenGL.GL import *


import init
from wintools import *
from Program import  Program
from math3d import *
from Tetrahedron import Tetrahedron
from Camera import Camera
import Texture 
from Bullet import Bullet
from Gun import Gun
import Globals
from Mesh import Mesh
from Sphere import Sphere 

def update_all(L,elapsed):
    i=0
    while i < len(L):
        L[i].update(elapsed)
        if L[i].is_dead():
            if i == len(L)-1:
                L.pop()
            else:
                L[i]=L.pop()
        else:
            i+=1


def main():
          
    
    win,dc = init.initGL()

    red = Texture.Texture2D(filename="red.png")
    heart = Texture.Texture2D(filename="heart.png")
    gameovertex = Texture.Texture2D(filename="gameover.png")
    
    glEnable(GL_DEPTH_TEST)
    glDepthFunc(GL_LEQUAL)
    
    glEnable(GL_BLEND)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
    
    keys = set()
    
    prog = Program("vs.txt","fs.txt")
    prog.use()
    
    cam = Camera(eye=vec4(0,0,20,1))
    tetras = []
    bullets = Globals.bullets
    
    #create the ground grid
    for i in range(-20,20,10):
        for j in range(-20,20,10):
            if random.randrange(2):
                tetras.append( Tetrahedron(vec3(i,0,j)) )
            else:
                tetras.append( Gun( vec3(i,0,j) ) )
     
    #unit square. Ah, so I didn't put it in a separate file. Blah.
    usq = Mesh( vdata= array.array('f',
        [ 
            -1,-1,-1,  0,0,  0,0,0,
            -1,1,-1,   0,1,  0,0,0,
            1,1,-1,  1,1, 0,0,0,
            1,-1,-1, 1,0, 0,0,0
        ]),
        idata = array.array( 'H',
            [0,3,2, 0,2,1]
        )
    )
    
    ground = Mesh(filename="assets/ground256.obj.mesh")
    
    glClearColor(0,0,0,1) 
    glClearColor(0.2,0.4,0.6,1.0)
    
    
    prev=time.time()
    last_bullet_time = 0
    
    thrust = 1.5
    boomalpha = 0
    lives=5
    gameover=False
    tiltamount=0


    
    prog.setUniform("lights[0].pos",normalize(vec3(1,1,1)))
    prog.setUniform("lights[0].color",vec3(1,1,1))
    prog.setUniform("lights[0].positional",0.0)
    prog.setUniform("lights[0].A0",1.0)
    prog.setUniform("lights[0].A1",0.0)
    prog.setUniform("lights[0].A2",0.0)
    
    prog.setUniform("lights[1].color",vec3(0.0,0,0))
    prog.setUniform("lights[1].positional",1.0)
    prog.setUniform("lights[1].A0",1.0)
    prog.setUniform("lights[1].A1",0.0)
    prog.setUniform("lights[1].A2",0.0)



    while True:
        ec=0
        while True:
            ev = PollEvent()
            if not ev:
                break
            ec+=1    
            if ev.message == WM_KEYDOWN:
                keys.add(ev.wParam)
            elif ev.message == WM_KEYUP:
                keys.discard(ev.wParam)
                if ev.wParam == ord("Q"):
                    print("Exit")
                    sys.exit(0)
                elif ev.wParam == ord("Z"):
                    print("U=",cam.U)
                    print("V=",cam.V)
                    print("W=",cam.W)
                #else:
                    #print("Key:",ev.wParam)
            else:
                DispatchMessage(byref(ev))
         
        now=time.time()
        elapsed = now-prev
        #elapsed *= 0.25
        prev=now
        
        if not gameover:
                
            if 87 in keys: #W
                cam.pitch(-0.4*elapsed)
            if 83 in keys: #S
                cam.pitch(0.4*elapsed)
            if 65 in keys: #A
                cam.tilt(0.4*elapsed)
            if 68 in keys: #D
                cam.tilt(-0.4*elapsed)
            if 32 in keys and last_bullet_time < now-0.5:
                bullets.append( Bullet( cam.eye,  -10*cam.W, 2.0 , None) )
                last_bullet_time=now
            if 79 in keys: #O
                thrust += 0.125
            if 76 in keys: #L
                thrust -= 0.125
            if 48 in keys: #0
                print("Game over!")
                gameover=True
            
            #use the amount of banking to
            #control the turning
            cam.axisTurn( vec3(0,1,0), 2*cam.U.y*elapsed )

                            
        update_all(tetras,elapsed)
        update_all(bullets,elapsed)

        pbullets=[]
        
        if not gameover:
            player_bounding_sphere = Sphere( cam.eye, 1 )
            
            #collision detection
            i=0
            while i < len(tetras):
                if tetras[i].state == 0 and player_bounding_sphere.overlaps(tetras[i].bounding_sphere()):
                    tetras[i].destroy()
                    lives-=1
                    boomalpha = 0.5
                i+=1
            
            i=0
            while i < len(bullets):
                b = bullets[i]
                j=0
                hadcollision=False
                
                if b.shooter != None:
                    if b.bounding_sphere().overlaps(player_bounding_sphere):
                        print("AARGH!")
                        boomalpha=0.5
                        if i == len(bullets)-1:
                            bullets.pop()
                        else:
                            bullets[i] = bullets.pop()
                        lives-=1
                        hadcollision=True
                    
                while not hadcollision and j < len(tetras):
                    t = tetras[j]
                    if b.shooter != t:
                        if b.bounding_sphere().overlaps(t.bounding_sphere()):
                            print("A HIT!")
                            if i == len(bullets)-1:
                                bullets.pop()
                            else:
                                bullets[i] = bullets.pop()
                            tetras[j].destroy()
                            hadcollision=True
                        else:
                            j+=1
                    else:
                        j+=1
                #end while j
                
                if not hadcollision:
                    if b.shooter == None:
                        pbullets.append(b)
                        
                    i+=1
            #end while i
            
            if lives<=0:
                gameover=True
                
            cam.walk(thrust*elapsed)
        #endif not gameover
        
        
        #we DO want this to happen even if we're in "game over"
        boomalpha -= 0.5*elapsed
        if boomalpha < 0:
            boomalpha = 0
            
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

        cam.draw(prog)

        

        prog.setUniform("lights[1].pos",cam.eye.xyz)
        
        prog.setUniform("alpha",1.0)
        prog.setUniform("unlit",0.0)
        prog.setUniform("billboard",0)
        
        prog.setUniform("worldMatrix",mat4.identity())
        ground.draw(prog)
        
        for t in tetras:
            t.draw(prog)
            
            
        prog.setUniform("unlit",1)
        prog.setUniform("billboard",1)
        for t in bullets:
            t.draw(prog)
        
        #draw the unit square overlay
        #unlit should still be 1.
        prog.setUniform("billboard",0)
        I=mat4.identity()
        prog.setUniform("worldMatrix",I)
        prog.setUniform("viewMatrix",I)
        prog.setUniform("projMatrix",I)
        prog.setUniform("texture",red)
        prog.setUniform("alpha",boomalpha)
        
        #FIXME
        usq.draw(prog)

        #unlit is still 1
        prog.setUniform("texture",heart)
        prog.setUniform("alpha",1.0)
        
        S=scaling(vec3(0.05,0.05,1))
        for i in range(lives):
            T=translation(vec3(-0.9,0.9-i*0.1,0))
            prog.setUniform("worldMatrix",S*T)
            usq.draw(prog)
        
        if gameover:
            prog.setUniform("worldMatrix",scaling(vec3(0.8,0.4,1)))
            prog.setUniform("texture",gameovertex)
            usq.draw(prog)
            
            
        SwapBuffers(dc)
        #time.sleep(0.01)
              
                

main()
