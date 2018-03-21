/*
 * File:   Player.c
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

static int i;
uint8_t inventory[INVENTORY_SIZE];

/**
 * Adds the specified item to the player's inventory if the inventory isn't full.
 * @param item The item number to be stored: valid values are 0-255.
 * @return SUCCESS if the item was added, STANDARD_ERRROR if the item couldn't be added.
 */
int AddToInventory(uint8_t item)
{
    for (i = 0; i < INVENTORY_SIZE; i++) {
        if(inventory[i] == 0){
            if(inventory[i] != item){
                inventory[i] = item;
                return SUCCESS;
            }
        }
    }
    return STANDARD_ERROR;
}

/**
 * Check if the given item exists in the player's inventory.
 * @param item The number of the item to be searched for: valid values are 0-255.
 * @return SUCCESS if it was found or STANDARD_ERROR if it wasn't.
 */
int FindInInventory(uint8_t item)
{
    for (i = 0; i < INVENTORY_SIZE; i++) {
        if (inventory[i] == item) {
            return SUCCESS;
        } else {
            return STANDARD_ERROR;
        }
    }
    return STANDARD_ERROR;
}
