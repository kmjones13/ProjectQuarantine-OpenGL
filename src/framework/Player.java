/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package framework;

import framework.math3d.math3d;
import framework.math3d.vec3;
import framework.math3d.vec4;

/**
 *
 * @author Kee
 */
public class Player {
    
        vec3 position = new vec3(0,0,0);
        Mesh Mcharacter_idle_1 = new Mesh("assets/MainCharacter/Frame/MainDude_idle_1.obj.mesh");
        Mesh Mcharacter_idle_2 = new Mesh("assets/MainCharacter/Frame/MainDude_idle_2.obj.mesh");
        Mesh Mcharacter_idle_3 = new Mesh("assets/MainCharacter/Frame/MainDude_idle_3.obj.mesh");
        Mesh Mcharacter_idle_4 = new Mesh("assets/MainCharacter/Frame/MainDude_idle_4.obj.mesh");
        Mesh Mcharacter_idle_5 = new Mesh("assets/MainCharacter/Frame/MainDude_idle_5.obj.mesh");
        Mesh Mcharacter_idle_6 = new Mesh("assets/MainCharacter/Frame/MainDude_idle_6.obj.mesh");
        
        
        Mesh Mcharacter_walk_1 = new Mesh("assets/MainCharacter/Frame/MainDude_walk_1.obj.mesh");
        Mesh Mcharacter_walk_2 = new Mesh("assets/MainCharacter/Frame/MainDude_walk_2.obj.mesh");
        
        Mesh Mcharacter_shoot_1 = new Mesh("assets/MainCharacter/Frame/MainDude_Shoot_1.obj.mesh");
        Mesh Mcharacter_shoot_2 = new Mesh("assets/MainCharacter/Frame/MainDude_Shoot_2.obj.mesh");
        
        Mesh Movement=Mcharacter_idle_1;
        float walkTime=0;
        int walkFrame=0;
        float idleTime=0;
        int idleFrame=0;
        float AttackTime=0;
        int AttackFrame=0;
        int Health=5;
        int BCount=0;
        void update(String Case,float elapsed,double angle,float run){
            
            if (Case.equals("Forward"))
            {
                idleTime=0;
                idleFrame=0;
                AttackTime=0;
                AttackFrame=0;
                
                walkTime+=elapsed;
                if(walkTime>=0.5 && run==0)
                {
                    walkTime=0;
                    walkFrame+=1;
                    if(walkFrame>1)
                    {
                        walkFrame=0;
                    }
                    
                }
                if(walkTime>=0.2 && run>0)
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
                    Movement=Mcharacter_walk_1;
                }
                else
                {
                    Movement=Mcharacter_walk_2;
                }
                
                
              
            
            }
            if (Case.equals("Backward"))
            {
                
                idleTime=0;
                idleFrame=0;
                AttackTime=0;
                AttackFrame=0;
                walkTime+=elapsed;
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
                    Movement=Mcharacter_walk_1;
                }
                else
                {
                    Movement=Mcharacter_walk_2;
                }
                
            }
            if (Case.equals("Left"))
            {
                
            }
            if (Case.equals("Right"))
            {
            
            }
            if (Case.equals("Shoot"))
            {
                Movement=Mcharacter_shoot_2;
            }
            if (Case.equals("Idle"))
            {
                
                walkTime=0;
                walkFrame=0;              
                AttackTime=0;
                AttackFrame=0; 
                idleTime+=elapsed;
                if(idleTime>=0.5)
                {
                    idleTime=0;
                    idleFrame+=1;
                    if(idleFrame>5)
                    {
                        idleFrame=0;
                    }
                    
                    
                    
                }
                if(idleFrame==0)
                {
                    Movement=Mcharacter_idle_1;
                }
                else if(idleFrame==1)
                {
                    Movement=Mcharacter_idle_2;
                }
                else if(idleFrame==2)
                {
                    Movement=Mcharacter_idle_3;
                }
                else if(idleFrame==3)
                {
                    Movement=Mcharacter_idle_4;
                }
                else if(idleFrame==4)
                {
                    Movement=Mcharacter_idle_5;
                }
                else if(idleFrame==5)
                {
                    Movement=Mcharacter_idle_6;
                }
                
                
            }
            
            
            
      
        }
        
    
        void draw(Program prog){
           Movement.centroid=position;
           Movement.draw(prog);

        }
        String drawHeath(){
            String t="assets/HUD/Heath1.png";
            if (Health==5)
            {
                 t = ("assets/HUD/Heath1.png");
            }
            else if (Health==4)
            {
                 t =("assets/HUD/Heath2.png");
            }
            else if (Health==3)
            {
                t =("assets/HUD/Heath3.png");
            }
            else if (Health==2)
            {
                 t =("assets/HUD/Heath3.png");
            }
            else if (Health==1)
            {
                 t =("assets/HUD/Heath4.png");
            }
            else if (Health==0)
            {
                 t =("assets/HUD/Heath5.png");
            }
            else if (Health<=-1)
            {
                 t =("assets/HUD/Heath6_Dead.png");
            }
            return t;
            
        }
        String drawBatteryCount(){
            String t="assets/HUD/Numbers/Number0.png";
            if (BCount==0)
            {
                 t =("assets/HUD/Numbers/Number0.png");
            }
            else if (BCount==1)
            {
                  t =("assets/HUD/Numbers/Number1.png");
            }
            else if (BCount==2)
            {
                  t =("assets/HUD/Numbers/Number2.png");
            }
            else if (BCount==3)
            {
                  t =("assets/HUD/Numbers/Number3.png");
            }
            else if (BCount==4)
            {
                  t =("assets/HUD/Numbers/Number4.png");
            }
            else if (BCount==5)
            {
                  t =("assets/HUD/Numbers/Number5.png");
            }
            else if (BCount==6)
            {
                  t =("assets/HUD/Numbers/Number6.png");
            }
            else if (BCount==7)
            {
                  t =("assets/HUD/Numbers/Number7.png");
            }
            else if (BCount==8)
            {
                  t =("assets/HUD/Numbers/Number8.png");
            }
            else if (BCount>=9) 
            {
                  t =("assets/HUD/Numbers/Number9.png");
            }
            return t;
        }
}
