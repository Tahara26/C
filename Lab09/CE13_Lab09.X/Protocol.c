/*
 * File: Protocol.c
 * Author: Rayne Jones
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Protocol.h"
#include "BOARD.h"

#define MSG_ID_LEN 3
#define MIDDLE_CHAR 1

typedef enum {
    WAITING,
    RECORDING,
    FIRST_CHECKSUM_HALF,
    SECOND_CHECKSUM_HALF,
    NEWLINE
} DecodeState;

typedef struct StateData {
    DecodeState state;
    uint8_t index;
    char sentence[PROTOCOL_MAX_MESSAGE_LEN];
    uint8_t checksum;
} StateData;

static uint8_t GetStringChecksum(char *);
static uint8_t AsciiConvert(char);

/**
 * Encodes the coordinate data for a guess into the string `message`. This string must be big
 * enough to contain all of the necessary data. The format is specified in PAYLOAD_TEMPLATE_COO,
 * which is then wrapped within the message as defined by MESSAGE_TEMPLATE. The final length of this
 * message is then returned. There is no failure mode for this function as there is no checking
 * for NULL pointers.
 * @param message The character array used for storing the output. Must be long enough to store the
 *                entire string, see PROTOCOL_MAX_MESSAGE_LEN.
 * @param data The data struct that holds the data to be encoded into `message`.
 * @return The length of the string stored into `message`.
 */
int ProtocolEncodeCooMessage(char *message, const GuessData *data)
{
    //create temporary values for storage into the message
    char temp[PROTOCOL_MAX_PAYLOAD_LEN];
    uint8_t checksum;

    //generate the payload
    sprintf(temp, PAYLOAD_TEMPLATE_COO, data->row, data->col);

    //generate the checksum
    checksum = GetStringChecksum(temp);

    //generate the message
    sprintf(message, MESSAGE_TEMPLATE, temp, checksum);
    return SUCCESS;
}

/**
 * Follows from ProtocolEncodeCooMessage above.
 */
int ProtocolEncodeHitMessage(char *message, const GuessData *data)
{
    //create temporary values for storage into the message
    char temp[PROTOCOL_MAX_PAYLOAD_LEN];
    uint8_t checksum;

    //generate the payload
    sprintf(temp, PAYLOAD_TEMPLATE_HIT, data->row, data->col, data->hit);

    //generate the checksum
    checksum = GetStringChecksum(temp);

    //generate the message
    sprintf(message, MESSAGE_TEMPLATE, temp, checksum);
    return SUCCESS;
}

/**
 * Follows from ProtocolEncodeCooMessage above.
 */
int ProtocolEncodeChaMessage(char *message, const NegotiationData *data)
{
    //create temporary values for storage into the message
    char temp[PROTOCOL_MAX_PAYLOAD_LEN];
    uint8_t checksum;

    //generate the payload
    sprintf(temp, PAYLOAD_TEMPLATE_CHA, data->encryptedGuess, data->hash);

    //generate the checksum
    checksum = GetStringChecksum(temp);

    //generate the message
    sprintf(message, MESSAGE_TEMPLATE, temp, checksum);
    return SUCCESS;
}

/**
 * Follows from ProtocolEncodeCooMessage above.
 */
int ProtocolEncodeDetMessage(char *message, const NegotiationData *data)
{
    //create temporary values for storage into the message
    char temp[PROTOCOL_MAX_PAYLOAD_LEN];
    uint8_t checksum;

    //generate the payload
    sprintf(temp, PAYLOAD_TEMPLATE_DET, data->guess, data->encryptionKey);

    //generate the checksum
    checksum = GetStringChecksum(temp);

    //generate the message
    sprintf(message, MESSAGE_TEMPLATE, temp, checksum);
    return SUCCESS;
}

/**
 * This function decodes a message into either the NegotiationData or GuessData structs depending
 * on what the type of message is. This function receives the message one byte at a time, where the
 * messages are in the format defined by MESSAGE_TEMPLATE, with payloads of the format defined by
 * the PAYLOAD_TEMPLATE_* macros. It returns the type of message that was decoded and also places
 * the decoded data into either the `nData` or `gData` structs depending on what the message held.
 * The onus is on the calling function to make sure the appropriate structs are available (blame the
 * lack of function overloading in C for this ugliness).
 *
 * PROTOCOL_PARSING_FAILURE is returned if there was an error of any kind (though this excludes
 * checking for NULL pointers), while
 * 
 * @param in The next character in the NMEA0183 message to be decoded.
 * @param nData A struct used for storing data if a message is decoded that stores NegotiationData.
 * @param gData A struct used for storing data if a message is decoded that stores GuessData.
 * @return A value from the UnpackageDataEnum enum.
 */
ProtocolParserStatus ProtocolDecode(char in, NegotiationData *nData, GuessData *gData)
{
    //char arrays for testing MSG_ID
    char id1[] = {'D', 'E', 'T'}, id2[] = {'C', 'H', 'A'}, id3[] = {'C', 'O', 'O'}, id4[] = {'H', 'I', 'T'};
    //state data struct
    static StateData stateData;

    switch (stateData.state) {
    case WAITING:
        //check for $ character
        if (in == '$') {
            stateData.index = 0;
            stateData.state = RECORDING;
            return PROTOCOL_PARSING_GOOD;
        } else {
            //case for non $ character
            return PROTOCOL_WAITING;
        }
        break;
    case RECORDING:
        //check for asterisk character
        if (in == '*') {
            stateData.state = FIRST_CHECKSUM_HALF;
            return PROTOCOL_PARSING_GOOD;
        } else {
            //case for non-asterisk character
            stateData.sentence[stateData.index] = in;
            (stateData.index)++;
            return PROTOCOL_PARSING_GOOD;
        }
        break;
    case FIRST_CHECKSUM_HALF:
        //check for valid ascii hex character
        if ((in >= '0' && in <= '9') || (in >= 'A' && in <= 'F') || (in >= 'a' && in <= 'f')) {
            //case if character is valid
            uint8_t temp = AsciiConvert(in);
            temp <<= 4;
            stateData.checksum = temp;
            stateData.state = SECOND_CHECKSUM_HALF;
            return PROTOCOL_PARSING_GOOD;
        } else {
            //case if character is not valid
            stateData.state = WAITING;
            return PROTOCOL_PARSING_FAILURE;
        }
        break;
    case SECOND_CHECKSUM_HALF:
        //check for valid ascii hex character
        if ((in >= '0' && in <= '9') || (in >= 'A' && in <= 'F') || (in >= 'a' && in <= 'f')) {
            uint8_t temp = AsciiConvert(in);
            stateData.checksum |= temp;
            //if checksum matching passes
            if (stateData.checksum == GetStringChecksum(stateData.sentence)) {
                stateData.sentence[stateData.index] = '\0';
                stateData.state = NEWLINE;
                return PROTOCOL_PARSING_GOOD;
            } else {
                //if checksum matching does not pass
                stateData.state = WAITING;
                return PROTOCOL_PARSING_FAILURE;
            }
        } else {
            stateData.state = WAITING;
            return PROTOCOL_PARSING_FAILURE;
        }
        break;
    case NEWLINE:
        //check for newline char
        if (in == '\n') {
            unsigned data1, data2, data3;
            if ((stateData.sentence[0] == id1[0]) && (stateData.sentence[1] == id1[1]) && (stateData.sentence[2] == id1[2])) {
                //DET
                //NegotiationData
                if (sscanf(stateData.sentence, PAYLOAD_TEMPLATE_DET, &data1, &data2) == 2) {
                    nData->guess = data1;
                    nData->encryptionKey = data2;
                    return PROTOCOL_PARSED_DET_MESSAGE;
                } else {
                    return PROTOCOL_PARSING_FAILURE;
                }
            } else if ((stateData.sentence[0] == id2[0]) && (stateData.sentence[1] == id2[1]) && (stateData.sentence[2] == id2[2])) {
                //CHA
                //NegotiationData
                if (sscanf(stateData.sentence, PAYLOAD_TEMPLATE_CHA, &data1, &data2) == 2) {
                    nData->encryptedGuess = data1;
                    nData->hash = data2;
                    return PROTOCOL_PARSED_CHA_MESSAGE;
                } else {
                    return PROTOCOL_PARSING_FAILURE;
                }
            } else if ((stateData.sentence[0] == id3[0]) && (stateData.sentence[1] == id3[1]) && (stateData.sentence[2] == id3[2])) {
                //COO
                //GuessData
                if (sscanf(stateData.sentence, PAYLOAD_TEMPLATE_COO, &data1, &data2) == 2) {
                    gData->row = data1;
                    gData->col = data2;
                    return PROTOCOL_PARSED_COO_MESSAGE;
                } else {
                    return PROTOCOL_PARSING_FAILURE;
                }
            } else if ((stateData.sentence[0] == id4[0]) && (stateData.sentence[1] == id4[1]) && (stateData.sentence[2] == id4[2])) {
                //HIT
                //GuessData
                if (sscanf(stateData.sentence, PAYLOAD_TEMPLATE_HIT, &data1, &data2, &data3) == 3) {
                    gData->row = data1;
                    gData->col = data2;
                    gData->hit = data3;
                    return PROTOCOL_PARSED_HIT_MESSAGE;
                } else {
                    return PROTOCOL_PARSING_FAILURE;
                }
            } else {
                //case for invalid MSG ID
                stateData.state = WAITING;
                return PROTOCOL_PARSING_FAILURE;
            }
        } else {
            //case for non-newline char
            stateData.state = WAITING;
            return PROTOCOL_PARSING_FAILURE;
        }
        break;
    default:
        return PROTOCOL_PARSING_FAILURE;
    }
}

/**
 * This function generates all of the data necessary for the negotiation process used to determine
 * the player that goes first. It relies on the pseudo-random functionality built into the standard
 * library. The output is stored in the passed NegotiationData struct. The negotiation data is
 * generated by creating two random 16-bit numbers, one for the actual guess and another for an
 * encryptionKey used for encrypting the data. The 'encryptedGuess' is generated with an
 * XOR(guess, encryptionKey). The hash is simply an 8-bit value that is the XOR() of all of the
 * bytes making up both the guess and the encryptionKey. There is no checking for NULL pointers
 * within this function.
 * @param data The struct used for both input and output of negotiation data.
 */
void ProtocolGenerateNegotiationData(NegotiationData *data)
{
    //generate data and encryption key
    data->guess = (rand() & 0xFFFF);
    data->encryptionKey = (rand() & 0xFFFF);

    //generate encrypted guess
    data->encryptedGuess = (data->encryptionKey) ^ (data->guess);

    //generate hash
    data->hash = (data->encryptionKey & 0x00FF) ^ (data->encryptionKey >> 8)
            ^ (data->guess & 0x00FF) ^ (data->guess >> 8);
}

/**
 * Validates that the negotiation data within 'data' is correct according to the algorithm given in
 * GenerateNegotitateData(). Used for verifying another agent's supplied negotiation data. There is
 * no checking for NULL pointers within this function. Returns TRUE if the NegotiationData struct
 * is valid or FALSE on failure.
 * @param data A filled NegotiationData struct that will be validated.
 * @return TRUE if the NegotiationData struct is consistent and FALSE otherwise.
 */
uint8_t ProtocolValidateNegotiationData(const NegotiationData *data)
{
    int counter = 0;
    //verify that the encrypted guess is actually the guess encrypted with the encryption key
    if ((data->encryptedGuess ^ data->encryptionKey) == data->guess) {
        counter++;
    }
    //verify that the hash is actually the xors of all the bytes of the encryption key and guess
    if (data->hash == ((data->encryptionKey & 0x00FF) ^ (data->encryptionKey >> 8)
            ^ (data->guess & 0x00FF) ^ (data->guess >> 8))) {
        counter++;
    }
    if (counter == 2) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * This function returns a TurnOrder enum type representing which agent has won precedence for going
 * first. The value returned relates to the agent whose data is in the 'myData' variable. The turn
 * ordering algorithm relies on the XOR() of the 'encryptionKey' used by both agents. The least-
 * significant bit of XOR(myData.encryptionKey, oppData.encryptionKey) is checked so that if it's a
 * 1 the player with the largest 'guess' goes first otherwise if it's a 0, the agent with the
 * smallest 'guess' goes first. The return value of TURN_ORDER_START indicates that 'myData' won,
 * TURN_ORDER_DEFER indicates that 'oppData' won, otherwise a tie is indicated with TURN_ORDER_TIE.
 * There is no checking for NULL pointers within this function.
 * @param myData The negotiation data representing the current agent.
 * @param oppData The negotiation data representing the opposing agent.
 * @return A value from the TurnOrdering enum representing which agent should go first.
 */
TurnOrder ProtocolGetTurnOrder(const NegotiationData *myData, const NegotiationData *oppData)
{
    //order algorithm data
    uint16_t temp = myData->encryptionKey ^ oppData->encryptionKey;
    //check if the algorithm data's LSB is 1 or 0
    if ((temp & 0x0001) != 0) {
        //case for LSB == 1
        if (myData->guess > oppData->guess) {
            return TURN_ORDER_START;
        } else if (myData->guess < oppData->guess) {
            return TURN_ORDER_DEFER;
        } else {
            return TURN_ORDER_TIE;
        }
    } else {
        //case for LSB == 0
        if (myData->guess > oppData->guess) {
            return TURN_ORDER_DEFER;
        } else if (myData->guess < oppData->guess) {
            return TURN_ORDER_START;
        } else {
            return TURN_ORDER_TIE;
        }
    }
}

static uint8_t GetStringChecksum(char *input)
{
    uint8_t output = *input;
    int i = 1;
    for (; i < strlen(input); i++) {
        output ^= (uint8_t) (*(input + i));
    }
    return output;
}

static uint8_t AsciiConvert(char input)
{
    switch (input) {
    case '0':
        return 0;
        break;
    case '1':
        return 1;
        break;
    case '2':
        return 2;
        break;
    case '3':
        return 3;
        break;
    case '4':
        return 4;
        break;
    case '5':
        return 5;
        break;
    case '6':
        return 6;
        break;
    case '7':
        return 7;
        break;
    case '8':
        return 8;
        break;
    case '9':
        return 9;
        break;
    case 'a':
    case 'A':
        return 10;
        break;
    case 'b':
    case 'B':
        return 11;
        break;
    case 'c':
    case 'C':
        return 12;
        break;
    case 'd':
    case 'D':
        return 13;
        break;
    case 'e':
    case 'E':
        return 14;
        break;
    case 'f':
    case 'F':
        return 15;
        break;
    default:
        return -1;
        break;
    }
}