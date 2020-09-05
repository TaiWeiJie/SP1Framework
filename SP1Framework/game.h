#ifndef _GAME_H
#define _GAME_H

#include "Framework\timer.h"

extern CStopWatch g_swTimer;
extern bool g_bQuitGame;
//extern bool openguide;  may be useless

// struct to store keyboard events
// a small subset of KEY_EVENT_RECORD
struct SKeyEvent
{
    bool keyDown;
    bool keyReleased;
};

// struct to store mouse events
// a small subset of MOUSE_EVENT_RECORD
struct SMouseEvent
{
    COORD mousePosition;
    DWORD buttonState;
    DWORD eventFlags;
};

// Enumeration to store the control keys that your game will have
enum EKEYS
{
    K_UP,
    K_DOWN,
    K_LEFT,
    K_RIGHT,
    K_ESCAPE,
    K_SPACE,
    K_COUNT
};

enum TERRAIN
{
    GRASS = 32,
    ROCK = 132,
};


// Enumeration for the different screen states
enum EGAMESTATES
{
    S_SPLASHSCREEN,
    S_LOSESCREEN,
    S_WINSCREEN,
    S_PESTCONTROLSCREEN,
    S_KILLSPIDERSCREEN,
    S_MENU,
    S_GUIDE,
    S_GAME,
    S_GAME2,
    S_GAME3,
    S_COUNT,
   
};

// struct for the game character
struct SGameChar
{
    COORD m_cLocation;
    bool  m_bActive;
    bool powered;
};

struct SGameCrop
{
    COORD m_cLocation;
    bool  m_bActive;

};

struct SGameSpider
{
    COORD m_cLocation;
    bool  m_bActive;

};

struct DISPLAY
{
    bool lvl1;
    bool lvl2;
    bool lvl3;
};

struct WINSTATE
{
    bool lvl1;
    bool lvl2;
    bool lvl3;
};

struct DIRECTION
{
    bool up;
    bool down;
    bool left;
    bool right;
};

void init(void); // initialize your variables, allocate memory, etc
void getInput(void); // get input from player
void update(double dt); // update the game and the state of the game
void render(void); // renders the current state of the game to the console
void shutdown(void); // do clean up, free memory
void splashScreenWait(); // waits for time to pass in splash screen
void updateGame(); // gameplay logic
void spiderMovement();
//void MoveInput();
void moveCharacter(); // moves the character, collision detection, physics, etc
void processUserInput(); // checks if you should change states or do something else with the game, e.g.pause, exit
void clearScreen(); // clears the current screen and draw from scratch
void renderSplashScreen(); // renders the splash screen
void renderGame(); // renders the game stuff
void renderMap(); // renders the map to the buffer first
void renderCharacter(); // renders the character into the buffer
void renderTimer();
//void PlayerLimiter();
void renderCrops();
void renderSpiders();
void renderPowerups();
void renderToScreen(); // dump the contents of the buffer to the screen, one frame worth of game
void renderInputEvents(); // renders the status of input events
void updateguide(); // Updates guide
void updatemenu(); // Updates Menu
void updateLosingscreen();
void updateWinscreen();
void updateKillspiderscreen();
// void updatePrintscreen();
void UpdateSpiders();
void UpdateCrops();
void UpdatePowerups();
void UpdatePestcontroltimer();
void renderguide(); // renders How To Play Screen
void GuideInput(); // Check for inputs in How To Play screen
void WinInput();
void LoseInput();
void Killspiderinput();
void PestControlinput();
void updatePestcontrol();
void renderLosingscreen();
void renderWinscreen();
void renderKillspiderscreen();
void renderPestscreen();
void rendermenu(); // Renders the menu screen
void MenuInput(); // Check for inputs in main menu



// keyboard and mouse input event managers
void keyboardHandler(const KEY_EVENT_RECORD& keyboardEvent);  // define this function for the console to call when there are keyboard events
void mouseHandler(const MOUSE_EVENT_RECORD& mouseEvent);      // define this function for the console to call when there are mouse events

void gameplayKBHandler(const KEY_EVENT_RECORD& keyboardEvent);   // handles keyboard events for gameplay 
void gameplayMouseHandler(const MOUSE_EVENT_RECORD& mouseEvent); // handles mouse events for gameplay 


#endif // _GAME_H