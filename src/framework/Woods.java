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
import framework.math3d.vec4;

/**
 *
 * @author jonesk13
 */
public class Woods {
    Framebuffer fbo1;
    Framebuffer fbo2;
    double angle=180;
    Texture2D dummytex = new SolidTexture(GL_UNSIGNED_BYTE,0,0,0,0);
    //sky
    Mesh sky = new Mesh("assets/Main_Level/City_Land/scene/misc/sky.obj.mesh");
    //ground
    Mesh Ground = new Mesh("assets/Main_Level/Flat_Land/misc/ground.obj.mesh");
    //trees
    Mesh Tree1 = new Mesh("assets/Main_Level/Flat_Land/misc/tree1.obj.mesh");
    Mesh Tree2 = new Mesh("assets/Main_Level/Flat_Land/misc/tree2.obj.mesh");
    Mesh Tree3 = new Mesh("assets/Main_Level/Flat_Land/misc/tree3.obj.mesh");
    Mesh Tree4 = new Mesh("assets/Main_Level/Flat_Land/misc/tree4.obj.mesh");
    Mesh Tree5 = new Mesh("assets/Main_Level/Flat_Land/misc/tree5.obj.mesh");
    Mesh Tree6 = new Mesh("assets/Main_Level/Flat_Land/misc/tree6.obj.mesh");
    Mesh Tree7 = new Mesh("assets/Main_Level/Flat_Land/misc/tree7.obj.mesh");
    Mesh Tree8 = new Mesh("assets/Main_Level/Flat_Land/misc/tree8.obj.mesh");
    Mesh Tree9 = new Mesh("assets/Main_Level/Flat_Land/misc/tree9.obj.mesh");
    Mesh Tree10 = new Mesh("assets/Main_Level/Flat_Land/misc/tree10.obj.mesh");
    Mesh Tree11 = new Mesh("assets/Main_Level/Flat_Land/misc/tree11.obj.mesh");
    Mesh Tree12 = new Mesh("assets/Main_Level/Flat_Land/misc/tree12.obj.mesh");
    Mesh Tree13 = new Mesh("assets/Main_Level/Flat_Land/misc/tree13.obj.mesh");
    Mesh Tree14 = new Mesh("assets/Main_Level/Flat_Land/misc/tree14.obj.mesh");
    Mesh Tree15 = new Mesh("assets/Main_Level/Flat_Land/misc/tree15.obj.mesh");
    Mesh Tree16 = new Mesh("assets/Main_Level/Flat_Land/misc/tree16.obj.mesh");
    Mesh Tree17 = new Mesh("assets/Main_Level/Flat_Land/misc/tree17.obj.mesh");
    Mesh Tree18 = new Mesh("assets/Main_Level/Flat_Land/misc/tree18.obj.mesh");
    Mesh Tree19 = new Mesh("assets/Main_Level/Flat_Land/misc/tree19.obj.mesh");
    Mesh Tree20 = new Mesh("assets/Main_Level/Flat_Land/misc/tree20.obj.mesh");
    Mesh Tree21 = new Mesh("assets/Main_Level/Flat_Land/misc/tree21.obj.mesh");
    Mesh Tree22 = new Mesh("assets/Main_Level/Flat_Land/misc/tree22.obj.mesh");
    Mesh Tree23 = new Mesh("assets/Main_Level/Flat_Land/misc/tree23.obj.mesh");
    Mesh Tree24 = new Mesh("assets/Main_Level/Flat_Land/misc/tree24.obj.mesh");
    Mesh Tree25 = new Mesh("assets/Main_Level/Flat_Land/misc/tree25.obj.mesh");
    Mesh Tree26 = new Mesh("assets/Main_Level/Flat_Land/misc/tree26.obj.mesh");
    Mesh Tree27 = new Mesh("assets/Main_Level/Flat_Land/misc/tree28.obj.mesh");
    Mesh Tree28 = new Mesh("assets/Main_Level/Flat_Land/misc/tree28.obj.mesh");
    Mesh Tree29 = new Mesh("assets/Main_Level/Flat_Land/misc/tree29.obj.mesh");
    Mesh Tree30 = new Mesh("assets/Main_Level/Flat_Land/misc/tree30.obj.mesh");
    Mesh Tree31 = new Mesh("assets/Main_Level/Flat_Land/misc/tree31.obj.mesh");
    Mesh Tree32 = new Mesh("assets/Main_Level/Flat_Land/misc/tree32.obj.mesh");
    Mesh Tree33 = new Mesh("assets/Main_Level/Flat_Land/misc/tree33.obj.mesh");
    Mesh Tree34 = new Mesh("assets/Main_Level/Flat_Land/misc/tree34.obj.mesh");
    Mesh Tree35 = new Mesh("assets/Main_Level/Flat_Land/misc/tree35.obj.mesh");
    Mesh Tree36 = new Mesh("assets/Main_Level/Flat_Land/misc/tree36.obj.mesh");
    Mesh Tree37 = new Mesh("assets/Main_Level/Flat_Land/misc/tree37.obj.mesh");
    Mesh Tree38 = new Mesh("assets/Main_Level/Flat_Land/misc/tree38.obj.mesh");
    Mesh Tree39 = new Mesh("assets/Main_Level/Flat_Land/misc/tree39.obj.mesh");
    Mesh Tree40 = new Mesh("assets/Main_Level/Flat_Land/misc/tree40.obj.mesh");
    Mesh Tree41 = new Mesh("assets/Main_Level/Flat_Land/misc/tree41.obj.mesh");
    //barrels of water
    Mesh Water1 = new Mesh("assets/Main_Level/Flat_Land/misc/water1.obj.mesh");
    Mesh Water2 = new Mesh("assets/Main_Level/Flat_Land/misc/water2.obj.mesh");
    Mesh Water3 = new Mesh("assets/Main_Level/Flat_Land/misc/water3.obj.mesh");
    Mesh Water4 = new Mesh("assets/Main_Level/Flat_Land/misc/water4.obj.mesh");
    Mesh Water5 = new Mesh("assets/Main_Level/Flat_Land/misc/water5.obj.mesh");
    Mesh Water6 = new Mesh("assets/Main_Level/Flat_Land/misc/water6.obj.mesh");
    Mesh Water7 = new Mesh("assets/Main_Level/Flat_Land/misc/water7.obj.mesh");
    Mesh Water8 = new Mesh("assets/Main_Level/Flat_Land/misc/water8.obj.mesh");
    Mesh Water9 = new Mesh("assets/Main_Level/Flat_Land/misc/water9.obj.mesh");
    //rocks
    Mesh Rock1 = new Mesh("assets/Main_Level/Flat_Land/misc/rock1.obj.mesh");
    Mesh Rock2 = new Mesh("assets/Main_Level/Flat_Land/misc/rock2.obj.mesh");
    Mesh Rock3 = new Mesh("assets/Main_Level/Flat_Land/misc/rock3.obj.mesh");
    Mesh Rock4 = new Mesh("assets/Main_Level/Flat_Land/misc/rock4.obj.mesh");
    Mesh Rock5 = new Mesh("assets/Main_Level/Flat_Land/misc/rock5.obj.mesh");
    Mesh Rock6 = new Mesh("assets/Main_Level/Flat_Land/misc/rock6.obj.mesh");
    Mesh Rock7 = new Mesh("assets/Main_Level/Flat_Land/misc/rock7.obj.mesh");
    Mesh Rock8 = new Mesh("assets/Main_Level/Flat_Land/misc/rock8.obj.mesh");
    Mesh Rock9 = new Mesh("assets/Main_Level/Flat_Land/misc/roc65.obj.mesh");
    //red barrels
    Mesh Barrel1 = new Mesh("assets/Main_Level/Flat_Land/misc/barrel1.obj.mesh");
    Mesh Barrel2 = new Mesh("assets/Main_Level/Flat_Land/misc/barrel2.obj.mesh");
    Mesh Barrel3 = new Mesh("assets/Main_Level/Flat_Land/misc/barrel3.obj.mesh");
    Mesh Barrel4 = new Mesh("assets/Main_Level/Flat_Land/misc/barrel4.obj.mesh");
    Mesh Barrel5 = new Mesh("assets/Main_Level/Flat_Land/misc/barrel5.obj.mesh");
    Mesh Barrel6 = new Mesh("assets/Main_Level/Flat_Land/misc/barrel6.obj.mesh");
    Mesh Barrel7 = new Mesh("assets/Main_Level/Flat_Land/misc/barrel7.obj.mesh");
    Mesh Barrel8 = new Mesh("assets/Main_Level/Flat_Land/misc/barrel8.obj.mesh");
    Mesh Barrel9 = new Mesh("assets/Main_Level/Flat_Land/misc/barrel9.obj.mesh");
    Program prog2 = new Program("vs2.txt","fs2.txt");
    Program prog3 = new Program("vs.txt","fs.txt");
    Camera cam2 = new Camera();
    
    
    String[] tex =new String[6];
    
    ImageCubeTexture skytex;
    Program skyboxShader = new Program("Cubevs.txt","Cubefs.txt");
    public Woods(){
        tex[0]="assets/Main_Level/Flat_Land/s3px.png";
        tex[1]="assets/Main_Level/Flat_Land/s3nx.png";
        tex[2]="assets/Main_Level/Flat_Land/s3py.png";
        tex[3]="assets/Main_Level/Flat_Land/s3ny.png";
        tex[4]="assets/Main_Level/Flat_Land/s3pz.png";
        tex[5]="assets/Main_Level/Flat_Land/s3nz.png";
            skytex = new ImageCubeTexture(tex);
            
        fbo1 = new Framebuffer(512,512);
        fbo2 = new Framebuffer(512,512);
        cam2.eye=new vec4(0,70,0,1);
        
    } 
    
    void update(Program prog,float dt,vec3 target){
       
    }
    void drawSKY(Camera cam,Program prog){
        skyboxShader.use();
        skyboxShader.setUniform("samplerCube", skytex);
        //skyboxShader.setUniform("v_worldPos", cube.centroid);
        mat4 view = new mat4();
        view=cam.projMatrix.mul(cam.viewMatrix);
        skyboxShader.setUniform("viewProjMatrix",view);    
        cam.draw(skyboxShader);    
        
        sky.draw(skyboxShader);
    }
    void draw(Camera cam,Program prog){
     
        drawObj(prog);
        
       // prog.use();
       // cam.draw(prog);  
       // prog.setUniform("lightPos",new vec3(0,50,0) ); 
       // prog.setUniform("viewMatrix",mat4.identity());
       // prog.setUniform("worldMatrix",mat4.identity());
        
        
    }
    void drawObj(Program prog)
    {
        //trees
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
        Tree18.draw(prog);
        Tree19.draw(prog);
        Tree20.draw(prog);  
        Tree21.draw(prog);
        Tree22.draw(prog);
        Tree23.draw(prog);
        Tree24.draw(prog);
        Tree25.draw(prog);
        Tree26.draw(prog);
        Tree27.draw(prog);
        Tree28.draw(prog);
        Tree29.draw(prog);
        Tree30.draw(prog);
        Tree31.draw(prog);
        Tree32.draw(prog);
        Tree33.draw(prog);
        Tree34.draw(prog);
        Tree35.draw(prog);
        Tree36.draw(prog);
        Tree37.draw(prog);
        Tree38.draw(prog);
        Tree39.draw(prog);
        Tree40.draw(prog);
        Tree41.draw(prog);
        //barrels
        Barrel1.draw(prog);
        Barrel2.draw(prog);
        Barrel3.draw(prog);
        Barrel4.draw(prog);
        Barrel5.draw(prog);
        Barrel6.draw(prog);
        Barrel7.draw(prog);
        Barrel8.draw(prog);
        Barrel9.draw(prog);
        //water
        Water1.draw(prog);
        Water2.draw(prog);
        Water3.draw(prog);
        Water4.draw(prog);
        Water5.draw(prog);
        Water6.draw(prog);
        Water7.draw(prog);
        Water8.draw(prog);
        Water9.draw(prog);
        //rocks
        Rock1.draw(prog);
        Rock2.draw(prog);
        Rock3.draw(prog);
        Rock4.draw(prog);
        Rock5.draw(prog);
        Rock6.draw(prog);
        Rock7.draw(prog);
        Rock8.draw(prog);
        Rock9.draw(prog);
        //ground
        Ground.draw(prog);
        
    }
    
}
