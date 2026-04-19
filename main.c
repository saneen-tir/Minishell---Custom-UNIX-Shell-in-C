/*
 * Author      : Muhammed Saneen P
 * Date        : 17-01-2026
 * Description : This file serves as the driver module for the Mini Shell project.
 *               It initializes the shell environment, sets up signal handling,
 *               manages user input, and coordinates the execution of built-in
 *               and external commands, including job control and pipe handling.
 */

#include "minishell.h"

/* store the pid everytime , if a command runs , a child is created
and the pid>0 if no cmd is runnning , no child -> pid =0 */
int pid=0;

//to collect the exit code of last exited process
int status;

//to check if we have done ps1 command
int ps1_flag ;

//output for echo $$
int ech_pid;

char *builtins[] = {"echo", "printf", "read", "cd", "pwd", "pushd", "popd", "dirs", "let", "eval",
						"set", "unset", "export", "declare", "typeset", "readonly", "getopts", "source",
						"exit", "exec", "shopt", "caller", "true", "type", "hash", "bind", "help", NULL};


//to print the path along with the prompt
char path[100];

//the prompt string
char prompt[20] = "Minishell$:";

//sll to add the details of stopped process
struct list *head = NULL;

//to get command from the user
char input_string[50];

void my_handler(int signum)
{
    //for SIGINT , just print prompt if no cmd runs , else , let the process behaviour be default

    if(signum == SIGINT)
    {
        if(pid == 0)
        {
            
            //printing prompt
            if(ps1_flag==0)
            {
                printf(ANSI_COLOR_GREEN BOLD"\n%s"ANSI_COLOR_BLUE"%s "ANSI_COLOR_RESET,prompt,path);
            }
            else
            {
                printf("\n%s",prompt);
            }            
            fflush(stdout); 
        }
    }
    else if(signum == SIGTSTP)
    {
        if(pid == 0)
        {
            
            //printing prompt
            if(ps1_flag==0)
            {
                printf(ANSI_COLOR_GREEN BOLD"\n%s"ANSI_COLOR_BLUE"%s "ANSI_COLOR_RESET,prompt,path);
            }
            else
            {
                printf("\n%s",prompt);
            }
            fflush(stdout);
        }
        else if(pid > 0)
        {
            //add to list only if sigstp is there
            insert_at_first(&head,pid,input_string);
        }
    }
    else if(signum == SIGCHLD)  //SIGCHLD is sent to the process whenever there is a state change
    {
        waitpid(-1, &status, WNOHANG);
    }   
}

char* get_command(char* input_string)
{
    char* command = malloc(50);
    //returns the address of first command in the command string
    sscanf(input_string,"%s",command);
    return command;
}

int main()
{
    ech_pid = getpid();

    char command[50];

    //to store the external commands in this 2d array
    char *external_commands[154];
    extract_external_commands(external_commands);

    //clearing the shell
    system("clear");

    //signation action changed
    signal(SIGINT,my_handler);
    signal(SIGTSTP,my_handler);
    signal(SIGCHLD,my_handler);

    while(1)
    {
        //path is a global arary
        getcwd(path,100);

        //printing prompt
        if(ps1_flag==0)
        {
            printf(ANSI_COLOR_GREEN BOLD"%s"ANSI_COLOR_BLUE"%s "ANSI_COLOR_RESET,prompt,path);
        }
        else
        {
            printf("%s ",prompt);
        }
        
        //clear the command string , put all zeroes to it
        memset(input_string,0,sizeof(input_string));

        //command from user
        fgets(input_string,50,stdin);

        //to get the first cmd 
        strcpy(command,get_command(input_string));

        //fgets will take \n also , to handle it :
        int len = strlen(input_string);
        input_string[len-1] = '\0';
        
        // if input string is empty , display the prompt again
        if(strlen(input_string) == 0)
            continue;

        //to store the starting address of the PS1 command
        char* ptr = NULL;

        //1.modifying prompt using PS1 command
        if((ptr = strstr(input_string,"PS1=")) && input_string[0] == 'P' &&  input_string[1] == 'S' && input_string[2] == '1')
        {
            //checks if space present and anything exists after '='
            if(strchr(input_string,' ') || ptr[4] == '\0')
            {
                printf("ERROR: invalid input , space not expected\n");
            }
            else
            {
                //copies the value after '=' into the prompt string
                strcpy(prompt,ptr+4);
                ps1_flag = 1;
            }
            continue;
        }

        int type = check_command_type(command,external_commands);

        //check the command type and perform function
        if(type == BUILTIN)
        {
            printf("BUILTIN\n");
            execute_internal_commands(input_string,command);
        }
        else if(type == EXTERNAL)
        {
            printf("EXTERNAL\n");
            execute_external_commands(input_string,command);
        }
        else if(type == NO_COMMAND)
        {
            struct list *temp = head;
            //printing the linked list
            if(strcmp(command,"jobs") == 0)
            {
                int i=1;
                while(temp)
                {
                    printf("[%d]   Stopped     %d            %s\n",i++,temp->pid,temp->input_string);
                    temp = temp -> link;
                }
                
            }
            else if(strcmp(command,"fg") == 0)
            {
                 if(head != NULL)
                {
                    //continuing the last stopped process(continue the first node in sll)
                    kill(head->pid,SIGCONT);
                    pid = head->pid;
                    printf("%s\n",head->input_string); //bash behaviour
                    waitpid(head->pid,&status,WUNTRACED);//(WUNTRACED , bcz this process can be stopped again)
                    pid=0;
                    delete_at_first(&head);
                }
                else
                {
                    printf("fg: current: no such job\n");
                }                
            }
            else if(strcmp(command,"bg") == 0)
            {
                if(head != NULL)
                {
                    kill(head->pid,SIGCONT);
                    delete_at_first(&head);
                }
                else
                {
                    printf("bg: current: no such job\n");
                }
                
            }
            else
            {
                printf("Command '%s' not found\n",command);
            }
            
        }
    }

}

int check_command_type(char *command,char **external_commands)
{
    //check in the inbuilt command array
    int i=0;
    while(builtins[i])
    {
        if(strcmp(command,builtins[i])==0)
        {
            return BUILTIN;
        }
        i++;
    }


    //check in the external command array
    i=0;
    while(external_commands[i])
    {
        if(strcmp(command,external_commands[i])==0)
        {
            
            return EXTERNAL;
        }
        i++;
    }

    //in case not both external and builtin
    return NO_COMMAND;
}


void execute_internal_commands(char *input_string,char *command)
{
    if(strcmp(command,"exit")==0)
    {
        exit(0);
    }
    else if(strcmp(command,"pwd")==0)
    {
        char path[100];
        getcwd(path,100);
        printf("%s\n",path);
    }
    else if(strcmp(command,"cd")==0)
    {
        //if just cd is give , go to root(/)
        if (input_string[2] == '\0')
        {
            chdir("/"); 
        }
        else
        {
            chdir(input_string + 3);
        }
    }
    else if(strcmp(command,"echo")==0)
    {
        //if just cd is give , go to root(/)
        if(strcmp(input_string+5,"$$")==0)
        {
            printf("%d\n",ech_pid);
        }
        else if(strcmp(input_string+5,"$?")==0)
        {
            if(WIFEXITED(status))
            {
                printf("%d\n",WEXITSTATUS(status));
            }
        }
        else if(strcmp(input_string+5,"$SHELL")==0)
        {
            char *s = getenv("SHELL");
            printf("%s\n",s);
        }
        else
        {
            printf("%s\n",input_string+5);
        }
        
    }
}

void execute_external_commands(char *input_string, char *command)
{
    pid = fork();

    if(pid > 0)
    {
        waitpid(pid,&status,WUNTRACED);
        pid = 0;
    }
    else if(pid == 0)
    {
        signal(SIGINT,SIG_DFL);
        signal(SIGTSTP,SIG_DFL);
        signal(SIGCHLD,SIG_DFL);



        //to store the number of pipes
        int pipe_count=0;

        //make a 2d array to implement execvp
        char *exec_arr[20];
        int j=0;

        char temp[64];
        int k=0;

        char ch;

        //creating 2d arrays with each cmds
        int i=0;
        while(input_string[i])
        {
            if(input_string[i] ==' ')
            {
                temp[k] = '\0';

                if(strcmp(temp,"|")==0)
                {
                    pipe_count++;
                }

                exec_arr[j++] = strdup(temp);
                k=0;

            }
            else
            {
                temp[k++] = input_string[i];
            }

            i++;
        }

        //handle the last word
        temp[k] = '\0';
        exec_arr[j++] = strdup(temp);
        exec_arr[j] = NULL;

        if(pipe_count == 0)
        {
            //now , the exec_arr can be used insted of char* argv in execvp
            execvp(exec_arr[0],exec_arr);
        }
        else
        {
            //to store the cmd indexes
            int cmd_index[10];

            int cmd_count = 0;

            cmd_index[0] = 0;

            // index of cmd_index[]
            int j=1;

            //putting NULL and taking the indexes of commands
            i=0;
            while(exec_arr[i])
            {
                if(strcmp(exec_arr[i],"|") == 0)
                {
                    exec_arr[i] = NULL;
                    cmd_index[j++] = i+1;
                    cmd_count++;
                }
                i++;
            }
            cmd_count++;

            int pipefd[2];

            int newstdin = dup(0);
            int newstdout = dup(1);

            for(int i=0;i<cmd_count;i++)
            {
                if(i < cmd_count - 1 )
                {
                    pipe(pipefd);
                }
                
                int pid2 = fork();

                if(pid2 > 0)
                {
                    if(i<cmd_count-1)
                    {
                        dup2(pipefd[0],0);
                        close(pipefd[1]);
                    }
                    
                }
                else if(pid == 0)
                {
                    if(i<cmd_count-1)
                    {
                        dup2(pipefd[1],1);
                        close(pipefd[0]);
                    }
                    execvp(exec_arr[cmd_index[i]] , exec_arr + cmd_index[i]);
                    printf("Error in executing command\n");
                }
            }

            while(wait(&status)>0);
            dup2(newstdin,0);
            dup2(newstdout,1);
        }        
       exit(0);       
    }

}