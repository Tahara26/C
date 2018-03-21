/*
 * File:   Game.c
 * Author: JustinT
 *
 * Created on March 15, 2018, 9:20 PM
 */

#include "Game.h"
#include "Player.h"
#include "UNIXBOARD.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define FILE_TEMPLATE "RoomFiles/room%d.txt"

typedef struct Room {
    char title[GAME_MAX_ROOM_TITLE_LENGTH + 1];
    char description[GAME_MAX_ROOM_DESC_LENGTH + 1];
    uint8_t RoomNumber;
    uint8_t Item[INVENTORY_SIZE];
    uint8_t Exit[4];
    uint8_t Length;
    char File[24];
    uint8_t RoomInventory[INVENTORY_SIZE];
} Room;

static Room RPGRoom;

static void RoomUpdate(void);

static FILE *file;

/**
 * These function transitions between rooms. Each call should return SUCCESS if the current room has
 * an exit in the correct direction and the new room was able to be loaded, and STANDARD_ERROR
 * otherwise.
 * @return SUCCESS if the room CAN be navigated to and changing the current room to that new room
 *         succeeded.
 */
int GameGoNorth(void)
{
    if (GAME_ROOM_EXIT_NORTH_EXISTS & GameGetCurrentRoomExits()) {
        RPGRoom.RoomNumber = RPGRoom.Exit[0];
        RoomUpdate();
        return SUCCESS;
    } else {
        return STANDARD_ERROR;
    }
}

/**
 * @see GameGoNorth
 */
int GameGoEast(void)
{
    if (GAME_ROOM_EXIT_EAST_EXISTS & GameGetCurrentRoomExits()) {
        RPGRoom.RoomNumber = RPGRoom.Exit[1];
        RoomUpdate();
        return SUCCESS;
    } else {
        return STANDARD_ERROR;
    }
}

/**
 * @see GameGoNorth
 */
int GameGoSouth(void)
{
    if (GAME_ROOM_EXIT_SOUTH_EXISTS & GameGetCurrentRoomExits()) {
        RPGRoom.RoomNumber = RPGRoom.Exit[2];
        RoomUpdate();
        return SUCCESS;
    } else {
        return STANDARD_ERROR;
    }
}

/**
 * @see GameGoNorth
 */
int GameGoWest(void)
{
    if (GAME_ROOM_EXIT_WEST_EXISTS & GameGetCurrentRoomExits()) {
        RPGRoom.RoomNumber = RPGRoom.Exit[3];
        RoomUpdate();
        return SUCCESS;
    } else {
        return STANDARD_ERROR;
    }
}

/**
 * This function sets up anything that needs to happen at the start of the game. This is just
 * setting the current room to STARTING_ROOM and loading it. It should return SUCCESS if it succeeds
 * and STANDARD_ERROR if it doesn't.
 * @return SUCCESS or STANDARD_ERROR
 */
int GameInit(void)
{
    RPGRoom.title[0] = '\0';
    RPGRoom.description[0] = '\0';
    RPGRoom.Exit[0] = STARTING_ROOM;
    RPGRoom.Exit[1] = 0;
    RPGRoom.Exit[2] = 0;
    RPGRoom.Exit[3] = 0;
    RPGRoom.RoomNumber = 0;
    RPGRoom.File[0] = '\0';
    return GameGoNorth();
}

/**
 * Copies the current room title as a NULL-terminated string into the provided character array.
 * Only a NULL-character is copied if there was an error so that the resultant output string
 * length is 0.
 * @param title A character array to copy the room title into. Should be GAME_MAX_ROOM_TITLE_LENGTH+1
 *             in length in order to allow for all possible titles to be copied into it.
 * @return The length of the string stored into `title`. Note that the actual number of chars
 *         written into `title` will be this value + 1 to account for the NULL terminating
 *         character.
 */
int GameGetCurrentRoomTitle(char *title)
{
    sprintf(title, "%s", RPGRoom.title);
    return strlen(title);
}

/**
 * GetCurrentRoomDescription() copies the description of the current room into the argument desc as
 * a C-style string with a NULL-terminating character. The room description is guaranteed to be less
 * -than-or-equal to GAME_MAX_ROOM_DESC_LENGTH characters, so the provided argument must be at least
 * GAME_MAX_ROOM_DESC_LENGTH + 1 characters long. Only a NULL-character is copied if there was an
 * error so that the resultant output string length is 0.
 * @param desc A character array to copy the room description into.
 * @return The length of the string stored into `desc`. Note that the actual number of chars
 *          written into `desc` will be this value + 1 to account for the NULL terminating
 *          character.
 */
int GameGetCurrentRoomDescription(char *desc)
{
    sprintf(desc, "%s", RPGRoom.description);
    return strlen(desc);
}

/**
 * This function returns the exits from the current room in the lowest-four bits of the returned
 * uint8 in the order of NORTH, EAST, SOUTH, and WEST such that NORTH is in the MSB and WEST is in
 * the LSB. A bit value of 1 corresponds to there being a valid exit in that direction and a bit
 * value of 0 corresponds to there being no exit in that direction. The GameRoomExitFlags enum
 * provides bit-flags for checking the return value.
 *
 * @see GameRoomExitFlags
 *
 * @return a 4-bit bitfield signifying which exits are available to this room.
 */
uint8_t GameGetCurrentRoomExits(void)
{
    uint8_t exits = 0;
    if (RPGRoom.Exit[0] != 0) {
        exits |= GAME_ROOM_EXIT_NORTH_EXISTS;
    }
    if (RPGRoom.Exit[1] != 0) {
        exits |= GAME_ROOM_EXIT_EAST_EXISTS;
    }
    if (RPGRoom.Exit[2] != 0) {
        exits |= GAME_ROOM_EXIT_SOUTH_EXISTS;
    }
    if (RPGRoom.Exit[3] != 0) {
        exits |= GAME_ROOM_EXIT_WEST_EXISTS;
    }
    return exits;
}


// Helper function for updating rooms 
static void RoomUpdate(void)
{
    // Setting up all variables
    static char storevalue = 0;
    static int i;
    static char garbage[GAME_MAX_ROOM_DESC_LENGTH];
    static uint8_t flag = 0;

    // Clears the title and description each time
    memset(RPGRoom.title, '\0', sizeof (RPGRoom.title));
    memset(RPGRoom.description, '\0', sizeof (RPGRoom.description));

    // Opens the file
    sprintf(RPGRoom.File, FILE_TEMPLATE, RPGRoom.RoomNumber);
    file = fopen(RPGRoom.File, "rb");

    // Title
    storevalue = fgetc(file);
    RPGRoom.Length = (uint8_t) (storevalue ^ (uint8_t) (RPGRoom.RoomNumber + DECRYPTION_BASE_KEY));
    fread(RPGRoom.title, RPGRoom.Length, 1, file);
    for (i = 0; i < RPGRoom.Length; i++) {
        RPGRoom.title[i] = RPGRoom.title[i] ^ ((uint8_t) (RPGRoom.RoomNumber + DECRYPTION_BASE_KEY));
    }

    // Items Needed
    while (1) {
        storevalue = fgetc(file);
        RPGRoom.Length = (uint8_t) (storevalue ^ (uint8_t) (RPGRoom.RoomNumber + DECRYPTION_BASE_KEY));
        if (RPGRoom.Length == 0) {
            break;
        } else {
            for (i = 0; i < RPGRoom.Length; i++) {
                storevalue = fgetc(file);
                RPGRoom.Item[i] = (uint8_t) (storevalue ^ (uint8_t) (RPGRoom.RoomNumber + DECRYPTION_BASE_KEY));
            }
            for (i = 0; i < RPGRoom.Length; i++) {
                if (FindInInventory(RPGRoom.Item[i]) == SUCCESS) {
                    flag = 0;
                } else {
                    flag = 1;
                    fread(garbage, (RPGRoom.Length) - (i + 1), 1, file);
                    storevalue = fgetc(file);
                    RPGRoom.Length = (uint8_t) (storevalue ^ (uint8_t) (RPGRoom.RoomNumber + DECRYPTION_BASE_KEY));
                    fread(garbage, RPGRoom.Length, 1, file);
                    storevalue = fgetc(file);
                    RPGRoom.Length = (uint8_t) (storevalue ^ (uint8_t) (RPGRoom.RoomNumber + DECRYPTION_BASE_KEY));
                    fread(garbage, RPGRoom.Length, 1, file);
                    fread(garbage, 4, 1, file);
                    break;
                }
            }
            if (flag == 0) {
                break;
            }
        }
    }

    // Description
    storevalue = fgetc(file);
    RPGRoom.Length = (uint8_t) (storevalue ^ (uint8_t) (RPGRoom.RoomNumber + DECRYPTION_BASE_KEY));
    fread(RPGRoom.description, RPGRoom.Length, 1, file);
    for (i = 0; i < RPGRoom.Length; i++) {
        RPGRoom.description[i] = RPGRoom.description[i] ^ ((uint8_t) (RPGRoom.RoomNumber + DECRYPTION_BASE_KEY));
    }

    // Items inside room
    storevalue = fgetc(file);
    RPGRoom.Length = (uint8_t) (storevalue ^ (uint8_t) (RPGRoom.RoomNumber + DECRYPTION_BASE_KEY));
    if (RPGRoom.Length == 0) {
        ;
    } else {
        for (i = 0; i < RPGRoom.Length; i++) {
        storevalue = fgetc(file);
        AddToInventory(RPGRoom.RoomInventory[i]);
        }
    }
    
    // Exits
    for (i = 0; i < 4; i++) {
        storevalue = fgetc(file);
        RPGRoom.Exit[i] = (uint8_t) (storevalue ^ (uint8_t) (RPGRoom.RoomNumber + DECRYPTION_BASE_KEY));
    }
    
    // Closes file
    fclose(file);
    return;
}