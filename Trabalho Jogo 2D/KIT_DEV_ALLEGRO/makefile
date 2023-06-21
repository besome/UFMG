PASTINHA_CONJUNTA=../
CC = $(PASTINHA_CONJUNTA)/MINGW/bin/gcc
PATH_ALLEGRO=$(PASTINHA_CONJUNTA)/allegro-5.0.10-mingw-4.7.0/include
PATH_ALLEGRO5=$(PATH_ALLEGRO)/allegro5
LIBS=-lallegro-5.0.10-monolith-mt

all: DancingPlates.exe
	
DancingPlates.exe: DancingPlates.o
	$(CC) -O2 -o DancingPlates.exe DancingPlates.o -L $(PASTINHA_CONJUNTA)/allegro-5.0.10-mingw-4.7.0/lib $(LIBS)

DancingPlates.o: DancingPlates.c
	$(CC) -O2 -I $(PATH_ALLEGRO) -I $(PATH_ALLEGRO5) -c DancingPlates.c
	
clean:
	del DancingPlates.o
	del DancingPlates.exe
