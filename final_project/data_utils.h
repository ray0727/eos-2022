#ifndef __PKT_UTIL_H__
#define __PKT_UTIL_H__

#define NUM_CARDS 12
#define NUM_PLAYERS 2

#define ROUND_TIMES 8
#define GAME_STATE_INIT 0
#define GAME_STATE_PLAYING 1
#define GAME_STATE_END 2
#define SPACE 10

typedef struct
{
  int gameState;
  int score;
  bool newR;
  bool newS;// one round have two section, display section and hide_and_play section
  bool hide_p;
  int round_num;
  char card_states[NUM_CARDS];
} server_pkt;

#endif