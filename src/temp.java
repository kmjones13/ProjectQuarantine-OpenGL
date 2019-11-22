//package framework;
//
//import framework.math3d.vec3;
//import framework.math3d.mat4;
//import java.util.Set;
//import java.util.TreeSet;
//import static JGL.JGL.*;
//import static JSDL.JSDL.*;
//import framework.math3d.math3d;
//import static framework.math3d.math3d.mul;
//
//import framework.math3d.vec4;
//import java.math.*;
//
//public class Main{
//    
//    
//    public static void main(String[] args){  
//        
//        SDL_Init(SDL_INIT_VIDEO);        
//        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE);
//        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,24);
//        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE,8);
//        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);
//        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,2);
//        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
//        long win = SDL_CreateWindow("ETGG 2802",40,60, 700,600, SDL_WINDOW_OPENGL );
//        SDL_GL_CreateContext(win);
//        
//        //glDebugMessageControl(GL_DONT_CARE,GL_DONT_CARE,GL_DONT_CARE, 0,null, true );
//        //glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
//        //glDebugMessageCallback(
//        //        (int source, int type, int id, int severity, String message, Object obj ) -> {
//        //            System.out.println("GL message: "+message);
//        //            if( severity == GL_DEBUG_SEVERITY_HIGH )
//        //                System.exit(1);
//        //        },
//        //       null);
//
//        int[] tmp = new int[1];
//        glGenVertexArrays(1,tmp);
//        int vao = tmp[0];
//        glBindVertexArray(vao);
//
//        glClearColor(0.2f,0.4f,0.6f,1.0f);
//        glEnable(GL_DEPTH_TEST);
//        glDepthFunc(GL_LEQUAL);
//
//        glEnable(GL_BLEND);
//        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//        Set<Integer> keys = new TreeSet<>();
//        
//        float turns=1;
//        float turnV=1;
//        Camera cam;
//        Program prog;
//        Program prog2;
//        Program blurprog;
//        float prev;
//        Mesh hay;
//        Mesh Plane;
//        Mesh trees;
//        Mesh stemtrees;
//        Mesh Mcharacter;
//        UnitSquare usq;
//        Framebuffer fbo1;
//        Framebuffer fbo2;
//        double angle=180;
//        Texture2D dummytex = new SolidTexture(GL_UNSIGNED_BYTE,0,0,0,0);
//        //scene
//        Plane = new Mesh("assets/Main_Level/swamp/water.obj.mesh");
//        stemtrees = new Mesh("assets/Main_Level/swamp/strem_tree.obj.mesh");
//        trees = new Mesh("assets/Main_Level/swamp/trees_swamp.obj.mesh");
//        hay = new Mesh("assets/Main_Level/swamp/hayswamp.obj.mesh");
//        Mesh brick = new Mesh("assets/Main_Level/swamp/brickswamp.obj.mesh");
//        Mesh boat = new Mesh("assets/Main_Level/swamp/boatswamp.obj.mesh");
//        Mesh stump = new Mesh("assets/Main_Level/swamp/stump_Swamp.obj.mesh");
//        Mesh wood = new Mesh("assets/Main_Level/swamp/woodPileSwamp.obj.mesh");
//        Mesh cube = new Mesh("assets/Main_Level/swamp/cube.obj.mesh");
//        Mcharacter = new Mesh("assets/MainCharacter/dude.obj.mesh");
//        usq = new UnitSquare();
//        String[] tex =new String[6];
//        tex[0]="assets/Main_Level/swamp/s3px.png";
//        tex[1]="assets/Main_Level/swamp/s3nx.png";
//        tex[2]="assets/Main_Level/swamp/s3py.png";
//        tex[3]="assets/Main_Level/swamp/s3ny.png";
//        tex[4]="assets/Main_Level/swamp/s3pz.png";
//        tex[5]="assets/Main_Level/swamp/s3nz.png";
//        ImageCubeTexture skytex = new ImageCubeTexture(tex);
//        
//        //skycube = new Primitive(primitives.createCube(30),
//        //{tex:skytex});
//        
//        
//        //enemy
//        
//        Enemy[] aliens = new Enemy[15];
//        aliens[0] = new Enemy();
//        
//        fbo1 = new Framebuffer(512,512);
//        fbo2 = new Framebuffer(512,512);
//        
//
//        prog = new Program("vs.txt","fs.txt");
//        prog2 = new Program("vs.txt","SwampPfs.txt");
//        blurprog = new Program("blurvs.txt","blurfs.txt");
//        Program skyboxShader = new Program("Cubevs.txt","Cubefs.txt");
//
//        cam = new Camera();
//        
//                    
//        
//        
//        
//        prev = (float)(System.nanoTime()*1E-9);
//        int degree=0;
//        SDL_Event ev=new SDL_Event();
//        while(true){
//            while(true){
//                int rv = SDL_PollEvent(ev);
//                if( rv == 0 )
//                    break;
//                //System.out.println("Event "+ev.type);
//                if( ev.type == SDL_QUIT )
//                    System.exit(0);
//                if( ev.type == SDL_MOUSEMOTION ){
//                    //System.out.println("Mouse motion "+ev.motion.x+" "+ev.motion.y+" "+ev.motion.xrel+" "+ev.motion.yrel);
//                }
//                if( ev.type == SDL_KEYDOWN ){
//                    //System.out.println("Key press "+ev.key.keysym.sym+" "+ev.key.keysym.sym);
//                    keys.add(ev.key.keysym.sym);
//                }
//                if( ev.type == SDL_KEYUP ){
//                    keys.remove(ev.key.keysym.sym);
//                    //System.out.println(cam.eye);
//                    
//                    
//                }
//            }
//
//            float now = (float)(System.nanoTime()*1E-9);
//            float elapsed = now-prev;
//
//            prev=now;
//            
//            vec3 turns2 = new vec3(0,1,0);            
//            //forward = math3d.axisRotation(forward,Math.toRadians(degrees));
//            if( keys.contains(SDLK_w ))
//            {
//                //cam.walk(0.7f*elapsed);
//                //Mcharacter.centroid.z=Mcharacter.centroid.z-(1f*elapsed);
////                System.out.println(angle);
//                vec4 f =math3d.mul(new vec4(0,0,1,0), math3d.axisRotation(new vec3(0,1,0),angle));
//                vec3 q =new vec3(f.x,f.y,f.z);
//                Mcharacter.centroid = math3d.add(Mcharacter.centroid,math3d.mul(elapsed ,q)) ;
//                //Mcharacter.centroid.x += elapsed * Math.cos(angle);
//
//                //Mcharacter.centroid.z += elapsed * Math.sin(angle);
////                cam.eye.x = Mcharacter.centroid.x+1;
////                cam.eye.y = Mcharacter.centroid.y+4;
////                cam.eye.z = Mcharacter.centroid.z+2;
////                cam.compute_view_matrix();
//                //camPos.z= Mcharacter.centroid.z+2;
//                //cam.lookAt(camPos, new vec3(0,0,0), new vec3(0,1,0));
//                
//                System.out.println(turns);
//            }   
//            if( keys.contains(SDLK_s))
//            {
//            
//                vec4 f =math3d.mul(new vec4(0,0,1,0), math3d.axisRotation(new vec3(0,1,0),angle));
//                vec3 q =new vec3(f.x,f.y,f.z);
//                Mcharacter.centroid = math3d.sub(Mcharacter.centroid,math3d.mul(elapsed ,q)) ;
//                //Mcharacter.centroid.x -= elapsed * Math.cos(angle);
//
//                //Mcharacter.centroid.z -= elapsed * Math.sin(angle);
////                cam.eye.x = Mcharacter.centroid.x+1;
////                cam.eye.y = Mcharacter.centroid.y+4;
////                cam.eye.z = Mcharacter.centroid.z+2;
////                cam.compute_view_matrix();
//                //cam.walk(-100f*elapsed);
//            }   
//            if( keys.contains(SDLK_LEFT))
//            {
//               if(turns < 20)
//               { 
//                    cam.turn(0.4f*elapsed);
//                    turns= (turns+0.4f);
//                    System.out.println(turns);
//               }
//               //cam.turn(0.4f*elapsed);
//            }
//           if( keys.contains(SDLK_RIGHT))
//            { 
//                if(turns > -20)
//                {
//                    cam.turn(-0.4f*elapsed);
//                    turns= (turns+-0.4f);
//                    System.out.println(turns);
//                }
//                //cam.turn(-0.4f*elapsed);
//            }
//            if( keys.contains(SDLK_UP))
//            {
//                if( turnV <20)
//                {
//                    cam.pitch(0.4f*elapsed);
//                    turnV= (turnV+0.4f);
//                    System.out.println(turnV);
//                    
//                }
//            }
//            if( keys.contains(SDLK_DOWN))
//            {
//                if( turnV > -20)
//                {
//                    cam.pitch(-0.4f*elapsed);
//                    turnV= (turnV-0.4f);
//                    System.out.println(turnV);
//                }
//            }
//            if( keys.contains(SDLK_a))
//            {
//                angle=angle+0.04;
//               
//                
//                
//                //cam.pitch(0.4f*elapsed);\
//            }
//            if( keys.contains(SDLK_d))
//            {
//                   angle=angle-0.04;
//                   
//                
//              
//            }
//            vec4 f =math3d.mul(new vec4(0,0,1,0), math3d.axisRotation(new vec3(0,1,0),angle));
//            vec3 q =new vec3(f.x,f.y,f.z);
//            f =math3d.mul(new vec4(1,0,0,0), math3d.axisRotation(new vec3(0,1,0),angle));
//            vec3 r =new vec3(f.x,f.y,f.z);
//            vec3 coi = math3d.add(Mcharacter.centroid,math3d.mul(20,q));
//            vec3 eye= math3d.add(Mcharacter.centroid,math3d.mul(-4,q),new vec3(0,4,0));
//            cam.lookAt(eye, coi, new vec3 (0,1,0));
//            aliens[0].update(prog, elapsed, Mcharacter.centroid);
//            
////            vec4 f =math3d.mul(new vec4(0,0,1,0), math3d.axisRotation(new vec3(0,1,0),angle));
////            vec3 q =new vec3(f.x,f.y,f.z);
////            f =math3d.mul(new vec4(1,0,0,0), math3d.axisRotation(new vec3(0,1,0),angle));
////            vec3 r =new vec3(f.x,f.y,f.z);
////            vec3 coi = math3d.add(Mcharacter.centroid,math3d.mul(5,q));
////            vec3 eye= math3d.add(Mcharacter.centroid,math3d.mul(-3,q),new vec3(0,6,0));
////            //vec3 p1=Mcharacter.centroid.add(q.mul((float) (2.0)));
////           // vec3 p2=Mcharacter.centroid.add(q.mul((float) (-1.0)));
////            cam.lookAt(eye, coi, new vec3 (0,1,0));
//            //the fbo stuff is for later...
//            //fbo1.bind();
//            
//            //System.out.println("angle:"+angle+" eye="+eye+" coi="+coi+" q="+q+" r="+r);
//            
//            
//            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT );
//            
//            prog.use();
//            cam.draw(prog);
//           
//            prog.setUniform("lightPos",new vec3(0,50,0) ); 
//                                   
//            prog.setUniform("worldMatrix",mat4.identity());
//            hay.draw(prog);
//            stemtrees.draw(prog);
//            trees.draw(prog);
//            brick.draw(prog);        
//            boat.draw(prog);
//            stump.draw(prog);
//            wood.draw(prog);
//                 
//            prog.setUniform("worldMatrix",math3d.mul(math3d.axisRotation(new vec3(0,1,0), angle),math3d.translation(Mcharacter.centroid)));          
//            Mcharacter.draw(prog);
//            
//            //cam.draw(prog);
//            prog.setUniform("worldMatrix",math3d.mul(math3d.axisRotation(new vec3(0,1,0), angle),math3d.translation(aliens[0].alien.centroid)));  
//            aliens[0].alien.draw(prog);
//            //cam.draw(prog);
//            //if(math3d.distForm(Mcharacter.centroid,Plane.centroid) <1)
//           // {
//           //     System.out.println("hit");
//            //}
//            //System.out.println(math3d.distForm(Mcharacter.centroid,Plane.centroid));
//            //fbo1.unbind();
//            //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT );
//            //this is also for later...
///*          
//            
//            blurprog.use();
//            blurprog.setUniform("diffuse_texture",fbo1.texture);
//            usq.draw(blurprog);
//            blurprog.setUniform("diffuse_texture",dummytex);
//*/
//            mat4 reflec = new mat4( 1,0,0,0,  
//                                     0,-1,0,0,
//                                     0,0,1,0,
//                                     0,0,0,1);
//            prog.setUniform("worldMatrix",mat4.identity());
//            
//            prog.setUniform("viewMatrix",cam.drawWithAdditionalMatrix(prog, reflec));
////            glEnable(GL_STENCIL_TEST);        
////            glColorMask(true,true,true,true);
////            glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);
////            glStencilFunc(GL_EQUAL,1,~0);
////            glDepthMask(true);
////            
////            glStencilFunc(GL_ALWAYS,1,~0);
////            glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);
////            glDepthMask(false);
////            
////            glColorMask(false,false,false,false);
// //           Plane.draw(prog);
//            //reflection maxtrix
////            glStencilFunc(GL_ALWAYS,1,~0);
//            hay.draw(prog);
//            stemtrees.draw(prog);
//            trees.draw(prog);
//            brick.draw(prog);
//            
//            boat.draw(prog);
//            stump.draw(prog);
//            wood.draw(prog);
//            //System.out.print(Plane.centroid);
//            
//            prog2.use();
//            prog2.setUniform("lightPos",new vec3(0,50,0) ); 
//            prog2.setUniform("viewMatrix",mat4.identity());
//            prog2.setUniform("worldMatrix",mat4.identity());
//            cam.draw(prog2);
//            Plane.draw(prog2);
//            
//            skyboxShader.use();
//            skyboxShader.setUniform("samplerCube", skytex);
//            //skyboxShader.setUniform("v_worldPos", cube.centroid);
//            mat4 view = new mat4();
//            view=cam.projMatrix.mul(cam.viewMatrix);
//            skyboxShader.setUniform("viewProjMatrix",view);    
//            cam.draw(skyboxShader);       
//            cube.draw(skyboxShader);
//            
//            //cam.lookAt(Mcharacter.centroid, new vec3(0,0,0), new vec3(0,1,0));
//            //cam.lookAt(mainPos, new vec3(0,0,0), new vec3(0,1,0));
//            SDL_GL_SwapWindow(win);
//
//
//        }//endwhile
//    }//end main
//}
//
