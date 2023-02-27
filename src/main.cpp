#include "Window.h"

int main(int argc, char* argv[])
{
	Window window;
	while (!window.isClosed()) {
		window.pollEvents();
	}
	return 0;
}