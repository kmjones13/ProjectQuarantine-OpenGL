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
 **/
public class City {
    Framebuffer fbo1;
    Framebuffer fbo2;
    double angle=180;
    Texture2D dummytex = new SolidTexture(GL_UNSIGNED_BYTE,0,0,0,0);
    //sky
    Mesh sky = new Mesh("assets/Main_Level/City_Land/scene/misc/sky.obj.mesh");
    //
    Mesh Bboard1 = new Mesh("assets/Main_Level/City_Land/scene/misc/billboard1.obj.mesh");
    Mesh Bboard2 = new Mesh("assets/Main_Level/City_Land/scene/misc/billboard2.obj.mesh");
    Mesh blocker1 = new Mesh("assets/Main_Level/City_Land/scene/misc/blocker1.obj.mesh");
    Mesh blocker2 = new Mesh("assets/Main_Level/City_Land/scene/misc/blocker2.obj.mesh");
    Mesh blocker3 = new Mesh("assets/Main_Level/City_Land/scene/misc/blocker3.obj.mesh");
    Mesh blocker4 = new Mesh("assets/Main_Level/City_Land/scene/misc/blocker4.obj.mesh");
    Mesh blocker5 = new Mesh("assets/Main_Level/City_Land/scene/misc/blocker5.obj.mesh");
    
//    Mesh cone1 = new Mesh("assets/Main_Level/City_Land/scene/misc/cone1.obj.mesh");
//    Mesh cone2 = new Mesh("assets/Main_Level/City_Land/scene/misc/cone2.obj.mesh");
//    Mesh cone3 = new Mesh("assets/Main_Level/City_Land/scene/misc/cone3.obj.mesh");
//    Mesh cone4 = new Mesh("assets/Main_Level/City_Land/scene/misc/cone4.obj.mesh");
//    Mesh cone5 = new Mesh("assets/Main_Level/City_Land/scene/misc/cone5.obj.mesh");
//    Mesh cone6 = new Mesh("assets/Main_Level/City_Land/scene/misc/cone6.obj.mesh");
    
    Mesh fire1 = new Mesh("assets/Main_Level/City_Land/scene/misc/fire1.obj.mesh");
    Mesh fire2 = new Mesh("assets/Main_Level/City_Land/scene/misc/fire1.obj.mesh");
    Mesh fire3 = new Mesh("assets/Main_Level/City_Land/scene/misc/fire3.obj.mesh");
    Mesh fire4 = new Mesh("assets/Main_Level/City_Land/scene/misc/fire4.obj.mesh");
    
    Mesh ground = new Mesh("assets/Main_Level/City_Land/scene/misc/ground.obj.mesh");
    
    Mesh water1 = new Mesh("assets/Main_Level/City_Land/scene/misc/WaterTower1.obj.mesh");
    Mesh water2 = new Mesh("assets/Main_Level/City_Land/scene/misc/WaterTower2.obj.mesh");
    
    Mesh wall = new Mesh("assets/Main_Level/City_Land/scene/BrickWall/Wall.obj.mesh");
    
    Mesh House1 = new Mesh("assets/Main_Level/City_Land/scene/Buildings/House.obj.mesh");
    Mesh House2 = new Mesh("assets/Main_Level/City_Land/scene/Buildings/House1.obj.mesh");
    Mesh House3 = new Mesh("assets/Main_Level/City_Land/scene/Buildings/House2.obj.mesh");
    Mesh House4 = new Mesh("assets/Main_Level/City_Land/scene/Buildings/House3.obj.mesh");
    Mesh House5 = new Mesh("assets/Main_Level/City_Land/scene/Buildings/House4.obj.mesh");
    Mesh House6 = new Mesh("assets/Main_Level/City_Land/scene/Buildings/House5.obj.mesh");
    Mesh House7 = new Mesh("assets/Main_Level/City_Land/scene/Buildings/House6.obj.mesh");
    Mesh House8 = new Mesh("assets/Main_Level/City_Land/scene/Buildings/House7.obj.mesh");
    Mesh House9 = new Mesh("assets/Main_Level/City_Land/scene/Buildings/House8.obj.mesh");
    Mesh House10 = new Mesh("assets/Main_Level/City_Land/scene/Buildings/House9.obj.mesh");
  
    
    Mesh Tower1= new Mesh("assets/Main_Level/City_Land/scene/Buildings/Skyscraper1.obj.mesh");
    Mesh Tower2= new Mesh("assets/Main_Level/City_Land/scene/Buildings/Skyscraper2.obj.mesh");
    Mesh Tower3= new Mesh("assets/Main_Level/City_Land/scene/Buildings/Skyscraper3.obj.mesh");
    Mesh Tower4= new Mesh("assets/Main_Level/City_Land/scene/Buildings/Skyscraper4.obj.mesh");
    Mesh Tower5= new Mesh("assets/Main_Level/City_Land/scene/Buildings/Skyscraper5.obj.mesh");
    Mesh Tower6= new Mesh("assets/Main_Level/City_Land/scene/Buildings/Skyscraper6.obj.mesh");
    Mesh Tower7= new Mesh("assets/Main_Level/City_Land/scene/Buildings/Skyscraper7.obj.mesh");
    Mesh Tower8= new Mesh("assets/Main_Level/City_Land/scene/Buildings/Skyscraper8.obj.mesh");
    Mesh Tower9= new Mesh("assets/Main_Level/City_Land/scene/Buildings/Skyscraper9.obj.mesh");
    Mesh Tower10= new Mesh("assets/Main_Level/City_Land/scene/Buildings/Skyscraper10.obj.mesh");
   
    
    Mesh store1= new Mesh("assets/Main_Level/City_Land/scene/Buildings/store1.obj.mesh");
    Mesh store2= new Mesh("assets/Main_Level/City_Land/scene/Buildings/store2.obj.mesh");
    Mesh store3= new Mesh("assets/Main_Level/City_Land/scene/Buildings/store3.obj.mesh");
    
    String[] tex =new String[6];
    
    ImageCubeTexture skytex;
    Program skyboxShader = new Program("Cubevs.txt","Cubefs.txt");
    public City(){
        
            tex[0]="assets/Main_Level/City_Land/s3px.png";
            tex[1]="assets/Main_Level/City_Land/s3nx.png";
            tex[2]="assets/Main_Level/City_Land/s3py.png";
            tex[3]="assets/Main_Level/City_Land/s3ny.png";
            tex[4]="assets/Main_Level/City_Land/s3pz.png";
            tex[5]="assets/Main_Level/City_Land/s3nz.png";
            skytex = new ImageCubeTexture(tex);
            fbo1 = new Framebuffer(512,512);
            fbo2 = new Framebuffer(512,512);
        
    } 
    
    void update(Program prog,float dt,vec3 target){
       
    }
    
    void draw(long win,Camera cam,Program prog){
        glClearColor(0.0f,0.0f,0.0f,0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT );
            
        prog.use();
        cam.draw(prog);
           
        prog.setUniform("lightPos",new vec3(0,20,0) ); 
                                   
        prog.setUniform("worldMatrix",mat4.identity());
        
        Bboard1.draw(prog);
        Bboard2.draw(prog);
        blocker1.draw(prog);
        blocker2.draw(prog);
        blocker3.draw(prog);
        blocker4.draw(prog);
        blocker5.draw(prog);
//        cone1.draw(prog);
//        cone2.draw(prog);
//        cone3.draw(prog);
//        cone4.draw(prog);
//        cone5.draw(prog);
//        cone6.draw(prog);
    
        fire1.draw(prog);
        fire2.draw(prog);
        fire3.draw(prog);
        fire4.draw(prog);
        
    
        water1.draw(prog);
        water2.draw(prog);
    
        wall.draw(prog);
    
        House1.draw(prog);
        House2.draw(prog);
        House3.draw(prog);
        House4.draw(prog);
        House5.draw(prog);
        House6.draw(prog);
        House7.draw(prog);
        House8.draw(prog);
        House9.draw(prog);
        House10.draw(prog);
       
    
        Tower1.draw(prog);
        Tower2.draw(prog);
        Tower3.draw(prog);
        Tower4.draw(prog);
        Tower5.draw(prog);
        Tower6.draw(prog);
        Tower7.draw(prog);
        Tower8.draw(prog);
        Tower9.draw(prog);
        Tower10.draw(prog);
      
    
        store1.draw(prog);
        store2.draw(prog);
        store3.draw(prog);
        
        
        ground.draw(prog); 
        
        
        skyboxShader.use();
        skyboxShader.setUniform("samplerCube", skytex);
        //skyboxShader.setUniform("v_worldPos", cube.centroid);
        mat4 view = new mat4();
        view=cam.projMatrix.mul(cam.viewMatrix);
        skyboxShader.setUniform("viewProjMatrix",view);    
        cam.draw(skyboxShader);    
        
        sky.draw(skyboxShader);
        
        
    }
    
}
