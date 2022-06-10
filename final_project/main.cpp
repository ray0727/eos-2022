#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <signal.h>
#include <iostream>

#include "game.h"

using namespace std;

void intHandler(int);

int main(int argc, char *argv[])
{
	// play
	struct sigaction sa;

  	memset(&sa, 0, sizeof(sa));
  	sa.sa_handler = intHandler;

	sigaction(SIGINT, &sa, NULL);
	myGame.gameLoop();

	return 0;
}

void intHandler(int signum){
	printf("catch SIGING\n");
	myGame.gameStop();
}