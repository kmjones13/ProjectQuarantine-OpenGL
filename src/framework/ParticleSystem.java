package framework;
import static JGL.JGL.*;
import framework.math3d.vec3;
import framework.math3d.vec4;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.Random;

public class ParticleSystem {
    static Program2 updateprog;
    static Program2 drawprog;
    static Random R = new Random();
    static GPUBuffer randomBuffer;
    
    static float randrange(float n, float x){
        float r = n + R.nextFloat()*(x-n);
        if( r >= x )
            r=x-1;
        return r;
    }
    
    int nump;
    int vbuff;
    int vao;
    Texture tex;
    
    public ParticleSystem(String texfile, int nump,vec3 s){
        tex = new ImageTexture(texfile);
        vec3 pos=s;
        this.nump=nump;
        if( updateprog == null ){
            updateprog = new Program2("particlecs.txt");
            drawprog = new Program2("particlevs.txt","particlegs.txt",
                    "particlefs.txt");
            randomBuffer = new GPUBuffer(new int[]{0});
        }

        int[] tmp = new int[1];
        glGenVertexArrays(1,tmp);
        vao = tmp[0];
        glBindVertexArray(vao);
        glGenBuffers(1,tmp);
        vbuff = tmp[0];
        glBindBuffer(GL_ARRAY_BUFFER,vbuff);
        glEnableVertexAttribArray(Program.POSITION_INDEX);
        //notice the stride is eight now
        glVertexAttribPointer(Program.POSITION_INDEX,3,GL_FLOAT,false,8*4,0);
        glBindVertexArray(0);
        init(pos);
    }
    
    void init(vec3 s){
        //8 floats per particle, 4 bytes per float
        ByteBuffer bb = ByteBuffer.allocate( nump * 8 * 4 );
        bb.order(ByteOrder.nativeOrder());
        float[] D = new float[nump * 8];
        int j=0;
        for(int i=0;i<nump;++i){
            D[j++] = s.x;
            D[j++] = s.y;
            D[j++] = s.z;
            D[j++] = 1;
            D[j++] = randrange(-0.2f,0.2f);
            D[j++] = randrange(0.9f,1.2f);
            D[j++] = randrange(-0.2f,0.2f);
            D[j++] = 0.0f;
        }
        bb.asFloatBuffer().put(D);
        byte[] bdata = bb.array();
        glBindBuffer(GL_ARRAY_BUFFER,vbuff);
        glBufferData(GL_ARRAY_BUFFER,bdata.length,bdata,GL_STREAM_DRAW);
    }
    
   void update(float elapsed){// ,ArrayList<vec4> spheres, GPUBuffer spherebuffer){
        updateprog.use();
        updateprog.setUniform("elapsed",elapsed);
        updateprog.setUniform("gravity",new vec3(0,0,0));
        
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER,0,vbuff);
        
        updateprog.dispatch(nump/64,1,1);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER,0,0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER,1,0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER,2,0);
    }
    
    void draw(Camera cam){
        drawprog.use();
        cam.draw2(drawprog);
        drawprog.setUniform("texture",tex);
        drawprog.setUniform("alpha",1);
        glBindVertexArray(vao);
        glDrawArrays(GL_POINTS,0,nump);
    }
}
