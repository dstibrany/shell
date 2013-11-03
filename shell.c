#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h> 

#define MAXINPUT 512
#define MAXARGS  512

char input_buf[MAXINPUT];
char* args[MAXARGS];

void display_prompt() {
    write(2, "$ ", 2);
}

void read_input() {
    if (read(0, input_buf, 128) < 0) {
        perror("An error occured reading from stdin\n");
        exit(-1);
    }
}

void tokenize_input() {
    char *cur = input_buf;
    char *start = cur;
    int arg_count = 0;

    while (*cur != '\n') {
        if (*cur == ' ') {
            *cur = '\0';
            args[arg_count++] = start;
            start = ++cur; 
        } else {
            cur++;
        }
    }
    
    *cur = '\0';
    args[arg_count++] = start;

    args[arg_count] = (char*)0;
}

void fork_process() {
    int status, exit_status;
    switch(fork()) {
        case -1:
            perror("Error forking process:");
            exit(-1);
        case 0: /* child */
            execv(args[0], args);
            perror("exec failed");
            exit(-1);
        default: /* parent */
            if (wait(&status) == -1) {
                perror("Wait failed");
                exit(-1);
            }

            if (WIFEXITED(status)) {
                exit_status = WEXITSTATUS(status);
                printf("Exit status was %d\n", exit_status);
            } else {
                printf("%s\n", "SIGNAL");
            }
    }
}

int main() {
    while(1) {
        display_prompt();
        read_input();
        tokenize_input();
        fork_process();
    }
    exit(0);
}
