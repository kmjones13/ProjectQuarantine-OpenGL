/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package framework;

import static JGL.JGL.GL_COLOR_BUFFER_BIT;
import static JGL.JGL.GL_DEPTH_BUFFER_BIT;
import static JGL.JGL.GL_STENCIL_BUFFER_BIT;
import static JGL.JGL.GL_UNSIGNED_BYTE;
import static JGL.JGL.glClear;
import static JGL.JGL.glClearColor;
import framework.math3d.mat4;
import framework.math3d.vec3;

/**
 *
 * @author jonesk13
 * 
 */

public class Tunnels {
    
    
    
    Framebuffer fbo1;
    Framebuffer fbo2;
    double angle=180;
    Texture2D dummytex = new SolidTexture(GL_UNSIGNED_BYTE,0,0,0,0);
    //scene
    Mesh BrickSwer = new Mesh("assets/Main_Level/tunnels/swers.obj.mesh");   
   
    
    
    public Tunnels(){
        
    }
    
    void update(){
            
    }
        
    void draw(long win,Camera cam,Program prog){
        glClearColor(0.0f,0.0f,0.0f,0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT );
            
        prog.use();
        cam.draw(prog);
           
        prog.setUniform("lightPos",new vec3(0,5,0) ); 
                                   
        prog.setUniform("worldMatrix",mat4.identity());
        BrickSwer.draw(prog);
    }
}
