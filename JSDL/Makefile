


all:
	python3 parsesdl.py
	javac -classpath .. JSDL.java
	(cd .. && javah -d JSDL JSDL.JSDL)
	$(CC) -fPIC -shared -I/usr/include/SDL2 -I/usr/lib/jvm/java/include -I/usr/lib/jvm/java/include/linux JSDL.c -o JSDL.so  -ldl -lSDL2
	
		
	
clean:
	-/bin/rm *.obj *.aobj *.dll *.lib *.class *.exe  *.pyc *.c.exp  */*.obj  */*.dll */*.lib */*.exp *.so
