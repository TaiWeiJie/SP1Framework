// This is the main file for the game logicand function
//
//
#include "game.h"
#include "Framework\console.h"
#include <iostream>
#include <iomanip>
#include <sstream>

#define VK_W 0x57
#define VK_A 0x41
#define VK_S 0x53
#define VK_D 0x44

double  g_dElapsedTime;
double  g_dDeltaTime;
double EnemyUpdateRate = 0.0;
SKeyEvent g_skKeyEvent[K_COUNT];
SMouseEvent g_mouseEvent;

// Game specific variables here
SObject   g_sChar;
SObject   g_sCrops[20];
SObject g_sSpiders[15];
SObject g_sPowerups[3];
bool powered;

//DIRECTION   direction;
EGAMESTATES g_eGameState = S_MENU; // initial state

// Console object
Console g_Console(80, 25, "SP1 Framework");
int limiter = 0;

//--------------------------------------------------------------
// Purpose  : Initialisation function
//            Initialize variables, allocate memory, load data from file, etc. 
//            This is called once before entering into your main loop
// Input    : void
// Output   : void
//--------------------------------------------------------------
void init(void)
{
    // Set precision for floating point output
    g_dElapsedTime = 0.0;

    // sets the initial state for the game
    g_eGameState = S_MENU;

    g_sChar.m_cLocation.X = g_Console.getConsoleSize().X / 2;
    g_sChar.m_cLocation.Y = g_Console.getConsoleSize().Y / 2;
    g_sChar.m_bActive = true;

    //sets the width, height and the font name to use in the console
    g_Console.setConsoleFont(0, 16, L"Consolas");

    // remember to set your keyboard handler, so that your functions can be notified of input events
    g_Console.setKeyboardHandler(keyboardHandler);
    g_Console.setMouseHandler(mouseHandler);

    powered = false;
}

//--------------------------------------------------------------
// Purpose  : Reset before exiting the program
//            Do your clean up of memory here
//            This is called once just before the game exits
// Input    : Void
// Output   : void
//--------------------------------------------------------------
void shutdown(void)
{
    // Reset to white text on black background
    colour(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);

    g_Console.clearBuffer();
}

//--------------------------------------------------------------
// Purpose  : Get all the console input events
//            This function sets up the keyboard and mouse input from the console.
//            We will need to reset all the keyboard status, because no events will be sent if no keys are pressed.
//
//            Remember to set the handlers for keyboard and mouse events.
//            The function prototype of the handler is a function that takes in a const reference to the event struct
//            and returns nothing. 
//            void pfKeyboardHandler(const KEY_EVENT_RECORD&);
//            void pfMouseHandlerconst MOUSE_EVENT_RECORD&);
// Input    : Void
// Output   : void
//--------------------------------------------------------------
void getInput(void)
{
    // resets all the keyboard events
    memset(g_skKeyEvent, 0, K_COUNT * sizeof(*g_skKeyEvent));
    // then call the console to detect input from user
    g_Console.readConsoleInput();
}  

//--------------------------------------------------------------
// Purpose  : This is the handler for the keyboard input. Whenever there is a keyboard event, this function will be called.
//            Ideally, you should pass the key event to a specific function to handle the event.
//            This is because in some states, some keys would be disabled. Hence, to reduce your code complexity, 
//            it would be wise to split your keyboard input handlers separately.
//            
//            The KEY_EVENT_RECORD struct has more attributes that you can use, if you are adventurous enough.
//
//            In this case, we are not handling any keyboard event in the Splashscreen state
//            
// Input    : const KEY_EVENT_RECORD& keyboardEvent - reference to a key event struct
// Output   : void
//--------------------------------------------------------------
void keyboardHandler(const KEY_EVENT_RECORD & keyboardEvent)
{
    switch (g_eGameState)
    {
    case S_SPLASHSCREEN: // don't handle anything for the splash screen
        break;
    case S_GAME: gameplayKBHandler(keyboardEvent); // handle gameplay keyboard event 
        break;
    }
}

//--------------------------------------------------------------
// Purpose  : This is the handler for the mouse input. Whenever there is a mouse event, this function will be called.
//            Ideally, you should pass the key event to a specific function to handle the event.
//            This is because in some states, some keys would be disabled. Hence, to reduce your code complexity, 
//            it would be wise to split your keyboard input handlers separately.
//            
//            For the mouse event, if the mouse is not moved, no event will be sent, hence you should not reset the mouse status.
//            However, if the mouse goes out of the window, you would not be able to know either. 
//
//            The MOUSE_EVENT_RECORD struct has more attributes that you can use, if you are adventurous enough.
//
//            In this case, we are not handling any mouse event in the Splashscreen state
//            
// Input    : const MOUSE_EVENT_RECORD& mouseEvent - reference to a mouse event struct
// Output   : void
//--------------------------------------------------------------
void mouseHandler(const MOUSE_EVENT_RECORD & mouseEvent)
{
    switch (g_eGameState)
    {
    case S_SPLASHSCREEN: // don't handle anything for the splash screen
        break;
    case S_GAME: gameplayMouseHandler(mouseEvent); // handle gameplay mouse event
        break;
    case S_GUIDE: gameplayMouseHandler(mouseEvent);
        break;
    case S_MENU: gameplayMouseHandler(mouseEvent);
        break;
    case S_LOSESCREEN: gameplayMouseHandler(mouseEvent);
        break;
    case S_WINSCREEN: gameplayMouseHandler(mouseEvent);
        break;
    case S_PESTCONTROLSCREEN: gameplayMouseHandler(mouseEvent);
        break;
    case S_DECIMATIONSCREEN: gameplayMouseHandler(mouseEvent);
        break;
    }
}

//--------------------------------------------------------------
// Purpose  : This is the keyboard handler in the game state. Whenever there is a keyboard event in the game state, this function will be called.
//            
//            Add more keys to the enum in game.h if you need to detect more keys
//            To get other VK key defines, right click on the VK define (e.g. VK_UP) and choose "Go To Definition" 
//            For Alphanumeric keys, the values are their ascii values (uppercase).
// Input    : const KEY_EVENT_RECORD& keyboardEvent
// Output   : void
//--------------------------------------------------------------
void gameplayKBHandler(const KEY_EVENT_RECORD & keyboardEvent)
{
    // here, we map the key to our enums
    EKEYS key = K_COUNT;
    switch (keyboardEvent.wVirtualKeyCode)
    {
    case VK_UP: key = K_UP; break;
    case VK_DOWN: key = K_DOWN; break;
    case VK_LEFT: key = K_LEFT; break;
    case VK_RIGHT: key = K_RIGHT; break;
    case VK_SPACE: key = K_SPACE; break;
    case VK_ESCAPE: key = K_ESCAPE; break;
    case VK_W: key = K_UP; break;
    case VK_A: key = K_LEFT; break;
    case VK_S: key = K_DOWN; break;
    case VK_D: key = K_RIGHT; break;
    }
    // a key pressed event would be one with bKeyDown == true
    // a key released event would be one with bKeyDown == false
    // if no key is pressed, no event would be fired.
    // so we are tracking if a key is either pressed, or released
    if (key != K_COUNT)
    {
        g_skKeyEvent[key].keyDown = keyboardEvent.bKeyDown;
        g_skKeyEvent[key].keyReleased = !keyboardEvent.bKeyDown;
    }
}

//--------------------------------------------------------------
// Purpose  : This is the mouse handler in the game state. Whenever there is a mouse event in the game state, this function will be called.
//            
//            If mouse clicks are detected, the corresponding bit for that mouse button will be set.
//            mouse wheel, 
//            
// Input    : const KEY_EVENT_RECORD& keyboardEvent
// Output   : void
//--------------------------------------------------------------
void gameplayMouseHandler(const MOUSE_EVENT_RECORD & mouseEvent)
{
    if (mouseEvent.dwEventFlags & MOUSE_MOVED) // update the mouse position if there are no events
    {
        g_mouseEvent.mousePosition = mouseEvent.dwMousePosition;
    }
    g_mouseEvent.buttonState = mouseEvent.dwButtonState;
    g_mouseEvent.eventFlags = mouseEvent.dwEventFlags;
}

//--------------------------------------------------------------
// Purpose  : Update function
//            This is the update function
//            double dt - This is the amount of time in seconds since the previous call was made
//
//            Game logic should be done here.
//            Such as collision checks, determining the position of your game characters, status updates, etc
//            If there are any calls to write to the console here, then you are doing it wrong.
//
//            If your game has multiple states, you should determine the current state, and call the relevant function here.
//
// Input    : dt = deltatime
// Output   : void
//--------------------------------------------------------------
void update(double dt)
{
    //g_dDeltaTime = dt;
    if (g_eGameState == S_GAME)
    {
        // get the delta time
        g_dElapsedTime += dt;
        g_dDeltaTime = dt;
    }
    else if (g_eGameState == S_MENU)
    {
        g_dElapsedTime = 0;
    }
    else if (g_eGameState == S_WINSCREEN)
    {
        g_dElapsedTime = 0;
    }
    else if (g_eGameState == S_LOSESCREEN)
    {
        g_dElapsedTime = 0;
    }
    else if (g_eGameState == S_PESTCONTROLSCREEN)
    {
        g_dElapsedTime = 0; 
    } 

    switch (g_eGameState)
    {
    case S_SPLASHSCREEN: splashScreenWait(); // game logic for the splash screen
        break;
    case S_GAME: updateGame(); // gameplay logic when we are in the game
        break;
    case S_GUIDE: updateguide(); // logic for how to play screen
        break;
    case S_MENU: updatemenu();   // logic for menu
        break;
    case S_LOSESCREEN: updateLosingscreen();
        break;
    case S_WINSCREEN: updateWinscreen();
        break;  
    case S_PESTCONTROLSCREEN: updatePestcontrol();
        break;
    case S_DECIMATIONSCREEN: updateDecimationscreen(); // later put above
break;

    }

}


void splashScreenWait()    // waits for time to pass in splash screen
{
    if (g_dElapsedTime > 3.0) // wait for 3 seconds to switch to game mode, else do nothing
        g_eGameState = S_MENU;

    //if(g_sChar.m_cLocation.X == g_sCrops.m_cLocation.X && g_sChar.m_cLocation.Y == g_sCrops.m_cLocation.Y)

}

void updateGame()       // gameplay logic
{
    processUserInput(); // checks if you should change states or do something else with the game, e.g. pause, exit
    moveCharacter();    // moves the character, collision detection, physics, etc
    UpdateCrops();
    UpdateSpiders();
    spiderMovement();                    // sound can be played here too.
    UpdatePestcontroltimer();
    UpdatePowerups();
}

void UpdateCrops()
{
    bool win;
    for (int i = 0; i < 20; i++)
    {
        if (g_sChar.m_cLocation.X == g_sCrops[i].m_cLocation.X &&
            g_sChar.m_cLocation.Y == g_sCrops[i].m_cLocation.Y && g_sCrops[i].m_bActive == true)
        {
            Beep(1440, 30);
            g_sCrops[i].m_bActive = false;
        }
    }

    if (g_sCrops[0].m_bActive == false && g_sCrops[1].m_bActive == false && g_sCrops[2].m_bActive == false && g_sCrops[3].m_bActive == false && g_sCrops[4].m_bActive == false
        && g_sCrops[5].m_bActive == false && g_sCrops[6].m_bActive == false && g_sCrops[7].m_bActive == false && g_sCrops[8].m_bActive == false && g_sCrops[9].m_bActive == false
        && g_sCrops[10].m_bActive == false && g_sCrops[11].m_bActive == false && g_sCrops[12].m_bActive == false && g_sCrops[13].m_bActive == false && g_sCrops[14].m_bActive == false
        && g_sCrops[15].m_bActive == false && g_sCrops[16].m_bActive == false && g_sCrops[17].m_bActive == false && g_sCrops[18].m_bActive == false && g_sCrops[19].m_bActive == false
        )
        g_eGameState = S_WINSCREEN;

    // characters location same with spider then spider = false AND spider must stop moving
      //for (int i = 0; i < 20; i++)
      //{
      //    if (g_sCrops[i].m_bActive == true) // later try put x = x and y = y
      //        win = false;
      //    else 
      //        win = true;
      //}
      //if (win == true)
      //{
      //    g_eGameState = S_WINSCREEN;
      //}
}

void UpdatePowerups()
{
    for (int i = 0; i < 3; i++)
    {
        if (g_sChar.m_cLocation.X == g_sPowerups[i].m_cLocation.X &&
            g_sChar.m_cLocation.Y == g_sPowerups[i].m_cLocation.Y && g_sPowerups[i].m_bActive == true)
        {
            Beep(1440, 30);
            g_sPowerups[i].m_bActive = false;
            powered = true;
            g_sChar.m_bActive = powered;
        }
    } // if problem cant be solved use powerups 1

    //if (g_sPowerups[0].m_bActive == false && g_sPowerups[1].m_bActive == false && g_sPowerups[2].m_bActive == false)
    //{
    //    // g_eGameState = S_DECIMATIONSCREEN; // DECIMATION SCREEN
    //    // g_Console.writeToBuffer(g_sChar.m_cLocation, (char)7, 13);
    //}
}

void UpdatePestcontroltimer()
{
    if (g_dElapsedTime > 50.0)
    {
        g_eGameState = S_PESTCONTROLSCREEN;
    }
}

void UpdateSpiders()
{
    for (int i = 0; i < 15; i++)
    {
        if (g_sChar.m_bActive = powered && g_sPowerups[0].m_bActive == false && g_sPowerups[1].m_bActive == false && g_sPowerups[2].m_bActive == false)
        {
            g_sSpiders[i].m_bActive = false;
        }

        //if (g_sChar.m_cLocation.X == g_sSpiders[i].m_cLocation.X &&
        //    g_sChar.m_cLocation.Y == g_sSpiders[i].m_cLocation.Y && g_sSpiders[i].m_bActive == false)
        //{
        //    // if spider is false, counter goes down // becomes 0 below
        //    /*g_sSpiders[14].m_bActive == g_sSpiders[13].m_bActive == g_sSpiders[12].m_bActive == g_sSpiders[11].m_bActive == g_sSpiders[10].m_bActive == g_sSpiders[9].m_bActive == g_sSpiders[8].m_bActive == g_sSpiders[7].m_bActive
        //        == g_sSpiders[6].m_bActive == g_sSpiders[5].m_bActive == g_sSpiders[4].m_bActive == g_sSpiders[3].m_bActive == g_sSpiders[2].m_bActive == g_sSpiders[1].m_bActive == g_sSpiders[0].m_bActive;*/

        //}

        if (g_sChar.m_cLocation.X == g_sSpiders[i].m_cLocation.X &&
             g_sChar.m_cLocation.Y == g_sSpiders[i].m_cLocation.Y && g_sSpiders[i].m_bActive == false)
        {
            g_eGameState = S_WINSCREEN;
        }
        

        if (g_sChar.m_cLocation.X == g_sSpiders[i].m_cLocation.X &&
            g_sChar.m_cLocation.Y == g_sSpiders[i].m_cLocation.Y && g_sSpiders[i].m_bActive == true)
        {
            g_eGameState = S_LOSESCREEN;
        }
    }
}

void updateLosingscreen()
{
    LoseInput(); 
    renderLosingscreen();
}

void updateWinscreen()
{
    WinInput();
    renderWinscreen();
}
void updateDecimationscreen()
{
}


void updatePestcontrol() 
{
    PestControlinput();  
    renderPestscreen();
}

void moveCharacter()
{
    // Updating the location of the character based on the key release
    // providing a beep sound whenver we shift the character
    if (g_skKeyEvent[K_UP].keyDown && g_sChar.m_cLocation.Y > 0 && g_dDeltaTime > 0)
    {
        //Beep(1440, 30);
        g_sChar.m_cLocation.Y--;
    }
    if (g_skKeyEvent[K_LEFT].keyDown && g_sChar.m_cLocation.X > 0)
    {
        //Beep(1440, 30);
        g_sChar.m_cLocation.X--;
    }
    if (g_skKeyEvent[K_DOWN].keyDown && g_sChar.m_cLocation.Y < g_Console.getConsoleSize().Y - 1)
    {
        //Beep(1440, 30);
        g_sChar.m_cLocation.Y++;
    }
    if (g_skKeyEvent[K_RIGHT].keyDown && g_sChar.m_cLocation.X < g_Console.getConsoleSize().X - 1)
    {
        //Beep(1440, 30);
        g_sChar.m_cLocation.X++;
    }
    if (g_skKeyEvent[K_SPACE].keyReleased)
    {
        g_sChar.m_bActive = !g_sChar.m_bActive;
    }

}

//void PlayerLimiter()
//{
//    playerUpdateRate += g_dDeltaTime;
//    if (playerUpdateRate > 2)
//    {
//        moveCharacter();
//        playerUpdateRate = 0.0;
//    }
//}

void processUserInput()
{
    // quits the game if player hits the escape key
    if (g_skKeyEvent[K_ESCAPE].keyReleased)
        g_eGameState = S_MENU;

}

// for detecting if user clicks on back button in How To PLay screen
void GuideInput()
{
    if (g_mouseEvent.mousePosition.Y == 23 && g_mouseEvent.mousePosition.X >= 63 && g_mouseEvent.mousePosition.X < 67 &&
        g_mouseEvent.buttonState == FROM_LEFT_1ST_BUTTON_PRESSED)
    {
        g_eGameState = S_MENU;
    } 
}

void MenuInput()
{
    // quits the game if user clicks on "quit" button
    if (g_mouseEvent.mousePosition.Y == 13 && g_mouseEvent.mousePosition.X >= 37 && g_mouseEvent.mousePosition.X < 41 &&
        g_mouseEvent.buttonState == FROM_LEFT_1ST_BUTTON_PRESSED)
    {
        g_bQuitGame = true;
    }

    // displays guide for game (Not functional yet)
    if (g_mouseEvent.mousePosition.Y == 11 && g_mouseEvent.mousePosition.X >= 33 && g_mouseEvent.mousePosition.X < 44 &&
        g_mouseEvent.buttonState == FROM_LEFT_1ST_BUTTON_PRESSED)
    {
        g_eGameState = S_GUIDE;
    }

    if (g_mouseEvent.mousePosition.Y == 9 && g_mouseEvent.mousePosition.X >= 36 && g_mouseEvent.mousePosition.X < 41 &&
        g_mouseEvent.buttonState == FROM_LEFT_1ST_BUTTON_PRESSED)
    {
        g_eGameState = S_GAME;

        for (int i = 0; i < 20; i++)
        {
            g_sCrops[i].m_cLocation.X = (rand() % 68);
            g_sCrops[i].m_cLocation.Y = (rand() % 22);
            g_sCrops[i].m_bActive = true;
        }

        for (int i = 0; i < 15; i++)
        {
            g_sSpiders[i].m_cLocation.X = (rand() % 68);
            g_sSpiders[i].m_cLocation.Y = (rand() % 22);
            g_sSpiders[i].m_bActive = true;
        }

        for (int i = 0; i < 3; i++)
        {
            g_sPowerups[i].m_cLocation.X = (rand() % 68);
            g_sPowerups[i].m_cLocation.Y = (rand() % 22);
            g_sPowerups[i].m_bActive = true;
        }
    }
}

void LoseInput()
{
    if (g_mouseEvent.mousePosition.Y == 11 && g_mouseEvent.mousePosition.X > 31 && g_mouseEvent.mousePosition.X < 43 &&
        g_mouseEvent.buttonState == FROM_LEFT_1ST_BUTTON_PRESSED)
    {
        g_eGameState = S_GAME;
        if (g_eGameState = S_GAME)
        {
            g_sChar.m_cLocation.X = 40;
            g_sChar.m_cLocation.Y = 12;
            for (int i = 0; i < 15; i++)
            {
                g_sSpiders[i].m_cLocation.X = (rand() % 68);
                g_sSpiders[i].m_cLocation.Y = (rand() % 22);
                g_sSpiders[i].m_bActive = true;
            }

            for (int i = 0; i < 20; i++)
            {
                g_sCrops[i].m_cLocation.X = (rand() % 64);
                g_sCrops[i].m_cLocation.Y = (rand() % 22);
                g_sCrops[i].m_bActive = true;
            }
            for (int i = 0; i < 3; i++)
            {
                g_sPowerups[i].m_cLocation.X = (rand() % 68);
                g_sPowerups[i].m_cLocation.Y = (rand() % 22);
                g_sPowerups[i].m_bActive = true;
            }
        }
    }

    if (g_mouseEvent.mousePosition.Y == 14 && g_mouseEvent.mousePosition.X > 31 && g_mouseEvent.mousePosition.X < 43 &&
        g_mouseEvent.buttonState == FROM_LEFT_1ST_BUTTON_PRESSED)
    {
        g_eGameState = S_MENU;
    }  


}

void WinInput()
{
    if (g_mouseEvent.mousePosition.Y == 11 && g_mouseEvent.mousePosition.X > 31 && g_mouseEvent.mousePosition.X < 43 &&
        g_mouseEvent.buttonState == FROM_LEFT_1ST_BUTTON_PRESSED)
    {
        g_eGameState = S_GAME;
        if (g_eGameState = S_GAME)
        {
            g_sChar.m_cLocation.X = 40;
            g_sChar.m_cLocation.Y = 12; 
            for (int i = 0; i < 15; i++)
            {
                g_sSpiders[i].m_cLocation.X = (rand() % 68);
                g_sSpiders[i].m_cLocation.Y = (rand() % 22);
                g_sSpiders[i].m_bActive = true;
            }
            for (int i = 0; i < 20; i++)
            {
                g_sCrops[i].m_cLocation.X = (rand() % 64);
                g_sCrops[i].m_cLocation.Y = (rand() % 22);
                g_sCrops[i].m_bActive = true;
            }

            for (int i = 0; i < 3; i++)
            {
                g_sPowerups[i].m_cLocation.X = (rand() % 68);
                g_sPowerups[i].m_cLocation.Y = (rand() % 22);
                g_sPowerups[i].m_bActive = true;
            }

            g_sChar.m_bActive = true;
        }
    }

    if (g_mouseEvent.mousePosition.Y == 14 && g_mouseEvent.mousePosition.X > 31 && g_mouseEvent.mousePosition.X < 43 &&
        g_mouseEvent.buttonState == FROM_LEFT_1ST_BUTTON_PRESSED)
    {
        g_eGameState = S_MENU;
    }

}

void PestControlinput() 
{
    if (g_mouseEvent.mousePosition.Y == 11 && g_mouseEvent.mousePosition.X > 31 && g_mouseEvent.mousePosition.X < 43 &&
        g_mouseEvent.buttonState == FROM_LEFT_1ST_BUTTON_PRESSED)
    {
        g_eGameState = S_GAME;
        
        g_sChar.m_cLocation.X = 40;
        g_sChar.m_cLocation.Y = 12;
        for (int i = 0; i < 15; i++)
        {
            g_sSpiders[i].m_cLocation.X = (rand() % 68);
            g_sSpiders[i].m_cLocation.Y = (rand() % 22);
            g_sSpiders[i].m_bActive = true;
        }

        for (int i = 0; i < 20; i++)
        {
            g_sCrops[i].m_cLocation.X = (rand() % 64);
            g_sCrops[i].m_cLocation.Y = (rand() % 22);
            g_sCrops[i].m_bActive = true;  
        }

        for (int i = 0; i < 3; i++)
        {
            g_sPowerups[i].m_cLocation.X = (rand() % 68);
            g_sPowerups[i].m_cLocation.Y = (rand() % 22);
            g_sPowerups[i].m_bActive = true;
        }

        
    }

    if (g_mouseEvent.mousePosition.Y == 14 && g_mouseEvent.mousePosition.X > 31 && g_mouseEvent.mousePosition.X < 43 &&
        g_mouseEvent.buttonState == FROM_LEFT_1ST_BUTTON_PRESSED)
    {
        g_eGameState = S_MENU;
    }
}

//--------------------------------------------------------------
// Purpose  : Render function is to update the console screen
//            At this point, you should know exactly what to draw onto the screen.
//            Just draw it!
//            To get an idea of the values for colours, look at console.h and the URL listed there
// Input    : void
// Output   : void
//--------------------------------------------------------------
void render()
{
    clearScreen();      // clears the current screen and draw from scratch 
    switch (g_eGameState)
    {
    case S_SPLASHSCREEN: renderSplashScreen();
        break;
    case S_GAME: renderGame();
        break;
    case S_GUIDE: renderguide();
        break;
    case S_MENU: rendermenu();
        break;
    case S_LOSESCREEN: renderLosingscreen();
        break;
    case S_WINSCREEN: renderWinscreen();
        break;
    case S_PESTCONTROLSCREEN: renderPestscreen();
        break;
    }


    renderInputEvents();    // renders status of input events
    renderToScreen();       // dump the contents of the buffer to the screen, one frame worth of game
}

void renderTimer()
{


    COORD c;
    // displays the time
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(3);
    ss.str("");
    ss << g_dElapsedTime << "secs";
    c.X = 70;
    c.Y = 0;
    g_Console.writeToBuffer(c, ss.str(), 46);

    if (g_dElapsedTime > 30.0) 
        g_Console.writeToBuffer(c, ss.str(), 44);
    

}

void spiderMovement()
{
    for (int i = 0; i < 15; i++)
    {
        EnemyUpdateRate += g_dDeltaTime;
        int random = rand() % 8 + 1;
        switch (random)
        {
        case 1:
            if (g_sSpiders[i].m_cLocation.X < g_Console.getConsoleSize().X - 1 && EnemyUpdateRate > 0.3)
            {
                g_sSpiders[i].m_cLocation.X++;
                EnemyUpdateRate = 0;
            }
            break;
        case 2:
            if (g_sSpiders[i].m_cLocation.Y < g_Console.getConsoleSize().Y - 1 && EnemyUpdateRate > 0.3)
            {
                g_sSpiders[i].m_cLocation.Y++;
                EnemyUpdateRate = 0;
            }
            break;
        case 3:
            if (g_sSpiders[i].m_cLocation.X > 0 && EnemyUpdateRate > 0.3)
            {
                g_sSpiders[i].m_cLocation.X--;
                EnemyUpdateRate = 0;
            }
            break;
        case 4:
            if (g_sSpiders[i].m_cLocation.Y > 0 && EnemyUpdateRate > 0.3)
            {
                g_sSpiders[i].m_cLocation.Y--;
                EnemyUpdateRate = 0;
            }
            break;
        }
    }
}


void clearScreen()
{
    // Clears the buffer with this colour attribute
    g_Console.clearBuffer(0x0F);
}

void renderToScreen()
{
    // Writes the buffer to the console, hence you will see what you have written
    g_Console.flushBufferToConsole();
}

void renderSplashScreen()  // renders the splash screen
{
    COORD c = g_Console.getConsoleSize();
    c.Y /= 3;
    c.X = c.X / 2 - 9;
    g_Console.writeToBuffer(c, "A game in 3 seconds", 0x03);
    c.Y += 1;
    c.X = g_Console.getConsoleSize().X / 2 - 20;
    g_Console.writeToBuffer(c, "Press <Space> to change character colour", 0x09);
    c.Y += 1;
    c.X = g_Console.getConsoleSize().X / 2 - 9;
    g_Console.writeToBuffer(c, "Press 'Esc' to quit", 0x09);
}

void renderGame()
{
    renderMap();        // renders the map to the buffer first  
    renderTimer();      // renders debug information, frame rate, elapsed time, etc
    renderCrops();
    renderPowerups();
    renderSpiders();
    renderCharacter();  // renders the character into the buffer
}

//Renders the menu screen
void rendermenu()
{
    const WORD colors[] = {
        9,26,20,22,31,
    };

    COORD c;

    c.X = 33;
    c.Y = 5;
    colour(colors[4]);
    g_Console.writeToBuffer(c, "LOCUST RUSH", colors[0]);

    c.X = 36;
    c.Y = 9;
    colour(colors[0]);
    g_Console.writeToBuffer(c, "Start", colors[0]);

    c.X = 33;
    c.Y = 11;
    colour(colors[0]);
    g_Console.writeToBuffer(c, "How to Play", colors[0]);

    c.X = 37;
    c.Y = 13;
    colour(colors[4]);
    g_Console.writeToBuffer(c, "Quit", colors[0]);

    //color change when mouse is over buttons
    if (g_mouseEvent.mousePosition.Y == 9 && g_mouseEvent.mousePosition.X >= 36 && g_mouseEvent.mousePosition.X < 41)
    {
        c.X = 36;
        c.Y = 9;
        colour(colors[1]);
        g_Console.writeToBuffer(c, "Start", colors[1]);
    }

    if (g_mouseEvent.mousePosition.Y == 11 && g_mouseEvent.mousePosition.X >= 33 && g_mouseEvent.mousePosition.X < 44)
    {
        c.X = 33;
        c.Y = 11;
        colour(colors[3]);
        g_Console.writeToBuffer(c, "How to Play", colors[3]);
    }

    if (g_mouseEvent.mousePosition.Y == 13 && g_mouseEvent.mousePosition.X >= 37 && g_mouseEvent.mousePosition.X < 41)
    {
        c.X = 37;
        c.Y = 13;
        colour(colors[2]); 
        g_Console.writeToBuffer(c, "Quit", colors[2]);
    }
}

void renderLosingscreen()
{
    const WORD colors[] = {
    9,26,20,22,31,
    };

    COORD c;

    c.X = 33;
    c.Y = 5;
    colour(colors[4]);
    g_Console.writeToBuffer(c, "YOU DIED", colors[0]);

    c.X = 32;
    c.Y = 11;
    colour(colors[0]);
    g_Console.writeToBuffer(c, "Play again", colors[0]);

    c.X = 31;
    c.Y = 14;
    colour(colors[0]);
    g_Console.writeToBuffer(c, "Back to menu", colors[0]);

    if (g_mouseEvent.mousePosition.Y == 11 && g_mouseEvent.mousePosition.X >= 32 && g_mouseEvent.mousePosition.X < 43)
    {
        c.X = 32;
        c.Y = 11;
        colour(colors[3]);
        g_Console.writeToBuffer(c, "Play again", colors[3]);
    }

    if (g_mouseEvent.mousePosition.Y == 14 && g_mouseEvent.mousePosition.X >= 31 && g_mouseEvent.mousePosition.X < 43)
    {
        c.X = 31;
        c.Y = 14;
        colour(colors[3]);
        g_Console.writeToBuffer(c, "Back to menu", colors[3]);
    }

}

void renderWinscreen()
{
    {
        const WORD colors[] = {
        9,26,20,22,31,
        };

        COORD c;

        c.X = 33;
        c.Y = 5;
        colour(colors[4]);
        g_Console.writeToBuffer(c, "YOU WIN", colors[0]);

        c.X = 32;
        c.Y = 11;
        colour(colors[0]);
        g_Console.writeToBuffer(c, "Play again", colors[0]);

        c.X = 31;
        c.Y = 14;
        colour(colors[0]);
        g_Console.writeToBuffer(c, "Back to menu", colors[0]);

        if (g_mouseEvent.mousePosition.Y == 11 && g_mouseEvent.mousePosition.X >= 32 && g_mouseEvent.mousePosition.X < 43)
        {
            c.X = 32;
            c.Y = 11;
            colour(colors[3]);
            g_Console.writeToBuffer(c, "Play again", colors[3]);
        }

        if (g_mouseEvent.mousePosition.Y == 14 && g_mouseEvent.mousePosition.X >= 31 && g_mouseEvent.mousePosition.X < 43)
        {
            c.X = 31;
            c.Y = 14;
            colour(colors[3]);
            g_Console.writeToBuffer(c, "Back to menu", colors[3]);
        }

    }
}

void renderPestscreen() 
{
    const WORD colors[] = {
  9,26,20,22,31,
    };
    
    COORD c;

    c.X = 25;
    c.Y = 5;
    colour(colors[4]);
    g_Console.writeToBuffer(c, "PEST CONTROL HAS ARRIVED", colors[0]);

    c.X = 32;
    c.Y = 11;
    colour(colors[0]);
    g_Console.writeToBuffer(c, "Play again", colors[0]);

    c.X = 31;
    c.Y = 14;
    colour(colors[0]);
    g_Console.writeToBuffer(c, "Back to menu", colors[0]);

    if (g_mouseEvent.mousePosition.Y == 11 && g_mouseEvent.mousePosition.X >= 32 && g_mouseEvent.mousePosition.X < 43)
    {
        c.X = 32;
        c.Y = 11;
        colour(colors[3]);
        g_Console.writeToBuffer(c, "Play again", colors[3]);
    }

    if (g_mouseEvent.mousePosition.Y == 14 && g_mouseEvent.mousePosition.X >= 31 && g_mouseEvent.mousePosition.X < 43)
    {
        c.X = 31;
        c.Y = 14;
        colour(colors[3]);
        g_Console.writeToBuffer(c, "Back to menu", colors[3]);
    }
}

void renderDecimationscreen()
{
}


void updatemenu()
{
    MenuInput();
    rendermenu();
}



void updateguide()
{
    GuideInput();
    renderguide();
}

//renders How To Play screen
void renderguide()
{
    COORD c = g_Console.getConsoleSize();
    c.Y /= 7;
    c.X = c.X / 2 - 10;
    g_Console.writeToBuffer(c, "==Controls==", 15);
    c.Y += 1;
    c.X = g_Console.getConsoleSize().X / 2 - 9;
    g_Console.writeToBuffer(c, "Arrow Keys", 15);
    c.Y += 1;
    c.X = g_Console.getConsoleSize().X / 2 - 9;
    g_Console.writeToBuffer(c, "WASD keys", 15);
    c.Y += 2;
    c.X = g_Console.getConsoleSize().X / 2 - 11;
    g_Console.writeToBuffer(c, "==How to Play==", 15);
    c.Y += 1;
    c.X = g_Console.getConsoleSize().X / 2 - 40;
    g_Console.writeToBuffer(c, "Move your locust to a piece of farmland, it will solwly start to eat the crops.", 15);
    c.Y += 2;
    c.X = g_Console.getConsoleSize().X / 2 - 40;
    g_Console.writeToBuffer(c, "By doing so, it will increase your locust population. The larger the population, the faster you consume crops.", 15);
    c.Y += 3;
    c.X = g_Console.getConsoleSize().X / 2 - 40;
    g_Console.writeToBuffer(c, "Watch out for predators, they will attack your locust population on close promixity. If your whole swarm is eaten, you will lose.", 15);
    c.Y += 3;
    c.X = g_Console.getConsoleSize().X / 2 - 40;
    g_Console.writeToBuffer(c, "Bewarned, increasing your locust population beyond (limit) will result in the pest control being called in, resulting in a defeat.", 15);
    c.Y += 3;
    c.X = g_Console.getConsoleSize().X / 2 - 40;
    g_Console.writeToBuffer(c, "To win, the swarm must consume all the crops on the field in the shortest amount of time, without dying to either predators or pest control.", 15);

    c.X = 63;
    c.Y = 23;
    g_Console.writeToBuffer(c, "Back", 15);

    if (g_mouseEvent.mousePosition.Y == 23 && g_mouseEvent.mousePosition.X >= 63 && g_mouseEvent.mousePosition.X < 67)
    {
        c.X = 63;
        c.Y = 23;
        g_Console.writeToBuffer(c, "Back", 12);
    }

}

void renderCrops()
{
    // Draw the location of the spiders

    for (int i = 0; i < 20; i++)
    {
        g_Console.writeToBuffer(g_sCrops[i].m_cLocation, (char)10, 110);
        if (g_sCrops[i].m_bActive == false)
        {
            g_Console.writeToBuffer(g_sCrops[i].m_cLocation, (char)10, 46);
        }
    }
}


void renderSpiders()
{

    for (int i = 0; i < 15; i++)
    {
        if (g_sSpiders[i].m_bActive == true)
        {
            g_Console.writeToBuffer(g_sSpiders[i].m_cLocation, (char)31, 7);
        }
        
        if (g_sSpiders[i].m_bActive == false)
        {
            g_Console.writeToBuffer(g_sSpiders[i].m_cLocation, (char)25, 1);
        }
    }
}

void renderPowerups()
{
    for (int i = 0; i < 3; i++)
    {
        g_Console.writeToBuffer(g_sPowerups[i].m_cLocation, (char)30, 9);
        if (g_sPowerups[i].m_bActive == false)
        {
            g_Console.writeToBuffer(g_sPowerups[i].m_cLocation, (char)30, 2);
        }
    }
}

void renderCharacter()
{
    // Draw the location of the character
    /*WORD charColor = 0x0C;
    if (g_sChar.m_bActive)
    {
        charColor = 224;
    }*/
    g_Console.writeToBuffer(g_sChar.m_cLocation, (char)5, 10);
}


void renderMap()
{

    const WORD colors[80][25] =
    {
        32, 32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32, 32,132,132,132, 32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32, 132,132,132,132,132,132,32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32, 132,132,132,132,132,132,32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	132,132,132,132,132,32, 32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	132,132,132,132,32, 32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	132,132,132,32, 32, 32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	132,132,32,	32,
32,	132,132,132,32, 32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	132,132,32, 32,
32,	132,132,32, 32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	132,132,32, 32,
32,	32,132, 32, 32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	132,132,32, 32,
32,	32, 32, 32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	132,32,	32,
32,	32, 32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32, 32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	132,132,132,132,132,32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	132,132,132,132,132,132,132,32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	132,132,132,132,132,132,132,32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	132,132,132,132,132,132,32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	132,132,32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	132,132,132,32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	132,132,132,32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,132,32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	132,32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	132,32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	132,132,32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	132,132,32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	132,132,132,32, 32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	132,132,132,32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	132,132,132,32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	132,132,132,132,32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	132,132,132,132,32,	32,	32,	32,	32,	132,132,132,132,32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	132,132,132,132,132,32,	32,	32,	32,	32,	32,	132,132,132,32, 32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	132,132,132,132,132,32,	32,	32,	32,	32,	32,	132,32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	132,132,132,132,32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	132,132,132,132,32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	132,132,132,132,32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	132,132,132,32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	132,32,32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	132,132,32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	132,132,32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,132,132,132,132,32, 32,	32,	32, 32,
32,	32,	32,	132,132,32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	132,132,132,132,132,32,	32,	32,	32, 32,
32,	32,	32,	132,132,32, 32,	32,	32,	32,	32,	32,	32,	32,	32,	132,132,132,132,32,32,	32,	32,	32, 32,
32,	32,	32,	132,132,32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	132,32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	132,132,132,132,132,32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	132,132,132,132,132,132,132,32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	132,132,132,132,132,132,132,32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	132,132,132,132,132,132,32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,
32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32,	32
    };

    for (int x = 0; x < 80; x++)
    {
        for (int y = 0; y < 25; y++) {
            COORD c;
            c.X = x;
            c.Y = y;
            g_Console.writeToBuffer(c, "  ", colors[x][y]);
        }
    }

}

// this is an example of how you would use the input events
void renderInputEvents()
{
    // keyboard events
    COORD startPos = { 50, 2 };
    std::ostringstream ss;
    std::string key;
    for (int i = 0; i < K_COUNT; ++i)
    {
        ss.str("");
        switch (i)
        {
        case K_UP: key = "UP";
            break;
        case K_DOWN: key = "DOWN";
            break;
        case K_LEFT: key = "LEFT";
            break;
        case K_RIGHT: key = "RIGHT";
            break;
        case K_SPACE: key = "SPACE";
            break;
        default: continue;
        }

        COORD c = { startPos.X, startPos.Y + i };
        g_Console.writeToBuffer(c, ss.str(), 23);
    }

    //// mouse events    
    //ss.str("");
    //ss << "Mouse position (" << g_mouseEvent.mousePosition.X << ", " << g_mouseEvent.mousePosition.Y << ")";
    //g_Console.writeToBuffer(g_mouseEvent.mousePosition, ss.str(), 0x59);
    //ss.str("");
    //switch (g_mouseEvent.eventFlags)
    //{
    //case 0:
    //    if (g_mouseEvent.buttonState == FROM_LEFT_1ST_BUTTON_PRESSED)
    //    {
    //        ss.str("Left Button Pressed");
    //        g_Console.writeToBuffer(g_mouseEvent.mousePosition.X, g_mouseEvent.mousePosition.Y + 1, ss.str(), 0x59);
    //    }
    //    else if (g_mouseEvent.buttonState == RIGHTMOST_BUTTON_PRESSED)
    //    {
    //        ss.str("Right Button Pressed");
    //        g_Console.writeToBuffer(g_mouseEvent.mousePosition.X, g_mouseEvent.mousePosition.Y + 2, ss.str(), 0x59);
    //    }
    //    else
    //    {
    //        ss.str("Some Button Pressed");
    //        g_Console.writeToBuffer(g_mouseEvent.mousePosition.X, g_mouseEvent.mousePosition.Y + 3, ss.str(), 0x59);
    //    }
    //    break;
    //case DOUBLE_CLICK:
    //    ss.str("Double Clicked");
    //    g_Console.writeToBuffer(g_mouseEvent.mousePosition.X, g_mouseEvent.mousePosition.Y + 4, ss.str(), 0x59);
    //    break;
    //case MOUSE_WHEELED:
    //    if (g_mouseEvent.buttonState & 0xFF000000)
    //        ss.str("Mouse wheeled down");
    //    else
    //        ss.str("Mouse wheeled up");
    //    g_Console.writeToBuffer(g_mouseEvent.mousePosition.X, g_mouseEvent.mousePosition.Y + 5, ss.str(), 0x59);
    //    break;
    //default:
    //    break;
    //}

}





