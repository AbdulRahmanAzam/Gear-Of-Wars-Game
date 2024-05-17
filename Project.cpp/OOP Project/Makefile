all : compile link run 

compile: 
	g++ -IC:\SFML-2.6.1\include -c main.cpp -o main.o

link:
	g++ -LC:\SFML-2.6.1\lib main.o -o main.exe -lmingw32 -lsfml-audio -lsfml-graphics -lsfml-window -lsfml-system -lsfml-main -mwindows

run:
	.\main
	
# clean:
# erase -f main *.o
	