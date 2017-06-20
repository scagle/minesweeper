#define WIDTH 30
#define HEIGHT 20 

#define HIDDEN '#'
#define BOMB 'X'
#define CURSOR '0'
#define FLAG 'F'

#define BOMBS 25 

#include <stdlib.h>
#include <time.h>
#include <string>
#include <ncurses.h>

using namespace std;

bool hasWon, hasLost, initialized;
int boardArray [WIDTH] [HEIGHT];
bool discoveredArray [WIDTH] [HEIGHT];
bool flagArray [WIDTH] [HEIGHT];
int unused [WIDTH] [HEIGHT];        // only referenced once in setup_board
int cursorX, cursorY = 0;                   
int boardX = 50 - (WIDTH / 2);     // centers the board as much as possible
int boardY = 2;

void setup_board()
{
    initialized = true;
    if (BOMBS > WIDTH * HEIGHT - 9)
    {
        printw("Too many bombs, not enough space");
        getch();
        exit(0);
    }
    srand(time(NULL));
    ///////////// makes sure blocks around cursor are free ////////////////
    unused[cursorX][cursorY]++;
    if (cursorX > 0)
    {
        unused[cursorX-1][cursorY]++;
        if (cursorY > 0)
            unused[cursorX-1][cursorY-1]++;
        if (cursorY < HEIGHT-1)
            unused[cursorX-1][cursorY+1]++;
    }
    if (cursorX < WIDTH-1)
    {
        unused[cursorX+1][cursorY]++;
        if (cursorY > 0)
            unused[cursorX+1][cursorY-1]++;
        if (cursorY < HEIGHT-1)
            unused[cursorX+1][cursorY+1]++;
    }
    if (cursorY > 0)
        unused[cursorX][cursorY-1]++;
    if (cursorY < HEIGHT-1)
        unused[cursorX][cursorY+1]++;
    ///////////////////////////////////////////////////////////////////////
    int bombsleft = BOMBS;
    while (bombsleft > 0)
    {
        int x = rand() % WIDTH;
        int y = rand() % HEIGHT;
        if (unused[x][y] == 0)
        {
            unused[x][y] = 2;
            bombsleft--;
        }
    }
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            if (unused[x][y] == 2)
            {
                boardArray[x][y] = 100;
                if (x > 0)
                {
                    boardArray[x-1][y]++;
                    if (y > 0)
                        boardArray[x-1][y-1]++;
                    if (y < HEIGHT-1)
                        boardArray[x-1][y+1]++;
                }
                if (x < WIDTH-1)
                {
                    boardArray[x+1][y]++;
                    if (y > 0)
                        boardArray[x+1][y-1]++;
                    if (y < HEIGHT-1)
                        boardArray[x+1][y+1]++;
                }
                if (y > 0)
                    boardArray[x][y-1]++;
                if (y < HEIGHT-1)
                    boardArray[x][y+1]++;
            }
        }
    }

}
bool draw_board(WINDOW *gameWindow)
{
    bool gameover = true; //needs to be disproven every loop
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            char character = ' ';
            if (flagArray[x][y])
            {
                character = FLAG;
            }
            else if (discoveredArray[x][y])
            {
                if (boardArray[x][y] >= 100)
                {
                    character = BOMB;
                }
                else
                {
                    if (boardArray[x][y] == 0)
                        character = ' ';
                    else
                        character = '0' + boardArray[x][y];
                }
            }
            else
            {
                if (boardArray[x][y] < 100)
                    gameover = false; // if we get here atleast once, the game is not over.
                character = HIDDEN;
            }
            if ((cursorX == x) && (cursorY == y))
            {
                character = CURSOR;
            }
            if (character == FLAG)
                wattron(gameWindow, COLOR_PAIR(5));
            else if (character == BOMB)
                wattron(gameWindow, COLOR_PAIR(7));
            else if (character == HIDDEN)
                wattron(gameWindow, COLOR_PAIR(6));
            else if (character >= '4')
                wattron(gameWindow, COLOR_PAIR(4));
            else if (character == '3')
                wattron(gameWindow, COLOR_PAIR(3));
            else if (character == '2')
                wattron(gameWindow, COLOR_PAIR(2));
            else if (character == '1')
                wattron(gameWindow, COLOR_PAIR(1));
            else if (character == CURSOR)
                wattron(gameWindow, COLOR_PAIR(8));
            else
                wattron(gameWindow, COLOR_PAIR(9));
            waddch(gameWindow, character);
            wattron(gameWindow, COLOR_PAIR(9));
        }
//        wprintw(gameWindow, "\n");
    }
    if (gameover && !hasLost && !hasWon)
    {
        // Gotta test to see if it really is over yet.
        for (int y = 0; y < HEIGHT; y++)
        {
            for (int x = 0; x < WIDTH; x++)
            {
                if (flagArray[x][y] == 1 && boardArray[x][y] < 100)
                    gameover = false; // If there's a flag on a non-bomb spot then continue on
            }
        }
    }
    return gameover;
}
void display_board(WINDOW *gameWindow)
{
    bool gameover = draw_board(gameWindow); // draws board and determines if game's over or not
    if (gameover && !hasWon && !hasLost)
    {
        hasWon = true;
        // when you win, flag all the bombs / refresh the window
        for (int y = 0; y < HEIGHT; y++)
        {
            for (int x = 0; x < WIDTH; x++)
            {
                if (boardArray[x][y] >= 100) // if tile is a bomb, then flag when game is over
                {
                    flagArray[x][y] = 1;               // flag all bombs
                    wclear(gameWindow);           // clears game window
                    display_board(gameWindow);     // refresh board once again
                }
            }
        }
    }
}
void recurse(int x, int y)
{
    if (flagArray[x][y])   // don't count tiles with flags on them (exploitable)
    {
        return;
    }
    discoveredArray[x][y] = 1;
    if (boardArray[cursorX][cursorY] >= 100) 
    {
        hasLost = true;
        return;
    }
    if (boardArray[x][y] == 0 && !flagArray[x][y])
    {
        // Checks to see if tiles nearby are not flags / bombs
        bool upIsEmpty =        (boardArray[x][y-1] <= 8) && (discoveredArray[x][y-1] == 0);
        bool leftIsEmpty =      (boardArray[x-1][y] <= 8) && (discoveredArray[x-1][y] == 0);
        bool rightIsEmpty =     (boardArray[x+1][y] <= 8) && (discoveredArray[x+1][y] == 0);
        bool downIsEmpty =      (boardArray[x][y+1] <= 8) && (discoveredArray[x][y+1] == 0);
        bool upleftIsEmpty =    (boardArray[x-1][y-1] <= 8) && (discoveredArray[x-1][y-1] == 0);
        bool uprightIsEmpty =   (boardArray[x+1][y-1] <= 8) && (discoveredArray[x+1][y-1] == 0);
        bool downleftIsEmpty =  (boardArray[x-1][y+1] <= 8) && (discoveredArray[x-1][y+1] == 0);
        bool downrightIsEmpty = (boardArray[x+1][y+1] <= 8) && (discoveredArray[x+1][y+1] == 0);

        if ((y > 0) && upIsEmpty) // continues up
            recurse(x, y-1);
        if ((x > 0) && leftIsEmpty) // continues left
            recurse(x-1, y);
        if ((x < WIDTH-1) && rightIsEmpty) // continues right
            recurse(x+1, y);
        if ((y < HEIGHT-1) && downIsEmpty) // continues down
            recurse(x, y+1);
/*
        if ((y > 0) && (x > 0) && upleftIsEmpty) //upleft
            recurse(x-1, y-1);
        if ((y > 0) && (x < WIDTH-1) && uprightIsEmpty) //upright
            recurse(x+1, y-1);
        if ((y < HEIGHT-1) && (x > 0) && downleftIsEmpty) //downleft
            recurse(x-1, y+1);
        if ((y < HEIGHT-1) && (x < WIDTH-1) && downrightIsEmpty) //downright
            recurse(x+1, y+1);
*/
    }
}

void press()
{
    recurse(cursorX, cursorY);
}

void place_flag()
{
    int discovered = discoveredArray[cursorX][cursorY];

    if (discovered == 0)
    {
        if (flagArray[cursorX][cursorY] == 0)
        {
            flagArray[cursorX][cursorY] = 1;
        }
        else
        {
            flagArray[cursorX][cursorY] = 0;
        }
    }
}
void reset() // Resets all variables for next match
{
    initialized = false;
    hasWon = false;
    hasLost = false;
    cursorX = 0;
    cursorY = 0;

    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            boardArray[x][y] = 0;
            discoveredArray[x][y] = 0;
            flagArray[x][y] = 0;
            unused[x][y] = 0;
        }
    }
}

void process_input(char keyInputChar)
{
    switch(keyInputChar)
    {
        case 'w' : 
            {
                if (cursorY > 0)
                    cursorY--;
                break;
            }
        case 'a' : 
            {
                if (cursorX > 0)
                    cursorX--;
                break;
            }
        case 's' : 
            {
                if (cursorY < HEIGHT-1)
                    cursorY++;
                break;
            }
        case 'd' : 
            {
                if (cursorX < WIDTH-1)
                    cursorX++;
                break;
            }
        case 'p' :      // p = Press / Click
            {
                if (!hasWon && !hasLost)
                {
                    if (!initialized)
                        setup_board();
                    press();
                }
                break;
            }
        case 'o' :      // o = Flag (name doesn't make but it's convienient on fingers)
            {
                if (!hasWon && !hasLost)
                    place_flag();
                break;
            }
        case 'r' :
                reset();
    }
}
WINDOW* make_description()
{
    WINDOW *descriptionWindow = newwin(10, WIDTH, boardY + HEIGHT, boardX);
    wprintw(descriptionWindow, "\n'wasd' to move your cursor\n'p' to reveal/press\n'o' to flag\n'r' to reset\n'p' to quit\n");
    if (hasWon)                    // If you win then show win message
    {
        wattron(descriptionWindow, COLOR_PAIR(5));
        wprintw(descriptionWindow, "YOU WON!\n");
    }
    else if (hasLost)              // Otherwise if you lose then show lose message
    {
        wattron(descriptionWindow, COLOR_PAIR(7));
        wprintw(descriptionWindow, "Awww bummer looks like you hit a mine!\n");
    }
    return descriptionWindow;
}

int init()
{
    // A bunch of initialization for ncurses
    initscr();          // initializes stdscr
    cbreak();           // allows directional keys to be recorded
    noecho();           // stops echo
    start_color();      // initializes colors
    init_pair(1, COLOR_BLUE, COLOR_BLACK);      // 1
    init_pair(2, COLOR_GREEN, COLOR_BLACK);     // 2
    init_pair(3, COLOR_RED, COLOR_BLACK);       // 3
    init_pair(4, COLOR_MAGENTA, COLOR_BLACK);   // 4+
    init_pair(5, COLOR_BLACK, COLOR_GREEN);     // FLAG
    init_pair(6, COLOR_BLACK, COLOR_WHITE);     // HIDDEN
    init_pair(7, COLOR_WHITE, COLOR_RED);       // BOMB
    init_pair(8, COLOR_MAGENTA, COLOR_BLACK);   // CURSOR
    init_pair(9, COLOR_WHITE, COLOR_BLACK);     // Default
}

int main()
{
    init();
    char keyInputChar;
    while (keyInputChar != 'q')
    {
        clear();
        WINDOW *gameWindow = newwin(HEIGHT, WIDTH, boardY, boardX);
        WINDOW *descriptionWindow = make_description();
        display_board(gameWindow);
        refresh();      // refreshes/flips/draws the 'stdscr' (standard screen)
        wrefresh(gameWindow);  // refreshes/flips/draws the game screen
        wrefresh(descriptionWindow);
        keyInputChar = getch();
        process_input(keyInputChar);
    }
    clear();
    endwin();
    return 0;
}
