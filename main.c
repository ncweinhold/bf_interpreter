/**
  * Brainfuck interpreter written by Nick Weinhold
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DATA_SIZE 30000
#define MAX_TAPE_SIZE 30000

typedef struct {
    unsigned short data[MAX_DATA_SIZE];
    unsigned int data_ptr;

    char program_tape[MAX_TAPE_SIZE];
    unsigned int pc;
} BFInterpreter;

void BFInterpreter_compile(BFInterpreter *interpreter, char *program);
void BFInterpreter_dump_tape(BFInterpreter *interpreter);
void BFInterpreter_eval(BFInterpreter *interpreter);

void BFInterpreter_compile(BFInterpreter *interpreter, char *program) {
    int len = strlen(program);
    /*
     * Temporary error checking - for now, if the entire program is too large,
     * exit. Eventually this will be changed so that only actual brainfuck
     * instructions count towards the count...
     */
    if (len > MAX_TAPE_SIZE) {
        printf("The program tape is too large to be processed!\n");
        exit(1);
    }

#ifdef DEBUG
    printf("Compiling the program: %s\n", program);
#endif

    int idx = 0;
    int pc = 0;
    char current_char = 0;
    while (idx != len) {

        current_char = program[idx];
#ifdef DEBUG
        printf("Current token is: %c\n", current_char);
#endif

        if ((current_char == '+') || (current_char == '-') || (current_char == '>') 
            || (current_char == '<') || (current_char == '.') || (current_char == ',')) {
            interpreter->program_tape[pc] = current_char;
            pc++;
        }
        idx++;
    }
}

void BFInterpreter_dump_tape(BFInterpreter *interpreter) {
    int idx = 0;
    while (interpreter->program_tape[idx] != 0) {
        printf("%c", interpreter->program_tape[idx]);
        idx++;
    }
    printf("\n");
}

void BFInterpreter_eval(BFInterpreter *interpreter) {
    while (interpreter->program_tape[interpreter->pc] != 0) {
        char current_instruction = interpreter->program_tape[interpreter->pc];
        if (current_instruction == '+') {
            interpreter->data[interpreter->data_ptr]++;
        }
        if (current_instruction == '-') {
            interpreter->data[interpreter->data_ptr]--;
        }
        if (current_instruction == '>') {
            interpreter->data_ptr++;
        }
        if (current_instruction == '<') {
            interpreter->data_ptr--;
        }
        if (current_instruction == '.') {
            putchar(interpreter->data[interpreter->data_ptr]);
        }
        if (current_instruction == ',') {
            interpreter->data[interpreter->data_ptr] = getchar();
        }
        interpreter->pc++;
    }
#ifdef DEBUG
    printf("Item at data[0] is: %d\n", interpreter->data[0]);
#endif
}

int main(int argc, char* argv[]) {
    BFInterpreter interpreter;
    BFInterpreter_compile(&interpreter, "++++,..>+++[]--<-");
    BFInterpreter_eval(&interpreter);

    printf("\n");
    return 0;
}
