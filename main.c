/**
  * Brainfuck interpreter written by Nick Weinhold
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DATA_SIZE 30000
#define MAX_TAPE_SIZE 30000
#define MAX_STACK_SIZE 5000

typedef enum {SIMPLE, JUMP, END} instruction_type;

typedef struct {
    instruction_type type;
    union {
        struct {
            char opcode;
        } SimpleInstruction;
        struct {
            char opcode;
            unsigned int jump_location;
        } JumpInstruction;
    } Instruction;
} BrainfuckInstruction;

typedef struct {
    unsigned int size;
    unsigned int pc_values[MAX_STACK_SIZE];
} Stack;

void Stack_push(Stack *stack, unsigned int pc_value);
unsigned int Stack_pop(Stack *stack);
int Stack_empty(Stack *stack);

void Stack_push(Stack *stack, unsigned int pc_value) {
    stack->pc_values[stack->size] = pc_value;
    stack->size++;
}

unsigned int Stack_pop(Stack *stack) {
    stack->size--;
    return stack->pc_values[stack->size];
}

int Stack_empty(Stack *stack) {
    return (stack->size == 0);
}

typedef struct {
    unsigned short data[MAX_DATA_SIZE];
    unsigned int data_ptr;

    BrainfuckInstruction program_tape[MAX_TAPE_SIZE];
    unsigned int pc;
    Stack jump_locations;
} BFInterpreter;

void BFInterpreter_compile(BFInterpreter *interpreter, char *program);
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
            interpreter->program_tape[pc].type = SIMPLE;
            interpreter->program_tape[pc].Instruction.SimpleInstruction.opcode = current_char;
            pc++;
        }
        if (current_char == '[') {
            Stack_push(&interpreter->jump_locations, pc);
            interpreter->program_tape[pc].type = JUMP;
            interpreter->program_tape[pc].Instruction.JumpInstruction.opcode = current_char;
            pc++;
        }
        if (current_char == ']') {
            if (Stack_empty(&interpreter->jump_locations)) {
                printf("Unmatched loop instruction!\n");
                exit(1);
            }
            unsigned int jl = Stack_pop(&interpreter->jump_locations);
            interpreter->program_tape[pc].type = JUMP;
            interpreter->program_tape[pc].Instruction.JumpInstruction.opcode = current_char;
            interpreter->program_tape[pc].Instruction.JumpInstruction.jump_location = jl;
            interpreter->program_tape[jl].Instruction.JumpInstruction.jump_location = pc;
#ifdef DEBUG
            printf("Instruction at program_tape[%d] will jump to location: %d\n", pc, jl);
            printf("Instruction at program_tape[%d] will jump to location: %d\n", jl, pc);
#endif
            pc++;
        }
        idx++;
    }
    if (!Stack_empty(&interpreter->jump_locations)) {
        printf("Unmatched loop instruction!\n");
        exit(1);
    }
    interpreter->program_tape[pc].type = END;
}

void BFInterpreter_eval(BFInterpreter *interpreter) {
    while (interpreter->program_tape[interpreter->pc].type != END) {
        BrainfuckInstruction current_instruction = interpreter->program_tape[interpreter->pc];
        char current_opcode;
        if (current_instruction.type == SIMPLE) {
            current_opcode = current_instruction.Instruction.SimpleInstruction.opcode;
        }
        if (current_instruction.type == JUMP) {
            current_opcode = current_instruction.Instruction.JumpInstruction.opcode;
        }
        if (current_opcode == '+') {
            interpreter->data[interpreter->data_ptr]++;
        }
        if (current_opcode == '-') {
            interpreter->data[interpreter->data_ptr]--;
        }
        if (current_opcode == '>') {
            interpreter->data_ptr++;
        }
        if (current_opcode == '<') {
            interpreter->data_ptr--;
        }
        if (current_opcode == '.') {
            putchar(interpreter->data[interpreter->data_ptr]);
        }
        if (current_opcode == ',') {
            interpreter->data[interpreter->data_ptr] = getchar();
        }
        if (current_opcode == ']') {
            if (interpreter->data[interpreter->data_ptr]) {
                interpreter->pc = current_instruction.Instruction.JumpInstruction.jump_location;
#ifdef DEBUG
                printf("Jumping back to location: %d\n", interpreter->pc);
#endif
            }
        }
        if (current_opcode == '[') {
            if (!interpreter->data[interpreter->data_ptr]) {
                interpreter->pc = current_instruction.Instruction.JumpInstruction.jump_location;
#ifdef DEBUG
                printf("Jumping forward to location: %d\n", interpreter->pc);
#endif
            }
        }
        interpreter->pc++;
    }
#ifdef DEBUG
    printf("Item at data[0] is: %d\n", interpreter->data[0]);
#endif
}

int main(int argc, char* argv[]) {
    BFInterpreter interpreter;
    BFInterpreter_compile(&interpreter, "++++++++++[>+++++++>++++++++++>+++>+<<<<-]>++.>+.+++++++..+++.>++.<<+++++++++++++++.>.+++.------.--------.>+.>.");
    BFInterpreter_eval(&interpreter);

    return 0;
}
