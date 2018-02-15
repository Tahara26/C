/*
 * File:   LinkedList.c
 * Author: JustinT
 *
 * Created on February 8, 2018, 11:07 AM
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "BOARD.h"
#include "xc.h"
#include "LinkedList.h"

ListItem *LinkedListNew(char *data)
{
    // Creating my own list
    ListItem* myList = malloc(sizeof (ListItem));
    if (myList == NULL) {
        return NULL;
    }
        // Checking to see items around it and then make a new list
    else {
        myList->previousItem = NULL;
        myList->nextItem = NULL;
        myList->data = data;
        return myList;
    }
}

char *LinkedListRemove(ListItem *item)
{
    char* temp;
    if (item == NULL) {
        return NULL;
    }

    // Single item list 
    if ((item->previousItem == NULL) && (item->nextItem == NULL)) {
    }        // Removes the previous Item from the next Item 
    else if ((item->previousItem == NULL) && (item->nextItem != NULL)) {
        item->nextItem->previousItem = NULL;
    }
        // Removes the next Item from the previous Item 
    else if ((item->nextItem == NULL) && (item->previousItem != NULL)) {
        item->previousItem->nextItem = NULL;
    }
        // Links the two items from the previous and next
    else {
        item->nextItem->previousItem = item->previousItem;
        item->previousItem->nextItem = item->nextItem;
    }

    temp = item->data;
    free(item);
    return temp;
}

int LinkedListSize(ListItem *list)
{
    // Sets up counter 
    int size = 0;

    // Makes a new item
    ListItem* current = list;
    while (current != NULL) {
        size++;
        current = current->nextItem;
    }

    // Returns the size of the list
    return size;
}

ListItem *LinkedListGetFirst(ListItem *list)
{
    if (list == NULL) {
        return NULL;
    }
    ListItem *new = list;

    // If the previous Item is NULL that means we are at the head
    if (new->previousItem == NULL) {
        return new;
    }

    // When the list is valid it iterates through the list
    while (new->previousItem != NULL) {
        // Goes to previous Item
        new = new->previousItem;
    }
    return new;
}

ListItem *LinkedListCreateAfter(ListItem *item, char *data)
{
    // Makes a new item
    ListItem* myItem = (ListItem *) malloc(sizeof (ListItem));
    if (myItem == NULL) {
        return NULL;
    }

    // If item is by itself then create after and link
    if (item == NULL) {
        myItem->data = data;
        myItem->previousItem = NULL;
        myItem->nextItem = NULL;
    }
        // Case for the tail linking 
    else if (item->nextItem == NULL) {
        item->nextItem = myItem;
        myItem->data = data;
        myItem->nextItem = NULL;
        myItem->previousItem = item;
    }        // The case with both a next and previous item and adding a new item after
    else {
        myItem->data = data;
        myItem->nextItem = item->nextItem;
        item->nextItem->previousItem = myItem;
        item->nextItem = myItem;
        myItem->previousItem = item;
    }

    return myItem;
}

int LinkedListSwapData(ListItem *firstItem, ListItem *secondItem)
{
    char *temp = NULL;

    // Checks to make sure both Items are not Null
    if (firstItem == NULL || secondItem == NULL) {
        return STANDARD_ERROR;
    }
        // Swaps the items and returns a Success
    else {
        temp = firstItem->data;
        firstItem->data = secondItem->data;
        secondItem->data = temp;
        return SUCCESS;
    }
}

int LinkedListSort(ListItem *list)
{
   
    if (list == NULL) {
        return STANDARD_ERROR;
    } else { 
        
        // Making a head and next to use as temporary value
        ListItem *head = LinkedListGetFirst(list);
        ListItem *next = head->nextItem;

        // Nested for loop to go through and sort through size
        int i, j;
        for (i = 0; i < LinkedListSize(list) - 2; i++) {
            for (j = i + 1; j < LinkedListSize(list) - 1; j++) {

                // If head is null just shift to the next item
                if (head->data == NULL) {
                    head = next;
                    next = next->nextItem;
                }

                // If next is null swap with the head and then shift to the next item
                if (next->data == NULL) {
                    LinkedListSwapData(head, next);
                    head = next;
                    next = next->nextItem;
                }

                // Checks the string length and if the next item is larger than the head swap and shift
                if (strlen(head->data) > strlen(next->data)) {
                    LinkedListSwapData(head, next);
                    head = next;
                    next = next->nextItem;
                }

                // If next is larger than head just shift
                if (strlen(head->data) < strlen(next->data)) {
                    head = next;
                    next = next->nextItem;
                }
            }
        }

        // Resets the head
        head = LinkedListGetFirst(list);
        i = 0;
        j = 0;

        // Nested for loop to go through and sort alphabetically
        for (i = 0; i < LinkedListSize(list) - 2; i++) {
            for (j = i + 1; j < LinkedListSize(list) - 1; j++) {

                // If head is null just shift to the next item
                if (head->data == NULL) {
                    break;
                    head = next;
                    next = next->nextItem;
                }                    // If next is null swap with the head and then shift to the next item
                else if (next->data == NULL) {
                    LinkedListSwapData(head, next);
                    head = next;
                    next = next->nextItem;
                }                    // Compares the strings and if the next item is larger than the head swap and shift
                else if (strcmp(head->data, next->data) > 0) {
                    LinkedListSwapData(list->nextItem, list);
                    head = next;
                    next = next->nextItem;
                } else if (strcmp(head->data, next->data) <= 0) {
                    head = next;
                    next = next->nextItem;
                }

            }
        }

        return SUCCESS;
    }
}

int LinkedListPrint(ListItem *list)
{
    // If null just return a Standard Error
    if (list == NULL) {
        return STANDARD_ERROR;
    }

    list = LinkedListGetFirst(list);
    // Gets the sorted list and prints them out in order
    printf("[");
    while (list->nextItem != NULL) {
        printf("%s, ", list->data);
        list = list->nextItem;
    }
    printf("%s]\n", list->data);
    return SUCCESS;
}
