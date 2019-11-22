package framework;

import static JGL.JGL.*;
import framework.math3d.ivec2;
import framework.math3d.mat4;
import static framework.math3d.math3d.dot;
import static framework.math3d.math3d.length;
import static framework.math3d.math3d.mul;
import static framework.math3d.math3d.sub;
import framework.math3d.vec2;
import framework.math3d.vec3;
import framework.math3d.vec4;
import java.awt.Color;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import javax.imageio.ImageIO;

/**
 *
 * @author jhudson
 */
public class Heightmap {
    ivec2 resolutioni;
    vec2 resolutionf;
    static Program2 drawprog;
    int vao;
    int vbuff;
    GrayscaleImageTexture htex;
    int numtris;
    float recip;
    BufferedImage MapImage;
    //mat4 worldMatrix;
    //vec3 U,V;
    //float Ulen, Vlen;
    //vec3 Min;
    
    vec3 Min,Max;
    public Heightmap(String fname, float xsize, float ysize, float zsize, int resfactor ) throws IOException{
        if( drawprog == null ){
            drawprog = new Program2("hmapvs.txt","hmapfs.txt");
        }
        htex = new GrayscaleImageTexture(fname);
        int w = htex.w;
        int h = htex.h;
        
        Min = new vec3( 0,0,0);
        Max = new vec3( xsize,ysize,zsize);
        int gray;
        int white=10;
        int black= 0;
        
        resolutioni = new ivec2(htex.w,htex.h);
        resolutionf = new vec2(resolutioni.x,resolutioni.y);
        File img = new File(fname);
        MapImage = ImageIO.read(img);

        
        
        
    }
    
    void generate(int resfactor){
        //set up vertex buffer
        //if we have n x n grid: We have (n-1)x(n-1) quads.
        //Each quad has 2 triangles; each triangle has 3 vertices; each vertex
        //has 2 floats
        this.numtris = (resfactor*(resolutioni.x-1))*(resfactor*(resolutioni.y-1))*2;
        float[] vdata = new float[this.numtris*3*2];
        int k=0;
        recip = 1.0f/resfactor;
        
        for(int i=0;i<htex.w-1;++i){
            float ii = i;
            for(int iinc = 0 ; iinc < resfactor ; iinc++, ii += recip ){
                for(int j=0;j<htex.h-1;++j){
                    float jj = j;
                    for(int jinc=0;jinc<resfactor;++jinc, jj += recip){
                        vdata[k++] = ii; vdata[k++] = jj;       //a
                        vdata[k++] = ii; vdata[k++] = jj+recip;     //c
                        vdata[k++] = ii+recip; vdata[k++] = jj;     //b

                        vdata[k++] = ii+recip; vdata[k++] = jj;     //b
                        vdata[k++] = ii; vdata[k++] = jj+recip;     //c
                        vdata[k++] = ii+recip; vdata[k++] = jj+recip;   //d
                    }
                }
            }
        }
        if( k != vdata.length )
            throw new RuntimeException("?");
        
        int[] tmp = new int[1];
        if( this.vao == 0 ){
            glGenVertexArrays(1, tmp);
            this.vao = tmp[0];
        }
        glBindVertexArray(vao);
        if(this.vbuff == 0 ){
            glGenBuffers(1, tmp);
            this.vbuff = tmp[0];
        }
        glBindBuffer(GL_ARRAY_BUFFER,this.vbuff);
        glBufferData(GL_ARRAY_BUFFER,vdata.length*4,Main.floatsToBytes(vdata),GL_STATIC_DRAW);
        glEnableVertexAttribArray(Program.POSITION_INDEX);
        glVertexAttribPointer(Program.POSITION_INDEX, 2, GL_FLOAT, false, 2*4,0);
        glBindVertexArray(0);
    }

    float texel(int xi, int zi){
        int idx = zi*htex.w+xi;
        if( idx < 0 || idx >= htex.pix.length)
            return 0;
        int val = htex.pix[idx];
        val = val & 0xff;
        float valf = val;
        valf /= 255.0;
        valf = Min.y + valf * (Max.y-Min.y);
        return valf;
        //return mul( new vec4(0,valf,0,1), worldMatrix).y;
    }
    
    float getHeight(float x, float z ){
//        float pctx = (x - Min.x ) / (Max.x-Min.x);
//        float pctz = (z - Min.z ) / (Max.z-Min.z);
//        float x_ = pctx * resolutionf.x;
//        float z_ = pctz * resolutionf.y;
//        int xf = (int)x_;   //floor of x
//        int zf = (int)z_;   //floor of z
//        float A = texel( xf,zf);
//        float B = texel( xf+1,zf);
//        float C = texel( xf,zf+1);
//        float D = texel( xf+1,zf+1);
//        float xpct = x_-xf;
//        float zpct = z_-zf;
//        float E = A + xpct * ( B-A );
//        float F = C + xpct * ( D-C );
//        float G = E + zpct * (F-E);
        float px = x;
        float pz = z;
        float percentX = (px-(-75))/(75-(-75));
        float percentZ = (pz-(-75))/(75-(-75));
        int pointX = (int) (MapImage.getWidth()*percentX);
        int pointY = (int) (MapImage.getHeight()*percentZ);
        int color =  MapImage.getRGB(pointX, pointY)&255;
        float red = color;
        System.out.println(red);
        System.out.println(color);
        float G= (float) ((red/255.0)*(10)); 
        return G;
    }
    
    void draw(Camera camera, vec3 lightPos){
        Program2 prog = drawprog;
        prog.use();
        camera.draw2(prog);
        prog.setUniform("lightPos",lightPos);
        prog.setUniform("lightColor",new vec4(1,1,1,1));
        prog.setUniform("Min",Min);
        prog.setUniform("Max",Max);
        prog.setUniform("htex",htex);
        prog.setUniform("resolution",resolutionf);
        prog.setUniform("recip",recip);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES,0,this.numtris*3);
    }
}
