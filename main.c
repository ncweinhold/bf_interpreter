/**
  * Brainfuck interpreter written by Nick Weinhold
  */

#include <stdio.h>
#include <string.h>

typedef struct {
    unsigned short data[30000];
    unsigned int data_ptr;

    char program_tape[30000];
    unsigned int pc;
} BFInterpreter;

void BFInterpreter_compile(BFInterpreter *interpreter, char *program);
void BFInterpreter_dump_tape(BFInterpreter *interpreter);
void BFInterpreter_eval(BFInterpreter *interpreter);

void BFInterpreter_compile(BFInterpreter *interpreter, char *program) {

#ifdef DEBUG
    printf("Compiling the program: %s\n", program);
#endif

    int len = strlen(program);
    int idx = 0;
    int pc = 0;
    while (idx != len) {

#ifdef DEBUG
        printf("Current token is: %c\n", program[idx]);
#endif
        if (program[idx] == '+') {
            interpreter->program_tape[pc] = program[idx];
            pc++;
        }
        if (program[idx] == '-') {
            interpreter->program_tape[pc] = program[idx];
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
        interpreter->pc++;
    }
#ifdef DEBUG
    printf("Item at data[0] is: %d\n", interpreter->data[0]);
#endif
}

int main(int argc, char* argv[]) {
    BFInterpreter interpreter;
    BFInterpreter_compile(&interpreter, "+++++[]---");

    BFInterpreter_eval(&interpreter);

    BFInterpreter_dump_tape(&interpreter);

    return 0;
}
