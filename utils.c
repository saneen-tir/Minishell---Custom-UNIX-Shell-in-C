/*
 * Author      : Muhammed Saneen P
 * Date        : 17-01-2026
 * Description : This file implements helper utilities for the Mini Shell project.
 *               It provides linked-list operations for managing stopped jobs,
 *               and functions to extract external commands from a file and
 *               populate the external command list used by the shell.
 */

#include "minishell.h"

extern char command[50];


void insert_at_first(struct list **head , int pid , char *input_string)
{
    struct list *new = malloc(sizeof(struct list));
    new->pid = pid;
    strcpy(new->input_string,input_string);
    new->link = *head;
    *head = new;
}

void delete_at_first(struct list **head)
{
    if(*head == NULL)
    {
        return;
    }
    struct list *temp = *head;
    *head = temp->link;
    free(temp);
}

void extract_external_commands(char **external_commands)
{
    int fd = open("external.txt",O_RDONLY);

    //temporarily stores the command and then copies to another location
    char temp[64];
    int i=0;
    
    //index of 2darray
    int j=0;

    char ch;
    while((read(fd,&ch,1)) == 1)
    {
        if(ch =='\n')
        {
            temp[i] = '\0';

            //should handle '\r' also
            if (i > 0 && temp[i-1] == '\r')
            temp[i-1] = '\0';

            external_commands[j++] = strdup(temp);
            i=0;
        }
        else
        {
            temp[i++] = ch;
        }
    }

    //the last command doesnt contain '\n' , so we handle it here
    temp[i] = '\0';
    external_commands[j++] = strdup(temp);

    external_commands[j] = NULL;

    close(fd);
}