#include "mouse.h"


//Insert at the first position
MouseLI * insertMouseLI(MouseLI **head, const char *data){
    MouseLI *elem = (MouseLI*) malloc(sizeof(MouseLI));
    if(elem == NULL)
        return elem;
    //Assign data
    memset(elem->word, 0, 16);
    strcpy(elem->word, data);
    //Next node is head
    elem->next = *head;
    // this is the new First node
    *head = elem;
    //Just to get a reference...
    return elem;
}
//Delete a given element
void deleteMouseLI(MouseLI *head, MouseLI *elem){
    MouseLI *tmp = head;
    MouseLI *prev = NULL;
    while(tmp){
        if(tmp == elem){
            prev->next = tmp->next;
            free(tmp);
        }
        prev = tmp;
        tmp = tmp->next;
    }
}
//Purge the entire list
void deleteAllMouseLI(MouseLI **head){
    while(*head){
        MouseLI *tmp = *head;
        *head = (*head)->next;
        free(tmp);
    }
    *head = NULL;
}
//Dump it
void printMouseLI(MouseLI *head){
    MouseLI *elem = head;
    while(elem){
        printf("%s ", elem->word);
        elem = elem->next;
    }
    printf("\n");
}
