all:
	g++ -std=c++17 -pthread -I src/include/SDL2 -L src/lib -o snake new.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer
	./snake
	
	g++ -std=c++17 -pthread -I src/include/SDL2 -L src/lib -o snake level1.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer
	./snake

	g++ -std=c++17 -pthread -I src/include/SDL2 -L src/lib -o snake game.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer
	./snake
	
