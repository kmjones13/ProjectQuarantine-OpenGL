/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package framework;

import framework.math3d.math3d;
import framework.math3d.vec3;
import java.util.Random;

/**
 *
 * @author Kee
 */
public class Collectable {
    Random rand;
    int min;   
    int max;
    int x;
    int z;
    vec3 Cpos = new vec3(x,0,z);
    Mesh iteam;
    public Collectable(String b){
        iteam =new Mesh(b);
        rand = new Random();
        
        min =-60;   
        max=60;
        x = rand.nextInt((max - min) + 1) + min;
        z= rand.nextInt((max - min) + 1) + min;
        Cpos = new vec3(x,0,z);
    }
    public Boolean collion(vec3 target)
    {
         
        double xDistance = target.x-Cpos.x;
        double yDistance = target.y-Cpos.y;
        double zDistance = target.z-Cpos.z;
       
        double hypotenuse = Math.sqrt((xDistance * xDistance) + (yDistance * yDistance)+ (zDistance * zDistance));
        if(hypotenuse < 3)
        {
            return true;
        }
        return false;
    }
    void draw(Program prog){
        iteam.centroid=Cpos;
        prog.setUniform("worldMatrix",math3d.translation(iteam.centroid)); 
        iteam.draw(prog);
    }
    
}
