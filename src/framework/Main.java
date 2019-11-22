 package framework;

import framework.math3d.vec3;
import framework.math3d.mat4;
import java.util.Set;
import java.util.TreeSet;
import static JGL.JGL.*;
import static JSDL.JSDL.*;
import framework.math3d.math3d;
import static framework.math3d.math3d.mul;
import framework.math3d.vec2;

import framework.math3d.vec4;
import framework.math3d.vec5;
import java.io.IOException;
import java.math.*;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.Random;

public class Main{
    private enum STATE{
        MENU,
        SWAMP,
        PAUSE,
        WOODS,
        TUNNELS,
        CITY,
        DEAD,
        WIN,
        HILLS
    };
   
//    struct ParticleData{
//    vec4 position;  //only x,y,z are used
//    vec4 velocity;  //only x,y,z are used
//};
//layout(std430,binding=0) buffer Foo {
//    ParticleData particles[];
//};
    public static byte[] floatsToBytes(float[] f){
        ByteBuffer b = ByteBuffer.allocate(f.length*4);
        b.order(ByteOrder.nativeOrder());
        b.asFloatBuffer().put(f);
        return b.array();
    }
    
    public static void main(String[] args) throws IOException{  
        class sphere{
            vec4 center;
            float radius;
            vec4 color;
            vec2 anisotropy;
        }
        Random rand = null;
        boolean showshadows=false;
        STATE State = STATE.MENU;
        //current level
        STATE CurrState = STATE.MENU;
        
        SDL_Init(SDL_INIT_VIDEO);        
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE,8);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
        long win = SDL_CreateWindow("ETGG 2802",40,60, 700,600, SDL_WINDOW_OPENGL );
        SDL_GL_CreateContext(win);
        
        glDebugMessageControl(GL_DONT_CARE,GL_DONT_CARE,GL_DONT_CARE, 0,null, true );
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(
                (int source, int type, int id, int severity, String message, Object obj ) -> {
                    System.out.println("GL message: "+message);
                    if( severity == GL_DEBUG_SEVERITY_HIGH )
                        System.exit(1);
                },
               null);

        int[] tmp = new int[1];
        glGenVertexArrays(1,tmp); 
        int vao = tmp[0];
        glBindVertexArray(vao);


        glClearColor(0.2f,0.4f,0.6f,1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        Set<Integer> keys = new TreeSet<>();
        //levels
        Swamp swamp= null;
        Tunnels Tunnels= null;
        Woods Woods   = null;
        Hills Hills = null;
//        Swamp swamp = new Swamp();
//        
//        Tunnels Tunnels = new Tunnels();
       City City = new City();
//        Hills Hills= new Hills();
 //       Woods Woods = new Woods();
        
        
        
        Heightmap hmap = new Heightmap( "assets/Main_Level/Hill_Land/level1_HeightMap.png", 20, 20, 20,20 );
        float turns=1;
        float turnV=1;
        Camera cam;
        Camera cam2= new Camera();
        boolean pressed = false;
        Program prog;
        Program progHud;
        Mesh HUD2;
        Program blurprog;
        float prev;
        int level=1;
        boolean move=true;
        Framebuffer fbo1;
        Framebuffer fbo2;
        double angle=180;
        vec3 q;
        vec4 f =math3d.mul(new vec4(0,0,1,0), math3d.axisRotation(new vec3(0,1,0),angle));
        q =new vec3(f.x,f.y,f.z);
        //Swamp
        //IWater lake = new IWater(256,10);
        
        //PLAYER
        Player GDude = new Player();
        float run=(float) 0.0;
        
        //shadows    
        fbo1 = new Framebuffer(2000,2000,GL_RGBA,GL_FLOAT);
        Texture2D dummytex = new SolidTexture(GL_UNSIGNED_BYTE,0,0,0,0);
        cam2.eye=new vec4(0,120,0,1);
        //cam2.eye = new vec4(0,100,0,1);       
        cam2.yon=200;
        cam2.hither=1;
        //cam2.fov_h = cam2.fov_v = 25.0f;
        cam2.compute_proj_matrix();
        cam2.lookAt(cam2.eye.xyz(), new vec3(0,0,0), new vec3(1,0,0));
        
        //menu
        Mesh BG = new Mesh("assets/MENU/BG.obj.mesh");
        Mesh ground = new Mesh("assets/MENU/Ground.obj.mesh");
        Mesh HillGo = new Mesh("assets/MENU/HillsGo.obj.mesh");
        Mesh HillNo = new Mesh("assets/MENU/HillsNo.obj.mesh");
        Mesh SwampNo = new Mesh("assets/MENU/SwampNo.obj.mesh");
        Mesh SwampGo = new Mesh("assets/MENU/SwampGo.obj.mesh");
        Mesh CityNo = new Mesh("assets/MENU/CityNo.obj.mesh");
        Mesh CityGo = new Mesh("assets/MENU/CityGo.obj.mesh");
        Mesh WoodsGo = new Mesh("assets/MENU/WoodsGo.obj.mesh");
        Mesh WoodsNo = new Mesh("assets/MENU/WoodsNo.obj.mesh");
        Mesh TunnelsGo = new Mesh("assets/MENU/TunnelsGo.obj.mesh");
        Mesh TunnelsNo = new Mesh("assets/MENU/TunnelsNo.obj.mesh");
        
        //HUD
        UnitSquare HUD=new UnitSquare();
        UnitSquare CrossHair=new UnitSquare();
        UnitSquare Score=new UnitSquare();
        
        //bullest
        ArrayList<Bullet> Bullets=new ArrayList<Bullet>();//creating arraylist of Bullets
        
        //ripples
        ArrayList<vec5> pending_ripples=new ArrayList<vec5>();
        
        //iteams
        ArrayList<Collectable> packs =new ArrayList<Collectable>();
        for (int i = 0; i < 15; i++) {	
            Collectable P = new Collectable("assets/PickipItems/HealthBox.obj.mesh");
            packs.add(P);
        }
        
        ArrayList<Collectable> Battery =new ArrayList<Collectable>();
        for (int i = 0; i < 15; i++) {	
            Collectable Be = new Collectable("assets/PickipItems/Power.obj.mesh");
            Battery.add(Be);
        }
        
        
        //enemy       
        Enemy[] aliens = new Enemy[10];
        
        for (int i = 0; i < 10; i++) {			
            aliens[i] = new Enemy();
        }
        
        //particles
        //ParticleSystem Burst= new ParticleSystem("assets/Particles/u.png",128);
        ArrayList<ParticleSystem> Sparks = new ArrayList<ParticleSystem>();
        //ArrayList<vec4> spheres=new ArrayList<vec4>();
        //GPUBuffer spherebuffer = new GPUBuffer(tmpf);
        
        
//        fbo1 = new Framebuffer(512,512);
//        fbo2 = new Framebuffer(512,512);
//        
//
        prog = new Program("vs.txt","fs.txt");
        Program prog2 = new Program("vs.txt","fs.txt");
        Program progShawdows = new Program("vs2.txt","fs2.txt");
        Program progDistancesGrey = new Program("vsDistance.txt","fsDistance.txt");
        blurprog = new Program("blurvs.txt","blurfs.txt");
        
        
        progHud = new Program("vs.txt","fs.txt");

        cam = new Camera();
       
        
        prev = (float)(System.nanoTime()*1E-9);
        int degree=0;
        SDL_Event ev=new SDL_Event();
        while(true){
            while(true){
                int rv = SDL_PollEvent(ev);
                if( rv == 0 )
                    break;
                //System.out.println("Event "+ev.type);
                if( ev.type == SDL_QUIT )
                    System.exit(0);
                if( ev.type == SDL_MOUSEMOTION ){
                    //System.out.println("Mouse motion "+ev.motion.x+" "+ev.motion.y+" "+ev.motion.xrel+" "+ev.motion.yrel);
                }
                if( ev.type == SDL_KEYDOWN ){
                    //System.out.println("Key press "+ev.key.keysym.sym+" "+ev.key.keysym.sym);
                    if( ev.key.keysym.sym == SDLK_F1)
                        showshadows = !showshadows;
                    
                    keys.add(ev.key.keysym.sym);
                   
                }
                if( ev.type == SDL_KEYUP ){
                    keys.remove(ev.key.keysym.sym);
                    //System.out.println(cam.eye);
                    move=true;
                    pressed = false;
                    
                }
            }
            float now = (float)(System.nanoTime()*1E-9);
            float elapsed = now-prev;

            prev=now;
           
            if (State == STATE.MENU){
                 if( keys.contains(SDLK_UP) & move==true)
                { 
                    move=false;
                    level--;
                    if (level==0)
                    {
                       level=5; 
                    }
                }
                if( keys.contains(SDLK_DOWN) & move==true)
                { 
                    move=false;
                    level++;
                    if (level==6)
                    {
                       level=1; 
                    }
                }
                if( keys.contains(SDLK_RETURN) & move==true & pressed==false)
                { 
                    if (level==1)
                    {
                        State = STATE.SWAMP;
                        //current level
                        CurrState = STATE.SWAMP;
                        //swamp = new Swamp();
                    }
                    if (level==2)
                    {
                        State = STATE.CITY;
                        //current level
                        CurrState = STATE.CITY;
                      
                       // City = new City();
        
                    }
                    if (level==3)
                    {
                        State = STATE.HILLS;
                        //current level
                        CurrState = STATE.HILLS;
                        
                        //Hills= new Hills();
                    }
                    if (level==4)
                    {
                        State = STATE.WOODS;
                        //current level
                        CurrState = STATE.WOODS;
                       // Woods = new Woods();
        
                    }
                    if (level==5)
                    {
                        State = STATE.TUNNELS;
                        //current level
                        CurrState = STATE.TUNNELS;
                        
                        //Tunnels = new Tunnels();
        
                    }
                    Bullets.clear();
                    packs.clear();
                    for (int i = 0; i < 15; i++) {	
                        Collectable P = new Collectable("assets/PickipItems/HealthBox.obj.mesh");
                        packs.add(P);
                    }

                    Battery.clear();
                    for (int i = 0; i < 15; i++) {	
                        Collectable Be = new Collectable("assets/PickipItems/Power.obj.mesh");
                        Battery.add(Be);
                    }


                    //enemy       
                    aliens = new Enemy[15];
                    for (int i = 0; i < 15; i++) {			
                        aliens[i] = new Enemy();
                    }
                    GDude.position=new vec3(0,0,0);
                }
                
               
                 
            }
            else if (State == STATE.PAUSE){
                
               if( keys.contains(SDLK_p) & pressed==false)
               {
                   State = CurrState;
                   pressed=true;
               }
               if( keys.contains(SDLK_RETURN)& pressed==false)
               {
                   State = State.MENU;
                   pressed=true;
                   
               }
            }
            else if (State == STATE.DEAD || State == STATE.WIN){
                
               
               if( keys.contains(SDLK_RETURN)& pressed==false)
               {
                   State = State.MENU;
                   pressed=true;
                   
               }
            }
            else
            {
                vec3 turns2 = new vec3(0,1,0);            
                //forward = math3d.axisRotation(forward,Math.toRadians(degrees));
                if( keys.contains(SDLK_LSHIFT))
                {
                    run=(float) 0.1;
                }
                else
                {
                    run=0;
                }
                
                if( keys.contains(SDLK_LEFT))
                {
                   
                   if(turns < 20)
                   { 
                        cam.turn(0.4f*elapsed);
                        turns= (turns+0.4f);
                        System.out.println(turns);
                   }
                   //cam.turn(0.4f*elapsed);
                }
               if( keys.contains(SDLK_RIGHT))
                { 
                    
                    if(turns > -20)
                    {
                        cam.turn(-0.4f*elapsed);
                        turns= (turns+-0.4f);
                        System.out.println(turns);
                    }
                    //cam.turn(-0.4f*elapsed);
                }
                if( keys.contains(SDLK_UP))
                {
                    
                    if( turnV <20)
                    {
                        cam.pitch(0.4f*elapsed);
                        turnV= (turnV+0.4f);
                        System.out.println(turnV);

                    }
                }
                if( keys.contains(SDLK_DOWN))
                {
                    
                    if( turnV > -20)
                    {
                        cam.pitch(-0.4f*elapsed);
                        turnV= (turnV-0.4f);
                        
                    }
                }
                //Charcter movement
                if( keys.contains(SDLK_w ))
                {
                    f =math3d.mul(new vec4(0,0,1,0), math3d.axisRotation(new vec3(0,1,0),angle));
                    q =new vec3(f.x,f.y,f.z);
                    GDude.position = math3d.add(GDude.position,math3d.mul(elapsed+run ,q)) ;
                    
                    GDude.update("Forward",elapsed,angle,run);
    
                    
  
                }   
                else if( keys.contains(SDLK_s))
                {
                    f =math3d.mul(new vec4(0,0,1,0), math3d.axisRotation(new vec3(0,1,0),angle));
                    q =new vec3(f.x,f.y,f.z);
                    GDude.position = math3d.sub(GDude.position,math3d.mul(elapsed+run ,q)) ;
                    GDude.update("Backward",elapsed,angle,run);

                }   
                else if( keys.contains(SDLK_a))
                {
                    GDude.update("Left",elapsed,angle,run);
                    angle=angle+0.04;
                }
                else if( keys.contains(SDLK_d))
                {
                    GDude.update("Right",elapsed,angle,run);
                       angle=angle-0.04;
                }
                else if(keys.contains(SDLK_p)& pressed==false)
                {
                    State = STATE.PAUSE;
                    pressed=true;
                    
                } 
                              
                else if(keys.contains(SDLK_SPACE))
                {
                    GDude.update("Shoot",elapsed,angle,run);
//                    Mesh b=new Mesh("assets/MainCharacter/Bullet.obj.mesh");
//                    b.centroid=GDude.position;
                    
                    vec3 Bpos= math3d.add(cam.W.xyz().neg());
                    vec3 BBB =cam.eye.xyz();
                    //BBB.y=4;
                   
                    Bullet B = new Bullet(Bpos,BBB,angle);
                    Bullets.add(B);
                    
                    
                }
                else
                {
                    GDude.update("Idle",elapsed,angle,run);
                }
                
                f =math3d.mul(new vec4(0,0,1,0), math3d.axisRotation(new vec3(0,1,0),angle));
                q =new vec3(f.x,f.y,f.z);
                f =math3d.mul(new vec4(1,0,0,0), math3d.axisRotation(new vec3(0,1,0),angle));
                vec3 r =new vec3(f.x,f.y,f.z);
                vec3 coi = math3d.add(GDude.position,math3d.mul(20,q));
                vec3 eye= math3d.add(GDude.position,math3d.mul(-4,q),math3d.mul(-2.3,r),new vec3(0,4,0));
                cam.lookAt(eye, coi, new vec3 (0,1,0));
                for (int i = 0; i < aliens.length; i++) {			
                    aliens[i].checkCollison(GDude.position);
                }
                for (int i = 0; i < aliens.length; i++) {			
                   int dec =aliens[i].update(elapsed);
                   GDude.Health=GDude.Health-dec;
                   if(GDude.Health<=-1)
                   {
                       State = STATE.DEAD;
                   }
                }
                
                
                if (State == STATE.HILLS){
                    GDude.position.y=hmap.getHeight(GDude.position.x, GDude.position.z);
                    aliens[0].Apos.y=hmap.getHeight(aliens[0].Apos.x, aliens[0].Apos.z);
                    System.out.println(GDude.position.y);
                }
                else
                {
                    GDude.position.y=0;
                }
                //bullet update
                for (int i = 0; i < Bullets.size(); i++) {			
                        Bullets.get(i).update(elapsed);
		}
                //particle update
                for (int i = 0; i < Sparks.size(); i++) {			
                        Sparks.get(i).update(elapsed);
		}
                //collision
                for (int i = 0; i < packs.size(); i++) {			
                    boolean HIT=   packs.get(i).collion(GDude.position);
                    if(HIT==true)
                    {
                         packs.remove(i);
                         GDude.Health=GDude.Health+1;
                    }
		}
                for (int i = 0; i < Battery.size(); i++) {			
                    boolean HIT=Battery.get(i).collion(GDude.position);
                    if(HIT==true)
                    {
                        Battery.remove(i);
                        GDude.BCount=GDude.BCount+1;
                        if(GDude.BCount>=9)
                        {
                            State = STATE.WIN;
                        }
                    }
		}
                
                for (int i = 0; i < Bullets.size(); i++) {
                    for (int x = 0; x < aliens.length; x++) {
                        boolean Away= Bullets.get(i).collion(aliens[x].Apos);
                        if(Away==true)
                        {
                            ParticleSystem Burst= new ParticleSystem("assets/Particles/u.png",128,aliens[x].Apos);
                            Sparks.add(Burst); 
        
                            //aliens[x].Apos=new vec3(rand.nextInt((60 - -60) + 1) + 60,0,rand.nextInt((60 - -60) + 1) + 60);

                        }
                    }
		}
                
            
                
                //particle update
                //Burst.update(elapsed);
//                float t = -cam.eye /vec4( GDude.position,0);
//                if( t < 0.0 ){
//                    //no intersection
//                }
//                else{
//                    var ip = tdl.add(camera.eye,tdl.mul(t,Q));
//                    pending_ripples.add([ip[0],0.001,ip[2],8,1.0] );
//                }
               //lake.update(elapsed,pending_ripples);
            }
            

           
            

            //the fbo stuff is for later...
            //fbo1.bind();
            
            //System.out.println("angle:"+angle+" eye="+eye+" coi="+coi+" q="+q+" r="+r);
            
            
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT );
            if (State == STATE.WOODS){
                     
                    
               if(!showshadows)     
                    fbo1.bind(); 
               glClearColor(1.0f,1.0f,1.0f,1.0f);
               glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT );
               progDistancesGrey.use();
               cam2.draw(progDistancesGrey);
                    
                    
                      
                    
               progDistancesGrey.setUniform("lightHitherYon",new vec3(cam2.hither,cam2.yon,cam2.yon-cam2.hither));
               progDistancesGrey.setUniform("worldMatrix",mat4.identity());
               Woods.draw(cam2, progDistancesGrey);
               progDistancesGrey.setUniform("worldMatrix",math3d.mul(math3d.axisRotation(new vec3(0,1,0), angle),math3d.translation(GDude.position))); 
                    
               
               GDude.draw(progDistancesGrey);
               
               if(!showshadows)
                    fbo1.unbind();

               if( !showshadows){
                    progShawdows.use();

                    cam.draw(progShawdows);


                    progShawdows.setUniform("lightPos",new vec4(cam2.eye));
                    progShawdows.setUniform("lightColor",new vec4(0,0,0,0));
                    progShawdows.setUniform("shadowbuffer",fbo1.texture);
                    progShawdows.setUniform("light_viewMatrix",cam2.viewMatrix);
                    progShawdows.setUniform("light_projMatrix",cam2.projMatrix);  
                    progShawdows.setUniform("lightHitherYon",new vec3(cam2.hither,cam2.yon,cam2.yon-cam2.hither));
                    progShawdows.setUniform("worldMatrix",mat4.identity());
                    Woods.draw(cam, progShawdows);
                    progShawdows.setUniform("worldMatrix",math3d.mul(math3d.axisRotation(new vec3(0,1,0), angle),math3d.translation(GDude.position)));          

                    GDude.draw(progShawdows);
                    
                    progShawdows.setUniform("shadowbuffer",dummytex);
                    prog.use();
                    cam.draw(prog);
                    for (int i = 0; i < aliens.length; i++) {			
                        prog.setUniform("worldMatrix",math3d.mul(math3d.axisRotation(new vec3(0,1,0), angle),math3d.translation(aliens[i].Movement.centroid)));  
                        aliens[i].Movement.draw(prog);
                    }
                    Woods.drawSKY( cam, progShawdows);
               }
            }
            if (State == STATE.TUNNELS){
               Tunnels.draw(win, cam, prog);
               prog.use();
               cam.draw(prog);

               prog.setUniform("lightPos",new vec3(0,5,0) );
               
               prog.setUniform("worldMatrix",mat4.identity());
               prog.setUniform("worldMatrix",math3d.mul(math3d.axisRotation(new vec3(0,1,0), angle),math3d.translation(GDude.position)));          
                     
               GDude.draw(prog);
            }
            if (State == STATE.HILLS){
                Hills.draw(win, cam, prog);
                prog.use();
                cam.draw(prog);

                prog.setUniform("lightPos",new vec3(0,50,0) ); 
                
                //float x = GDude.position.x/5860;
                // float y = GDude.position.y/5860;
                prog.setUniform("worldMatrix",mat4.identity());
                prog.setUniform("worldMatrix",math3d.mul(math3d.axisRotation(new vec3(0,1,0), angle),math3d.translation(GDude.position)));          
                GDude.draw(prog);

                //cam.draw(prog);
                for (int i = 0; i < aliens.length; i++) {			
                    prog.setUniform("worldMatrix",math3d.mul(math3d.axisRotation(new vec3(0,1,0), angle),math3d.translation(aliens[i].Movement.centroid)));  
                    aliens[i].Movement.draw(prog);
                }
                
                prog.use();
                cam.draw(prog);
                Hills.drawSKY(win, cam, prog);
            }
            if (State == STATE.SWAMP){
                
                
                swamp.draw(win, cam, prog2);
                prog2.use();
                
                cam.draw(prog2);

                prog2.setUniform("lightPos",new vec3(0,30,0) ); 

                prog2.setUniform("worldMatrix",mat4.identity());
                prog2.setUniform("worldMatrix",math3d.mul(math3d.axisRotation(new vec3(0,1,0), angle),math3d.translation(GDude.position)));          
                GDude.draw(prog2);
                 
                //cam.draw(prog);
                for (int i = 0; i < aliens.length; i++) {			
                    prog2.setUniform("worldMatrix",math3d.mul(math3d.axisRotation(new vec3(0,1,0), angle),math3d.translation(aliens[i].Movement.centroid)));  
                    aliens[i].Movement.draw(prog2);
                }
                //prog.setUniform("worldMatrix",math3d.mul(math3d.axisRotation(new vec3(0,1,0), angle),math3d.translation(aliens[0].Movement.centroid)));  
                //aliens[0].Movement.draw(prog);
                //lake.draw(cam, new vec3(0,30,0));
                swamp.drawSKY(win, cam, prog2);
            }
            if (State == STATE.CITY){
                     
                City.draw(win, cam, prog);
                prog.use();
                cam.draw(prog);

                prog.setUniform("lightPos",new vec3(0,30,0) ); 

                prog.setUniform("worldMatrix",mat4.identity());
                prog.setUniform("worldMatrix",math3d.mul(math3d.axisRotation(new vec3(0,1,0), angle),math3d.translation(GDude.position)));          
                GDude.draw(prog);

                for (int i = 0; i < aliens.length; i++) {			
                    prog.setUniform("worldMatrix",math3d.mul(math3d.axisRotation(new vec3(0,1,0), angle),math3d.translation(aliens[i].Movement.centroid)));  
                    aliens[i].Movement.draw(prog);
                }
            }
            if (State == STATE.MENU){             
                cam.eye.y=20;
                cam.eye.z=60;          
             
                cam.lookAt(cam.eye.xyz(), new vec3(0,0,0), new vec3(0,1,0));
                cam.pitch(.4f);
                prog.use();
               
                prog.setUniform("lightPos",new vec3(0,30,20)); 
                cam.draw(prog);
                         
                prog.setUniform("worldMatrix",mat4.identity());
                BG.draw(prog);
                ground.draw(prog);
                if(level==1)
                {
                    SwampGo.draw(prog);
                    CityNo.draw(prog);
                    HillNo.draw(prog);
                    WoodsNo.draw(prog);
                    TunnelsNo.draw(prog);
                }
                if(level==2)
                {
                    SwampNo.draw(prog);
                    CityGo.draw(prog);
                    HillNo.draw(prog);
                    WoodsNo.draw(prog);
                    TunnelsNo.draw(prog);
                    
                }
                if(level==3)
                {
                    SwampNo.draw(prog);
                    CityNo.draw(prog);
                    HillGo.draw(prog);
                    WoodsNo.draw(prog);
                    TunnelsNo.draw(prog);
                }
                if(level==4)
                {
                    SwampNo.draw(prog);
                    CityNo.draw(prog);
                    HillNo.draw(prog);
                    WoodsGo.draw(prog);
                    TunnelsNo.draw(prog);
                }
                if(level==5)
                {
                    SwampNo.draw(prog);
                    CityNo.draw(prog);
                    HillNo.draw(prog);
                    WoodsNo.draw(prog);
                    TunnelsGo.draw(prog);
                }
                
               
                //prog.setUniform("worldMatrix",math3d.translation(usq2.centroid));   
               
            }
            if(State == STATE.PAUSE)
            {
                prog.use();
                glDisable(GL_DEPTH_TEST);
                prog.setUniform("lightPos",new vec3(cam.eye.x,cam.eye.y,cam.eye.z));
                HUD.m.centroid.x=0f;
                HUD.m.centroid.y=0.02f;
                HUD.m.texture= new ImageTexture("assets/HUD/PAUSE.png");
                prog.setUniform("worldMatrix",mat4.identity());
                prog.setUniform("worldMatrix",math3d.mul(math3d.scaling(5.3f,17 ,1),math3d.translation(HUD.m.centroid)));  
                prog.setUniform("viewMatrix",mat4.identity());               


                HUD.draw(prog);
            }
            if(State == STATE.DEAD)
            {
                prog.use();
                glDisable(GL_DEPTH_TEST);
                prog.setUniform("lightPos",new vec3(cam.eye.x,cam.eye.y,cam.eye.z));
                HUD.m.centroid.x=0f;
                HUD.m.centroid.y=0.02f;
                HUD.m.texture= new ImageTexture("assets/HUD/Heath6_Dead.png");
                prog.setUniform("worldMatrix",mat4.identity());
                prog.setUniform("worldMatrix",math3d.mul(math3d.scaling(5.3f,17 ,1),math3d.translation(HUD.m.centroid)));  
                prog.setUniform("viewMatrix",mat4.identity());               


                HUD.draw(prog);
                
            }
            if(State == STATE.WIN)
            {
                prog.use();
                glDisable(GL_DEPTH_TEST);
                prog.setUniform("lightPos",new vec3(cam.eye.x,cam.eye.y,cam.eye.z));
                HUD.m.centroid.x=0f;
                HUD.m.centroid.y=0.02f;
                HUD.m.texture= new ImageTexture("assets/HUD/WIN.png");
                prog.setUniform("worldMatrix",mat4.identity());
                prog.setUniform("worldMatrix",math3d.mul(math3d.scaling(5.3f,17 ,1),math3d.translation(HUD.m.centroid)));  
                prog.setUniform("viewMatrix",mat4.identity());               


                HUD.draw(prog);
                
            }
//            float vertices[] = {1.0f, -1.0f,  0.0f,-1.0f,  1.0f,  0.0f, 1.0f, -1.0f,  0.0f};
//            int[] tmp2 = new int[1];
//            glGenVertexArrays(1,tmp2); 
//            int vao2 = tmp2[0];
//            glBindVertexArray(vao2);
//            glEnableVertexAttribArray(0);
//            glActiveTexture(GL_TEXTURE0);
//            glBindTexture(GL_TEXTURE_2D,HUD);
//            glDrawArrays(GL_TRIANGLES_STRIP,0,)
//            
//            glDisableVertexAttribArray(0);
//            glBindVertexArray(0);
            if (State != STATE.MENU ||State != STATE.PAUSE ||State != STATE.DEAD||State != STATE.WIN){
                prog.use();
                for (int i = 0; i < Bullets.size(); i++) {			
                            Bullets.get(i).draw(prog);
                }
                for (int i = 0; i < packs.size(); i++) {			
                            packs.get(i).draw(prog);
                }
                for (int i = 0; i < Battery.size(); i++) {			
                            Battery.get(i).draw(prog);
                }
                

                    

                //HUD

                prog.use();

//                glDisable(GL_DEPTH_TEST);
//                prog.setUniform("lightPos",new vec3(cam.eye.x,cam.eye.y,cam.eye.z));
//                Score.m.centroid.x=0.8f;
//                Score.m.centroid.y=1.7f;
//                HUD.m.centroid.x=0f;
//                HUD.m.centroid.y=0.02f;
//                String t = GDude.drawHeath();
//                HUD.m.texture= new ImageTexture(t);
//                CrossHair.m.texture= new ImageTexture("assets/HUD/CrossHair.png");
//                String t2 = GDude.drawBatteryCount();
//                Score.m.texture= new ImageTexture(t2);
//                prog.setUniform("worldMatrix",mat4.identity());
//                prog.setUniform("worldMatrix",math3d.mul(math3d.scaling(5.3f,17 ,1),math3d.translation(HUD.m.centroid)));  
//                prog.setUniform("viewMatrix",mat4.identity());               
//
//
//                HUD.draw(prog);
//                prog.setUniform("worldMatrix",math3d.mul(math3d.scaling(.3f,1 ,1),math3d.translation(Score.m.centroid)));
//                Score.draw(prog);
//                prog.setUniform("worldMatrix",math3d.mul(math3d.scaling(.3f,1 ,1),math3d.translation(CrossHair.m.centroid)));
//                CrossHair.draw(prog);

                
                glEnable(GL_DEPTH_TEST);
                glDepthMask(false);
                //fixme
                //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                //Burst.draw(cam);
                for (int i = 0; i < Sparks.size(); i++) {			
                            Sparks.get(i).draw(cam);
                    }
                glDepthMask(true);
            }
            SDL_GL_SwapWindow(win);
           

        }//endwhile
    }//end main
}
