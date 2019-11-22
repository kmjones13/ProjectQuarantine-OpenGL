/*
 */
package framework;

import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import javax.imageio.ImageIO;
import static JGL.JGL.*;
import static framework.Texture.isPowerOf2;


/**
 *
 * @author jhudson
 */
public class GrayscaleImageTexture  extends Texture2D{
    byte[] pix;
    public GrayscaleImageTexture(String filename){
        super(0,0);
        try {
            BufferedImage img = ImageIO.read(new File(filename));
            int[] pdata = img.getRGB(0,0,img.getWidth(),img.getHeight(),null,0,img.getWidth()); 
            pix = new byte[pdata.length];
            //need to invert the y's 
            int i=0;
            for(int row=0;row<img.getHeight();++row){
                int j= (img.getHeight()-row-1)*img.getWidth();
                for(int k=0;k<img.getWidth();++k){
                    int red = ((pdata[i] & 0x00ff0000)>>16);
                    int green = ((pdata[i] & 0x0000ff00)>>8);
                    int blue = ((pdata[i] & 0x000000ff));
                    int avg = (red+green+blue)/3;
                    pix[j++] = (byte) avg;
                    i++;
                }
            }
            this.w=img.getWidth();
            this.h=img.getHeight();
        } catch (IOException ex) {
            throw new RuntimeException("Cannot read image "+filename);
        }

        bind(0);
        
        int fmt = GL_R8;

        glTexImage2D(GL_TEXTURE_2D,0,GL_R8,w,h,0,GL_RED,GL_UNSIGNED_BYTE, pix);
        
        if(isPowerOf2(w) && isPowerOf2(h) ){
            glGenerateMipmap(GL_TEXTURE_2D);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        }
        else{
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        }
    }
   
}
