package framework;

import JGL.JGL;
import static JGL.JGL.*;
import framework.math3d.ivec2;
import framework.math3d.mat4;
import framework.math3d.vec2;
import framework.math3d.vec3;
import framework.math3d.vec4;
import framework.math3d.vec5;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.util.ArrayList;

/**
 *
 * @author jhudson
 */
public class IWater {
    int resolution;
    static Program2 updateprog;
    static Program drawprog;
    static Mesh usq;
    float F,D,q,t,d,size;  //documented below
    GPUBuffer[] buffers;
    int pidx=0;       //index of 'prev' buffer
    float accum_elapsed=0.0f;
    int numtris;
    int vao;
    int vbuff;
    mat4 worldMatrix;
    vec2 sz,sz2;
    
    public IWater(int resolution, float size){
        if( updateprog == null ){
            updateprog = new Program2("iwater_updatecs.txt");
            drawprog = new Program("iwater_drawvs.txt","iwater_drawfs.txt");
            usq = new Mesh("assets/usq.obj.mesh");
        }
    
        this.worldMatrix = mat4.identity();
        this.F=1;   //force (tension)
        this.D=1;   //density
        this.q=1;   //viscosity
        this.t=0.01f;    //time between evals
        this.d=0.01f;    //distance between grid points for evaluation phase
        this.resolution=resolution; //resolution of grid
        this.size=size; //size in world space

        sz = new vec2(size,size);
        sz2 = new vec2(size*0.5f,size*0.5f);
        
        buffers = new GPUBuffer[]{
            new GPUBuffer(resolution*resolution*4*4),
            new GPUBuffer(resolution*resolution*4*4),
            new GPUBuffer(resolution*resolution*4*4)
        };
        
        pidx=0;
    
        ByteBuffer bb = ByteBuffer.allocate(4*4);
        bb.order(ByteOrder.nativeOrder());
        bb.asFloatBuffer().put(new float[]{0,1,0,0});
        
        for(int i=0;i<3;++i){
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[i].buffer );
            glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_RGBA32F, GL_RGBA, GL_FLOAT, bb.array());
        }
        
        //set up vertex buffer
        float minx = -size/2.0f;
        float maxx = size/2.0f;
        float minz = -size/2.0f;
        float maxz = size/2.0f;
        //if we have n x n grid: We have (n-1)x(n-1) quads.
        //Each quad has 2 triangles; each triangle has 3 vertices; each vertex
        //has 4 floats
        this.numtris = (resolution-1)*(resolution-1)*2;
        float[] vdata = new float[this.numtris*3*4];
        int k=0;
        int n = resolution;
        for(int ii=0;ii<resolution-1;++ii){
            for(int jj=0;jj<resolution-1;++jj){
                float i = ii * 1.0f/n;
                float j = jj * 1.0f/n;
                float i1 = (ii+1) * 1.0f/n;
                float j1 = (jj+1) * 1.0f/n;
                float ax = minx + i*size;
                float az = minz + j*size;
                float bx = minx + i1*size;
                float cz = minz + j1*size;
                
                int ii1 = ii+1;
                int jj1 = jj+1;
                
                
                //vertex format: x,z, row, col
                
                // a   b
                // +--+
                // | /|
                // |/ |
                // +--+
                // c   d
                //acb
                vdata[k++]=ax; vdata[k++]=az; vdata[k++]= ii; vdata[k++]= jj;
                vdata[k++]=ax; vdata[k++]=cz; vdata[k++]= ii; vdata[k++]=jj1;
                vdata[k++]=bx; vdata[k++]=az; vdata[k++]=ii1; vdata[k++]= jj;
                //cdb
                vdata[k++]=ax; vdata[k++]=cz; vdata[k++]= ii; vdata[k++]=jj1;
                vdata[k++]=bx; vdata[k++]=cz; vdata[k++]=ii1; vdata[k++]=jj1;
                vdata[k++]=bx; vdata[k++]=az; vdata[k++]=ii1; vdata[k++]= jj;
            }
        }
        if( k != vdata.length )
            throw new RuntimeException("?");
        
        int[] tmp = new int[1];
        glGenVertexArrays(1, tmp);
        this.vao = tmp[0];
        glBindVertexArray(vao);
        glGenBuffers(1, tmp);
        this.vbuff = tmp[0];
        glBindBuffer(GL_ARRAY_BUFFER,this.vbuff);
        glBufferData(GL_ARRAY_BUFFER,vdata.length*4,Main.floatsToBytes(vdata),GL_STATIC_DRAW);
        glEnableVertexAttribArray(Program.POSITION_INDEX);
        glVertexAttribPointer(Program.POSITION_INDEX, 4, GL_FLOAT, false, 4*4,0);
        glBindVertexArray(0);
    }

    void update(float elapsed, ArrayList<vec5> pr){
    
        Program2 prog = updateprog;
        prog.use();
        
        this.accum_elapsed += elapsed;
    
        if( this.F/this.D <= 0 )
            System.out.println("Diverge 1");
        if( this.F/this.D >= Math.sqrt( this.q*this.t+2) / (2*this.t) )
            System.out.println("Diverge 2");

        //scale factor
        float sf = this.t*0.400f;
        while( this.accum_elapsed >= sf ){

            prog.setUniform("worldMatrix",this.worldMatrix);
            prog.setUniform("sz",this.sz);
            prog.setUniform("sz2",this.sz2);
            prog.setUniform("d",this.d);
            prog.setUniform("taus",new vec4(
                2*this.F*this.t*this.t/(this.D*this.d),
                this.q*this.d-2,
                4-(8*this.F*this.t*this.t)/(this.D*this.d),
                this.q*this.t+2));
            if( pr.size() > 0 ){
                vec5 R = pr.remove(0);
                //R=[x,dy,z,power,dist]
                prog.setUniform("ripplepos",R.xyz());
                prog.setUniform("ripplepower",R.w);
                prog.setUniform("rippledist",R.v);
            }
            else{
                prog.setUniform("ripplepos",new vec3(0,0,0));
                prog.setUniform("ripplepower",1.0);
                prog.setUniform("rippledist",0.0);
            }
            
            GPUBuffer prev = this.buffers[this.pidx]; 
            GPUBuffer curr = this.buffers[(this.pidx+1)%3];
            GPUBuffer next = this.buffers[(this.pidx+2)%3];
 
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, prev.buffer);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, curr.buffer);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, next.buffer);
            prog.setUniform("resolution",new ivec2(resolution,resolution));
            
            prog.dispatch(resolution/32,resolution/32,1);
            
            this.pidx = (this.pidx+1)%3;
            this.accum_elapsed -= sf;
        }
    }
    void draw(Camera camera, vec3 lightPos){
        Program prog = drawprog;
        prog.use();
        camera.draw(prog);
        prog.setUniform("lightPos",lightPos);
        prog.setUniform("lightColor",new vec4(1,1,1,1));
        prog.setUniform("worldMatrix", this.worldMatrix);
        prog.setUniform("resolution",new ivec2(resolution,resolution));
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER,0,buffers[(this.pidx+2)%3].buffer);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES,0,this.numtris*3);
    }
}
