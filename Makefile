main:
	g++ src/main.cpp src/Window.cpp src/Game.cpp src/BoardState.cpp src/Piece.cpp -std=c++17 -lSDL2main -lSDL2 -lSDL2_image -o compiled
