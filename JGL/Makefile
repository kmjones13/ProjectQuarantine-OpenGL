


all: JGL.so 

JGL.class: parsegl.py
	python3 parsegl.py
	javac -classpath .. JGL.java
	(cd .. && javah -d JGL JGL.JGL)
	
JGL.so: JGL.class
	$(CC) -g -Wall -Wno-int-to-pointer-cast -Wno-pointer-sign -fPIC -shared -I/usr/lib/jvm/java/include -I/usr/lib/jvm/java/include/linux JGL.c -o JGL.so  -ldl
	
	
	
clean:
	-/bin/rm *.obj *.aobj *.dll *.lib *.class *.exe  *.pyc *.c.exp  */*.obj  */*.dll */*.lib */*.exp *.so
