/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package framework;

import framework.math3d.math3d;
import framework.math3d.vec3;

/**
 *
 * @author jonesk13
 */
public class Bullet {
    
    vec3 Dirction;
    float time =0;
    Mesh BMesh = new Mesh("assets/MainCharacter/Bullet.obj.mesh");
    double angle;
    public Bullet(vec3 dir,vec3 start,double a){
        Dirction=dir;
        BMesh.centroid=start;
        angle=a;
        
    }
    void update(float dt){
        //Direction = campostion* charcters dirction
        time+=dt;
        //if(time<=20)
       // {        
        BMesh.centroid=math3d.add(BMesh.centroid,math3d.mul(Dirction,dt+2));
        //}
    }
    public Boolean collion(vec3 target)
    {
         
        double xDistance = target.x-BMesh.centroid.x;
        double yDistance = target.y-BMesh.centroid.y;
        double zDistance = target.z-BMesh.centroid.z;
       
        double hypotenuse = Math.sqrt((xDistance * xDistance) + (yDistance * yDistance)+ (zDistance * zDistance));
        if(hypotenuse < 3)
        {
            return true;
        }
        return false;
    }
    void draw(Program prog){
        
        prog.setUniform("worldMatrix",math3d.mul(math3d.axisRotation(new vec3(0,1,0), angle),math3d.translation(BMesh.centroid))); 
        BMesh.draw(prog);
        
    }
}
