#ifndef main_h
#define main_h

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>

int exit_(int argc, char** argv);
int setPrompt(int argc, char** argv);
int setPath(int argc, char** argv);

char* builtin_funcs_str[] = {
    "exit",
    "setPrompt",
    "setPath"
};

int (*builtin_funcs[]) (int argc, char** argv) = {
    &exit_,
    &setPrompt,
    &setPath,
};

int builtin_funcs_size = sizeof(builtin_funcs_str) / sizeof(char*);

char* first_prompt = "> Ingrese un comando: ";
char* prompt = strdup(first_prompt);
char* next_command = NULL;
int next_argc = 0;
char** next_argv = NULL;

#endif /* main_h */
