package framework;

import framework.math3d.vec3;
import framework.math3d.mat4;
import java.util.Set;
import java.util.TreeSet;
import static JGL.JGL.*;
import static JSDL.JSDL.*;
import framework.math3d.math3d;
import framework.math3d.vec4;
import java.math.*;
import java.util.*;
/**
 *
 * @author jonesk13
 */
public class Enemy  {
    
    Mesh alien_Walk1 = new Mesh("assets/Main_Level/martian/Alien_Walk1.obj.mesh");
    Mesh alien_Walk2 = new Mesh("assets/Main_Level/martian/Alien_Walk2.obj.mesh");
    
    Mesh alien_Lhit1 = new Mesh("assets/Main_Level/martian/Alien_Lhit1.obj.mesh");
    Mesh alien_Lhit2 = new Mesh("assets/Main_Level/martian/Alien_Lhit2.obj.mesh");
    Mesh alien_Lhit3 = new Mesh("assets/Main_Level/martian/Alien_Lhit3.obj.mesh");
    
    Mesh alien_Rhit1 = new Mesh("assets/Main_Level/martian/Alien_Rhit1.obj.mesh");
    Mesh alien_Rhit2 = new Mesh("assets/Main_Level/martian/Alien_Rhit2.obj.mesh");
    Mesh alien_Rhit3 = new Mesh("assets/Main_Level/martian/Alien_Rhit3.obj.mesh");
    
    Random rand;
    int min;   
    int max;
    int x;
    int z;
    vec3 Apos = new vec3(x,0,z);
    Mesh Movement=alien_Walk1 ;
    float walkTime=0;
    float attackTime=0;
    int walkFrame=0;
    int attackFrame=0;
    boolean hit=false;
    double xDistance;
    double yDistance;
    double zDistance;
    double hypotenuse;
    public Enemy(){
 
        rand = new Random();
        
        min =-60;   
        max=60;
        x = rand.nextInt((max - min) + 1) + min;
        z= rand.nextInt((max - min) + 1) + min;
        Apos = new vec3(x,0,z);
        Movement.centroid=Apos;
        
    }
    void checkCollison(vec3 target)
    {
        target.y+=2;
        float A= Apos.y+3;
        xDistance = target.x-Apos.x;
        yDistance = target.y-A;
        zDistance = target.z-Apos.z;
        
        hypotenuse = Math.sqrt((xDistance * xDistance) + (yDistance * yDistance)+ (zDistance * zDistance));
        int TargetR=4;
        int AlienR=2;
        if(hypotenuse > TargetR-AlienR)
        {
            hit = false;
        }
        else if(hypotenuse < TargetR-AlienR)
        {
            hit = true;
        }
    }
    int update(float dt){
//        target.y+=2;
//        float A= Apos.y+3;
//        double xDistance = target.x-Apos.x;
//        double yDistance = target.y-A;
//        double zDistance = target.z-Apos.z;
//        
//        double hypotenuse = Math.sqrt((xDistance * xDistance) + (yDistance * yDistance)+ (zDistance * zDistance));
//        int TargetR=3;
//        int AlienR=2;
        if(hit==false)
        {
                attackFrame=0;
                walkTime+=dt;
                if(walkTime>=0.5)
                {
                    walkTime=0;
                    walkFrame+=1;
                    if(walkFrame>1)
                    {
                        walkFrame=0;
                    }
                    
                }
                if(walkFrame==0)
                {
                    Movement=alien_Walk1;
                }
                else
                {
                    Movement=alien_Walk2;
                }
                //System.out.println(hypotenuse);    
                Apos.y += dt*2*(yDistance/hypotenuse);
                Apos.x += dt*2*(xDistance/hypotenuse);
                Apos.z += dt*2*(zDistance/hypotenuse);
        }
        if(hit==true)
        {
            attackTime+=dt;
            if(attackTime>=0.4)
            {
                attackTime=0;
                
                if(attackFrame>5)
                {
                    attackFrame=0;
                }
                    
            }
            if(attackFrame==0)
            {
                Movement=alien_Lhit1;
            }
            if(attackFrame==1)
            {
                Movement=alien_Lhit2;
                Apos.y=0;
                Movement.centroid=Apos;
                return 1;
            }
            if(attackFrame==2)
            {
                Movement=alien_Lhit3;
            }
            if(attackFrame==3)
            {
                Movement=alien_Rhit1;
            }
            if(attackFrame==4)
            {
                Movement=alien_Rhit2;
                Apos.y=0;
                Movement.centroid=Apos;
                return 1;
            }
            if(attackFrame==5)
            {
                Movement=alien_Rhit3;
            }
            attackFrame+=1;
            
            
        }
        Apos.y=0;
        Movement.centroid=Apos;
        return 0;
    }
    void draw(Program prog){
        Movement.centroid=Apos;
        Movement.draw(prog);
    }
}
