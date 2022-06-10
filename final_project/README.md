# eos_2022

# Memory Game (Multiple players PK)

## How to play this game?
1. Run the `server` on PC and `clents` on each PXA270
2. Wait for initialization (around 10 seconds)
3. Start game loop. Repeat step 4 and 5
4. Memorize each pair of patterns on the LCD (`Figure1`)
5. Hit the corresponding button to pair the patterns (one point for each pair) (`Figure2`)

## Mutual interactive
1. Every players have the same patterns' positions in same round.
2. If a pair is hit, then the positions for the pair will show on every players' LCD and can not get point from the pair again.

<table>
    <tr>
        <td>Figure1</td>
        <td>Figure2</td>
    </tr>
    <tr>
        <td><img src="https://imgur.com/NIR2Rnw.jpg" height="250"></td>
        <td><img src="https://imgur.com/2JuKeC7.jpg" height="250"> </td>
    </tr>
</table>

## Hardware
1. `1~N` PXA270 boards   
(To change the player number, check `data_utils.h` NUM_PLAYERS variable)  
(Default: 2 Players)
2. A server computer (default: Ubuntu20.04 system)

## Software
1. `client.cpp`: Main function to run the game on PXA270.
2. `data_utils.h`: Game variables for both client and server,  
  structure for messages send from server.
3. `game_client.cpp`: Functions for running client on PXA270  
  include: `readServer()`, `sendServer()`, `run()`, `read_pad()`, `show_LCD_pic()` and so on.
4. `game.cpp`: Server, to control how this game work.
5. `LCD.c`: Functions to visualize patterns or words on PXA270
6. `LCD.h`: Saved patterns for `Memory Game`, include 6 patterns and one back ground (cards all hidden).
7. `random_map.c`: Functions for generating the random patterns for the game.
8. `main.cpp`: Main function to run the server.

## Video
https://youtu.be/DSxPIYOiOls