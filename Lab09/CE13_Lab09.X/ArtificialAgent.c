/*
 * File:   ArtificialAgent.c
 * Author: Justin Tahara and Rayne Jones
 *
 * Created on March 9, 2018, 11:13 AM
 */

#include <stdint.h>

#include "Agent.h"
#include "Field.h"
#include "FieldOled.h"
#include "Protocol.h"
#include "Oled.h"
#include "xc.h"

static Field myField;
static Field enemyField;
static NegotiationData myData;
static NegotiationData enemyData;
static GuessData myGuess;
static GuessData enemyGuess;
static ProtocolParserStatus parsingStatus = PROTOCOL_WAITING;
static AgentEvent agentStatus = AGENT_EVENT_NONE;
static uint8_t boatStatus = 0;

static FieldOledTurn playerTurn = FIELD_OLED_TURN_NONE;

static AgentState agentState = AGENT_STATE_GENERATE_NEG_DATA;

static AgentEvent ProtocolToAgentEvent(ProtocolParserStatus input);

/**
 * The Init() function for an Agent sets up everything necessary for an agent before the game
 * starts. This can include things like initialization of the field, placement of the boats,
 * etc. The agent can assume that stdlib's rand() function has been seeded properly in order to
 * use it safely within.
 */
void AgentInit(void)
{
    int saveVal;
    FieldInit(&myField, FIELD_POSITION_EMPTY);
    FieldInit(&enemyField, FIELD_POSITION_UNKNOWN);
    myData.encryptedGuess = 0;
    myData.encryptionKey = 0;
    myData.guess = 0;
    myData.hash = 0;
    myGuess.col = 0;
    myGuess.row = 0;
    myGuess.hit = 0;

    enemyData.encryptedGuess = 0;
    enemyData.encryptionKey = 0;
    enemyData.guess = 0;
    enemyData.hash = 0;
    enemyGuess.col = 0;
    enemyGuess.row = 0;
    enemyGuess.hit = 0;

    //    FieldAddBoat(&myField, 0, 0, 1, FIELD_BOAT_SMALL);
    do {
        saveVal = FieldAddBoat(&myField, rand() % FIELD_ROWS, rand() % FIELD_COLS, rand() % 4, FIELD_BOAT_SMALL);
    } while (saveVal != TRUE);

    do {
        saveVal = FieldAddBoat(&myField, rand() % FIELD_ROWS, rand() % FIELD_COLS, rand() % 4, FIELD_BOAT_MEDIUM);
    } while (saveVal != TRUE);

    do {
        saveVal = FieldAddBoat(&myField, rand() % FIELD_ROWS, rand() % FIELD_COLS, rand() % 4, FIELD_BOAT_LARGE);
    } while (saveVal != TRUE);

    do {
        saveVal = FieldAddBoat(&myField, rand() % FIELD_ROWS, rand() % FIELD_COLS, rand() % 4, FIELD_BOAT_HUGE);
    } while (saveVal != TRUE);
    FieldOledDrawScreen(&myField, &enemyField, playerTurn);
}

/**
 * The Run() function for an Agent takes in a single character. It then waits until enough
 * data is read that it can decode it as a full sentence via the Protocol interface. This data
 * is processed with any output returned via 'outBuffer', which is guaranteed to be 255
 * characters in length to allow for any valid NMEA0183 messages. The return value should be
 * the number of characters stored into 'outBuffer': so a 0 is both a perfectly valid output and
 * means a successful run.
 * @param in The next character in the incoming message stream.
 * @param outBuffer A string that should be transmit to the other agent. NULL if there is no
 *                  data.
 * @return The length of the string pointed to by outBuffer (excludes \0 character).
 */
int AgentRun(char in, char *outBuffer)
{
    //    //message parser
    //    // call decode
    //    //store message recieved
    //    if(in != 0){
    //        printf("%c", in);
    //        parsingStatus = ProtocolDecode(in, &enemyData, &enemyGuess);
    //        agentStatus = ProtocolToAgentEvent(parsingStatus);
    //    }
    //
    //    if (agentStatus == AGENT_EVENT_RECEIVED_CHA_MESSAGE){
    //        printf("\nReceived Challenge Messages!!!!!!!");
    //    } else if (agentStatus == AGENT_EVENT_RECEIVED_DET_MESSAGE){
    //        printf("\nreceived DET message");
    //    } else if (agentStatus == AGENT_EVENT_RECEIVED_COO_MESSAGE){
    //        printf("\nreceived COO message");
    //    } else if (agentStatus == AGENT_EVENT_RECEIVED_HIT_MESSAGE){
    //        printf("\nreceived HIT message");
    //    }
    //    return 0;
    if (in != '\0') {
        parsingStatus = ProtocolDecode(in, &enemyData, &enemyGuess);
        switch (agentState) {
        case AGENT_STATE_GENERATE_NEG_DATA:
            //generate negotiation data
            ProtocolGenerateNegotiationData(&myData);
            agentState = AGENT_STATE_SEND_CHALLENGE_DATA;
            //send challenge data
            return ProtocolEncodeChaMessage(outBuffer, &myData);
            break;
        case AGENT_STATE_SEND_CHALLENGE_DATA:
            //decode enemy data
            //parsingStatus = ProtocolDecode(in, &enemyData, &enemyGuess);
            agentStatus = ProtocolToAgentEvent(parsingStatus);
            if (agentStatus == AGENT_EVENT_RECEIVED_CHA_MESSAGE) {
                //change agent state
                agentState = AGENT_STATE_DETERMINE_TURN_ORDER;
                //send determine data
                return ProtocolEncodeDetMessage(outBuffer, &myData);
            } else if (agentStatus == AGENT_EVENT_MESSAGE_PARSING_FAILED) {
                //clear screen and draw error message
                OledClear(OLED_COLOR_BLACK);
                OledDrawString(AGENT_ERROR_STRING_PARSING);
                //                OledDrawString("failing send challenge data");
                OledUpdate();
                agentState = AGENT_STATE_INVALID;
                outBuffer = NULL;
            }
            break;
        case AGENT_STATE_DETERMINE_TURN_ORDER:
            //decode enemy data
            //parsingStatus = ProtocolDecode(in, &enemyData, &enemyGuess);
            agentStatus = ProtocolToAgentEvent(parsingStatus);
            if (agentStatus == AGENT_EVENT_RECEIVED_DET_MESSAGE) {
                if (ProtocolValidateNegotiationData(&enemyData)) {
                    if (ProtocolGetTurnOrder(&myData, &enemyData) == TURN_ORDER_TIE) {
                        //set turn to NONE
                        playerTurn = FIELD_OLED_TURN_NONE;
                        FieldOledDrawScreen(&myField, &enemyField, playerTurn);
                        //if the turn order is a tie, clear the screen and print error message
                        OledClear(OLED_COLOR_BLACK);
                        OledDrawString(AGENT_ERROR_STRING_ORDERING);
                        //                        OledDrawString("turn order tied");
                        OledUpdate();
                        agentState = AGENT_STATE_INVALID;
                        outBuffer = NULL;
                    } else if (ProtocolGetTurnOrder(&myData, &enemyData) == TURN_ORDER_START) {
                        //set turn to MINE
                        playerTurn = FIELD_OLED_TURN_MINE;
                        //update screen
                        FieldOledDrawScreen(&myField, &enemyField, playerTurn);
                        agentState = AGENT_STATE_SEND_GUESS;
                        outBuffer = NULL;
                    } else if (ProtocolGetTurnOrder(&myData, &enemyData) == TURN_ORDER_DEFER) {
                        //set turn to THEIRS
                        playerTurn = FIELD_OLED_TURN_THEIRS;
                        //update screen
                        FieldOledDrawScreen(&myField, &enemyField, playerTurn);
                        agentState = AGENT_STATE_WAIT_FOR_GUESS;
                        outBuffer = NULL;
                    }
                } else {
                    //if the enemy data is invalid, print error message
                    OledClear(OLED_COLOR_BLACK);
                    OledDrawString(AGENT_ERROR_STRING_NEG_DATA);
                    //                    OledDrawString("failed validating negotiation");
                    OledUpdate();
                    agentState = AGENT_STATE_INVALID;
                    outBuffer = NULL;
                }
            } else if (agentStatus == AGENT_EVENT_MESSAGE_PARSING_FAILED) {
                //if the enemy data failed to parse, print error message
                OledClear(OLED_COLOR_BLACK);
                OledDrawString(AGENT_ERROR_STRING_PARSING);
                OledUpdate();
                agentState = AGENT_STATE_INVALID;
                outBuffer = NULL;
            }
            break;
        case AGENT_STATE_SEND_GUESS:
            ;
            //if generated valid coordinates
            int i;
            for (i = 0; i < (BOARD_GetPBClock() >> 3); i++);
            myGuess.row = rand() % (FIELD_ROWS - 1);
            myGuess.col = rand() % (FIELD_COLS - 1);
            //myGuess.hit = FieldAt(&enemyField, myGuess.row, myGuess.col);
            //change agent state
            agentState = AGENT_STATE_WAIT_FOR_HIT;
            //send coo message
            return ProtocolEncodeCooMessage(outBuffer, &myGuess);
            break;
        case AGENT_STATE_WAIT_FOR_HIT:
            //parsingStatus = ProtocolDecode(in, &enemyData, &enemyGuess);
            agentStatus = ProtocolToAgentEvent(parsingStatus);
            boatStatus = AgentGetEnemyStatus();
            if (agentStatus == AGENT_EVENT_RECEIVED_HIT_MESSAGE) {
                //check enemy boat status
                if (boatStatus != 0) {
                    //update knowledge of enemy field
                    FieldUpdateKnowledge(&enemyField, &myGuess);
                    //set turn to theirs
                    playerTurn = FIELD_OLED_TURN_THEIRS;
                    //update screen
                    FieldOledDrawScreen(&myField, &enemyField, playerTurn);
                    agentState = AGENT_STATE_WAIT_FOR_GUESS;
                    outBuffer = NULL;
                    return 0;
                } else {
                    //set turn to NONE
                    playerTurn = FIELD_OLED_TURN_NONE;
                    //update screen
                    FieldOledDrawScreen(&myField, &enemyField, playerTurn);
                    agentState = AGENT_STATE_WON;
                    outBuffer = NULL;
                    return 0;
                }
            }
            break;
        case AGENT_STATE_WAIT_FOR_GUESS:
            //parsingStatus = ProtocolDecode(in, &enemyData, &enemyGuess);
            agentStatus = ProtocolToAgentEvent(parsingStatus);
            boatStatus = AgentGetStatus();
            FieldOledDrawScreen(&myField, &enemyField, playerTurn);
            if (agentStatus == AGENT_EVENT_RECEIVED_COO_MESSAGE) {
                //register attack
                FieldRegisterEnemyAttack(&myField, &enemyGuess);
                //check boat status
                if (boatStatus == 0) {
                    //set turn to NONE
                    playerTurn = FIELD_OLED_TURN_NONE;
                    //update screen
                    FieldOledDrawScreen(&myField, &enemyField, playerTurn);
                    //send hit message
                    agentState = AGENT_STATE_LOST;
                    return ProtocolEncodeHitMessage(outBuffer, &enemyGuess);
                } else {
                    //set turn to MINE
                    playerTurn = FIELD_OLED_TURN_MINE;
                    //update screen
                    FieldOledDrawScreen(&myField, &enemyField, playerTurn);
                    //send hit message
                    agentState = AGENT_STATE_SEND_GUESS;
                    return ProtocolEncodeHitMessage(outBuffer, &enemyGuess);
                }
            } else if (agentStatus == AGENT_EVENT_MESSAGE_PARSING_FAILED) {
                OledClear(OLED_COLOR_BLACK);
                OledDrawString(AGENT_ERROR_STRING_PARSING);
                OledUpdate();
                agentState = AGENT_STATE_INVALID;
                outBuffer = NULL;
            }
            break;
        case AGENT_STATE_INVALID:
            return 0;
            break;
        case AGENT_STATE_LOST:
            return 0;
            break;
        case AGENT_STATE_WON:
            return 0;
            break;
        default:
            return 0;
        }
        return 0;
    }
    return 0;
}

/**
 * StateCheck() returns a 4-bit number indicating the status of that agent's ships. The smallest
 * ship, the 3-length one, is indicated by the 0th bit, the medium-length ship (4 tiles) is the
 * 1st bit, etc. until the 3rd bit is the biggest (6-tile) ship. This function is used within
 * main() to update the LEDs displaying each agents' ship status. This function is similar to
 * Field::FieldGetBoatStates().
 * @return A bitfield indicating the sunk/unsunk status of each ship under this agent's control.
 *
 * @see Field.h:FieldGetBoatStates()
 * @see Field.h:BoatStatus
 */
uint8_t AgentGetStatus(void)
{
    return FieldGetBoatStates(&myField);
}

/**
 * This function returns the same data as `AgentCheckState()`, but for the enemy agent.
 * @return A bitfield indicating the sunk/unsunk status of each ship under the enemy agent's
 *         control.
 *
 * @see Field.h:FieldGetBoatStates()
 * @see Field.h:BoatStatus
 */
uint8_t AgentGetEnemyStatus(void)
{
    return FieldGetBoatStates(&enemyField);
}

static AgentEvent ProtocolToAgentEvent(ProtocolParserStatus input)
{
    switch (input) {
    case PROTOCOL_PARSING_FAILURE:
        return AGENT_EVENT_MESSAGE_PARSING_FAILED;
        break;
    case PROTOCOL_WAITING:
        return AGENT_EVENT_NONE;
        break;
    case PROTOCOL_PARSING_GOOD:
        return AGENT_EVENT_NONE;
        break;
    case PROTOCOL_PARSED_COO_MESSAGE:
        return AGENT_EVENT_RECEIVED_COO_MESSAGE;
        break;
    case PROTOCOL_PARSED_HIT_MESSAGE:
        return AGENT_EVENT_RECEIVED_HIT_MESSAGE;
        break;
    case PROTOCOL_PARSED_CHA_MESSAGE:
        return AGENT_EVENT_RECEIVED_CHA_MESSAGE;
        break;
    case PROTOCOL_PARSED_DET_MESSAGE:
        return AGENT_EVENT_RECEIVED_DET_MESSAGE;
        break;
    default:
        return AGENT_EVENT_NONE;
    }
}
