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
import static JSDL.JSDL.SDL_GL_SwapWindow;
import framework.math3d.mat4;
import framework.math3d.math3d;
import framework.math3d.vec3;

/**
 *
 * @author jonesk13
 */
public class Swamp {
        
        
        
        
        
        Framebuffer fbo1;
        Framebuffer fbo2;
        double angle=180;
        Texture2D dummytex = new SolidTexture(GL_UNSIGNED_BYTE,0,0,0,0);
        //scene
        Mesh Plane = new Mesh("assets/Main_Level/swamp/water.obj.mesh");
       
       
        Mesh hay = new Mesh("assets/Main_Level/swamp/hayswamp.obj.mesh");
        Mesh brick = new Mesh("assets/Main_Level/swamp/brickswamp.obj.mesh");
        Mesh boat = new Mesh("assets/Main_Level/swamp/boatswamp.obj.mesh");
        Mesh stump = new Mesh("assets/Main_Level/swamp/stump_Swamp.obj.mesh");
        Mesh wood = new Mesh("assets/Main_Level/swamp/woodPileSwamp.obj.mesh");
        Mesh cube = new Mesh("assets/Main_Level/swamp/cube.obj.mesh");
        Mesh Tree1 = new Mesh("assets/Main_Level/swamp/Trees/Tree_Trunk_1.obj.mesh");
        Mesh Tree2 = new Mesh("assets/Main_Level/swamp/Trees/Tree_Trunk_2.obj.mesh");
        Mesh Tree3 = new Mesh("assets/Main_Level/swamp/Trees/Tree_Trunk_3.obj.mesh");
        Mesh Tree4 = new Mesh("assets/Main_Level/swamp/Trees/Tree_Trunk_4.obj.mesh");
        Mesh Tree5 = new Mesh("assets/Main_Level/swamp/Trees/Tree_Trunk_5.obj.mesh");
        Mesh Tree6 = new Mesh("assets/Main_Level/swamp/Trees/Tree_Trunk_6.obj.mesh");
        Mesh Tree7 = new Mesh("assets/Main_Level/swamp/Trees/Tree_Trunk_7.obj.mesh");
        Mesh Tree8 = new Mesh("assets/Main_Level/swamp/Trees/Tree_Trunk_8.obj.mesh");
        Mesh Tree9 = new Mesh("assets/Main_Level/swamp/Trees/Tree_Trunk_9.obj.mesh");
        Mesh Tree10 = new Mesh("assets/Main_Level/swamp/Trees/Tree_Trunk_10.obj.mesh");
        Mesh Tree11 = new Mesh("assets/Main_Level/swamp/Trees/Tree_Trunk_11.obj.mesh");
        Mesh Tree12 = new Mesh("assets/Main_Level/swamp/Trees/Tree_Trunk_12.obj.mesh");
        Mesh Tree13 = new Mesh("assets/Main_Level/swamp/Trees/Tree_Trunk_13.obj.mesh");
        Mesh Tree14 = new Mesh("assets/Main_Level/swamp/Trees/Tree_Trunk_14.obj.mesh");
        Mesh Tree15 = new Mesh("assets/Main_Level/swamp/Trees/Tree_Trunk_15.obj.mesh");
        Mesh Tree16 = new Mesh("assets/Main_Level/swamp/Trees/Tree_Trunk_16.obj.mesh");
        Mesh Tree17 = new Mesh("assets/Main_Level/swamp/Trees/Tree_Trunk_17.obj.mesh");
        
        UnitSquare usq = new UnitSquare();
        String[] tex =new String[6];
        Enemy[] aliens = new Enemy[15];
        ImageCubeTexture skytex;
        Program skyboxShader = new Program("Cubevs.txt","Cubefs.txt");
        Program prog2 = new Program("vs.txt","SwampPfs.txt");
        public Swamp(){
            
            aliens[0] = new Enemy();
            
            tex[0]="assets/Main_Level/swamp/s3px.png";
            tex[1]="assets/Main_Level/swamp/s3nx.png";
            tex[2]="assets/Main_Level/swamp/s3py.png";
            tex[3]="assets/Main_Level/swamp/s3ny.png";
            tex[4]="assets/Main_Level/swamp/s3pz.png";
            tex[5]="assets/Main_Level/swamp/s3nz.png";
            skytex = new ImageCubeTexture(tex);
            fbo1 = new Framebuffer(512,512);
            fbo2 = new Framebuffer(512,512);
        

            
            
          
            
        }
        
        void update(){
            
        }
        
        void draw(long win,Camera cam,Program prog){
            glClearColor(0.0f,0.0f,0.0f,0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT );
            
            prog.use();
            cam.draw(prog);
           
            prog.setUniform("lightPos",new vec3(0,20,0) ); 
                                   
            prog.setUniform("worldMatrix",mat4.identity());
            hay.draw(prog);
            
            brick.draw(prog);        
            boat.draw(prog);
            stump.draw(prog);
            wood.draw(prog);
            Tree1.draw(prog);
            Tree2.draw(prog);
            Tree3.draw(prog);
            Tree4.draw(prog);
            Tree5.draw(prog);
            Tree6.draw(prog);
            Tree7.draw(prog);
            Tree8.draw(prog);
            Tree9.draw(prog);
            Tree10.draw(prog);
            Tree11.draw(prog);
            Tree12.draw(prog);
            Tree13.draw(prog);
            Tree14.draw(prog);
            Tree15.draw(prog);
            Tree16.draw(prog);
            Tree17.draw(prog);
            
            mat4 reflec = new mat4( 1,0,0,0,  
                                     0,-1,0,0,
                                     0,0,1,0,
                                     0,0,0,1);
            prog.setUniform("worldMatrix",mat4.identity());
            
            

            hay.draw(prog);
 
            brick.draw(prog);
            
            boat.draw(prog);
            stump.draw(prog);
            wood.draw(prog);
            
            Tree1.draw(prog);
            Tree2.draw(prog);
            Tree3.draw(prog);
            Tree4.draw(prog);
            Tree5.draw(prog);
            Tree6.draw(prog);
            Tree7.draw(prog);
            Tree8.draw(prog);
            Tree9.draw(prog);
            Tree10.draw(prog);
            Tree11.draw(prog);
            Tree12.draw(prog);
            Tree13.draw(prog);
            Tree14.draw(prog);
            Tree15.draw(prog);
            Tree16.draw(prog);
            Tree17.draw(prog);
            //System.out.print(Plane.centroid);
            
            prog2.use();
            prog2.setUniform("lightPos",new vec3(0,50,0) ); 
            prog2.setUniform("viewMatrix",mat4.identity());
            prog2.setUniform("worldMatrix",mat4.identity());
            cam.draw(prog2);
            Plane.draw(prog2);
            
//            skyboxShader.use();
//            skyboxShader.setUniform("samplerCube", skytex);
//            //skyboxShader.setUniform("v_worldPos", cube.centroid);
//            mat4 view = new mat4();
//            view=cam.projMatrix.mul(cam.viewMatrix);
//            skyboxShader.setUniform("viewProjMatrix",view);    
//            cam.draw(skyboxShader);       
//            cube.draw(skyboxShader);
            
           
        
    }
    void drawSKY(long win,Camera cam,Program prog)
    {
        skyboxShader.use();
        skyboxShader.setUniform("samplerCube", skytex);
        //skyboxShader.setUniform("v_worldPos", cube.centroid);
        mat4 view = new mat4();
        view=cam.projMatrix.mul(cam.viewMatrix);
        skyboxShader.setUniform("viewProjMatrix",view);    
        cam.draw(skyboxShader);       
        cube.draw(skyboxShader);
    }
}
