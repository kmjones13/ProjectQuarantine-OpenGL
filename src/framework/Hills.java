/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package framework;

import static JGL.JGL.GL_ARRAY_BUFFER;
import static JGL.JGL.GL_COLOR_BUFFER_BIT;
import static JGL.JGL.GL_DEPTH_BUFFER_BIT;
import static JGL.JGL.GL_FLOAT;
import static JGL.JGL.GL_STATIC_DRAW;
import static JGL.JGL.GL_STENCIL_BUFFER_BIT;
import static JGL.JGL.GL_TRIANGLES;
import static JGL.JGL.GL_UNSIGNED_BYTE;
import static JGL.JGL.glBindVertexArray;
import static JGL.JGL.glClear;
import static JGL.JGL.glClearColor;
import static JGL.JGL.glEnableVertexAttribArray;
import static JGL.JGL.glGenBuffers;
import static JGL.JGL.glGenVertexArrays;
import static JGL.JGL.glVertexAttribPointer;
import framework.math3d.mat4;
import framework.math3d.vec3;
import java.util.Random;

/**
 *
 * @author jonesk13
 */
public class Hills {
    
    Framebuffer fbo1;
    Framebuffer fbo2;
    double angle=180;
    Texture2D dummytex = new SolidTexture(GL_UNSIGNED_BYTE,0,0,0,0);
    //sky
    Mesh sky = new Mesh("assets/Main_Level/City_Land/scene/misc/sky.obj.mesh");
    //scene
    Mesh Plane = new Mesh("assets/Main_Level/Hill_Land/Level_Hill.obj.mesh");
    
    String[] tex =new String[6];
    
    ImageCubeTexture skytex;
    Program skyboxShader = new Program("Cubevs.txt","Cubefs.txt");
   // Program MapProg = new Program("hmapvs.txt","hmapfs.txt");
    ColorTexture htex;
    private int vao;
    
    public Hills(){
        tex[0]="assets/Main_Level/Hill_Land/s3px.png";
        tex[1]="assets/Main_Level/Hill_Land/s3nx.png";
        tex[2]="assets/Main_Level/Hill_Land/s3py.png";
        tex[3]="assets/Main_Level/Hill_Land/s3ny.png";
        tex[4]="assets/Main_Level/Hill_Land/s3pz.png";
        tex[5]="assets/Main_Level/Hill_Land/s3nz.png";
        skytex = new ImageCubeTexture(tex);
        fbo1 = new Framebuffer(512,512);
        fbo2 = new Framebuffer(512,512);
                  
        
        
    } 
    void drawSKY(long win,Camera cam,Program prog){
        skyboxShader.use();
        skyboxShader.setUniform("samplerCube", skytex);
        //skyboxShader.setUniform("v_worldPos", cube.centroid);
        mat4 view = new mat4();
        view=cam.projMatrix.mul(cam.viewMatrix);
        skyboxShader.setUniform("viewProjMatrix",view);    
        cam.draw(skyboxShader);    

        sky.draw(skyboxShader);
    }
    
    void update(Program prog,float dt,vec3 target){
       
    }
    void HeightMap()
    {
        //how to load file and right texture
//        ColorTexture htex;
//        
//        numtris = (htex.w-1)*(htex.h-1)*2;
//        float[] vdata = new float[numtris*3*2];
//        int k=0;
//        for(int ii=0;ii<htex.w-1;++ii){
//        for(int jj=0;jj<htex.h-1;++jj){
//            //vertex format: x,y as integers
//            vdata[k++] = ii; vdata[k++] = jj;
//            vdata[k++] = ii; vdata[k++] = jj+1;
//            vdata[k++] = ii+1; vdata[k++] = jj;
//            vdata[k++] = ii+1; vdata[k++] = jj;
//            vdata[k++] = ii; vdata[k++] = jj+1;
//            vdata[k++] = ii+1; vdata[k++] = jj+1;
//        int[] tmp = new int[1];
//        glGenVertexArrays(1, tmp);
//        this.vao = tmp[0];
//        glBindVertexArray(vao);
//        glGenBuffers(1, tmp);
//        this.vbuff = tmp[0];
//        glBindBuffer(GL_ARRAY_BUFFER,this.vbuff);
//        glBufferData(GL_ARRAY_BUFFER,vdata.length*4,Main.floatsToBytes(vdata),GL_STATIC_DRAW);
//        glEnableVertexAttribArray(Program.POSITION_INDEX);
//        glVertexAttribPointer(Program.POSITION_INDEX, 2, GL_FLOAT, false, 2*4,0);
//        glBindVertexArray(0);
    }

    
    
    void draw(long win,Camera cam,Program prog){
        glClearColor(0.0f,0.0f,0.0f,0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT );
            
        prog.use();
        cam.draw(prog);
           
        prog.setUniform("lightPos",new vec3(0,50,0) ); 
                                   
        prog.setUniform("worldMatrix",mat4.identity());
        Plane.draw(prog);
        
        //MapProg.use();
        //cam.draw(prog);
        //prog.setUniform("lightPos",new vec3(0,50,0) ); 
        //glBindVertexArray(vao);
        
        
    }
    
   
    
}
