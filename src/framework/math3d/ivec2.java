package framework.math3d;


public class ivec2{
    public int x,y;
    
    public ivec2(){
        x=y=0;
    }
    
    public ivec2(Object... args){
        init(args);
    }
    public Object clone(){
        return new ivec2(x,y);
    }
    
    public String toString(){
        return "["+x+" "+y+"]";
    }
    private void init(Object[] args){
        int ctr=0;
        int[] _M = new int[2];
        for(Object a : args){
            if( a instanceof Number )
                _M[ctr++] = ((Number)a).intValue();
            else if( a instanceof ivec2 ){
                _M[ctr++] = ((ivec2)a).x;
                _M[ctr++] = ((ivec2)a).y;
            }
            else if( a instanceof int[] ){
                int[] tmp = (int[])a;
                for(int i=0;i<tmp.length;i++)
                    _M[ctr++] = tmp[i];
            }
            else
                throw new RuntimeException("Bad type for ivec2 constructor");
        }
        
        if( ctr != 2 )
            throw new RuntimeException("Bad number of arguments for ivec2 constructor");
        
        x=_M[0];
        y=_M[1];
        
    }

    public int[] toints(){
        return new int[]{x,y};
    }
    
    public ivec2 add(ivec2 o){
        return new ivec2(x+o.x,y+o.y);
    }
    public ivec2 sub(ivec2 o){
        return new ivec2(x-o.x,y-o.y);
    }
        
    public ivec2 mul(int d){
        return new ivec2(d*x,d*y);
    }
    
    public ivec2 neg(){
        return mul(-1);
    }
    
    public ivec2 xx(){ return new ivec2(x, x); }
    public ivec2 xy(){ return new ivec2(x, y); }
    public ivec2 yx(){ return new ivec2(y, x); }
    public ivec2 yy(){ return new ivec2(y, y); }
    
    
}