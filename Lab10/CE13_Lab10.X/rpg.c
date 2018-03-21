// **** Include libraries here ****
// Standard libraries
#include <string.h>
#include <math.h>
#include <stdio.h>

//CMPE13 Support Library
#include "UNIXBOARD.h"
#include "Game.h"
#include "Player.h"


// User libraries


// **** Set any macros or preprocessor directives here ****

// **** Declare any data types here ****

// **** Define any global or external variables here ****
static char roomTitle[GAME_MAX_ROOM_TITLE_LENGTH + 1];
static char roomDesc[GAME_MAX_ROOM_DESC_LENGTH + 1];
static uint8_t exits;

// **** Declare any function prototypes here ****
static void UpdateScreen(void);

int main()
{



    /******************************** Your custom code goes below here ********************************/
    
    GameInit();
    UpdateScreen();
    static char input;
    while (1) {
        input = getchar();
        if (input == 'n') {
            GameGoNorth();
            UpdateScreen();
        } else if (input == 'e') {
            GameGoEast();
            UpdateScreen();
        } else if (input == 's') {
            GameGoSouth();
            UpdateScreen();
        } else if (input == 'w') {
            GameGoWest();
            UpdateScreen();
        } else if (input == 'q') {
            exit(0);
        }
    }
    /**************************************************************************************************/
}

static void UpdateScreen(void)
{

    GameGetCurrentRoomTitle(roomTitle);
    GameGetCurrentRoomDescription(roomDesc);
    exits = GameGetCurrentRoomExits();
    // Clears the Screen
    printf("\033[2J");

    // Moves the cursor to the top
    printf("\033[22A");

    // Prints the Title at the top
    printf("\033[41m\033[34m%s\033[0m\n", roomTitle);

    // Prints the Description right below the Title
    printf("\033[0m%s\n", roomDesc);
    
    // Moves down the screen 
    printf("\033[%dB", 14);
    
    // Directions 
    // The Exit is green if available and red if not available
    // North
    printf("\033[8C");
    if (exits & GAME_ROOM_EXIT_NORTH_EXISTS) {
        printf("\033[32m");
    } else {
        printf("\033[31m");
    }
    printf("North");
    
    // West 
    printf("\033E");
    if (exits & GAME_ROOM_EXIT_WEST_EXISTS) {
        printf("\033[32m");
    } else {
        printf("\033[31m");
    }
    printf("West");
    
    // East
    printf("\033[13C");
    if (exits & GAME_ROOM_EXIT_EAST_EXISTS) {
        printf("\033[32m");
    } else {
        printf("\033[31m");
    }
    printf("East");
    
    // South
    printf("\033E");
    printf("\033[8C");
    if (exits & GAME_ROOM_EXIT_SOUTH_EXISTS) {
        printf("\033[32m");
    } else {
        printf("\033[31m");
    }
    printf("South");
    printf("\033E");
    printf("\033[37m");
    
    // Prints prompt below the directions
    printf("Select Direction by using N, E, S, or W\n");
}