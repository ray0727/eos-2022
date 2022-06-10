#include "game.h"

Game::Game()
{
  printf("constructor\n");
  quit = 0;
  for(int i=0;i<NUM_PLAYERS;i++){
    myGame.secondrcv[i] = false;
  }
  myGame.newround = false;
  myGame.newsection = false;
  myGame.hide_and_play = true;
  for (int i = 0; i < NUM_CARDS; i++)
  {
    myGame.cards_empty[i] = 0;
    myGame.cards[i] = SPACE;
  }
  gameState = GAME_STATE_INIT; //0 menu 1 start
}

Game::~Game()
{

}

void Game::timer_handler(int signum)
{
  if(myGame.hide_and_play == false)//display section
  {
    myGame.hide_and_play = true;
    printf("hide and play\n");
    myGame.newsection = true;
    for (int i = 0; i < NUM_CARDS; i++)
    {
      myGame.cards_empty[i] = 0;
    }
  }
  else
  {
    printf("timer tick %d\n", myGame.round_cnt);
    if (myGame.round_cnt++ >= ROUND_TIMES)
    {
      myGame.gameOver = 1;
      setitimer(ITIMER_REAL, NULL, NULL);
    }
    for(int i=0;i<NUM_PLAYERS;i++){
      myGame.secondrcv[i] = false;
    }

    myGame.newround = true;
    // TODO: Update card states
    int temp[ROL_SIZE][COL_SIZE];
    Get_uniform_pattern(temp, ROL_SIZE, COL_SIZE, CAT_NUM);

    printf("card states: \n");
    for (int i = 0; i < ROL_SIZE; i++)
    {
      for(int j=0;j<COL_SIZE;j++)
      {
        myGame.cards[i*COL_SIZE+j] = temp[i][j];
        printf("%d ", temp[i][j]);
      } 
      printf("\n");
    }
    printf("\n");
    
    myGame.hide_and_play = false;
  }
  myGame.broadcastToPlayers();
}

void Game::gameLoop()
{
  while (!myGame.quit)
  {
    switch (myGame.gameState)
    {
    case GAME_STATE_INIT:
      myGame.gameState = handleInit();
      break;
    case GAME_STATE_PLAYING:
      myGame.gameState = handlePlaying();
      break;
    case GAME_STATE_END:
      myGame.gameState = GAME_STATE_INIT;
      break;
    default:
      break;
    }
    myGame.broadcastToPlayers();
  }
}

void Game::gameStop(){
  myGame.gameOver = true;
  myGame.quit = true;
}

void Game::broadcastToPlayers()
{
  // printf("player1\n");
  // printf("score: %d\n", players[0].score);
  // printf("player2\n");
  // printf("score: %d\n", players[1].score);

  server_pkt pkt;
  if(myGame.hide_and_play == false)
  {
    memcpy(pkt.card_states, cards, sizeof(cards));
  }
  else
  {
    memcpy(pkt.card_states, cards_empty, sizeof(cards_empty));
  }
  pkt.gameState = gameState;
  pkt.round_num = myGame.round_cnt;
  pkt.hide_p = myGame.hide_and_play;
  pkt.newR = myGame.newround;
  myGame.newround = false;
  pkt.newS = myGame.newsection;
  myGame.newsection = false;
  for (int i = 0; i < NUM_PLAYERS; i++)
  {
    if(fcntl(players[i].connfd, F_GETFD) == -1) continue;
    printf("writing to connfd = %d\n", players[i].connfd);
    pkt.score = players[i].score;
    write(players[i].connfd, &pkt, sizeof(pkt));
  }
}

int Game::handleInit()
{
  // Accept client connections
  int i;
  printf("binding port %d\n", SERVER_PORT);
  if(fcntl(sockfd, F_GETFD) != -1){
    close(sockfd);
  }
  sockfd = createServerSock(SERVER_PORT, TRANSPORT_TYPE_TCP);
  printf("waiting for connections...\n");
  //connect
  for (i = 0; i < NUM_PLAYERS && !this->quit; i++)
  {
    // TODO
    struct sockaddr_in cln_addr;
    socklen_t sLen = sizeof(cln_addr);
    int connfd = accept(sockfd, (struct sockaddr *)&cln_addr, &sLen);

    if (connfd == -1)
    {
      perror("Error: accept()");
    }
    myGame.players[i].connfd = connfd;
    myGame.players->score = 0;
    printf("accept connection from: %d, connfd = %d\n", cln_addr.sin_addr, myGame.players[i].connfd);
  }
  return GAME_STATE_PLAYING;
}

int Game::handlePlaying()
{
  // Setup Game environment: Game timer, card States, Player Threads
  myGame.sem = semget(SEM_KEY, 1, IPC_CREAT | IPC_EXCL | SEM_MODE);
  // sem = semget(SEM_KEY, 1, IPC_CREAT | SEM_MODE);
  if (myGame.sem < 0)
  {
    perror("Error create semaphore\n");
    semctl(myGame.sem, 0, IPC_RMID, 0);
    exit(-1);
  }
  printf("sem created\n");
  if (semctl(myGame.sem, 0, SETVAL, 1) < 0)
  {
    perror("Error semctl\n");
    exit(-1);
  }

  myGame.round_cnt = 0;
  //timer start
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = &this->timer_handler;
  sigaction(SIGALRM, &sa, NULL);

  game_timer.it_value.tv_sec = ROUND_INTERVAL;
  game_timer.it_value.tv_usec = 0;

  game_timer.it_interval.tv_sec = ROUND_INTERVAL;
  game_timer.it_interval.tv_usec = 0;
  setitimer(ITIMER_REAL, &game_timer, NULL);

  for (int i = 0; i < NUM_PLAYERS; i++)
  {
    pthread_t thread;

    printf("thread> id = %d\n", i);
    printf("thread> connfd = %d\n", myGame.players[i].connfd);
    if (pthread_create(&thread, NULL, (void *(*)(void *)) & (Game::thread_handler), (void *)i))
    {
      perror("Error: pthread_create()\n");
    }
  }
  while (!myGame.gameOver)
  {
  }

  printf("remove sem\n");
  if (semctl(myGame.sem, 0, IPC_RMID, 0) < 0)
  {
    perror("Error removing sem\n");
    exit(-1);
  }
  close(sockfd);
  for(int i=0; i<NUM_PLAYERS; i++){
    close(myGame.players[i].connfd);
  }

  return GAME_STATE_END;
}

void *Game::thread_handler(void *arg)
{
  int index = *((int *)(&arg));
  char msg[20];
  int key, key2;
  printf("id = %d\n", index);
  printf("conndf = %d\n", myGame.players[index].connfd);
  while (!myGame.gameOver)
  {
    int n;
    if ((n = read(myGame.players[index].connfd, msg, 20)) == 0)
    {
      printf("Connection closed\n");
      close(myGame.players[index].connfd);
      return NULL;
    }
    if (msg[n - 1] == '\n')
      msg[n - 1] = '\0';
    else
      msg[n] = '\0';
    printf("> %s\n", msg);
    std::cout << msg << std::endl;
    // key = msg[0] - '1';
    // Check key and hit cards
    if(myGame.hide_and_play == true)
    {
      myGame.P(myGame.sem);
      if(myGame.secondrcv[index]==false){//first rcv
          if((msg[0] > '0') && (msg[0] <= '9'))
              key = msg[0]-'1';
          else if(msg[0] == '*')
              key = 9;
          else if(msg[0] == '0')
              key = 10;
          else if(msg[0] == '#')
              key = 11;
          myGame.secondrcv[index] = true;
      }
      else{
          if((msg[0] > '0') && (msg[0] <= '9'))
              key2 = msg[0]-'1';
          else if(msg[0] == '*')
              key2 = 9;
          else if(msg[0] == '0')
              key2 = 10;
          else if(msg[0] == '#')
              key2 = 11;
        if (myGame.cards[key] && myGame.cards[key2] && key!=key2 && (myGame.cards[key] == myGame.cards[key2]))
        {
          myGame.players[index].score++;
          myGame.cards_empty[key] = myGame.cards[key];
          myGame.cards_empty[key2] = myGame.cards[key2];
          myGame.cards[key] = 0;
          myGame.cards[key2] = 0;
        }
        myGame.secondrcv[index] = false;
      }
      usleep(200000);
      myGame.V(myGame.sem);
    }
    myGame.broadcastToPlayers();
  }
  close(myGame.players[index].connfd);
  return NULL;
}

int Game::P(int s)
{
  struct sembuf sop;
  sop.sem_num = 0;
  sop.sem_op = -1;
  sop.sem_flg = 0;

  if (semop(s, &sop, 1) < 0)
  {
    perror("P(): sem failed\n");
    return -1;
  }
  else
  {
    return 0;
  }
}

int Game::V(int s)
{
  struct sembuf sop;
  sop.sem_num = 0;
  sop.sem_op = 1;
  sop.sem_flg = 0;

  if (semop(s, &sop, 1) < 0)
  {
    perror("V(): sem failed\n");
    return -1;
  }
  else
  {
    return 0;
  }
}
