/*
 * File:   Field.c
 * Author: JustinT
 *
 * Created on March 8, 2018, 5:26 PM
 */
#include "BOARD.h"
#include "Field.h"
#include "FieldOled.h"
#include "Oled.h"
#include "OledDriver.h"
#include "Protocol.h"
#include "xc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * FieldInit() will fill the passed field array with the data specified in positionData. Also the
 * lives for each boat are filled according to the `BoatLives` enum.
 * @param f The field to initialize.
 * @param p The data to initialize the entire field to, should be a member of enum
 *                     FieldPosition.
 */
void FieldInit(Field *f, FieldPosition p)
{
    // Make a for loop to iterate through all of the points on the field
    static int i, j;
    for (i = 0; i < FIELD_ROWS; i++) {
        for (j = 0; j < FIELD_COLS; j++) {
            f->field[i][j] = p;
        }
    }
    
    // Set the lives of the ships to the set values
    f->smallBoatLives = FIELD_BOAT_LIVES_SMALL;
    f->mediumBoatLives = FIELD_BOAT_LIVES_MEDIUM;
    f->largeBoatLives = FIELD_BOAT_LIVES_LARGE;
    f->hugeBoatLives = FIELD_BOAT_LIVES_HUGE;
}

/**
 * Retrieves the value at the specified field position.
 * @param f The Field being referenced
 * @param row The row-component of the location to retrieve
 * @param col The column-component of the location to retrieve
 * @return
 */
FieldPosition FieldAt(const Field *f, uint8_t row, uint8_t col)
{
    // Returns the position
    return f->field[row][col];
}

/**
 * This function provides an interface for setting individual locations within a Field struct. This
 * is useful when FieldAddBoat() doesn't do exactly what you need. For example, if you'd like to use
 * FIELD_POSITION_CURSOR, this is the function to use.
 * 
 * @param f The Field to modify.
 * @param row The row-component of the location to modify
 * @param col The column-component of the location to modify
 * @param p The new value of the field location
 * @return The old value at that field location
 */
FieldPosition FieldSetLocation(Field *f, uint8_t row, uint8_t col, FieldPosition p)
{
    // Make a temp to hold location
    FieldPosition temporary1 = f->field[row][col];
    
    // Set position to p
    f->field[row][col] = p;
    return temporary1;
}

/**
 * FieldAddBoat() places a single ship on the player's field based on arguments 2-5. Arguments 2, 3
 * represent the x, y coordinates of the pivot point of the ship.  Argument 4 represents the
 * direction of the ship, and argument 5 is the length of the ship being placed. All spaces that
 * the boat would occupy are checked to be clear before the field is modified so that if the boat
 * can fit in the desired position, the field is modified as SUCCESS is returned. Otherwise the
 * field is unmodified and STANDARD_ERROR is returned. There is no hard-coded limit to how many
 * times a boat can be added to a field within this function.
 *
 * So this is valid test code:
 * {
 *   Field myField;
 *   FieldInit(&myField,FIELD_POSITION_EMPTY);
 *   FieldAddBoat(&myField, 0, 0, FIELD_BOAT_DIRECTION_EAST, FIELD_BOAT_SMALL);
 *   FieldAddBoat(&myField, 1, 0, FIELD_BOAT_DIRECTION_EAST, FIELD_BOAT_MEDIUM);
 *   FieldAddBoat(&myField, 1, 0, FIELD_BOAT_DIRECTION_EAST, FIELD_BOAT_HUGE);
 *   FieldAddBoat(&myField, 0, 6, FIELD_BOAT_DIRECTION_SOUTH, FIELD_BOAT_SMALL);
 * }
 *
 * should result in a field like:
 *  _ _ _ _ _ _ _ _
 * [ 3 3 3       3 ]
 * [ 4 4 4 4     3 ]
 * [             3 ]
 *  . . . . . . . .
 *
 * @param f The field to grab data from.
 * @param row The row that the boat will start from, valid range is from 0 and to FIELD_ROWS - 1.
 * @param col The column that the boat will start from, valid range is from 0 and to FIELD_COLS - 1.
 * @param dir The direction that the boat will face once places, from the BoatDirection enum.
 * @param boatType The type of boat to place. Relies on the FIELD_POSITION_*_BOAT values from the
 * FieldPosition enum.
 * @return TRUE for success, FALSE for failure
 */
uint8_t FieldAddBoat(Field *f, uint8_t row, uint8_t col, BoatDirection dir, BoatType type)
{
    // Counter
    int i; 

    // Makes sure initial spaces on the field for boat are empty
    if (f->field[row][col] != FIELD_POSITION_EMPTY) {
        return FALSE;
    } 
    
    // Checks all other cases
    else {
        // Check if all boat spaces are clear
        // Place boat spots accordingly
        if (dir == FIELD_BOAT_DIRECTION_NORTH) {
            for (i = 0; i < (type + 3); i++) {
                // Check if there is enough space on the field
                if ((row - i) < 0) {
                    return FALSE;
                }
                // Check if there are boats in the way
                if (f->field[row - i][col] != FIELD_POSITION_EMPTY) {
                    return FALSE;
                }
            }
            // Places the boat
            for (i = 0; i < (type + 3); i++) {
                f->field[row][col] = type + 1;
                row--;
            }
            return TRUE;
        } 
        
        // Case for South
        else if (dir == FIELD_BOAT_DIRECTION_SOUTH) {
            for (i = 0; i < (type + 3); i++) {
                if (((row + i) + 1) > FIELD_ROWS) {
                    return FALSE;
                }
                if (f->field[row + i][col] != FIELD_POSITION_EMPTY) {
                    return FALSE;
                }
            }
            for (i = 0; i < (type + 3); i++) {
                f->field[row][col] = type + 1;
                row++;
            }
            return TRUE;
        } 
        
        // Case for East
        else if (dir == FIELD_BOAT_DIRECTION_EAST) {
            for (i = 0; i < (type + 3); i++) {
                if (((col + i) + 1) > FIELD_COLS) {
                    return FALSE;
                }
                if (f->field[row][col + i] != FIELD_POSITION_EMPTY) {
                    return FALSE;
                }
            }
            for (i = 0; i < (type + 3); i++) {
                f->field[row][col] = type + 1;
                col++;
            }
            return TRUE;
        } 
        
        // Case for West
        else if (dir == FIELD_BOAT_DIRECTION_WEST) {
            for (i = 0; i < (type + 3); i++) {
                if ((col - i) < 0) {
                    return FALSE;
                }
                if (f->field[row][col - i] != FIELD_POSITION_EMPTY) {
                    return FALSE;
                }
            }
            for (i = 0; i < (type + 3); i++) {
                f->field[row][col] = type + 1;
                col--;
            }
            return TRUE;
        }
    }
    return FALSE;
}

/**
 * This function registers an attack at the gData coordinates on the provided field. This means that
 * 'f' is updated with a FIELD_POSITION_HIT or FIELD_POSITION_MISS depending on what was at the
 * coordinates indicated in 'gData'. 'gData' is also updated with the proper HitStatus value
 * depending on what happened AND the value of that field position BEFORE it was attacked. Finally
 * this function also reduces the lives for any boat that was hit from this attack.
 * @param f The field to check against and update.
 * @param gData The coordinates that were guessed. The HIT result is stored in gData->hit as an
 *               output.
 * @return The data that was stored at the field position indicated by gData before this attack.
 */
FieldPosition FieldRegisterEnemyAttack(Field *f, GuessData *gData)
{
    // Make a temp to hold data
    FieldPosition temporary2 = f->field[gData->row][gData->col];
    
    // Makes sure the field is not empty
    if (f->field[gData->row][gData->col] != FIELD_POSITION_EMPTY) {
        
        // Small boat and still has more than one life
        if (f->field[gData->row][gData->col] == FIELD_POSITION_SMALL_BOAT && f->smallBoatLives != 1) {
            gData->hit = HIT_HIT;
            f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
            f->smallBoatLives -= 1;
            return FIELD_POSITION_SMALL_BOAT;
        } 
        
        // Medium boat and still has more than one life
        else if (f->field[gData->row][gData->col] == FIELD_POSITION_MEDIUM_BOAT && f->mediumBoatLives != 1) {
            gData->hit = HIT_HIT;
            f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
            f->mediumBoatLives -= 1;
            return FIELD_POSITION_MEDIUM_BOAT;
        } 
        
        // Large boat and still has more than one life
        else if (f->field[gData->row][gData->col] == FIELD_POSITION_LARGE_BOAT && f->largeBoatLives != 1) {
            gData->hit = HIT_HIT;
            f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
            f->largeBoatLives -= 1;
            return FIELD_POSITION_LARGE_BOAT;
        } 
        
        // Huge boat and still has more than one life
        else if (f->field[gData->row][gData->col] == FIELD_POSITION_HUGE_BOAT && f->hugeBoatLives != 1) {
            gData->hit = HIT_HIT;
            f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
            f->hugeBoatLives -= 1;
            return FIELD_POSITION_HUGE_BOAT;
        } 
        
        // Small boat and only has one life
        else if (f->field[gData->row][gData->col] == FIELD_POSITION_SMALL_BOAT && f->smallBoatLives == 1) {
            gData->hit = HIT_SUNK_SMALL_BOAT;
            f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
            f->smallBoatLives -= 1;
            return FIELD_POSITION_SMALL_BOAT;
        } 
        
        // Medium boat and only has one life
        else if (f->field[gData->row][gData->col] == FIELD_POSITION_MEDIUM_BOAT && f->mediumBoatLives == 1) {
            gData->hit = HIT_SUNK_MEDIUM_BOAT;
            f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
            f->mediumBoatLives -= 1;
            return FIELD_POSITION_MEDIUM_BOAT;
        } 
        
        // Large boat and only has one life
        else if (f->field[gData->row][gData->col] == FIELD_POSITION_LARGE_BOAT && f->largeBoatLives == 1) {
            gData->hit = HIT_SUNK_LARGE_BOAT;
            f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
            f->largeBoatLives -= 1;
            return FIELD_POSITION_LARGE_BOAT;
        } 
        
        // Huge boat and only has one life
        else if (f->field[gData->row][gData->col] == FIELD_POSITION_HUGE_BOAT && f->hugeBoatLives == 1) {
            gData->hit = HIT_SUNK_HUGE_BOAT;
            f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
            f->hugeBoatLives -= 1;
            return FIELD_POSITION_HUGE_BOAT;
        }
    } 
    
    // For the miss case
    else {
        gData->hit = HIT_MISS;
        f->field[gData->row][gData->col] = FIELD_POSITION_MISS;
        return FIELD_POSITION_EMPTY;
    }
    return temporary2;
}

/**
 * This function updates the FieldState representing the opponent's game board with whether the
 * guess indicated within gData was a hit or not. If it was a hit, then the field is updated with a
 * FIELD_POSITION_HIT at that position. If it was a miss, display a FIELD_POSITION_EMPTY instead, as
 * it is now known that there was no boat there. The FieldState struct also contains data on how
 * many lives each ship has. Each hit only reports if it was a hit on any boat or if a specific boat
 * was sunk, this function also clears a boats lives if it detects that the hit was a
 * HIT_SUNK_*_BOAT.
 * @param f The field to grab data from.
 * @param gData The coordinates that were guessed along with their HitStatus.
 * @return The previous value of that coordinate position in the field before the hit/miss was
 * registered.
 */
FieldPosition FieldUpdateKnowledge(Field *f, const GuessData *gData)
{
    // Store the temp data
    FieldPosition temporary3 = f->field[gData->row][gData->col];
    
    // Check for hit 
    if (gData->hit != HIT_MISS) {
        
        // If hit return hit
        if (gData->hit == HIT_HIT){
            f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
        }
        
        // If there is no more lives for small boat return a 0
        else if (gData->hit == HIT_SUNK_SMALL_BOAT){
            f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
            f->smallBoatLives = 0;
        }
        
        // If there is no more lives for medium boat return a 0
        else if (gData->hit == HIT_SUNK_MEDIUM_BOAT){
            f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
            f->mediumBoatLives = 0;
        }
        
        // If there is no more lives for large boat return a 0
        else if (gData->hit == HIT_SUNK_LARGE_BOAT){
            f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
            f->largeBoatLives = 0;
        }
        
        // If there is no more lives for huge boat return a 0
        else if (gData->hit == HIT_SUNK_HUGE_BOAT){
            f->field[gData->row][gData->col] = FIELD_POSITION_HIT;
            f->hugeBoatLives = 0;
        }
    }
    
    // For the miss case 
    else if (gData->hit == HIT_MISS) {
        f->field[gData->row][gData->col] = FIELD_POSITION_MISS;
    }
    return temporary3;
}

/**
 * This function returns the alive states of all 4 boats as a 4-bit bitfield (stored as a uint8).
 * The boats are ordered from smallest to largest starting at the least-significant bit. So that:
 * 0b00001010 indicates that the small boat and large boat are sunk, while the medium and huge boat
 * are still alive. See the BoatStatus enum for the bit arrangement.
 * @param f The field to grab data from.
 * @return A 4-bit value with each bit corresponding to whether each ship is alive or not.
 */
uint8_t FieldGetBoatStates(const Field *f)
{
    // Create a integer
    uint8_t boatstates;
    
    // Set the value of the integer
    boatstates = (FIELD_BOAT_STATUS_HUGE | FIELD_BOAT_STATUS_LARGE | FIELD_BOAT_STATUS_MEDIUM | FIELD_BOAT_STATUS_SMALL);
    
    // AND and NOT the value in order to update value 
    if (f->smallBoatLives == 0) {
        boatstates = boatstates&(!FIELD_BOAT_STATUS_SMALL);
    }
    
    // AND and NOT the value in order to update value 
    else if (f->mediumBoatLives == 0) {
        boatstates = boatstates&(!FIELD_BOAT_STATUS_MEDIUM);
    }
    
    // AND and NOT the value in order to update value 
    else if (f->largeBoatLives == 0) {
        boatstates = boatstates&(!FIELD_BOAT_STATUS_LARGE);
    }
    
    // AND and NOT the value in order to update value 
    else if (f->hugeBoatLives == 0) {
        boatstates = boatstates&(!FIELD_BOAT_STATUS_HUGE);
    }
    return boatstates;
}