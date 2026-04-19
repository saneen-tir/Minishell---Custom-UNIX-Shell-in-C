/*
 * Author      : Muhammed Saneen P
 * Date        : 17-01-2026
 * Description : This header file defines macros, data structures, and function
 *               prototypes used across the Mini Shell project. It provides
 *               common declarations for command classification, ANSI color
 *               formatting, signal handling, job control using linked lists,
 *               and interfaces for internal and external command execution.
 */
#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
 

#define BUILTIN		1
#define EXTERNAL	2
#define NO_COMMAND  3

#define BOLD "\x1b[1m"
#define RESET "\x1b[0m"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[92m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"


struct list
{
    int pid;
    char input_string[50];
    struct list *link;
};


void scan_input(char *prompt, char *input_string);
char *get_command(char *input_string);
// void copy_change(char *prompt, char *input_string);
int check_command_type(char *command,char **external_commands);
void echo(char *input_string, int status);
void execute_internal_commands(char *input_string, char *command);
void execute_external_commands(char *input_string, char *command);
void signal_handler(int sig_num);
void extract_external_commands(char **external_commands);
void insert_at_first(struct list **head , int pid , char *input_string);
void delete_at_first(struct list **head);


#endif