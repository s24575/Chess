#include "Window.h"

#undef main

int main(int argc, char* args[])
{
	Window window("Chess", 640, 640);
	while (!window.isClosed()) {
		window.pollEvents();
	}
	return 0;
}