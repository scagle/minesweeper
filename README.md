# minesweeper
minesweeper - An attempt to recreate the classic minesweeper game in the terminal with C++
* (Made for a class assignment in early 2017)

## DESCRIPTION

An attempt to recreate the classic minesweeper game in the terminal. Uses ncurses to
draw the game, and to get input from the player. Uses recursion to find out which 
adjacent tiles should be revealed based on the number of adjacent bombs.

## GETTING STARTED
(NOTE: You must link the ncurses library when compiling)
```
g++ -o minesweeper.out minesweeper.cpp -lncurses
```

### DEPENDENCIES
* C++
* This requires the ncurses library to render the game in the terminal
* Supported for Ubuntu Linux, will probably work for others

### CONTROLS
```
Explained in game
```
