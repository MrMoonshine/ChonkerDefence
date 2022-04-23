#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>

//Insert at the first position
Path * insertPath(Path **head, uint8_t x, uint8_t y){
    Path *elem = (Path*) malloc(sizeof(Path));
    if(elem == NULL)
        return elem;
    //Assign data
    elem->x = x;
    elem->y = y;
    elem->follow = NULL;
    //Next node is head
    elem->next = *head;
    // this is the new First node
    *head = elem;
    //Just to get a reference...
    return elem;
}
//Delete a given element
void deletePath(Path **head, Path *elem){
    Path *tmp = *head;
    Path *prev = NULL;
    while(tmp){
        if(tmp == elem){
            if(prev)
                prev->next = tmp->next;
            else
                *head = tmp->next;
            free(tmp);
            return;
        }
        prev = tmp;
        tmp = tmp->next;
    }
}
// Delete Last element
void deleteLastPath(Path **head){
    Path *tmp = *head;
    Path *prev = NULL;
    while(tmp){
        if(tmp->next == NULL){
            if(prev)
                prev->next = tmp->next;
            else
                *head = tmp->next;
            free(tmp);
            return;
        }
        prev = tmp;
        tmp = tmp->next;
    }
}
//Purge the entire list
void deleteAllPath(Path **head){
    while(*head){
        Path *tmp = *head;
        *head = (*head)->next;
        free(tmp);
    }
    *head = NULL;
}
//Dump it
void printPath(Path *head){
    Path *elem = head;
    while(elem){
        SDL_Log("Path (%d|%d)", elem->x, elem->y);
        elem = elem->next;
    }
    SDL_Log("\n");
}

size_t lengthPath(Path *head){
    size_t length = 0;
    Path *elem = head;
    while(elem){
        length++;
        elem = elem->next;
    }
    return length;
}
