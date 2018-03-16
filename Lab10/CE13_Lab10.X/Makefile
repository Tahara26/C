CFLAGS = -g -wall -std=c99

all:rpg

rpg.o:rpg.c Player.h Game.h
	gcc -c ${CFLAGS} rpg.c
	
Player.o:Player.c Player.h
	gcc -c ${CFLAGS} Player.c
	
Game.o:Game.c Game.h
	gcc -c ${CFLAGS} Game.c
	
rpg:rpg.o Player.o Game.o
	gcc -o rpg rpg.o Player.o Game.o
	
clean:
	rm rpg.o Player.o Game.o rpg
