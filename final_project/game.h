#ifndef __GAME_H__
#define __GAME_H__

#include <iostream>
#include <string.h>
#include <sys/time.h>
#include <signal.h>
#include <pthread.h>

#include <sys/sem.h>
#define SEM_KEY 310512009
#define SEM_MODE 0666

#include "socket_utils.h"
#include "data_utils.h"
#include "random_map.h"


#define ROUND_INTERVAL 10

#define CARD_STATE_UP 0
#define CARD_STATE_DOWN 1

#define SERVER_PORT 8898


struct Player{
	int connfd;
	int score;
};


class Game{
private:
	static void* thread_handler(void*);
	static void timer_handler(int);
	int handleInit();
	int handlePlaying();
	void broadcastToPlayers();
	struct sigaction sa;
	struct itimerval game_timer;
	int quit; 			// boolean to keep game loop running
	int round_cnt;
	int gameOver;
	int gameState;  	// to keep track of the game's state (0: menu, 1: game)
	int sockfd;
	bool secondrcv[NUM_PLAYERS];
	bool newround;
	bool newsection;
	bool hide_and_play;
	char cards[NUM_CARDS];
	char cards_empty[NUM_CARDS];
	struct Player players[NUM_PLAYERS];

	int sem;
	int P(int s);
	int V(int s);

public:
	Game();
	~Game();
	void gameLoop();
	void gameStop();
};

static Game myGame;



#endif
