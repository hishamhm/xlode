CC=gcc

#SDL_FLIC=-lSDL_flic
#SDL_FLIC=SDL_flic-1.2/SDL_flic.c -ISDL_flic-1.2 `pkg-config sdl --cflags` -DINLINE=inline

xlode: xlode.c
	$(CC) -o xlode -O2 xlode.c -lSDL -lSDL_mixer

debug: xlode.c
	$(CC) -o xlode -g xlode.c -lSDL -lSDL_mixer

clean:
	rm -f core *.o *.gch xlode
